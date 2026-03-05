#include <iostream>

#include "EnemyPlaneManager.h"

#include "Utils.h"

#include "Registry.h"

void EnemyPlaneManager::Compute() {
	TimeToNextSpawn -= Registry::DisplayRefreshTime; // count down to when the next 'wave' will happen

	if ((TimeToNextSpawn <= 0 || SpawnedEnemyOnePlanes.size() == 0) && SpawnedEnemyTwoPlane == nullptr) { // if the spawn timer reaches zero...
		// or there are no enemies in the level, and the larger plane isn't in the scene, spawn new planes
		TimeToNextSpawn = random_float(5.0f, 15.0f); // reset the timer

		if (EnemyOnePlanesWaveCounter <= 0) { // if enough waves have passed, the larger enemy will spawn
			SpawnedEnemyTwoPlane = new EnemyTwoPlane(); // create new larger enemy plane
			EnemyOnePlanesWaveCounter = 1 + rand() % 3; // [1, 3] incl. how many waves should pass until the next spawn
		}
		else {
			EnemyOnePlanesWaveCounter--; // reduce the number of waves by one.
			int NumberOfEnemyOnePlanesToSpawn = 1 + rand() % 4; // [1, 4] incl. how many level one planes to spawn
			for (int i = 0; i < NumberOfEnemyOnePlanesToSpawn; i++) {
				SpawnedEnemyOnePlanes.emplace_back(make_unique<EnemyOnePlane>()); // spawn in new level one planes
			}
		}
	}

	if (SpawnedEnemyTwoPlane != nullptr) { // if the larger level two plane exists, compute that too. If compute returns true, plane has left the top of the
		// window and should be garbage collected.
		if (SpawnedEnemyTwoPlane->Compute()) {
			delete SpawnedEnemyTwoPlane;
			SpawnedEnemyTwoPlane = nullptr;
		}
	}

	vector<bool> ShouldBeShrunk(SpawnedEnemyOnePlanes.size(), false); // create a vector the same size as the number of level one enemy planes
	// and set every element to false.

	vector<bool> EnemyOnePlaneComputed(SpawnedEnemyOnePlanes.size(), false); // keep track of which enemy one planes have been computed

	bool EnemyTwoPlaneSpawned = SpawnedEnemyTwoPlane != nullptr;

	for (int i = 0; i < SpawnedEnemyOnePlanes.size(); i++) {
		EnemyOnePlane& FirstPlane = *SpawnedEnemyOnePlanes[i];

		if (!EnemyOnePlaneComputed[i]) {
			FirstPlane.Compute(); // compute the plane to get the latest position
			EnemyOnePlaneComputed[i] = true;
		}

		for (int j = i + 1; j < SpawnedEnemyOnePlanes.size(); j++) { // check each level one plane against each other level one plane
			EnemyOnePlane& SecondPlane = *SpawnedEnemyOnePlanes[j];

			if (!EnemyOnePlaneComputed[j]) {
				SecondPlane.Compute(); // compute the plane to get the latest position
				EnemyOnePlaneComputed[j] = true;
			}

			if (FirstPlane.PlaneRect.intersects(SecondPlane.PlaneRect)) { // if they are in collision
				if (i > j) { // Move the plane rendered first down
					ShouldBeShrunk[j] = true;
					ShouldBeShrunk[i] = false;
				}
				else {
					ShouldBeShrunk[i] = true;
					ShouldBeShrunk[j] = false;
				}
			}
		}

		if (EnemyTwoPlaneSpawned && FirstPlane.PlaneRect.intersects(SpawnedEnemyTwoPlane->PlaneRect)) { // also check if the lever one plane is in collision with the level two plane
			// and if it is, move it 'down'
			ShouldBeShrunk[i] = true;
		}
	}

	for (int i = 0; i < SpawnedEnemyOnePlanes.size(); i++) { // for all the planes that need to be shrunk, shrink them slowly to avoid graphical flickering,
		// planes that don't need to be shrunk can be made larger again
		if (ShouldBeShrunk[i]) {
			SpawnedEnemyOnePlanes[i]->DynamicScaling -= 0.225f * Registry::DisplayRefreshTime; // move down faster as assisted by gravity
			SpawnedEnemyOnePlanes[i]->DynamicScaling = max(SpawnedEnemyOnePlanes[i]->DynamicScaling, 0.25f);
		}
		else {
			SpawnedEnemyOnePlanes[i]->DynamicScaling += 0.075f * Registry::DisplayRefreshTime; // move up slower as working against gravity
			SpawnedEnemyOnePlanes[i]->DynamicScaling = min(SpawnedEnemyOnePlanes[i]->DynamicScaling, 0.5f);
		}
	}
}

void EnemyPlaneManager::Render() { // render all the spawned enemy planes to the window
	for (int i = 0; i < SpawnedEnemyOnePlanes.size(); i++) {
		SpawnedEnemyOnePlanes[i]->Render();
	}
	if (SpawnedEnemyTwoPlane != nullptr) {
		SpawnedEnemyTwoPlane->Render();
	}
}

EnemyPlaneManager::~EnemyPlaneManager() {
	SpawnedEnemyOnePlanes.clear(); // destroy all currently spawned enemy one planes

	delete SpawnedEnemyTwoPlane;
	SpawnedEnemyTwoPlane = nullptr;
}