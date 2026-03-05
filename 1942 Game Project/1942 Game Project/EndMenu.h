#include <string>

#include "TextRenderer.h"

using namespace std;

class EndMenu {
	/*Used to render the end menu, complete with the player's stats.*/
private:
	TextRenderer MainMenuTitle;
	TextRenderer GiveUpText; // Used to display the special message 'we give up' when the player completes level one and two successfully.
	TextRenderer PlayerResultsText; // Used to display the player's results.
	TextRenderer PlayAgainButton; // Used to allow the player to return immediately to level one (is interactable)
	TextRenderer MainMenuButton; // Used to allow the player to return to the main menu (is interactable)
	TextRenderer QuitButton; // Used to allow the player to immediately exit the game (is interactable)

	float ChangeColorTimer = 1.0f; // Used to determine how long it has been since the GiveUpText's colour changed.
	sf::Color RandomColor; // Used to store the random colour the GiveUpText will be.
public:
	EndMenu();

	string RunMenu();
};