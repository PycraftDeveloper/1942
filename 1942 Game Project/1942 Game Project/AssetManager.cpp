#pragma warning( disable : 4715)

#include <string>
#include <vector>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "AssetManager.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

void GameTexture::Init(string input_texture_file) {
	TextureFile = string("resources") + Registry::path_seperator + "images" + Registry::path_seperator + input_texture_file; // ...
	// Store the path to the texture, so I don't need to keep specifying it.
}

void GameTexture::Load() {
	if (!loaded) { // Only load the texture if it isn't already loaded
		Texture.loadFromFile(TextureFile);
		Texture.setSmooth(false); // disable smoothing for pixel-art style
		loaded = true;
	}
};

void GameTexture::GarbageCollect() {
	delete TextureLoader;
	TextureLoader = new sf::Thread(&GameTexture::Load, this); // reset the loading thread so it can be re-loaded if needed
	if (loaded) {
		loaded = false;
		sf::Texture empty_texture;
		Texture = empty_texture;
	}
};

sf::Texture* GameTexture::GetTexture() {
	if (!loaded) {
		if (TextureLoader == nullptr) {
			TextureLoader = new sf::Thread(&GameTexture::Load, this); // Set up the thread to load the texture
			TextureLoader->launch(); // Launch the thread to load the texture
		}
		TextureLoader->wait();
		delete TextureLoader;
		TextureLoader = nullptr;
	}
	return &Texture;
}

GameTextureManager::GameTextureManager() { // Set up an instance of the GameTexture for each texture in the game.
	MainCharacterTexture.Init("Player_0.png");
	BulletTexture.Init("Bullet.png");
	EnemyOneTexture.Init("Enemy_0.png");
	EnemyTwoTexture.Init("Enemy_1.png");
	PowerUpPlaneTexture.Init("Player_Support_0.png");
	// NOTE: This does NOT automatically load all these textures immediately, they are loaded lazily when needed.
}

void GameTextureManager::AdvanceLoadHelper(GameTexture& Texture) {
	if (Texture.references == 0) {
		if (Texture.TextureLoader == nullptr) {
			Texture.TextureLoader = new sf::Thread(&GameTexture::Load, &Texture); // Set up the thread to load the texture
		}
		Texture.TextureLoader->launch(); // Launch the texture loading thread, if it hasn't already been launched.
	}
	Texture.references += 1; // Keep track of how many times a texture is referenced to ensure it never goes out of scope when still in use.
}

void GameTextureManager::GarbageCollectorHelper(GameTexture& Texture) {
	Texture.references -= 1; // keep updating the references to the texture
	if (Texture.references <= 0) { // if the texture is no longer used it can be garbage collected to free up space.
		Texture.references = 0;
		Texture.GarbageCollect();
	}
}

void GameTextureManager::AdvanceLoad(string texture_name) { // Used to redirect the advance loading to the necessary GameTexture
	// in the helper class. Note for all subsequent classes, the texture name refers only to its identifier in code, not its file name.
	if (texture_name == Constants::MAIN_CHARACTER_TEXTURE) {
		AdvanceLoadHelper(MainCharacterTexture);
	}
	else if (texture_name == Constants::BULLET_TEXTURE) {
		AdvanceLoadHelper(BulletTexture);
	}
	else if (texture_name == Constants::ENEMY_ONE_TEXTURE) {
		AdvanceLoadHelper(EnemyOneTexture);
	}
	else if (texture_name == Constants::ENEMY_TWO_TEXTURE) {
		AdvanceLoadHelper(EnemyTwoTexture);
	}
	else {
		AdvanceLoadHelper(PowerUpPlaneTexture);
	}
}

void GameTextureManager::GarbageCollect(string texture_name) { // Used to redirect the garbage collection to the necessary GameTexture
	// in the helper class
	if (texture_name == Constants::MAIN_CHARACTER_TEXTURE) {
		GarbageCollectorHelper(MainCharacterTexture);
	}
	else if (texture_name == Constants::BULLET_TEXTURE) {
		GarbageCollectorHelper(BulletTexture);
	}
	else if (texture_name == Constants::ENEMY_ONE_TEXTURE) {
		GarbageCollectorHelper(EnemyOneTexture);
	}
	else if (texture_name == Constants::ENEMY_TWO_TEXTURE) {
		GarbageCollectorHelper(EnemyTwoTexture);
	}
	else {
		GarbageCollectorHelper(PowerUpPlaneTexture);
	}
}

sf::Texture* GameTextureManager::GetTexture(string texture_name) { // Used to return a reference to the loaded texture.
	if (texture_name == Constants::MAIN_CHARACTER_TEXTURE) {
		return MainCharacterTexture.GetTexture();
	}
	else if (texture_name == Constants::BULLET_TEXTURE) {
		return BulletTexture.GetTexture();
	}
	else if (texture_name == Constants::ENEMY_ONE_TEXTURE) {
		return EnemyOneTexture.GetTexture();
	}
	else if (texture_name == Constants::ENEMY_TWO_TEXTURE) {
		return EnemyTwoTexture.GetTexture();
	}
	else {
		return PowerUpPlaneTexture.GetTexture();
	}
}