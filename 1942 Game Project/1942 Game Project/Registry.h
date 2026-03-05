#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "AssetManager.h"
#include "SoundEffectManager.h"

using namespace std;

#ifndef REGISTRY_H
#define REGISTRY_H

namespace Registry {
	extern sf::RenderWindow* window; // Stores a reference to the window to avoid needing to pass it into every render function
	extern sf::Vector2i window_size; // Stores the window size in pixels
	extern sf::Vector2f window_scale; // Stores the window size relative to the original window size (360,  640}.
	extern float aspect_ratio;
	extern char path_seperator; // Stores the path separator character for this operating system.
	extern GameTextureManager* TextureManager; // Stores a pointer to the texture manager, which is used in classes to load textures in parallel.
	extern string MenuStack[2]; // Stores the name of the current and previous menu for menu navigation
	extern sf::Font* GameFont; // Stores the font used by the game. Would normally be in classes that use the font, but the font is used
	// in every menu
	extern int TextPadding; // Stores how many pixels to space apart text
	extern bool MouseButtonDown; // Used to determine if the player has clicked the mouse, and is reset to ensure the player can't
	// accidentally click the current button and any behind too
	extern sf::Vector2i MousePosition; // Used to store the mouse position, used to determine when interacting with text.
	extern float UnitWindowScale; // Stores the smallest scaling for the window, effectively `min(window_scale)`
	extern int DisplayRefreshRate; // Stores the refresh rate (in Hz) for the game in the PREVIOUS frame
	extern float DisplayRefreshTime; // Stores the refresh time (in seconds) for the game in the PREVIOUS frame.
	extern SoundEffectManager* SFXManager; // Used to play one-shot sound effects throughout the game
	extern float GameRunTime; // Stores how long the game has been running for.
	extern int PlayerScore;
	extern int WindowDiagonalSize; // Stores the Pythagorean distance between the top-left and bottom-left points of the screen.
	extern bool GiveUp; // Used to unlock the special text in the end menu when the player completes both levels
	extern int SFXVolume; // Stores the sound effect volume (as a percentage) for all sound effects.
	extern int MouseScrollWheelDelta; // Stores how far the scroll-wheel has been scrolled since the previous frame
	extern sf::IntRect WindowViewportRect; // Stores the window viewport as a rect, used in determining when objects are off-screen
	// for garbage collection.
}

#endif