#include <fstream>
#include <iostream>
#include <string>
#include "save.h"
#include "util.h"

using namespace std;

bool saveGame(const GameState *g) {
    ofstream out("savegame.txt");
    if (!out)
        return false;
    const Player &p = g->player;
    out << "diffindex " << g->diffIndex << "\n";
    out << "floor " << g->floor << "\n";
    out << "player " << p.x << ' ' << p.y << ' ' << p.hp << ' ' << p.maxHp
        << ' ' << p.atk << ' ' << p.gold << ' ' << p.potions
        << ' ' << p.level << ' ' << p.exp << "\n";
    out << "map " << g->map->width << ' ' << g->map->height << "\n";
    for (int y = 0; y < g->map->height; y++) {
        for (int x = 0; x < g->map->width; x++)
            out << g->map->tiles[y][x];
        out << "\n";
    }
    int count = 0;
    for (Monster *m = g->monsters; m != NULL; m = m->next)
        count++;
    out << "monsters " << count << "\n";
    for (Monster *m = g->monsters; m != NULL; m = m->next)
        out << m->x << ' ' << m->y << ' ' << m->hp << ' ' << m->maxHp
            << ' ' << m->atk << ' ' << m->expReward << ' ' << m->goldReward
            << ' ' << (m->isBoss ? 1 : 0) << "\n";
    return (bool)out;
}

GameState* loadGame() {
    ifstream in("savegame.txt");
    if (!in)
        return NULL;

    GameState *g = new GameState;
    g->monsters = NULL;
    g->map = NULL;
    g->gameOver = false;
    g->victory = false;
    g->quit = false;

    string tag;
    int di = 2;
    if (!(in >> tag >> di) || tag != "diffindex") {
        delete g;
        return NULL;
    }
    g->diffIndex = di;
    g->diff = getDifficulty(di);

    in >> tag >> g->floor;
    Player &p = g->player;
    in >> tag >> p.x >> p.y >> p.hp >> p.maxHp >> p.atk
       >> p.gold >> p.potions >> p.level >> p.exp;

    int w, h;
    in >> tag >> w >> h;
    if (!in || w <= 0 || h <= 0 || w > 500 || h > 500) {
        delete g;
        return NULL;
    }
    g->map = createMap(w, h);
    string row;
    getline(in, row);   // consume the rest of the "map w h" line
    for (int y = 0; y < h; y++) {
        if (!getline(in, row)) {
            freeGame(g);
            return NULL;
        }
        for (int x = 0; x < w && x < (int)row.size(); x++)
            g->map->tiles[y][x] = row[x];
    }

    int n = 0;
    in >> tag >> n;
    for (int i = 0; i < n; i++) {
        int x, y, hp, maxHp, atk, expR, goldR, boss;
        if (!(in >> x >> y >> hp >> maxHp >> atk >> expR >> goldR >> boss)) {
            freeGame(g);
            return NULL;
        }
        addMonster(g->monsters,
                   createMonster(x, y, hp, atk, expR, goldR, boss == 1));
        g->monsters->maxHp = maxHp;
    }
    if (!in) {
        freeGame(g);
        return NULL;
    }
    addMessage(g, "Save file loaded. Welcome back!");
    return g;
}

void recordScore(const char *result, const GameState *g) {
    ofstream out("highscores.txt", ios::app);
    if (!out)
        return;
    out << result << ' ' << g->diff.name << ' ' << g->floor << ' '
        << g->player.level << ' ' << g->player.gold << "\n";
}

// One line of the high-score file.
struct ScoreEntry {
    string result;
    string diff;
    int floor, level, gold;
};

void showHighscores() {
    clearScreen();
    cout << "=== HIGH SCORES ===\n\n";
    ifstream in("highscores.txt");
    if (!in) {
        cout << "No records yet. Play a game first!\n";
        return;
    }
    // Read entries into a dynamically growing array.
    int cap = 8, n = 0;
    ScoreEntry *arr = new ScoreEntry[cap];
    string result, diff;
    int floor, level, gold;
    while (in >> result >> diff >> floor >> level >> gold) {
        if (n == cap) {
            int newCap = cap * 2;
            ScoreEntry *bigger = new ScoreEntry[newCap];
            for (int i = 0; i < n; i++)
                bigger[i] = arr[i];
            delete[] arr;
            arr = bigger;
            cap = newCap;
        }
        arr[n].result = result;
        arr[n].diff = diff;
        arr[n].floor = floor;
        arr[n].level = level;
        arr[n].gold = gold;
        n++;
    }
    // Sort by gold, highest first (selection sort).
    for (int i = 0; i < n; i++) {
        int best = i;
        for (int j = i + 1; j < n; j++)
            if (arr[j].gold > arr[best].gold)
                best = j;
        ScoreEntry tmp = arr[i];
        arr[i] = arr[best];
        arr[best] = tmp;
    }
    if (n == 0) {
        cout << "No records yet. Play a game first!\n";
    } else {
        cout << "Rank  Result   Difficulty  Floor  Level  Gold\n";
        int shown = n < 10 ? n : 10;
        for (int i = 0; i < shown; i++) {
            cout << " " << (i + 1) << ".    "
                 << arr[i].result << "\t " << arr[i].diff
                 << "\t   " << arr[i].floor
                 << "\t " << arr[i].level
                 << "\t " << arr[i].gold << "\n";
        }
    }
    delete[] arr;
}
