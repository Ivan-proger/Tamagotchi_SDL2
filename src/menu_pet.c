#include <SDL2/SDL.h>
#include "graphics.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "menu_pet.h"
#include "ui.h"             
#include "globals.h"   

// Кнопка старт
static Button exittButton;


// Метод для удаления меню
static void menuPet_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&exittButton);
}

// Реакция на нажатаю кнопку
void onexittButtonClick() {
    extern SDL_Renderer* gRenderer;
    // Логика при нажатии кнопки
    menuPet_destroy();
    set_scene(&GAME_SCENE);
}

// Инициализация меню(его создание и отображение)
static void menuPet_init() {
    // Инициализация кнопки (координаты, размеры)
    if (!initButton(&exittButton,
        WINDOW_WIDTH/2-250, WINDOW_HEIGHT-250, 500, 300,
        "assets/button_start.png",
        "assets/button_start_watch.png", // используем default для hover
        "assets/button_start_click.png", // используем default для click
        onexittButtonClick))
    {
    SDL_Log("Ошибка инициализации кнопки!");
    }
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menuPet_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&exittButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void menuPet_update(float delta) {
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void menuPet_render() {
    // Рисуем кнопку
    exittButton.rect.x = WINDOW_WIDTH/2-250;
    exittButton.rect.y = WINDOW_HEIGHT/2-150;

    renderButton(&exittButton);
}


// Объект сцены
Scene MENU_PET = {
    .init = menuPet_init,
    .handle_events = menuPet_handle_events,
    .update = menuPet_update,
    .render = menuPet_render,
    .destroy = menuPet_destroy
};
