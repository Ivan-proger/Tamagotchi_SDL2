#include <SDL2/SDL.h>
#include "graphics.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "menu_pet.h"
#include "ui.h"     
#include "pet.h"        
#include "globals.h"   



#define MAX_SKINS 4
static char* SKIN_PATHS[MAX_SKINS] = {
    "assets/pet.png",
    "assets/pet_skin2.png",
    "assets/pet_skin3.png",
    "assets/pet_skin4.png",
};

// Выбранный скин
static int selectedSkinIndex = 0;
SDL_Texture *previewTexture;

// Кнопки
static Button exittButton;
static Button prevButton;
static Button nextButton;
static Button applyButton;

// Метод для удаления меню
static void menuPet_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&exittButton);
    destroyButton(&applyButton);
    destroyButton(&nextButton);
    destroyButton(&prevButton);
}

void reloadPreviewTexture(void) {
    if (previewTexture) {
        SDL_DestroyTexture(previewTexture);
        previewTexture = NULL;
    }
    previewTexture = loadTexture(SKIN_PATHS[selectedSkinIndex]);
}

// Реакция на нажатаю кнопку
void onexittButtonClick() {
    extern SDL_Renderer* gRenderer;
    // Логика при нажатии кнопки
    menuPet_destroy();
    set_scene(&GAME_SCENE);
}
void onPrevClick() {
    selectedSkinIndex--;
    if (selectedSkinIndex < 0) selectedSkinIndex = MAX_SKINS - 1; // цикл по кругу
    reloadPreviewTexture();
}
void onNextClick() {
    selectedSkinIndex++;
    if (selectedSkinIndex >= MAX_SKINS) selectedSkinIndex = 0;
    reloadPreviewTexture();
}
void onApplyClick() {
    // Применяем скин к питомцу
    extern Pet pet; // или получите доступ через getPet()
    if (pet.texture) SDL_DestroyTexture(pet.texture);
    pet.texture = loadTexture(SKIN_PATHS[selectedSkinIndex]);
    pet.pathImage = SKIN_PATHS[selectedSkinIndex];

    onexittButtonClick();
    // Дополнительно: обновить сцену или вывести сообщение «Скин применён»
}

// Инициализация меню(его создание и отображение)
static void menuPet_init() {
    // Инициализация кнопки (координаты, размеры)
    if (!initButton(&exittButton,
        25, 25, 50, 50,
        "assets/button-return.png",
        NULL, // используем default для hover
        NULL, // используем default для click
        onexittButtonClick))
    {
        SDL_Log("Ошибка инициализации кнопки!");
    }
    initButton(&prevButton, 50, 400, 50, 50, "assets/button_left.png", NULL, NULL , onPrevClick);
    initButton(&nextButton, 100, 400, 50, 50, "assets/button_right.png", NULL, NULL , onNextClick);
    initButton(&applyButton, 200, 400, 100, 50, "assets/button_accept.png", NULL, NULL , onApplyClick);

    // Загрузите текущий скин в "previewTexture"
    selectedSkinIndex = 0;
    previewTexture = loadTexture(SKIN_PATHS[selectedSkinIndex]);
}


// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menuPet_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&exittButton, e);
    handleButtonEvent(&applyButton, e);
    handleButtonEvent(&nextButton, e);
    handleButtonEvent(&prevButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void menuPet_update(float delta) {
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void menuPet_render() {
    // Рисуем кнопку
    exittButton.rect.x = 25;
    exittButton.rect.y = 25;

    renderButton(&exittButton);
    renderButton(&applyButton);
    renderButton(&nextButton);
    renderButton(&prevButton);

    if (previewTexture) {
        SDL_Rect dstRect = { 150, 100, 170, 170 }; // примерное место и размер
        renderTexture(previewTexture, &dstRect);

        if(selectedSkinIndex+1 < MAX_SKINS){
            dstRect.x += 170;
            SDL_Texture *temp = loadTexture(SKIN_PATHS[selectedSkinIndex+1]);
            applyGrayTransparency(temp, 128);
            renderTexture(temp, &dstRect);
        }

        if(selectedSkinIndex - 1 >= 0){
            dstRect.x = 10;
            SDL_Texture *temp = loadTexture(SKIN_PATHS[selectedSkinIndex-1]);
            applyGrayTransparency(temp, 128);
            renderTexture(temp, &dstRect);
        }
    }
}


// Объект сцены
Scene MENU_PET = {
    .init = menuPet_init,
    .handle_events = menuPet_handle_events,
    .update = menuPet_update,
    .render = menuPet_render,
    .destroy = menuPet_destroy
};
