#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <time.h>
#include "pet.h"
#include "SDL_error.h"
#include "SDL_log.h"
#include "SDL_render.h"
#include "animation.h"
#include "graphics.h"
#include "scene_manager.h"
#include "dead_scene.h"
#include "globals.h"
#include "notify.h"


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
        fread(&pet.scaleW, sizeof(pet.scaleW), 1, file);
        fread(&pet.scaleH, sizeof(pet.scaleH), 1, file);

        fread(&lastSavedTime, sizeof(lastSavedTime), 1, file);

        // Считываем анимацию если есть
        bool isAnim;
        fread(&isAnim, sizeof(bool), 1, file);
        if(isAnim) {
            // Выделяем память под структуру Animation
            pet.stayAnim = malloc(sizeof(Animation));
            if (!pet.stayAnim) {
                SDL_Log("Ошибка выделения памяти для stayAnim!");
                return;
            }
            // Считываем длину строки
            fread(&len, sizeof(len), 1, file);

            // Выделяем память под такую длинну строки
            pet.stayAnim->spriteSheetPath = malloc(len);

            if(!pet.stayAnim->spriteSheetPath){
                SDL_Log("Ошибка выделения памяти для пути к анимации %s", SDL_GetError());
            }

            // Считываем путь по которому восстановим анимацию
            if(!(fread(pet.stayAnim->spriteSheetPath, sizeof(char), len, file))){
                SDL_Log("Ошибка загрузки пути к анимации %s", SDL_GetError());
                fclose(file);
                return;
            }

            // Общее количество кадров
            fread(&pet.stayAnim->frameCount, sizeof(int), 1, file);

            // Выделяем память под кадры
            pet.stayAnim->frames = malloc(pet.stayAnim->frameCount * sizeof(SDL_Rect));
            if(!(fread(pet.stayAnim->frames, sizeof(SDL_Rect), pet.stayAnim->frameCount, file))){
                SDL_Log("Ошибка загрузки SDL_Rect для анимации %s", SDL_GetError());
                fclose(file);
                return; 
            }

            // Время кадра
            fread(&pet.stayAnim->frameTime, sizeof(float), 1, file);

        }


        fclose(file);

        time_t currentTime = time(NULL);
        // timeDiff - Секнуды с момента последнего сохранения 
        int timeDiff = difftime(currentTime, lastSavedTime);

        // Сытость 
        if((int)pet.satiety - timeDiff <= 0){
            pet.satiety = 0;
        } else {pet.satiety -= timeDiff*10;}
        // Настроение
        if((int)pet.cheer - timeDiff <= 0){
            pet.cheer = 0;
        } else {pet.cheer -= timeDiff;}
        // Здоровье
        if((int)pet.health - timeDiff <= 0){
            pet.health = 0;
        } else {pet.health -= timeDiff/6;}

        return;
    }    

    pet.pathImage = "assets/pets/pet.png";
    pet.stayAnim = NULL;
    pet.scaleH = 0.2;
    pet.scaleW = 0.2;
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
    if((int)pet.cheer + (int)value <= 255){
        pet.cheer += value;
        pet.health = ((int)pet.health + value/10 > 255) ? 255 : pet.health + (char)value/10;
    } else {
        pet.cheer = 255;
    }
}

void add_satiety(unsigned char value)
{
    if((int)pet.satiety + (int)value <= 255){
        pet.satiety += value;
        pet.health = ((int)pet.health + value/10 > 255) ? 255 : pet.health + (char)value/10;
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

// Кулдавн между уведомлениями
static float notify_timer = 0.0;
// Глобальная переменная для счета времени чтобы изменить характеристики питомца
double gtimer; 
/**
 * @brief Обновление параметров питомца во время работы программы
 * 
 * @param delta   -- мс с предыдущего кадра
 * @param gtimer  -- перемена для глобального времени(там складываются милисекунды и накапливаются для таймера)
 * @param scaling -- насколько изменить парметры со временем
 */
void update_pet(double delta, float scaling)
{
    // Складываем и ждем секунды
    gtimer += delta;
    if (gtimer >= 10.0){
        if((int)pet.satiety - 2*scaling > 0){
            pet.satiety -= (unsigned char)2*scaling;
        }
        if((int)pet.cheer - 1*scaling > 0){
            pet.cheer -= (unsigned char)1*scaling;
        }
        // Изменение здоровья от характеристик 
        if(pet.cheer > 150 && pet.satiety > 80){
            pet.health = ((int)pet.health + 10*scaling > 255) ? 255 : pet.health + 10*scaling;
        } else{
            pet.
            health = ((int)pet.health - 3*scaling < 0) ? 0 : pet.health - 1*scaling;
        }
        gtimer = 0.0;
        
    }
    
    if (pet.health < 40){
        if(notify_timer >= 40.0){
            notify_user("Тамагочи", "У меня мало здоровья!");
            notify_timer=0.0;
        }

        notify_timer+=delta;
    }
}

// Отображение питомца
void show_pet(void)
{
    if(pet.health == 0){
        //! СМЕРТЬ
        notify_user("Тамагочи", "Хозяин я умер!");
        set_scene(&DEAD_SCENE);
    } else{
        pet.x = WINDOW_WIDTH/2-((int)(pet.w*pet.scaleW))/2;
        pet.y = WINDOW_HEIGHT/2-((int)(pet.h*pet.scaleH))/2;

        if(!pet.stayAnim){
            if(!pet.texture)
                pet.texture = loadTexture(pet.pathImage);

            renderTextureScaled(pet.texture, pet.x, pet.y, pet.scaleW, pet.scaleH);
        } else {
            renderAnimation(pet.stayAnim, pet.x, pet.y, pet.w*pet.scaleW, pet.h*pet.scaleH);
        }
    }
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
            fwrite(&pet.scaleW, sizeof(pet.scaleW), 1, file);
            fwrite(&pet.scaleH, sizeof(pet.scaleH), 1, file);

            fwrite(&lastSavedTime, sizeof(lastSavedTime), 1, file);
            
            bool isAnim;
            // Сохранение анимации если она есть у скина питомца
            if(pet.stayAnim) {
                // Указываем что анимация есть
                isAnim = true;
                fwrite(&isAnim, sizeof(bool), 1, file);

                // Сохраняем размер строки чтобы потом правильно считать ее
                len = strlen(pet.stayAnim->spriteSheetPath) + 1;  
                fwrite(&len, sizeof(len), 1, file);
                // Сохраняем путь до анимации
                fwrite(pet.stayAnim->spriteSheetPath, sizeof(char), len, file);

                // Общее количество кадров
                fwrite(&pet.stayAnim->frameCount, sizeof(int), 1, file);

                // Сохранение разметки анимации
                fwrite(pet.stayAnim->frames, sizeof(SDL_Rect), pet.stayAnim->frameCount, file);

                // Время кадра
                fwrite(&pet.stayAnim->frameTime, sizeof(float), 1, file);

            } else {
                isAnim = false;
                // Указываем что анимации нету
                fwrite(&isAnim, sizeof(bool), 1, file);
            }

            fclose(file);
    } else {
        SDL_Log("ERROR SAVE DATA! ");
    }
}

