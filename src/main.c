#include "../include/graphics.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include "scene_manager.h"
#include "menu_scene.h"
#include "globals.h"
#include "pet.h"

int WINDOW_WIDTH = 460;
int WINDOW_HEIGHT = 700;

int main(int argc, char* argv[]) {
    // Инициализация графики
    if (!initGraphics("Tamagotchi Game", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        printf("Ошибка инициализации графики\n");
        return 1;
    }
    

    // Инициализация питомца
    init_pet();

    //  игровой цикл
    int running = 1;
    SDL_Event event;
    // Ставим сцену меню
    set_scene(&MENU_SCENE);

    Uint32 lastTick = SDL_GetTicks();

    // Глобальные переменные для подсчёта FPS
    int frameCount = 0;
    float fpsTimer = 0.0f;
    float currentFPS = 0.0f;

    while (running) {
        // Обработка событий
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                break;
            }
            // Обработка оконных событий
            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    WINDOW_WIDTH = e.window.data1;
                    WINDOW_HEIGHT = e.window.data2;
                    // Дополнительно можно обновлять матрицы или другие параметры рендеринга
                }
            }
            // Отдаем эвент сцене
            scene_handle_events(&e);
        }

        // Подсчёт delta
        Uint32 currentTick = SDL_GetTicks();
        float delta = (currentTick - lastTick) / 1000.0f;
        lastTick = currentTick;

        // Увеличиваем счётчик кадров и добавляем время, прошедшее с прошлого кадра
        frameCount++;
        fpsTimer += delta;

        // Если прошло 1 или более секунд, вычисляем FPS и сбрасываем счётчики
        if (fpsTimer >= 1.0f) {
            currentFPS = frameCount / fpsTimer;
            printf("FPS: %.2f\n", currentFPS);
            frameCount = 0;
            fpsTimer = 0.0f;
        }
        
        // Вызов апдейтера сцены
        scene_update(delta);

        // Рендер
        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        SDL_RenderClear(gRenderer);
        
        // Рендер сцены
        scene_render(gRenderer);

        SDL_RenderPresent(gRenderer);
    }

    // Уничтожаем сцену
    save_game(); //! Сохранение
    scene_destroy();
    cleanupGraphics();
    return 0;
}
