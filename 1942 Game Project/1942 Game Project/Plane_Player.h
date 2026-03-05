#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Bullet.h"
#include "PowerUpPlane.h"

using namespace std;

class Player {
	/*This class represents the player in this game. Typically the instantiated version of this object is referenced as the
	'protagonist'. This plane is locked to always face and shoot upwards, but is free to move around the screen.*/
private:
	sf::Texture* Texture; // Stores the plane's associated texture
	sf::Sprite Sprite;
public:
	sf::Vector2f Position; // stored as a vector2f, as position is adjusted on a per frame basis, and this could be a small fraction of a pixel,
	// especially at high refresh rates.
	sf::Vector2i SpriteSize;
	sf::IntRect SpriteRect;
	sf::Transform Transform; // Stores the plane's transform, which is needed for the separate axis theorem calculations, needed in
	// determining when the enemy one planes have this plane in their sights.
	sf::IntRect LocalSpriteRect; // Stores the plane's local position, which is also used in the separate axis theorem calculations
	// because the sprite's rect doesn't consider rotations.
private:
	float BulletLastFiredTime = -10; // Used to determine when the plane last fired, set up to allow immediate firing
public:
	int RemainingLives = 3; // Used to store how many lives the player has, once this hits zero, the next hit ends the game
	int Movement[2]; // Used to determine how to move the player's plane based on the user's input. (see event handling in main.cpp)
private:
	bool Loaded = false;
public:
	Player();

	void Compute(PowerUpPlane* Left_PowerUpPlane, PowerUpPlane* Right_PowerUpPlane); // ...
	// Used to compute the plane, which handles any position, texturing and sprite changes. Note here the additional inputs for the player's power up
	// components, which are considered here to ensure the player's plane (including power ups) don't go off screen.

	void Render();

	bool FireBullets(vector<unique_ptr<Bullet>>& Bullets); // Used to fire bullets on behalf of the player. Called externally in 'main.cpp'

	~Player(); // Used to ensure that when this object is destroyed, the associated pointers are also cleaned up to prevent a memory leak.
};