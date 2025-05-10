#ifndef UI_H
#define UI_H

#include <SDL.h>
#include <stdbool.h>
#include <SDL_mixer.h>
#include "animation.h"

// Вытаскиваем базовый звук для нажатия на кнопку
extern Mix_Chunk *clickSound;

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
    bool isAnimatingClick;   // Флаг, указывающий, что проигрывается анимация клика
    float clickAnimTimer;    // Таймер анимации
    float clickAnimDuration; // Длительность анимации клика (например, 0.3 секунды)
    Mix_Chunk *clickSound;   // Звук при нажатие на кнопку
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
 */
void initButton(Button *button,
                int x, int y, int w, int h,
                char *defaultImagePath,
                char *hoverImagePath,
                char *clickImagePath,
                void (*onClick)(void),
                Mix_Chunk *clickSound);


/**
 * @brief Нарезает png картинку на анимации в длину, 
 * чтобы создать анимацию из 4 кадров длиной и широй 300пх надо иметь картинку 1200пх (300*4 в длинну)
 * нарезка происходит слева направа в заданых в slice_w(ширина) и slice_h(высота) пикселей
 * 
 * @param button     -- Указатель на кнопку в чью структуру добавят анимацию
 * @param animPath   -- Путь до файла с анимацией (фото .png)
 * @param quantity   -- Количество кадров в анимации
 * @param frametime  -- Время одного кадра
 * @param slice_w    -- Ширина для нарзеки(не ширина проигрываемой анимации)
 * @param slice_h    -- Высота нарезки
 */
void initButtonAnimation(Button *button, char* animPath,const int quantity, float frametime, int slice_w, int slice_h);

/**
 * Занимаеться анимацией
 */
void updateButton(Button *button, float delta);

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
