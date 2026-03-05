#pragma once

#include <SFML/Graphics.hpp>

#include "PerlinNoise.h"

#include "Utils.h"

class EnemyOnePlane {
	/*This is used to create a new lower tier enemy plane for the 1942 game. This plane flies around using a position determined
	by Perlin noise, and can determine when the player is within it's 'sights' using the Separate Axis Theorem in order to appear to
	fight them. The plane also spawns off-screen and moves into the position the Perlin noise function starts at smoothly using a
	linear interpolation algorithm.*/
private:
	sf::Texture* PlaneTexture; // Stores a reference to the texture this plane renders with.
	PerlinNoise noise = PerlinNoise(rand() % 512, 1, 0); // Used to generate new positions for the plane to simulate the motion
	// seen in aerial combat.
	sf::Sprite PlaneSprite;
	sf::Vector2i PlaneAIMovementStartingPosition; // Stores where the Perlin noise algorithm starts, used to smoothly transition
	// from an off-screen spawning location to here.
	sf::Vector2i PlaneSpawningPosition; // Stores the planes spawning position, which is determined by extrapolating the angle
	// the plane takes in the first few samples of the Perlin noise algorithm back to the edge of the screen. (so the player
	// doesn't see them pop-in)
public:
	sf::Vector2i PlaneSpriteSize;
	sf::Vector2i CharacterPosition;
	sf::IntRect PlaneRect;
	sf::RectangleShape PlaneFOV; // Used to determine where the plane's sight is - so if the player is inside this rectangle, the
	// plane will begin to fire. Can be rendered in the render method for a graphical demonstration.

	float DynamicScaling = 0.5f; // Controls the plane's scale - this is used to give the illusion of depth for enemy planes that
	// might otherwise appear to collide in the air.
	float CharacterRotation; // Stores the rotation of the plane currently, this is determined by calculating the angle from where
	// they are now to where they will be (as perlin noise allows for ahead of time prediction).
private:
	float PlaneStartSpawnTime; // Determines what time the plane spawned at (relative to overall game time)
	float PlaneRunTime = 0.f; // Stores how long the plane has existed for - used in perlin noise calculations.
	float MoveInTime; // Stores how long it takes for the plane to interpolate from its starting location to the position needed
	// for the AI.
	float PositionNoiseOffset = random_float(0.0f, 13.0f); // used to ensure each plane doesn't pass through common points (centre).
	float BulletLastFiredTime = Registry::GameRunTime; // Stores when the plane last fired a bullet, used to control how frequently the plane shoots.

	bool Loaded = false;
	bool InPosition = false; // Used to determine if the plane is moving using the linear interpolation algorithm, or the Perlin
	// noise pathing.
	bool DefaultPositionSet = false; // Used to determine when to calculate the player's default starting position by process of
	// extrapolating from the AI start position.
public:
	EnemyOnePlane();

	void Compute();

	void Render();

	void FireBullets(vector<unique_ptr<Bullet>>& Bullets); // Used to fire two bullets from the plane when it is facing the player. These bullets
	// then become part of the game level and not associated to the enemy plane directly.

	~EnemyOnePlane(); // Used to garbage collect the plane to free up resources.
};