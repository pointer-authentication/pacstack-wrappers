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
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#ifndef LEVEL_OF_NESTING
#define LEVEL_OF_NESTING 1
#endif

void B(int b);
void C(void);

jmp_buf at_main;

int main()
{
    printf("%s ENTER\n", __FUNCTION__);

    if (setjmp(at_main)) {
        /* 1 -> execution context was restored from jmpbuf */
        printf("returned from longjmp\n");
        printf("%s EXIT\n", __FUNCTION__);
        exit(EXIT_SUCCESS);
    } else {
        /* 0 -> execution context was saved in jmpbuf */
        printf("calling B(%d)\n", LEVEL_OF_NESTING);
        B(LEVEL_OF_NESTING);
    }

    /* We should never reach this */
    printf("%s RETURN\n", __FUNCTION__);
    exit(EXIT_FAILURE);
}

__attribute__((noinline))
void B(int i)
{
    printf("%s ENTER\n", __FUNCTION__);

    if (i > 1) {
        printf("calling B(%d)\n", i-1);
        B(--i);
    } else {
        printf("calling C\n");
        C();
    }

    printf("%s RETURN\n", __FUNCTION__);
    return;
}

__attribute__((noinline))
void C(void)
{
    printf("%s ENTER\n", __FUNCTION__);
    
    longjmp(at_main, 1);
    
    /* We should never reach this */
    printf("%s RETURN\n", __FUNCTION__);
}

