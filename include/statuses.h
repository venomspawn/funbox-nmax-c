#ifndef STATUSES_H
#define STATUSES_H

/*
 * Contains status codes used by operations and routines. One can assume that
 * success code is always zero, error codes are always negative and
 * informational codes are always (strictly) positive.
 */

#define SUCCESS            0
#define NOT_ENOUGH_MEMORY -1
#define INVALID_ADDRESS   -2
#define TOO_LONG          -3

#endif /* #ifndef STATUSES_H */
