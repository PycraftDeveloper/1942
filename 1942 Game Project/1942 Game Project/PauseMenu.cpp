#include <iostream>
#include <string>

#include "PauseMenu.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

PauseMenu::PauseMenu() {
	// Allow the player to interact with the buttons with their mouse.
	ResumeButton.SetTextIsInteractable(true);
	MainMenuButton.SetTextIsInteractable(true);
	QuitButton.SetTextIsInteractable(true);
}

string PauseMenu::RunMenu() {
	// Note the game in the background is frozen whilst in this menu.
	PauseMenuTitle.Render("1942", TextSizeScaler(60), sf::Color::Black, { -1, 0 }, true, true);
	int menu_title_maximum_y_position = PauseMenuTitle.GetSize(true).y + Registry::TextPadding;

	int options_y_size = 0;
	options_y_size += ResumeButton.GetSize().y + Registry::TextPadding;
	options_y_size += QuitButton.GetSize().y;

	int y_position = menu_title_maximum_y_position + ((Registry::window_size.y - menu_title_maximum_y_position) - options_y_size) / 2; // something
	ResumeButton.Render("<Resume>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += MainMenuButton.GetSize().y + Registry::TextPadding;
	MainMenuButton.Render("<Main Menu>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += QuitButton.GetSize().y + Registry::TextPadding;
	QuitButton.Render("<Quit>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });

	if (ResumeButton.Interact()) { // if the player clicks on the resume button.
		return Registry::MenuStack[1]; // return to the previous menu.
	}

	if (MainMenuButton.Interact()) { // if the player clicks on the main menu button.
		return Constants::MAIN_MENU; // return to the main menu.
	}

	if (QuitButton.Interact()) { // if the player clicks on the quit button.
		return Constants::QUIT; // exit the game.
	}

	return Constants::PAUSE_MENU; // otherwise stay on current menu.
}