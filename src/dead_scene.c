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
static int i;

// Кнопка старт
static Button exitButton;

// Фон
static SDL_Texture* background;
static SDL_Rect rectdict;

// Предполагается, что petTextures — массив из 4 указателей на SDL_Texture*, полученный из splitTextureFour,
void deadAnimation(SDL_Texture **petTextures, int startX, int startY, int gap) {
    int texW=300;
    int texH=300;
    // Получаем размеры одной текстуры (предполагается, что все имеют одинаковый размер)
    //sizeTexture(petTextures[0], &texW, &texH);
    
    // Определяем прямоугольники для каждой текстуры с промежутком (gap)
    SDL_Rect dstRects[4];
    
    // Верхний левый элемент
    dstRects[0].x = startX;
    dstRects[0].y = startY;
    dstRects[0].w = texW;
    dstRects[0].h = texH;
    
    // Верхний правый элемент
    dstRects[1].x = startX + texW + gap;
    dstRects[1].y = startY;
    dstRects[1].w = texW;
    dstRects[1].h = texH;
    
    // Нижний левый элемент
    dstRects[2].x = startX;
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
}

// Реакция на нажатаю кнопку
void onexitButtonClick() {
    // Логика при нажатии кнопки
    dead_destroy();
    set_scene(&MENU_SCENE);
}

// Инициализация меню(его создание и отображение)
static void dead_init() {
    // Загрузка фона
    background = loadTexture("assets/Background.png");

    // Инициализация кнопки (координаты, размеры)
    if (!initButton(&exitButton,
        WINDOW_WIDTH/2-250, WINDOW_HEIGHT-100, 500, 300,
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
    deadAnimation(petTextures, pet.x, pet.y, 1);
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
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}


// Отображение меню(его статической состоявляющей)
static void dead_render() {
    // Фон
    rectdict.w = WINDOW_WIDTH;
    rectdict.h = WINDOW_HEIGHT;
    renderTexture(background, &rectdict);

    // Рисуем анимацию и т.д.
    deadAnimation(petTextures, pet.x, pet.y, i);
    i++;

    // Рисуем кнопку
    exitButton.rect.x = WINDOW_WIDTH/2-250;
    exitButton.rect.y = WINDOW_HEIGHT/2-150;

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
