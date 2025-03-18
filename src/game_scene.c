#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "graphics.h"
#include "game_scene.h"
#include "scene_manager.h"

// Локальная переменная: текстура питомца
static SDL_Texture *petTexture = NULL;
static SDL_Rect petRect; // Координаты для отрисовки

// ============ Локальные функции ============

/**
 * @brief Инициализация игровой сцены
 * 
 */
static void game_init() {
    // Загружаем картинку питомца (здесь нужно иметь renderer, см. общий контекст)
    // Загрузка ресурсов
    petTexture = loadTexture("assets/pet.png");
    printf("DOWNLOAD TEXTURE! \n");

    // Отрисовка питомца (пример)
    petRect.x = 300;
    petRect.y = 200;
    petRect.w = 200;
    petRect.h = 200;
    
    renderTexture(petTexture, &petRect);
}

/**
 * @brief Обработка эвентов 
 * 
 * @param e -- Эвент sdl.
 */
static void game_handle_events(SDL_Event* e) {
    // Обработка событий в игре (клики, клавиатура и т.п.)
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
    // Логика "Тамагочи": голод, настроение и т.д.
    (void)delta; // Заглушка
}

// Отображение статики
static void game_render() {
    // Отрисовываем картинку
    renderTexture(petTexture, &petRect);
}

// Удаление сцены
static void game_destroy(void) {
    if (petTexture) {
        SDL_DestroyTexture(petTexture);
        petTexture = NULL;
    }
}

// Объект сцены
Scene GAME_SCENE = {
    .init = game_init,
    .handle_events = game_handle_events,
    .update = game_update,
    .render = game_render,
    .destroy = game_destroy
};
