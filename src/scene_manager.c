#include <SDL2/SDL.h>
#include "graphics.h"
#include "scene_manager.h"

static Scene* current_scene = NULL;

void set_scene(Scene* new_scene) {
    if (current_scene && current_scene->destroy)
        current_scene->destroy();
    current_scene = new_scene;
    if (current_scene && current_scene->init)
        current_scene->init();
}


void scene_handle_events(SDL_Event* e) {
    if (current_scene && current_scene->handle_events)
        current_scene->handle_events(e);
}

void scene_update(float delta) {
    if (current_scene && current_scene->update)
        current_scene->update(delta);
}


void scene_render() {
    if (current_scene && current_scene->render)
        current_scene->render();
}


void scene_destroy(void) {
    if (current_scene && current_scene->destroy)
        current_scene->destroy();
    current_scene = NULL;
}
