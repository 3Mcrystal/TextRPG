#include "GameManager.h"

int main() {
    GameManager game;

    game.start();
    game.loop();
    game.shutdown();

    return 0;
}
