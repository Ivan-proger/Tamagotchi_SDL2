#include <SDL2/SDL.h>
#include "graphics.h"
#include "pet.h"
#include "menu_scene.h"
#include "dead_scene.h"
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   




// Кусочки тамагочи
static SDL_Texture** petTextures;
static SDL_Rect dstRects[4];
static int i;

// Кнопка старт
static Button exitButton;

// Фон
static SDL_Texture* background;
static SDL_Rect rectdict;

// Предполагается, что petTextures — массив из 4 указателей на SDL_Texture*, полученный из splitTextureFour,
static void deadAnimation(SDL_Rect dstRects[4], SDL_Texture **petTextures, int startX, int startY, float scaleW, float scaleH, int gap) {
    int texW, texH;

    // Получаем размеры одной текстуры (предполагается, что все имеют одинаковый размер)
    sizeTexture(petTextures[0], &texW, &texH);
    texW *= scaleW;
    texH *= scaleH;
    // Определяем прямоугольники для каждой текстуры с промежутком (gap)
    
    
    // Верхний левый элемент
    dstRects[0].x = startX - gap;
    dstRects[0].y = startY-gap;
    dstRects[0].w = texW;
    dstRects[0].h = texH;
    
    // Верхний правый элемент
    dstRects[1].x = startX + texW + gap;
    dstRects[1].y = startY-gap;
    dstRects[1].w = texW;
    dstRects[1].h = texH;
    
    // Нижний левый элемент
    dstRects[2].x = startX - gap;
    dstRects[2].y = startY + texH + gap;
    dstRects[2].w = texW;
    dstRects[2].h = texH;
    
    // Нижний правый элемент
    dstRects[3].x = startX + texW + gap;
    dstRects[3].y = startY + texH + gap;
    dstRects[3].w = texW;
    dstRects[3].h = texH;
    
    // Отрисовка всех текстур
    for (int i = 0; i < 4; i++) {
        renderTexture(petTextures[i], &dstRects[i]);
    }
}

// Метод для удаления меню
static void dead_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&exitButton);

    // Очищаем фон
    if (background) {
        SDL_DestroyTexture(background);
        background = NULL;
    }

    // удаляем питомца
    invisible_pet();

    // Очистка текстур
    if (petTextures != NULL) {
        // Предполагаем что массив заканчивается NULL-указателем
        for(int i = 0; i < 4; i++) {
            if (petTextures[i] != NULL) {
                SDL_DestroyTexture(petTextures[i]);
                petTextures[i] = NULL;
            }
        }
        
        // Освобождаем сам массив указателей
        free(petTextures);
        petTextures = NULL;
    }
}

// Реакция на нажатаю кнопку
static void onexitButtonClick() {
    // Логика при нажатии кнопки
    //dead_destroy();
    set_scene(&MENU_SCENE);
}

// Инициализация меню(его создание и отображение)
static void dead_init() {
    // Загрузка фона
    background = loadTexture("assets/Background.png");

    // Инициализация кнопки (координаты, размеры)
    if (!initButton(&exitButton,
        WINDOW_WIDTH/2-250, WINDOW_HEIGHT-100, 250, 150,
        "assets/button_start.png",
        "assets/button_start_watch.png", // используем default для hover
        "assets/button_start_click.png", // используем default для click
        onexitButtonClick))
    {
    SDL_Log("Ошибка инициализации кнопки!");
    }

    i = 1;

    // Разделяем на эллементы
    petTextures = splitTextureFour(pet.pathImage);
    if(!(pet.x) && !(pet.y)){
        pet.x = WINDOW_WIDTH/2-((int)(pet.w*pet.scaleW))/2;
        pet.y = WINDOW_HEIGHT/2-((int)(pet.h*pet.scaleH))/2;        
    }
    deadAnimation(dstRects, petTextures, pet.x, pet.y, pet.scaleW, pet.scaleH, 1);
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void dead_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&exitButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void dead_update(float delta) {
    updateButton(&exitButton, delta);
}


// Отображение меню(его статической состоявляющей)
static void dead_render() {
    // Фон
    rectdict.w = WINDOW_WIDTH;
    rectdict.h = WINDOW_HEIGHT;
    renderTexture(background, &rectdict);

    // Затемнить фон
    drawTransparentBlackSquare(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Рисуем анимацию и т.д.
    deadAnimation(dstRects, petTextures, pet.x, pet.y,pet. scaleW, pet.scaleH, i/2);
    i++;

    // Рисуем кнопку
    exitButton.rect.x = WINDOW_WIDTH/2-(250/2);
    exitButton.rect.y = WINDOW_HEIGHT-150;

    renderButton(&exitButton);
}


// Объект сцены
Scene DEAD_SCENE = {
    .init = dead_init,
    .handle_events = dead_handle_events,
    .update = dead_update,
    .render = dead_render,
    .destroy = dead_destroy
};
