#include <cstdlib>
#include <iostream>
#include "game.h"
#include "save.h"
#include "util.h"

using namespace std;

Difficulty getDifficulty(int choice) {
    Difficulty d;
    if (choice == 1) {          // Easy
        d.name = "Easy";
        d.mapWidth = 40;  d.mapHeight = 16;  d.roomCount = 8;
        d.floors = 3;
        d.playerHp = 120; d.playerAtk = 12;
        d.monsterCount = 5;
        d.monsterHpMin = 15; d.monsterHpMax = 25;
        d.monsterAtkMin = 3; d.monsterAtkMax = 6;
        d.bossHp = 60;  d.bossAtk = 8;
        d.potionCount = 4; d.goldCount = 5; d.trapCount = 2; d.shrineCount = 2;
    } else if (choice == 3) {   // Hard
        d.name = "Hard";
        d.mapWidth = 60;  d.mapHeight = 20;  d.roomCount = 12;
        d.floors = 7;
        d.playerHp = 80;  d.playerAtk = 9;
        d.monsterCount = 9;
        d.monsterHpMin = 25; d.monsterHpMax = 45;
        d.monsterAtkMin = 5; d.monsterAtkMax = 10;
        d.bossHp = 130; d.bossAtk = 14;
        d.potionCount = 2; d.goldCount = 4; d.trapCount = 4; d.shrineCount = 2;
    } else {                    // Normal (default)
        d.name = "Normal";
        d.mapWidth = 50;  d.mapHeight = 18;  d.roomCount = 10;
        d.floors = 5;
        d.playerHp = 100; d.playerAtk = 10;
        d.monsterCount = 7;
        d.monsterHpMin = 20; d.monsterHpMax = 35;
        d.monsterAtkMin = 4; d.monsterAtkMax = 8;
        d.bossHp = 90;  d.bossAtk = 11;
        d.potionCount = 3; d.goldCount = 5; d.trapCount = 3; d.shrineCount = 2;
    }
    return d;
}

GameState* newGame(int difficultyChoice) {
    GameState *g = new GameState;
    g->monsters = NULL;
    g->map = NULL;
    g->diffIndex = difficultyChoice;
    g->diff = getDifficulty(difficultyChoice);
    g->floor = 1;
    g->gameOver = false;
    g->victory = false;
    g->quit = false;

    Player p;
    p.hp = p.maxHp = g->diff.playerHp;
    p.atk = g->diff.playerAtk;
    p.gold = 0;
    p.potions = 1;
    p.level = 1;
    p.exp = 0;
    p.x = p.y = 0;
    g->player = p;

    setupFloor(g);
    addMessage(g, "You enter the dungeon. Find the stairs down!");
    return g;
}

void freeGame(GameState* &g) {
    if (g == NULL)
        return;
    freeMonsters(g->monsters);
    freeMap(g->map);
    delete g;
    g = NULL;
}

void setupFloor(GameState *g) {
    freeMonsters(g->monsters);
    freeMap(g->map);
    g->map = createMap(g->diff.mapWidth, g->diff.mapHeight);
    generateMap(g->map, g->diff.roomCount);

    // Items, traps, shrines and (except on the last floor) the stairs.
    for (int i = 0; i < g->diff.potionCount; i++) placeRandom(g->map, '!');
    for (int i = 0; i < g->diff.goldCount; i++)   placeRandom(g->map, '$');
    for (int i = 0; i < g->diff.trapCount; i++)   placeRandom(g->map, '^');
    for (int i = 0; i < g->diff.shrineCount; i++) placeRandom(g->map, '?');
    if (g->floor < g->diff.floors)
        placeRandom(g->map, '>');

    // Player starts on a random floor cell.
    randomFloorTile(g->map, g->player.x, g->player.y);

    // Monsters grow stronger on deeper floors.
    int placed = 0, tries = 0;
    while (placed < g->diff.monsterCount && tries < 500) {
        tries++;
        int x, y;
        if (!randomFloorTile(g->map, x, y))
            break;
        if ((x == g->player.x && y == g->player.y) || monsterAt(g->monsters, x, y))
            continue;
        int hp  = randInt(g->diff.monsterHpMin, g->diff.monsterHpMax) + (g->floor - 1) * 4;
        int atk = randInt(g->diff.monsterAtkMin, g->diff.monsterAtkMax) + (g->floor - 1);
        addMonster(g->monsters,
                   createMonster(x, y, hp, atk, 8 + g->floor * 3, randInt(5, 15), false));
        placed++;
    }

    // The boss guards the last floor.
    if (g->floor == g->diff.floors) {
        int x, y;
        if (randomFloorTile(g->map, x, y)) {
            addMonster(g->monsters,
                       createMonster(x, y, g->diff.bossHp, g->diff.bossAtk,
                                     100, 100, true));
        }
    }
}

