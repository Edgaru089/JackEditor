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
