#pragma warning( disable : 4244)

#include <SFML/Graphics.hpp>
#include <iostream>

#include "Plane_EnemyTwo.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

EnemyTwoPlane::EnemyTwoPlane() {
	// Load the enemy texture in advance to avoid stuttering when the plane is created.
	Registry::TextureManager->AdvanceLoad(Constants::ENEMY_TWO_TEXTURE);
}

bool EnemyTwoPlane::Compute() {
	if (Loaded == false) { // if the texture hasn't been assigned to the sprite yet.
		PlaneTexture = Registry::TextureManager->GetTexture(Constants::ENEMY_TWO_TEXTURE); // Get the texture or wait for it to finish loading.
		PlaneSprite.setTexture(*PlaneTexture);
		PlaneSpriteSize = sf::Vector2i(PlaneSprite.getGlobalBounds().getSize());
		PlaneSprite.setOrigin(PlaneSpriteSize.x / 2, PlaneSpriteSize.y / 2); // Set origin to the centre of the plane.
		PlaneSprite.setRotation(180); // face the plane upwards (as this is the only direction it can go in).
		CharacterPosition = sf::Vector2f(
			PlaneSpriteSize.x / 2.0f + (rand() % static_cast<int>(Registry::window_size.x - PlaneSpriteSize.x)),
			Registry::window_size.y + PlaneSpriteSize.y / 2.0f); // spawn the plane randomly in the x axis, and off the bottom of the screen
		// in the y axis
		Loaded = true;
	}

	PlaneSprite.setScale(sf::Vector2f(Registry::UnitWindowScale * 0.75, Registry::UnitWindowScale * 0.75)); // 0.5 -> 0.75 so larger.
	PlaneSpriteSize = sf::Vector2i(PlaneSprite.getGlobalBounds().getSize());

	PlaneSprite.setPosition(CharacterPosition);

	CharacterPosition.y -= Constants::ENEMY_PLANE_TWO_SPEED * Registry::UnitWindowScale * Registry::DisplayRefreshTime; // move the plane up the frame every frame
	// considering the time delta between frames for smoother movement

	PlaneRect = sf::IntRect(PlaneSprite.getGlobalBounds());

	if (CharacterPosition.y + PlaneSpriteSize.y / 2.f < 0) { // if the plane has moved off the top of the screen, garbage collect it
		return true;
	}
	return false;
}

void EnemyTwoPlane::Render() {
	Registry::window->draw(PlaneSprite);
}

EnemyTwoPlane::~EnemyTwoPlane() { // used to properly garbage collect the pointers to the associated texture - note the texture is not destroyed
	// but the reference counter to it is adjusted (which can trigger it to be destroyed).
	Loaded = false;
	PlaneTexture = nullptr;
	Registry::TextureManager->GarbageCollect(Constants::ENEMY_TWO_TEXTURE);
}

void EnemyTwoPlane::FireBullets(vector<unique_ptr<Bullet>>& Bullets, Player& Protagonist) { // fires four bullets not two like other planes
	// Determine the firing frequency.
	int LevelNumber = 0;
	if (Registry::MenuStack[0] == Constants::LEVEL_TWO) {
		LevelNumber = 1;
	}

	if (Registry::GameRunTime > BulletLastFiredTime + Constants::ENEMY_TWO_BULLET_FIRE_COOLDOWN[LevelNumber]) { // if the plane is ready to fire and no longer on cool down
		pair<sf::Vector2i, sf::Vector2i> InnerBulletInitialPositions = CalculateBulletPositions(sf::Vector2i(CharacterPosition), 25, -5, 0); // get the positions for the inner
		// two of the four bullets, which are fired at the player.
		pair<sf::Vector2i, sf::Vector2i> OuterBulletInitialPositions = CalculateBulletPositions(sf::Vector2i(CharacterPosition), 50, -5, 0); // get the positions for the outer
		// two of the four bullets, which are fired at the player.

		// calculate which direction to fire the bullets in, based on the position of the player and the plane.
		sf::Vector2f direction = sf::Vector2f(
			Protagonist.Position.x - CharacterPosition.x,
			Protagonist.Position.y - CharacterPosition.y
		); // get the displacement direction between the player and the current position

		float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
		if (length != 0.f) {
			direction /= length; // normalise the direction vector
		}

		// calculate the plane's blind spot (45 degrees either side the bottom of the plane, look at the FOV map in the write up).
		sf::Vector2f up = sf::Vector2f(0.f, 1.f);
		float dot = direction.x * up.x + direction.y * up.y;

		if (dot > Constants::ENEMY_TWO_BLIND_SPOT_SIZE[LevelNumber]) { // adjust the blind spot size based on level progression
			return;
		}

		// create the bullets and set their positions and firing angles
		Bullets.emplace_back(make_unique<Bullet>());
		Bullets.back()->SetPosition(InnerBulletInitialPositions.first);
		Bullets.back()->SetFiringAngle(direction);
		Bullets.back()->SetShotFrom(Constants::ENEMY);

		Bullets.emplace_back(make_unique<Bullet>());
		Bullets.back()->SetPosition(InnerBulletInitialPositions.second);
		Bullets.back()->SetFiringAngle(direction);
		Bullets.back()->SetShotFrom(Constants::ENEMY);

		Bullets.emplace_back(make_unique<Bullet>());
		Bullets.back()->SetPosition(OuterBulletInitialPositions.first);
		Bullets.back()->SetFiringAngle(direction);
		Bullets.back()->SetShotFrom(Constants::ENEMY);

		Bullets.emplace_back(make_unique<Bullet>());
		Bullets.back()->SetPosition(OuterBulletInitialPositions.second);
		Bullets.back()->SetFiringAngle(direction);
		Bullets.back()->SetShotFrom(Constants::ENEMY);

		BulletLastFiredTime = Registry::GameRunTime; // reset when the bullets where last fired to allow for a cool down

		Registry::SFXManager->PlayOneshot(Constants::BULLET_FIRE_SFX); // play the bullet fire sound effect
	}
}