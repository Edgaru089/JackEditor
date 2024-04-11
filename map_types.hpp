
#include "gui/imgui/imgui.h"
#include "types.hpp"
#include "ig001.hpp"
#include "gui/imgui/imgui_stdlib.h"
#include <string>


class EntityBase {
public:
	virtual std::string type() = 0;

	virtual std::string to_file() = 0;

	// Pointer to the entity base is pushed outside
	virtual void imgui() {}
	virtual void draw(Vec2 offset, bool selected) {}
};

inline static void draw_box2(Box2 box, bool selected, ImU32 color) {
	ImDrawList *list = ig::GetBackgroundDrawList();
	list->AddRect(box.lefttop.im(), (box.lefttop + box.size).im(), color);
	if (selected) {
		list->AddLine(box.lefttop.im(), (box.lefttop + box.size).im(), color);
		list->AddLine((box.lefttop + Vec2(box.size.x, 0)).im(), (box.lefttop + Vec2(0, box.size.y)).im(), color);
	}
}

inline static void draw_vec2(Vec2 vec, bool selected, ImU32 color) {
	ImDrawList *list = ig::GetBackgroundDrawList();
	list->AddLine((vec + Vec2(-10, -10)).im(), (vec + Vec2(10, 10)).im(), color);
	list->AddLine((vec + Vec2(-10, 10)).im(), (vec + Vec2(10, -10)).im(), color);
	if (selected)
		list->AddCircle(vec.im(), 7.0f, color);
}


// Defined in bundle.cpp
extern char buf[1024];

class Hitbox: public EntityBase {
public:
	std::string type() override { return "hitbox"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "HITBOX %lf %lf %lf %lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		return buf;
	}

	void imgui() override { DragBox2("", &box); }
	void draw(Vec2 offset, bool selected) override { draw_box2(box.offset(offset), selected, IM_COL32(0, 255, 0, 255)); }

	Box2 box;
};

class Player: public EntityBase {
public:
	std::string type() override { return "player"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "PLAYER %lf %lf\n", pos.x, pos.y);
		return buf;
	}

	void imgui() override { DragVec2("Player Spawn", &pos); }
	void draw(Vec2 offset, bool selected) override { draw_vec2(pos, selected, IM_COL32_WHITE); }

	Vec2 pos;
};

class HazardRespawn: public EntityBase {
public:
	std::string type() override { return "hazard_respawn"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "HAZARD_RESPAWN %lf %lf %lf %lf %lf %lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y, pos.x, pos.y);
		return buf;
	}

	void imgui() override {
		DragBox2("", &box);
		DragVec2("Respawn", &pos);
	}
	void draw(Vec2 offset, bool selected) override {
		draw_box2(box, selected, IM_COL32(255, 0, 255, 255));
		draw_vec2(pos, selected, IM_COL32(255, 0, 255, 255));
	}

	Box2 box;
	Vec2 pos;
};

class Hazard: public EntityBase {
public:
	std::string type() override { return "hazard"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "HAZARD %lf %lf %lf %lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		return buf;
	}

	void imgui() override { DragBox2("", &box); }
	void draw(Vec2 offset, bool selected) override { draw_box2(box, selected, IM_COL32(255, 0, 0, 255)); }

	Box2 box;
};

class Textbox: public EntityBase {
public:
	std::string type() override { return "textbox"; }
	std::string to_file() override {
		std::string str;
		snprintf(buf, sizeof(buf), "TEXTBOX %lf %lf %lf %lf ", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		str += buf;

		// Escape all the whitespaces
		for (char c: text) {
			switch (c) {
				case '\t':
					str += "\\t";
					break;
				case ' ':
					str += "\\s";
					break;
				case '\n':
					str += "\\n";
					break;
				default:
					str.push_back(c);
			}
		}

		str += "\n";
		return str;
	}

	void imgui() override {
		DragBox2("", &box);
		ig::InputTextMultiline("##Text", &text, ImVec2(-1, 0));
	}
	void draw(Vec2 offset, bool selected) override {
		draw_box2(box, selected, IM_COL32(0, 0, 255, 255));
		// Draw text
		ImDrawList *list = ig::GetBackgroundDrawList();
		list->AddText(box.lefttop.im(), IM_COL32_WHITE, text.c_str());
	}

	std::string text;
	Box2        box;
};
