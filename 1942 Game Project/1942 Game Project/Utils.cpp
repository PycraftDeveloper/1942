#pragma warning( disable : 4244)
#pragma warning( disable : 4267)

#include <vector>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "Utils.h"

#include "BackgroundTexture.h"
#include "Bullet.h"

#include "Registry.h"
#include "Constants.h"

using namespace std;

void HandleDisplaySizeChanged(Background& background_texture) {
	bool WindowTooSmall = false; // used to force the window to a specific min size to avoid a window-resizing bug.
	sf::Vector2i current_window_size = sf::Vector2i(Registry::window->getSize()); // get window size

	if (current_window_size.x < 360) { // force the window to have min x size of 360
		current_window_size.x = 360;
		WindowTooSmall = true;
	}
	if (current_window_size.y < 642) { // force the window to have a min y size of 640
		current_window_size.y = 642;
		WindowTooSmall = true;
	}

	if (WindowTooSmall) { // adjust window dimensions if necessary
		Registry::window->setSize(sf::Vector2u(current_window_size));
		WindowTooSmall = false;
	}

	if (current_window_size != Registry::window_size) { // used to add compatibility with window resizing
		// adjust the window size dependant variables in the registry
		Registry::window_size = current_window_size;
		Registry::aspect_ratio = static_cast<float>(Registry::window_size.x) / Registry::window_size.y;
		Registry::window_scale = { current_window_size.x / 360.f, current_window_size.y / 642.f };
		Registry::TextPadding = Constants::TEXT_PADDING * Registry::window_scale.y;

		float scale = current_window_size.x / 360.f;
		if (current_window_size.x > current_window_size.y) {
			scale = current_window_size.y / 642.f;
		}
		Registry::UnitWindowScale = scale; // find the smallest window scale
		Registry::WindowDiagonalSize = sqrt(pow(current_window_size.x, 2) + pow(current_window_size.y, 2)) + 1; // find the Pythagorean distance
		// add one to account for integer truncation.

		sf::FloatRect viewport(0, 0, current_window_size.x, current_window_size.y); // update the window viewport
		Registry::WindowViewportRect = sf::IntRect(0, 0, current_window_size.x, current_window_size.y); // update the area that objects can exist in without being
		// garbage collected.
		Registry::window->setView(sf::View(viewport));

		background_texture.HandleWindowSizeChanged(); // change background size adjustments
	}
}

void Positioner(sf::Vector2i& position, const sf::Vector2i& size) {
	if (position.x == -1) { // centre in x axis
		position.x = (Registry::window_size.x - size.x) / 2;
	}
	// there is no place in the game where text anchoring to the right side of the screen is needed, so is removed.

	if (position.y == -1) { // centre in y axis
		position.y = (Registry::window_size.y - size.y) / 2;
	}
	else if (position.y < -1) { // anchor to the bottom of the screen
		position.y = ((Registry::window_size.y - size.y) / 2) - position.y;
	}
}

int TextSizeScaler(const int text_size) {
	// text size to pixel size
	int pixel_size = 1.481 * text_size + 4.773;

	// scale pixel size with screen size
	pixel_size = pixel_size * min(Registry::window_scale.x, Registry::window_scale.y);

	// convert pixel size to text size
	return (pixel_size - 4.773) / 1.481;
}

void BulletsManager(vector<unique_ptr<Bullet>>& Bullets) {
	for (int i = Bullets.size() - 1; i >= 0; i--) { // garbage collection loop, so iterates backwards to avoid issue of vector
		if (Bullets[i]->Loaded && !Registry::WindowViewportRect.intersects(Bullets[i]->BulletSpriteRect)) { // if the bullet has been
			// fully loaded, and is no longer on screen
			Bullets.erase(Bullets.begin() + i); // garbage collect it.
		}
	}
}

float random_float(const float minimum, const float maximum) {
	float random = static_cast<float>(rand()) / RAND_MAX; // this is used to generate a random
	// integer number between 0 and the maximum random value (which can be identified using the RAND_MAX)
	// constant. Then this random integer value is converted into a float between 0 and 1 (inclusive) by dividing
	// the value by the RAND_MAX constant. This allows for the generation of a random floating point value.
	return minimum + random * (maximum - minimum); // the previously generated floating point value is then converted
	// from the range [0, 1] to [minimum, maximum] using the following calculation, which was initially identified by
	// using the interpolation equation from earlier.
}

float RotationFromDisplacement(const float XDisplacement, const float YDisplacement) {
	return (atan2f(YDisplacement, XDisplacement) * (180.f / Constants::PI)) - 90.f; // calculate the angle in degrees based on the
	// displacement in the x and y axis. The -90 degrees is used to make the angle point in the direction of the plane
}

vector<sf::Vector2f> getCorners(sf::RectangleShape& rect) {
	vector<sf::Vector2f> corners(4); // create a vector of 4 elements
	sf::Transform transform = rect.getTransform(); // get the transform for the plane FOV
	sf::FloatRect local = rect.getLocalBounds(); // get the local bounds for the plane FOV
	// calculate the position (including rotation as the local bounds are not rotated) of the corners of the rectangle
	corners[0] = transform.transformPoint(local.left, local.top); // top left
	corners[1] = transform.transformPoint(local.left + local.width, local.top); // top right
	corners[2] = transform.transformPoint(local.left + local.width, local.top + local.height); // bottom right
	corners[3] = transform.transformPoint(local.left, local.top + local.height); // bottom left
	return corners;
}

