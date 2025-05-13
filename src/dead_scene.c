#include <SDL.h>
#include <SDL2_gfxPrimitives.h> // Для aaLineRGBA (антиалиасинговой линии)
#include <stdlib.h>
#include "graphics.h"
#include "pet.h"
#include "game_scene.h"
#include "dead_scene.h"
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   
#include "file_manager.h"


// Константы
#define NUM_DROPLETS 50
#define MAX_SPLASHES 100

// Кусочки тамагочи
static SDL_Texture** petTextures;
static SDL_Rect dstRects[4];
static int dif_pieces;

// Кнопка старт
static Button exitButton;

// Фон
static SDL_Texture* background;
static SDL_Rect rectdict;

// Музыка на фоне
static Mix_Music *backgroundMusic = NULL;

// Спрайты
static SDL_Texture* text_dead;
static SDL_Rect text_dead_rect;

typedef struct {
    float x, y;       // Позиция капли
    float speed;      // Скорость падения (пикселей/сек)
    float length;     // Длина капли (в пикселях)
    Uint8 alpha;      // Прозрачность капли
} Droplet;

static Droplet droplets[NUM_DROPLETS];

typedef struct {
    float x, y;          // Позиция брызги
    float radius;        // Текущий радиус брызги
    float maxRadius;     // Максимальный радиус, до которого должна расшириться брызга
    float lifetime;      // Общее время жизни (в секундах)
    float elapsed;       // Прошедшее время с момента появления (в секундах)
    SDL_Color color;     // Цвет брызги (с альфа-каналом)
    bool active;         // Флаг, указывающий, активна ли данная брызга
} RedSplash;

// Массив частиц
static RedSplash splashes[MAX_SPLASHES];


// Предполагается, что petTextures — массив из 4 указателей на SDL_Texture*, полученный из splitTextureFour,
static void deadAnimation(SDL_Rect dstRects[4], SDL_Texture **petTextures, int startX, int startY, float scaleW, float scaleH, int gap) {
    int texW, texH;

    // Получаем размеры одной текстуры (предполагается, что все имеют одинаковый размер)
    sizeTexture(petTextures[0], &texW, &texH);
    texW *= scaleW;
    texH *= scaleH;
    // Определяем прямоугольники для каждой текстуры с промежутком (gap)
    
    
    // Верхний левый элемент
    dstRects[0].x = startX - gap;
    dstRects[0].y = startY-gap;
    dstRects[0].w = texW;
    dstRects[0].h = texH;
    
    // Верхний правый элемент
    dstRects[1].x = startX + texW + gap;
    dstRects[1].y = startY-gap;
    dstRects[1].w = texW;
    dstRects[1].h = texH;
    
    // Нижний левый элемент
    dstRects[2].x = startX - gap;
    dstRects[2].y = startY + texH + gap;
    dstRects[2].w = texW;
    dstRects[2].h = texH;
    
    // Нижний правый элемент
    dstRects[3].x = startX + texW + gap;
    dstRects[3].y = startY + texH + gap;
    dstRects[3].w = texW;
    dstRects[3].h = texH;
    
    // Отрисовка всех текстур
    for (int i = 0; i < 4; i++) {
        renderTexture(petTextures[i], &dstRects[i]);
    }
}

// Метод для удаления меню
static void dead_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&exitButton);

    // Очищаем фон
    SDL_DestroyTexture(background);
    background = NULL;
    // Спрайты
    SDL_DestroyTexture(text_dead);
    text_dead = NULL;

    // удаляем питомца
    invisible_pet();

    // Очистка текстур
    if (petTextures != NULL) {
        // Предполагаем что массив заканчивается NULL-указателем
        for(int i = 0; i < 4; i++) {
            if (petTextures[i] != NULL) {
                SDL_DestroyTexture(petTextures[i]);
                petTextures[i] = NULL;
            }
        }
        
        // Освобождаем сам массив указателей
        free(petTextures);
        petTextures = NULL;
    }

    if(IS_SOUND && backgroundMusic){
        Mix_FreeMusic(backgroundMusic); // Перестать воспроизводить музыку
    }
}

