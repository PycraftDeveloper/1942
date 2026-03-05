#pragma warning( disable : 4244)
#pragma warning( disable : 4018)

#include <string>
#include <iostream>

#include "Utils.h"

#include "LevelTwo.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

string LevelTwo::RunMenu() {
	// Mostly similar to level one.
	if (LevelDescriptorPosition < Registry::window_size.y && RenderLevelDetailText) {
		LevelDescriptorPosition += UIVerticalSize;
		LevelDetailText.Render("<Level Two>", TextSizeScaler(50), sf::Color::Black, { -1, (int)LevelDescriptorPosition });
		LevelDescriptorPosition += (Constants::BACKGROUND_SCROLL_SPEED * Registry::DisplayRefreshTime) - UIVerticalSize;
	}
	else {
		RenderLevelDetailText = false;
	}

	if (Compute()) {
		return Constants::END_MENU;
	}

	if (PauseButton.Interact()) {
		return Constants::PAUSE_MENU;
	}

	if (LevelRunTime > Constants::LEVEL_TWO_RUN_TIME) { // if the player lasts the entire level, special message displayed on end menu
		// and extra points awarded.
		Registry::GiveUp = true;
		Registry::PlayerScore += 10000000; // 10 million point bonus
		return Constants::END_MENU; // go to end menu as there is no level three.
	}

	return Constants::LEVEL_TWO;
}