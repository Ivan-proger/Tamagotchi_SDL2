#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL.h>
#include <stdbool.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))  // Макрос для нахождения минимума

// Для масштабирования ui под смартфоны (изначально все версталось для 700px высоты)
#define BASE_WINDOW_HEIGH 700 
#define BASE_WINDOW_WIDTH 460 

// Ширина окна
extern int WINDOW_WIDTH;

// Высота окна
extern int WINDOW_HEIGHT;

// Коэффициент масштабирования
extern float sizerH; 
extern float sizerW;

// Музыка on/off
extern bool IS_SOUND;

// Громкость музыки и звуков
extern int VALUE_SOUND;

#endif
