/*
 * Provides random numbers
 */

#ifndef RANDOM_H
#define RANDOM_H

int rnd();

int rnd_range(int low, int high);

extern int rnd_seed;

#endif