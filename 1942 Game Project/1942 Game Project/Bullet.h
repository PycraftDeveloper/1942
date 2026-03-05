#pragma once

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;

class Bullet {
	/*This class is used to create and display the bullets spawned when either the player or enemy fire.*/
private:
	sf::Texture* BulletTexture; // Stores a reference to the bullet texture, loaded centrally.
	sf::Sprite BulletSprite;
public:
	sf::Vector2i BulletSpriteSize;
	sf::IntRect BulletSpriteRect; // Stores the sprite rect for collisions later.
	sf::Vector2f Velocity; // Stores the velocity the bullet has when fired. Never updated after being fired, allows bullets to
	// move in the direction they where fired.
	string ShotFrom; // Stores the name of the object that fired the bullet, used to determine on collision if the player or enemy
	// has been hit (and to prevent the player hitting themselves).
	bool Loaded = false; // Used to determine if the sprite has been assigned a texture or not.

	Bullet();

	void SetShotFrom(string Shooter); // Used to specify which object in the game spawned the bullet.

	void SetPosition(sf::Vector2i& bullet_position); // Sets the position of the bullet, effectively it's starting position.

	void Compute(); // Used to displace the bullet based on its velocity, and update its hit-box for collisions and associate textures for rendering.

	void Render(); // Used to render the bullet to the screen.

	void SetFiringAngle(sf::Vector2f NewVelocity); // Sets the velocity the bullet will be fired at.

	~Bullet(); // Used to garbage collect the bullet when it is no longer needed.
};