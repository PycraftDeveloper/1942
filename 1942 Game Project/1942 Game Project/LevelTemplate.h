#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "TextRenderer.h"

#include "Plane_Player.h"
#include "EnemyPlaneManager.h"
#include "PowerUp.h"
#include "PowerUpPlane.h"

using namespace std;

class LevelTemplate {
	/*This class defines the default behaviour for both game levels - although it must be noted that this behaviour is effected by
	the values stored in the Registry namespace.*/
protected:
	EnemyPlaneManager EnemyPlaneManagementSystem; // This handles all the enemy plane spawning and AI.

	TextRenderer HeadsUpDisplayText; // This is used to display the UI
	TextRenderer LevelDetailText; // This is used to display which level you are on.
	TextRenderer PauseButton; // Used to create the pause button on-screen.

	PowerUp ProtagonistPowerUp; // This stores any power-up the player may have available to collect.
public:
	Player Protagonist; // This stores the player and its associated behaviours
	vector<unique_ptr<Bullet>> Bullets; // This stores a list of all the bullets that are currently in the level

	PowerUpPlane* Left_PowerUpPlane = nullptr; // This creates the pointer for the power up plane that appears to the left of the player
	PowerUpPlane* Right_PowerUpPlane = nullptr; // This creates the power up plane that appears to the right of the player
protected:
	float LevelDescriptorPosition = 0; // Used to determine how far down the screen the level descriptor is, so it can be moved down out the frame
	float LevelRunTime = 0; // Stores in seconds how long the level has been running for
	float NextWaveTime = 0; // Stores how long it has been in seconds since the last wave of enemies
	float WaveTimer = 0; // Stores how long in seconds it will be before the next wave

	int LargePlanesShotDown = 0; // Keeps track of total large planes destroyed, used in scoring
	int UIVerticalSize = 0; // Stores the size in pixels for the UI text, used to prevent level text from intersecting.

	bool ProtagonistPowerUpExists = false; // Used to determine if the power up should be rendered/computed/collidable on-screen
	bool RenderLevelDetailText = true; // Used to determine if the level descriptor should be rendered - will stop when no longer visible
public:
	LevelTemplate();

	bool Compute(); // Used to compute all the level objects, also controls the position of the level detail text, and notably also
	// the collision detection for the bullets and planes against each other.

	~LevelTemplate(); // Used to properly garbage collect the level contents
};