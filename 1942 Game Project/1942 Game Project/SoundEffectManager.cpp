#pragma warning( disable : 4267)
#pragma warning( disable : 4244)

#include <iostream>

#include "SoundEffectManager.h"

#include "Registry.h"

void SoundEffectManager::PlayOneshot(string SoundEffectName) {
	// check if the sound effect is already loaded, if not load it
	if (CachedSoundEffects.find(SoundEffectName) == CachedSoundEffects.end()) {
		CachedSoundEffects[SoundEffectName].loadFromFile(
			string("resources") + Registry::path_seperator +
			"sounds" + Registry::path_seperator +
			SoundEffectName); // load and cache the current sound effect
	}

	PlayingSoundEffects.emplace_back(); // create a new copy of the sound effect
	PlayingSoundEffects.back().setBuffer(CachedSoundEffects[SoundEffectName]); // set the sound 'buffer' to play (the cached sound)
	PlayingSoundEffects.back().setVolume(Registry::SFXVolume); // set the volume to the current sound effect volume
	PlayingSoundEffects.back().play(); // play the sound effect
}

void SoundEffectManager::GarbageCollectSoundEffects() {
	for (int i = PlayingSoundEffects.size() - 1; i >= 0; i--) { // garbage collection loop, so iterates backwards to avoid issue of vector
		// changing size as sounds that have finished playing are removed.
		if (PlayingSoundEffects[i].getStatus() == sf::Sound::Stopped) { // if the sound effect has stopped playing
			PlayingSoundEffects.erase(PlayingSoundEffects.begin() + i); // garbage collect it.
		}
	}
}