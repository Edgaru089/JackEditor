
#include <cmath>
#include "gui/gui.hpp"
#include "gui/imgui/imgui.h"
#include "gui/imgui/imgui_stdlib.h"
#include "types.hpp"

std::map<std::string, render_Bundle> bundles;
render_Bundle                        temp_b;
render_Primitive                     temp_p;

std::map<render_PrimitiveType, std::string> primitive_name = {
	{render_Lines, "render_Lines"},
	{render_LineStrip, "render_LineStrip"},
	{render_Polygon, "render_Polygon"},
};
std::map<render_PrimitiveType, std::string> primitive_name_upper = {
	{render_Lines, "LINES"},
	{render_LineStrip, "LINESTRIP"},
	{render_Polygon, "POLY"},
};

std::string toString(render_Bundle &b) {
#define APPEND_FMT(...)                          \
	do {                                         \
		snprintf(buf, sizeof(buf), __VA_ARGS__); \
		str.append(buf);                         \
	} while (false)

#define EXPAND_RGBA(packed) ((packed >> IM_COL32_R_SHIFT) & 0xff), ((packed >> IM_COL32_G_SHIFT) & 0xff), ((packed >> IM_COL32_B_SHIFT) & 0xff)

	std::string str;
	static char buf[1024];

	APPEND_FMT("BUNDLE %s\n", b.name.c_str());
	for (auto &p: b.prims) {
		APPEND_FMT("PRIM %s\n", primitive_name_upper[p.type].c_str());
		APPEND_FMT("FG %d %d %d\n", EXPAND_RGBA(p.fg));
		APPEND_FMT("BG %d %d %d\n", EXPAND_RGBA(p.bg));
		for (auto &pnt: p.points)
			APPEND_FMT("P %f %f\n", pnt.x, pnt.y);
		APPEND_FMT("ENDPRIM\n");
	}

	APPEND_FMT("ENDBUNDLE\n");
#undef APPEND_FMT
	return str;
}

