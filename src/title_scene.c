#include <SDL.h>
#include <SDL_ttf.h>
#include "SDL_render.h"
#include "graphics.h"
#include "menu_scene.h"
#include "title_scene.h"
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h" 
#include "file_manager.h"  

// Кнопка старт
static Button startButton;

// text
SDL_Texture* textTexture;
SDL_Color textColor = {0, 0, 0, 255}; // Белый цвет


/**
 * @brief Читает текст из файла, рендерит его в SDL_Surface с переносом строк и создает текстуру.
 *
 * @param renderer   Указатель на SDL_Renderer.
 * @param filePath   Путь к текстовому файлу.
 * @param font       Шрифт для рендеринга текста.
 * @param textColor  Цвет текста.
 * @param wrapLength Максимальная ширина строки (в пикселях), после которой текст будет переноситься.
 * @return SDL_Texture*, содержащая отрендеренный текст, или NULL в случае ошибки.
 */
 SDL_Texture* renderTextFromFile(const char* filePath, TTF_Font* font, SDL_Color textColor, int wrapLength) {
    SDL_RWops *rw = SDL_RWFromFile(getAssetPath(filePath), "rb");
    if (!rw) {
        SDL_Log("Ошибка открытия: %s", SDL_GetError());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Ошибка",
            "Не удалось открыть файл", 
            NULL
        );
        return NULL;
    }
    Sint64 size = SDL_RWsize(rw);
    char* text = SDL_malloc(size + 1);
    SDL_RWread(rw, text, 1, size);
    text[size] = '\0';
    SDL_RWclose(rw);

    // Рендерим текст с автоматическим переносом строк
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended_Wrapped(font, text, textColor, wrapLength);
    SDL_free(text);
    
    if (!textSurface) {
        SDL_Log("Ошибка рендеринга текста: %s", TTF_GetError());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Ошибка",
            "Ошибка рендеринга текста", 
            NULL
        );
        return NULL;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    SDL_FreeSurface(textSurface);
    
    return textTexture;
}

// Метод для удаления меню
static void menu_destroy(void) {
    // Удаляем ресурсы кнопки (если есть текстуры)
    destroyButton(&startButton);
    SDL_DestroyTexture(textTexture);
}

// Реакция на нажатаю кнопку
static void onStartButtonClick() {
    extern SDL_Renderer* gRenderer;
    // Логика при нажатии кнопки
    menu_destroy();
    set_scene(&MENU_SCENE);
}

// Инициализация меню(его создание и отображение)
static void menu_init() {
    // Инициализация кнопки (координаты, размеры)
    initButton(&startButton,
        0, 0, 
        1558*sizerH*0.5, 
        834*sizerH*0.5,
        "button_start.png",
        "button_start.png", // используем для hover
        "button_start.png", // используем для click
        onStartButtonClick,
        NULL
    );

    TTF_Font* font = TTF_OpenFont(getAssetPath("fonts/BenbowSemibold.ttf"), 24*sizerW);
    if (!font) {
        SDL_Log("Ошибка загрузки шрифта: %s", TTF_GetError());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Ошибка",
            "Ошибка загрузки шрифта", 
            NULL
        );
        // Обработка ошибки
    }
    
    // Создаем текстуру из файла с переносом строк по ширине окна
    textTexture = renderTextFromFile("txt/info.txt", font, textColor, WINDOW_WIDTH);
    if (!textTexture) {
        SDL_Log("Не удалось создать текстуру из файла");
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Ошибка",
            "Не удалось создать текстуру из файла", 
            NULL
        );
        return;
    }
}

// Обработка эвентов когда меню инициализировано 
// @param e - эвенты из SDL_Event
static void menu_handle_events(SDL_Event* e) {
    // Обрабатываем кнопку
    handleButtonEvent(&startButton, e);
}

// Логика во время меня(обновляется в бесконечном цикле)
// @param delta - тик времени
static void menu_update(float delta) {
    (void)delta;  // Заглушка, чтобы не ругался компилятор
}

// Отображение меню(его статической состоявляющей)
static void menu_render() {
    
    // Устанавливаем прямоугольник для вывода на весь экран
    SDL_Rect destRect = { 
        35*sizerW, 
        35*sizerH, 
        WINDOW_WIDTH-50*sizerW, 
        WINDOW_HEIGHT-50*sizerH 
    };
    renderTexture(textTexture, &destRect);
    
    // Рисуем кнопку
    startButton.rect.w = 1558*sizerH*0.15;
    startButton.rect.h = 834*sizerH*0.15;     
    startButton.rect.x = WINDOW_WIDTH-0.9*startButton.rect.w - 40*sizerH;
    startButton.rect.y = WINDOW_HEIGHT-0.9*startButton.rect.h - 40*sizerH;

    renderButton(&startButton);
}

// Объект сцены
Scene TITLE_SCENE = {
    .init = menu_init,
    .handle_events = menu_handle_events,
    .update = menu_update,
    .render = menu_render,
    .destroy = menu_destroy
};
