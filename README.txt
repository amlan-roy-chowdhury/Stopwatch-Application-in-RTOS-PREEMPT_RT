Stopwatch Application on BeagleBone Black

Overview:

This application is a stopwatch program implemented in C for the BeagleBone Black. It uses GPIO pins for button and LED control and demonstrates multithreading with a real-time operating system (RTOS) concept.

Files Included:

stopwatch.c: Main program logic for the stopwatch.
gpio.c: GPIO functions for controlling LEDs and reading button inputs.
RTOS.h: Header file defining constants, structures, and function prototypes.
Makefile: File to automate the build, clean, and execution process.

Prerequisites:

A working BeagleBone Black with Debian installed.
GCC compiler installed:
sudo apt-get install gcc
Place all files (stopwatch.c, gpio.c, RTOS.h, and Makefile) in the same directory.

How to Build and Run:

1. Build the Executable:

Open the terminal and navigate to the directory containing the files. Run the following command to compile the program:

make

This will:

Compile stopwatch.c and gpio.c into object files (stopwatch.o and gpio.o).
Link the object files into an executable named stopwatch.

2. Run the Stopwatch Application:

To execute the program, run:

make run

Alternatively, you can directly run the generated executable:

./stopwatch

3. Clean Build Files:

To clean up the compiled object files and the executable, run:

make clean

This will remove all .o files and the stopwatch executable.

GPIO Pin Configuration:

Button B1 (Start/Stop Button): 46
Button B2 (Reset Button): 47
LED 1 (Red LED): 44
LED 2 (Green LED): 26