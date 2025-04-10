#ifndef PET_H
#define PET_H

#include <SDL2/SDL.h>


/**
 * @brief Структура питомца
 * 
 * @param SDL_Texture* texture: Текстура 
 * @param char* pathImage: Текущие изображение питомца
 * @param int x: Позиция
 * @param int y
 * @param int w: Ширина
 * @param int h: Высота
 * @param float scaleW: Коэфицент сужения/расширения по ширине
 * @param float scaleH: Коэфицент сужения/расширения по высоте
 * @param unsigned_char health: Здоровье чудика
 * @param unsigned_char satiety: Голод
 * @param unsigned_char cheer: Настроение
 */
typedef struct
{
    SDL_Texture* texture; // Текстура 
    char* pathImage;      // Текущие изображение питомца
    int x;               // Позиция
    int y;
    int w;                // Ширина
    int h;                // Высота   
    float scaleW;           // Коэфицент сужения/расширения по ширине
    float scaleH;           // Коэфицент сужения/расширения по высоте
    unsigned char health;           // Здоровье чудика
    unsigned char satiety;           // Голод
    unsigned char cheer;            // Настроение
} Pet;

// Питомец
extern Pet pet;

// Создания питомца и загрузка его сохранения
void init_pet(void);

// Показывать питомца посреди экрана
void show_pet(void);

// Удалить питомца с экрана - перестать показывать и очистить память всвязи с этим
void invisible_pet(void);

// Загрузить текстуру питомца
void load_texture_pet(void);

// Добавить настроение питомцу, value - на сколько едениц добавить (максимум 255)
void add_cheer(unsigned char value);

// Добавить насыщеность, value - на сколько едениц добавить (максимум 255)
void add_satiety(unsigned char value);

// Отрисовка анимации
void update_pet(double delta, float scaling);

// Сохранить данные о питомце
void save_game(void);

#endif