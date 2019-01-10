#ifndef LEX_H
#define LEX_H

#include "num.h"

/*
 * Interface for managing with lexers, which are used to extract numbers from
 * input
 */

/*
 * Type of lexer
 */
typedef uintptr_t lex;

/*
 * Allocates and initializes lexer's memory structures. If succeeds, copies the
 * lexer to the address `p`. Returns status of the operation.
 */
int
lex_create(lex *p);

/*
 * Deallocates memory structures of the lexer `l`
 */
void
lex_destroy(lex l);

/*
 * Feeds the symbol with code `c` to the lexer `l` and returns status of the
 * operation
 */
int
lex_feed(lex l, int c);

/*
 * Flushes a number from buffer of the lexer `l`. If succeeds, copies the
 * number to the address `p`. Returns status of the operation.
 */
int
lex_flush(lex l, num *p);

#endif  /* #ifndef LEX_H */