vector<sf::Vector2f> getCorners(Player& Protagonist) {
	vector<sf::Vector2f> corners(4);
	sf::IntRect local = Protagonist.LocalSpriteRect; // get the local bounds for the player
	// Transform the points on the local position, using the transform of the player
	corners[0] = Protagonist.Transform.transformPoint(local.left, local.top); // top left
	corners[1] = Protagonist.Transform.transformPoint(local.left + local.width, local.top); // top right
	corners[2] = Protagonist.Transform.transformPoint(local.left + local.width, local.top + local.height); // bottom right
	corners[3] = Protagonist.Transform.transformPoint(local.left, local.top + local.height); // bottom left
	return corners;
}

bool SeperateAxisIntersection(sf::RectangleShape& FOV, Player& Protagonist) {
	std::vector<sf::Vector2f> FOVCorners = getCorners(FOV);
	std::vector<sf::Vector2f> ProtagonistCorners = getCorners(Protagonist);
	std::vector<sf::Vector2f> Axes;
	float FOVCornerMin, FOVCornerMax, ProtagonistCornersMin, ProtagonistCornersMax;

	// calculate the axes to project onto
	Axes.push_back(GetNormal(FOVCorners[0], FOVCorners[1]));
	Axes.push_back(GetNormal(ProtagonistCorners[0], ProtagonistCorners[1]));

	Axes.push_back(GetNormal(FOVCorners[1], FOVCorners[2]));
	Axes.push_back(GetNormal(ProtagonistCorners[1], ProtagonistCorners[2]));

	// perform the projections
	for (const sf::Vector2f& axis : Axes) {
		ProjectToAxis(FOVCorners, axis, FOVCornerMin, FOVCornerMax);
		ProjectToAxis(ProtagonistCorners, axis, ProtagonistCornersMin, ProtagonistCornersMax);

		// evaluate if the projections overlap, and perform early exit.
		if (FOVCornerMax < ProtagonistCornersMin || ProtagonistCornersMax < FOVCornerMin)
			return false; // no intersection
	}
	return true; // intersection exists
}

sf::Vector2f GetNormal(const sf::Vector2f& PointOne, const sf::Vector2f& PointTwo) {
	sf::Vector2f edge = PointTwo - PointOne; // calculate the delta between the two points
	return sf::Vector2f(-edge.y, edge.x) / sqrt(edge.x * edge.x + edge.y * edge.y); // normalise the vector to get the normal
}

void ProjectToAxis(vector<sf::Vector2f>& CornerPoints, const sf::Vector2f& Axis, float& Min, float& Max) {
	Min = CornerPoints[0].x * Axis.x + CornerPoints[0].y * Axis.y; // initialise the min and max values
	Max = Min;
	for (size_t i = 1; i < CornerPoints.size(); i++) { // iterate over each of the points for the projection
		float Projection = CornerPoints[i].x * Axis.x + CornerPoints[i].y * Axis.y; // calculate the projection of the point onto the axis

		// update the min and max projection values (and pass them back out of the procedure)
		if (Projection < Min) {
			Min = Projection;
		}

		if (Projection > Max) {
			Max = Projection;
		}
	}
}

pair<sf::Vector2i, sf::Vector2i> CalculateBulletPositions(const sf::Vector2i& Position, const int HorizontalOffset, const int VerticalOffset, const float Angle) {
	float angleRadians = Angle * (Constants::PI / 180.0f); // convert degrees to radians

	float CosAngle = cos(angleRadians); // calculate the x component
	float SinAngle = sin(angleRadians); // calculate the y component

	// calculate the bullet positions relative to the plane's position without rotations
	sf::Vector2i bulletRight = { HorizontalOffset, VerticalOffset };
	sf::Vector2i bulletLeft = { -HorizontalOffset, VerticalOffset };

	sf::Vector2i rotatedRight = {
		(int)(bulletRight.x * CosAngle - bulletRight.y * SinAngle),
		(int)(bulletRight.x * SinAngle + bulletRight.y * CosAngle)
	}; // apply the rotations to the right bullet position
	sf::Vector2i rotatedLeft = {
		(int)(bulletLeft.x * CosAngle - bulletLeft.y * SinAngle),
		(int)(bulletLeft.x * SinAngle + bulletLeft.y * CosAngle)
	}; // apply the rotations to the left bullet position

	// add the plane's position to the bullet positions to get global positions
	rotatedRight.x += Position.x;
	rotatedRight.y += Position.y;
	rotatedLeft.x += Position.x;
	rotatedLeft.y += Position.y;

	return { rotatedLeft, rotatedRight }; // return the two vectors
}

sf::Vector2f InterpolateBetweenVectors(const sf::Vector2f& StartPosition, const sf::Vector2f& EndPosition, const float CurrentTime, const float Duration) {
	if (CurrentTime > Duration) { // safety check to prevent extrapolation when the current time is greater than the duration
		return EndPosition;
	}

	return StartPosition + (EndPosition - StartPosition) * (CurrentTime / Duration); // perform a linear interpolation between the start and end positions
}