void addMessage(GameState *g, const string &msg) {
    g->messages.push_back(msg);
    if (g->messages.size() > 6)
        g->messages.erase(g->messages.begin());
}

void render(const GameState *g) {
    clearScreen();
    cout << "=== DUNGEON ESCAPE ===   Difficulty: " << g->diff.name
         << "   Floor " << g->floor << "/" << g->diff.floors << "\n";
    renderMap(g->map, g->player.x, g->player.y, g->monsters);
    const Player &p = g->player;
    cout << "HP: " << p.hp << "/" << p.maxHp
         << "   ATK: " << p.atk
         << "   Lv: " << p.level
         << "   EXP: " << p.exp << "/" << p.level * 20
         << "   Gold: " << p.gold
         << "   Potions: " << p.potions << "\n";
    cout << "[w/a/s/d] move & attack   [i] drink potion   [q] save & quit\n";
    cout << "----------------------------------------------------------\n";
    for (size_t i = 0; i < g->messages.size(); i++)
        cout << g->messages[i] << "\n";
}

// Mark the player dead if HP dropped to zero.
// Input: game state. Output: none (sets gameOver flag).
static void checkPlayerDeath(GameState *g) {
    if (g->player.hp <= 0) {
        g->player.hp = 0;
        g->gameOver = true;
        addMessage(g, "You have fallen in the dungeon...");
    }
}

// Award experience after a kill and apply any level-ups.
// Input: game state, gained exp. Output: none (updates player).
static void gainExp(GameState *g, int exp) {
    g->player.exp += exp;
    while (g->player.exp >= g->player.level * 20) {
        g->player.exp -= g->player.level * 20;
        g->player.level++;
        g->player.maxHp += 10;
        g->player.hp += 10;
        g->player.atk += 2;
        addMessage(g, "Level up! You are now level " +
                   to_string(g->player.level) + " (+10 HP, +2 ATK).");
    }
}

// Handle rewards when a monster dies; killing the boss wins the game.
// Input: game state, the dead monster. Output: none.
static void onMonsterKilled(GameState *g, Monster *m) {
    g->player.gold += m->goldReward;
    addMessage(g, string(m->isBoss ? "The BOSS is defeated!" : "Monster slain!") +
               " +" + to_string(m->goldReward) + " gold, +" +
               to_string(m->expReward) + " exp.");
    if (!m->isBoss && randInt(1, 4) == 1) {   // 25% random bonus drop
        g->player.potions++;
        addMessage(g, "It dropped a potion!");
    }
    gainExp(g, m->expReward);
    if (m->isBoss)
        g->victory = true;
}

// Fire the random event of a mysterious shrine.
// Input: game state. Output: none (updates player / log).
static void triggerShrine(GameState *g) {
    int roll = randInt(1, 4);
    if (roll == 1) {
        int heal = 20;
        g->player.hp += heal;
        if (g->player.hp > g->player.maxHp)
            g->player.hp = g->player.maxHp;
        addMessage(g, "The shrine blesses you: +" + to_string(heal) + " HP.");
    } else if (roll == 2) {
        int dmg = randInt(5, 12);
        g->player.hp -= dmg;
        addMessage(g, "The shrine curses you: -" + to_string(dmg) + " HP!");
        checkPlayerDeath(g);
    } else if (roll == 3) {
        g->player.potions++;
        addMessage(g, "The shrine grants you a potion.");
    } else {
        if (randomFloorTile(g->map, g->player.x, g->player.y))
            addMessage(g, "The shrine teleports you somewhere else!");
    }
}

// Apply the effect of the tile the player is standing on, then
// consume it if it was an item/trap/shrine/stairs.
// Input: game state. Output: none.
static void applyTile(GameState *g) {
    char c = g->map->tiles[g->player.y][g->player.x];
    if (c == '!') {
        g->player.potions++;
        addMessage(g, "You pick up a potion.");
        g->map->tiles[g->player.y][g->player.x] = '.';
    } else if (c == '$') {
        int gold = randInt(5, 20);
        g->player.gold += gold;
        addMessage(g, "You pick up " + to_string(gold) + " gold.");
        g->map->tiles[g->player.y][g->player.x] = '.';
    } else if (c == '^') {
        int dmg = randInt(5, 15);
        g->player.hp -= dmg;
        addMessage(g, "You step on a trap! -" + to_string(dmg) + " HP.");
        g->map->tiles[g->player.y][g->player.x] = '.';
        checkPlayerDeath(g);
    } else if (c == '?') {
        addMessage(g, "You touch the mysterious shrine...");
        g->map->tiles[g->player.y][g->player.x] = '.';
        triggerShrine(g);
    } else if (c == '>') {
        g->floor++;
        setupFloor(g);
        addMessage(g, "You descend to floor " + to_string(g->floor) + ".");
    }
}

