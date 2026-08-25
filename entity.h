#ifndef ENTITY_H
#define ENTITY_H

#include <string>

// The player character and all of its stats.
struct Player {
    int x, y;        // position on the map
    int hp, maxHp;   // current / maximum hit points
    int atk;         // attack power
    int gold;        // gold collected
    int potions;     // number of healing potions carried
    int level;       // experience level
    int exp;         // experience points towards the next level
};

// A monster, stored as a node of a singly linked list.
struct Monster {
    int x, y;            // position on the map
    int hp, maxHp;       // current / maximum hit points
    int atk;             // attack power
    int expReward;       // experience granted when killed
    int goldReward;      // gold granted when killed
    bool isBoss;         // true for the floor boss
    Monster *next;       // next node in the linked list
};

// Allocate a new monster node with the given attributes.
// Input: position, stats, rewards and boss flag.
// Output: pointer to the new heap-allocated node (next = NULL).
Monster* createMonster(int x, int y, int hp, int atk,
                       int expReward, int goldReward, bool isBoss);

// Prepend node m to the linked list headed by head.
// Input: list head (by reference), node to insert. Output: none.
void addMonster(Monster* &head, Monster* m);

// Find the monster occupying cell (x, y).
// Input: list head, coordinates.
// Output: pointer to the monster, or NULL if the cell is free.
Monster* monsterAt(Monster* head, int x, int y);

// Remove node m from the list and free its memory.
// Input: list head (by reference), node to remove.
// Output: true if the node was found and deleted.
bool removeMonster(Monster* &head, Monster* m);

// Free every node of the list and set head to NULL.
// Input: list head (by reference). Output: none.
void freeMonsters(Monster* &head);

// Roll a randomized damage value: atk plus/minus a small random bonus.
// Input: base attack power. Output: damage dealt (at least 1).
int rollDamage(int atk);

// Resolve one round of combat: the player hits the monster first;
// if the monster survives it strikes back once.
// Input: player (by reference), monster, output flag monsterDied.
// Output: a human-readable combat log line; monsterDied tells whether
//         the monster was slain.
std::string fight(Player &p, Monster *m, bool &monsterDied);

#endif
