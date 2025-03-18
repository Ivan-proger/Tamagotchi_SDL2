#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

typedef struct Scene {
    void (*init)(void);           // Инициализация
    void (*handle_events)(SDL_Event *e); // Обработка событий
    void (*update)(float delta);      // Обновление логики
    void (*render)(void);             // Отрисовка
    void (*destroy)(void);            // Очистка при выходе
} Scene;

// Управление активной сценой
void set_scene(Scene* new_scene);
void scene_handle_events(SDL_Event* e);
void scene_update(float delta);
void scene_render();
void scene_destroy(void);

#endif
