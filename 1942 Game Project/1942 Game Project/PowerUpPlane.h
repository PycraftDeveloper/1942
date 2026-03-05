#pragma once

#include <SFML/Graphics.hpp>

#include "Bullet.h"

class PowerUpPlane {
	/*This class represents the player's accompanying power up planes. Unlike the player these planes can take only one hit
	but will often take a hit meant for the player, and are likewise positioned a few pixels further vertically down than the player
	to defend against attacks from the bottom.*/
private:
	sf::Texture* Texture;
	sf::Sprite Sprite;
	sf::Vector2i Position; // Stores the position of the plane.
	string PositionToPlayer; // Used to determine if on the left or right hand side of the player, which is needed when calculating
	// the plane's position.
public:
	sf::IntRect SpriteRect;
	sf::Vector2i SpriteSize;
private:
	bool Loaded = false;
public:
	PowerUpPlane(string Position);

	void Compute(sf::Vector2i ProtagonistPosition, sf::Vector2i ProtagonistSize); // Calculates the plane's position, relative to the player's
	// plane, and uses the player's size to ensure they are wing-tip to wing-tip.

	void Render();

	void FireBullets(vector<unique_ptr<Bullet>>& Bullets); // Used to fire bullets, will fire bullets when the player fires bullets, resulting in a much
	// wider spread - making the game easier.

	~PowerUpPlane(); // Destroys the object, ensuring that pointers are properly reset before exiting.
};