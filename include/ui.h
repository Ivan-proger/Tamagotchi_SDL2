#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "animation.h"

// Структура кнопки
typedef struct {
    SDL_Rect rect;                // Позиция и размеры кнопки
    SDL_Texture *defaultTexture;  // Текстура в обычном состоянии
    SDL_Texture *hoverTexture;    // Текстура при наведении курсора
    SDL_Texture *clickTexture;    // Текстура при нажатии
    Animation *clickAnim;         // Поле для анимации при клике
    void (*onClick)(void);        // Функция-обработчик нажатия
    bool isHovered;               // Флаг: курсор внутри кнопки
    bool isClicked;               // Флаг: кнопка нажата
} Button;

/**
 * Инициализирует кнопку.
 * 
 * @param button       Указатель на структуру кнопки.
 * @param renderer     Рендерер SDL.
 * @param x, y, w, h   Позиция и размеры кнопки.
 * @param defaultImagePath  Путь к изображению для обычного состояния.
 * @param hoverImagePath    Путь к изображению для состояния "наведено" (может быть NULL, тогда используется default).
 * @param clickImagePath    Путь к изображению для состояния "нажато" (может быть NULL, тогда используется default).
 * @param onClick      Функция, вызываемая при нажатии на кнопку.
 * @return true, если инициализация успешна, иначе false.
 */
bool initButton(Button *button,
                int x, int y, int w, int h,
                const char *defaultImagePath,
                const char *hoverImagePath,
                const char *clickImagePath,
                void (*onClick)(void));

/**
 * Отрисовывает кнопку.
 */
void renderButton(Button *button);

/**
 * Обрабатывает события (мыши) для кнопки.
 */
void handleButtonEvent(Button *button, SDL_Event *event);

/**
 * Освобождает ресурсы, связанные с кнопкой.
 */
void destroyButton(Button *button);

#endif
