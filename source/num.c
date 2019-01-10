#include "../include/num.h"
#include "../include/statuses.h"
#include <stdlib.h>
#include <string.h>

/*
 * Long numbers are implemented in the following way.
 *
 * 1.  Main value is 64-bit unsigned integer, which contains information on
 *     number length, number of leading zeroes and digits data. There are two
 *     modes of digits data to be contained in the main value:
 *     -   `in`-mode, which is used for strings with short (<= 12) length,
 *         providing data right from the main value;
 *     -   `ex`-mode for long string, providing data by memory address encoded
 *         into main value.
 *
 * 2.  The modes are controlled by flag value of the least significant bit. The
 *     actual bitmap for the modes is as following:
 *
 *     -   `in`-mode:
 *         0 1   6     16   64
 *         +-+---+------+----+
 *         |1|len|zeroes|data|
 *         +-+---+------+----+
 *
 *         Here `len` means length of used bytes in `data` block, `zeroes`
 *         means number of leading zeroes and `data` means data block. The
 *         implementation assumes little-endianness (which is supported by x86
 *         and x86-64 architectures) of the main value to get a pointer on the
 *         data block.
 *
 *     -   `ex`-mode:
 *         0 1  10     20      64
 *         +-+---+------+-------+
 *         |0|len|zeroes|pointer|
 *         +-+---+------+-------+
 *
 *         Here `len` means length of used bytes in data block, `zeroes` means
 *         number of leading zeroes and `pointer` is address value of data
 *         block. The implementation assumes (but still checks) that pointers
 *         use no more than 48 bits. 16-bytes data alignment used for
 *         allocation cuts 4 least significant bits and allows to use 44 bits
 *         only.
 *
 * 3.  In either mode numbers are represented by packed digits data. Every pair
 *     of digits is packed into single byte: most significant goes into higher
 *     4 bits, least significant — into lesser 4 bits. The only special case is
 *     the most significant digit of a number of an odd length: the digit is
 *     packed into lesser 4 bits of a single byte. This method allows to reduce
 *     comparison of the numbers to comparison of data lengths and, if the data
 *     lengths are equal, to comparison of data as strings. Also it effectively
 *     halves required memory.
 *
 *     Examples:
 *     -   `123` ~> 0x01 0x23
 *     -   `4567` ~> 0x45 0x67
 */

#define NUM_RSA(u, s, a) ((size_t) (((u) >> (s)) & (a)))
#define NUM_MASK(bits) ((1ULL << (bits)) - 1)

#define NUM_IN_FLAG_BITS 1
#define NUM_IN_FLAG_MASK (NUM_MASK(NUM_IN_FLAG_BITS))
#define NUM_IN_FLAG(n) (n & NUM_IN_FLAG_MASK)
#define NUM_IN_MAX_LEN 12
#define NUM_IN_LEN_POS 1
#define NUM_IN_LEN_BITS 5
#define NUM_IN_LEN_MASK (NUM_MASK(NUM_IN_LEN_BITS))
#define NUM_IN_MAX_LEN 12
#define NUM_IN_LEN(n) (NUM_RSA(n, NUM_IN_LEN_POS, NUM_IN_LEN_MASK))
#define NUM_IN_ZEROES_POS (NUM_IN_LEN_POS + NUM_IN_LEN_BITS)
#define NUM_IN_ZEROES_BITS 10
#define NUM_IN_ZEROES_MASK (NUM_MASK(NUM_IN_ZEROES_BITS))
#define NUM_IN_ZEROES(n) (NUM_RSA(n, NUM_IN_ZEROES_POS, NUM_IN_ZEROES_MASK))
#define NUM_IN_DATA(n) (((uint8_t *) &n) + 2)
#define NUM_IN(l, z) (((num) (z) << NUM_IN_ZEROES_POS) | \
                      ((num) (l) << NUM_IN_LEN_POS) | \
                      ((num) 1))

#define NUM_EX_LEN_POS 1
#define NUM_EX_LEN_BITS 9
#define NUM_EX_LEN_MASK (NUM_MASK(NUM_EX_LEN_BITS))
#define NUM_EX_LEN(n) (NUM_RSA(n, NUM_EX_LEN_POS, NUM_EX_LEN_MASK))
#define NUM_EX_ZEROES_BITS 10
#define NUM_EX_ZEROES_POS (NUM_EX_LEN_POS + NUM_EX_ZEROES_BITS)
#define NUM_EX_ZEROES_MASK (NUM_MASK(NUM_EX_ZEROES_BITS))
#define NUM_EX_ZEROES(n) (NUM_RSA(n, NUM_EX_ZEROES_POS, NUM_EX_ZEROES_MASK))
#define NUM_EX_DATA_ZEROES 4
#define NUM_EX_DATA_BITS 48
#define NUM_EX_DATA_POS (NUM_EX_ZEROES_POS + NUM_EX_ZEROES_BITS - \
                         NUM_EX_DATA_ZEROES)
