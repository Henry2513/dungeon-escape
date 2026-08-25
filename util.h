#ifndef UTIL_H
#define UTIL_H

// Seed the random number generator with the current time.
// Input: none. Output: none (side effect on rand()).
void seedRandom();

// Return a random integer in [lo, hi], inclusive.
// Input: lower bound lo, upper bound hi (lo <= hi).
// Output: a random int between lo and hi.
int randInt(int lo, int hi);

// Clear the terminal screen using ANSI escape codes.
// Input: none. Output: none (writes escape codes to stdout).
void clearScreen();

#endif
