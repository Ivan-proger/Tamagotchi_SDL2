#include "../include/graphics.h"
#include <stdio.h>
#include <SDL2/SDL_image.h>

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

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
                               SDL_WINDOW_SHOWN);
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

void renderTexture(SDL_Texture* texture, SDL_Rect* dstRect) {
    // Если нужно использовать источник (часть текстуры), можно добавить дополнительный параметр.
    SDL_RenderCopy(gRenderer, texture, NULL, dstRect);
}

void presentGraphics() {
    SDL_RenderPresent(gRenderer);
}

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
