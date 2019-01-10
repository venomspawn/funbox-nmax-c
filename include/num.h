#ifndef NUM_H
#define NUM_H

#include <stdint.h>
#include <stddef.h>

/*
 * Interface for managing with very long numbers
 */

/*
 * Type of number structures
 */
typedef uint64_t num;

/*
 * Maximum length of a number represented as a string, including leading zeroes
 */
#define NUM_MAX_LEN 1000

/*
 * Special value to represent absence of number
 */
#define NUM_NULL 0

/*
 * Allocates and initializes memory structures for a number, which is
 * represented by string of digits `s` of length `len` with `zeroes` leading
 * zeroes. If suceeded, copies the number to the address `p`. Returns status of
 * the operation.
 */
int
num_create(const char *s, size_t len, size_t zeroes, num *p);

/*
 * Deallocates memory structure of number `n`
 */
void
num_destroy(num n);

/*
 * Compares numbers `a` and `b`. Returns an integer less than, equal to, or
 * greater than zero if `a` is found, respectively, to be less than, to match,
 * or be greater than `b`.
 */
int
num_compare(num a, num b);

/*
 * Fills buffer `s` with digits of number `n` including leading zeroes. Assumes
 * that `s` has enough space.
 */
void
num_fill_string(char *s, num n);

#endif /* #ifndef NUM_H */
