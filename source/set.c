#include "../include/set.h"
#include "../include/statuses.h"
#include <stdlib.h>

struct set_c {
	num   *items;
	num    min;
	size_t cap;
	size_t size;
};

int
set_create(size_t cap, set *p)
{
	struct set_c *ps = malloc(sizeof(*ps));
	if (NULL == ps)
		return NOT_ENOUGH_MEMORY;

	num *items = malloc(sizeof(*items) * cap);
	if (NULL == items) {
		free(ps);
		return NOT_ENOUGH_MEMORY;
	}

	ps->items = items;
	ps->min = NUM_NULL;
	ps->cap = cap;
	ps->size = 0;

	*p = (set) ps;

	return SUCCESS;
}

void
set_destroy(set s)
{
	struct set_c *ps = (struct set_c *) s;
	num *items = ps->items;
	size_t size = ps->size;
	int i;

	for (i = 0; i < size; i++)
		num_destroy(items[i]);

	free(items);
	free(ps);
}

static int
set_index(const struct set_c *ps, num n)
{
	num *items = ps->items;
	int l = 0;
	int r = (int) ps->size;
	int m, c;

	while (l < r) {
		m = (l + r) / 2;
		c = num_compare(n, items[m]);
		if (0 == c)
			return -1;
		if (0 < c)
			r = m;
		else
			l = m + 1;
	}

	return l;
}

static void
set_insert(struct set_c *ps, int index, num n)
{
	num *items = ps->items;

	if (ps->size < ps->cap)
		ps->size++;
	else
		num_destroy(ps->min);

	size_t size = ps->size;
	int i;

	for (i = size - 1; index < i; i--)
		items[i] = items[i - 1];
	items[index] = n;

	ps->min = items[size - 1];
}

void
set_add(set s, num n)
{
	struct set_c *ps = (struct set_c *) s;

	if (ps->cap <= ps->size && num_compare(n, ps->min) <= 0)
		return;

	int index = set_index(ps, n);
	if (index < 0)
		return;

	set_insert(ps, index, n);
}

void
set_iterate(set s, void (*iter)(num))
{
	struct set_c *ps = (struct set_c *) s;
	num *items = ps->items;
	size_t size = ps->size;
	int i;

	for (i = size - 1; 0 <= i; i--)
		iter(items[i]);
}
