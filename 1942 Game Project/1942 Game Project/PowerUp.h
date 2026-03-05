#pragma once

#include <SFML/Graphics.hpp>

#include "TextRenderer.h"

class PowerUp {
	/*This class renders the power up text that spawns when the player manages to defeat a large enemy plane. This spawns approximately
	in the location of the large enemy plane's defeat, then moves down the screen slowly, meaning you have a limited amount of time
	to get it.*/
private:
	TextRenderer PowerUpText; // Renders the power up text on-screen. Note that this text is intractable only be other objects in the game
	// NOT by the player using their keyboard/mouse, like other text objects.
	sf::Vector2f Position; // Stores the position to render the text at. Initially the large enemy plane's last position, before
	// slowly moving down the frame.
public:
	sf::IntRect PowerUpGloablBounds; // Stores the power up text rect, which is used in collisions with the player.

	void Initialise(const sf::Vector2i& NewPosition); // Initialise, setting the position to the large enemy plane's last position.

	bool Compute();

	void Render();
};