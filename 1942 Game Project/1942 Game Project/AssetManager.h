#pragma once

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;

class GameTexture {
	/*This class assists with the loading of textures for the game, and there is one GameTexture associated to each unique
	texture in the game. Any multiples of the texture are all references to the texture loaded by this class.*/
private:
	// This stores the name and texture for the class. The name is used to allow the loading process to take place separate from
	// class instantiation.
	string TextureFile = "";
	sf::Texture Texture;
public:
	// The thread and mutex allow the file to be loaded in parallel, so the game doesn't need to pause whilst the slower secondary
	// storage loads the data.
	// The lock prevents any race conditions when the thread is running.
	sf::Thread* TextureLoader = nullptr;
	int references = 0; // keeps track of the number of references to the texture, so it can be freed from memory when no longer
	// in use.
	bool loaded = false; // Used to determine if the texture has been loaded into memory.

	void Init(string input_texture_file); // Acts as a replacement for the constructor for the class, but takes a texture name.

	void Load(); // Used to load the texture from a file, note it is the GameTextureManager that parallelises this process.

	void GarbageCollect(); // Used to free up space by removing the texture when it is no longer in use (references <= 0)

	sf::Texture* GetTexture(); // Used to return the texture for use by the caller.
};

class GameTextureManager {
	/*This class assists with the loading of textures for the game by enabling them to be loaded in parallel and accessed through a
	central system.*/
private:
	// Stores an instance of the GameTexture class for each unique texture in the game.
	GameTexture MainCharacterTexture;
	GameTexture BulletTexture;
	GameTexture EnemyOneTexture;
	GameTexture EnemyTwoTexture;
	GameTexture PowerUpPlaneTexture;

	void AdvanceLoadHelper(GameTexture& Texture); // A helper function designed to load the associated GameTexture in parallel, if
	// it hasn't yet been loaded.

	void GarbageCollectorHelper(GameTexture& Texture); // A helper function designed to garbage collect the associated GameTexture.
	// This is not done in parallel as it does not require an interaction with secondary storage.

public:
	GameTextureManager(); // Class constructor, used to tell the GameTextures which texture to associate with.

	void AdvanceLoad(string texture_name); // Used to load a texture in advance, or add a reference to it if it already has been
	// loaded

	void GarbageCollect(string texture_name); // Used garbage collect a specific GameTexture, and prepare it for potential
	// re-initialisation later

	sf::Texture* GetTexture(string texture_name); // Used to get a reference to the texture, to use minimal memory usage and
	// prevent making copies.
};