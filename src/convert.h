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
#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <stdint.h>

static inline void uint8_array_to_uint16(const uint8_t *src, int size, uint16_t *dest)
{
    int i, j, k;

    size /= INT8_IN_INT16;
    for (i=0, j=0; i < size ; i++, j+=INT8_IN_INT16) {
        dest[i] = 0;
        for (k = 0; k < INT8_IN_INT16; k++) {
            dest[i] <<= BITS_IN_BYTE;
            dest[i] += src[j+k];
        }
    }
}

static inline void uint16_array_to_uint8(const uint16_t *src, int size, uint8_t *dest)
{
    int i, j, k;
    uint16_t tmp;

    for (i=0, j=0; i < size ; i++, j+=INT8_IN_INT16) {
        tmp = src[i];
        for (k = INT8_IN_INT16-1; k >= 0; k--) {
            dest[j+k] = (uint8_t) tmp;
            tmp >>= BITS_IN_BYTE;
        }
    }
}

#endif /* __CONVERT_H__ */