void ui_bundle() {
	if (ig::Begin("Draw & Render")) {
		static ImVec2 scrolling(260.0f, 310.0f);
		static float  fg[] = {1.0f, 1.0f, 1.0f}, bg[] = {0.0f, 0.0f, 0.0f};
		static bool   adding = false;

		if (ig::InputText("Bundle name", &temp_b.name, ImGuiInputTextFlags_EnterReturnsTrue)) {
			bundles.insert_or_assign(temp_b.name, temp_b);
			temp_b.prims.clear();
		}

		ig::ColorEdit3("Foreground", fg, ImGuiColorEditFlags_PickerHueWheel);
		temp_p.fg = ImGui::ColorConvertFloat4ToU32(ImVec4(fg[0], fg[1], fg[2], 1.0f));
		ig::ColorEdit3("Background", bg, ImGuiColorEditFlags_PickerHueWheel);
		temp_p.bg = ImGui::ColorConvertFloat4ToU32(ImVec4(bg[0], bg[1], bg[2], 1.0f));

		ig::RadioButton("Lines", (int *)&temp_p.type, render_Lines);
		ig::SameLine();
		ig::RadioButton("LineStrip", (int *)&temp_p.type, render_LineStrip);
		ig::SameLine();
		ig::RadioButton("Polygon", (int *)&temp_p.type, render_Polygon);

		ig::SameLine();
		ig::Text("adding=%s", adding ? "true" : "false");
		if (temp_b.prims.size() > 0) {
			ig::SameLine();
			if (ig::Button("Pop back"))
				temp_b.prims.pop_back();
		}

		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();    // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
		if (canvas_sz.x < 50.0f)
			canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f)
			canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Draw border and background color
		ImGuiIO    &io        = ImGui::GetIO();
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		// This will catch our interactions
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool   is_hovered = ImGui::IsItemHovered();                          // Hovered
		const bool   is_active  = ImGui::IsItemActive();                           // Held
		const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f)) {
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
		}

		draw_list->PushClipRect(canvas_p0, canvas_p1, true);

		draw_list->AddLine(ImVec2(canvas_p0.x, origin.y), ImVec2(canvas_p1.x, origin.y), IM_COL32(255, 255, 255, 255));
		draw_list->AddLine(ImVec2(origin.x, canvas_p0.y), ImVec2(origin.x, canvas_p1.y), IM_COL32(255, 255, 255, 255));

		const float GRID_STEP = 50.0f;
		for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
			draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
		for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
			draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));


		static std::vector<ImVec2> buff;

		auto convert_point = [&](ImVec2 point) -> ImVec2 { return ImVec2(point.x + origin.x, point.y + origin.y); };
		auto draw_prim     = [&](render_Primitive *p) {
            if (p->type == render_Lines) {
                for (int i = 0; i < (ssize_t)p->points.size() - 1; i += 2)
                    draw_list->AddLine(convert_point(p->points[i]), convert_point(p->points[i + 1]), p->fg);
            } else if (p->type == render_LineStrip) {
                if (p->points.size() < 2)
                    return;
                for (int i = 0; i < (ssize_t)p->points.size() - 1; i++)
                    draw_list->AddLine(convert_point(p->points[i]), convert_point(p->points[i + 1]), p->fg);
            } else if (p->type == render_Polygon) {
                if (p->points.size() < 3)
                    return;
                buff.resize(p->points.size());
                for (int i = 0; i < p->points.size(); i++)
                    buff[i] = convert_point(p->points[i]);
                draw_list->AddConvexPolyFilled(buff.data(), buff.size(), p->fg);
            }
		};


		// Update last point position
		if (is_hovered && adding) {
			temp_p.points.back() = mouse_pos_in_canvas;
			if (ig::IsMouseClicked(ImGuiMouseButton_Left)) {
				printf("added point %f,%f\n", mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);
				temp_p.points.push_back(mouse_pos_in_canvas);
			}
			if (ig::IsMouseClicked(ImGuiMouseButton_Right)) {
				// Commit the primitive
				temp_p.points.pop_back();
				printf("commited %lu points\n", temp_p.points.size());
				temp_b.prims.push_back(temp_p);
				temp_p.points.clear();
				adding = false;
			}
		}
		// Begin adding primitive
		if (is_hovered && !adding && ig::IsMouseClicked(ImGuiMouseButton_Left)) {
			adding = true;
			temp_p.points.push_back(mouse_pos_in_canvas);
			temp_p.points.push_back(mouse_pos_in_canvas);
			printf("added point %f,%f\n", mouse_pos_in_canvas.x, mouse_pos_in_canvas.y);
		}

		// Draw bundle
		for (auto &i: temp_b.prims)
			draw_prim(&i);
		draw_prim(&temp_p);


		draw_list->PopClipRect();
	}
	ig::End();


	static std::string current_bundle = "";
	if (ig::Begin("Bundle List") && bundles.size() > 0) {
		static int                          current = 0;
		static std::vector<render_Bundle *> temp_bundles;

		temp_bundles.clear();
		for (auto &i: bundles)
			temp_bundles.push_back(&i.second);

		ig::ListBox(
			"Bundles", &current,
			[](void *user, int id) -> const char * {
				return temp_bundles[id]->name.c_str();
			},
			NULL, temp_bundles.size());
		current_bundle = temp_bundles[current]->name;

		// Drawing only
		static ImVec2 scrolling(260.0f, 310.0f);

		// Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();    // ImDrawList API uses screen coordinates!
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail(); // Resize canvas to what's available
		if (canvas_sz.x < 50.0f)
			canvas_sz.x = 50.0f;
		if (canvas_sz.y < 50.0f)
			canvas_sz.y = 50.0f;
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

		// Draw border and background color
		ImGuiIO    &io        = ImGui::GetIO();
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
		draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(255, 255, 255, 255));

		// This will catch our interactions
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		const bool   is_hovered = ImGui::IsItemHovered();                          // Hovered
		const bool   is_active  = ImGui::IsItemActive();                           // Held
		const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, 0.0f)) {
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
		}

		draw_list->PushClipRect(canvas_p0, canvas_p1, true);

		draw_list->AddLine(ImVec2(canvas_p0.x, origin.y), ImVec2(canvas_p1.x, origin.y), IM_COL32(255, 255, 255, 255));
		draw_list->AddLine(ImVec2(origin.x, canvas_p0.y), ImVec2(origin.x, canvas_p1.y), IM_COL32(255, 255, 255, 255));

		const float GRID_STEP = 50.0f;
		for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
			draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
		for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
			draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));


		static std::vector<ImVec2> buff;

		auto convert_point = [&](ImVec2 point) -> ImVec2 { return ImVec2(point.x + origin.x, point.y + origin.y); };
		auto draw_prim     = [&](render_Primitive *p) {
            if (p->type == render_Lines) {
                for (int i = 0; i < (ssize_t)p->points.size() - 1; i += 2)
                    draw_list->AddLine(convert_point(p->points[i]), convert_point(p->points[i + 1]), p->fg);
            } else if (p->type == render_LineStrip) {
                if (p->points.size() < 2)
                    return;
                for (int i = 0; i < (ssize_t)p->points.size() - 1; i++)
                    draw_list->AddLine(convert_point(p->points[i]), convert_point(p->points[i + 1]), p->fg);
            } else if (p->type == render_Polygon) {
                if (p->points.size() < 3)
                    return;
                buff.resize(p->points.size());
                for (int i = 0; i < p->points.size(); i++)
                    buff[i] = convert_point(p->points[i]);
                draw_list->AddConvexPolyFilled(buff.data(), buff.size(), p->fg);
            }
		};
		// Draw bundle
		for (auto &i: temp_bundles[current]->prims)
			draw_prim(&i);
		draw_list->PopClipRect();
	}
	ig::End();


	if (ig::Begin("Properities") && !current_bundle.empty()) {
		render_Bundle &b = bundles[current_bundle];
		ig::Text("%s, %d primitives", b.name.c_str(), (int)b.prims.size());
		for (int i = 0; i < b.prims.size(); i++) {
			ig::PushID(i);
			auto &p = b.prims[i];
			char  buf[512];
			snprintf(buf, sizeof(buf), "Type %s", primitive_name[p.type].c_str(), i);
			if (ig::CollapsingHeader(buf)) {
				for (int i = 0; i < p.points.size(); i++) {
					ig::PushID(i);
					float pnt[2] = {p.points[i].x, p.points[i].y};
					snprintf(buf, sizeof(buf), "Point %d", i + 1);
					ig::SliderFloat2(buf, pnt, -200, 200);
					p.points[i].x = pnt[0];
					p.points[i].y = pnt[1];
					ig::SameLine();
					ig::Button("Drag");
					if (ig::IsItemActive()) {
						ImVec2 delta = ig::GetIO().MouseDelta;
						p.points[i].x += delta.x;
						p.points[i].y += delta.y;
					}
					ig::Separator();
					ig::PopID();
				}
			}
			ig::PopID();
		}
	}
	ig::End();


	if (ig::Begin("Text") && !current_bundle.empty()) {
		render_Bundle &b   = bundles[current_bundle];
		std::string    str = toString(b);
		ig::InputTextMultiline("###Output", &str, ImVec2(-1, -1), ImGuiInputTextFlags_ReadOnly);
	}
	ig::End();
}