// Try to move the player by (dx, dy). Bumping a monster starts a fight.
// Input: game state, direction delta.
// Output: true if the move spent a full turn (monsters should act).
static bool tryMove(GameState *g, int dx, int dy) {
    int nx = g->player.x + dx;
    int ny = g->player.y + dy;
    if (!isWalkable(g->map, nx, ny)) {
        addMessage(g, "You bump into a wall.");
        return false;
    }
    Monster *m = monsterAt(g->monsters, nx, ny);
    if (m != NULL) {
        bool died;
        addMessage(g, fight(g->player, m, died));
        if (died) {
            onMonsterKilled(g, m);
            removeMonster(g->monsters, m);
            g->player.x = nx;
            g->player.y = ny;
            applyTile(g);
        }
        checkPlayerDeath(g);
        return false;   // the fight itself was this turn
    }
    g->player.x = nx;
    g->player.y = ny;
    applyTile(g);
    return true;
}

// Drink one potion to restore HP.
// Input: game state. Output: true if a potion was actually used.
static bool usePotion(GameState *g) {
    if (g->player.potions <= 0) {
        addMessage(g, "You have no potions left.");
        return false;
    }
    if (g->player.hp >= g->player.maxHp) {
        addMessage(g, "You are already at full HP.");
        return false;
    }
    g->player.potions--;
    int heal = 30;
    g->player.hp += heal;
    if (g->player.hp > g->player.maxHp)
        g->player.hp = g->player.maxHp;
    addMessage(g, "You drink a potion: +" + to_string(heal) + " HP.");
    return true;
}

// Every live monster takes one action: attack when adjacent to the
// player, otherwise wander one step towards the player (randomly).
// Input: game state. Output: none.
static void monstersAct(GameState *g) {
    for (Monster *m = g->monsters; m != NULL; m = m->next) {
        int dist = abs(m->x - g->player.x) + abs(m->y - g->player.y);
        if (dist == 1) {
            int dmg = rollDamage(m->atk);
            g->player.hp -= dmg;
            addMessage(g, string(m->isBoss ? "The BOSS" : "A monster") +
                       " hits you for " + to_string(dmg) + " damage.");
            checkPlayerDeath(g);
            if (g->gameOver)
                return;
            continue;
        }
        if (randInt(0, 1) == 0)
            continue;   // 50% chance to stay put
        // Step towards the player, trying the longer axis first.
        int dx = (g->player.x > m->x) - (g->player.x < m->x);
        int dy = (g->player.y > m->y) - (g->player.y < m->y);
        int cand[2][2] = { {m->x + dx, m->y}, {m->x, m->y + dy} };
        if (randInt(0, 1) == 1) {   // randomize which axis to try first
            cand[0][0] = m->x;      cand[0][1] = m->y + dy;
            cand[1][0] = m->x + dx; cand[1][1] = m->y;
        }
        for (int i = 0; i < 2; i++) {
            int tx = cand[i][0], ty = cand[i][1];
            if (tx == m->x && ty == m->y)
                continue;
            if (!isWalkable(g->map, tx, ty))
                continue;
            if (monsterAt(g->monsters, tx, ty) != NULL)
                continue;
            if (tx == g->player.x && ty == g->player.y)
                continue;
            m->x = tx;
            m->y = ty;
            break;
        }
    }
}

void gameLoop(GameState *g) {
    while (!g->gameOver && !g->victory && !g->quit) {
        render(g);
        cout << "> ";
        char cmd;
        if (!(cin >> cmd)) {        // EOF: auto-save and leave
            saveGame(g);
            g->quit = true;
            break;
        }
        bool turnSpent = false;
        switch (cmd) {
            case 'w': turnSpent = tryMove(g, 0, -1); break;
            case 's': turnSpent = tryMove(g, 0, 1);  break;
            case 'a': turnSpent = tryMove(g, -1, 0); break;
            case 'd': turnSpent = tryMove(g, 1, 0);  break;
            case 'i': turnSpent = usePotion(g);      break;
            case 'q':
                if (saveGame(g))
                    addMessage(g, "Game saved.");
                else
                    addMessage(g, "Could not write the save file!");
                g->quit = true;
                break;
            default:
                addMessage(g, "Unknown command.");
                break;
        }
        if (turnSpent && !g->gameOver && !g->victory && !g->quit)
            monstersAct(g);
    }
}
