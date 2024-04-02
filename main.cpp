#include <SFML/System.hpp>
#include <cmath>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

#include "gui/gui.hpp"
#include "gui/imgui/imgui.h"
#include "gui/imgui/imgui_stdlib.h"

#include "types.hpp"

using namespace std;


void ui_bundle();
void ui_map();


int main(int argc, char *argv[]) {
	UI ui(sf::Vector2u(1200, 1000), "JackEditor");

	while (ui.Update()) {
		static bool demo_open = false;

		static bool bundleOpen = false, mapOpen = true;
		ig::BeginMainMenuBar();
		ig::Checkbox("Demo Window", &demo_open);
		ig::Separator();
		ig::Checkbox("Bundle", &bundleOpen);
		ig::Separator();
		ig::Checkbox("Map", &mapOpen);
		ig::EndMainMenuBar();

		if (demo_open)
			ig::ShowDemoWindow(&demo_open);
		if (bundleOpen)
			ui_bundle();


		ui.Render();
	}


	return 0;
}
