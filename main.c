#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "debug.h"

#ifdef __linux__ 
    #include<SDL2/SDL.h>
#elif __WIN64__
    #include "SDL2/SDL.h"

#endif

int main(int argc, char *argv[]){
    printf("test1\n");
    test();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window *window = SDL_CreateWindow("SDL Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_ALLOW_HIGHDPI);
    if (window == NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
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

    printf("over\n");
    
    log_trace(1, "%s error: buffer overflow at address %02x", "memory", 0xF002);

    // cpu cpu_ctx;
    // cpu_init(&cpu_ctx);
    // char* file_name = "GAMES/pokemonYellow.gb";
    // dump_game_content(file_name);
    // test();
    // debugger(&cpu_ctx);
    return 0;
} 