#include <SDL2/SDL.h>
#include "graphics.h"
#include "scene_manager.h"

static Scene* current_scene = NULL;

// Установить сцену главной
// @param new_scene - экземпляр структуры сцены
void set_scene(Scene* new_scene) {
    if (current_scene && current_scene->destroy)
        current_scene->destroy();
    current_scene = new_scene;
    if (current_scene && current_scene->init)
        current_scene->init();
}

// Передача эвентов в нужную сцену выбранную главной
// @param e - эвенты из SDL_Event
void scene_handle_events(SDL_Event* e) {
    if (current_scene && current_scene->handle_events)
        current_scene->handle_events(e);
}
// Обработка эвентов когда сцена инициализировано 
// @param e - эвенты из SDL_Event
void scene_update(float delta) {
    if (current_scene && current_scene->update)
        current_scene->update(delta);
}

// Логика во время выбраной сцены(обновляется в бесконечном цикле)
void scene_render() {
    if (current_scene && current_scene->render)
        current_scene->render();
}

// Удаление сцены
void scene_destroy(void) {
    if (current_scene && current_scene->destroy)
        current_scene->destroy();
    current_scene = NULL;
}
