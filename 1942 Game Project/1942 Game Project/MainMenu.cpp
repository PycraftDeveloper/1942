#include <iostream>
#include <string>

#include "MainMenu.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

MainMenu::MainMenu() {
	// Set-up the main menu buttons to be clickable
	PlayButton.SetTextIsInteractable(true);
	OptionsButton.SetTextIsInteractable(true);
	CreditsButton.SetTextIsInteractable(true);
	QuitButton.SetTextIsInteractable(true);
}

string MainMenu::RunMenu() {
	MainMenuTitle.Render("1942", TextSizeScaler(60), sf::Color::Black, { -1, 0 }, true, true);
	int menu_title_maximum_y_position = MainMenuTitle.GetSize(true).y + Registry::TextPadding;

	int options_y_size = 0; // Used to position the buttons centrally in the frame
	options_y_size += PlayButton.GetSize().y + Registry::TextPadding;
	options_y_size += OptionsButton.GetSize().y + Registry::TextPadding;
	options_y_size += CreditsButton.GetSize().y + Registry::TextPadding;
	options_y_size += QuitButton.GetSize().y;

	int y_position = menu_title_maximum_y_position + ((Registry::window_size.y - menu_title_maximum_y_position) - options_y_size) / 2;
	PlayButton.Render("<Play>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += OptionsButton.GetSize().y + Registry::TextPadding;
	OptionsButton.Render("<Options>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += CreditsButton.GetSize().y + Registry::TextPadding;
	CreditsButton.Render("<Credits>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += QuitButton.GetSize().y + Registry::TextPadding;
	QuitButton.Render("<Quit>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });

	if (PlayButton.Interact()) { // if play button clicked, change the menu to the first game level
		return Constants::LEVEL_ONE;
	}

	if (OptionsButton.Interact()) { // if the options button clicked, change the menu to the options menu
		return Constants::OPTIONS_MENU;
	}

	if (CreditsButton.Interact()) { // if the credits button clicked, change the menu to the credits menu
		return Constants::CREDITS_MENU;
	}

	if (QuitButton.Interact()) { // if the quit button clicked, change the menu to the quit menu
		return Constants::QUIT;
	}

	return Constants::MAIN_MENU; // Otherwise, on the next frame return to the main menu
}