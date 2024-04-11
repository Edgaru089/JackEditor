
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
static double                  cutoff = 1500;


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
					CMD("CUTOFF")
					cutoff = strtod(strtok(NULL, " "), NULL);

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
		if (ig::Button("Hitbox"))
			entities.push_back((new Hitbox())->setpos(center));
		ig::SameLine();
		if (ig::Button("Player"))
			entities.push_back((new Player())->setpos(center));
		ig::SameLine();
		if (ig::Button("Hazard Respawn"))
			entities.push_back((new HazardRespawn())->setpos(center));
		if (ig::Button("Hazard"))
			entities.push_back((new Hazard())->setpos(center));
		ig::SameLine();
		if (ig::Button("Textbox"))
			entities.push_back((new Textbox())->setpos(center));
		ig::SameLine();
		if (ig::Button("Level Transition"))
			entities.push_back((new LevelTransition())->setpos(center));

		DragDouble("Cutoff depth", &cutoff);

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

	if (ig::Begin("Properities") && selected_entity != NULL) {
		ig::PushID(selected_entity);
		selected_entity->imgui();
		ig::PopID();
	}
	ig::End();

	ImVec2      screen_size = ig::GetIO().DisplaySize;
	ImDrawList *drawlist    = ig::GetBackgroundDrawList();
	drawlist->AddLine(ImVec2(0, offset.y), ImVec2(screen_size.x, offset.y), IM_COL32_WHITE);
	drawlist->AddLine(ImVec2(offset.x, 0), ImVec2(offset.x, screen_size.y), IM_COL32_WHITE);
	drawlist->AddLine(ImVec2(0, cutoff + offset.y), ImVec2(screen_size.x, cutoff + offset.y), IM_COL32(255, 0, 0, 255));
	// Draw every entity
	for (EntityBase *e: entities)
		e->draw(offset, e == selected_entity);


	static std::string buff;
	buff.clear();
	buff = "CUTOFF " + std::to_string((int)std::round(cutoff)) + "\n";
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