#define NUM_EX_DATA_ALIGNMENT (1 << NUM_EX_DATA_ZEROES)
#define NUM_EX_DATA_MAX_ADDRESS 0x0000FFFFFFFFFFFFULL
#define NUM_EX_DATA_MASK (NUM_MASK(NUM_EX_DATA_BITS) - \
                          NUM_MASK(NUM_EX_DATA_ZEROES))
#define NUM_EX_DATA(n) ((uint8_t *) \
                        NUM_RSA(n, NUM_EX_DATA_POS, NUM_EX_DATA_MASK))

#define NUM_EX(s, l, z) (((num) (s) << NUM_EX_DATA_POS) | \
		         ((num) (z) << NUM_EX_ZEROES_POS) | \
		         ((num) (l) << NUM_EX_LEN_POS))

#define NUM_DIGIT_VALUE(d) ((uint8_t) ((d) - '0'))
#define NUM_DATA_SIZE(l) (((l) / 2) + ((l) & 1))

#define NUM_LEN(n) (NUM_IN_FLAG(n) ? NUM_IN_LEN(n) : NUM_EX_LEN(n))
#define NUM_ZEROES(n) (NUM_IN_FLAG(n) ? NUM_IN_ZEROES(n) : NUM_EX_ZEROES(n))
#define NUM_DATA(n) (NUM_IN_FLAG(n) ? NUM_IN_DATA(n) : NUM_EX_DATA(n))

static void
num_pack(const char *s, size_t len, uint8_t *data)
{
	if (0 == len)
		return;

	int i = len & 1;
	int j;

	if (0 < i)
		data[0] = NUM_DIGIT_VALUE(s[0]);

	for (j = i; i < len; j++, i += 2)
		data[j] = (NUM_DIGIT_VALUE(s[i]) << 4) |
		           NUM_DIGIT_VALUE(s[i + 1]);
}

static int
num_create_in(const char *s, size_t len, size_t zeroes, num *p)
{
	num n = NUM_IN(NUM_DATA_SIZE(len), zeroes);
	uint8_t *data = NUM_IN_DATA(n);

	num_pack(s, len, data);

	*p = n;

	return SUCCESS;
}

static int
num_create_ex(const char *s, size_t len, size_t zeroes, num *p)
{
	uint8_t *data;
	size_t l = NUM_DATA_SIZE(len);
	int err;

	err = posix_memalign((void **) &data, NUM_EX_DATA_ALIGNMENT, l);
	if (err)
		return NOT_ENOUGH_MEMORY;

	if ((uint64_t) data > NUM_EX_DATA_MAX_ADDRESS) {
		free(data);
		return INVALID_ADDRESS;
	}

	num_pack(s, len, data);

	*p = NUM_EX(data, l, zeroes);

	return SUCCESS;
}

int
num_create(const char *s, size_t len, size_t zeroes, num *p)
{
	if (len + zeroes > NUM_MAX_LEN)
		return TOO_LONG;

	if (len <= NUM_IN_MAX_LEN)
		return num_create_in(s, len, zeroes, p);
	else
		return num_create_ex(s, len, zeroes, p);

}

void
num_destroy(num n)
{
	if (NUM_NULL == n || NUM_IN_FLAG(n))
		return;

	void *p = NUM_EX_DATA(n);
	free(p);
}

int
num_compare(num a, num b)
{
	size_t len_a = NUM_LEN(a);
	size_t len_b = NUM_LEN(b);

	if (len_a < len_b)
		return -1;
	if (len_a > len_b)
		return 1;

	uint8_t *data_a = NUM_DATA(a);
	uint8_t *data_b = NUM_DATA(b);

	return strncmp((char *) data_a, (char *) data_b, len_a);
}

#define NUM_VALUE_DIGIT(v) ((v) + '0')

void
num_fill_string(char *buf, num n)
{
	size_t zeroes = NUM_ZEROES(n);
	size_t len = NUM_LEN(n);
	uint8_t *data = NUM_DATA(n);

	memset(buf, '0', zeroes);

	if (0 == len) {
		buf[zeroes] = 0;
		return;
	}

	uint8_t v = data[0];
	int i = 0;
	int j = zeroes;

	if (v < 0x0F) {
		buf[j] = NUM_VALUE_DIGIT(v);
		j++;
		i++;
	}

	while (i < len) {
		v = data[i++];
		buf[j++] = NUM_VALUE_DIGIT(v >> 4);
		buf[j++] = NUM_VALUE_DIGIT(v & 0x0F);
	}

	buf[j] = 0;
}
