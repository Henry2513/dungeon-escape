#ifndef SAVE_H
#define SAVE_H

#include "game.h"

// Write the current game state to savegame.txt.
// Input: game state. Output: true if the file was written successfully.
bool saveGame(const GameState *g);

// Read a saved game from savegame.txt and rebuild the whole state.
// Input: none. Output: heap-allocated GameState, or NULL if there is
//         no save file or it could not be parsed.
GameState* loadGame();

// Append one result line to highscores.txt.
// Input: result text ("VICTORY" / "DEFEAT"), game state. Output: none.
void recordScore(const char *result, const GameState *g);

// Print the high-score table (top 10 runs, sorted by gold).
// Input: none. Output: none (prints).
void showHighscores();

#endif
