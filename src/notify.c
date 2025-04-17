#include "notify.h"
#include <stdlib.h>
#include <stdio.h>


// LINUX 👨‍❤️‍💋‍👨
#if defined(__linux__)    
    #include <libnotify/notify.h>
    #include "pet.h" // Для изображения питомца в уведомление

    static GError *error = NULL;

    static void on_action_cb(NotifyNotification *n, char *action, gpointer user_data) {
        g_print("Пользователь нажал: %s\n", action);
        notify_notification_close(n, NULL);
    }

    void notify_user(const char* title, const char* msg) {
        if (!notify_init("Tamagotchi")) return;

        NotifyNotification *n =
            notify_notification_new(title, msg, "dialog-information");

        // Кнопка действия
        notify_notification_add_action(
            n,
            "close",
            "Закрыть",
            on_action_cb,
            NULL,
            NULL
        );

        // Заменять по ключу, если уже выводилось
        notify_notification_set_hint(n,
            "x-canonical-private-synchronous",
            g_variant_new_string("tamagotchi-status"));

        // Срочность и время
        notify_notification_set_urgency(n, NOTIFY_URGENCY_NORMAL);
        notify_notification_set_timeout(n, 7000); // 7 сек

        // Встраиваем иконку (стандартная иконка info)
        // можно также load pixbuf из файла
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(pet.pathImage, &error);
        if (!pixbuf) {
            g_printerr("Ошибка загрузки изображения: %s\n", error->message);
            g_error_free(error);
            // Обработка ошибки
        }
        notify_notification_set_image_from_pixbuf(n, pixbuf);

        // Отобразить
        notify_notification_show(n, NULL);

        g_object_unref(G_OBJECT(pixbuf));
        g_object_unref(G_OBJECT(n));
        notify_uninit();
    }

#elif defined(_WIN32)
  // Windows: простое диалоговое окно MessageBoxA (Win32 API) :contentReference[oaicite:6]{index=6}
  #include <windows.h>
  void notify_user(const char* title, const char* msg) {
      MessageBoxA(NULL, msg, title,
                  MB_OK | MB_ICONINFORMATION);
  }

#else
  // Всё остальное: fallback на SDL2 modal message box :contentReference[oaicite:7]{index=7}
  #include <SDL2/SDL.h>
  void notify_user(const char* title, const char* msg) {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
                               title, msg, NULL);
  }
#endif
