#include <SDL.h>
#include "saves_scene.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "pet.h"
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   

#define SAVES 3

// Кнопка старт
static Button savesButton[SAVES];

// Метод для удаления меню
static void saves_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    for(int i = 0; i < SAVES; i++){
        destroyButton(&savesButton[i]);
    }
}

// Реакция на нажатаю кнопку
static void onButtonClick() {
    for(int i = 0; i < SAVES; i++){
        if(savesButton[i].isClicked){
            pet.id = i;
            init_pet(i);
        }
    }
    // Логика при нажатии кнопки
    saves_destroy();
    set_scene(&GAME_SCENE);
}


// Инициализация меню(его создание и отображение)
static void saves_init() {
    char* s1 = "save_button";
    char* s3 = ".png";

    // Инициализация кнопки (координаты, размеры)
    for(int i = 0; i < SAVES; i++){   
        size_t len = snprintf(NULL, 0, "%s%d%s", s1, i+1, s3);
        char *result = malloc(len + 1); // +1 для нулевого терминатора 
        // Копируем и объединяем строки
        snprintf(result, len + 1, "%s%d%s", s1, i+1, s3);

        initButton(&savesButton[i],
            WINDOW_WIDTH/2-150/2, WINDOW_HEIGHT/2-250/2+i*50, 300, 160,
            result,
            NULL, // используем для hover
            NULL, // используем для click
            onButtonClick,
            NULL
        );
    }
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void saves_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    for(int i = 0; i < SAVES; i++){
        handleButtonEvent(&savesButton[i], e);
    }
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void saves_update(float delta) {
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void saves_render() {
    for(int i = 0; i < SAVES; i++){
        // Рисуем кнопку
        savesButton[i].rect.x = WINDOW_WIDTH/2-300/2;
        savesButton[i].rect.y = (WINDOW_HEIGHT/2)-(250)+(i*150);
        renderButton(&savesButton[i]);
    }
}



// Объект сцены
Scene SAVES_SCENE = {
    .init = saves_init,
    .handle_events = saves_handle_events,
    .update = saves_update,
    .render = saves_render,
    .destroy = saves_destroy
};
