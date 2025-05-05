#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <locale.h>
#include <string.h>
#include "graphics.h"
#include "text_input.h"


// Инициализация поля ввода
void InputField_Init(InputField *field,
                     TTF_Font *font,
                     SDL_Color color,
                     SDL_Rect rect)
{
    field->length   = 0;
    field->capacity = 64;  // или иной стартовый размер :contentReference[oaicite:3]{index=3}
    field->buffer   = malloc(field->capacity);
    if (!field->buffer) {
        // обработка ошибки аллока
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Failed to alloc buffer\n");
        exit(1);
    }
    field->buffer[0]    = '\0';
    field->rect         = rect;
    field->font         = font;
    field->color        = color;
    field->active       = false;
}

// Обработка SDL-событий для поля ввода
// Возвращает true, если пользователь нажал Enter
bool InputField_HandleEvent(InputField *field, const SDL_Event *event)
{
    // Переключение активности при клике мышью
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;
        bool inside = (x >= field->rect.x && x <= field->rect.x + field->rect.w &&
                       y >= field->rect.y && y <= field->rect.y + field->rect.h);
        if (inside && !field->active) {
            field->active = true;
            SDL_StartTextInput();  // Включаем режим ввода текста
        } else if (!inside && field->active) {
            field->active = false;
            SDL_StopTextInput();   // Отключаем режим ввода текста
        }
        return false;
    }

    // Обработка ввода символов и клавиш Backspace/Enter
    if (field->active) {    
        if (!field->active) return false;

        if (event->type == SDL_TEXTINPUT) {
            // длина нового куска UTF-8
            size_t add_len = strlen(event->text.text);
            // если не хватает места — вдвое расширяем
            if (field->length + add_len + 1 > field->capacity) {
                size_t new_cap = field->capacity * 2;
                char *new_buf = realloc(field->buffer, new_cap);
                if (!new_buf) {
                    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                                "Failed to realloc buffer\n");
                    return false;
                }
                field->buffer   = new_buf;
                field->capacity = new_cap;
            }
            // копируем новые байты и обновляем длину
            memcpy(field->buffer + field->length,
                event->text.text, add_len);
                field->length += add_len;
            field->buffer[field->length] = '\0';
        }
        else if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_BACKSPACE && field->length > 0) {
                // простое удаление одного байта (упрощённо)
                field->buffer[--field->length] = '\0';
            }
            else if (event->key.keysym.sym == SDLK_RETURN) {
                return true;
            }
        }
    }
    return false;
}

// Отрисовка поля ввода и текста
void InputField_Render(const InputField *field)
{
    // Рисуем рамку: зелёной если активно, иначе белой
    if (field->active) {
        SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    }
    SDL_RenderDrawRect(gRenderer, &field->rect);

    // Рендер текста через SDL_ttf
    SDL_Surface *surf = TTF_RenderUTF8_Blended(field->font,
                                              field->buffer,
                                              field->color);
    if (surf) {
        SDL_Texture *tex = SDL_CreateTextureFromSurface(gRenderer, surf);
        SDL_FreeSurface(surf);
        if (tex) {
            int w, h;
            SDL_QueryTexture(tex, NULL, NULL, &w, &h);
            SDL_Rect dst = { field->rect.x + 5,
                             field->rect.y + (field->rect.h - h) / 2,
                             w, h };
            SDL_RenderCopy(gRenderer, tex, NULL, &dst);
            SDL_DestroyTexture(tex);
        }
    }
}

// Установить начальный текст (перезаписать буфер)
void InputField_SetText(InputField *f, const char *text)
{
    size_t new_len = strlen(text);
    if (new_len + 1 > f->capacity) {
        size_t new_cap = f->capacity;
        while (new_cap <= new_len) new_cap *= 2;
        char *new_buf = realloc(f->buffer, new_cap);
        if (!new_buf) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to realloc buffer in SetText\n");
            return;
        }
        f->buffer = new_buf;
        f->capacity = new_cap;
    }
    memcpy(f->buffer, text, new_len + 1);
    f->length = new_len;
}

// Получить указатель на текущий текст
char* InputField_GetText(const InputField *f)
{
    return f->buffer;
}

// Освобождение ресурсов поля ввода
void InputField_Destroy(InputField *field)
{
    if (field->active) SDL_StopTextInput();
    // Освобождение шрифта и renderer выполнять снаружи
    free(field->buffer);  // освобождаем динамический буфер
}
