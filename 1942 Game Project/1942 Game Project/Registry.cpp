#include <string>
#include <filesystem>

#include <SFML/Graphics.hpp>

#include "AssetManager.h"
#include "SoundEffectManager.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

namespace Registry {
	sf::RenderWindow* window = nullptr; // store a reference to the window so I don't need to pass it into every class
	sf::Vector2i window_size = { 360, 640 };
	sf::Vector2f window_scale = { 1.f, 1.f }; // store the window scale so the content on the window can be scaled with screen size
	float aspect_ratio = 360.f / 640.f;
	char path_seperator = filesystem::path::preferred_separator; // get the operating system path separator - should be cross comparable
	// with all operating systems.
	GameTextureManager* TextureManager = nullptr; // Store a reference to the texture manager so, like the window, I don't need to pass it into every class
	string MenuStack[2] = { Constants::MAIN_MENU, Constants::MAIN_MENU }; // Store the current and previous menu for improved menu navigation.
	sf::Font* GameFont = nullptr; // Store a reference to the game font used in every level. It is loaded once on start and remains loaded for the entire game.
	int TextPadding = Constants::TEXT_PADDING; // Store how many pixels to space apart text, is scaled with screen size.
	bool MouseButtonDown = false; // used to determine if the player is clicking the mouse - used in button interactions.
	sf::Vector2i MousePosition = { 0, 0 }; // Stores the mouse position, used to determine when interacting with buttons.
	float UnitWindowScale = 1.f; // Store the smallest scaling for the window, effectively `min(window_scale)`
	int DisplayRefreshRate = 60; // Store the refresh rate (in Hz) for the game in the PREVIOUS frame
	float DisplayRefreshTime = 1.0f / 60.0f; // Store the refresh time (in seconds) for the game in the PREVIOUS frame.
	SoundEffectManager* SFXManager; // Store a reference to the sound effect manager, used to play one-shot sound effects throughout the game.
	float GameRunTime = 0; // Store how long the game has been running for.
	int PlayerScore = 0; // keep track of the player's score so it can be accessed and displayed on multiple levels
	int WindowDiagonalSize = 735; // Store the Pythagorean distance between the top-left and bottom-left points of the screen, used to determine enemy view distances
	bool GiveUp = false; // Stores if the player has made it through both levels, in which case the 'give up' text is displayed on the end menu.
	int SFXVolume = 75; // Stores the current sound effect volume.
	int MouseScrollWheelDelta = 0; // Stores how far the scroll wheel has been moved since the last frame, used in the options manager.
	sf::IntRect WindowViewportRect = sf::IntRect(0, 0, 360, 640); // Store the window viewport as a rect, used in determining when objects are off-screen for garbage collection.
}