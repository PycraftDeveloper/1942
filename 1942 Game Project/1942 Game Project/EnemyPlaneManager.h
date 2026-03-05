#pragma once

#include <vector>

#include "Plane_EnemyOne.h"
#include "Plane_EnemyTwo.h"

using namespace std;

class EnemyPlaneManager {
	/*Used to spawn and handle the compute/rendering of enemy planes. This also controls when a plane should shrink because
	it is 'underneath' another enemy plane. */
public:
	EnemyTwoPlane* SpawnedEnemyTwoPlane = nullptr; // There can only ever be one enemy two plane at a time, this is stored here.
	vector<unique_ptr<EnemyOnePlane>> SpawnedEnemyOnePlanes; // Used to store the numerous enemy planes dynamically.
private:
	float TimeToNextSpawn = 3.0f; // Controls how long it will be before the next 'wave' of planes spawn.
	int EnemyOnePlanesWaveCounter = 3; // Controls how many enemy one planes to spawn
public:
	void Compute();

	void Render();

	~EnemyPlaneManager();
};