
#include "gui/imgui/imgui.h"
#include "types.hpp"
#include "ig001.hpp"
#include "gui/imgui/imgui_stdlib.h"
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>


class EntityBase {
public:
	virtual ~EntityBase() {}
	virtual std::string type() = 0;

	virtual std::string to_file()     = 0;
	virtual void        from_strtok() = 0;

	// Pointer to the entity base is pushed outside
	virtual void        imgui() {}
	virtual void        draw_prev(Vec2 offset, bool selected) {}
	virtual void        draw(Vec2 offset, bool selected) {}
	virtual EntityBase *setpos(Vec2 pos) { return this; }
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

#define TOKEN        (strtok(NULL, " "))
#define TOKEN_INT    (atoi(TOKEN))
#define TOKEN_DOUBLE (strtod(TOKEN, NULL))


// Defined in bundle.cpp
extern char buf[1024];

class Hitbox: public EntityBase {
public:
	std::string type() override { return "hitbox"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "HITBOX %.0lf %.0lf %.0lf %.0lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		return buf;
	}

	void from_strtok() override {
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);
	}

	void        imgui() override { DragBox2("", &box); }
	void        draw(Vec2 offset, bool selected) override { draw_box2(box.offset(offset), selected, IM_COL32(0, 255, 0, 255)); }
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		return this;
	}

	Box2 box;
};

class Player: public EntityBase {
public:
	std::string type() override { return "player"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "PLAYER %.0lf %.0lf\n", pos.x, pos.y);
		return buf;
	}
	void from_strtok() override {
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		pos = Vec2(a, b);
	}

	void        imgui() override { DragVec2("Player Spawn", &pos); }
	void        draw(Vec2 offset, bool selected) override { draw_vec2(pos + offset, selected, IM_COL32_WHITE); }
	EntityBase *setpos(Vec2 pos) override {
		this->pos = pos;
		return this;
	}

	Vec2 pos;
};

class HazardRespawn: public EntityBase {
public:
	std::string type() override { return "hazard_respawn"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "HAZARD_RESPAWN %.0lf %.0lf %.0lf %.0lf %.0lf %.0lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y, pos.x, pos.y);
		return buf;
	}
	void from_strtok() override {
		double a, b, c, d, e, f;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		e   = TOKEN_DOUBLE;
		f   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);
		pos = Vec2(e, f);
	}

	void imgui() override {
		DragBox2("", &box);
		DragVec2("Respawn", &pos);
	}
	void draw(Vec2 offset, bool selected) override {
		draw_box2(box.offset(offset), selected, IM_COL32(255, 0, 255, 255));
		draw_vec2(pos + offset, selected, IM_COL32(255, 0, 255, 255));
	}
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		this->pos   = pos;
		return this;
	}

	Box2 box;
	Vec2 pos;
};

class Hazard: public EntityBase {
public:
	std::string type() override { return "hazard"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "HAZARD %.0lf %.0lf %.0lf %.0lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		return buf;
	}
	void from_strtok() override {
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);
	}

	void        imgui() override { DragBox2("", &box); }
	void        draw(Vec2 offset, bool selected) override { draw_box2(box.offset(offset), selected, IM_COL32(255, 0, 0, 255)); }
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		return this;
	}

	Box2 box;
};

class Textbox: public EntityBase {
public:
	std::string type() override { return "textbox"; }
	std::string to_file() override {
		std::string str;
		snprintf(buf, sizeof(buf), "TEXTBOX %.0lf %.0lf %.0lf %.0lf %s ", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y, render_bundle.c_str());
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
	void from_strtok() override {
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);

		char *t          = TOKEN;
		render_bundle    = (t ? t : "");
		t                = TOKEN;
		std::string temp = (t ? t : "");
		text.clear();
		for (int i = 0; i < temp.size(); i++) {
			if (temp[i] == '\\') {
				if (i == temp.size() - 1)
					text.push_back('\\'); // Weird case
				else {
					if (temp[i + 1] == 's')
						text.push_back(' ');
					else if (temp[i + 1] == 'n')
						text.push_back('\n');
					else if (temp[i + 1] == 't')
						text.push_back('\t');
					i++;
				}
			} else
				text.push_back(temp[i]);
		}
	}

