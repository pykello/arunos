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
 * a polling virtio block driver and read-only FAT16 filesystem,
 * ELF loading from disk,
 * basic system calls: putch, getch, exit, getpid, fork, exec, yield, wait,
 * a small user-space library with string, stdio, math, and syscall helpers,
 * a kernel monitor implementation with diagnostic commands.

On boot, the kernel initializes memory, virtual memory, process management, and
the console, mounts `disk.img`, then starts `0.ELF`, built from
`user/shell.c`. The user shell prompts with `$` and runs disk ELF files by
numeric index:

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
 * `execute <n>`: create a process and run `<n>.ELF` from the disk.


Building
--------

You need a host C compiler, Python 3, dosfstools, mtools, and GNU
cross-compilation tools for the `arm-none-eabi` target. The build expects these commands to be on `PATH`:

    gcc
    arm-none-eabi-ar
    arm-none-eabi-as
    arm-none-eabi-gcc
    arm-none-eabi-ld
    arm-none-eabi-objcopy
    arm-none-eabi-objdump

On Debian or Ubuntu, the cross-compiler packages are commonly:

    sudo apt install gcc gcc-arm-none-eabi binutils-arm-none-eabi \
        dosfstools mtools python3

Build the default VersatilePB image:

    make

Build a specific architecture:

    make arch=versatilepb

The build produces:

 * `arunos.bin`: raw boot image,
 * `arunos.elf`: ELF image with symbols,
 * `arunos.asm`: disassembly,
 * `disk.img`: 16 MiB FAT16 disk containing `0.ELF` through `4.ELF`,
 * `user/shell`, `user/hello`, etc.: standalone user ELF files.

Clean generated files with:

    make clean


Emulating
---------

Install QEMU's ARM system emulator. On Debian or Ubuntu:

    sudo apt install qemu-system-arm

Run the default image in QEMU:

    make qemu

QEMU attaches `disk.img` as a read-only legacy virtio PCI block device.
It runs with `-nographic`, so the serial console is attached to the terminal.
To exit QEMU, press `Ctrl-A`, then `X`.


Disk and ELF Scope
------------------

Storage intentionally supports one disk, synchronous 512-byte reads, and
FAT16 starting at sector zero. Files must use root-directory 8.3 names;
there are no partitions, subdirectories, long names, writes, or file syscalls.
`exec(n)` loads `n.ELF` for indices 0 through 9 and returns -1 on failure.
The image builder installs programs 0 through 4; the other names are optional.

ELFs must be static little-endian ARM32 executables, with at most 16 program
headers and load segments below 1 MiB. The loader reads segments directly
from the filesystem and zeroes BSS. It stages a replacement in a spare
process slot so failure preserves the caller; exec needs one free slot.
A missing disk, invalid filesystem, or unusable `0.ELF` stops boot with a
serial error message.

To replace a program without rebuilding the kernel (with QEMU stopped):

    mcopy -o -i disk.img user/hello ::1.elf

`make` rebuilds the disk when a user program changes. `make clean` removes
it, including any manual changes.

Run the host FAT tests and QEMU boot/process tests:

    make test

Tests cover file contents and fragmentation, invalid filesystem metadata,
malformed ELF headers, BSS, failed exec, disk-only replacement, and the
existing hello, fork, exec, and concurrency programs.


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
 * `user/`: user programs and FAT disk-image build rules,
 * `tests/`: host filesystem tests and QEMU integration tests.


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
