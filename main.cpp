#include <cstdio>
#include <iostream>
#include "game.h"
#include "save.h"
#include "util.h"

using namespace std;

// Wait for one key press so the player can read what is on screen.
// Input: none. Output: none.
static void waitKey() {
    cout << "\nPress any key, then Enter, to continue...\n> ";
    char c;
    cin >> c;   // on EOF this fails immediately, which is fine
}

// Wrap up a finished run: show the ending, record the score and
// delete the save file of concluded games.
// Input: game state after gameLoop has returned. Output: none.
static void endGame(GameState *g) {
    if (g->victory) {
        render(g);
        cout << "\n*** VICTORY! You defeated the boss and escaped the dungeon! ***\n";
        recordScore("VICTORY", g);
        remove("savegame.txt");
        waitKey();
    } else if (g->gameOver) {
        render(g);
        cout << "\n*** GAME OVER on floor " << g->floor << ". Better luck next time! ***\n";
        recordScore("DEFEAT", g);
        remove("savegame.txt");
        waitKey();
    } else if (g->quit) {
        cout << "\nProgress saved to savegame.txt. See you soon!\n";
        waitKey();
    }
}

int main() {
    seedRandom();
    bool running = true;
    while (running) {
        clearScreen();
        cout << "==========================================\n"
                "            D U N G E O N\n"
                "              E S C A P E\n"
                "==========================================\n"
                "  1. New game\n"
                "  2. Load game\n"
                "  3. High scores\n"
                "  4. Quit\n"
                "> ";
        char choice;
        if (!(cin >> choice))
            break;
        if (choice == '1') {
            clearScreen();
            cout << "Select difficulty:\n"
                    "  1. Easy   - 3 floors, weak monsters, 120 HP\n"
                    "  2. Normal - 5 floors, 100 HP\n"
                    "  3. Hard   - 7 floors, tough monsters, 80 HP\n"
                    "> ";
            char d;
            if (!(cin >> d))
                break;
            if (d < '1' || d > '3') {
                cout << "Invalid choice.\n";
                waitKey();
                continue;
            }
            GameState *g = newGame(d - '0');
            gameLoop(g);
            endGame(g);
            freeGame(g);
        } else if (choice == '2') {
            GameState *g = loadGame();
            if (g == NULL) {
                clearScreen();
                cout << "No saved game found.\n";
                waitKey();
                continue;
            }
            gameLoop(g);
            endGame(g);
            freeGame(g);
        } else if (choice == '3') {
            showHighscores();
            waitKey();
        } else if (choice == '4') {
            running = false;
        }
    }
    cout << "Thanks for playing!\n";
    return 0;
}