	void imgui() override {
		DragBox2("", &box);
		ig::InputText("Render Bundle", &render_bundle);
		ig::InputTextMultiline("##Text", &text, ImVec2(-1, 0));
	}
	void draw(Vec2 offset, bool selected) override {
		draw_box2(box.offset(offset), selected, IM_COL32(0, 0, 255, 255));
		// Draw text
		ImDrawList *list = ig::GetBackgroundDrawList();
		list->AddText((box.lefttop + offset).im(), IM_COL32_WHITE, text.c_str());
		list->AddText((box.lefttop + offset + Vec2(0, box.size.y)).im(), IM_COL32_WHITE, render_bundle.c_str());
	}
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		return this;
	}

	std::string text;
	std::string render_bundle;
	Box2        box;
};


class LevelTransition: public EntityBase {
public:
	std::string type() override { return "level_transition"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "LEVEL_TRANSITION %.0lf %.0lf %.0lf %.0lf %s\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y, next_level.c_str());
		return buf;
	}

	void from_strtok() override {
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);

		char *t = TOKEN;
		if (t != NULL)
			next_level = t;
		else
			next_level.clear();
	}

	void imgui() override {
		DragBox2("", &box);
		ig::InputText("Next Level", &next_level);
	}
	void draw(Vec2 offset, bool selected) override {
		draw_box2(box.offset(offset), selected, IM_COL32(255, 255, 0, 255));
		// Draw text
		ImDrawList *list = ig::GetBackgroundDrawList();
		list->AddText((box.lefttop + offset).im(), IM_COL32_WHITE, next_level.c_str());
	}
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		return this;
	}

	Box2        box;
	std::string next_level;
};


class CameraFocus: public EntityBase {
public:
	std::string type() override { return "camera_focus"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "CAMERA_FOCUS %.0lf %.0lf %.0lf %.0lf\n", box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		return buf;
	}
	void from_strtok() override {
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);
	}

	void        imgui() override { DragBox2("", &box); }
	void        draw(Vec2 offset, bool selected) override { draw_box2(box.offset(offset), selected, IM_COL32(0, 255, 255, 255)); }
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		return this;
	}

	Box2 box;
};

class FillBox: public EntityBase {
public:
	std::string type() override { return "fill"; }
	std::string to_file() override {
		snprintf(buf, sizeof(buf), "FILL %.0f %.0f %.0f %.0lf %.0lf %.0lf %.0lf\n", color[0] * 255, color[1] * 255, color[2] * 255, box.lefttop.x, box.lefttop.y, box.size.x, box.size.y);
		return buf;
	}
	void from_strtok() override {
		color[0] = TOKEN_DOUBLE / 255.0;
		color[1] = TOKEN_DOUBLE / 255.0;
		color[2] = TOKEN_DOUBLE / 255.0;
		double a, b, c, d;
		a   = TOKEN_DOUBLE;
		b   = TOKEN_DOUBLE;
		c   = TOKEN_DOUBLE;
		d   = TOKEN_DOUBLE;
		box = Box2(a, b, c, d);
	}

	void imgui() override {
		ig::ColorEdit3("Fill", color, ImGuiColorEditFlags_PickerHueWheel);
		DragBox2("", &box);
	}
	void draw_prev(Vec2 offset, bool selected) override {
		ImDrawList *d     = ig::GetBackgroundDrawList();
		Box2        world = box.offset(offset);
		d->AddRectFilled(world.lefttop.im(), (world.lefttop + world.size).im(), ImColor(color[0], color[1], color[2]));
	}
	EntityBase *setpos(Vec2 pos) override {
		box.lefttop = pos;
		return this;
	}

	Box2  box;
	float color[3];
};
