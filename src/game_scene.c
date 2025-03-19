#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "graphics.h"
#include "game_scene.h"
#include "scene_manager.h"
#include "pet.h"
#include "globals.h"

SDL_Texture* background;
SDL_Rect rectdict;
// Инициализация игровой сцены
static void game_init() {
    load_texture_pet();
    show_pet();

    background = loadTexture("assets/Background.png");
    rectdict.x = 0;
    rectdict.y = 0;
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
    rectdict.w = WINDOW_WIDTH;
    rectdict.h = WINDOW_HEIGHT;
    renderTexture(background, &rectdict);

    // Отрисовываем картинку
    show_pet();


}

// Удаление сцены
static void game_destroy(void) {
    invisible_pet();
}

// Объект сцены
Scene GAME_SCENE = {
    .init = game_init,
    .handle_events = game_handle_events,
    .update = game_update,
    .render = game_render,
    .destroy = game_destroy
};
