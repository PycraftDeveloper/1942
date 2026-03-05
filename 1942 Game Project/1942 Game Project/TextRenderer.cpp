#include <iostream>

#include <SFML/Graphics.hpp>

#include "TextRenderer.h"

#include "Utils.h"

#include "Registry.h"

using namespace std;

TextRenderer::TextRenderer() {
	ObjectText.setFont(*Registry::GameFont); // set the font to the one loaded centrally for the game
}

void TextRenderer::SetTextIsInteractable(bool is_interactable) {
	TextIsInteractable = is_interactable;
}

void TextRenderer::Render(string content, int size, sf::Color color, sf::Vector2i position, bool underline, bool bold) {
	underline |= TextHovering; // if the text is being hovered over, it should be underlined regardless of the underline parameter passed in.
	// but TextHovering is only set when the text is set to be intractable.
	if (content == TextContent && size == TextSize && color == TextColor && underline == TextUnderline) { // uses an optimisation to avoid
		// needing to re-render the same text every frame, only changes to the text will cause it to be re-rendered.
		Positioner(position, sf::Vector2i(ObjectText.getGlobalBounds().getSize())); // make any adjustments to the position
		ObjectText.setPosition(sf::Vector2f(position));
		Registry::window->draw(ObjectText); // render the same text again to the screen (not render the text object)
		return;
	}
	// update the text cache with the changed parameters
	TextContent = content;
	TextSize = size;
	TextColor = color;
	TextUnderline = underline;
	TextBold = bold;

	// set the text parameters
	ObjectText.setString(TextContent);
	ObjectText.setCharacterSize(TextSize);
	ObjectText.setFillColor(TextColor);

	// set the text styles
	int style = sf::Text::Regular;
	if (TextUnderline) {
		style |= sf::Text::Underlined;
	}
	if (TextBold) {
		style |= sf::Text::Bold;
	}
	ObjectText.setStyle(style);

	// set the text position, this is a custom positioner, it allows more control on where the text is positioned.
	Positioner(position, sf::Vector2i(ObjectText.getGlobalBounds().getSize()));
	ObjectText.setPosition(sf::Vector2f(position));

	Registry::window->draw(ObjectText);
}

sf::Vector2i TextRenderer::GetSize(bool position_offset) {
	sf::Vector2i size = sf::Vector2i(ObjectText.getGlobalBounds().getSize()); // get the text size
	if (position_offset) {
		return size + sf::Vector2i(ObjectText.getGlobalBounds().getPosition()); // determine the size of the text including the position offset
	}
	return size;
}
bool TextRenderer::Interact() {
	TextHovering = false; // reset text hovering state
	if (TextIsInteractable) { // check if the text is actually intractable
		if (ObjectText.getGlobalBounds().contains(sf::Vector2f(Registry::MousePosition))) { // if mouse in text bounds
			TextHovering = true;
			if (Registry::MouseButtonDown) { // if clicked
				Registry::MouseButtonDown = false; // reset the mouse button state
				return true; // return true to indicate the text was clicked
			}
		}
	}
	return false; // otherwise return false to indicate the text was not clicked
}

bool TextRenderer::IsTextHovering() {
	if (TextIsInteractable) {
		if (ObjectText.getGlobalBounds().contains(sf::Vector2f(Registry::MousePosition))) { // if the mouse is in the text bounds
			return true; // return true to indicate the text is being hovered over
		}
	}
	return false; // otherwise return false to indicate the text is not being hovered over
}

sf::FloatRect TextRenderer::GetTextGloablBounds() {
	return ObjectText.getGlobalBounds(); // return the text's global bounds, used for collision detection
}