// Реакция на нажатаю кнопку
static void onexitButtonClick() {
    // Логика при нажатии кнопки
    const char *prefix = "save№";
    const char *suffix = ".dat";
    int petID = getIdPet();

    // Вычисляем необходимую длину для итоговой строки
    size_t len = snprintf(NULL, 0, "%s%d%s", prefix, petID, suffix);
    char *filename = malloc(len + 1); // +1 для нулевого терминатора

    snprintf(filename, len + 1, "%s%d%s", prefix, petID, suffix);
    remove(get_save_path(filename));
    SDL_free(filename); // очищаем
    init_pet(petID);
    set_scene(&GAME_SCENE);
}


// Инициализация меню(его создание и отображение)
static void dead_init() {
    // Загрузка фона
    background = loadTexture("Background.png");

    // Инициализация капель
    for (int i = 0; i < NUM_DROPLETS; i++) {
        droplets[i].x = rand() % WINDOW_WIDTH;
        droplets[i].y = -(rand() % 200); // Начинаем выше экрана для плавного появления
        droplets[i].speed = 100 + rand() % 200; // Случайная скорость от 100 до 300 пикселей/сек
        droplets[i].length = 10 + rand() % 20;    // Длина от 10 до 30 пикселей
        droplets[i].alpha = 200;                  // Полупрозрачные капли
    }

    // Инициализация системы брызг.
    for (int i = 0; i < MAX_SPLASHES; i++) {
        splashes[i].active = false;
    }

    // Инициализация кнопки (координаты, размеры)
    initButton(&exitButton,
        WINDOW_WIDTH/2-101, WINDOW_HEIGHT-100, 203, 108,
        "button_restart.png",
        "button_restart_hover.png", // используем default для hover
        "button_restart_hover.png", // используем default для click
        onexitButtonClick,
        NULL
    );

    // Разница кусочков
    dif_pieces = 1;

    // Разделяем на эллементы
    petTextures = splitTextureFour(getAssetPath(getTexturePet()));
    
    // Спрайты 
    text_dead = loadTexture("potracheno.png");
    int textW, textH;
    sizeTexture(text_dead, &textW, &textH);
    text_dead_rect.w = textW/2;
    text_dead_rect.h = textH/2;   
    text_dead_rect.y = 0;  

    if(IS_SOUND){
        // Музыка
        backgroundMusic = Mix_LoadMUS(getAssetPath("sounds/music_dead.mp3"));
        if (!backgroundMusic) {
            SDL_Log("Failed to load music: %s\n", Mix_GetError());
        } else {
            // Проиграть музыку бесконечно (-1)
            Mix_PlayMusic(backgroundMusic, -1);
        }
    }
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void dead_handle_events(SDL_Event* e) {
    // Выход в меню
    if (e->type == SDL_KEYDOWN) {
        // Пример: ESC -> вернуть в меню
        if (e->key.keysym.sym == SDLK_ESCAPE ||
            e->key.keysym.scancode == SDL_SCANCODE_AC_BACK
        ) {
            extern Scene MENU_SCENE;
            save_game();
            set_scene(&MENU_SCENE);
        }
    }
    // Обрабатываем кнопку
    handleButtonEvent(&exitButton, e);
}

/**
 * @brief Создает новую брызгу (частицу) с заданными параметрами.
 *
 * @param x            Позиция X появления брызги.
 * @param y            Позиция Y появления брызги.
 * @param initialRadius Начальный радиус брызги.
 * @param maxRadius     Максимальный радиус, до которого расширится брызга.
 * @param lifetime      Время жизни брызги в секундах.
 */
void spawnSplash(float x, float y, float initialRadius, float maxRadius, float lifetime) {
    for (int i = 0; i < MAX_SPLASHES; i++) {
        if (!splashes[i].active) {
            splashes[i].x = x + rand() % 100;
            splashes[i].y = y + rand() % 100;
            splashes[i].radius = initialRadius + rand() % 10;
            splashes[i].maxRadius = maxRadius + rand() % 15;
            splashes[i].lifetime = (lifetime+ rand() % 3)/2;
            splashes[i].elapsed = 0.0f;
            // Начальный цвет — ярко-красный, полностью непрозрачный
            splashes[i].color = (SDL_Color){255, 0, 0, 255};
            splashes[i].active = true;
            break;
        }
    }
}

// Переменные для анимаций
static bool is_extension = true;
static float splash_time = 0.0;

// Логика во время меня(обновляется в бесконечном цикле)
// delta - тик времени
static void dead_update(float delta) {    
    updateButton(&exitButton, delta);

    // Анимация расчленения питомца
    if(is_extension){
        dif_pieces += 100 * delta;
        if(dif_pieces > 100)
            is_extension = false;
    } else{
        dif_pieces -= 60 * delta;
        if(dif_pieces < 30)
            is_extension = true;
    }


    // Надпись на экране об окончание игры
    if(text_dead_rect.w+1 < WINDOW_WIDTH){
        text_dead_rect.w += 100 * delta ;
        text_dead_rect.h += 60 * delta;
    }

    // Обновление капель
    for (int i = 0; i < NUM_DROPLETS; i++) {
        droplets[i].y += droplets[i].speed * delta;
        // Если капля ушла за нижнюю границу, сбрасываем её в верхнюю часть с новым случайным X.
        if (droplets[i].y > WINDOW_HEIGHT) {
            droplets[i].y = -droplets[i].length;
            droplets[i].x = rand() % WINDOW_WIDTH;
        }
    }

    // Спавн крови
    splash_time += delta;
    if(splash_time >= 0.2){
        splash_time = 0.0;
        spawnSplash(
            (float)WINDOW_WIDTH/2-50, 
            (float)WINDOW_HEIGHT/2-50, 
            1.0f, 
            20.0f, 
            0.4f
        );
    }

    // Обновление анимации крови
    for (int i = 0; i < MAX_SPLASHES; i++) {
        if (splashes[i].active) {
            splashes[i].elapsed += delta;
            float t = splashes[i].elapsed / splashes[i].lifetime;
            if (t > 1.0f) t = 1.0f;
            // Линейная интерполяция радиуса от начального до максимального
            // (Можно изменить алгоритм для нелинейного эффекта)
            splashes[i].radius = (1.0f - t) * splashes[i].radius + t * splashes[i].maxRadius;
            // Постепенное уменьшение прозрачности
            splashes[i].color.a = (Uint8)(255 * (1.0f - t));
            if (splashes[i].elapsed >= splashes[i].lifetime) {
                splashes[i].active = false;
            }
        }
    }
}


// Отображение меню(его статической состоявляющей)
static void dead_render() {
    // Фон
    rectdict.w = WINDOW_WIDTH;
    rectdict.h = WINDOW_HEIGHT;
    renderTexture(background, &rectdict);

    // Затемнить фон
    drawTransparentBlackSquare(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Рисуем анимацию и т.д.
    deadAnimation(
        dstRects, petTextures, 
        WINDOW_WIDTH/2-((int)(getWidthPet()*getScaleW()))/2, 
        WINDOW_HEIGHT/2-((int)(getHighPet()*getScaleH()))/2,
        getScaleW(), getScaleH(), 
        dif_pieces/3
    );

    // Отрисовка брызгов крови
    for (int i = 0; i < MAX_SPLASHES; i++) {
        if (splashes[i].active) {
            filledCircleRGBA(gRenderer,
                (Sint16)splashes[i].x,(Sint16)splashes[i].y,
                (Sint16)splashes[i].radius*MIN(sizerH, sizerW),
                splashes[i].color.r,
                splashes[i].color.g,
                splashes[i].color.b,
                splashes[i].color.a
            );
        }
    }

    // Отрисовка капель. Используется aaLineRGBA для антиалиасинга.
    for (int i = 0; i < NUM_DROPLETS; i++) {
        aalineRGBA(gRenderer, 
                   (Sint16)droplets[i].x, (Sint16)droplets[i].y, 
                   (Sint16)droplets[i].x+2, (Sint16)(droplets[i].y + droplets[i].length), 
                   1, 5, 249, droplets[i].alpha);
    }

    // Спрайты
    text_dead_rect.x = WINDOW_WIDTH/2 - text_dead_rect.w/2;
    renderTexture(text_dead, &text_dead_rect);

    // Рисуем кнопку
    exitButton.rect.w = 203*sizerH;
    exitButton.rect.h = 108*sizerH;
    exitButton.rect.x = WINDOW_WIDTH/2-exitButton.rect.w/2;
    exitButton.rect.y = WINDOW_HEIGHT-exitButton.rect.h-20*sizerH;
    renderButton(&exitButton);
}


// Объект сцены
Scene DEAD_SCENE = {
    .init = dead_init,
    .handle_events = dead_handle_events,
    .update = dead_update,
    .render = dead_render,
    .destroy = dead_destroy
};
