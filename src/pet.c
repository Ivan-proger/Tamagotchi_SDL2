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
    renderTextureScaled(pet.texture, 300, 200, 0.2, 0.2);
}

// Удаление сцены
void invisible_pet(void) {
    if (pet.texture) {
        SDL_DestroyTexture(pet.texture);
        pet.texture = NULL;
    }
}