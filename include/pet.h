#ifndef PET_H
#define PET_H

#include "animation.h"
#include <SDL.h>
#include <stdbool.h>


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
    char   *name;          // Динамический буфер для имени
    SDL_Texture* texture; // Текстура 
    Animation* stayAnim;   // АФК анимация 
    SDL_Texture* textureWithBone; // Текстура с костью в зубах
    char* pathImageWithBone; // Путь до картинки с костью
    char* pathImage;      // Текущие изображение питомца
    int x;               // Позиция
    int y;
    int w;                // Ширина
    int h;                // Высота   
    float scaleW; // Коэффициент сужения/расширения по ширине
    float scaleH; // Коэффициент сужения/расширения по высоте
    unsigned char health;           // Здоровье чудика
    unsigned char satiety;           // Голод
    unsigned char cheer;            // Настроение
    long long timeLife;  // Общение время жизни
    int id;              // Номер питомца
} Pet;

// Питомец
//extern Pet pet;

// Создания питомца и загрузка его сохранения
void init_pet(int id);
// Возвращает id питомца
void setIdPet(int id);

// Возвращает путь (относительный) до статичной картинки 
char* getTexturePet();

// Установить новую текстуру питомца (Статичное изображение, изображение во время кормления и анимация(по желанию))
void setTexturePet(char* path, float scaleW, float scaleH, char* pathImageWithBone, Animation* anim);

// Возвращает id питомца
int getIdPet();

// Возвращает шириину изображения питомца
int getWidthPet();
// Возвращает высоту изображения питомца
int getHighPet();

// Возвращает коэффициент ширины изображения
float getScaleW();
// Возвращает коэффициент высоты изображения
float getScaleH();

// Возвращает имя питомца
char* getNamePet();

// Возвращает x координату
int getXpet();
// Возвращает y координату
int getYpet();

// Получить здоровье питомца
unsigned char getHealthPet();
// Получить настроение питомца
unsigned char getCheerPet();
// Получить насыщенность питомца
unsigned char getSatietyPet();



// Ставит питомца чуть ниже середины экрана нужно чтобы адаптировать игру под разные разрешения
void centeringImagePet();

// если у питомца есть анимация обновляем ее
void isAnimationPetUpdate(float delta);

// Показывать питомца посреди экрана
void showPet(bool isFeed);

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

// Смена имени питомца
void setNamePet(const char *new_name);

// Сохранить данные о питомце
void save_game();

#endif