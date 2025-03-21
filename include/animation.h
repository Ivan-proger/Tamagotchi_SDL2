#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>

// Структура анимации
typedef struct {
    SDL_Texture *spriteSheet;  // Спрайт-лист, содержащий все кадры анимации.
    SDL_Rect *frames;          // Массив областей (кадров) в спрайт-листе.
    int frameCount;            // Общее количество кадров.
    int currentFrame;          // Индекс текущего кадра.
    float frameTime;           // Время (в секундах) показа одного кадра.
    float elapsedTime;         // Накопленное время для переключения кадров.
} Animation;

/**
 * Создаёт новую анимацию.
 *
 * @param spriteSheet Указатель на SDL_Texture со спрайт-листом.
 * @param frames Массив областей (SDL_Rect), определяющих кадры.
 * @param frameCount Количество кадров в массиве.
 * @param frameTime Время показа одного кадра (в секундах).
 * @return Указатель на созданную анимацию или NULL при ошибке.
 */
Animation* createAnimation(SDL_Texture *spriteSheet, SDL_Rect *frames, int frameCount, float frameTime);

/**
 * Освобождает память, занятую анимацией.
 * Обратите внимание: сама текстура спрайт-листа не освобождается,
 * так как может использоваться и в других объектах.
 *
 * @param anim Указатель на анимацию.
 */
void destroyAnimation(Animation *anim);

/**
 * Обновляет анимацию.
 *
 * @param anim Указатель на анимацию.
 * @param delta Время, прошедшее с предыдущего обновления (в секундах).
 */
void updateAnimation(Animation *anim, float delta);

/**
 * Отрисовывает текущий кадр анимации.
 *
 * @param renderer SDL_Renderer для отрисовки.
 * @param anim Указатель на анимацию.
 * @param x, y Координаты, где будет отрисована анимация.
 * @param w, Ширина высота.
 */
void renderAnimation(Animation *anim, int x, int y, int w, int h);

#endif // ANIMATION_H
