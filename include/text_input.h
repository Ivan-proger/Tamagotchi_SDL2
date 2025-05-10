#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>

// InputField: структура для текстового поля ввода
typedef struct {
    char  *buffer;         // Динамически аллоцируемый буфер
    size_t length;         // Текущая длина текста
    size_t capacity;       // Выделенный объём памяти
    size_t len;         // Текущая длина текста в буфере
    SDL_Rect rect;         // Прямоугольник области поля
    TTF_Font *font;        // Шрифт для отрисовки текста
    SDL_Color color;       // Цвет текста
    bool active;           // Флаг активности поля (принимает ли ввод)
} InputField;

// Инициализация
void InputField_Init(InputField *field,
                     TTF_Font *font,
                     SDL_Color color,
                     SDL_Rect rect);

// Обработка событий
// Возвращает true, если ввод завершён (нажат Enter)
bool InputField_HandleEvent(InputField *field, const SDL_Event *event);

// Отрисовка
void InputField_Render(const InputField *field);

// Установить начальный текст (перезаписать буфер)
void InputField_SetText(InputField *f, const char *text);

// Получить указатель на текущий текст
char* InputField_GetText(const InputField *f);

// Освобождение ресурсов
void InputField_Destroy(InputField *field);


#endif