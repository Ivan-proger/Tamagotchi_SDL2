#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "graphics.h"
#include "game_scene.h"
#include "scene_manager.h"
#include "pet.h"
#include "animation.h"
#include "ui.h"
#include "globals.h"
#include "menu_pet.h"


// Кнопки
Button caressButton;
Button feedButton;
Button customButton;

SDL_Texture* background;
SDL_Rect rectdict;

// Цвета шкал
SDL_Color bg = {100, 100, 100, 255};    // Темно-серый фон
SDL_Color fg = {0, 255, 0, 255};        // Зеленый для заполненной области
SDL_Color border = {0, 0, 0, 255};      // Черная обводка

// Координаты и размеры шкал
struct {
    int x;
    int y;
    int w;
    int h;
    int borderRadius;
} sparm = {30, 30, 40, 110, 10};

/**
 * @brief Функция для отрисовки шкалы с округлёнными углами
 * 
 * @param x             -- координаты левого угла шкалы
 * @param y             -- у координата
 * @param width         -- ширина шкалы
 * @param height        -- высота шкалы
 * @param value         -- значение заполнения (от 0 до 255)
 * @param bgColor       -- цвет фона (незаполненной части)
 * @param fgColor       -- цвет заполненной части
 * @param borderColor   -- цвет обводки
 * @param borderRadius  -- радиус округления углов
 */
void renderProgressBarRounded(int x, int y, 
                            int width, int height,
                            unsigned char value,
                            SDL_Color bgColor,
                            SDL_Color fgColor,
                            SDL_Color borderColor,
                            int borderRadius) {
                          
    // Вычисляем процент заполнения (0.0 - 1.0)
    float ratio = value / 255.0f;
    //int fillWidth = (int)(width * ratio);

    // Переменные для размеров заполненной области
    int fillWidth = width;
    int fillHeight = height;

    // Для вертикальной шкалы вычисляем высоту заполнения
    fillHeight = (int)(height * ratio);

    // Отрисовываем обводку шкалы с округлёнными углами
    filledRoundedRectangleRGBA(
        x-5, y-5, x + width + 5, y + height + 5,
        borderRadius,
        borderColor.r, borderColor.g, borderColor.b, borderColor.a
        );

    // Отрисовываем фон шкалы с округлёнными углами
    filledRoundedRectangleRGBA(
        x, y, x + width, y + height,
        borderRadius,
        bgColor.r, bgColor.g, bgColor.b, bgColor.a);

    // Отрисовываем заполненную область, если значение > 0
    if (fillHeight > 0) {
        // Подбираем подходящий радиус для заполненной области,
        // чтобы избежать слишком сильного округления, если fillWidth маленький
        int fillRadius = borderRadius;
        if (fillHeight < borderRadius * 2) {
            fillRadius = fillHeight / 2;
        }

        filledRoundedRectangleRGBA(
            x, (height - fillHeight) + y, width + x, height + y,
            fillRadius,
            fgColor.r-value, fgColor.g+value, fgColor.b, fgColor.a
        );
    }
}

// Кнопка гладить
void onCaressButton(void){
    add_cheer(15);
}
// Кнопка покормить
void onFeedButton(void){
    add_satiety(25);
}
// Кнопка кастомизация
void onCustomize(void){
    set_scene(&MENU_PET);
}

// Инициализация игровой сцены
static void game_init() {
    load_texture_pet();

    background = loadTexture("assets/Background.png");
    rectdict.x = 0;
    rectdict.y = 0;

    //* Кнопка погладить
    if (!initButton(&caressButton,
        0, 0, 100, 100,
        "assets/button_caress1.png",
        NULL, // используем default для hover
        NULL, // используем default для click
        onCaressButton))
    {
        SDL_Log("Ошибка инициализации кнопки: %s", SDL_GetError());
    }

    // Анимация кнопки
    initButtonAnimation(&caressButton, "assets/animations/button_caress_anim.png", 4, 0.2, 300, 300);

    //* Кнопка покормить
    if (!initButton(&feedButton,
        0, 0, 100, 100,
        "assets/button_feed1.png",
        "assets/button_feed2.png", // используем default для hover
        "assets/button_feed1.png", // используем default для click
        onFeedButton))
    {
        SDL_Log("Ошибка инициализации кнопки: %s", SDL_GetError());
    }
    // Анимация кнопки
    initButtonAnimation(&feedButton, "assets/animations/button_feed_anim.png", 12, 0.1, 135, 135);

    //* Кнопка кастомизация
    if (!initButton(&customButton,
        0, 0, 80, 80,
        "assets/customize_button.png",
        NULL, // используем default для hover
        NULL, // используем default для click
        onCustomize))
    {
        SDL_Log("Ошибка инициализации кнопки: %s", SDL_GetError());
    }
}

/**
 * @brief Обработка эвентов 
 * 
 * @param e -- Эвент sdl.
 */
static void game_handle_events(SDL_Event* e) {
    // Обработка событий в игре (клики, клавиатура и т.п.)
    handleButtonEvent(&caressButton, e);
    handleButtonEvent(&feedButton, e);
    handleButtonEvent(&customButton, e);

    if (e->type == SDL_KEYDOWN) {
        // Пример: ESC -> вернуть в меню
        if (e->key.keysym.sym == SDLK_ESCAPE) {
            extern Scene MENU_SCENE;
            set_scene(&MENU_SCENE);
        }
    }
}

/**
 * @brief Логика в реальном времени(физика)
 * 
 * @param delta -- тик времени
 */
static void game_update(float delta) {
    // delta – время, прошедшее с прошлого кадра
    updateButton(&caressButton, delta);
    updateButton(&feedButton, delta);
}

// Отображение статики
static void game_render() {
    rectdict.w = WINDOW_WIDTH;
    rectdict.h = WINDOW_HEIGHT;
    renderTexture(background, &rectdict);

    // Отрисовываем картинку
    show_pet();

    // Рисуем здоровье
    renderProgressBarRounded(sparm.x, sparm.y, 
                            sparm.w, sparm.h, 
                            pet.health, 
                            bg, fg, border, 
                            sparm.borderRadius);

    // Рисуем голод(сытность)
    renderProgressBarRounded(sparm.x*4, sparm.y, 
                            sparm.w, sparm.h, 
                            pet.satiety, bg, fg, border, 
                            sparm.borderRadius);

    // Рисуем настроение
    renderProgressBarRounded(sparm.x*7, sparm.y, 
                            sparm.w, sparm.h, 
                            pet.cheer, 
                            bg, fg, border, 
                            sparm.borderRadius);

    // Кнопка гладить
    caressButton.rect.x = WINDOW_WIDTH/2-50;
    caressButton.rect.y = WINDOW_HEIGHT-150;    
    renderButton(&caressButton);
    // Кнопка кормить
    feedButton.rect.x = caressButton.rect.x+140; 
    feedButton.rect.y = caressButton.rect.y-70;
    renderButton(&feedButton);
    // Кнопка кастомизвция
    customButton.rect.x = WINDOW_WIDTH-100;
    customButton.rect.y = 100;    
    renderButton(&customButton); 
}

// Удаление сцены
static void game_destroy(void) {
    invisible_pet();   
    destroyButton(&caressButton);
    destroyButton(&feedButton);
    destroyButton(&customButton);
}

// Объект сцены
Scene GAME_SCENE = {
    .init = game_init,
    .handle_events = game_handle_events,
    .update = game_update,
    .render = game_render,
    .destroy = game_destroy
};