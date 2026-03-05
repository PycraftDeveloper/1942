#pragma warning( disable : 4244)
#pragma warning( disable : 4267)

#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>

#include "Utils.h"

#include "LevelTemplate.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

LevelTemplate::LevelTemplate() {
	PauseButton.SetTextIsInteractable(true);
}

bool LevelTemplate::Compute() {
	Protagonist.Compute(Left_PowerUpPlane, Right_PowerUpPlane);

	BulletsManager(Bullets); // determine which bullets can be garbage collected from the scene

	for (int i = 0; i < Bullets.size(); i++) { // compute and render the remaining bullets
		Bullets[i]->Compute();
		Bullets[i]->Render();
	}

	if (ProtagonistPowerUpExists) { // If the player's power up exists on screen to be collected
		if (ProtagonistPowerUp.Compute()) { // check if the player's power up is still on-screen
			ProtagonistPowerUpExists = false; // if not then discard it
		}
		ProtagonistPowerUp.Render();

		if (ProtagonistPowerUp.PowerUpGloablBounds.intersects(Protagonist.SpriteRect)) { // check if player has attempted to collect power up
			ProtagonistPowerUpExists = false; // then discard it

			if (Left_PowerUpPlane == nullptr) {
				Left_PowerUpPlane = new PowerUpPlane(Constants::LEFT); // Spawn new left power up plane
			}
			else if (Right_PowerUpPlane == nullptr) {
				Right_PowerUpPlane = new PowerUpPlane(Constants::RIGHT); // spawn new right power up plane
			}
			else {
				Protagonist.RemainingLives++; // if both planes are already spawned, then give the player an extra life
			}
		}
	}

	if (Left_PowerUpPlane != nullptr) { // if the power up planes exist then compute and render the left one
		Left_PowerUpPlane->Compute(sf::Vector2i(Protagonist.Position), sf::Vector2i(Protagonist.SpriteSize));
		Left_PowerUpPlane->Render();
	}

	Protagonist.Render(); // render the player.

	if (Right_PowerUpPlane != nullptr) { // if the power up planes exist then compute and render the right one
		Right_PowerUpPlane->Compute(sf::Vector2i(Protagonist.Position), sf::Vector2i(Protagonist.SpriteSize));
		Right_PowerUpPlane->Render();
	}

	EnemyPlaneManagementSystem.Compute(); // compute and render all the enemy planes and determine if more are to be spawned

	EnemyPlaneManagementSystem.Render();

	// Render the UI on top
	HeadsUpDisplayText.Render(
		"Your score: " + to_string(Registry::PlayerScore) + "\n" \
		"Remaining Lives: " + to_string(Protagonist.RemainingLives),
		TextSizeScaler(30), sf::Color::Black, sf::Vector2i(0, 0)); // render the UI text at the top left of the screen

	PauseButton.Render("<Pause>", TextSizeScaler(30), sf::Color::Black, sf::Vector2i(0, HeadsUpDisplayText.GetSize().y + Constants::TEXT_PADDING)); //...
	// render the pause button at the top left of the screen, below the UI text

	UIVerticalSize = 0; // This is normally before rendering, but not used here for that purpose so can be afterwards for improved
	// visual fidelity. See LevelN.cpp for application
	UIVerticalSize += HeadsUpDisplayText.GetSize().y + Registry::TextPadding;
	UIVerticalSize += PauseButton.GetSize().y + Registry::TextPadding;

	// Check if bullets have hit something in the scene
	for (int i = Bullets.size() - 1; i >= 0; i--) { // iterate backwards over the bullets to check for collisions, and for one-loop
		// garbage collection, also looping backwards is faster.
		Bullet& FiredBullet = *Bullets[i]; // Get a reference to the bullet to avoid getting it multiple times
		bool BulletHitTarget = false; // Determine if the bullet needs to be garbage collected

		if (FiredBullet.ShotFrom == Constants::PLAYER) { // if its the player's bullet (so player can't hit its self)
			for (int j = EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.size() - 1; j >= 0; j--) { // check every enemy plane
				EnemyOnePlane& TierOneEnemyPlane = *EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes[j];
				if (TierOneEnemyPlane.DynamicScaling < 0.375) { // ensure the enemy one plane is not in its 'below' state.
					continue;
				}

				if (FiredBullet.BulletSpriteRect.intersects(TierOneEnemyPlane.PlaneRect)) { // if collision occurs
					BulletHitTarget = true; // signal the bullet to be garbage collected
					Registry::PlayerScore += 50; // award the player some points
					EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.erase(EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.begin() + j); // garbage collect the enemy plane
					break; // stop looping - bullet no longer exists and cannot hit other planes
				}
			}

			if (!BulletHitTarget && EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane != nullptr) { // check if the bullet has hit the larger enemy plane
				if (FiredBullet.BulletSpriteRect.intersects(EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->PlaneRect)) {
					BulletHitTarget = true;
					EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->Health--; // can take multiple hits...
					Registry::PlayerScore += 100; // award points for each hit
					if (EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->Health <= 0) { // if enemy plane runs out of health
						LargePlanesShotDown++; // calculate score
						int CumulativeScore = 2000 + LargePlanesShotDown * 500;
						if (CumulativeScore > 9000) {
							CumulativeScore = 9000;
						}
						Registry::PlayerScore += CumulativeScore;

						ProtagonistPowerUp = PowerUp(); // display power up text for player to collect
						ProtagonistPowerUp.Initialise(sf::Vector2i(
							static_cast<int>(EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->CharacterPosition.x),
							static_cast<int>(EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->CharacterPosition.y)));
						ProtagonistPowerUpExists = true;

						delete EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane; // garbage collect the enemy plane
						EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane = nullptr;
					}
				}
			}
		}
		else { // if the bullet was fired by the enemy
			if (FiredBullet.BulletSpriteRect.intersects(Protagonist.SpriteRect)) { // if it hits the player
				BulletHitTarget = true;
				Protagonist.RemainingLives--; // remove one of the player's lives
				if (Protagonist.RemainingLives < 0) {
					return true; // if death, signal to go to end menu
				}
			}
			else if (Left_PowerUpPlane != nullptr && FiredBullet.BulletSpriteRect.intersects(Left_PowerUpPlane->SpriteRect)) { // if hit left power up plane
				// delete that instead of player
				delete Left_PowerUpPlane;
				Left_PowerUpPlane = nullptr;
				BulletHitTarget = true;
			}
			else if (Right_PowerUpPlane != nullptr && FiredBullet.BulletSpriteRect.intersects(Right_PowerUpPlane->SpriteRect)) {// if hit right power up plane
				// delete that instead of player
				delete Right_PowerUpPlane;
				Right_PowerUpPlane = nullptr;
				BulletHitTarget = true;
			}
		}

		if (BulletHitTarget) { // if the bullet has hit something, then it needs to be garbage collected
			Bullets.erase(Bullets.begin() + i);
		}
	}

	// now check if any objects have collided
	for (int j = EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.size() - 1; j >= 0; j--) { // check every small enemy plane
		EnemyOnePlane& TierOneEnemyPlane = *EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes[j]; // store a reference for easier access
		if (TierOneEnemyPlane.DynamicScaling >= 0.375) { // if the plane is not in its 'below' state
			if (Protagonist.SpriteRect.intersects(TierOneEnemyPlane.PlaneRect)) { // if hits the player (kamikaze
				Registry::PlayerScore += 100; // more points for the self-sacrifice.
				EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.erase(EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.begin() + j); // destroy enemy plane
				Protagonist.RemainingLives--; // take one of the player's lives
				if (Protagonist.RemainingLives < 0) {
					return true; // if player runs out of health, signal to go to end menu
				}
			}
			// same as easier, but instead of bullets check for enemy direct collision
			else if (Left_PowerUpPlane != nullptr && Left_PowerUpPlane->SpriteRect.intersects(TierOneEnemyPlane.PlaneRect)) {
				delete Left_PowerUpPlane;
				Left_PowerUpPlane = nullptr;
				Registry::PlayerScore += 75; // more points for the self-sacrifice.
				EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.erase(EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.begin() + j);
			}
			else if (Right_PowerUpPlane != nullptr && Right_PowerUpPlane->SpriteRect.intersects(TierOneEnemyPlane.PlaneRect)) {
				delete Right_PowerUpPlane;
				Right_PowerUpPlane = nullptr;
				Registry::PlayerScore += 75; // more points for the self-sacrifice.
				EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.erase(EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes.begin() + j);
			}
			else if (SeperateAxisIntersection(EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes[j]->PlaneFOV, Protagonist)) { // if there have been no
				// collisions, check if the enemy plane is in the player's field of view
				EnemyPlaneManagementSystem.SpawnedEnemyOnePlanes[j]->FireBullets(Bullets); // if player in field of view, fire bullets at it
			}
		}
	}

	if (EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane != nullptr) { // if the larger enemy plane exists, check if it has hit the player
		if (Protagonist.SpriteRect.intersects(EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->PlaneRect)) {
			delete EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane; // if so, immediately destroy this plane too, no health system here
			EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane = nullptr;
			LargePlanesShotDown++;
			int CumulativeScore = 2000 + LargePlanesShotDown * 500;
			if (CumulativeScore > 9000) {
				CumulativeScore = 9000;
			}
			Registry::PlayerScore += CumulativeScore * 1.5; // award extra points for self-sacrifice
			Protagonist.RemainingLives--;
			if (Protagonist.RemainingLives < 0) {
				return true; // if player runs out of health, signal to go to end menu
			}
		}
		else {
			EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->FireBullets(Bullets, Protagonist); // if not, check if the player is in the field of view and shoot it
		}
	}

	// Now check if power up planes have collided with large enemy planes
	if (Left_PowerUpPlane != nullptr && EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane != nullptr && Left_PowerUpPlane->SpriteRect.intersects(EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->PlaneRect)) {
		delete Left_PowerUpPlane;
		Left_PowerUpPlane = nullptr;
		delete EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane;
		EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane = nullptr;
		LargePlanesShotDown++;
		int CumulativeScore = 2000 + LargePlanesShotDown * 500;
		if (CumulativeScore > 9000) {
			CumulativeScore = 9000;
		}
		Registry::PlayerScore += CumulativeScore * 1.25;
	}

	if (Right_PowerUpPlane != nullptr && EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane != nullptr && Right_PowerUpPlane->SpriteRect.intersects(EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane->PlaneRect)) {
		delete Right_PowerUpPlane;
		Right_PowerUpPlane = nullptr;
		delete EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane;
		EnemyPlaneManagementSystem.SpawnedEnemyTwoPlane = nullptr;
		LargePlanesShotDown++;
		int CumulativeScore = 2000 + LargePlanesShotDown * 500;
		if (CumulativeScore > 9000) {
			CumulativeScore = 9000;
		}
		Registry::PlayerScore += CumulativeScore * 1.25;
	}
	// note that the large enemy plane and small enemy plane do NOT target the power up planes.

	LevelRunTime += Registry::DisplayRefreshTime; // increment the level run time by the time since the last frame

	return false; // signal that the game should remain in this level
}

LevelTemplate::~LevelTemplate() { // garbage collect the pointers created earlier to avoid memory leak.
	Bullets.clear();

	delete Left_PowerUpPlane;
	Left_PowerUpPlane = nullptr;

	delete Right_PowerUpPlane;
	Right_PowerUpPlane = nullptr;
}