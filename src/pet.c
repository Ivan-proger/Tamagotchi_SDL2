#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pet.h"
#include "graphics.h"
#include "globals.h"

Pet pet;

//! Инициализация питомца (и загрузка из файла в будущем)
void init_pet(void)
{
    pet.scaleW = 0.2;
    pet.scaleH = 0.2;
    pet.health = 100;
    pet.hungry = 0;
    pet.cheer = 50;
}

// Загрузка текстур питомца
void load_texture_pet(void)
{
    pet.texture = loadTexture("assets/pet.png");
    sizeTexture(pet.texture, &pet.w, &pet.h); // Записываем шириину и высоту
}

// Отоброжение питомца
void show_pet(void)
{
    pet.x = WINDOW_WIDTH/2-((int)(pet.w*pet.scaleW))/2;
    pet.y = WINDOW_HEIGHT/2-((int)(pet.h*pet.scaleH))/2;
    renderTextureScaled(pet.texture, pet.x, pet.y, pet.scaleW, pet.scaleH);
}

// Удаление сцены
void invisible_pet(void) {
    if (pet.texture) {
        SDL_DestroyTexture(pet.texture);
        pet.texture = NULL;
    }
}