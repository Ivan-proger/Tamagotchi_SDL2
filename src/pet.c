#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <time.h>
#include "pet.h"
#include "graphics.h"
#include "globals.h"


Pet pet;

time_t lastSavedTime;

//! Инициализация питомца (и загрузка из файла в будущем)
void init_pet(void)
{
    FILE *file = fopen("tamagotchi_save.dat", "rb");
    if(file) {
        // Загружаем длину строки pathImage
        size_t len;
        fread(&len, sizeof(len), 1, file);
        // Выделяем память для pathImage
        pet.pathImage = (char*)malloc(len);
        // Загружаем строку pathImage
        fread(pet.pathImage, sizeof(char), len, file);

        fread(&pet.health, sizeof(unsigned char), 1, file);
        fread(&pet.satiety, sizeof(unsigned char), 1, file);
        fread(&pet.cheer, sizeof(unsigned char), 1, file);

        fread(&lastSavedTime, sizeof(lastSavedTime), 1, file);
        fclose(file);
        printf("The game was loaded.\n");

        time_t currentTime = time(NULL);
        // timeDiff - Секнуды с момента последнего сохранения 
        int timeDiff = difftime(currentTime, lastSavedTime);

        // Сытость 
        if((int)pet.satiety - timeDiff <= 0){
            pet.satiety = 0;  //! Тест *1000
        } else {pet.satiety -= timeDiff*10;}
        // Настроение
        if((int)pet.cheer - timeDiff <= 0){
            pet.cheer = 0;  //! Тест *1000
        } else {pet.cheer -= timeDiff;}
        // Здоровье
        if((int)pet.health - timeDiff <= 0){
            pet.health = 0;  //! Тест *1000
        } else {pet.health -= timeDiff/6;}
        if(pet.health > 255){
            pet.health = 255;
        }

        pet.health = fmin(255, fmax(0, pet.health));

    } else {    
        pet.pathImage = "assets/pet.png";
        pet.health = 200;
        pet.satiety = 100;
        pet.cheer = 50;
        lastSavedTime = time(NULL);
    }

    pet.scaleW = 0.2;
    pet.scaleH = 0.2;    
}

/**
 * @brief Добавить настроение(поиграть с питомцем)
 * 
 * @param value -- насколько увеличить
 */
void add_cheer(unsigned char value)
{
    if((int)pet.cheer + (int)value <= 255){
        pet.cheer += value;
        pet.health = (pet.health + value / 2 > 255) ? 255 : pet.health + value / 2;
    } else {
        pet.cheer = 255;
    }
}

void add_satiety(unsigned char value)
{
    if((int)pet.satiety + (int)value <= 255){
        pet.satiety += value;
        pet.health = (pet.health + value > 255) ? 255 : pet.health + value;
    } else {
        pet.satiety = 255;
    }
}

// Загрузка текстур питомца
void load_texture_pet(void)
{
    pet.texture = loadTexture(pet.pathImage);
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
            // Сохраняем длину строки pathImage
            size_t len = strlen(pet.pathImage) + 1;  // включая терминальный ноль
            fwrite(&len, sizeof(len), 1, file);

            // Сохраняем саму строку pathImage
            fwrite(pet.pathImage, sizeof(char), len, file);

            fwrite(&pet.health, sizeof(unsigned char), 1, file);
            fwrite(&pet.satiety, sizeof(unsigned char), 1, file);
            fwrite(&pet.cheer, sizeof(unsigned char), 1, file);

            fwrite(&lastSavedTime, sizeof(lastSavedTime), 1, file);
            fclose(file);
            printf("Saved!\n");
    } else {
        printf("Cant save!\n");
    }
}