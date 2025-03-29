#include <SDL2/SDL_image.h>
#include "graphics.h"
#include "ui.h"

/**
 * @brief Инициализация кнопки
 * 
 * @param button -- Структура кнопки.
 * @param x      -- X координата для наличия кнопки.
 * @param y      -- Y координата для наличия кнопки.
 * @param w      -- Ширина кнопки.
 * @param h      -- Высота кнопки.
 * @param defaultImagePath -- Изначальное изображение кнопки .png.
 * @param hoverImagePath   -- Изображение при наведение на кнопку .png.
 * @param clickImagePath   -- Изображение при клике на кнопку .png.
 * @param onClick -- Функция срабатывающая после нажатия (аргументы в нее отсуствуют).
 * @return true .
 * @return false  -- В случае ошибки.
 */
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

/**
 * @brief Нарезает png картинку на анимации в длину, 
 * чтобы создать анимацию из 4 кадров длиной и широй 300пх надо иметь картинку 1200пх (300*4 в длинну)
 * нарезка происходит слева направа в заданых в slice_w(ширина) и slice_h(высота) пикселей
 * 
 * @param button     -- Указатель на кнопку в чью структуру добавят анимацию
 * @param animPath   -- Путь до файла с анимацией (фото .png)
 * @param quantity   -- Количество кадров в анимации
 * @param frametime  -- Время одного кадра
 * @param slice_w    -- Ширина для нарзеки(не ширина проигрываемой анимации)
 * @param slice_h    -- Высота нарезки
 */
void initButtonAnimation(Button *button, char* animPath,const int quantity, float frametime, int slice_w, int slice_h){
    // Анимация кнопки

    SDL_Texture* ButtonSheet = loadTexture(animPath);
    if (!ButtonSheet) {
        SDL_Log("Ошибка загрузки спрайт-листа питомца: %s", SDL_GetError());
    }

    SDL_Rect *ButtonFrames = malloc(quantity * sizeof(SDL_Rect));
    if(!(ButtonFrames)) {
        SDL_Log("Ошибка выделения памяти: %s", SDL_GetError());
    }

    for (int i = 0; i < quantity; i++) {
        ButtonFrames[i].x = i * slice_w;
        ButtonFrames[i].y = 0;
        ButtonFrames[i].w = slice_w;
        ButtonFrames[i].h = slice_h;
    }
    
    Animation* ButtonAnim;
    // Создаем анимацию, где каждый кадр показывается 0.5 секунды
    ButtonAnim = createAnimation(ButtonSheet, ButtonFrames, quantity, frametime);

    button->clickAnim = ButtonAnim;
    button->isAnimatingClick = false;
    button->clickAnimTimer = 0.0f;
    button->clickAnimDuration = frametime*quantity; // Задаём, например, 0.3 секунды
}

// Функция обновления кнопки (вызывается каждый кадр с delta временем)
void updateButton(Button *button, float delta) {
    if (button->isAnimatingClick && button->clickAnim) {
        button->clickAnimTimer += delta;
        // Обновляем анимацию клика
        updateAnimation(button->clickAnim, delta);
        if (button->clickAnimTimer >= button->clickAnimDuration) {
            button->isAnimatingClick = false;
            button->clickAnimTimer = 0.0f;
            // Возращаем в начальное положение начала анимации
            button->clickAnim->currentFrame = 0;
        }
    }
}

/**
 * @brief Отображение кнопки
 * 
 * @param button -- Структура кнопки.
 */
void renderButton(Button *button)
{
    // Если проигрывается анимация клика, отрисовываем её и выходим
    if (button->isAnimatingClick && button->clickAnim != NULL) {
        renderAnimation(button->clickAnim, button->rect.x, button->rect.y, 
                        button->rect.w, button->rect.h);
        return;
    }

    // Выбираем текущее состояние кнопки
    SDL_Texture *currentTexture = button->defaultTexture;

    if (button->isClicked && button->clickTexture)
        currentTexture = button->clickTexture;
    else if (button->isHovered && button->hoverTexture)
        currentTexture = button->hoverTexture;
    
    // Отрисовка выбранной текстуры
    renderTexture(currentTexture, &button->rect);
}

/**
 * @brief Обработчик реакции при нажатие 
 * 
 * @param button -- Структура кнопки
 * @param event  -- Эвент SDL2
 */
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
                if (button->isClicked && button->isAnimatingClick == false) {
                    // Если нажата кнопка, запускаем анимацию клика, если она есть,
                    // и вызываем обработчик нажатия
                    if (button->clickAnim != NULL) {
                        button->isAnimatingClick = true;
                        button->clickAnimTimer = 0.0f;
                    }
                    if (button->onClick) {
                        button->onClick();
                    }
                }
                button->isClicked = false;
            }
        }
    }
}

/**
 * @brief Удаление кнопки
 * 
 * @param button -- Структура кнопки. 
 */
void destroyButton(Button *button)
{
    if (button->defaultTexture)
        SDL_DestroyTexture(button->defaultTexture);
    // Если hover и click текстуры отличаются от default, удаляем их отдельно
    if (button->hoverTexture && button->hoverTexture != button->defaultTexture)
        SDL_DestroyTexture(button->hoverTexture);
    if (button->clickTexture && button->clickTexture != button->defaultTexture)
        SDL_DestroyTexture(button->clickTexture);
    // Анимация
    if (button->clickAnim){
        destroyAnimation(button->clickAnim);
    }
}
