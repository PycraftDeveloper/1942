#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "TextRenderer.h"

using namespace std;

class OptionsMenu {
	/*Used to render the game's options menu.*/
private:
	TextRenderer MainMenuTitle;
	TextRenderer SFXVolumeControl; // Unlike other text objects, this one is not clickable, but triggers an event when hovered.
	TextRenderer BackButton;

	string SFXVolumeControlHint = ""; // Used to tell the user how to interact with the option.
public:
	OptionsMenu();

	string RunMenu();
};