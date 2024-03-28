
#include "gui.hpp"
#include <SFML/Window/VideoMode.hpp>
#include "imgui-sfml/imgui-SFML.h"
#include "imgui/imgui_internal.h"

using namespace sf;


UI::UI(sf::Vector2u size, std::string title): win(VideoMode(size.x, size.y), title) {
	win.setFramerateLimit(200);
	win.setVerticalSyncEnabled(true);

	ig::SFML::Init(win, false);

	auto        &io = ig::GetIO();
	ImFontConfig cfg;
	cfg.OversampleH = 1;
	cfg.OversampleV = 1;
	cfg.SizePixels  = 16;
	io.Fonts->Clear();
	io.Fonts->AddFontFromFileTTF("unifont.otf", 16, &cfg, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());

	ig::SFML::UpdateFontTexture();
	win.resetGLStates();
}

UI::~UI() {
	win.close();
	ig::SFML::Shutdown();
}


bool UI::Update() {
	Event e;
	bool  closed = false;
	while (win.pollEvent(e)) {
		ig::SFML::ProcessEvent(e);

		if (e.type == Event::Closed)
			closed = true;
	}

	ig::SFML::Update(win, delta.restart());

	return !closed;
}

void UI::Render() {
	win.clear();
	ig::SFML::Render(win);
	win.display();
}
