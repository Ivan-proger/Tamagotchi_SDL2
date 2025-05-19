#include <SDL.h>
#include "menu_scene.h"
#include "title_scene.h"
#include "saves_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   
#include "pet.h"
#include "graphics.h"


// Кнопка старт
static Button startButton;

static Button powerOffButton;
static Button helpButton;

// Кнопка вкл/выкл звук
static Button soundButton;

// Метод для удаления меню
static void menu_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&startButton);
    destroyButton(&powerOffButton);
    destroyButton(&helpButton);
    destroyButton(&soundButton);
}

// Реакция на нажатаю кнопку
static void onStartButtonClick() {
    // Логика при нажатии кнопки
    menu_destroy();
    set_scene(&SAVES_SCENE);
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
     // Уничтожаем сцену
    save_game(); //! Сохранение
    scene_destroy();
    Mix_CloseAudio(); // Закрываем музыку
    Mix_FreeChunk(clickSound); // Закрываем базовый звук для нажатия на кнопку
    Mix_Quit();
    cleanupGraphics();
    exit(EXIT_SUCCESS);
    
}

void onSoundFalseClick();
void onSoundTrueClick();

// Реакция на включения звука
void onSoundFalseClick(){
    destroyButton(&soundButton);
    IS_SOUND = true;
    VALUE_SOUND = MIX_MAX_VOLUME;
    initButton(&soundButton,
        0, 0, 
        0, 0, // Обновляем в рендере
        "sound_true.png",
        NULL, // используем для hover
        NULL, // используем для click
        onSoundTrueClick,
        NULL
    );   
}
// Реакция на выключения звука
void onSoundTrueClick(){
    destroyButton(&soundButton);
    IS_SOUND = false;
    VALUE_SOUND = 0;
    initButton(&soundButton,
        0, 0, 
        0, 0, // Обновляем в рендере
        "sound_false.png",
        NULL, // используем для hover
        NULL, // используем для click
        onSoundFalseClick,
        NULL
    );   
}

// Инициализация меню(его создание и отображение)
static void menu_init() {
    // Инициализация кнопки (координаты, размеры)
    initButton(&startButton,
        0, 0, 1558, 834,
        "button_start.png",
        "button_start_hover.png", // используем для hover
        "button_start_click.png", // используем для click
        onStartButtonClick,
        NULL
    );
    initButton(&powerOffButton,
        0, 0, 
        0, 0,
        "poweroff.png",
        NULL, // используем для hover
        NULL, // используем для click
        onPowerOFFButtonClick,
        NULL
    );
    initButton(&helpButton,
        0, 0, 100*sizerH, 100*sizerH,
        "button-return.png",
        "button-return_click.png", // используем для hover
        "button-return_click.png", // используем для click
        onHelpButtonClick,
        NULL
    );

    if(IS_SOUND){
        initButton(&soundButton,
            0, 0, 256/2, 256/2,
            "sound_true.png",
            NULL, // используем для hover
            NULL, // используем для click
            onSoundTrueClick,
            NULL
        );        
    } else{
        initButton(&soundButton,
            0, 0, 256/2, 256/2,
            "sound_false.png",
            NULL, // используем для hover
            NULL, // используем для click
            onSoundFalseClick,
            NULL
        );         
    }
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menu_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&startButton, e);
    handleButtonEvent(&powerOffButton, e);
    handleButtonEvent(&helpButton, e);
    handleButtonEvent(&soundButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void menu_update(float delta) {
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void menu_render() {
    // Рисуем кнопку
    startButton.rect.w = 1558*MIN(sizerH, sizerW)*0.2;
    startButton.rect.h = 834*MIN(sizerH, sizerW)*0.2;
    startButton.rect.x = WINDOW_WIDTH/2-startButton.rect.w/2;
    startButton.rect.y = WINDOW_HEIGHT/2-startButton.rect.h/2;
    renderButton(&startButton);

    powerOffButton.rect.x = WINDOW_WIDTH-powerOffButton.rect.w-50*sizerW;
    powerOffButton.rect.y = 30*sizerH/2;
    powerOffButton.rect.w = 75*sizerH*0.9;
    powerOffButton.rect.h = 75*sizerH*0.9;
    renderButton(&powerOffButton);

    helpButton.rect.x = 50*sizerW; 
    helpButton.rect.y = 30*sizerH;
    helpButton.rect.w = 110*sizerH*0.5;
    helpButton.rect.h = 110*sizerH*0.5;
    renderButton(&helpButton);

    soundButton.rect.x = WINDOW_WIDTH-0.9*soundButton.rect.w-20*sizerH;
    soundButton.rect.y = WINDOW_HEIGHT-0.9*soundButton.rect.w-10*sizerH;
    soundButton.rect.w = (0.8*256/2)*sizerH*0.9;
    soundButton.rect.h = (0.8*256/2)*sizerH*0.9;
    renderButton(&soundButton);
}



// Объект сцены
Scene MENU_SCENE = {
    .init = menu_init,
    .handle_events = menu_handle_events,
    .update = menu_update,
    .render = menu_render,
    .destroy = menu_destroy
};
