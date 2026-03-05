#pragma once
#pragma warning( disable : 4305)

#include <SFML/Graphics.hpp>

#include "PerlinNoise.h"

#include "Registry.h"
#include "Constants.h"

class Background {
	/*This class is used to calculate and display the vertically scrolling background for the game, and also has full support for
	parallel execution through the 'ParallelExecution' method (with rendering handled in the main thread). Note that to achieve
	the pixel-art aesthetic, the background is rendered at 1/3rd the size, then the sprite is scaled to fit the window.*/
private:
	sf::Uint8* pixels; // Stores the per-pixel data for the background image.
	sf::Texture background_texture; // Used to store the background image as a texture for rendering with SFML.
	sf::Sprite background_sprite; // Used to render and scale the background image.
	PerlinNoise noise = PerlinNoise(rand() % 512, 4, 0.4f); // Used to generate the noise the background is generated from. Octaves: 4, Persistence: 0.4f.

	sf::Mutex TextureLock; // Used to lock the texture, so it can't be rendered by the main loop whilst it is being updated.

	int background_dimensions[2] = {
		static_cast<int>(Registry::window_size.x / Constants::SCALE) + 1,
		static_cast<int>(Registry::window_size.y / Constants::SCALE) + 1 }; // Used to store the size of the background image
	// (in pixels), note the '+1' to fix a bug.

	int vertical_offset = 0; // Used to control the vertical displacement of the background noise - effectively adding in the 'scroll'

	int NormalisedPerlinNoiseRange[2] = { -1, 1 }; // Used to normalise the Perlin noise values.

	bool WindowSizeChanged = false; // Used to determine when the window size has changed (as the background size also needs to change)

	void Create(); // Used to create (and subsequently re-create when window size changes) the full background image.

	void OnWindowSizeChanged(); // Used to update the size of the background when the window size changes.

	void Compute(); // Used to add a SINGLE ROW to the top of the blurred background texture, so the whole image isn't being\
								// re-written every frame (as an optimisation).
public:
	Background();

	void HandleWindowSizeChanged(); // Acts as an event called externally when the window size changes.

	void Render(); // Used to display the scrolling background to the window in the main thread.

	void ParallelLoop(); // Added as an optimisation, allows the background image to be updated and calculated in the background
	// between frames, massively improving performance by using threads.
};