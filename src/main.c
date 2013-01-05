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
#include <stdio.h>
#include <stdlib.h>

#include "constants.h"
#include "convert.h"
#include "superaes.h"

enum action_type {
    ACTION_TYPE_ENCRYPT,
    ACTION_TYPE_DECRYPT
};

static int output_superaes(const uint8_t *block,
        const struct key *key,
        enum action_type type,
        FILE *out)
{
    uint16_t  block16_in[BLOCK_SIZE_IN_INT16],
              block16_out[BLOCK_SIZE_IN_INT16];
    uint8_t   out_block[BLOCK_SIZE_IN_INT8];

    uint8_array_to_uint16(block, BLOCK_SIZE_IN_INT8, block16_in);

    if (type == ACTION_TYPE_ENCRYPT) {
        if (superaes_encrypt(block16_in, block16_out, key) < 0)
            goto error;
    } else {
        if (superaes_decrypt(block16_in, block16_out, key) < 0)
            goto error;
    }

    /* Converting back to uint8_t to be endianness independant */
    uint16_array_to_uint8(block16_out, BLOCK_SIZE_IN_INT16, out_block);

    /* XXX: Bad code structure, this function shoudn't do any action, just return
     * the result to print
     */
    return fwrite(out_block, sizeof(uint8_t), BLOCK_SIZE_IN_INT8, out);
error:
    return -1;
}

int main(int argc, char *argv[])
{
    int      to_read,
             read,
             error,
             i;
    uint8_t  buffer[BLOCK_SIZE_IN_INT8];
    FILE    *in,
            *out;

    struct key *key = NULL,
               *expanded_key = NULL;
    enum action_type action;

    /* No error for the moment */
    error = 0;

    /* Parsing command line arguments */
    /* TODO: Argument parsing */
    in = stdin;
    out = stdout;
    action = ACTION_TYPE_ENCRYPT;
    key = read_key(fopen(argv[1], "r"));
    if (key == NULL) {
        error = 1;
        goto out;
    }
    else {
        expanded_key = superaes_KeyExpansion(key);
    }
    if (argc > 2)
        action = ACTION_TYPE_DECRYPT;

    /* Read blocks and encrypt them*/
    /* XXX: main function is making the coffee */
    to_read = BLOCK_SIZE_IN_INT8;
    while ((read = fread(buffer, sizeof(uint8_t), to_read, in)) > 0) {
        to_read -= read;
        if (to_read == 0) {
            if (output_superaes(buffer, key, action, out) < 0) {
                error = 1;
                goto out;
            }
            to_read = BLOCK_SIZE_IN_INT8;
        }
    }
    if (feof(in) != 0) { /* End of file */
        if (to_read < BLOCK_SIZE_IN_INT8) { /* Uncomplete block */
            /* PKCS7 Padding */
            /* TODO: When decrypt, detect and remove padding */
            read = BLOCK_SIZE_IN_INT8 - to_read;

            for (i = read; i < BLOCK_SIZE_IN_INT8; i++)
                buffer[i] = (uint8_t) to_read;

            if (output_superaes(buffer, key, action, out) < 0) {
                error = 1;
                goto out;
            }
        }
    }
    else {
        error = 1;
        perror("Reading from input");
        goto out;
    }

out:
    if (key != NULL) {
        destroy_key(key);
    }
    if (expanded_key != NULL) {
        destroy_key(expanded_key);
    }
    if (error)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
