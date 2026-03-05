#pragma warning( disable : 4244)

#include <iostream>

#include <SFML/Graphics.hpp>

#include "Bullet.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

Bullet::Bullet() {
	Registry::TextureManager->AdvanceLoad(Constants::BULLET_TEXTURE); // Tell the texture manager to begin loading this texture in the background
	// if it hasn't been already. Then by the time the code gets to 'compute' the texture exists, reducing the time spent waiting for secondary
	// storage.
}

void Bullet::SetShotFrom(string Shooter) {
	ShotFrom = Shooter;
}

void Bullet::SetPosition(sf::Vector2i& bullet_position) {
	BulletSprite.setPosition(sf::Vector2f(bullet_position.x, bullet_position.y));
}

void Bullet::Compute() {
	if (Loaded == false) { // if the sprite hasn't been assigned a texture yet, do it here.
		BulletTexture = Registry::TextureManager->GetTexture(Constants::BULLET_TEXTURE); // Get the texture we asked the texture manager
		// to load earlier (by reference)
		BulletSprite.setTexture(*BulletTexture);
		BulletSprite.setScale(sf::Vector2f(Registry::UnitWindowScale * 0.1, Registry::UnitWindowScale * 0.1));
		BulletSpriteSize = sf::Vector2i(BulletSprite.getGlobalBounds().getSize());
		BulletSprite.setOrigin(BulletSpriteSize.x / 2, BulletSpriteSize.y / 2); // set the origin to the middle of the sprite.
		Loaded = true;
	}

	BulletSprite.setScale(sf::Vector2f(Registry::UnitWindowScale * 0.1, Registry::UnitWindowScale * 0.1)); // scale the sprite so it scales with
	// window size, but also generally appears small on-screen.
	BulletSpriteSize = sf::Vector2i(BulletSprite.getGlobalBounds().getSize());
	BulletSprite.setOrigin(BulletSpriteSize.x / 2, BulletSpriteSize.y / 2);
	sf::Vector2f bullet_position = BulletSprite.getPosition();

	bullet_position += Velocity * Registry::DisplayRefreshTime * Registry::UnitWindowScale; // move the bullet around the window by applying velocity.

	BulletSprite.setPosition(bullet_position);

	BulletSpriteRect = sf::IntRect(BulletSprite.getGlobalBounds()); // update the bullet rect for the next round of collision detection
}

void Bullet::Render() {
	Registry::window->draw(BulletSprite);
}

void Bullet::SetFiringAngle(sf::Vector2f NewVelocity) {
	Velocity = NewVelocity * (float)Constants::BULLET_SPEED; // set bullet velocity
	float Rotation = atan2(NewVelocity.y, NewVelocity.x) * (180 / Constants::PI);
	BulletSprite.setRotation(Rotation + 90); // rotate the sprite so it faces in the direction of motion
}

Bullet::~Bullet() {
	Loaded = false;
	Registry::TextureManager->GarbageCollect(Constants::BULLET_TEXTURE); // reduce number of references - then if 0 texture also garbage collected.
	BulletTexture = nullptr; // clean up pointer to avoid memory leak
}