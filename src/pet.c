#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pet.h"
#include "graphics.h"

Pet pet;

//! Инициализация питомца (и загрузка из файла в будущем)
void init_pet(void)
{
    pet.x = 300;
    pet.y = 200;
    pet.w = 200;
    pet.h = 200;  

    pet.health = 100;
    pet.hungry = 0;
    pet.cheer = 50;
}

// Загрузка текстур питомца
void load_texture_pet(void)
{
    pet.texture = loadTexture("assets/pet.png");
}

// Отоброжение питомца
void show_pet(void)
{
    extern SDL_Renderer* gRenderer;
    SDL_Rect petRect;

    petRect.x = pet.x;
    petRect.y = pet.y;
    petRect.w = pet.w;
    petRect.h = pet.h;

    renderTexture(pet.texture, &petRect);
}

// Удаление сцены
void invisible_pet(void) {
    if (pet.texture) {
        SDL_DestroyTexture(pet.texture);
        pet.texture = NULL;
    }
}