#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include "pet.h"
#include "SDL_error.h"
#include "SDL_render.h"
#include "animation.h"
#include "graphics.h"
#include "scene_manager.h"
#include "dead_scene.h"
#include "globals.h"
#include "notify.h"


char* get_save_path(const char* filename);

Pet pet;

time_t lastSavedTime;

//! Инициализация питомца (и загрузка из файла в будущем)
void init_pet(int id)
{
    const char *prefix = "save№";
    const char *suffix = ".dat";

    // Вычисляем необходимую длину для итоговой строки
    size_t len = snprintf(NULL, 0, "%s%d%s", prefix, id, suffix);
    char *filename = malloc(len + 1); // +1 для нулевого терминатора

    if (filename) {
        snprintf(filename, len + 1, "%s%d%s", prefix, id, suffix);
    } else {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return;
    }

    FILE *file = fopen(get_save_path(filename), "rb");
    if(file) {
        // Загружаем длину строки pathImage
        size_t len;
        if(!(fread(&len, sizeof(len), 1, file))){
            SDL_Log("Ошибка загрузки длины строки для пути изображения %s", SDL_GetError());
            fclose(file);
        }
        // Выделяем память для pathImage
        pet.pathImage = (char*)malloc(len);
        // Загружаем строку pathImage
        if(!(fread(pet.pathImage, sizeof(char), len, file))){
            SDL_Log("Ошибка загрузки строки%s", SDL_GetError());
            fclose(file);
        }

        // Загружаем картинку с костью в зубах
        if(!(fread(&len, sizeof(len), 1, file))){
            SDL_Log("Ошибка загрузки длины строки для пути изображения с костью %s", SDL_GetError());
            fclose(file);
        }
        // Выделяем память для pathImageWithBone
        pet.pathImageWithBone = (char*)malloc(len);
        // Загружаем строку pathImageWithBone
        if(!(fread(pet.pathImageWithBone, sizeof(char), len, file))){
            SDL_Log("Ошибка загрузки строки%s", SDL_GetError());
            fclose(file);
        }

        // Считываем имя питомца
        if(!(fread(&len, sizeof(len), 1, file))){
            SDL_Log("Ошибка загрузки длины строки для имени питомца %s", SDL_GetError());
            fclose(file);
        }
        pet.name = malloc(len);
        if(!(fread(pet.name, sizeof(char), len, file))){
            SDL_Log("Ошибка загрузки имени питомца %s", SDL_GetError());
            fclose(file);
        }

        if(!(fread(&pet.health, sizeof(unsigned char), 1, file))){
            SDL_Log("Ошибка загрузки здоровья%s", SDL_GetError());
            fclose(file);
        }
        if(!(fread(&pet.satiety, sizeof(unsigned char), 1, file))){
            SDL_Log("Ошибка загрузки насыщенности %s", SDL_GetError());
            fclose(file);
        }
        if(!(fread(&pet.cheer, sizeof(unsigned char), 1, file))){
            SDL_Log("Ошибка загрузки настроения%s", SDL_GetError());
            fclose(file);
        }
        if(!(fread(&pet.scaleW, sizeof(pet.scaleW), 1, file))){
            SDL_Log("Ошибка загрузки масштаба в ширину%s", SDL_GetError());
            fclose(file);
        }
        if(!(fread(&pet.scaleH, sizeof(pet.scaleH), 1, file))){
            SDL_Log("Ошибка загрузки масштаба в высоту%s", SDL_GetError());
            fclose(file);
        }

        if(!(fread(&lastSavedTime, sizeof(lastSavedTime), 1, file))){
            SDL_Log("Ошибка загрузки последнего времени сохранения%s", SDL_GetError());
            fclose(file);
        }

        // Считываем время жизни из сохранения
        if(!(fread(&pet.timeLife, sizeof(long long), 1, file))){
            SDL_Log("Ошибка загрузки общего времени жизни %s", SDL_GetError());
            fclose(file);
        }

        // Считываем анимацию если есть
        bool isAnim;
        if(!(fread(&isAnim, sizeof(bool), 1, file))){
            SDL_Log("Ошибка загрузки наличия у питомца анимации%s", SDL_GetError());
            fclose(file);
        }
        if(isAnim) {
            // Выделяем память под структуру Animation
            pet.stayAnim = malloc(sizeof(Animation));
            if (!pet.stayAnim) {
                SDL_Log("Ошибка выделения памяти для stayAnim!");
                return;
            }
            // Считываем длину строки
            if(!(fread(&len, sizeof(len), 1, file))){
                SDL_Log("Ошибка загрузки длины строки для пути изображения анимации%s", SDL_GetError());
                fclose(file);
            }

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
            } else {pet.stayAnim->spriteSheet = loadTexture(pet.stayAnim->spriteSheetPath);}

            // Общее количество кадров
            if(!(fread(&pet.stayAnim->frameCount, sizeof(int), 1, file))){
                SDL_Log("Ошибка загрузки числа кадров анимации %s", SDL_GetError());
                fclose(file);
            }

            // Выделяем память под кадры
            pet.stayAnim->frames = malloc(pet.stayAnim->frameCount * sizeof(SDL_Rect));
            if (!pet.stayAnim->frames) {
                SDL_Log("Не удалось выделить память под кадры");
                fclose(file);
                return;
            }
            if(!(fread(pet.stayAnim->frames, sizeof(SDL_Rect), pet.stayAnim->frameCount, file))){
                SDL_Log("Ошибка загрузки SDL_Rect для анимации %s", SDL_GetError());
                fclose(file);
                return; 
            } 

            // Время кадра
            if(!(fread(&pet.stayAnim->frameTime, sizeof(float), 1, file))){
                SDL_Log("Ошибка загрузки времени кадра %s", SDL_GetError());
                fclose(file);
            }

            // Ставим в 0
            pet.stayAnim->currentFrame = 0;
            pet.stayAnim->elapsedTime = 0.0;

        } else {
            // Точно в нул ставим
            if(pet.stayAnim)
                destroyAnimation(pet.stayAnim);
            pet.stayAnim = NULL;
        }


        fclose(file);

        time_t currentTime = time(NULL);
        // timeDiff - Секнуды с момента последнего сохранения 
        int timeDiff = difftime(currentTime, lastSavedTime);
        // Добавляем время жизни
        pet.timeLife += timeDiff;

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

        free(filename);
        return;
    }    
    pet.name = malloc(1);
    pet_set_name("dog");    
    pet.pathImage = "pets/pet.png";
    pet.pathImageWithBone = "pets/pet_bone.png";
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
    pet.textureWithBone = loadTexture(pet.pathImageWithBone);
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
        pet.timeLife += gtimer;
        gtimer = 0.0;
    }
    
    if(notify_timer >= 10.0){
        if (pet.health < 40){
            notify_user(pet.name, "У меня мало здоровья!");
            notify_timer=0.0;
        }
        if (pet.cheer >= 250){
            notify_user(pet.name, "Я полностью доволен");
            notify_timer=0.0;
        }
        if (pet.satiety >= 250){
            notify_user(pet.name, "Я больше не голоден");
            notify_timer=0.0;
        }
    }
    notify_timer+=delta;
}

