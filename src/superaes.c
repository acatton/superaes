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
#include <string.h>

#include "constants.h"
#include "superaes.h"
#include "tables.h"

static void superaes_AddRoundKey(uint16_t *state,
        const struct key key,
        int from)
{
    int i = from * WORD_SIZE,
        j,
        to = i + BLOCK_SIZE_IN_INT16;

    for (j = 0; i < to; i++, j++)
        state[j] ^= key.value[i];
}

static void state_substitute(uint16_t *state, const uint16_t *table)
{
    int i;

    for (i = 0; i < BLOCK_SIZE_IN_INT16 ; i++)
        state[i] = table[state[i]];
}

static void superaes_SubBytes(uint16_t *state)
{
    state_substitute(state, subbytes_table);
}

static void superaes_InvSubBytes(uint16_t *state)
{
    state_substitute(state, invsubbytes_table);
}

/* XXX: Not optimised at all. And very complex. Even I can't read me */
/* TODO: Optimize this shit and make it readable */
static void shift(uint16_t *word, int way, int shift)
{
    uint16_t tmp;
    int      i, j, index_dest, index_from;

    for (i = 0; i < shift; i++) {
        index_dest = (way + 1 + WORD_SIZE) % WORD_SIZE;
        tmp = word[index_dest];
        for (j = 1; j < WORD_SIZE; j++)
        {
            index_from = (index_dest - way + WORD_SIZE) % WORD_SIZE;
            word[index_dest] = word[index_from];
            index_dest = index_from;
        }
        word[index_dest] = tmp;
    }
}

static void shift_rows(uint16_t *state, int way)
{
    int i;

    for (i = 0; i < BLOCK_SIZE_IN_WORD ; i++) {
        shift(&(state[i*WORD_SIZE]), way, i);
    }
}

static void superaes_ShiftRows(uint16_t *state)
{
    shift_rows(state, -1);
}

static void superaes_InvShiftRows(uint16_t *state)
{
    shift_rows(state, 1);
}

static const uint16_t *get_lookup_table(uint16_t n)
{
    int                 i;
    struct lookup_match match;

    i = 0;
    do {
        match = lookup_table_list[i];
        if (match.value == n)
            return match.lookup_table;
        i++;
    } while (match.lookup_table != NULL);
    return NULL;

}

uint16_t galois_multiply(uint16_t n, uint16_t mixcolumn)
{
    /* TODO: Fix if NULL pointer */
    const uint16_t *table = get_lookup_table(mixcolumn); /* XXX: Ignore if return NULL */
    return table[n];
}

#define state_matrix(state, y, x) state[y * WORD_SIZE + x]

static void mixcolumns(uint16_t *state, const uint16_t matrix[MIXCOLUMNS_SIZE_Y][MIXCOLUMNS_SIZE_X])
{
    uint16_t state_copy[BLOCK_SIZE_IN_INT16],
             value;
    int      i, j, x, y;

    memcpy(state_copy, state, sizeof(state_copy));
    /* XXX: Matrix multiplication. Quite complexe */
    /* TODO: Make this readable */
    for (i = 0; i < MIXCOLUMNS_SIZE_X; i++) {
        for (j = 0; j < MIXCOLUMNS_SIZE_Y; j++) {
            value = 0;
            for (x = 0, y = 0; x < MIXCOLUMNS_SIZE_X; x++, y++) {
                value ^= galois_multiply(state_matrix(state_copy, y, i),
                        matrix[j][x]);
            }
            state_matrix(state, j, i) = value;
        }
    }
}

static void superaes_MixColumns(uint16_t *state)
{
    mixcolumns(state, mixcolumn_matrix);
}

static void superaes_InvMixColumns(uint16_t *state)
{
    mixcolumns(state, invmixcolumn_matrix);
}

int superaes_encrypt(const uint16_t *in, uint16_t *out, const struct key key)
{
    int round_amount, i;

    round_amount = key.size / BLOCK_SIZE_IN_WORD;
    memcpy(out, in, sizeof(uint16_t) * BLOCK_SIZE_IN_INT16);

    /* AES Cipher Algorithm */
    superaes_AddRoundKey(out, key, 0);
    for (i = 1; i < round_amount ; i++) {
        superaes_SubBytes(out);
        superaes_ShiftRows(out);
        superaes_MixColumns(out);
        superaes_AddRoundKey(out, key, i * BLOCK_SIZE_IN_WORD);
    }
    superaes_SubBytes(out);
    superaes_ShiftRows(out);
    superaes_AddRoundKey(out, key, round_amount * BLOCK_SIZE_IN_WORD);

    return 0;
}
int superaes_decrypt(const uint16_t *in, uint16_t *out, const struct key key)
{
    int round_amount, i;

    round_amount = key.size / BLOCK_SIZE_IN_WORD;
    memcpy(out, in, sizeof(uint16_t) * BLOCK_SIZE_IN_INT16);

    /* AES Decipher Algorithm */
    superaes_AddRoundKey(out, key, round_amount * BLOCK_SIZE_IN_WORD);
    for (i = round_amount - 1; i > 0 ; i--) {
        superaes_InvShiftRows(out);
        superaes_InvSubBytes(out);
        superaes_AddRoundKey(out, key, i * BLOCK_SIZE_IN_WORD);
        superaes_InvMixColumns(out);
    }
    superaes_InvShiftRows(out);
    superaes_InvSubBytes(out);
    superaes_AddRoundKey(out, key, 0);

    return 0;
}
