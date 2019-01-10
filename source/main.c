#include "../include/lex.h"
#include "../include/set.h"
#include "../include/statuses.h"
#include "../include/version.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char buf[1001] = {};

static void
putn(num n)
{
	num_fill_string(buf, n);
	puts(buf);
}

static int
exit_lexer_creation(int r)
{
	fprintf(stderr, "Error during lexer creation: %d\n", r);
	return EXIT_FAILURE;
}

static int
exit_set_creation(int r)
{
	fprintf(stderr, "Error during set creation: %d\n", r);
	return EXIT_FAILURE;
}

static int
exit_processing(int r)
{
	fprintf(stderr, "Error during processing: %d\n", r);
	return EXIT_FAILURE;
}

int
main(int argc, const char **argv)
{
	int capacity = argc < 1 ? 100 : atoi(argv[1]);
	if (capacity <= 0)
		capacity = 100;

	int r;
	lex l;

	r = lex_create(&l);
	if (r < 0)
		return exit_lexer_creation(r);

	set s;

	r = set_create((size_t) capacity, &s);
	if (r < 0)
		return exit_set_creation(r);

	int c;
	num n;

	do {
		c = getchar();
		r = lex_feed(l, c);
		if (r < 0) {
			return exit_processing(r);
		} else if (NUMBER_READY == r) {
			r = lex_flush(l, &n);
			if (r < 0)
				return exit_processing(r);
			set_add(s, n);
		}
	} while (c != EOF);

	set_iterate(s, &putn);

	set_destroy(s);
	lex_destroy(l);

	return EXIT_SUCCESS;
}
