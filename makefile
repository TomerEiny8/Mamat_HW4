# Welcomre to Mamat, 044101
#
# This is the project's Makefile. It contains the instructions for compiling
# the projects C files, and then link the generated objects into an executable.
# This simple file will compile "main.c" to "main.o" and then link it (with C
# runtime library) to the executable "prog.exe".
#
# To compile, open the terminal in the folder that contains
# this file and run "make".
#
# To clean, open the terminal in the folder that contains
# this file and run "make clean".
#
# All eclipse projects in this machine are under /home/mamat/Documents/

CC      := gcc
CFLAGS  := -fPIC           # position-independent code for a shared library
LDFLAGS := -shared         # link into a shared object

# 2. Library search path and libraries to link
LIB_DIR := .               # assume liblinked-list.so is in the same directory
LIBS    := -L$(LIB_DIR) -llinked-list

# 3. Targets
TARGET  := libgrades.so

# 4. Source, headers, and object files
SRC     := grades.c
DEPS    := grades.h linked-list.h
OBJS    := grades.o

.PHONY: all clean

# Default rule: build the shared library
all: $(TARGET)

# Link grades.o into libgrades.so, linking against liblinked-list.so
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

# Compile grades.c to grades.o; recompile if grades.c, grades.h, or linked-list.h changes
grades.o: $(SRC) $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up only the generated files
clean:
	rm -f $(OBJS) $(TARGET)
