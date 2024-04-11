#pragma once

#include "gui/gui.hpp"
#include "gui/imgui/imgui.h"
#include "types.hpp"
#include <string>

static inline Vec2 DragButton(const char *pushid) {
	if (strlen(pushid) > 0)
		ig::PushID(pushid);

	Vec2 d; // = 0
	ig::Button("Drag");
	if (ig::IsItemActive())
		d = ig::GetIO().MouseDelta;

	if (strlen(pushid) > 0)
		ig::PopID();
	return d;
}

static inline bool DragVec2(const char *label, Vec2 *vec2, float speed = 1.0f) {
	float f[2] = {(float)vec2->x, (float)vec2->y};
	bool  ok   = ig::DragFloat2(label, f, speed);
	vec2->x    = f[0];
	vec2->y    = f[1];

	ig::SameLine();
	Vec2 delta = DragButton(label);
	if (delta != Vec2()) {
		ok    = true;
		*vec2 = *vec2 + delta;
	}

	return ok;
}

static inline bool DragBox2(const char *pushid, Box2 *box2, float speed = 1.0f) {
	if (strlen(pushid) > 0)
		ig::PushID(pushid);

	bool ok1 = DragVec2("Lefttop", &box2->lefttop, speed);
	bool ok2 = DragVec2("Size", &box2->size, speed);

	if (strlen(pushid) > 0)
		ig::PopID();

	return ok1 || ok2;
}

static inline bool DragDouble(const char *label, double *d, float speed = 1.0f) {
	float f  = (float)*d;
	bool  ok = ig::DragFloat(label, &f, speed);
	*d       = f;
	return ok;
}
