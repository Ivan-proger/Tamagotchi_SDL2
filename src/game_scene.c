#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL2_gfxPrimitives.h>
#include "SDL_render.h"
#include "animation.h"
#include "graphics.h"
#include "game_scene.h"
#include "scene_manager.h"
#include "pet.h"
#include "ui.h"
#include "globals.h"
#include "menu_pet.h"
#include "file_manager.h"

// Количество эффектов в анимации (спрайтов)
#define counteffects 7
// Длительность удовольствия того как гладят питомца
#define durationcheer 3.0

// Кнопки
static Button caressButton;
static Button feedButton;
static Button customButton;

// Фон
static SDL_Texture* background;
static SDL_Rect rectdict;

// Цвета шкал
static SDL_Color bg = {100, 100, 100, 255};    // Темно-серый фон
static SDL_Color fg = {0, 255, 0, 255};        // Зеленый для заполненной области
static SDL_Color border = {0, 0, 0, 255};      // Черная обводка

// Сердце
static SDL_Texture* heart;
static SDL_Rect heartrect;

// Эффект при кормлении
typedef struct {
    SDL_Texture *texture;        // Указатель на текстуру изображения
    float x, y;           // Текущие координаты
    float startX, startY; // Начальная точка движения
    float targetX, targetY; // Конечная точка движения
    float angle, startAngle, targetAngle; // Текущий, начальный и конечный углы (в градусах)
    float duration;       // Общее время анимации (в секундах)
    float elapsed;        // Уже прошедшее время (в секундах)
} MovingSprite; MovingSprite movingBone;

// Эффект удовольствия
static SDL_Texture* hearteffect;
// Диапазон для эффекта
static SDL_Rect areaheart = {.w=40, .h=40 }; 
// Массив количества объектов
static int listeffects[counteffects][2];
// Накопительное время для проигрывание эффекта
static float duration = 255.0;

// Насыщенность
static SDL_Texture* satiety;
static SDL_Rect satietyrect;
// Настроение
static SDL_Texture* tcheer;
static SDL_Rect cheerrect;

// Координаты и размеры шкал
static struct {
    int x;
    int y;
    int w;
    int h;
    int borderRadius;
} sparm = {30, 30, 40, 110, 10};

// Музыка на фоне
static Mix_Music *backgroundMusic = NULL;


// Кнопка гладить
static void onCaressButton(void){
    duration = 0.0;
    // Заполняем спрайтами со случайными позициями
    for(int k=0; k < counteffects; k++){
        listeffects[k][0] = (int)(rand() % pet.w * pet.scaleW) + pet.x;
        listeffects[k][1] = (int)(rand() % pet.y * pet.scaleH) + pet.y;
    }
    add_cheer(15);
}
// Кнопка покормить
static void onFeedButton(void){
    movingBone.elapsed = 0.0;
    movingBone.startX = feedButton.rect.x-20; 
    movingBone.startY = feedButton.rect.y-20;
    add_satiety(25);
}
// Кнопка кастомизация
static void onCustomize(void){
    set_scene(&MENU_PET);
}

