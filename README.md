# PACStack Wrappers

This repository contains wrapper functions adding PACStack support for glibc
`setjmp` and `longjmp`.

# Usage

PACStack uses the ARMv8.3-a PAuth extension and supports only AArch64 targets
with PAuth extension (e.g., `-march=armv8.3-a`). Consequently the testcases in
this repository have to be (cross-)compiled for AArch64 using the PACStack LLVM
toolchain available at [https://github.com/pointer-authentication/pacstack-llvm](https://github.com/pointer-authentication/pacstack-llvm).

Note that since LLVM project does not have implementations of all the parts of
toolchain and library dependencies it must be combined with a GCC cross compile
toolchain to provide missing components. The instructions here assume that the
Linaro toolchain (available at [https://www.linaro.org/downloads/](https://www.linaro.org/downloads/)) is installed
on host system.

Test binaries can be use executed using either ARM's [Armv8-A Architecture Fixed
Virtual Platform (FVP)](https://developer.arm.com/tools-and-software/simulation-models/fixed-virtual-platforms) simulation models, or the [QEMU](https://www.qemu.org/) machine emulator version
4.0 or newer. The instructions here assume QEMU and that binary formats for the
Aarch64 binaries have been registered with Linux so that it will be possible to
run foreign binaries directly. On Ubuntu and other Debian-based distributions
this can be achieved by installing the `qemu-user-static` deb package. 

The wrapper and tests can be built using the PACStack LLVM & Linaro toolchain
and the tests ran using QEMU as follows:

<pre>
$ git clone https://github.com/pointer-authentication/pacstack-wrappers
$ cd pacstack-wrappers
$ ./autogen.sh

$ ./configure --host=aarch64-unknown-linux-gnu \
    CC=<i>/path/to/pacstack/llvm/toolchain</i>/bin/clang \
    LDSHARED=<i>/path/to/pacstack/llvm/toolchain</i>/bin/clang \
    CFLAGS="--target=aarch64-linux-gnu --sysroot=<i>/path/to/linaro/sysroot</i> --gcc-toolchain=<i>/path/to/linaro/gcc</i>" \
    LDFLAGS="-Wc,--target=aarch64-linux-gnu -Wc,--sysroot=<i>/path/to/linaro/sysroot</i> -Wc,--gcc-toolchain=<i>/path/to/linaro/gcc</i> -Wl,--dynamic-linker=<i>/path/to/linaro/sysroot</i>/lib/ld-linux-aarch64.so.1"

$ make check TESTS_ENVIRONMENT=LD_LIBRARY_PATH=<i>/path/to/linaro/sysroot</i>/lib;"
</pre>

To use the wrappers with a PACStack instrumented binary set the `LD_PRELOAD`
environmental variable to point to `libpacstack.so`. This ensures that the
PACStack wrappers are loaded before glibc `libc.so`:

<pre>
$ LD_PRELOAD=<i>path/to/</i>libpacstack.so <i>./executable</i>
</pre>

# License

> Copyright 2020 Aalto University Secure Systems Group  
> Copyright 2020 University of Waterloo Secure Systems Group  
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.

