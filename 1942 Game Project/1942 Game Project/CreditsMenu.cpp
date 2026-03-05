#include <iostream>
#include <string>

#include "CreditsMenu.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

CreditsMenu::CreditsMenu() {
	BackButton.SetTextIsInteractable(true); // Set-up the back button to be interactable with a mouse
}

string CreditsMenu::RunMenu() {
	MainMenuTitle.Render("1942", TextSizeScaler(60), sf::Color::Black, { -1, 0 }, true, true); // render the title text
	int menu_title_maximum_y_position = MainMenuTitle.GetSize(true).y + Registry::TextPadding; // determine how far down the frame the text goes,
	// used to determine where to render credits content

	int options_y_size = 0; // used to centre text on-screen
	options_y_size += CreditsContent.GetSize().y;
	options_y_size += BackButton.GetSize().y;

	int y_position = menu_title_maximum_y_position + ((Registry::window_size.y - menu_title_maximum_y_position) - options_y_size) / 2; // Position the credits text centrally in y axis
	CreditsContent.Render(
		"Game produced by: Thomas JEBSON\n\
\n\
Sounds:\n\
	Gun Shot #1 by danlucaz (Creative Commons 0). \n\
\n\
Font:\n\
	Jersey 15 by Sarah Cadigan-Fried (SIL Open Font License, Version 1.1).",
		TextSizeScaler(20), sf::Color::Black, { -1, y_position });
	y_position += CreditsContent.GetSize().y + Registry::TextPadding; // space the back button apart so it appears spaced below the credits menu text
	BackButton.Render("<Back>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });

	if (BackButton.Interact()) { // if back button clicked
		return Constants::MAIN_MENU; // return to main menu
	}

	return Constants::CREDITS_MENU; // stay on this scene for the next frame
}