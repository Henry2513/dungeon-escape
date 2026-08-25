# Dungeon Escape

A text-based roguelike dungeon crawler that runs in the terminal. Explore
procedurally generated dungeon floors, fight monsters, grab potions and gold,
dodge traps, trigger mysterious shrines — and defeat the boss waiting on the
final floor.

## Features

- **Procedurally generated dungeons** — every floor is built from random
  rooms connected by corridors, so no two runs are the same.
- **Three difficulty levels** — Easy (3 floors), Normal (5 floors) and
  Hard (7 floors) with different map sizes, monster strength and player HP.
- **Turn-based combat** — bump into a monster to trade blows. Damage rolls
  are randomized, monsters grow stronger on deeper floors, and kills grant
  experience, gold and the occasional potion drop.
- **Character progression** — level up to gain max HP and attack power.
- **Random events** — traps deal damage, mysterious shrines bless you, curse
  you, hand you a potion or teleport you across the map.
- **Living dungeon** — after every move, monsters wander towards you and
  attack once they stand next to you.
- **Save / load** — press `q` to save the exact game state (map, monsters,
  stats) to `savegame.txt` and continue later.
- **High scores** — every finished run is recorded in `highscores.txt` and
  the top 10 runs are shown from the main menu.
- **Colorful ANSI rendering** — the dungeon is drawn with terminal colors;
  no external libraries are needed.

## Build

With `make`:

```sh
make
```

Or compile manually:

```sh
g++ -std=c++11 -Wall -Wextra -o game main.cpp game.cpp map.cpp entity.cpp save.cpp util.cpp
```

Only a C++11 compiler and the standard library are required — there are no
third-party dependencies.

## Run

```sh
./game        # Linux / macOS / Git Bash
game.exe      # Windows
```

For the colored display, use a terminal that understands ANSI escape codes
(any modern Linux terminal, macOS Terminal, Windows Terminal, Git Bash).

## How to play

- `w` / `a` / `s` / `d` — move up / left / down / right. Moving into a
  monster attacks it.
- `i` — drink a potion (restores 30 HP).
- `q` — save the game and return to the main menu.

Map legend:

| Symbol | Meaning                              |
| ------ | ------------------------------------ |
| `@`    | you                                  |
| `M`    | monster                              |
| `B`    | boss (guards the final floor)        |
| `#`    | wall                                 |
| `.`    | floor                                |
| `>`    | stairs down to the next floor        |
| `!`    | potion                               |
| `$`    | gold                                 |
| `^`    | trap                                 |
| `?`    | mysterious shrine (random event)     |

**Goal:** fight your way down to the final floor and slay the boss. If your
HP reaches zero, the run ends. Both endings are recorded on the high-score
board.

## Project layout

| File                 | Contents                                              |
| -------------------- | ----------------------------------------------------- |
| `main.cpp`           | main menu, difficulty selection, run wrap-up          |
| `game.h` / `game.cpp`| game state, main loop, combat turns, events, leveling |
| `map.h` / `map.cpp`  | dynamic 2D map, dungeon generation, rendering         |
| `entity.h` / `entity.cpp` | player/monster structs, monster linked list, damage rolls |
| `save.h` / `save.cpp`| save/load file I/O, high-score table                  |
| `util.h` / `util.cpp`| random number helpers, screen clearing                |
| `Makefile`           | build, `make run`, `make clean`                       |

## Notes

- `savegame.txt` and `highscores.txt` are created next to the executable
  when you save or finish a run. The save file is deleted once a run ends
  in victory or defeat.
