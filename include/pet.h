#ifndef PET_H
#define PET_H

#include <SDL2/SDL.h>

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

extern Pet pet;

void init_pet(void);
void show_pet(void);
void invisible_pet(void);
void load_texture_pet(void);
void add_cheer(unsigned char value);
void add_satiety(unsigned char value);
void save_game(void);
void load_game(void);

#endif