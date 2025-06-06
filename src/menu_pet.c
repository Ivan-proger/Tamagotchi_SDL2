#include <SDL.h>
#include "SDL_render.h"
#include "file_manager.h"
#include "graphics.h"
#include "animation.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "menu_pet.h"
#include "ui.h"     
#include "pet.h"  
#include "text_input.h"      
#include "notify.h"
#include "globals.h"

// Количество скинов
#define MAX_SKINS 4

// Структура скинов для питомцев
typedef struct {
    char path[64]; // Путь до изображения
    Animation* anim; // Указатель на анимацию
    float scaleW; // Скалирование по ширине
    float scaleH; // Скалирование по высоте
    char pathBone[64]; // Путь до изображения кормления
} SkinPet;

// Массив путей до скинов
static SkinPet SKIN_PATHS[MAX_SKINS] = {
    {"pets/pet.png", NULL, 
        0.2, 0.2, 
        "pets/pet_bone.png"},
    {"pets/pet_white.png", NULL, 
        0.75, 0.75, 
        "pets/pet_white_bone.png"},
    {"pets/pet_gsd.png", NULL, 
        0.82, 0.82,
    "pets/pet_gsd_bone.png"},
    {"pets/pet_pug.png", NULL, 
        0.9, 0.9,
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
    postSkin = NULL;
    SDL_DestroyTexture(previewTexture);
    previewTexture = NULL;

    // Очищаем анимации
    for(int i = 0; i < MAX_SKINS; i++){
        if(SKIN_PATHS[i].anim){
            if(strcmp(SKIN_PATHS[i].path ,SKIN_PATHS[selectedSkinIndex].path) != 0){
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
    setTexturePet(
        SKIN_PATHS[selectedSkinIndex].path,
        SKIN_PATHS[selectedSkinIndex].scaleW,
        SKIN_PATHS[selectedSkinIndex].scaleH,
        SKIN_PATHS[selectedSkinIndex].pathBone,
        SKIN_PATHS[selectedSkinIndex].anim
    );

    char* new_name = InputField_GetText(&nameField);
    if(strcmp(getNamePet(), new_name) != 0) {
        setNamePet(new_name);
        notify_user(getNamePet(), "Имя изменено!");
    }

    
    onexittButtonClick();
}

// Кнопка применить новое имя
static void onApplyClickSetName() {
    char* new_name = InputField_GetText(&nameField);
    setNamePet(new_name);
    // Смена названия окна
    SDL_SetWindowTitle(gWindow, new_name);
}
// Инициализация меню(его создание и отображение)
static void menuPet_init() {
    // Инициализация кнопки (координаты, размеры)
    initButton(&exittButton, 0, 0, 0, 0, 
        "button-return.png", 
        "button-return_click.png", 
        "button-return_click.png", 
        onexittButtonClick, NULL);

    initButton(&prevButton, 50, 400, 50, 50, 
        "button_left.png", 
        "button_left.png", 
        "button_left.png" , 
        onPrevClick, NULL);

    initButton(&nextButton, 100, 400, 50, 50, 
        "button_right.png", 
        "button_right.png", 
        "button_right.png" , 
        onNextClick, NULL);

    initButton(&applyButton, 200, 400, 100, 50, 
        "button_accept.png", 
        "button_accept_click.png", 
        "button_accept_click.png" , 
        onApplyClick, NULL);

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
    TTF_Font* font = TTF_OpenFont(getAssetPath("fonts/BenbowSemibold.ttf"), 24*sizerH);
    if (!font) {
        SDL_Log("Ошибка загрузки шрифта: %s", TTF_GetError());
        // Обработка ошибки
    }

    SDL_Rect rectNameField = {0, 0, 0, 0};
    SDL_Color colornameField = {0,0,0,255};
    InputField_Init(&nameField, font, colornameField, rectNameField);
    InputField_SetText(&nameField, getNamePet());
    // Кнопка подтверждения смены имени
    initButton(
        &applyNameButton, 
        110+300, 25, 100/3, 50/2, 
        "button_accept_name.png", 
        "button_accept_name_click.png", 
        "button_accept_name_click.png" , 
        onApplyClickSetName, 
        NULL)
        ;
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
    exittButton.rect.w = 110*sizerH*0.5;
    exittButton.rect.h = 110*sizerH*0.5, 
    exittButton.rect.x = 25*sizerW; 
    exittButton.rect.y = 30*sizerH; 
    renderButton(&exittButton);

    applyButton.rect.w = 100*sizerH;
    applyButton.rect.h = 100*sizerH;
    applyButton.rect.x = WINDOW_WIDTH/2 - applyButton.rect.w/2;
    applyButton.rect.y = WINDOW_HEIGHT - applyButton.rect.h - 100*sizerH;
    renderButton(&applyButton);

    nextButton.rect.w = 50*sizerH;
    nextButton.rect.h = 50*sizerH;
    nextButton.rect.x = applyButton.rect.x + applyButton.rect.w + 20*sizerW;
    nextButton.rect.y = applyButton.rect.y + applyButton.rect.h/4;
    renderButton(&nextButton);

    prevButton.rect.w = 50*sizerH;
    prevButton.rect.h = 50*sizerH;
    prevButton.rect.x = applyButton.rect.x - prevButton.rect.w - 20*sizerW;
    prevButton.rect.y = applyButton.rect.y + applyButton.rect.h/4;
    renderButton(&prevButton);


    nameField.rect.w = WINDOW_WIDTH - exittButton.rect.x*2 - exittButton.rect.w/2 - applyNameButton.rect.w*2;
    nameField.rect.h = 55*sizerH;
    nameField.rect.x = 25*sizerW + exittButton.rect.x + exittButton.rect.w;
    nameField.rect.y = 25*sizerH;
    // Поле для ввода
    InputField_Render(&nameField);

    applyNameButton.rect.w = (1.0*60)*sizerH;
    applyNameButton.rect.h = nameField.rect.h;
    applyNameButton.rect.x = WINDOW_WIDTH - (WINDOW_WIDTH - nameField.rect.w - nameField.rect.x)/2 - applyNameButton.rect.w/2;
    applyNameButton.rect.y = nameField.rect.y;
    renderButton(&applyNameButton);   
    
    if (previewTexture) {
        SDL_Rect dstRect = { 
            150*sizerW, 
            nameField.rect.y+nameField.rect.h + 100 * sizerH, 
            170*MIN(sizerH, sizerW), 
            170*MIN(sizerH, sizerW) 
        }; // место и размер

        renderTexture(previewTexture, &dstRect);

        if(selectedSkinIndex+1 < MAX_SKINS){
            dstRect.x += dstRect.w + 10*sizerW;
            renderTexture(postSkin, &dstRect);
        }

        if(selectedSkinIndex - 1 >= 0){
            dstRect.x = 10*sizerW;
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
