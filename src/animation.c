
#include <stdlib.h>
#include "animation.h"
#include "graphics.h"

// Создание анимации: копирование массива кадров и установка начальных значений
Animation* createAnimation(SDL_Texture *spriteSheet, SDL_Rect *frames, int frameCount, float frameTime) {
    Animation *anim = (Animation*) malloc(sizeof(Animation));
    if (!anim){
        SDL_Log("Ошибка получения анимации текстуры: %s", SDL_GetError());
        return NULL;
    }
    anim->spriteSheet = spriteSheet;
    anim->frameCount = frameCount;
    anim->frameTime = frameTime;
    anim->elapsedTime = 0.0f;
    anim->currentFrame = 0;

    // Выделяем память для копирования массива кадров
    anim->frames = (SDL_Rect*) malloc(sizeof(SDL_Rect) * frameCount);
    if (!anim->frames) {
        SDL_Log("Ошибка выделения памяти анимации: %s", SDL_GetError());
        free(anim);
        return NULL;
    }
    for (int i = 0; i < frameCount; i++) {
        anim->frames[i] = frames[i];
    }

    return anim;
}

// Создание анимации без массива REct для каждого кадра (подразумевается что каждый кдр одинаковый)
Animation* createAnimationOneType(SDL_Texture *spriteSheet, int wight, int hight, int frameCount, float frameTime) {
    Animation *anim = (Animation*) malloc(sizeof(Animation));
    if (!anim){
        SDL_Log("Ошибка получения анимации текстуры: %s", SDL_GetError());
        return NULL;
    }
    anim->spriteSheet = spriteSheet;
    anim->frameCount = frameCount;
    anim->frameTime = frameTime;
    anim->elapsedTime = 0.0f;
    anim->currentFrame = 0;

    // Выделяем память для копирования массива кадров
    anim->frames = (SDL_Rect*) malloc(sizeof(SDL_Rect) * frameCount);
    if (!anim->frames) {
        SDL_Log("Ошибка выделения памяти анимации: %s", SDL_GetError());
        free(anim);
        return NULL;
    }
    for (int i = 0; i < frameCount; i++) {
        // Нарезка
        SDL_Rect frame = {i*wight, 0, wight, hight};
        anim->frames[i] = frame;
    }

    return anim;
}

// Освобождение памяти, выделенной для анимации
void destroyAnimation(Animation *anim) {
    if (!anim) return;
    if (anim->frames) {
        free(anim->frames);
        anim->frames = NULL;
    }
    free(anim);
}

// Обновление анимации: накапливаем время и переключаем кадр, если оно превышает заданное время кадра
void updateAnimation(Animation *anim, float delta) {
    if (!anim){ 
        SDL_Log("Ошибка получения анимации текстуры: %s", SDL_GetError());
        return;
    }
    anim->elapsedTime += delta;
    if (anim->elapsedTime >= anim->frameTime) {
        anim->currentFrame = (anim->currentFrame + 1) % anim->frameCount;
        anim->elapsedTime = 0.0f;
    }
}

// Отрисовка текущего кадра анимации с заданным масштабом
void renderAnimation(Animation *anim, int x, int y, int w, int h) {
    if (!anim){ 
        SDL_Log("Ошибка получения анимации текстуры: %s", SDL_GetError());
        return;
    }
    SDL_Rect src = anim->frames[anim->currentFrame];
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_RenderCopy(gRenderer, anim->spriteSheet, &src, &dst);
}
