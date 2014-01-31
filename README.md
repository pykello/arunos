Arunos
======

Arunos is a simple operating system for ARM processors. I started creating it to
teach myself how operating systems work. It is design is based on the JOS 
operating system which is used in the labs of the [6.828 operating system
engineering course at MIT](http://pdos.csail.mit.edu/6.828/2011/schedule.html). 
I will improve it as I continue studying this course.


Hardware Support
================
I test the operating system on [ARM926](http://www.arm.com/products/processors/classic/arm9/arm926.php)
processor and Versatile Platform Baseboard using [QEMU]
(http://wiki.qemu.org/Main_Page). I haven't tested it on any real hardware yet.
The reason for choosing ARM926 and VersatilePB was simply that QEMU supports
them well.

I aim to port it to a real Raspberry Pi computer in future.


Current Status
==============
Currently, the OS is able to communicate with serial I/O port uart0, and provides
a simple shell which have few commands. To see the commands supported, you can
run help in the shell.


Compiling
=========
You need to install the GNU tools for cross-compilation for arm-none-eabi target
to be able to compile the code. To install these tools in Fedora 19, you can do:

    sudo yum install arm-none-eabi-binutils-cs
    sudo yum install arm-none-eabi-gcc-cs

After you install these tools, you can run the following command to build the
kernel:

    make

After making, the OS image is created as a single "arunos.bin" binary file.


Emulating
=========
To emulate the operating system, you need to install QEMU. To do this in Fedora 19,
you can do:

    sudo yum install qemu-system-arm

After qemu-system-arm is installed, run the following command to emulate the OS:

    make qemu

To terminate the emulation, you can press Ctrl-A X keys.


Debugging
=========
The default GDB doesn't work with ARM binaries, so you need to compile GDB
manually to be able to debug Arunos. To do this, you can download gdb, then
configure it for arm-none-eabi target, and then make and install it:

    wget http://ftp.gnu.org/gnu/gdb/gdb-7.6.tar.gz
    tar -xzf gdb-7.6.tar.gz
    cd gdb-7.6
    ./configure --target=arm-none-eabi
    make
    sudo make install

After installation finishes, open two terminal windows and set their current
directory to the directory of Arunos. In terminal window 1, run:

    make qemu-gdb

In terminal window 2, run:
    
    gdb

Then, you can use gdb commands to debug Arunos.

**Note**. You may need to add the following line to ~/.gdbinit:

    set auto-load safe-path /


License
========
This software is BSD licensed. For more details, see the LICENSE file.

