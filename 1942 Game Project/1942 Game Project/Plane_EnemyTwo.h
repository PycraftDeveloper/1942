#pragma once

#include <SFML/Graphics.hpp>

#include "Plane_Player.h"

#include "Bullet.h"

#include "Registry.h"

class EnemyTwoPlane {
	/*Used to create the higher tier enemy plane for the 1942 game. This plane spawns off the bottom of the screen and moves
	vertically up the frame. Also can target and shoot the player from anywhere with a 270 degree field of view (see write up)
	meaning the plane has a blind spot behind it. Can see across the entire screen.*/
private:
	sf::Texture* PlaneTexture; // Stores a reference to the texture the plane renders with.
	sf::Sprite PlaneSprite;
public:
	sf::Vector2i PlaneSpriteSize; // Stores the size of the sprite
	sf::Vector2f CharacterPosition; // Stores the position of the sprite, as this moves with respect to time, it needs to be stored as a vector2f
	sf::IntRect PlaneRect; // Stores the sprite's rectangle, used in collision detection
	float BulletLastFiredTime = Registry::GameRunTime; // Stores when the plane last fired a bullet, used to control how frequently the plane shoots.
	int Health = 5 + rand() % 5; // Randomly generates an initial 'health' value for the plane, meaning it can sustain multiple hits
	// min: 5, max: 9
private:
	bool Loaded = false; // Used to determine if the sprite has a texture loaded or not.
public:
	EnemyTwoPlane();

	bool Compute();

	void Render();

	~EnemyTwoPlane(); // Used to garbage collect the plane's assets when no longer needed.

	void FireBullets(vector<unique_ptr<Bullet>>& Bullets, Player& Protagonist); // Used to fire four bullets from the plane when it is facing the player. These bullets
	// then become part of the game level and not associated to the enemy plane directly.
};