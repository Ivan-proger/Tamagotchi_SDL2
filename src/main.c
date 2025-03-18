#include "../include/graphics.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include "scene_manager.h"
#include "menu_scene.h"
#include "globals.h"

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
    // Инициализация графики
    if (!initGraphics("Tamagotchi Game", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        printf("Ошибка инициализации графики\n");
        return 1;
    }
    
    // Простой игровой цикл
    int running = 1;
    SDL_Event event;
    // Ставим сцену меню
    set_scene(&MENU_SCENE);

    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        // Обработка событий
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                break;
            }
            // Изменение окна
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                WINDOW_WIDTH = event.window.data1;
                WINDOW_HEIGHT = event.window.data2;
                // Дополнительно можно обновлять матрицы или другие параметры рендеринга
            }

            scene_handle_events(&e);
        }

        // Подсчёт delta
        Uint32 currentTick = SDL_GetTicks();
        float delta = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        scene_update(delta);

        // Рендер
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);
        scene_render(gRenderer);
        SDL_RenderPresent(gRenderer);
    }

    // Уничтожаем сцену
    scene_destroy();
    cleanupGraphics();
    return 0;
}
