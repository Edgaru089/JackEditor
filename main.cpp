#include <SFML/System.hpp>
#include <cmath>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <cstdio>
#include <vector>

#include "gui/gui.hpp"
#include "gui/imgui/imgui.h"
#include "gui/imgui/imgui_stdlib.h"

#include "types.hpp"

using namespace std;

extern std::map<std::string, render_Bundle> bundles;

render_Bundle    temp_b;
render_Primitive temp_p;


int main(int argc, char *argv[]) {
	UI ui(sf::Vector2u(1200, 1000), "JackEditor");

	while (ui.Update()) {
		static bool demo_open = true;
		ig::ShowDemoWindow(&demo_open);


		if (ig::Begin("Draw & Render")) {
			static ImVec2 scrolling(260.0f, 310.0f);
			static float  fg[] = {1.0f, 1.0f, 1.0f}, bg[] = {0.0f, 0.0f, 0.0f};

			ig::InputText("Bundle name", &temp_b.name);

			ig::ColorEdit3("Foreground", fg, ImGuiColorEditFlags_PickerHueWheel);
			ig::ColorEdit3("Background", bg, ImGuiColorEditFlags_PickerHueWheel);

			ig::RadioButton("Lines", (int *)&temp_p.type, render_Lines);
			ig::SameLine();
			ig::RadioButton("LineStrip", (int *)&temp_p.type, render_LineStrip);
			ig::SameLine();
			ig::RadioButton("Polygon", (int *)&temp_p.type, render_Polygon);

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
                    for (int i = 0; i < p->points.size(); i += 2)
                        draw_list->AddLine(convert_point(p->points[i]), convert_point(p->points[i + 1]), p->fg);
                } else if (p->type == render_LineStrip) {
                    if (p->points.size() < 2)
                        return;
                    buff.resize(p->points.size());
                    for (int i = 0; i < p->points.size(); i++)
                        buff[i] = convert_point(p->points[i]);
                    draw_list->AddPolyline(buff.data(), buff.size(), p->fg, ImDrawFlags_Closed, 1.0f);
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


			draw_list->PopClipRect();
		}
		ig::End();


		ui.Render();
	}


	return 0;
}
