#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * @brief Универсальный вызов уведомления для Linux, Windows и SDL2.
 *
 * @param title Заголовок уведомления (UTF-8).
 * @param msg   Текст уведомления (UTF-8).
 */
void notify_user(const char* title, const char* msg);

#endif
