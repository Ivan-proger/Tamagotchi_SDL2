#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>

#include <SDL2/SDL.h>

// Глобальные указатели на окно и рендерер (если они нужны во многих модулях)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

// Инициализация графической подсистемы.
// title — заголовок окна, width и height — размеры окна.
// Возвращает true при успешной инициализации, false в случае ошибки.
bool initGraphics(const char* title, int width, int height);

// Функция для загрузки текстуры из файла.
SDL_Texture* loadTexture(const char* filePath);

// Функция для отрисовки текстуры на экране.
// texture — указатель на текстуру, dstRect — область, куда выводить.
void renderTexture(SDL_Texture* texture, SDL_Rect* dstRect);

//Получаем изначальные размеры текстуры
void sizeTexture(SDL_Texture* texture, int* originalWidth, int* originalHeight);

// Функция для отрисовки текстуры с масштабированием
void renderTextureScaled(SDL_Texture* texture, int x, int y, float scaleX, float scaleY);

// Функция для обновления экрана.
void presentGraphics();

// Делает текстуру серой и полупрозрачной
void applyGrayTransparency(SDL_Texture* texture, Uint8 alpha);

// Очистка ресурсов графической подсистемы.
void cleanupGraphics();

/**
 * @brief Рисует округленую обводку 
 * 
 * @param x1 - 1 точка ее x
 * @param y1 - 1 точка ее y
 * @param x2 - 2 точка ее x
 * @param y2 - 2 точка ее y
 * @param radius - радиус округления
 * @param r - красный
 * @param g - зеленый
 * @param b - синий
 * @param a - альфа канал (прозрачность)
 */
void filledRoundedRectangleRGBA(Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// Функция разбивает изображение, загруженное из imagePath, на 4 равные части и возвращает массив текстур.
// Если происходит ошибка, функция возвращает NULL.
SDL_Texture** splitTextureFour(const char* imagePath);

// Полупрозрачный прямоугольник черного цвета
void drawTransparentBlackSquare(int x, int y, int w, int h);

#endif // GRAPHICS_H
