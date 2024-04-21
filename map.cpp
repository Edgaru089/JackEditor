
#include "gui/gui.hpp"
#include "gui/imgui/imgui.h"
#include "gui/imgui/imgui_stdlib.h"
#include "ig001.hpp"
#include "map_types.hpp"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <list>
#include <sstream>


static std::list<EntityBase *> entities;
static EntityBase             *selected_entity = NULL;
static EntityBase             *to_delete       = NULL;
static Vec2                    offset0(0, 0);
static double                  cutoff     = 1500;
static float                   bgcolor[3] = {0.0f, 0.0f, 0.0f};


void ui_map() {
	Vec2 newdelta(ig::GetMouseDragDelta(ImGuiMouseButton_Right));
	Vec2 offset = offset0 + newdelta;
	if (!ig::IsMouseDragging(ImGuiMouseButton_Right))
		offset0 = offset0 + newdelta;

	if (ig::Begin("Entities")) {
		if (ig::BeginMenu("Load Entities from Text")) {
			static std::string buf_code;
			ig::InputTextMultiline("##Code", &buf_code, ImVec2(400, 300));
			if (ig::Button("Load!")) {
				for (EntityBase *e: entities)
					delete e;
				entities.clear();
				selected_entity = NULL;

				std::istringstream is(buf_code);
				std::string        line;
				while (std::getline(is, line)) {
					if (line.size() == 0)
						continue;
					char *cmd = std::strtok(line.data(), " ");
#define CMD1(str) if (std::strcmp(cmd, str) == 0)
#define CMD(str)  else if (std::strcmp(cmd, str) == 0)
					if (!cmd)
						continue;
					EntityBase *e = NULL;
					CMD1("HITBOX")
					e = new Hitbox();
					CMD("PLAYER")
					e = new Player();
					CMD("HAZARD_RESPAWN")
					e = new HazardRespawn();
					CMD("HAZARD")
					e = new Hazard();
					CMD("TEXTBOX")
					e = new Textbox();
					CMD("LEVEL_TRANSITION")
					e = new LevelTransition();
					CMD("CAMERA_FOCUS")
					e = new CameraFocus();
					CMD("FILL")
					e = new FillBox;
					CMD("FILLPOLY")
					e = new FillPoly;
					CMD("CUTOFF")
					cutoff = strtod(strtok(NULL, " "), NULL);
					CMD("BACKGROUND") {
						bgcolor[0] = strtof(TOKEN, NULL) / 255.0f;
						bgcolor[1] = strtof(TOKEN, NULL) / 255.0f;
						bgcolor[2] = strtof(TOKEN, NULL) / 255.0f;
					}

					if (e != NULL) {
						e->from_strtok();
						entities.push_back(e);
					}
				}

				buf_code.clear();
			}
			ig::EndMenu();
		}

		Vec2 center = Vec2(ig::GetIO().DisplaySize) * 0.5 - offset;
		ig::SeparatorText("Add Entity");
		if (ig::Button("Hitbox")) {
			entities.push_back((new Hitbox())->setpos(center));
			selected_entity = entities.back();
		}
		ig::SameLine();
		if (ig::Button("Player")) {
			entities.push_back((new Player())->setpos(center));
			selected_entity = entities.back();
		}
		ig::SameLine();
		if (ig::Button("Hazard Respawn")) {
			entities.push_back((new HazardRespawn())->setpos(center));
			selected_entity = entities.back();
		}
		if (ig::Button("Hazard")) {
			entities.push_back((new Hazard())->setpos(center));
			selected_entity = entities.back();
		}
		ig::SameLine();
		if (ig::Button("Textbox")) {
			entities.push_back((new Textbox())->setpos(center));
			selected_entity = entities.back();
		}
		ig::SameLine();
		if (ig::Button("Level Transition")) {
			entities.push_back((new LevelTransition())->setpos(center));
			selected_entity = entities.back();
		}
		if (ig::Button("Camera Focus")) {
			entities.push_back((new CameraFocus())->setpos(center));
			selected_entity = entities.back();
		}
		ig::SameLine();
		if (ig::Button("Fill Box")) {
			entities.push_back((new FillBox())->setpos(center));
			selected_entity = entities.back();
		}
		ig::SameLine();
		if (ig::Button("Fill Poly")) {
			entities.push_back((new FillPoly())->setpos(center));
			selected_entity = entities.back();
		}

		DragDouble("Cutoff depth", &cutoff);
		ImGui::ColorEdit3("Background", bgcolor, ImGuiColorEditFlags_PickerHueWheel);

		ig::SeparatorText("Entities");
		if (ig::Selectable("<<< CLEAR >>>", selected_entity == NULL))
			selected_entity = NULL;
		ig::Separator();

		for (EntityBase *e: entities) {
			ig::PushID(e);
			if (ig::Selectable(e->type().c_str(), selected_entity == e))
				selected_entity = e;
			if (ig::IsItemHovered())
				if (ig::IsMouseDoubleClicked(ImGuiMouseButton_Right))
					to_delete = e;
			ig::PopID();
		}
	}
	ig::End();

	ImVec2      screen_size = ig::GetIO().DisplaySize;
	ImDrawList *drawlist    = ig::GetBackgroundDrawList();
	drawlist->AddRectFilled(ImVec2(0, 0), screen_size, ImGui::ColorConvertFloat4ToU32(ImVec4(bgcolor[0], bgcolor[1], bgcolor[2], 1.0f)));

	if (ig::Begin("Properities") && selected_entity != NULL) {
		ig::PushID(selected_entity);
		selected_entity->imgui();
		ig::PopID();
	}
	ig::End();

	drawlist->AddLine(ImVec2(0, offset.y), ImVec2(screen_size.x, offset.y), IM_COL32_WHITE);
	drawlist->AddLine(ImVec2(offset.x, 0), ImVec2(offset.x, screen_size.y), IM_COL32_WHITE);
	drawlist->AddLine(ImVec2(0, cutoff + offset.y), ImVec2(screen_size.x, cutoff + offset.y), IM_COL32(255, 0, 0, 255));
	// Draw every entity
	for (EntityBase *e: entities)
		e->draw_prev(offset, e == selected_entity);
	for (EntityBase *e: entities)
		e->draw(offset, e == selected_entity);


	static std::string buff;
	buff.clear();
	buff = "CUTOFF " + std::to_string((int)std::round(cutoff)) + "\n";
	buff += "BACKGROUND " + std::to_string((int)std::round(bgcolor[0] * 255.0f)) + " " + std::to_string((int)std::round(bgcolor[1] * 255.0f)) + " " + std::to_string((int)std::round(bgcolor[2] * 255.0f)) + "\n";
	for (EntityBase *e: entities) {
		buff += e->to_file();
	}
	if (ig::Begin("Output")) {
		if (ig::Button("Copy!"))
			ig::SetClipboardText(buff.c_str());
		ig::InputTextMultiline("##Text", &buff, ImVec2(-1, -1), ImGuiInputTextFlags_ReadOnly);
	}
	ig::End();


	if (to_delete != NULL) {
		entities.remove(to_delete);
		to_delete = NULL;
	}
}
