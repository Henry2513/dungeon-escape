#include <iostream>
#include <string>
#include "map.h"
#include "entity.h"
#include "util.h"

using namespace std;

Map* createMap(int width, int height) {
    Map *m = new Map;
    m->width = width;
    m->height = height;
    m->tiles = new char*[height];
    for (int y = 0; y < height; y++) {
        m->tiles[y] = new char[width];
        for (int x = 0; x < width; x++)
            m->tiles[y][x] = '#';
    }
    return m;
}

void freeMap(Map* &m) {
    if (m == NULL)
        return;
    for (int y = 0; y < m->height; y++)
        delete[] m->tiles[y];
    delete[] m->tiles;
    delete m;
    m = NULL;
}

void generateMap(Map *m, int roomCount) {
    int prevCx = -1, prevCy = -1;
    for (int i = 0; i < roomCount; i++) {
        // Random room, always at least one cell away from the border.
        int rw = randInt(4, 8);
        int rh = randInt(3, 6);
        if (rw >= m->width - 2 || rh >= m->height - 2)
            continue;
        int rx = randInt(1, m->width - rw - 1);
        int ry = randInt(1, m->height - rh - 1);
        for (int y = ry; y < ry + rh; y++)
            for (int x = rx; x < rx + rw; x++)
                m->tiles[y][x] = '.';

        // Connect this room's center to the previous room's center
        // with an L-shaped corridor so the whole dungeon is reachable.
        int cx = rx + rw / 2;
        int cy = ry + rh / 2;
        if (prevCx != -1) {
            int x0 = cx < prevCx ? cx : prevCx;
            int x1 = cx < prevCx ? prevCx : cx;
            for (int x = x0; x <= x1; x++)
                m->tiles[prevCy][x] = '.';
            int y0 = cy < prevCy ? cy : prevCy;
            int y1 = cy < prevCy ? prevCy : cy;
            for (int y = y0; y <= y1; y++)
                m->tiles[y][cx] = '.';
        }
        prevCx = cx;
        prevCy = cy;
    }
}

bool isInside(const Map *m, int x, int y) {
    return x >= 0 && x < m->width && y >= 0 && y < m->height;
}

bool isWalkable(const Map *m, int x, int y) {
    return isInside(m, x, y) && m->tiles[y][x] != '#';
}

bool randomFloorTile(const Map *m, int &x, int &y) {
    for (int tries = 0; tries < 2000; tries++) {
        int rx = randInt(1, m->width - 2);
        int ry = randInt(1, m->height - 2);
        if (m->tiles[ry][rx] == '.') {
            x = rx;
            y = ry;
            return true;
        }
    }
    // Fallback: scan for the first floor cell.
    for (int ry = 0; ry < m->height; ry++)
        for (int rx = 0; rx < m->width; rx++)
            if (m->tiles[ry][rx] == '.') {
                x = rx;
                y = ry;
                return true;
            }
    return false;
}

bool placeRandom(Map *m, char ch) {
    int x, y;
    if (!randomFloorTile(m, x, y))
        return false;
    m->tiles[y][x] = ch;
    return true;
}

// Return the monster at (x, y) without modifying the list.
static const Monster* findMonster(const Monster *head, int x, int y) {
    for (const Monster *m = head; m != NULL; m = m->next)
        if (m->x == x && m->y == y)
            return m;
    return NULL;
}

void renderMap(const Map *m, int playerX, int playerY, const Monster *monsters) {
    const string RESET = "\033[0m";
    for (int y = 0; y < m->height; y++) {
        string row;
        for (int x = 0; x < m->width; x++) {
            if (x == playerX && y == playerY) {
                row += "\033[96m@\033[0m";          // player: bright cyan
                continue;
            }
            const Monster *mon = findMonster(monsters, x, y);
            if (mon != NULL) {
                row += mon->isBoss ? "\033[91mB\033[0m"   // boss: bright red
                                   : "\033[31mM\033[0m";  // monster: red
                continue;
            }
            char c = m->tiles[y][x];
            switch (c) {
                case '#': row += "\033[90m#\033[0m";  break;  // wall: gray
                case '>': row += "\033[95m>\033[0m";  break;  // stairs: magenta
                case '!': row += "\033[92m!\033[0m";  break;  // potion: green
                case '$': row += "\033[93m$\033[0m";  break;  // gold: yellow
                case '^': row += "\033[31m^\033[0m";  break;  // trap: red
                case '?': row += "\033[94m?\033[0m";  break;  // shrine: blue
                default:  row += c;                   break;  // floor
            }
        }
        cout << row << "\n";
    }
    (void)RESET;
}
