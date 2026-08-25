#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include "entity.h"
#include "map.h"

// All the numbers that make one difficulty level different from another.
struct Difficulty {
    std::string name;
    int mapWidth, mapHeight;
    int roomCount;
    int floors;          // total floors; the boss waits on the last one
    int playerHp, playerAtk;
    int monsterCount;    // normal monsters per floor
    int monsterHpMin, monsterHpMax;
    int monsterAtkMin, monsterAtkMax;
    int bossHp, bossAtk;
    int potionCount, goldCount, trapCount, shrineCount;
};

// Everything that defines one play-through.
struct GameState {
    Player player;
    Monster *monsters;               // linked list of live monsters
    Map *map;                        // current floor
    Difficulty diff;                 // tuning of the chosen difficulty
    int diffIndex;                   // 1 = Easy, 2 = Normal, 3 = Hard
    int floor;                       // current floor, 1-based
    bool gameOver;                   // player died
    bool victory;                    // boss defeated
    bool quit;                       // player saved and left
    std::vector<std::string> messages;   // recent event log
};

// Return the tuning table for difficulty choice (1/2/3).
// Input: menu choice. Output: Difficulty struct (defaults to Normal).
Difficulty getDifficulty(int choice);

// Create a brand new game on floor 1 with a fresh player.
// Input: difficulty choice. Output: heap-allocated GameState.
GameState* newGame(int difficultyChoice);

// Free the whole game state and set the pointer to NULL.
// Input: state pointer (by reference). Output: none.
void freeGame(GameState* &g);

// (Re)build the current floor: map, items, stairs, monsters and boss.
// Input: game state. Output: none.
void setupFloor(GameState *g);

// Append a line to the on-screen event log (keeps the last few).
// Input: game state, message text. Output: none.
void addMessage(GameState *g, const std::string &msg);

// Draw the whole screen: header, map, player stats, controls, log.
// Input: game state. Output: none (prints).
void render(const GameState *g);

// Run the game until victory, death or save-and-quit.
// Input: game state. Output: none (result flags live in the state).
void gameLoop(GameState *g);

#endif
