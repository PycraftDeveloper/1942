// This code was converted manually from Cython (.pyx) to C++ (.cpp). This
// code was originally written (by me) for PMMA (Python Multi-Media API) and the original
// code can be found here: https://github.com/PycraftDeveloper/PMMA/blob/main/cython_src/utility/perlin_noise.pyx
// This code exists in the public domain and (at the time of writing) has only been worked on by me.

#pragma warning( disable : 4244)

#include <vector>
#include <cmath>
#include <iostream>

#include "PerlinNoise.h"

using namespace std;

float PerlinNoise::fade(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float t, float a, float b) {
	// performs a linear interpolation (Linear intERPolation).
	// This program generates a floating point number between a and b, using t
	// to determine the weighting.
	return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y) {
	// This program takes a point in the permutation array defined earlier and
	// uses this to generate a floating point result value for two inputs.
	int h = hash & 7; // This converts the value at the point in the permutation array
	// into the range [0, 7] using a bitwise AND mask.
	float u = y;
	float v = x;
	if (h < 4) {
		u = x;
		v = y;
	}
	float result = u;
	if (h & 1) {
		result = -u;
	}
	if (h & 2) {
		result -= v;
	}
	else {
		result += v;
	}
	return result;
}

void PerlinNoise::init_permutation(int seed) {
	srand(seed);

	vector<int> perm(256);

	for (int i = 0; i < 256; ++i) {
		perm[i] = i; // fill the element at an index, with its index value.
	}

	for (int i = 255; i > 0; --i) {
		int j = rand() % (i + 1);
		swap(perm[i], perm[j]);
		// Then shuffle this array around by randomly generating new indexes to swap with.
		// This is where the srand code above is used.
	}

	p.resize(512); // Change the size from 256 -> 512
	float value;
	for (int i = 0; i < 256; ++i) {
		value = perm[i];
		p[i] = value;
		p[256 + i] = value;
		// Repeat the first 256 values again for the extended portion of the array.
	}
}

float PerlinNoise::range_adjustment(float value, int output_perlin_range[2]) {  // output_perlin_range = targeted output range
	float old_range;
	float new_range;
	float new_value;

	old_range = original_perlin_range[1] - original_perlin_range[0];
	new_range = output_perlin_range[1] - output_perlin_range[0];
	// Calculate the difference between the upper and lower bounds of the range.

	// performs the range adjustment on the input value.
	new_value = (((value - original_perlin_range[0]) * new_range) / old_range) + output_perlin_range[0];
	return new_value;
}

PerlinNoise::PerlinNoise(int seed, int octaves, float persistence) : octaves(octaves), persistence(persistence) {
	// class constructor, generates a flattened grid of 512 values that
	init_permutation(seed);
}

float PerlinNoise::Perlin(float x, float y) {
	// samples noise at x and y input.
	int X = static_cast<int>(floor(x)) & 255;
	int Y = static_cast<int>(floor(y)) & 255;
	// Next the inputs are floored, before being masked with 11111111 which is 255 in base 2.
	x -= floor(x);
	y -= floor(y);
	// Next the decimal portion of the original input is isolated.
	float u = fade(x);
	float v = fade(y);

	int A = p[X] + Y;
	int B = p[X + 1] + Y;

	return lerp(v, lerp(u, grad(p[A], x, y), grad(p[B], x - 1, y)),
		lerp(u, grad(p[A + 1], x, y - 1), grad(p[B + 1], x - 1, y - 1)));
}

float PerlinNoise::fractalBrownianMotion(float x, float y, int output_perlin_range[2]) {
	// take many samples (octaves) of the noise function at different scales (or frequencies)
	// and finds the cumulative amplitude relative to the maximum amplitude possible for the samples.
	// then this result is checked against a set of output ranges as this solution is
	// imperfect and needs to be adjusted to generate values in the range [-1, 1].
	float total = 0.0;
	float frequency = 1.0;
	float amplitude = 1.0;
	float maxValue = 0.0;
	float result;

	for (int i = 0; i < octaves; ++i) { // take many samples of the Perlin noise
		total += Perlin(x * frequency, y * frequency) * amplitude;
		maxValue += amplitude;
		amplitude *= persistence;
		frequency *= 2.0;
	}

	result = total / maxValue;

	if (result > original_perlin_range[1]) {
		original_perlin_range[1] = result;
	}
	else if (result < original_perlin_range[0]) {
		original_perlin_range[0] = result;
	}
	// Here the output is checked against the ranges for min/max to ensure it can be mapped into [-1, 1]

	return range_adjustment(result, output_perlin_range); // map the result into range [-1, 1].
}