#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "imgui/imgui.h"
#include "imgui-sfml/imgui-SFML.h"


#define ig ImGui

class UI {
public:
	UI(sf::Vector2u size, std::string title);
	~UI();

	// Process events, prepares ImGUI frame.
	// Return false if window closed.
	bool Update();


	// Finishes a frame.
	void Render();

private:
	sf::RenderWindow win;
	sf::Clock        delta;
	bool             running;
};
