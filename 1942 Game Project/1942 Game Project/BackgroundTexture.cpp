#include <SFML/Graphics.hpp>
#include <iostream>

#include "BackgroundTexture.h"

#include "Utils.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

Background::Background() {
	pixels = new sf::Uint8[background_dimensions[0] * background_dimensions[1] * 4]; // Set-up the background pixel array.

	Create(); // generates data for the whole image and applies it to a texture - note in compute we only update portions, not the whole thing.
	background_sprite.setTexture(background_texture);
	background_sprite.setScale(
		static_cast<float>(Registry::window_size.x) / background_dimensions[0],
		static_cast<float>(Registry::window_size.y) / background_dimensions[1]
	); // Scale the background image. It generates at 1/3rd the resolution for more pixel art design.
}

void Background::Create() {
	float scale_x = Constants::THIRD_WINDOW_DEFAULT_X_SIZE / (75.0f * background_dimensions[0]); // identify the scale factors for the
	// x and y dimensions
	float scale_y = Constants::THIRD_WINDOW_DEFAULT_Y_SIZE / (75.0f * background_dimensions[1]);
	float y_offset = -vertical_offset / 75.0f; // The vertical offset is used to allow the background to scroll vertically.
	float aspect_ratio_inv = 1.0f / (Registry::aspect_ratio * 2.0f); // Used to adjust the generation to adapt to the display's aspect ratio.

	for (int x = 0; x < background_dimensions[0]; ++x) { // Generate the x axis pixels
		float x_value = x * scale_x;

		for (int y = 0; y < background_dimensions[1]; ++y) { // generate the y axis pixels
			int index = (y * background_dimensions[0] + x) * 4;

			float y_value = y * scale_y + y_offset;

			float noise_value = noise.fractalBrownianMotion(x_value, y_value * aspect_ratio_inv, NormalisedPerlinNoiseRange); // generate Perlin noise value

			const int* pixel_color_array = generate_color_from_value(noise_value); // Get the associated colour for that value
			std::copy_n(pixel_color_array, 4, pixels + index); // apply that colour to the texture.
		}
	}
	background_texture = sf::Texture(); // Create empty texture
	background_texture.create(background_dimensions[0], background_dimensions[1]); // with the background dimension as the size
	background_texture.update(pixels); // fill it with the pixel data just generated
}

void Background::OnWindowSizeChanged() { // When the window size changes, the background needs to be recreated as the number of pixels to calculate
	// has changed.
	background_dimensions[0] = static_cast<int>(Registry::window_size.x / Constants::SCALE) + 1;
	background_dimensions[1] = static_cast<int>(Registry::window_size.y / Constants::SCALE) + 1;
	delete[] pixels;
	pixels = new sf::Uint8[background_dimensions[0] * background_dimensions[1] * 4];
	Create();
	background_sprite.setTexture(background_texture, true);
	background_sprite.setScale(3.0f, 3.0f); // scale the background image by 3x to fit the window.
}

void Background::HandleWindowSizeChanged() {
	WindowSizeChanged = true;
}

void Background::Compute() {
	memmove(pixels + (background_dimensions[0] * 4), pixels, (background_dimensions[0] * (background_dimensions[1] - 1) * 4)); // ...
	// use direct memory manipulation commands to move the whole pixel arrangement up, destroying the top most row so it can be
	// regenerated. Doing it this way instead of using iteration is dramatically faster.

	float scale_x = Constants::THIRD_WINDOW_DEFAULT_X_SIZE / (75.0f * background_dimensions[0]);
	float x_multiplier = 1.0f * scale_x;

	float y_input = (-vertical_offset) / (Registry::aspect_ratio * 150.f);

	for (int x = 0; x < background_dimensions[0]; ++x) // Only generate a new bottom row, instead of the whole image
	{
		int index = x * 4;

		float x_value = x * x_multiplier;
		float noise_value = noise.fractalBrownianMotion(x_value, y_input, NormalisedPerlinNoiseRange);

		const int* pixel_color_array = generate_color_from_value(noise_value);

		std::copy_n(pixel_color_array, 4, pixels + index);
	}

	vertical_offset++; // Increase the vertical scrolling offset.
	background_texture.update(pixels); // update the background image texture.
}

void Background::Render() { // Render the background image to the window, using locks to avoid rendering during texture re-writes.
	TextureLock.lock();
	background_sprite.setPosition(0, 0);
	Registry::window->draw(background_sprite);
	TextureLock.unlock();
}

void Background::ParallelLoop() {
	sf::Clock LoopClock;
	sf::Time FrameTime = sf::seconds(1.0f / 60.f); // Lock to 60 fps.

	while (Registry::window->isOpen()) { // second game loop
		sf::Time LoopStart = LoopClock.getElapsedTime();

		TextureLock.lock(); // make sure to lock the texture to prevent race conditions
		if (WindowSizeChanged) {
			WindowSizeChanged = false;
			OnWindowSizeChanged();
		}

		Compute(); // generate new top row.
		TextureLock.unlock();

		sf::Time LoopEnd = LoopClock.getElapsedTime();
		sf::sleep(FrameTime - (LoopEnd - LoopStart)); // determine how long to wait before generating the new frame. Normally this
		// is handled in the operating system window updates, but this isn't the main game loop.
	}
}