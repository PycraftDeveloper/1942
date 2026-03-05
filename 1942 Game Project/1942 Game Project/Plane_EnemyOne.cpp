#pragma warning( disable : 4244)
#pragma warning( disable : 4838)

#include <iostream>

#include "Plane_EnemyOne.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

// For the enemy one plane, it spawns in off-screen and uses a linear interpolation to move from that off-screen location to
// the point where perlin noise starts. This is done to ensure that the plane doesn't pop in, and because I can't easily ensure that
// the perlin noise algorithm stars off-screen. Once the plane reaches the point where perlin noise starts, the position is determined
// using the perlin noise generation.

EnemyOnePlane::EnemyOnePlane() {
	Registry::TextureManager->AdvanceLoad(Constants::ENEMY_ONE_TEXTURE); // Tell the asset manager to load this texture in the
	// background, so that it is ready when I need it - provided it isn't already loaded.
	MoveInTime = random_float(1.5f, 3.0f); // randomly generate the time it takes to move into the position where the AI takes control.
}

void EnemyOnePlane::Compute() {
	float NewRotation; // Stores the new rotation of the plane, this is determined by the direction the plane is moving in.
	if (Loaded == false) { // if the texture has not been assigned to the sprite, do that here.
		PlaneTexture = Registry::TextureManager->GetTexture(Constants::ENEMY_ONE_TEXTURE);
		PlaneSprite.setTexture(*PlaneTexture);
		PlaneSpriteSize = sf::Vector2i(PlaneSprite.getGlobalBounds().getSize());
		PlaneSprite.setOrigin(PlaneSpriteSize.x / 2, PlaneSpriteSize.y / 2); // set the origin to the centre of the plane

		// Note that the PlaneFOV is NOT rendered under typical operation, but can be rendered by adjusting the
		// Render() method to include it. If the Player plane is in the PlanePOV rect, then it is shot at.
		PlaneFOV.setFillColor(sf::Color::Transparent); // Set-up the plane field of view rect
		PlaneFOV.setOutlineColor(sf::Color::Red); // Set the outline colour to red for easier spotting when debugging
		PlaneFOV.setOutlineThickness(3); // Set the width of the outline to 3 pixels

		Loaded = true;
	}

	PlaneRect = sf::IntRect(PlaneSprite.getGlobalBounds()); // Get the current rectangle of the plane sprite, used for collision detection

	// Update the sprite's size and get the value in pixels. Sprite scales with window size.
	PlaneSprite.setScale(sf::Vector2f(Registry::UnitWindowScale * DynamicScaling, Registry::UnitWindowScale * DynamicScaling));
	PlaneSpriteSize = sf::Vector2i(PlaneSprite.getGlobalBounds().getSize());

	PlaneFOV.setSize(sf::Vector2f(PlaneSpriteSize.x, Registry::WindowDiagonalSize)); // last param is the view distance (set to entire screen)
	PlaneFOV.setOrigin(sf::Vector2f(PlaneSpriteSize.x / 2.f, PlaneSpriteSize.y / -2.f));

	// Determine the x and y ranges for the perlin noise based on the sprite's size and window size, this ensures the sprite doesn't
	// leave the screen.
	int DisplayXPerlinRange[] = { PlaneSpriteSize.x / 2.0f, Registry::window_size.x - PlaneSpriteSize.x / 2.0f };
	int DisplayYPerlinRange[] = { PlaneSpriteSize.y / 2.0f, Registry::window_size.y - PlaneSpriteSize.y / 2.0f };

	// Calculate the position for the plane in the next frame, uses perlin noise offset to ensure all the planes don't return to same
	// point (centre of screen).
	float NewXPosition = noise.fractalBrownianMotion(PositionNoiseOffset + PlaneRunTime / 5.f, 0, DisplayXPerlinRange);
	float NewYPosition = noise.fractalBrownianMotion(0, PositionNoiseOffset + PlaneRunTime / 5.f, DisplayYPerlinRange);

	// Determine where the plane will be in the frame after the next frame, as this is used to calculate where to spawn the plane so it
		// flies in from the closest off-screen place that is directly in-line, as this ensures the plane doesn't move into position then
		// move immediately rotate and move in a different direction, planes aren't very good at sharp turns.
	float PredictedXPosition = noise.fractalBrownianMotion(PositionNoiseOffset + ((PlaneRunTime + Registry::DisplayRefreshTime) / 5.f) * 1.25f, 0, DisplayXPerlinRange);
	float PredictedYPosition = noise.fractalBrownianMotion(0, PositionNoiseOffset + ((PlaneRunTime + Registry::DisplayRefreshTime) / 5.f) * 1.25f, DisplayYPerlinRange);

	if (!DefaultPositionSet) { // If the plane is being spawned for the first time, set the default position. (This code runs only once)
		DefaultPositionSet = true;

		PlaneStartSpawnTime = Registry::GameRunTime; // Determine at what time relative to the start of the game the plane is spawning at

		sf::Vector2f currentPosition = sf::Vector2f(NewXPosition, NewYPosition); // Get the AI 'current position' of the plane
		PlaneAIMovementStartingPosition = sf::Vector2i(currentPosition);

		sf::Vector2f predictedPosition(PredictedXPosition, PredictedYPosition);

		// Determine the spawn position for the plane based on the angle between the AI positions for the first two frames of AI motion.
		sf::Vector2f direction = predictedPosition - currentPosition;
		float magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
		sf::Vector2f unitDirection = direction / magnitude;

		// If moving more in the horizontal axis, spawn on the left/right sides of the screen
		if (abs(unitDirection.x) > abs(unitDirection.y)) {
			float NearestWindow_X_Axis = 0;
			if (currentPosition.x > Registry::window_size.x / 2.f) {
				NearestWindow_X_Axis = Registry::window_size.x;
			}

			float X_AxisSpawningOffset = (NearestWindow_X_Axis - currentPosition.x) / unitDirection.x;
			sf::Vector2f spawnPosition = currentPosition + unitDirection * X_AxisSpawningOffset;

			PlaneSpawningPosition = sf::Vector2i(spawnPosition);
		}
		else { // otherwise spawn on the top/bottom sides of the screen otherwise the plane spawns really far away in the x axis.
			float NearestWindow_Y_Axis = 0;
			if (currentPosition.y > Registry::window_size.y / 2.f) {
				NearestWindow_Y_Axis = Registry::window_size.y;
			}

			float Y_AxisSpawningOffset = (NearestWindow_Y_Axis - currentPosition.y) / unitDirection.y;
			sf::Vector2f spawnPosition = currentPosition + unitDirection * Y_AxisSpawningOffset;

			PlaneSpawningPosition = sf::Vector2i(spawnPosition);
		}

		// Account for the player's size in the spawning position, to ensure the plane isn't visible.
		if (PlaneSpawningPosition.x <= 0) {
			PlaneSpawningPosition.x -= PlaneSpriteSize.x / 2.0f;
		}
		else {
			PlaneSpawningPosition.x += PlaneSpriteSize.x / 2.0f;
		}

		if (PlaneSpawningPosition.y <= 0) {
			PlaneSpawningPosition.y -= PlaneSpriteSize.y / 2.0f;
		}
		else {
			PlaneSpawningPosition.y += PlaneSpriteSize.y / 2.0f;
		}

		// Determine the direction the plane in order to point it in the direction of travel
		sf::Vector2f Displacement = sf::Vector2f(PlaneAIMovementStartingPosition) - sf::Vector2f(PlaneSpawningPosition);
		NewRotation = RotationFromDisplacement(Displacement.x, Displacement.y);
		PlaneSprite.setRotation(NewRotation);
		PlaneFOV.setRotation(NewRotation);
		CharacterRotation = NewRotation;
	}

	if (InPosition) { // If the plane is now controlled by the AI
		CharacterPosition = sf::Vector2i(NewXPosition, NewYPosition); // Set the position based on the Perlin noise positions generated earlier

		float XDisplacement = PredictedXPosition - NewXPosition; // Determine the displacement in the x and y axis
		float YDisplacement = PredictedYPosition - NewYPosition;
		if (XDisplacement != 0 && YDisplacement != 0) { // if the plane is moving, set the rotation of the plane
			NewRotation = RotationFromDisplacement(XDisplacement, YDisplacement);
			PlaneSprite.setRotation(NewRotation);
			PlaneFOV.setRotation(NewRotation);
			CharacterRotation = NewRotation;
		}
		PlaneRunTime += Registry::DisplayRefreshTime; // Update the plane run time, which is used to control the position of the plane.
	}
	else { // if the AI is not controlling the plane, use the linear interpolation algorithm to move the plane into position.
		CharacterPosition = sf::Vector2i(InterpolateBetweenVectors(sf::Vector2f(PlaneSpawningPosition), sf::Vector2f(PlaneAIMovementStartingPosition), Registry::GameRunTime - PlaneStartSpawnTime, MoveInTime));
		if (Registry::GameRunTime > PlaneStartSpawnTime + MoveInTime) {
			InPosition = true; // if the plane has moved into position, set the AI to control it.
		}
	}

	// Set the position for both the sprite and the field of view so the plane and FOV are correctly positioned.
	PlaneSprite.setPosition(sf::Vector2f(CharacterPosition));
	PlaneFOV.setPosition(sf::Vector2f(CharacterPosition));
}

