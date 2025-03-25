#include "../include/graphics.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

// Глобальные переменные для работы с графикой из вне
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

/**
 * @brief Инициализация SDL
 * 
 * @param title   -- Имя окна.
 * @param width   -- Ширина окна
 * @param height  -- Высота Окна
 * @return int    -- Отдает 0 если ошибка
 */
int initGraphics(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Ошибка инициализации SDL: %s\n", SDL_GetError());
        return 0;
    }
    // Если требуется работа с изображениями, инициализируем SDL_image:
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        printf("Ошибка инициализации SDL_image: %s\n", IMG_GetError());
        return 0;
    }
    
    gWindow = SDL_CreateWindow(title,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               width, height,
                               SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);
    if (!gWindow) {
        printf("Не удалось создать окно: %s\n", SDL_GetError());
        return 0;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        printf("Не удалось создать рендерер: %s\n", SDL_GetError());
        return 0;
    }
    return 1;
}

SDL_Texture* loadTexture(const char* filePath) {
    SDL_Texture* texture = IMG_LoadTexture(gRenderer, filePath);
    if (!texture) {
        printf("Ошибка загрузки текстуры (%s): %s\n", filePath, IMG_GetError());
    }
    return texture;
}
/**
 * @brief Получаем изначальные размеры текстуры
 * 
 * @param texture         -- Текстура чьи данные мы хотим получить
 * @param originalWidth   -- Переменная в которую запишем ширину
 * @param originalHeight  -- Переменная в которую запишем высоту
 */
void sizeTexture(SDL_Texture* texture, int* originalWidth, int* originalHeight) {
    // Получаем исходные размеры текстуры
    if (SDL_QueryTexture(texture, NULL, NULL, originalWidth, originalHeight) != 0) {
        SDL_Log("Ошибка получения размеров текстуры: %s", SDL_GetError());
        return;
    }
}

/**
 * @brief Функция для отрисовки текстуры с масштабированием
 * 
 * @param texture - Текстура
 * @param x 
 * @param y 
 * @param scaleX  - скейлинг по х
 * @param scaleY  - скейлинг по y
 */
void renderTextureScaled(SDL_Texture* texture, int x, int y, float scaleX, float scaleY) {
    int originalWidth, originalHeight;
    // Получаем исходные размеры текстуры
    sizeTexture(texture, &originalWidth, &originalHeight);

    // Определяем область источника (весь спрайт)
    SDL_Rect srcRect = { 0, 0, originalWidth, originalHeight };

    // Вычисляем область назначения с учётом масштаба
    SDL_Rect dstRect;
    dstRect.x = x;
    dstRect.y = y;
    dstRect.w = (int)(originalWidth * scaleX);
    dstRect.h = (int)(originalHeight * scaleY);

    // Отрисовываем текстуру в указанной области
    SDL_RenderCopy(gRenderer, texture, &srcRect, &dstRect);
}

/**
 * @brief Глобальная функция для рендеринга 
 * 
 * @param texture -- Текстура которую отображаем.
 * @param dstRect -- Структура позиции текстуры.
 */
void renderTexture(SDL_Texture* texture, SDL_Rect* dstRect) {
    // Если нужно использовать источник (часть текстуры), можно добавить дополнительный параметр.
    SDL_RenderCopy(gRenderer, texture, NULL, dstRect);
}


// @brief Обновления экрана 
void presentGraphics() {
    SDL_RenderPresent(gRenderer);
}

// @brief Очистка графики
void cleanupGraphics() {
    if (gRenderer) {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = NULL;
    }
    if (gWindow) {
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
    }
    IMG_Quit();
    SDL_Quit();
}

//gfx


void filledRoundedRectangleRGBA(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    // Ограничиваем радиус, чтобы он не превышал половину ширины или высоты
    // Вычисляем размеры
    Sint16 width = x2 - x1;
    Sint16 height = y2 - y1;
    // Ограничиваем радиус, чтобы он не превышал половину ширины или высоты
    if (radius * 2 > width)  radius = width / 2;
    if (radius * 2 > height) radius = height / 2;

    // Устанавливаем цвет для заливки
    SDL_SetRenderDrawColor(gRenderer, r, g, b, a);

    // 1. Заливка центрального прямоугольника (без учета закругленных углов)
    SDL_Rect centralRect = { x1 + radius, y1, width - 2 * radius, height };
    SDL_RenderFillRect(gRenderer, &centralRect);

    // 2. Заливка боковых частей (без углов)
    SDL_Rect leftRect  = { x1, y1 + radius, radius, height - 2 * radius };
    SDL_Rect rightRect = { x2 - radius, y1 + radius, radius, height - 2 * radius };
    SDL_RenderFillRect(gRenderer, &leftRect);
    SDL_RenderFillRect(gRenderer, &rightRect);

    // 3. Заливка углов с помощью заполненных кругов
    filledCircleRGBA(gRenderer, x1 + radius, y1 + radius, radius, r, g, b, a);
    filledCircleRGBA(gRenderer, x2 - radius, y1 + radius, radius, r, g, b, a);
    filledCircleRGBA(gRenderer, x1 + radius, y2 - radius, radius, r, g, b, a);
    filledCircleRGBA(gRenderer, x2 - radius, y2 - radius, radius, r, g, b, a);

    // 4. Отрисовка антиалиасингового контура для сглаживания краёв
    // Рисуем прямые линии для верхней, нижней, левой и правой сторон
    aalineRGBA(gRenderer, x1 + radius, y1, x2 - radius, y1, r, g, b, a);
    aalineRGBA(gRenderer, x1 + radius, y2, x2 - radius, y2, r, g, b, a);
    aalineRGBA(gRenderer, x1, y1 + radius, x1, y2 - radius, r, g, b, a);
    aalineRGBA(gRenderer, x2, y1 + radius, x2, y2 - radius, r, g, b, a);

    // Рисуем сглаженные дуги для каждого угла
    aacircleRGBA(gRenderer, x1 + radius, y1 + radius, radius, r, g, b, a);
    aacircleRGBA(gRenderer, x2 - radius, y1 + radius, radius, r, g, b, a);
    aacircleRGBA(gRenderer, x1 + radius, y2 - radius, radius, r, g, b, a);
    aacircleRGBA(gRenderer, x2 - radius, y2 - radius, radius, r, g, b, a);
}