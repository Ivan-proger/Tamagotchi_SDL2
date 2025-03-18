#include <SDL2/SDL_image.h>
#include "graphics.h"
#include "ui.h"


bool initButton(Button *button,
                int x, int y, int w, int h,
                const char *defaultImagePath,
                const char *hoverImagePath,
                const char *clickImagePath,
                void (*onClick)(void))
{
    button->rect.x = x;
    button->rect.y = y;
    button->rect.w = w;
    button->rect.h = h;

    extern SDL_Renderer* gRenderer;
    // Загружаем основную текстуру
    button->defaultTexture = IMG_LoadTexture(gRenderer, defaultImagePath);
    if (!button->defaultTexture)
        return false;
    
    // Если пути hover или click не указаны, используем основную текстуру
    if (hoverImagePath)
        button->hoverTexture = IMG_LoadTexture(gRenderer, hoverImagePath);
    else
        button->hoverTexture = button->defaultTexture;
    
    if (clickImagePath)
        button->clickTexture = IMG_LoadTexture(gRenderer, clickImagePath);
    else
        button->clickTexture = button->defaultTexture;

    button->onClick = onClick;
    button->isHovered = false;
    button->isClicked = false;

    return true;
}

void renderButton(Button *button)
{
    //extern SDL_Renderer* gRenderer;
    SDL_Texture *currentTexture = button->defaultTexture;
    if (button->isClicked && button->clickTexture)
        currentTexture = button->clickTexture;
    else if (button->isHovered && button->hoverTexture)
        currentTexture = button->hoverTexture;

    //SDL_RenderCopy(gRenderer, currentTexture, NULL, &button->rect);
    renderTexture(currentTexture, &button->rect);
}

void handleButtonEvent(Button *button, SDL_Event *event)
{
    extern SDL_Renderer* gRenderer;
    if (event->type == SDL_MOUSEMOTION ||
        event->type == SDL_MOUSEBUTTONDOWN ||
        event->type == SDL_MOUSEBUTTONUP)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        bool inside = (mx >= button->rect.x) && (mx <= button->rect.x + button->rect.w) &&
                      (my >= button->rect.y) && (my <= button->rect.y + button->rect.h);
        
        if (!inside) {
            button->isHovered = false;
            button->isClicked = false;
        } else {
            button->isHovered = true;
            if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
                button->isClicked = true;
            } else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
                if (button->isClicked && button->onClick) {
                    button->onClick();
                }
                button->isClicked = false;
            }
        }
    }
}

void destroyButton(Button *button)
{
    if (button->defaultTexture)
        SDL_DestroyTexture(button->defaultTexture);
    // Если hover и click текстуры отличаются от default, удаляем их отдельно
    if (button->hoverTexture && button->hoverTexture != button->defaultTexture)
        SDL_DestroyTexture(button->hoverTexture);
    if (button->clickTexture && button->clickTexture != button->defaultTexture)
        SDL_DestroyTexture(button->clickTexture);
}
