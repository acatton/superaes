/* Copyright (c) 2012, Antoine Catton and Jessy Mauclair-Richalet
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "convert.h"
#include "key.h"
#include "tables.h"

#define BUFFER_SIZE WORD_SIZE_IN_INT8

#define is_valid_key_length_bits(l)   \
    ((l == 320) || (l == 480) || (l == 640))

#define is_valid_key_length_bytes(l)  \
    is_valid_key_length_bits((l * BITS_IN_BYTE))

#define is_valid_key_length_buffers(l) \
    is_valid_key_length_bytes((l * BUFFER_SIZE))

/* Linked list of buffers */
struct buffer_list
{
    uint8_t             buffer[BUFFER_SIZE];
    struct buffer_list *next;
};

/* TODO: Key generation */
struct key *read_key(FILE *f)
{
    int                 n,
                        read,
                        error,
                        offset;
    struct buffer_list *list,
                       *current,
                       *next;
    struct key         *key;

    error = 0;

    list = malloc(sizeof(struct buffer_list));
    current = list;
    n = 0;
    while ((read = fread(current->buffer, sizeof(uint8_t), BUFFER_SIZE, f)) > 0) {
        if (read != BUFFER_SIZE) {
            fprintf(stderr, "error: key size is not a multiple of %d bytes.",
                    BUFFER_SIZE);
            error = 1;
            goto out;
        }
        current->next = malloc(sizeof(struct buffer_list));
        current = current->next;
        current->next = NULL;
        n++;
    }
    if (feof(f) == 0) {
        error = 1;
        perror("Reading key");
        goto out;
    }
    if (!is_valid_key_length_buffers(n)) {
        fprintf(stderr, "error: key size is not a valid length");
        error = 1;
        goto out;
    }

    key = malloc(sizeof(struct key));
    key->size = (BUFFER_SIZE / INT8_IN_INT16) * n;
    key->value = malloc(sizeof(uint16_t) * key->size);
    current = list;
    offset = 0;
    while ((next = current->next) != NULL) {
        uint8_array_to_uint16(current->buffer, BUFFER_SIZE,
                &(key->value[offset]));

        current = next;
        offset += (BUFFER_SIZE / INT8_IN_INT16);
    }

out:
    current = list;
    while ((next = current->next) != NULL) {
        free(current);
        current = next;
    }
    if (error)
        return NULL;
    return key;
}

void destroy_key(struct key *key)
{
    free(key->value);
    free(key);
}

/* +------------+------------------+ *
 * | Bit length | Number of rounds | *
 * +------------+------------------+ *
 * |    320     |       15         | *
 * |    480     |       30         | *
 * |    640     |       45         | *
 * +------------+------------------+ */
#define number_of_round(bit_length) \
    ((bit_length == 320) ? 15 : ((bit_length == 480) ? 30 : 45))

static void superaes_SubWord(uint16_t *word)
{
    int i;

    for (i = 0; i < WORD_SIZE; i++) {
        word[i] = subbytes_table[word[i]];
    }
}

static void superaes_RotWord(uint16_t *word)
{
    int      i;
    uint16_t tmp;

    tmp = word[0];
    for (i = 1; i < WORD_SIZE; i++)
        word[i-1] = word[i];
    word[i-1] = tmp;
}

static void superaes_Rcon(uint16_t *word, int n)
{
    word[0] ^= rcon[n];
}

struct key *superaes_KeyExpansion(struct key *in)
{
    int         i, j,
                n_word_key,
                n_word_out;
    struct key *out;
    int16_t     temp[WORD_SIZE];

    if (!is_valid_key_length_bits(in->size * BITS_IN_INT16))
        return NULL;

    out = malloc(sizeof(struct key));
    n_word_out = number_of_round(in->size * BITS_IN_INT16) * BLOCK_SIZE_IN_WORD;
    out->size = n_word_out * WORD_SIZE;
    out->value = malloc(sizeof(uint16_t) * out->size);

    memcpy(out->value, in->value, in->size * sizeof(uint16_t));

    n_word_key = in->size / WORD_SIZE;
    i = in->size / WORD_SIZE;
    while (i < n_word_out) {
        /* Copy previous word */
        for (j = 0; j < WORD_SIZE ; j++) {
            temp[j] = out->value[(i-1)*WORD_SIZE + j];
        }

        if ((i % n_word_key) == 0) {
            superaes_RotWord(temp);
            superaes_SubWord(temp);
            superaes_Rcon(temp, i/n_word_key);
        }

        for (j = 0; j < WORD_SIZE ; j++) {
            out->value[i*WORD_SIZE + j] = out->value[(i-n_word_key)*WORD_SIZE + j] ^ temp[j];
        }
        i++;
    }

    return out;
}
