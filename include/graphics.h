#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>

// Глобальные указатели на окно и рендерер (если они нужны во многих модулях)
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

// Инициализация графической подсистемы.
// title — заголовок окна, width и height — размеры окна.
// Возвращает 1 при успешной инициализации, 0 в случае ошибки.
int initGraphics(const char* title, int width, int height);

// Функция для загрузки текстуры из файла.
SDL_Texture* loadTexture(const char* filePath);

// Функция для отрисовки текстуры на экране.
// texture — указатель на текстуру, dstRect — область, куда выводить.
void renderTexture(SDL_Texture* texture, SDL_Rect* dstRect);

// Функция для обновления экрана.
void presentGraphics();

// Очистка ресурсов графической подсистемы.
void cleanupGraphics();

#endif // GRAPHICS_H