// Инициализация игровой сцены
static void game_init() {
    load_texture_pet();

    background = loadTexture("Background.png");
    rectdict.x = 0;
    rectdict.y = 0;

    //* Кнопка погладить
    initButton(&caressButton,
        0, 0, 100, 100,
        "button_caress1.png",
        NULL, // используем default для hover
        NULL, // используем default для click
        onCaressButton,
        Mix_LoadWAV(getAssetPath("sounds/petting_dog.wav"))
    );
    // Анимация кнопки
    initButtonAnimation(&caressButton, "animations/button_caress_anim.png", 4, 0.2, 300, 300);

    //* Кнопка покормить
    initButton(&feedButton,
        0, 0, 100, 100,
        "button_feed1.png",
        "button_feed2.png", // используем default для hover
        "button_feed1.png", // используем default для click
        onFeedButton,
        Mix_LoadWAV(getAssetPath("sounds/eat.wav"))
    );
    // Анимация кнопки
    initButtonAnimation(&feedButton, "animations/button_feed_anim.png", 12, 0.1, 135, 135);

    //* Кнопка кастомизация
    initButton(&customButton,
        0, 0, 80, 80,
        "customize_button.png",
        NULL, // используем default для hover
        NULL, // используем default для click
        onCustomize,
        NULL
    ); 

    int tempW, tempH;
    heart = loadTexture("heart.png");
    
    sizeTexture(heart, &tempW, &tempH);
    heartrect.w = tempW*0.1*sizerW;
    heartrect.h = tempH*0.1*sizerW;

    satiety = loadTexture("satiety.png");
    sizeTexture(heart, &tempW, &tempH);
    satietyrect.w = tempW*0.1*sizerW;
    satietyrect.h = tempH*0.1*sizerW;

    tcheer = loadTexture("cheer.png");
    sizeTexture(tcheer, &tempW, &tempH);
    cheerrect.w = tempW*0.1*sizerW;
    cheerrect.h = tempH*0.1*sizerW;

    if(IS_SOUND){
        // Музыка
        backgroundMusic = Mix_LoadMUS(getAssetPath("sounds/main_music.mp3"));
        if (!backgroundMusic) {
            SDL_Log("Failed to load music: %s\n", Mix_GetError());
        }
        // Проиграть музыку бесконечно (-1)
        Mix_PlayMusic(backgroundMusic, -1);
    }

    // Текстура для эффекта сердечек
    hearteffect = loadTexture("heart.png");

    // Инициализация кости
    movingBone.texture = loadTexture("bone.png");
    movingBone.startAngle = 0; movingBone.targetAngle = 360;
    movingBone.duration = 1.0;  // 2 секунды полёт+вращение
    movingBone.elapsed = 3.0;
    movingBone.startX = feedButton.rect.x-20; 
    movingBone.startY = feedButton.rect.y-20;
    movingBone.targetX = pet.x + (int)(pet.w*pet.scaleW/2); 
    movingBone.targetY = pet.y + (int)(pet.h*pet.scaleH/2);

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
        if (e->key.keysym.sym == SDLK_ESCAPE ||
            e->key.keysym.scancode == SDL_SCANCODE_AC_BACK
        ) {
            extern Scene MENU_SCENE;
            save_game(pet.id);
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

    // Эффект гладить (сердечки)
    if(duration < durationcheer){
        duration += delta;
    }

    if(pet.stayAnim) {
        // Обновления анимации питомца
        updateAnimation(pet.stayAnim, delta);
    }

    if(movingBone.elapsed < movingBone.duration){
        // Просчитываем анимацию кости
        movingBone.elapsed += delta;
        if (movingBone.elapsed > movingBone.duration) {
            movingBone.elapsed = movingBone.duration;
        }
        // Нормализованный прогресс от 0.0 до 1.0
        float t = movingBone.duration > 0.0f ? (movingBone.elapsed / movingBone.duration) : 1.0f;
        // Линейная интерполяция позиции
        movingBone.x = movingBone.startX + (movingBone.targetX - movingBone.startX) * t;  // LERP по X 
        movingBone.y = movingBone.startY + (movingBone.targetY - movingBone.startY) * t;  // LERP по Y
        // Линейная интерполяция угла
        movingBone.angle = movingBone.startAngle + (movingBone.targetAngle - movingBone.startAngle) * t;  // LERP по углу
        // Добавляем постепенно прозрачность
        SDL_SetTextureAlphaMod(movingBone.texture, (1 - t) * 255.0);
    }
}

// Отображение картинки
static void game_render() {
    rectdict.w = WINDOW_WIDTH;
    rectdict.h = WINDOW_HEIGHT;
    renderTexture(background, &rectdict);

    sparm.x = 30*sizerW;
    sparm.y = 30*sizerH;
    sparm.w = 40*sizerW;
    sparm.h = 110*sizerH;

    // Рисуем здоровье
    renderProgressBarRounded(sparm.x, sparm.y, 
                            sparm.w, sparm.h, 
                            pet.health, 
                            bg, fg, border, 
                            sparm.borderRadius);
    heartrect.x = sparm.x-5;
    heartrect.y = sparm.y*5+5;
    renderTexture(heart, &heartrect); // Картинка сердечка под шкалой

    // Рисуем голод(сытность)
    renderProgressBarRounded(sparm.x*4, sparm.y, 
                            sparm.w, sparm.h, 
                            pet.satiety, bg, fg, border, 
                            sparm.borderRadius);
    satietyrect.x = sparm.x*4-5;
    satietyrect.y = sparm.y*5+5;
    renderTexture(satiety, &satietyrect); // Картинка мясо под шкалой

    // Рисуем настроение
    renderProgressBarRounded(sparm.x*7, sparm.y, 
                            sparm.w, sparm.h, 
                            pet.cheer, 
                            bg, fg, border, 
                            sparm.borderRadius);
    cheerrect.x = sparm.x*7-5;
    cheerrect.y = sparm.y*5+5;
    renderTexture(tcheer, &cheerrect); // Картинка мясо под шкалой

    // Кнопка гладить
    caressButton.rect.w = 100*MIN(sizerH, sizerW);
    caressButton.rect.h = 100*MIN(sizerH, sizerW);
    caressButton.rect.x = WINDOW_WIDTH/2 - caressButton.rect.w/2;
    caressButton.rect.y = WINDOW_HEIGHT-1.0*caressButton.rect.h*2+30*sizerH;    
    renderButton(&caressButton);
    // Кнопка кормить
    feedButton.rect.w = 100*MIN(sizerH, sizerW);
    feedButton.rect.h = 100*MIN(sizerH, sizerW);
    feedButton.rect.x = caressButton.rect.x+150*MIN(sizerH, sizerW); 
    feedButton.rect.y = caressButton.rect.y-70*MIN(sizerH, sizerW);
    renderButton(&feedButton);
    // Кнопка кастомизвция
    customButton.rect.w = 80*MIN(sizerH, sizerW);
    customButton.rect.h = 80*MIN(sizerH, sizerW);
    customButton.rect.x = WINDOW_WIDTH-100*sizerW;
    customButton.rect.y = 100*sizerH;    
    renderButton(&customButton); 


    // Эффект гладить (сердечки)
    areaheart.w=40*sizerH;
    areaheart.h=40*sizerH;
    if(duration < durationcheer){
        fade_anim_with_timer(
            hearteffect, 
            areaheart, 
            durationcheer, 
            duration, 
            counteffects, 
            listeffects
        );
        //duration += delta;
    }

    // Эффект летящей кости к собаке при кормлении 
    if(movingBone.elapsed < movingBone.duration){
        // Рисуем картинку питомца
        show_pet(true);

        // Обновляем точки полета
        movingBone.startX = feedButton.rect.x-20; 
        movingBone.startY = feedButton.rect.y-20;

        movingBone.targetX = pet.x + (int)(pet.w*pet.scaleW/2); 
        movingBone.targetY = pet.y + (int)(pet.h*pet.scaleH/2);

        // Определяем целевой прямоугольник (тут без изменения размера)
        SDL_Rect dst = {
            .x = (int)(movingBone.x),
            .y = (int)(movingBone.y),
            .w = 0,  // оставляем 0, SDL_QueryTexture подставит ширину/высоту
            .h = 0
        };
        // Подготовка размеров dst
        SDL_QueryTexture(movingBone.texture, NULL, NULL, &dst.w, &dst.h);
        dst.w *= 0.5*sizerH;  // Немного уменьшаем
        dst.h *= 0.5*sizerH;
        // Центр вращения — середина текстуры
        SDL_Point center = { dst.w / 2, dst.h / 2 };  // по умолчанию центр
        // Отрисовка с поворотом
        SDL_RenderCopyEx(gRenderer,
                        movingBone.texture,   // источник
                        NULL,          // всю текстуру
                        &dst,          // куда отрисовать
                        movingBone.angle,     // угол вращения (по часовой) 
                        &center,       // точка вращения
                        SDL_FLIP_NONE  // без отражения 
        );
    } else {show_pet(false);}
}

// Удаление сцены
static void game_destroy(void) {
    invisible_pet();   // Убрать отображения питомцйа

    destroyButton(&caressButton); // Убрать кнопки
    destroyButton(&feedButton);
    destroyButton(&customButton);

    SDL_DestroyTexture(heart); // Убрать текстуры под шкалами (изображения)
    SDL_DestroyTexture(satiety); 
    SDL_DestroyTexture(tcheer); 
    SDL_DestroyTexture(background);// Убрать текстуру фона

    SDL_DestroyTexture(hearteffect); // Удалить текстуру эффекта

    if(IS_SOUND)
        Mix_FreeMusic(backgroundMusic); // Перестать воспроизводить музыку

    // Текстура кости
    SDL_DestroyTexture(movingBone.texture);
}


Scene GAME_SCENE = {
    .init = game_init,
    .handle_events = game_handle_events,
    .update = game_update,
    .render = game_render,
    .destroy = game_destroy
};