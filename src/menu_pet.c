#include <SDL2/SDL.h>
#include "SDL_render.h"
#include "graphics.h"
#include "animation.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "menu_pet.h"
#include "ui.h"     
#include "pet.h"        

// Количество скинов
#define MAX_SKINS 5

// Структура скинов дя пэтов
typedef struct {
    char path[64];
    Animation* anim;
    float scaleW;
    float scaleH;
} SkinPet ;



// Массив путей до скинов
static SkinPet SKIN_PATHS[MAX_SKINS] = {
    {"assets/pets/pet.png", NULL, 0.2, 0.2},
    {"assets/pets/pet_white.png", NULL, 1, 1},
    {"assets/pets/pet_skin2.png", NULL, 0.2, 0.2},
    {"assets/pets/pet_skin3.png", NULL, 0.2, 0.2},
    {"assets/pets/pet_skin4.png", NULL, 0.2, 0.2}
};

// Выбранный скин
static int selectedSkinIndex = 0;
static SDL_Texture *previewTexture;

// Кнопка назад
static SDL_Texture *prevSkin; 
// Кнопка вперед
static SDL_Texture *postSkin; 

// Кнопки
static Button exittButton; // Кнопка выход
static Button prevButton; // Кнопка назад проматать ленту скинов
static Button nextButton; // Кнопка вперед проматать ленту скинов
static Button applyButton; // Кнопка выбрать скин

// Метод для удаления меню
static void menuPet_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&exittButton);
    destroyButton(&applyButton);
    destroyButton(&nextButton);
    destroyButton(&prevButton);

    SDL_DestroyTexture(prevSkin);
    SDL_DestroyTexture(postSkin);
    SDL_DestroyTexture(previewTexture);

    // Очищаем анимации
    for(int i = 0; i < MAX_SKINS; i++){
        if((SKIN_PATHS[selectedSkinIndex].anim != pet.stayAnim) && SKIN_PATHS[selectedSkinIndex].anim){
            destroyAnimation(SKIN_PATHS[selectedSkinIndex].anim);
        }
    }
}

// Перезагрузка изображений(скинов)
static void reloadPreviewTexture(void) {
    SDL_DestroyTexture(previewTexture);
    SDL_DestroyTexture(prevSkin);
    SDL_DestroyTexture(postSkin);

    previewTexture = loadTexture(SKIN_PATHS[selectedSkinIndex].path);

    if(selectedSkinIndex - 1 >= 0){
        prevSkin = loadTexture(SKIN_PATHS[selectedSkinIndex-1].path);
        applyGrayTransparency(prevSkin, 128);
    }
    if(selectedSkinIndex+1 < MAX_SKINS){
        postSkin = loadTexture(SKIN_PATHS[selectedSkinIndex+1].path);
        applyGrayTransparency(postSkin, 128);
    }
}

// Реакция на нажатаю кнопку выхода
static void onexittButtonClick() {
    extern SDL_Renderer* gRenderer;
    // Логика при нажатии кнопки
    menuPet_destroy();
    set_scene(&GAME_SCENE);
}
// Кнопка назад
static void onPrevClick() {
    selectedSkinIndex--;
    if (selectedSkinIndex < 0) selectedSkinIndex = MAX_SKINS - 1; // цикл по кругу
    reloadPreviewTexture();
}
// Кнопка вперед
static void onNextClick() {
    selectedSkinIndex++;
    if (selectedSkinIndex >= MAX_SKINS) selectedSkinIndex = 0;
    reloadPreviewTexture();
}
// Кнопка применить
static void onApplyClick() {
    // Применяем скин к питомцу
    invisible_pet();
    pet.texture = loadTexture(SKIN_PATHS[selectedSkinIndex].path);
    pet.pathImage = SKIN_PATHS[selectedSkinIndex].path;

    pet.scaleW = SKIN_PATHS[selectedSkinIndex].scaleW;
    pet.scaleH = SKIN_PATHS[selectedSkinIndex].scaleH;

    if(SKIN_PATHS[selectedSkinIndex].anim){ // Если есть анимация для питомца
        pet.stayAnim = SKIN_PATHS[selectedSkinIndex].anim;
    } else {
        pet.stayAnim = NULL;
    }

    onexittButtonClick();
    // Дополнительно: обновить сцену или вывести сообщение «Скин применён»
}

// Инициализация меню(его создание и отображение)
static void menuPet_init() {
    // Инициализация кнопки (координаты, размеры)
    initButton(&exittButton, 25, 25, 50, 50, "assets/button-return.png", NULL, NULL, onexittButtonClick, NULL);
    initButton(&prevButton, 50, 400, 50, 50, "assets/button_left.png", NULL, NULL , onPrevClick, NULL);
    initButton(&nextButton, 100, 400, 50, 50, "assets/button_right.png", NULL, NULL , onNextClick, NULL);
    initButton(&applyButton, 200, 400, 100, 50, "assets/button_accept.png", NULL, NULL , onApplyClick, NULL);

    // Инициализация анимаций
    SKIN_PATHS[1].anim = createAnimationOneType(
        loadTexture("assets/animations/pet_white_anim.png"), 
        300 * SKIN_PATHS[1].scaleW,
        300 * SKIN_PATHS[1].scaleH,
        9, 
        0.5
    );

    // Загрузите текущий скин в "previewTexture"
    selectedSkinIndex = 0;
    reloadPreviewTexture();
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
            renderTexture(postSkin, &dstRect);
        }

        if(selectedSkinIndex - 1 >= 0){
            dstRect.x = 10;
            renderTexture(prevSkin, &dstRect);
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
