#include <cstdlib>
#include <ctime>
#include <iostream>
#include "util.h"

using namespace std;

void seedRandom() {
    srand((unsigned int)time(NULL));
}

int randInt(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}

void clearScreen() {
    // ANSI escape: erase screen, then move cursor to the top-left corner.
    cout << "\033[2J\033[1;1H";
}
