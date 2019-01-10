#ifndef SET_H
#define SET_H

#include "num.h"

/*
 * Interface for managing with sets with the following properties:
 * 1) elements of the sets are numbers;
 * 2) sets are ordered by elements descension;
 * 3) sets are of limited capacity;
 * 4) addition of an element keeps set's order and excludes minimal element if
 *    the set is full.
 */

/*
 * Type of set
 */
typedef uintptr_t set;

/*
 * Allocates and initializes set's memory structures. Sets maximum capacity of
 * the set to `cap`. If succeeds, copies the set to the address `p`. Returns
 * status of the operation.
 */
int
set_create(size_t cap, set *p);

/*
 * Deallocates memory structures of the set `s`
 */
void
set_destroy(set s);

/*
 * Adds number `n` to the set `s` keeping order. If the set is full, excludes
 * minimum element.
 */
void
set_add(set s, num n);

/*
 * Iterates over items of the set `s` in set's order and calls `iter` for every
 * item
 */
void
set_iterate(set s, void (*iter)(num));

#endif /* #ifndef SET_H */
