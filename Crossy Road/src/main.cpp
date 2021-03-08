#include "Game.h"

int main()
{
	Game* game = new Game();
	if (game->init())
		game->run();

	return 0;
}

