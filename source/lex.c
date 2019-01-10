#include "../include/lex.h"
#include "../include/statuses.h"
#include <stdlib.h>

struct lex_c {
	char  *buf;
	size_t len;
	size_t zeroes;
};

int
lex_create(lex *p)
{
	struct lex_c *lp = malloc(sizeof(*lp));
	if (NULL == lp)
		return NOT_ENOUGH_MEMORY;

	char *buf = malloc(sizeof(*buf) * NUM_MAX_LEN);
	if (NULL == buf) {
		free(lp);
		return NOT_ENOUGH_MEMORY;
	}

	lp->buf = buf;
	lp->len = 0;
	lp->zeroes = 0;

	*p = (lex) lp;

	return SUCCESS;
}

void
lex_destroy(lex l)
{
	struct lex_c *lp = (struct lex_c *) l;
	free(lp->buf);
	free(lp);
}

static int
lex_feed_digit(struct lex_c *lp, int c)
{
	if ('0' == c && 0 == lp->len)
		lp->zeroes++;
	else
		lp->buf[lp->len++] = c;

	if (NUM_MAX_LEN <= lp->len + lp->zeroes)
		return NUMBER_READY;
	else
		return SUCCESS;
}

int
lex_feed(lex l, int c)
{
	struct lex_c *lp = (struct lex_c *) l;

	if ('0' <= c && c <= '9')
		return lex_feed_digit(lp, c);
	else if (0 == lp->len + lp->zeroes)
		return SUCCESS;
	else
		return NUMBER_READY;
}

int
lex_flush(lex l, num *p)
{
	struct lex_c *lp = (struct lex_c *) l;
	num n;
	int c;

	c = num_create(lp->buf, lp->len, lp->zeroes, &n);
	if (c < 0)
		return c;

	lp->len = 0;
	lp->zeroes = 0;

	*p = n;

	return SUCCESS;
}
