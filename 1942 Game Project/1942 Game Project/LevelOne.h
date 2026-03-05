#include "LevelTemplate.h"

using namespace std;

class LevelOne : public LevelTemplate {
	/*Used to run the first level of the game, a lot of the functionality was moved into the LevelTemplate parent class,
	as level one and level two are very similar - but NOT the same.*/
public:
	string RunMenu();
};