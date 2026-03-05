#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

using namespace std;

class SoundEffectManager {
	/*This class is used to play multiple sound effects at the same time (effectively playing them as one-shots), by storing
	sounds currently playing in an array. Also used to cache sound effects so they don't need to be loaded each time the sound effect
	plays. Note that this is NOT suitable for music playback as the whole file is loaded into memory.*/
private:
	vector<sf::Sound> PlayingSoundEffects; // Stores currently playing sounds
	unordered_map<string, sf::SoundBuffer> CachedSoundEffects; // Stores caches for the sound effects to bypass secondary storage.
public:
	void PlayOneshot(string SoundEffectName); // Used to play a sound without pausing or stopping any other currently playing sounds.

	void GarbageCollectSoundEffects(); // Used to automatically remove sound effects from the 'playing sound effects' vector when they
	// are done.
};