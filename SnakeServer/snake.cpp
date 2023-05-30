#include "snake.h"

Snake::Snake()
{
    direction = right;
    _homeDots = {{0, 0}, {0, 0}};
    _enemyDots = {{0, 0}, {0, 0}};
}

Snake::~Snake() {}
