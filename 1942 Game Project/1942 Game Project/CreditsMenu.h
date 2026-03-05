#pragma once

#include <string>

#include "TextRenderer.h"

using namespace std;

class CreditsMenu {
	/*Used to render the credits menu screen.*/
private:
	TextRenderer MainMenuTitle; // Used to render the game's title.
	TextRenderer CreditsContent; // Used to render the main credit's content
	TextRenderer BackButton; // Used to render the interactable back button
public:
	CreditsMenu(); // Used to create the menu, sets the back button text to interactable

	string RunMenu(); // Used to render and display the menu.
};