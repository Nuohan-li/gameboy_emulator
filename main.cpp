#include <iostream>
#include <stdint.h>
#include "cpu.h"
#include "debug.h"

#ifdef __linux__ 
    #include<SDL2/SDL.h>
#elif __WIN64__
    #include "win64_sdl/include/SDL2/SDL.h"
#endif

using namespace std;

int main(int argc, char *argv[]){
    cout << "running\n";
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        cout << "SDL_CreateWindow error: " <<  SDL_GetError() << endl;
        return 1;
    }
    SDL_Event event;
    while(1){
        if(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                break;
            }
        }
    }
    SDL_DestroyWindow(window);
    SDL_Quit();

    // cpu cpu_ctx;
    const char* file_name = "GAME/pokemonYellow.gb";
    // dump_game_content(file_name);
    // dump_game_to_file(file_name, "GAME/pokemonyellowdump.txt");
    // test();
    // debugger(&cpu_ctx);
    return 0;
} 