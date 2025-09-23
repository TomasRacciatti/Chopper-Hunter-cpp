#include "Game.h"


int main()
{
    Game* game = new Game(); 
    game->Play();
    delete game;

    return 0;
}