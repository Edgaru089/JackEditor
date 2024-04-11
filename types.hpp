#pragma once

#include "gui/imgui/imgui.h"
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <SFML/System.hpp>


typedef enum {
	render_Lines,     // Individial lines
	render_LineStrip, // Connected lines
	render_Polygon    // Filled polygon
} render_PrimitiveType;


// render_Primitive describes one render operation.
typedef struct {
	// FillMode mode;   // fill mode
	uint32_t fg, bg; // foreground & background colors

	render_PrimitiveType type;
	std::vector<ImVec2>  points;
} render_Primitive;

typedef struct {
	std::string                   name;
	std::vector<render_Primitive> prims;
} render_Bundle;


class Vec2 {
public:
	Vec2(): x(0.0), y(0.0) {}
	Vec2(double x, double y): x(x), y(y) {}

	Vec2(ImVec2 iv): x(iv.x), y(iv.y) {}

	ImVec2 im() { return ImVec2(x, y); }

public:
	Vec2 operator+(Vec2 other) const {
		return Vec2(x + other.x, y + other.y);
	}
	Vec2 operator*(double other) const {
		return Vec2(x * other, y * other);
	}

	bool operator==(Vec2 other) const {
		return x == other.x && y == other.y;
	}
	bool operator!=(Vec2 other) const { return !(*this == other); }

	double x, y;
};

class Box2 {
public:
	Box2(): lefttop(), size() {}
	Box2(Vec2 lefttop, Vec2 size): lefttop(lefttop), size(size) {}
	Box2(double ltx, double lty, double sx, double sy): lefttop(ltx, lty), size(sx, sy) {}

public:
	Box2 offset(Vec2 off) {
		return Box2(lefttop + off, size);
	}

	Vec2 lefttop;
	Vec2 size;
};
