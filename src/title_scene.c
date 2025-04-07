#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "graphics.h"
#include "menu_scene.h"
#include "title_scene.h"
#include "game_scene.h"        // Чтобы при нажатии перейти в GAME_SCENE
#include "scene_manager.h"
#include "ui.h"             
#include "globals.h"   

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
    FILE *file = fopen(filePath, "r");
    if (!file) {
        SDL_Log("Не удалось открыть файл: %s", filePath);
        return NULL;
    }
    
    // Определяем размер файла
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);
    
    // Выделяем память для хранения текста
    char* text = malloc(fileSize + 1);
    if (!text) {
        SDL_Log("Ошибка выделения памяти для текста");
        fclose(file);
        return NULL;
    }
    
    fread(text, 1, fileSize, file);
    text[fileSize] = '\0';
    fclose(file);
    
    // Рендерим текст с автоматическим переносом строк
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended_Wrapped(font, text, textColor, wrapLength);
    free(text);
    
    if (!textSurface) {
        SDL_Log("Ошибка рендеринга текста: %s", TTF_GetError());
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
        0, 0, 500*0.4, 300*0.4,
        "assets/button_start.png",
        "assets/button_start_watch.png", // используем для hover
        "assets/button_start_click.png", // используем для click
        onStartButtonClick
    );

    TTF_Font* font = TTF_OpenFont("assets/fonts/BenbowSemibold.ttf", 24);
    if (!font) {
        SDL_Log("Ошибка загрузки шрифта: %s", TTF_GetError());
        // Обработка ошибки
    }
    
    // Создаем текстуру из файла с переносом строк по ширине окна
    textTexture = renderTextFromFile("info.txt", font, textColor, WINDOW_WIDTH);
    if (!textTexture) {
        SDL_Log("Не удалось создать текстуру из файла");
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
    SDL_Rect destRect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    renderTexture(textTexture, &destRect);
    
    // Рисуем кнопку
    startButton.rect.x = WINDOW_WIDTH-200;
    startButton.rect.y = WINDOW_HEIGHT-100;

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
