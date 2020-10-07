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

/*
 * PACStack wrappers for glibc setjmp and longjmp.
 *
 * Authors: Thomas Nyman <thomas.nyman@aalto.fi>
 *          Hans Liljestrand <hans@liljestrand.dev>
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <setjmp.h>
#include "pacstack_wrappers.h"

/*
 * Stored glibc guard value.
 *
 * This is needed to work around the glibc PTR_MANGLE jmp_buf protection guard
 * in sysdeps/aarch64/setjmp.S. The mangling is a simple XOR, so we can retrieve
 * the guard value in setjmp since we know the inputs. However, we also need
 * the guard value in longjmp, which is why we store it in this global.  This
 * value is already stored by glibc, but the __pointer_chk_guard_local variable
 * used for this purpose (declared in unix/sysc/linux/aarch64/sysdep.h) has
 * 'hidden' visibility, which means that the symbol will not be placed into the
 * dynamic symbol table, so no other module (executable or shared library) can
 * reference it directly (so attackers couldn't do this trick also.)
 */
static uintptr_t guard = 0;

/*
 * Stored pointers to real setjmp and longjmp in glibc.
 *
 * These are needed by the wrappers to call the real glibc functions.
 * Usually we could just look these up using dlsym(RTLD_NEXT, ...) on demand,
 * but as the wrappers are declared with the 'naked' attribute they cannot
 * constain non-ASM statements. Instead we look up the function pointers in an
 * automatically called constructor before execution enters main and store them
 * in real_setjmp and real_longjmp for larer use by the wrappers.
 */
static int (*real_setjmp)(jmp_buf env);
static void (*real_longjmp)(jmp_buf env, int val);

/*
 * Constructor for initializing pointers to real setjmp and longjmp.
 */
__attribute__((constructor))
static void __initialize_real_setjmp_ptrs()
{
    if ((real_setjmp = dlsym(RTLD_NEXT, "setjmp")) == NULL) {
	goto error;
    }

    if ((real_longjmp = dlsym(RTLD_NEXT, "longjmp")) == NULL) {
	goto error;
    }

    return;  /* All symbols were found, we're done! */

error:
    fprintf(stderr, "%s\n",
            dlerror());

    /* We dare not use abort() here because it would run atexit(3)
     * handlers and try to flush stdio. */
    _exit(EXIT_FAILURE);
}

/*
 * Wrapper for glibc setjmp.
 */
__attribute__((naked))
int setjmp(jmp_buf env)
{
    __asm volatile (
        "stp   x29, %[guard], [sp, #-32]!\n\t" // store frame record
        "stp   x0, x5,[sp, #16]          \n\t" // store env and &guard
	"blr   %[real_setjmp]            \n\t" // call setjmp (clobbers x0)
        "adr   x3, .                     \n\t" // x3 = setjmp return site
        "ldp   x1, x5, [sp, #16]         \n\t" // x1 = env, x5 = &guard
        "ldp   x29, x30, [sp], #32       \n\t" // load frame record
        "ldr   x4, [x1, %[clr]]          \n\t" // load lr from env
        "eor   x3, x3, x4                \n\t" // x3 = MANGLE value
        "mov   x2, lr                    \n\t" // x2 = return address
        "mov   x15, sp                   \n\t" // x15 = sp
        "eor   x15, x15, x3              \n\t" // mangle x15
        "str   x29, [x1, %[cfp]]         \n\t" // store FP in env
        "str   x15, [x1, %[csp]]         \n\t" // store SP in env
        "pacia x15, x28                  \n\t" // pacia sp, cr
        "pacia lr, x28                   \n\t" // pacia lr, cr
        "eor   lr, lr, x15               \n\t" // mask
        "str   lr, [x1, %[clr]]          \n\t" // store PACed+masked lr in env
        "str   x3, [x5]                  \n\t" // store the guard value
        "ret   x2                        \n\t" // return
        :
        : [csp] "i" (JB_SP<<3),
          [ccr] "i" (JB_X28<<3),
          [cfp] "i" (JB_X29<<3),
          [clr] "i" (JB_LR<<3),
          [guard] "r" (&guard),
	  [real_setjmp] "r" (real_setjmp)
        : "x0", "x1", "x2", "x3", "x4", "x5"
    );
}

/*
 * Wrapper for glibc longjmp.
 */
__attribute__((naked))
void longjmp(jmp_buf env, int val)
{
    __asm volatile (
        "ldr   x15, [x0, %[csp]]         \n\t" // load sp
        "ldr   x28, [x0, %[ccr]]         \n\t" // load cr
        "ldr   lr, [x0, %[clr]]          \n\t" // load PACed+masked lr
        "pacia x15, x28                  \n\t" // pacia sp, cr
        "eor   lr, lr, x15               \n\t" // unmask
        "autia lr, x28                   \n\t" // autia lr, cr
        "ldr   x5, [%[guard]]            \n\t" // load guard value
        "eor   lr, lr, x5                \n\t" // mangle lr
        "str   lr, [x0, %[clr]]          \n\t" // store lr in env
	"br %[real_longjmp]              \n\t" // everybody jump!
        ".ltorg\n"
        "1:\n"
        :
        : [csp] "i" (JB_SP<<3),
          [ccr] "i" (JB_X28<<3),
          [clr] "i" (JB_LR<<3),
          [guard] "r" (&guard),
	  [real_longjmp] "r" (real_longjmp)
        : "x0", "x1", "x5"
    );
}

