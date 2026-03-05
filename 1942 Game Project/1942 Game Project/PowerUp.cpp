#include "PowerUp.h"

#include "Utils.h"

void PowerUp::Initialise(const sf::Vector2i& NewPosition) {
	Position = sf::Vector2f(NewPosition);
}

bool PowerUp::Compute() {
	Position.y += Constants::BACKGROUND_SCROLL_SPEED * Registry::DisplayRefreshTime; // move the power up text down the screen at the same rate as the background scrolls.

	PowerUpGloablBounds = sf::IntRect(PowerUpText.GetTextGloablBounds()); // get the global bounds for the rect used to check if player has collided.

	if (Position.y + PowerUpText.GetSize().y / 2.f < 0) { // if the text is off the screen, the power up is no longer valid (and is garbage collected)
		return true;
	}

	return false;
}

void PowerUp::Render() {
	PowerUpText.Render("PowerUp!", TextSizeScaler(30), sf::Color::Black, sf::Vector2i(Position), false, false);
}