// Отображение питомца
void show_pet(bool isFeed)
{
    if(pet.health == 0){
        //! СМЕРТЬ
        notify_user("Тамагочи", "Хозяин я умер!");
        set_scene(&DEAD_SCENE);
    } else{
        pet.x = WINDOW_WIDTH/2-((int)(pet.w*pet.scaleW))/2;
        pet.y = WINDOW_HEIGHT/2-((int)(pet.h*pet.scaleH))/2+50;

        // Если собачка ест
        if(isFeed && pet.pathImageWithBone){
            if(pet.textureWithBone == NULL)
                loadTexture(pet.pathImageWithBone);
            renderTextureScaled(pet.textureWithBone, pet.x, pet.y, pet.scaleW, pet.scaleH);

            return;
        }

        if(!pet.stayAnim){
            if(!pet.texture)
                pet.texture = loadTexture(pet.pathImage);

            renderTextureScaled(pet.texture, pet.x, pet.y, pet.scaleW, pet.scaleH);
        } else {
            renderAnimation(pet.stayAnim, pet.x, pet.y, (int)(pet.w*pet.scaleW), (int)(pet.h*pet.scaleH));
        }
    }
}

// Смена имени питомца
void pet_set_name(const char *new_name) {
    // Вычисляем длину новой строки
    size_t new_len = strlen(new_name);

    size_t want = new_len + 1;
    char *tmp = realloc(pet.name, want);
    if (!tmp) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Pet_SetName: realloc(%zu) failed\n", want);
        return;
    }
    pet.name = tmp;
    memcpy(pet.name, new_name, want);
}

// Удаление сцены
void invisible_pet(void) {
    if (pet.texture) {
        SDL_DestroyTexture(pet.texture);
        pet.texture = NULL;
    }
    if (pet.textureWithBone) {
        SDL_DestroyTexture(pet.textureWithBone);
        pet.textureWithBone = NULL;
    }
}

void save_game(int id) {
    lastSavedTime = time(NULL);

    const char *prefix = "save№";
    const char *suffix = ".dat";

    // Вычисляем необходимую длину для итоговой строки
    size_t len = snprintf(NULL, 0, "%s%d%s", prefix, id, suffix);
    char *filename = malloc(len + 1); // +1 для нулевого терминатора

    snprintf(filename, len + 1, "%s%d%s", prefix, id, suffix);

    FILE *file = fopen(get_save_path(filename), "wb");
        if (file) {
            // Сохраняем длину строки pathImage
            size_t len = strlen(pet.pathImage) + 1;  // включая терминальный ноль
            fwrite(&len, sizeof(len), 1, file);

            // Сохраняем саму строку pathImage
            fwrite(pet.pathImage, sizeof(char), len, file);

            len = strlen(pet.pathImageWithBone) + 1;  // включая терминальный ноль
            fwrite(&len, sizeof(len), 1, file);
            fwrite(pet.pathImageWithBone, sizeof(char), len, file);

            len = strlen(pet.name) + 1;  // включая терминальный ноль
            fwrite(&len, sizeof(len), 1, file);
            fwrite(pet.name, sizeof(char), len, file);

            fwrite(&pet.health, sizeof(unsigned char), 1, file);
            fwrite(&pet.satiety, sizeof(unsigned char), 1, file);
            fwrite(&pet.cheer, sizeof(unsigned char), 1, file);
            fwrite(&pet.scaleW, sizeof(pet.scaleW), 1, file);
            fwrite(&pet.scaleH, sizeof(pet.scaleH), 1, file);

            fwrite(&lastSavedTime, sizeof(lastSavedTime), 1, file);

            // Считываем время жизни из сохранения
            fwrite(&pet.timeLife, sizeof(long long), 1, file);
            
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

            free(filename);

            fclose(file);
    } else {
        SDL_Log("ERROR SAVE DATA! ");
    }
}

