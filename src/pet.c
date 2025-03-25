#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "pet.h"
#include "graphics.h"
#include "globals.h"
#include <time.h>

Pet pet;

float scaleW, scaleH;
int health, satiety, cheer;

time_t lastSavedTime;

//! Инициализация питомца (и загрузка из файла в будущем)
void init_pet(void)
{
    pet.scaleW = 0.2;
    pet.scaleH = 0.2;
    pet.health = 200;
    pet.satiety = 100;
    pet.cheer = 50;
    lastSavedTime = time(NULL);
}

/**
 * @brief Добавить настроение(поиграть с питомцем)
 * 
 * @param value -- насколько увеличить
 */
void add_cheer(unsigned char value)
{
    printf("%d\n", pet.cheer);
    if((int)pet.cheer + (int)value <= 255){
        pet.cheer += value;
    } else {
        pet.cheer = 255;
    }
}

void add_satiety(unsigned char value)
{
    if((int)pet.satiety + (int)value <= 255){
        pet.satiety += value;
    } else {
        pet.satiety = 255;
    }
}
// Загрузка текстур питомца
void load_texture_pet(void)
{
    pet.texture = loadTexture("assets/pet.png");
    sizeTexture(pet.texture, &pet.w, &pet.h); // Записываем ширину и высоту
}

// Отображение питомца
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

void save_game(void) {
    lastSavedTime = time(NULL);

    FILE *file = fopen("tamagotchi_save.dat", "wb");
    if (file) {
        fwrite(&pet, sizeof(pet), 1, file);
        fwrite(&lastSavedTime, sizeof(lastSavedTime), 1, file);
        fclose(file);
        printf("Saved!\n");
    } else {
        printf("Cant save!\n");
    }
}

void load_game(void){
    FILE *file = fopen("tamagotchi_save.dat", "rb");
    if(file) {
        fread(&pet, sizeof(pet), 1, file);
        fread(&lastSavedTime, sizeof(lastSavedTime), 1, file);
        fclose(file);
        printf("The game was loaded.\n");

        time_t currentTime = time(NULL);
        double timeDiff = difftime(currentTime, lastSavedTime);

        // pet.satiety -= (int)(timeDiff / 60);
        // pet.cheer -= (int)(timeDiff / 60);
        // pet.health -= (int)(timeDiff / 120);

        // if (pet.satiety < 0) pet.satiety = 0;
        // if (pet.cheer < 0) pet.cheer = 0;
        // if (pet.health < 0) pet.health = 0;

        printf("\ncheer is %d\n", pet.cheer);
        printf("Прошло %.0f секунд.\n");
    } else {
        printf("No available saves.\n");
    }
}