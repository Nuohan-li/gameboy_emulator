
# Detect the operating system
ifeq ($(OS),Windows_NT)
    # windows
    EXECUTABLE = a.exe
    INCLUDE = -I win64_sdl/include -L win64_sdl/lib -lmingw32 -lSDL2main -lSDL2
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # mac
        EXECUTABLE = a
        INCLUDE = -lSDL2main -lSDL2 
    else
        # linux
        EXECUTABLE = a.out
        INCLUDE = -lSDL2main -lSDL2 
    endif
endif

# Default target
all: $(EXECUTABLE)

# Source files
SRCS = main.c memory.c cpu.c debug.c interrupt.c display.c

# Build the executable
$(EXECUTABLE): $(SRCS)
	gcc -Wall -Wextra $(SRCS) -o $(EXECUTABLE) $(INCLUDE) 