void EnemyOnePlane::Render() {
	Registry::window->draw(PlaneSprite);
	//Registry::window->draw(PlaneFOV); // use to view FOV.
}

EnemyOnePlane::~EnemyOnePlane() { // properly garbage collect the pointers to the associated texture - note the texture is not destroyed
	// here as other planes may be using it.
	Loaded = false;
	PlaneTexture = nullptr;
	Registry::TextureManager->GarbageCollect(Constants::ENEMY_ONE_TEXTURE);
}

void EnemyOnePlane::FireBullets(vector<unique_ptr<Bullet>>& Bullets) {
	// Determine the firing frequency.
	int LevelNumber = 0;
	if (Registry::MenuStack[0] == Constants::LEVEL_TWO) {
		LevelNumber = 1;
	}

	// if the plane is ready to fire and is not considered 'below' the player
	if (Registry::GameRunTime > BulletLastFiredTime + Constants::EMEMY_ONE_BULLET_FIRE_COOLDOWN[LevelNumber] && DynamicScaling == 0.5f) {
		Registry::SFXManager->PlayOneshot(Constants::BULLET_FIRE_SFX); // play the bullet fire sound effect

		pair<sf::Vector2i, sf::Vector2i> BulletInitialPositions = CalculateBulletPositions(CharacterPosition, 25, 15, CharacterRotation);
		// Calculate the initial positions of the bullets based on the plane's position and rotation.

		float AngleRadians = (CharacterRotation + 90) * (Constants::PI / 180.f); // calculate the angle in radians the bullet is firing at

		sf::Vector2f Angle = sf::Vector2f(cos(AngleRadians), sin(AngleRadians)); // convert this into a unit vector that forms the angle for the
		// bullet to fire at.

		// fire the first bullet
		Bullets.emplace_back(make_unique<Bullet>());
		Bullets.back()->SetPosition(BulletInitialPositions.first);
		Bullets.back()->SetFiringAngle(Angle);
		Bullets.back()->SetShotFrom(Constants::ENEMY);

		// fire the second bullet
		Bullets.emplace_back(make_unique<Bullet>());
		Bullets.back()->SetPosition(BulletInitialPositions.second);
		Bullets.back()->SetFiringAngle(Angle);
		Bullets.back()->SetShotFrom(Constants::ENEMY);

		BulletLastFiredTime = Registry::GameRunTime; // update the last time the plane fired a bullet so the plane doesn't fire too quickly.
	}
}