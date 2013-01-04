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
#ifndef __TABLES_H__
#define __TABLES_H__

#include <stdint.h>

#include "_tables_common.h"

/* 2^16 = 65536 */
#define TABLE_SIZE 65536

extern const uint16_t            subbytes_table[TABLE_SIZE];
extern const uint16_t            invsubbytes_table[TABLE_SIZE];
extern const uint16_t            lookup_571_table[TABLE_SIZE];
extern const uint16_t            lookup_CF7_table[TABLE_SIZE];
extern const uint16_t            lookup_6755_table[TABLE_SIZE];
extern const uint16_t            lookup_C475_table[TABLE_SIZE];
extern const uint16_t            lookup_CAE3_table[TABLE_SIZE];
extern const uint16_t            lookup_FFA9_table[TABLE_SIZE];
extern const uint16_t            lookup_FFC7_table[TABLE_SIZE];
extern const uint16_t            lookup_FFD9_table[TABLE_SIZE];
extern const uint16_t            lookup_FFEF_table[TABLE_SIZE];
extern const uint16_t            lookup_FFF1_table[TABLE_SIZE];
extern const struct lookup_match lookup_table_list[];
extern const uint16_t            mixcolumn_matrix[MIXCOLUMNS_SIZE_Y][MIXCOLUMNS_SIZE_X];
extern const uint16_t            invmixcolumn_matrix[MIXCOLUMNS_SIZE_Y][MIXCOLUMNS_SIZE_X];
extern const uint16_t            rcon[TABLE_SIZE];

#endif /* __TABLES_H__ */
