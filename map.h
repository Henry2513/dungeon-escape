#ifndef MAP_H
#define MAP_H

struct Monster;   // defined in entity.h

// A dungeon floor: a dynamically allocated 2D grid of characters.
// '#' wall, '.' floor, '>' stairs down, '!' potion, '$' gold,
// '^' trap, '?' mysterious shrine.
struct Map {
    int width, height;
    char **tiles;   // tiles[y][x], heap-allocated
};

// Allocate a Map of the given size, filled with walls.
// Input: width and height. Output: pointer to the new heap Map.
Map* createMap(int width, int height);

// Free a Map and all of its rows, and set the pointer to NULL.
// Input: map pointer (by reference). Output: none.
void freeMap(Map* &m);

// Carve a random dungeon: several random rooms connected by corridors.
// Input: map to carve, number of rooms to attempt. Output: none.
void generateMap(Map *m, int roomCount);

// Check whether (x, y) lies inside the map bounds.
// Input: map, coordinates. Output: true if inside.
bool isInside(const Map *m, int x, int y);

// Check whether (x, y) can be walked on (anything but a wall).
// Input: map, coordinates. Output: true if walkable.
bool isWalkable(const Map *m, int x, int y);

// Pick a random floor ('.') cell.
// Input: map, output references x and y.
// Output: true on success (x, y hold the cell), false if none found.
bool randomFloorTile(const Map *m, int &x, int &y);

// Place item character ch on a random floor cell.
// Input: map, item character. Output: true on success.
bool placeRandom(Map *m, char ch);

// Draw the map with the player and the monsters overlaid, in color.
// Input: map, player position, monster list. Output: none (prints).
void renderMap(const Map *m, int playerX, int playerY, const Monster *monsters);

#endif
