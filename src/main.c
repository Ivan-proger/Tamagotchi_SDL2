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

    // Получаем частоту счетчика (тиков в секунду). Делаем это один раз.
    Uint64 perf_frequency = SDL_GetPerformanceFrequency();
    if (perf_frequency == 0) {
         fprintf(stderr, "Error: Performance counter not supported or frequency is zero.\n");
         // Можно перейти на SDL_GetTicks64() как на запасной вариант, если нужно
         SDL_Quit();
         return 1;
    }

    // Получаем начальное значение счетчика
    Uint64 last_counter = SDL_GetPerformanceCounter();

    double delta = 0.0; // Дельта время в секундах


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
        // --- Начало кадра ---
        Uint64 current_counter = SDL_GetPerformanceCounter();

        // Вычисляем количество тиков, прошедших с прошлого кадра
        // Используем безопасное вычитание для предотвращения проблем с переполнением (хотя для 64 бит маловероятно)
        Uint64 elapsed_ticks = current_counter - last_counter;

        // Конвертируем тики в секунды (с плавающей точкой двойной точности)
        delta = (double)elapsed_ticks / (double)perf_frequency;

        // Обновляем счетчик для следующего кадра
        last_counter = current_counter;

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
        
       
        update_pet(delta, 2.0);
        // Вызов апдейтера сцены
        scene_update(delta);

        // Рендер
        SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
        SDL_RenderClear(gRenderer);
        
        // Рендер сцены
        scene_render();

        SDL_RenderPresent(gRenderer);
    }

    // Уничтожаем сцену
    save_game(); //! Сохранение
    scene_destroy();
    cleanupGraphics();
    return 0;
}
