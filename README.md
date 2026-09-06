Arunos
======

Arunos is a small teaching operating system for ARM processors. It started as
an experiment in learning operating-system internals, and its design is
influenced by the JOS operating system used in MIT's [6.828] operating-system
engineering course.


Hardware Support
----------------

The default target is an ARM926EJ-S CPU on QEMU's Versatile Platform Baseboard
machine:

    make
    make qemu

Current Status
--------------

Arunos currently has:

 * ARM startup code for the supported board,
 * UART serial I/O,
 * interrupt and timer support,
 * a page allocator and ARM section-table based virtual memory,
 * a small process table and scheduler,
 * ELF loading for embedded user programs,
 * basic system calls: putch, getch, exit, getpid, fork, exec, yield, wait,
 * a small user-space library with string, stdio, math, and syscall helpers,
 * a kernel monitor implementation with diagnostic commands.

On boot, the kernel initializes memory, virtual memory, process management, and
the console, then starts user program `0`, which is `user/shell.c`. The user
shell prompts with `$` and runs embedded user programs by numeric index:

 * `0`: shell
 * `1`: hello
 * `2`: fork_test
 * `3`: exec_test
 * `4`: concurrency_test

The kernel monitor code provides these commands when entered by kernel code:

 * `help`: list monitor commands,
 * `backtrace`: print a stack backtrace,
 * `hextee`: echo input bytes as hexadecimal until `q`,
 * `kerninfo`: print kernel symbol and footprint information,
 * `status`: print CPU, register, and memory status,
 * `execute <n>`: create a process and run embedded user program `<n>`.


Building
--------

You need a host C compiler and GNU cross-compilation tools for the
`arm-none-eabi` target. The build expects these commands to be on `PATH`:

    gcc
    arm-none-eabi-ar
    arm-none-eabi-as
    arm-none-eabi-gcc
    arm-none-eabi-ld
    arm-none-eabi-objcopy
    arm-none-eabi-objdump

On Debian or Ubuntu, the cross-compiler packages are commonly:

    sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi

Build the default VersatilePB image:

    make

Build a specific architecture:

    make arch=versatilepb

The build produces:

 * `arunos.bin`: raw boot image,
 * `arunos.elf`: ELF image with symbols,
 * `arunos.asm`: disassembly,
 * `user/user_programs.c`: generated embedded user-program table.

Clean generated files with:

    make clean


Emulating
---------

Install QEMU's ARM system emulator. On Debian or Ubuntu:

    sudo apt install qemu-system-arm

Run the default image in QEMU:

    make qemu

QEMU runs with `-nographic`, so the serial console is attached to the terminal.
To exit QEMU, press `Ctrl-A`, then `X`.


Debugging
---------

Use a GDB build that supports ARM bare-metal binaries, such as
`arm-none-eabi-gdb` or `gdb-multiarch`.

Start QEMU in one terminal:

    make qemu-gdb

Start GDB in another terminal:

    arm-none-eabi-gdb arunos.elf

or:

    gdb-multiarch arunos.elf

Then connect to QEMU:

    target remote :26000

If GDB refuses to auto-load local project files, you may need to allow this
repository in `~/.gdbinit`, for example:

    set auto-load safe-path /


Repository Layout
-----------------

 * `arch/`: board-specific configuration, linker scripts, UART, timer, and IRQ
   code,
 * `include/`: kernel and library headers,
 * `kernel/`: core kernel, memory management, monitor, process, and syscall
   code,
 * `lib/`: user/kernel support library code,
 * `user/`: embedded user programs and the host-side encoder used to package
   them into the kernel image.


Coding Style
------------

The coding style used in this project is based on the
[Linux kernel coding style]:

 * Use tabs for indentation,
 * Each tab is 8 characters wide,
 * Limit lines to 80 characters,
 * Use underscore-delimited names for functions and variables,
 * Use camel-case names for structs and enums,
 * Do not typedef structs and enums,
 * Open braces on the same line, except for functions,
 * If a condition spans multiple lines, open braces on a separate line,
 * Do not use braces for single-line blocks,
 * Do not indent `case` statements in a `switch`,
 * Separate functions by a single empty line,
 * Comment what is being done, not how obvious code works,
 * Short local names are allowed unless they introduce confusion,
 * Use descriptive names for global variables and functions.


License
-------

This software is BSD licensed. For more details, see the LICENSE file.


[6.828]: https://pdos.csail.mit.edu/6.828/
[Linux kernel coding style]: https://docs.kernel.org/process/coding-style.html
