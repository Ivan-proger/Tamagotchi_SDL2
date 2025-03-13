#include "../include/graphics.h"
#include <SDL2/SDL.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    // Инициализация графики
    if (!initGraphics("Tamagotchi Game", 800, 600)) {
        printf("Ошибка инициализации графики\n");
        return 1;
    }
    
    // Загрузка ресурсов
    SDL_Texture* petTexture = loadTexture("assets/pet.png");
    if (!petTexture) {
        cleanupGraphics();
        return 1;
    }
    
    // Простой игровой цикл
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = 0;
            // Дополнительная обработка ввода
        }
        // Очистка экрана (например, заливаем чёрным)
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
        SDL_RenderClear(gRenderer);
        
        // Отрисовка питомца (пример)
        SDL_Rect petRect = { 300, 200, 200, 200 };
        renderTexture(petTexture, &petRect);
        
        // Обновляем экран
        presentGraphics();
        
        // Контроль частоты кадров
        SDL_Delay(16);
    }
    // Очистка ресурсов
    SDL_DestroyTexture(petTexture);
    cleanupGraphics();
    return 0;
}
