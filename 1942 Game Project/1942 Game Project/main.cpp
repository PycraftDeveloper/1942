#pragma warning( disable : 4244)

#include <vector>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Utils.h"
#include "BackgroundTexture.h"
#include "AssetManager.h"
#include "SoundEffectManager.h"

#include "MainMenu.h"
#include "EndMenu.h"
#include "PauseMenu.h"
#include "LevelOne.h"
#include "LevelTwo.h"
#include "OptionsMenu.h"
#include "CreditsMenu.h"

#include "Constants.h"
#include "Registry.h"

/*
User-input guide
================

In all menus:
- The mouse pointer can be used to hover over clickable text.
- When hovering over clickable text, the mouse can be left-clicked to activate the button.
- The F11 key can be used to toggle between full screen and windowed modes.
- The Escape key can be used to return to the previous menu, or quit the game when on the main menu. When in either of the game levels the
		Escape key cab be used in addition to the on-screen buttons to pause or resume the game.

In the game levels:
- The W, A, S, D keys can be used to move the player around the screen.
- The Space key can be used to fire the player's bullets, and to also fire from any associated power up planes.

In the options menu:
- The mouse scroll wheel can be used to adjust the sound effect volume up or down.
*/

using namespace std;

int main()
{
	srand(time(0)); // generate new random seed for the game based on system time.
	sf::RenderWindow window(sf::VideoMode(1280, 720), "1942"); // 1280, 720 - adjusted from the original vertical orientation for better
	// comparability with widescreen monitors.
	Registry::window = &window; // avoids needing to pass the window around to menus exct.
	window.setVerticalSyncEnabled(true); // v-sync to avoid excessive hardware usage.
	bool fullscreen = false; // used to determine if the game is in full screen mode or not.

	GameTextureManager TextureManager; // Used to load and manage the textures used in the game.
	Registry::TextureManager = &TextureManager; // Store a reference to it centrally so it can be used by all classes.

	sf::Font GameFont;
	GameFont.loadFromFile(
		string("resources") + Registry::path_seperator +
		"fonts" + Registry::path_seperator +
		"Jersey_15" + Registry::path_seperator +
		"Jersey15-Regular.ttf"); // initialise the font used in the game, used in every menu so no need to garbage collect it or use the asset manager
	// for it
	Registry::GameFont = &GameFont; // store a reference to it centrally so it can be used by all classes.

	Background background_texture; // used to generate the background image.

	MainMenu* main_menu = nullptr; // Store each of the menus as pointers, so minimal memory is used when they are not in use.
	EndMenu* end_menu = nullptr;
	PauseMenu* pause_menu = nullptr;
	LevelOne* level_one = nullptr;
	LevelTwo* level_two = nullptr;
	OptionsMenu* options_menu = nullptr;
	CreditsMenu* credits_menu = nullptr;

	SoundEffectManager SFXManager; // used to manage the sound effects in the game.
	Registry::SFXManager = &SFXManager; // store a reference to it centrally so it can be used by all classes.

	string next_menu; // Used to store the next menu to be displayed, as each menu can return a string to indicate what it should do next.

	sf::Mouse MouseCursor; // used to get information about mouse cursor for the game

	sf::Thread* BackgroundThread = new sf::Thread(&Background::ParallelLoop, &background_texture);
	BackgroundThread->launch(); //  start the vertically scrolling background thread.

	bool Is_W_KeyPressed = false; // Used to improve how the player interacts with the game, by storing which keys are pressed.
	bool Is_A_KeyPressed = false;
	bool Is_S_KeyPressed = false;
	bool Is_D_KeyPressed = false;
	bool Is_SPACE_KeyPressed = false;

	while (window.isOpen()) // game loop starts
	{
		sf::Clock WindowRefreshRateClock;
		sf::Time StartTime = WindowRefreshRateClock.getElapsedTime(); // used to determine how long the game loop takes to run, and how long the game has been running for.
		Registry::MouseScrollWheelDelta = 0; // Reset how far the mouse has scrolled, as it is used in the options menu.

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {// allow the window to be closed.
				window.close();
			}
			if (event.type == sf::Event::MouseWheelMoved) {// Update the mouse scroll wheel delta
				Registry::MouseScrollWheelDelta = event.mouseWheel.delta;
			}
			if (event.type == sf::Event::MouseButtonPressed) {// Used to control button interactions
				Registry::MouseButtonDown = true;
			}
			if (event.type == sf::Event::MouseButtonReleased) {
				Registry::MouseButtonDown = false;
			}
			if (event.type == sf::Event::KeyReleased) {// Used in the game levels to control the resetting of movement of the player.
				switch (event.key.code) {
				case sf::Keyboard::W:
					Is_W_KeyPressed = false;
					break;
				case sf::Keyboard::S:
					Is_S_KeyPressed = false;
					break;
				case sf::Keyboard::A:
					Is_A_KeyPressed = false;
					break;
				case sf::Keyboard::D:
					Is_D_KeyPressed = false;
					break;
				case sf::Keyboard::Space:
					Is_SPACE_KeyPressed = false;
					break;
				}
			}
			if (event.type == sf::Event::KeyPressed) { // Used to control the game, and to toggle full screen mode.
				switch (event.key.code) {
				case sf::Keyboard::F11:
					window.close(); // close the window to allow it to be recreated in full screen mode, provided a 'full screen mode'
					// is available, otherwise it will just be recreated in the same mode as before.
					fullscreen = !fullscreen;
					if (fullscreen) {
						std::vector<sf::VideoMode> FullscreenVideoModes = sf::VideoMode::getFullscreenModes();

						if (FullscreenVideoModes.size() == 0) {
							cout << "No full screen video modes are available! As a first troubleshooting step, check your display drivers." << endl;
							window.create(sf::VideoMode(1280, 720), "1942");
						}
						else {
							sf::VideoMode FullscreenMode = FullscreenVideoModes[0]; // best video mode is first

							window.create(FullscreenMode, "1942", sf::Style::Fullscreen);
						}
					}
					else {
						window.create(sf::VideoMode(1280, 720), "1942");
					}
					window.setVerticalSyncEnabled(true); // reapplied v-sync to avoid excessive hardware usage.
					BackgroundThread = new sf::Thread(&Background::ParallelLoop, &background_texture);
					BackgroundThread->launch(); // reapply background thread (BUG-FIX).
					break;
				case sf::Keyboard::Escape: // Used to return to previous menu, or quit the game, or pause game
					if (Registry::MenuStack[0] == Constants::MAIN_MENU) {
						Registry::MenuStack[1] = Constants::MAIN_MENU;
						Registry::MenuStack[0] = Constants::QUIT;
					}
					else if (Registry::MenuStack[0] == Constants::LEVEL_ONE || Registry::MenuStack[0] == Constants::LEVEL_TWO) {
						Registry::MenuStack[1] = Registry::MenuStack[0];
						Registry::MenuStack[0] = Constants::PAUSE_MENU;
					}
					else if (Registry::MenuStack[0] == Constants::PAUSE_MENU) {
						Registry::MenuStack[0] = Registry::MenuStack[1];
						Registry::MenuStack[1] = Constants::PAUSE_MENU;
					}
					else if (Registry::MenuStack[0] == Constants::END_MENU) {
						Registry::MenuStack[0] = Constants::MAIN_MENU;
						Registry::MenuStack[1] = Constants::END_MENU;
					}
					else if (Registry::MenuStack[0] == Constants::OPTIONS_MENU) {
						Registry::MenuStack[0] = Constants::MAIN_MENU;
						Registry::MenuStack[1] = Constants::OPTIONS_MENU;
					}
					else if (Registry::MenuStack[0] == Constants::CREDITS_MENU) {
						Registry::MenuStack[0] = Constants::MAIN_MENU;
						Registry::MenuStack[1] = Constants::CREDITS_MENU;
					}
					break;
				case sf::Keyboard::W:
					Is_W_KeyPressed = true;
					break;
				case sf::Keyboard::S:
					Is_S_KeyPressed = true;
					break;
				case sf::Keyboard::A:
					Is_A_KeyPressed = true;
					break;
				case sf::Keyboard::D:
					Is_D_KeyPressed = true;
					break;
				case sf::Keyboard::Space: // Used to determine when to fire the player's bullets.
					Is_SPACE_KeyPressed = true;
					break;
				}
			}
		}

		if (Registry::MenuStack[0] == Constants::LEVEL_ONE && level_one != nullptr) { // Used to control the player in level one (most likely first)
			if (Is_W_KeyPressed && Is_S_KeyPressed) {
				level_one->Protagonist.Movement[1] = 0;
			}
			else if (Is_W_KeyPressed) {
				level_one->Protagonist.Movement[1] = -1;
			}
			else if (Is_S_KeyPressed) {
				level_one->Protagonist.Movement[1] = 1;
			}
			else {
				level_one->Protagonist.Movement[1] = 0;
			}

			if (Is_A_KeyPressed && Is_D_KeyPressed) {
				level_one->Protagonist.Movement[0] = 0;
			}
			else if (Is_A_KeyPressed) {
				level_one->Protagonist.Movement[0] = -1;
			}
			else if (Is_D_KeyPressed) {
				level_one->Protagonist.Movement[0] = 1;
			}
			else {
				level_one->Protagonist.Movement[0] = 0;
			}
		}
		else if (Registry::MenuStack[0] == Constants::LEVEL_TWO && level_two != nullptr) { // Used to control the player in level two.
			if (Is_W_KeyPressed && Is_S_KeyPressed) {
				level_two->Protagonist.Movement[1] = 0;
			}
			else if (Is_W_KeyPressed) {
				level_two->Protagonist.Movement[1] = -1;
			}
			else if (Is_S_KeyPressed) {
				level_two->Protagonist.Movement[1] = 1;
			}
			else {
				level_two->Protagonist.Movement[1] = 0;
			}

			if (Is_A_KeyPressed && Is_D_KeyPressed) {
				level_two->Protagonist.Movement[0] = 0;
			}
			else if (Is_A_KeyPressed) {
				level_two->Protagonist.Movement[0] = -1;
			}
			else if (Is_D_KeyPressed) {
				level_two->Protagonist.Movement[0] = 1;
			}
			else {
				level_two->Protagonist.Movement[0] = 0;
			}
		}

		if (Is_SPACE_KeyPressed) { // Used to fire bullets in the same game level as the player.
			if (Registry::MenuStack[0] == Constants::LEVEL_ONE && level_one != nullptr) {
				if (level_one->Protagonist.FireBullets(level_one->Bullets)) {
					Registry::SFXManager->PlayOneshot(Constants::BULLET_FIRE_SFX);

					if (level_one->Left_PowerUpPlane != nullptr) {
						level_one->Left_PowerUpPlane->FireBullets(level_one->Bullets);
					}

					if (level_one->Right_PowerUpPlane != nullptr) {
						level_one->Right_PowerUpPlane->FireBullets(level_one->Bullets);
					}
				}
			}
			else if (Registry::MenuStack[0] == Constants::LEVEL_TWO && level_two != nullptr) {
				if (level_two->Protagonist.FireBullets(level_two->Bullets)) {
					Registry::SFXManager->PlayOneshot(Constants::BULLET_FIRE_SFX);

					if (level_two->Left_PowerUpPlane != nullptr) {
						level_two->Left_PowerUpPlane->FireBullets(level_two->Bullets);
					}

					if (level_two->Right_PowerUpPlane != nullptr) {
						level_two->Right_PowerUpPlane->FireBullets(level_two->Bullets);
					}
				}
			}
		}

		Registry::MousePosition = MouseCursor.getPosition(window);

		HandleDisplaySizeChanged(background_texture); // Used to adjust the way the game looks when the window is resized.

		window.clear();

		background_texture.Render(); // draw the background first so it appears at back of scene

		if (Registry::MenuStack[0] == Constants::MAIN_MENU) { // Used to display the main menu
			if (main_menu == nullptr) {
				main_menu = new MainMenu(); // if the main menu is not already created, create it.
			}
			next_menu = main_menu->RunMenu(); // run the main menu and get which menu to go to next
			if (next_menu != Constants::MAIN_MENU) { // if moving to a new menu
				Registry::MenuStack[1] = Constants::MAIN_MENU;
				Registry::MenuStack[0] = next_menu;
				// reset the game state
				Registry::PlayerScore = 0;
				Registry::GiveUp = false;

				// garbage collect the main menu
				delete main_menu;
				main_menu = nullptr;
			}
		}
		else if (Registry::MenuStack[0] == Constants::END_MENU) { // Used to display the end menu
			if (end_menu == nullptr) {
				end_menu = new EndMenu();
			}
			next_menu = end_menu->RunMenu();
			if (next_menu != Constants::END_MENU) {
				Registry::MenuStack[1] = Constants::END_MENU;
				Registry::MenuStack[0] = next_menu;
				// also reset the game state here, as could be going back to level one
				Registry::PlayerScore = 0;
				Registry::GiveUp = false;

				delete end_menu;
				end_menu = nullptr;
			}
		}
		else if (Registry::MenuStack[0] == Constants::PAUSE_MENU) { // Used to display the pause menu
			if (pause_menu == nullptr) {
				pause_menu = new PauseMenu();
			}
			next_menu = pause_menu->RunMenu();
			if (next_menu != Constants::PAUSE_MENU) {
				Registry::MenuStack[1] = Constants::PAUSE_MENU;
				Registry::MenuStack[0] = next_menu;

				// don't reset the game

				delete pause_menu;
				pause_menu = nullptr;

				// if moving away from the pause menu, reset the game state if not returning to the game level
				if (next_menu != Constants::LEVEL_ONE && level_one != nullptr) {
					delete level_one;
					level_one = nullptr;
				}
				else if (next_menu != Constants::LEVEL_TWO && level_two != nullptr) {
					delete level_two;
					level_two = nullptr;
				}
			}
		}
		else if (Registry::MenuStack[0] == Constants::LEVEL_ONE) { // Used to display the first level of the game
			if (level_one == nullptr) {
				level_one = new LevelOne();
			}
			next_menu = level_one->RunMenu();
			if (next_menu != Constants::LEVEL_ONE) {
				Registry::MenuStack[1] = Constants::LEVEL_ONE;
				Registry::MenuStack[0] = next_menu;

				if (next_menu != Constants::PAUSE_MENU) { // if not going to the pause menu, garbage collect level one
					delete level_one;
					level_one = nullptr;
				}
			}
		}
		else if (Registry::MenuStack[0] == Constants::LEVEL_TWO) { // Used to display the second level of the game
			if (level_two == nullptr) {
				level_two = new LevelTwo();
			}
			next_menu = level_two->RunMenu();
			if (next_menu != Constants::LEVEL_TWO) {
				Registry::MenuStack[1] = Constants::LEVEL_TWO;
				Registry::MenuStack[0] = next_menu;

				if (next_menu != Constants::PAUSE_MENU) { // if not going to the pause menu, garbage collect level two
					delete level_two;
					level_two = nullptr;
				}
			}
		}
		else if (Registry::MenuStack[0] == Constants::OPTIONS_MENU) { // Used to display the options menu
			if (options_menu == nullptr) {
				options_menu = new OptionsMenu();
			}
			next_menu = options_menu->RunMenu();
			if (next_menu != Constants::OPTIONS_MENU) {
				Registry::MenuStack[1] = Constants::OPTIONS_MENU;
				Registry::MenuStack[0] = next_menu;

				delete options_menu;
				options_menu = nullptr;
			}
		}
		else if (Registry::MenuStack[0] == Constants::CREDITS_MENU) { // Used to display the credits menu
			if (credits_menu == nullptr) {
				credits_menu = new CreditsMenu();
			}
			next_menu = credits_menu->RunMenu();
			if (next_menu != Constants::CREDITS_MENU) {
				Registry::MenuStack[1] = Constants::CREDITS_MENU;
				Registry::MenuStack[0] = next_menu;

				delete credits_menu;
				credits_menu = nullptr;
			}
		}
		else if (Registry::MenuStack[0] == Constants::QUIT) { // Used to quit the game
			window.close();
		}
		else { // This code should NEVER BE REACHED and was used for debug purposes.
			cout << Registry::MenuStack[0] << " is not a recognised menu, resetting..." << endl;
			Registry::MenuStack[0] = Constants::MAIN_MENU;
			Registry::MenuStack[1] = Constants::MAIN_MENU;
		}

		SFXManager.GarbageCollectSoundEffects(); // clean up any sound effects that have finished playing

		window.display(); // update the contents of the window

		sf::Time EndTime = WindowRefreshRateClock.getElapsedTime();
		Registry::DisplayRefreshTime = EndTime.asSeconds() - StartTime.asSeconds(); // determine how long the game loop took to run (used for time scaling)
		Registry::DisplayRefreshRate = 1.0f / Registry::DisplayRefreshTime; // determine the refresh rate of the game

		Registry::GameRunTime += Registry::DisplayRefreshTime; // update game runtime
	}

	BackgroundThread->wait(); // Ensure background thread has stopped before exiting.

	return 0;
}