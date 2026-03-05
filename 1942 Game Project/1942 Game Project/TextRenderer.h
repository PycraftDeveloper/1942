#pragma once

#include <string>

#include <SFML/Graphics.hpp>

using namespace std;

class TextRenderer {
	/*Used to render text throughout the game, using the font loaded into registry. This class also handles text interactions with both
	the user's input (mouse) and also with entities on-screen. This class is highly optimised to cache the rendered text for re-use
	instead of rendering the same thing every frame. */
private:
	sf::Text ObjectText; // Stores the text that is to be rendered, used to cache when render parameters don't change
	string TextContent = ""; // Stores the previous content for the text, used to determine if render parameters have changed.
	int TextSize = -1; // Stores the previous size for the text, used to determine if render parameters have changed.
	sf::Color TextColor = sf::Color::Transparent; // Stores the previous colour for the text, used to determine if render parameters have changed.
	bool TextUnderline = false; // Stores the previous underline state for the text, used to determine if render parameters have changed.
	bool TextBold = false; // Stores the previous bold state for the text, used to determine if render parameters have changed.
	bool TextIsInteractable = false; // Used to store if the text is intractable by the mouse cursor.
	bool TextHovering = false; // Used to determine if the mouse cursor is currently inside the text's collider.
public:
	TextRenderer();

	void SetTextIsInteractable(bool is_interactable); // Used to tell the text object that this text object behaves as a button
	// and can therefore be interacted with.

	void Render(string content, int size, sf::Color color, sf::Vector2i position, bool underline = false, bool bold = false); // ...
	// A highly optimised rendering algorithm that only renders the ObjectText when it has changed.

	sf::Vector2i GetSize(bool position_offset = false); // Used to get the size of the text - needed when positioning text objects on-screen.

	bool Interact(); // Used to determine if the player is clicking on the text, which allows the text to behave as a button.

	bool IsTextHovering(); // Used to determine if the player's mouse is currently inside the text's collider

	sf::FloatRect GetTextGloablBounds(); // Used to get the text object's collider, which is needed for object collision detection
	// as seen in the power-up text.
};