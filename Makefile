all: cpu.cpp main.cpp debug.cpp memory.cpp 
	g++ -Wall -Wextra cpu.cpp memory.cpp main.cpp debug.cpp -I win64_sdl/include -L win64_sdl/lib -lmingw32 -lSDL2main -lSDL2