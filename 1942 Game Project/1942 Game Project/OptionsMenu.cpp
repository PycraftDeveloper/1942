#include <iostream>
#include <string>

#include "OptionsMenu.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

OptionsMenu::OptionsMenu() {
	// Allow the user to interact with the options in this menu using their mouse
	SFXVolumeControl.SetTextIsInteractable(true);
	BackButton.SetTextIsInteractable(true);
}

string OptionsMenu::RunMenu() {
	MainMenuTitle.Render("1942", TextSizeScaler(60), sf::Color::Black, { -1, 0 }, true, true);
	int menu_title_maximum_y_position = MainMenuTitle.GetSize(true).y + Registry::TextPadding;

	int options_y_size = 0; // Used to centre the options in the frame
	options_y_size += SFXVolumeControl.GetSize().y + Registry::TextPadding * 2;
	options_y_size += BackButton.GetSize().y;

	int y_position = menu_title_maximum_y_position + ((Registry::window_size.y - menu_title_maximum_y_position) - options_y_size) / 2; // something
	SFXVolumeControl.Render("<Sound Effect Volume: " + to_string(Registry::SFXVolume) + "%" + SFXVolumeControlHint + ">", TextSizeScaler(30), sf::Color::Black, { -1, y_position });
	y_position += SFXVolumeControl.GetSize().y + Registry::TextPadding * 2;
	BackButton.Render("<Back>", TextSizeScaler(30), sf::Color::Black, { -1, y_position });

	if (SFXVolumeControl.IsTextHovering()) { // The SFXVolume control is not a button, but something the user hovers over with the mouse to change the volume
		// by scrolling
		Registry::SFXVolume = clamp(Registry::SFXVolume + Registry::MouseScrollWheelDelta, 0, 100);
		SFXVolumeControlHint = " (scroll) "; // Display a hint to the user on how to edit the setting.
	}
	else {
		SFXVolumeControlHint = "";
	}

	if (BackButton.Interact()) { // if the back button clicked, change the menu to the main menu
		return Constants::MAIN_MENU;
	}

	return Constants::OPTIONS_MENU; // Otherwise, on the next frame return to the options menu
}