#pragma once

#include <string>

#include "TextRenderer.h"

using namespace std;

class MainMenu {
	/*Used to render the game's main menu.*/
private:
	TextRenderer MainMenuTitle;
	TextRenderer PlayButton;
	TextRenderer OptionsButton;
	TextRenderer CreditsButton;
	TextRenderer QuitButton;

public:
	MainMenu();

	string RunMenu();
};