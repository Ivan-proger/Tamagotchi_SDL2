#include <SDL.h>
#include "SDL_render.h"
#include "globals.h"
#include "graphics.h"
#include "animation.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "menu_pet.h"
#include "ui.h"     
#include "pet.h"  
#include "text_input.h"      
#include "notify.h"

// Количество скинов
#define MAX_SKINS 4

// Структура скинов дя пэтов
typedef struct {
    char path[64];
    Animation* anim;
    float scaleW;
    float scaleH;
    char pathBone[64];
} SkinPet ;



// Массив путей до скинов
static SkinPet SKIN_PATHS[MAX_SKINS] = {
    {"pets/pet.png", NULL, 0.2, 0.2, 
        "pets/pet_bone.png"},
    {"pets/pet_white.png", NULL, 0.8, 0.8, 
        "pets/pet_white_bone.png"},
    {"pets/pet_gsd.png", NULL, 1, 1,
    "pets/pet_gsd_bone.png"},
    {"pets/pet_pug.png", NULL, 0.9, 0.9,
    "pets/pet_pug_bone.png"},

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
static Button applyNameButton; // Кнопка подтверждения смены имени

// Поле для смены имени питомца
static InputField nameField;

// Метод для удаления меню
static void menuPet_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&exittButton);
    destroyButton(&applyButton);
    destroyButton(&nextButton);
    destroyButton(&prevButton);
    destroyButton(&applyNameButton);

    SDL_DestroyTexture(prevSkin);
    prevSkin = NULL;
    SDL_DestroyTexture(postSkin);
    prevSkin = NULL;
    SDL_DestroyTexture(previewTexture);
    previewTexture = NULL;

    // Очищаем анимации
    for(int i = 0; i < MAX_SKINS; i++){
        if(SKIN_PATHS[i].anim){
            if(SKIN_PATHS[i].anim != SKIN_PATHS[selectedSkinIndex].anim){
                destroyAnimation(SKIN_PATHS[i].anim);
                SKIN_PATHS[i].anim = NULL;
            }
        }
    }

    // Выключаем поле
    InputField_Destroy(&nameField);
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
    // Логика при нажатии кнопки;
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
    pet.stayAnim = NULL;
    pet.texture = loadTexture(SKIN_PATHS[selectedSkinIndex].path);
    pet.pathImage = SKIN_PATHS[selectedSkinIndex].path;

    pet.scaleW = SKIN_PATHS[selectedSkinIndex].scaleW;
    pet.scaleH = SKIN_PATHS[selectedSkinIndex].scaleH;

    if(SKIN_PATHS[selectedSkinIndex].anim){ // Если есть анимация для питомца
        pet.stayAnim = SKIN_PATHS[selectedSkinIndex].anim;
    }

    pet.pathImageWithBone = SKIN_PATHS[selectedSkinIndex].pathBone;
    pet.textureWithBone = loadTexture(SKIN_PATHS[selectedSkinIndex].pathBone);

    onexittButtonClick();
    // Дополнительно: обновить сцену или вывести сообщение «Скин применён»
}

// Кнопка применить новое имя
static void onApplyClickSetName() {
    char* new_name = InputField_GetText(&nameField);
    pet_set_name(new_name);
    // Смена названия окна
    SDL_SetWindowTitle(gWindow, new_name);
    notify_user(pet.name, "Имя изменено!");
}
// Инициализация меню(его создание и отображение)
static void menuPet_init() {
    // Инициализация кнопки (координаты, размеры)
    initButton(&exittButton, 25, 25, 50, 50, "button-return.png", NULL, NULL, onexittButtonClick, NULL);
    initButton(&prevButton, 50, 400, 50, 50, "button_left.png", NULL, NULL , onPrevClick, NULL);
    initButton(&nextButton, 100, 400, 50, 50, "button_right.png", NULL, NULL , onNextClick, NULL);
    initButton(&applyButton, 200, 400, 100, 50, "button_accept.png", NULL, NULL , onApplyClick, NULL);
    // Инициализация анимаций
    SKIN_PATHS[1].anim = createAnimationOneType(
        "animations/pet_white_anim.png", 
        300,
        300,
        9, 
        0.5
    );
    SKIN_PATHS[2].anim = createAnimationOneType(
        "animations/pet_gsd_anim.png", 
        300,
        300,
        8, 
        0.6
    );
    // Загрузите текущий скин в "previewTexture"
    selectedSkinIndex = 0;
    reloadPreviewTexture();

    // Шрифт для смены имени питомца
    TTF_Font* font = TTF_OpenFont(getAssetPath("fonts/BenbowSemibold.ttf"), 24);
    if (!font) {
        SDL_Log("Ошибка загрузки шрифта: %s", TTF_GetError());
        // Обработка ошибки
    }

    SDL_Rect rectNameField = {100, 25, 300, 55};
    SDL_Color colornameField = {0,0,0,255};
    InputField_Init(&nameField, font, colornameField, rectNameField);
    InputField_SetText(&nameField, pet.name);
    // Кнопка подтверждения смены имени
    initButton(&applyNameButton, 110+300, 25, 100/3, 50/2, "button_accept.png", NULL, NULL , onApplyClickSetName, NULL);
    
}


// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menuPet_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&exittButton, e);
    handleButtonEvent(&applyButton, e);
    handleButtonEvent(&nextButton, e);
    handleButtonEvent(&prevButton, e);
    handleButtonEvent(&applyNameButton, e);

    // Поле для ввода
    InputField_HandleEvent(&nameField, e);
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
    renderButton(&applyNameButton);

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

    // Поле для ввода
    InputField_Render(&nameField);
}


// Объект сцены
Scene MENU_PET = {
    .init = menuPet_init,
    .handle_events = menuPet_handle_events,
    .update = menuPet_update,
    .render = menuPet_render,
    .destroy = menuPet_destroy
};
