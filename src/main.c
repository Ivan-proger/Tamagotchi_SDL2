#include <locale.h>
#include "SDL_timer.h"
#include "graphics.h"
#include "globals.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include "scene_manager.h"
#include "title_scene.h"
#include "menu_scene.h"
#include "pet.h"
#include "file_manager.h"

int WINDOW_WIDTH = BASE_WINDOW_WIDTH;
int WINDOW_HEIGHT = BASE_WINDOW_HEIGH;

// Переменная для масштабирования интерфейса
float sizerW = 1;
float sizerH = 1; 

// звук on
bool IS_SOUND = true;
int VALUE_SOUND = MIX_MAX_VOLUME;
// Базовый звук нажатия кнопки (если не указан иной в ее инициализации)
Mix_Chunk *clickSound;

int main(int argc, char* argv[]) {
    // Установка локали
    const char *loc = setlocale(LC_ALL, "ru_RU.UTF-8");
    if (!loc) {
        SDL_Log("Локаль ru_RU.UTF-8 не установлена!\n");
    } else {
        SDL_Log("Текущая локаль: %s\n", loc);
    }

    // Инициализация графики
    if (!initGraphics("Tamagotchi Game", WINDOW_WIDTH, WINDOW_HEIGHT)) {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Ошибка",
            "Не удалось создать окно.",
            NULL
        );
        return 1;
    }

    // Инициализайия шрифтов
    if (TTF_Init() == -1) {
        SDL_Log("Ошибка инициализации TTF: %s", TTF_GetError());
        // Обработка ошибки
    }

    // Инициализация звуков и музыки
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL init error: %s\n", SDL_GetError());
        return -1;
    }
    
    // Инициализация формата MP3
    int flags = MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        SDL_Log("Mix_Init error: %s\n", Mix_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Открытие аудиоустройства
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
        SDL_Log("Mix_OpenAudio error: %s\n", Mix_GetError());
        Mix_Quit();
        SDL_Quit();
        return -1;
    }

    // Базовый звук нажатия кнопки (если не указан иной в ее инициализации)
    clickSound = Mix_LoadWAV(getAssetPath("sounds/click.wav"));
    if (!clickSound) {
        SDL_Log("Failed to load sound: %s\n", Mix_GetError());
    }

    //  игровой цикл
    int running = 1;
    SDL_Event event;

    // Ставим сцену меню
    set_scene(&TITLE_SCENE);

    // Получаем частоту счетчика (тиков в секунду). Делаем это один раз.
    Uint64 perf_frequency = SDL_GetPerformanceFrequency();
    if (perf_frequency == 0) {
         SDL_Log("Error: Performance counter not supported or frequency is zero.\n");
         // Можно перейти на SDL_GetTicks64() как на запасной вариант, если нужно
         return 1;
    }

    // Получаем начальное значение счетчика
    Uint64 last_counter = SDL_GetPerformanceCounter();

    double delta = 0.0; // Дельта время в секундах


    // Глобальные переменные для подсчёта FPS
    int frameCount = 0;
    float fpsTimer = 0.0f;
    float currentFPS = 0.0f;

    // Если вдруг откроется не в том разрешение 
    sizerW = 1.0*WINDOW_WIDTH/BASE_WINDOW_WIDTH;
    sizerH = 1.0*WINDOW_HEIGHT/BASE_WINDOW_HEIGH;

    while (running) {
        // Выход по кнопке
        if(POWEROFF){
            running = 0;
        }
        // Обработка событий
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
                break;
            }
            // Обработка оконных событий ИЗМЕНЕНИЯ ОКНА
            if (e.type == SDL_WINDOWEVENT) {
                if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                    WINDOW_WIDTH = e.window.data1;
                    WINDOW_HEIGHT = e.window.data2;

                    sizerW = 1.0*WINDOW_WIDTH/BASE_WINDOW_WIDTH;
                    sizerH = 1.0*WINDOW_HEIGHT/BASE_WINDOW_HEIGH;

                    SDL_Log("Изменения размеров окна w: %d  h: %d", WINDOW_WIDTH, WINDOW_HEIGHT);
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
    Mix_CloseAudio(); // Закрываем музыку
    Mix_FreeChunk(clickSound); // Закрываем базовый звук для нажатия на кнопку
    Mix_Quit();
    cleanupGraphics();
    return 0;
}
