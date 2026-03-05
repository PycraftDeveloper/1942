#pragma warning( disable : 4244)

#include <iostream>

#include "PowerUpPlane.h"
#include "Utils.h"

#include "Bullet.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

PowerUpPlane::PowerUpPlane(string Position) {
	Registry::TextureManager->AdvanceLoad(Constants::POWER_UP_PLANE_TEXTURE); // load the plane texture in advance if it doesn't exist yet
	PositionToPlayer = Position; // set the position of the plane to be either left or right of the player
}

void PowerUpPlane::Compute(sf::Vector2i ProtagonistPosition, sf::Vector2i ProtagonistSize) {
	if (Loaded == false) { // if the plane hasn't yet associated a texture to the sprite, do so now.
		Texture = Registry::TextureManager->GetTexture(Constants::POWER_UP_PLANE_TEXTURE); // get a reference to the texture I loaded in advance earlier
		Sprite.setTexture(*Texture);
		SpriteSize = sf::Vector2i(Sprite.getGlobalBounds().getSize());
		Sprite.setOrigin(SpriteSize.x / 2, SpriteSize.y / 2); // set the origin to the centre of the plane.
		Loaded = true;
	}

	Sprite.setScale(sf::Vector2f(Registry::UnitWindowScale * 0.5, Registry::UnitWindowScale * 0.5)); // scale the plane with window size
	SpriteSize = sf::Vector2i(Sprite.getGlobalBounds().getSize());

	Position = ProtagonistPosition; // Get the player's position so the power up can be positioned relative to it on either the left or right side.

	if (PositionToPlayer == Constants::LEFT) {
		Position.x -= (ProtagonistSize.x / 2) + (SpriteSize.x / 2);
	}
	else {
		Position.x += (ProtagonistSize.x / 2) + (SpriteSize.x / 2);
	}

	Position.y += 3; // Offset down so are destroyed before player is.

	Sprite.setPosition(sf::Vector2f(Position));

	SpriteRect = sf::IntRect(Sprite.getGlobalBounds()); // get the global bounds for the rect used to check if power up has collided.
}

void PowerUpPlane::Render() {
	Registry::window->draw(Sprite);
}

void PowerUpPlane::FireBullets(vector<unique_ptr<Bullet>>& Bullets) {
	// no need for any cool down as the power up planes only fire if the player has fired bullets
	pair<sf::Vector2i, sf::Vector2i> BulletInitialPositions = CalculateBulletPositions(Position, 25, -15, 0); // calculate the bullet positions relative to the
	// power up plane's position so the bullets are fired from under the planes wings. Note there is no rotation here either as power up plane like the player
	// is locked to always point up.

	Bullets.emplace_back(make_unique<Bullet>());
	Bullets.back()->SetPosition(BulletInitialPositions.first);
	Bullets.back()->SetFiringAngle(sf::Vector2f(0, -1));
	Bullets.back()->SetShotFrom(Constants::PLAYER);

	Bullets.emplace_back(make_unique<Bullet>());
	Bullets.back()->SetPosition(BulletInitialPositions.second);
	Bullets.back()->SetFiringAngle(sf::Vector2f(0, -1));
	Bullets.back()->SetShotFrom(Constants::PLAYER);
}

PowerUpPlane::~PowerUpPlane() {
	Loaded = false;
	Registry::TextureManager->GarbageCollect(Constants::POWER_UP_PLANE_TEXTURE); // make sure to tell the asset manager the texture is no longer used here
	// this allows it to garbage collect the texture when it is no longer used in the entire level.
	Texture = nullptr; // make sure all pointers are properly reset before exiting.
}