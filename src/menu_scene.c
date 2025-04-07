#include <SDL2/SDL.h>
#include "graphics.h"
#include "menu_scene.h"
#include "title_scene.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   

bool POWEROFF = false;

// Кнопка старт
static Button startButton;
static Button powerOffButton;
static Button helpButton;

// Метод для удаления меню
static void menu_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&startButton);
    destroyButton(&powerOffButton);
    destroyButton(&helpButton);
}

// Реакция на нажатаю кнопку
static void onStartButtonClick() {
    // Логика при нажатии кнопки
    menu_destroy();
    set_scene(&GAME_SCENE);
}
// Реакция на нажатаю кнопку
static void onHelpButtonClick() {
    // Логика при нажатии кнопки
    menu_destroy();
    set_scene(&TITLE_SCENE);
}
// Реакция на нажатаю кнопку
static void onPowerOFFButtonClick() {
    // Логика при нажатии кнопки
    menu_destroy();
    POWEROFF = true;
    
}

// Инициализация меню(его создание и отображение)
static void menu_init() {
    // Инициализация кнопки (координаты, размеры)
    initButton(&startButton,
        WINDOW_WIDTH/2-250, WINDOW_HEIGHT/2-150, 500, 300,
        "assets/button_start.png",
        "assets/button_start_watch.png", // используем для hover
        "assets/button_start_click.png", // используем для click
        onStartButtonClick
    );
    initButton(&powerOffButton,
        0, 0, 100, 100,
        "assets/poweroff.png",
        NULL, // используем для hover
        NULL, // используем для click
        onPowerOFFButtonClick
    );
    initButton(&helpButton,
        0, 0, 100, 100,
        "assets/info.png",
        NULL, // используем для hover
        NULL, // используем для click
        onHelpButtonClick
    );
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menu_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&startButton, e);
    handleButtonEvent(&powerOffButton, e);
    handleButtonEvent(&helpButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void menu_update(float delta) {
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void menu_render() {
    // Рисуем кнопку
    startButton.rect.x = WINDOW_WIDTH/2-250;
    startButton.rect.y = WINDOW_HEIGHT/2-150;

    renderButton(&startButton);

    powerOffButton.rect.x = WINDOW_WIDTH-120;
    powerOffButton.rect.y = 50;

    renderButton(&powerOffButton);

    helpButton.rect.x = 50;
    helpButton.rect.y = 50;

    renderButton(&helpButton);
}



// Объект сцены
Scene MENU_SCENE = {
    .init = menu_init,
    .handle_events = menu_handle_events,
    .update = menu_update,
    .render = menu_render,
    .destroy = menu_destroy
};
