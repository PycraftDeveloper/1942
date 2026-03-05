#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "Plane_Player.h"

#include "BackgroundTexture.h"
#include "Bullet.h"

using namespace std;

inline const int* generate_color_from_value(const float value) { // A highly optimised function used to convert noise values into
	// colours intended to represent terrain. Uses pointers to avoid unnecessary copies, and uses inline to avoid the function call
	// overhead. This is probably the most called function in the game, so serves to be the most optimal.
	if (value > -0.5f && value <= 0.15f) {
		return Constants::SHALLOW_OCEAN_COLOUR; // light blue
	}

	if (value > 0.15f && value < 0.2f) {
		return Constants::SAND_COLOUR; // sand
	}

	if (value >= 0.2f && value < 0.6f) {
		return Constants::PLAINS_COLOUR; // light green
	}

	if (value >= 0.6f) {
		return Constants::DENSE_FOREST_COLOUR; // dark green
	}

	return Constants::DEEP_OCEAN_COLOUR; // dark blue
}

void HandleDisplaySizeChanged(Background& background_texture); // Used to adjust the way the game looks when the window is resized.
// This updates the confinements for the objects in the game level, and the scrolling background, which is passed in as a function.

void Positioner(sf::Vector2i& position, const sf::Vector2i& size = sf::Vector2i(0, 0)); // Used to add special behaviour to text rendering
// by enabling the text to be rendered centrally in both axis using the '-1' value, or anchored to the right hand side of the window
// with other negative values.

int TextSizeScaler(const int text_size); // Used to automatically adjust the text size when the window size changes. This has an associated
// bug fix as it now uses the smallest window scale denominator, instead of the largest (which would cause text to be rendered at the incorrect
// scale).

void BulletsManager(vector<unique_ptr<Bullet>>& Bullets); // This procedure iterates backwards over each bullet currently in the scene, and determines if
// it needs to exist or not by determining if it is on-screen. Bullets no longer on-screen are garbage collected.

float random_float(const float minimum, const float maximum); // Used to generate a random floating point number within the associated range.

float RotationFromDisplacement(const float XDisplacement, const float YDisplacement); // Used to convert a displacement into a rotation (in degrees)
// which is used in the enemy one plane's rotation from current velocity.

vector<sf::Vector2f> getCorners(sf::RectangleShape& rect); // Used to get the corners from a rectangle shape, with a transform applied to them,
// as otherwise the rectangle doesn't consider rotations. Used as a part of the separate axis theorem.

vector<sf::Vector2f> getCorners(Player& Protagonist); // Used to get the corners from the protagonist sprite, with a transform applied to them,
// as otherwise the rectangle doesn't consider rotations. Used as a part of the separate axis theorem.

bool SeperateAxisIntersection(sf::RectangleShape& FOV, Player& Protagonist); // Used to determine if the protagonist is inside the field of view
// rectangle.

sf::Vector2f GetNormal(const sf::Vector2f& PointOne, const sf::Vector2f& PointTwo); // Used to find the normal between two points

void ProjectToAxis(vector<sf::Vector2f>& CornerPoints, const sf::Vector2f& Axis, float& Min, float& Max); // Used to perform a scalar projection
// from the points created in 'getCorners' using the Axis as direction. Note that the parameters Min and Max here are both input and outputs
// which are subsequently used to determine if an intersection exists between the FOV and protagonist objects.

pair<sf::Vector2i, sf::Vector2i> CalculateBulletPositions(const sf::Vector2i& Position, const int HorizontalOffset, const int VerticalOffset, const float Angle);
// This is used to calculate where, from the centre of the plane sprite (Position) the bullets should be fired from. Note first that all planes are perfectly
// symmetrical in the y-axis. Returns two vectors which represent the position on the left and right reflections for the bullets to be positioned.

sf::Vector2f InterpolateBetweenVectors(const sf::Vector2f& StartPosition, const sf::Vector2f& EndPosition, const float CurrentTime, const float Duration); // ...
// Used to interpolate safely between the start and end positions based on current time over duration. Note that this function is made safe by preventing
// extrapolation beyond the end position.