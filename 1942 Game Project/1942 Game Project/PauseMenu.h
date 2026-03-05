#pragma once

#include <string>

#include "TextRenderer.h"

using namespace std;

class PauseMenu {
	/*Used to render the game's pause menu. Note that when in this menu the game pauses, and is not reset until the player clicks
	the main menu button.*/
private:
	TextRenderer PauseMenuTitle;
	TextRenderer ResumeButton;
	TextRenderer MainMenuButton;
	TextRenderer QuitButton;
public:
	PauseMenu();

	string RunMenu();
};