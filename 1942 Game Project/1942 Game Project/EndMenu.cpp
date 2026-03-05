#include <iostream>
#include <string>

#include "EndMenu.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

EndMenu::EndMenu() {
	// allow the buttons to be intractable with a mouse.
	PlayAgainButton.SetTextIsInteractable(true);
	MainMenuButton.SetTextIsInteractable(true);
	QuitButton.SetTextIsInteractable(true);
}

string EndMenu::RunMenu() {
	MainMenuTitle.Render("1942", TextSizeScaler(60), sf::Color::Black, { -1, 0 }, true, true);
	int menu_title_maximum_y_position = MainMenuTitle.GetSize(true).y + Registry::TextPadding;

	int options_y_size = 0; // Used to position text content centrally in the frame
	if (Registry::GiveUp) { // Used to consider special scenario text
		options_y_size += GiveUpText.GetSize().y + Registry::TextPadding;
	}
	options_y_size += PlayerResultsText.GetSize().y + Registry::TextPadding * 2;
	options_y_size += PlayAgainButton.GetSize().y + Registry::TextPadding;
	options_y_size += MainMenuButton.GetSize().y + Registry::TextPadding;
	options_y_size += QuitButton.GetSize().y;

	int y_position = menu_title_maximum_y_position + ((Registry::window_size.y - menu_title_maximum_y_position) - options_y_size) / 2; // something
	if (Registry::GiveUp) {
		if (ChangeColorTimer > 1 / 15.0f) { // every 15 frames update the text colour for the special text, slow enough not to cause much
			// visual discomfort
			ChangeColorTimer = 0;
			RandomColor = sf::Color(rand() % 256, rand() % 256, rand() % 256, 255); // generate new random colour
		}
		GiveUpText.Render("WE GIVE UP!", TextSizeScaler(50), RandomColor, { -1, y_position }, false, true);
		y_position += GiveUpText.GetSize().y + Registry::TextPadding;
	}

	// render level contents
	PlayerResultsText.Render("Your score: " + to_string(Registry::PlayerScore), TextSizeScaler(40), sf::Color::Black, { -1, y_position }, false, true);
	y_position += PlayerResultsText.GetSize().y + Registry::TextPadding * 2;
	PlayAgainButton.Render("<Play Again>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += MainMenuButton.GetSize().y + Registry::TextPadding;
	MainMenuButton.Render("<Main Menu>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += QuitButton.GetSize().y + Registry::TextPadding;
	QuitButton.Render("<Quit>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });

	ChangeColorTimer += Registry::DisplayRefreshTime;

	if (PlayAgainButton.Interact()) { // if player wants to play again, take them back to the first level
		return Constants::LEVEL_ONE;
	}

	if (MainMenuButton.Interact()) { // if the player clicks the main menu button, take them there
		return Constants::MAIN_MENU;
	}

	if (QuitButton.Interact()) { // if the player clicks the quit button, exit the game
		return Constants::QUIT;
	}

	return Constants::END_MENU; // otherwise stay on this level for the next frame
}