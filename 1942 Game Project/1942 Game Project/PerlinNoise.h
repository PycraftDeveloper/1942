#pragma once
#pragma warning( disable : 4305)

#include <vector>

using namespace std;

class PerlinNoise {
	/*Used to generate a rudimentary form of 2D Perlin noise. This was converted from Cython into C++ from a separate project.*/
private:
	vector<int> p; // Stores the permutations to use.
	int octaves; // Stores the number of octaves (samples) of noise to generate
	float persistence; // Used to store how significant other samples of the noise are.
	float original_perlin_range[2] = { -0.772464f, 0.712795f }; // Approximations for the min/max values the noise generates.
	// This is then used in normalisation, and updated at runtime if the ranges change to ensure the target range, typically {-1, 1}
	// remains true.

	static float fade(float t); // Used to smooth the transition between different values during the interpolation process.

	static float lerp(float t, float a, float b); // Performs a linear interpolation

	static float grad(int hash, float x, float y); // Used to generate gradient values for the perlin noise process.

	void init_permutation(int seed); // Used to generate the permutations to use for the perlin noise. Reproducible via the seed input.

	float range_adjustment(float value, int output_perlin_range[2]); // Used to adjust the range as needed for the target application.

public:
	PerlinNoise(int seed, int octaves, float persistence);

	float Perlin(float x, float y); // Used to generate a single octave for the Perlin noise function.

	float fractalBrownianMotion(float x, float y, int output_perlin_range[2]); // Layers multiple samples of the Perlin noise function
	// together, by sampling values close to the (x, y) coordinate input. Uses Octaves and Persistence to adjust it's behaviour.
};