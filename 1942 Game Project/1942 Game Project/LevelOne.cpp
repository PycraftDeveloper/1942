#pragma warning( disable : 4244)
#pragma warning( disable : 4018)

#include <string>
#include <iostream>

#include "Utils.h"

#include "LevelOne.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

string LevelOne::RunMenu() {
	if (LevelDescriptorPosition < Registry::window_size.y && RenderLevelDetailText) { // Only render and move the level descriptor when
		// visible on-screen
		LevelDescriptorPosition += UIVerticalSize; // Stores the height in pixels that the UI extends down from the top of the screen
		// which is used to ensure the UI and level descriptors aren't overlapping.
		LevelDetailText.Render("<Level One>", TextSizeScaler(50), sf::Color::Black, { -1, (int)LevelDescriptorPosition });
		LevelDescriptorPosition += (Constants::BACKGROUND_SCROLL_SPEED * Registry::DisplayRefreshTime) - UIVerticalSize; // Move text downwards, ignoring the UI height
	}
	else {
		RenderLevelDetailText = false;
	}

	if (Compute()) { // if player fails
		return Constants::END_MENU;
	}

	if (PauseButton.Interact()) { // if player clicks pause button
		return Constants::PAUSE_MENU;
	}

	if (LevelRunTime > Constants::LEVEL_ONE_RUN_TIME) { // if player lasts the entire level
		return Constants::LEVEL_TWO;
	}

	return Constants::LEVEL_ONE;
}