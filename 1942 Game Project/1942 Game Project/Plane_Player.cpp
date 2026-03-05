#pragma warning( disable : 4244)

#include <iostream>

#include "Plane_Player.h"
#include "Utils.h"
#include "PowerUpPlane.h"

#include "Bullet.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

Player::Player() {
	Registry::TextureManager->AdvanceLoad(Constants::MAIN_CHARACTER_TEXTURE); // load plane texture in advance if it doesn't exist yet
	Movement[0] = 0; // set the planes movement to zero in both axis
	Movement[1] = 0;
}

void Player::Compute(PowerUpPlane* Left_PowerUpPlane, PowerUpPlane* Right_PowerUpPlane) {
	if (Loaded == false) { // if the plane hasn't yet associated a texture to the sprite, do so now.
		Texture = Registry::TextureManager->GetTexture(Constants::MAIN_CHARACTER_TEXTURE); // get a reference to the texture I loaded in advance earlier
		Sprite.setTexture(*Texture);
		SpriteSize = sf::Vector2i(Sprite.getGlobalBounds().getSize());
		Sprite.setOrigin(SpriteSize.x / 2, SpriteSize.y / 2); // set the origin to the centre of the plane.
		Position = sf::Vector2f(Registry::window_size.x / 2, Registry::window_size.y / 2); // spawn the plane in the centre of the screen
		Loaded = true;
	}

	Sprite.setScale(sf::Vector2f(Registry::UnitWindowScale * 0.5, Registry::UnitWindowScale * 0.5)); // scale the plane, considering window size to ensure
	// the player is never appearing too small.
	SpriteSize = sf::Vector2i(Sprite.getGlobalBounds().getSize());

	// Calculate the plane's movement based on the user's input from main.cpp. Considers delta time to be frame rate independent.
	Position.x += Movement[0] * Registry::UnitWindowScale * Constants::PLAYER_MOVEMENT_SPEED * Registry::DisplayRefreshTime;
	Position.y += Movement[1] * Registry::UnitWindowScale * Constants::PLAYER_MOVEMENT_SPEED * Registry::DisplayRefreshTime;

	// power up planes only appear next to the player in the x axis, so only consider if the power up plane is off-screen in the x-axis
	// check if on the top left sides of the window
	if (Left_PowerUpPlane != nullptr) { // if the left power up plane exists
		if (Position.x - ((SpriteSize.x / 2) + (Left_PowerUpPlane->SpriteSize.x)) < 0) { // if the left power up plane is off screen
			Position.x = ((SpriteSize.x / 2) + (Left_PowerUpPlane->SpriteSize.x)); // move the player plane and power up plane back into the screen.
		}
	}
	else {
		if (Position.x - SpriteSize.x / 2 < 0) { // if the player plane is off screen (not considering the power up plane)
			Position.x = SpriteSize.x / 2; // move the player plane back into the screen.
		}
	}

	if (Position.y - SpriteSize.y / 2 < 0) {
		Position.y = SpriteSize.y / 2;
	}

	// check if on the bottom right sides of the window.
	if (Right_PowerUpPlane != nullptr) { // if the right power-up plane exists
		if (Position.x + ((SpriteSize.x / 2) + (Right_PowerUpPlane->SpriteSize.x)) > Registry::window_size.x) {
			Position.x = Registry::window_size.x - ((SpriteSize.x / 2) + (Right_PowerUpPlane->SpriteSize.x));
		}
	}
	else {
		if (Position.x + SpriteSize.x / 2 > Registry::window_size.x) {
			Position.x = Registry::window_size.x - SpriteSize.x / 2;
		}
	}

	if (Position.y + SpriteSize.y / 2 > Registry::window_size.y) {
		Position.y = Registry::window_size.y - SpriteSize.y / 2;
	}

	Sprite.setPosition(sf::Vector2f(Position));

	SpriteRect = sf::IntRect(Sprite.getGlobalBounds()); // get the global rect for the sprite, used in collision detection
	Transform = Sprite.getTransform(); // get the transform for the player, used in the separate axis theorem calculations
	LocalSpriteRect = sf::IntRect(Sprite.getLocalBounds()); // get the local rect, also used in the separate axis theorem calculations
}

void Player::Render() {
	Registry::window->draw(Sprite);
}

bool Player::FireBullets(vector<unique_ptr<Bullet>>& Bullets) {
	if (Registry::GameRunTime > BulletLastFiredTime + Constants::PLAYER_BULLET_FIRE_COOLDOWN) { // if the player is not on cool down for firing bullets
		pair<sf::Vector2i, sf::Vector2i> BulletInitialPositions = CalculateBulletPositions(sf::Vector2i(Position), 25, -15, 0); // calculate the positions the
		// bullets should spawn from relative to the player position.

		Bullets.emplace_back(make_unique<Bullet>()); // create the first bullet on the left
		Bullets.back()->SetPosition(BulletInitialPositions.first);
		Bullets.back()->SetFiringAngle(sf::Vector2f(0, -1)); // the firing angle is always up the frame
		Bullets.back()->SetShotFrom(Constants::PLAYER); // specifically indicate the bullet was fired from the player.

		Bullets.emplace_back(make_unique<Bullet>()); // create the second bullet on the right
		Bullets.back()->SetPosition(BulletInitialPositions.second);
		Bullets.back()->SetFiringAngle(sf::Vector2f(0, -1));
		Bullets.back()->SetShotFrom(Constants::PLAYER);

		BulletLastFiredTime = Registry::GameRunTime; // put the player on cool-down so they can't shoot again immediately.

		return true; // used to indicate the player has fired a bullet, which is used to trigger power up planes to fire and the sound
		// effects to play.
	}

	return false;
}

Player::~Player() {
	Loaded = false;
	Registry::TextureManager->GarbageCollect(Constants::MAIN_CHARACTER_TEXTURE); // make sure to tell the asset manager the texture is no longer
	// used here - this will decrement the reference counter to the texture, and if it is zero (which it should be as only ever one main character)
	// the texture gets garbage collected.
	Texture = nullptr; // ensure that the pointers are properly reset.
}