/*************************************************************************************
 * Copyright (c) 2020 Aalto University Secure Systems Group                          *
 * Copyright (c) 2020 University of Waterloo Secure Systems Group                    *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of   *
 * this software and associated documentation files (the "Software"), to deal in     *
 * the Software without restriction, including without limitation the rights to      *
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of  *
 * the Software, and to permit persons to whom the Software is furnished to do so,   *
 * subject to the following conditions:                                              *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS  *
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR    *
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER    *
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN           *
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        *
 *************************************************************************************/
#ifndef PACSTACK_WRAPPERS_H
#define PACSTACK_WRAPPERS_H
#include <setjmp.h>

/* 
 * jmp_buf offsets for glibc (from glibc/sysdeps/aarch64/jmpbuf-offsets.h) 
 */
#define JB_X19            0
#define JB_X20            1
#define JB_X21            2
#define JB_X22            3
#define JB_X23            4
#define JB_X24            5
#define JB_X25            6
#define JB_X26            7
#define JB_X27            8
#define JB_X28            9
#define JB_X29           10
#define JB_LR            11
#define JB_SP            13
#define JB_D8            14
#define JB_D9            15
#define JB_D10           16
#define JB_D11           17
#define JB_D12           18
#define JB_D13           19
#define JB_D14           20
#define JB_D15           21

/*
 * PACStack wrappers for glibc setjmp and longjmp.
 */
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);

#endif /* PACSTACK_WRAPPERS_H */ 
