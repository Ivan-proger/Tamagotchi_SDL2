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

static Button caressButton;

// Анимации 
SDL_Rect caressButtonFrames[4];
Animation* caressButtonAnim;
SDL_Texture* caressButtonSheet;

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
    add_cheer(5);
}

// Инициализация игровой сцены
static void game_init() {
    load_texture_pet();

    background = loadTexture("assets/Background.png");
    rectdict.x = 0;
    rectdict.y = 0;

    if (!initButton(&caressButton,
        WINDOW_WIDTH/2-50, WINDOW_HEIGHT-150, 100, 100,
        "assets/button_caress1.png",
        NULL, // используем default для hover
        NULL, // используем default для click
        onCaressButton))
        save_game;
    {
    SDL_Log("Ошибка инициализации кнопки!");
    }

    // Анимация кнопки
    caressButtonSheet = loadTexture("assets/animations/button_caress_anim.png");
    if (!caressButtonSheet) {
        SDL_Log("Ошибка загрузки спрайт-листа питомца: %s", SDL_GetError());
    }

    for (int i = 0; i < 4; i++) {
        caressButtonFrames[i].x = i * 300;
        caressButtonFrames[i].y = 0;
        caressButtonFrames[i].w = 300;
        caressButtonFrames[i].h = 300;
    }
    
    // Создаем анимацию, где каждый кадр показывается 0.5 секунды
    caressButtonAnim = createAnimation(caressButtonSheet, caressButtonFrames, 4, 0.5f);

    caressButton.clickAnim = caressButtonAnim;


    if(caressButtonAnim) printf("YEEES!!! \n");
}

/**
 * @brief Обработка эвентов 
 * 
 * @param e -- Эвент sdl.
 */
static void game_handle_events(SDL_Event* e) {
    // Обработка событий в игре (клики, клавиатура и т.п.)
    handleButtonEvent(&caressButton, e);

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
    updateAnimation(caressButtonAnim, delta);
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

    // Отрисовка анимации питомца по координатам (100, 150) с масштабом 1.0 (без изменения)
    //renderAnimation(caressButtonAnim, WINDOW_WIDTH/2-50, WINDOW_HEIGHT-150, 0.35f, 0.35f);
}

// Удаление сцены
static void game_destroy(void) {
    invisible_pet();
    destroyButton(&caressButton);
    destroyAnimation(caressButtonAnim);
}

// Объект сцены
Scene GAME_SCENE = {
    .init = game_init,
    .handle_events = game_handle_events,
    .update = game_update,
    .render = game_render,
    .destroy = game_destroy
};