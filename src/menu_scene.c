#include <SDL2/SDL.h>
#include "graphics.h"
#include "menu_scene.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   

// Простейшая кнопка
static Button startButton;


// ============ Локальные функции-обработчики ============
// Метод для удаления меню
static void menu_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&startButton);
}

// Реакция на нажатаю кнопку
void onStartButtonClick() {
    extern SDL_Renderer* gRenderer;
    // Логика при нажатии кнопки "Начать игру"
    // Например, переключение сцены
    menu_destroy();
    set_scene(&GAME_SCENE);
}

// Инициализация меню(его создание и отображение)
static void menu_init() {
    extern SDL_Renderer* gRenderer;
    // Инициализация кнопки (координаты, размеры)
    if (!initButton(&startButton,
        WINDOW_WIDTH/2-250, WINDOW_HEIGHT/2-150, 500, 300,
        "assets/button_start.png",
        "assets/button_start_watch.png", // используем default для hover
        "assets/button_start_click.png", // используем default для click
        onStartButtonClick))
    {
    SDL_Log("Ошибка инициализации кнопки!");
    }
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menu_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&startButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void menu_update(float delta) {
    // Здесь можно обновлять анимации меню, если есть
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void menu_render() {
    // Очищаем экран (обычно это делается в главном цикле, но можно и тут)
    // SDL_SetRenderDrawColor(renderer, ...);
    // SDL_RenderClear(renderer);
    // Рисуем кнопку
    startButton.rect.x = WINDOW_WIDTH/2-250;
    startButton.rect.y = WINDOW_HEIGHT/2-150;

    renderButton(&startButton);

    // SDL_RenderPresent(renderer); // Обычно в главном цикле
}



// Объект сцены
Scene MENU_SCENE = {
    .init = menu_init,
    .handle_events = menu_handle_events,
    .update = menu_update,
    .render = menu_render,
    .destroy = menu_destroy
};
