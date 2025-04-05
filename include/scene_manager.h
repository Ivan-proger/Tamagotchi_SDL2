#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H


/**
 * @brief Структура сцены
 * 
 * @param *init Указатель на функцию инициализации сцены
 * @param *handle_events Указатель на функцию обработки событий, принимает SDL_Event *e
 * @param *update Указатель на функцию обновлений принимает delta - время между кадрами в мс
 * @param *render Указаитель на функцию отрисовки
 * @param *destroy Указатель на функцию удаления сцены (очистка сцены при прекелючении на другую)
 */
typedef struct Scene {
    void (*init)(void);           // Инициализация
    void (*handle_events)(SDL_Event *e); // Обработка событий
    void (*update)(float delta);      // Обновление логики
    void (*render)(void);             // Отрисовка
    void (*destroy)(void);            // Очистка при выходе
} Scene;

// Установить сцену главной, также уничтожает предыдущую сцену
// @param new_scene экземпляр структуры сцены
void set_scene(Scene* new_scene);

// Считывать эвенты sdl на сцене 
void scene_handle_events(SDL_Event* e);

// Обновление сцены с учетом времени delta
void scene_update(float delta);

// Отрисовка сцены
void scene_render();

// Удаление сцены (очещение памяти)
void scene_destroy(void);

#endif
