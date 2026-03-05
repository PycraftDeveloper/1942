#pragma once
#pragma warning( disable : 4244)

#include <string>

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
	const int SCALE = 3; // The pixel size for all generated textures
	const int THIRD_WINDOW_DEFAULT_X_SIZE = 120; // Used to determine the scale for the scrolling background
	const int THIRD_WINDOW_DEFAULT_Y_SIZE = 214;
	const int TEXT_PADDING = 10; // Number of pixels to space some text apart by.
	const int LEVEL_ONE_RUN_TIME = 2 * 60; // Number of seconds each level lasts.
	const int LEVEL_TWO_RUN_TIME = 2.5f * 60;

	const int NORMALIZED_PERLIN_NOISE_RANGE[] = { -1, 1 }; // The default Perlin noise range to normalise to.

	// Used in determining where the power up plane is relative to the player
	const string LEFT = "left";
	const string RIGHT = "right";

	// Stores the names for the textures on the disk.
	const string MAIN_CHARACTER_TEXTURE = "main character texture";
	const string BULLET_TEXTURE = "bullet texture";
	const string ENEMY_ONE_TEXTURE = "enemy one texture";
	const string ENEMY_TWO_TEXTURE = "enemy two texture";
	const string POWER_UP_PLANE_TEXTURE = "power up texture";

	// Stores the names for each of the levels
	const string MAIN_MENU = "main menu";
	const string END_MENU = "end menu";
	const string PAUSE_MENU = "pause menu";
	const string LEVEL_ONE = "level one";
	const string LEVEL_TWO = "level two";
	const string OPTIONS_MENU = "options menu";
	const string CREDITS_MENU = "credits menu";

	const string QUIT = "quit";

	// Stores the different entity categorisations for the game, used to determine how a bullet's interaction.
	const string PLAYER = "player";
	const string ENEMY = "enemy";

	// Stores the file name for the bullet shot sound effect.
	const string BULLET_FIRE_SFX = "569847__danlucaz__gun-shot-1.ogg";

	const int PLAYER_MOVEMENT_SPEED = 375; // Stores the number of pixels the player moves. Uses time delta to ensure same movement
	// speed for all frame rates
	const int BULLET_SPEED = 525; // Stores the bullet's speed in pixels per second, similar to the value stored above.
	const int ENEMY_PLANE_TWO_SPEED = 175; // Stores how many pixels the enemy two plane moves up the frame by per second.
	const int BACKGROUND_SCROLL_SPEED = 225; // Stores how many pixels the background moves up the screen per second.

	const float PLAYER_BULLET_FIRE_COOLDOWN = 0.2f; // Determines how many seconds the player has to wait before they can fire again
	const float EMEMY_ONE_BULLET_FIRE_COOLDOWN[] = { 1.0f, 0.75f }; // Enemies fire faster on higher levels.
	const float ENEMY_TWO_BULLET_FIRE_COOLDOWN[] = { 1.0f, 0.8f };
	const float ENEMY_TWO_BLIND_SPOT_SIZE[] = { 0.707f, 0.819f }; // 0.707 = approx cos(45*), 0.819 = approx cos(35*)

	const float PI = 3.142f;

	// Determines the colours to generate for the dynamically scrolling terrain.
	const int DENSE_FOREST_COLOUR[4] = { 102, 141, 46, 255 };
	const int PLAINS_COLOUR[4] = { 128, 161, 66, 255 };
	const int SAND_COLOUR[4] = { 202, 189, 147, 255 };
	const int SHALLOW_OCEAN_COLOUR[4] = { 146, 184, 239, 255 };
	const int DEEP_OCEAN_COLOUR[4] = { 127, 157, 226, 255 };
}

#endif