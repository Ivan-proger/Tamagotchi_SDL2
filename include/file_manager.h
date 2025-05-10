#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

// Возвращает полное имя файла assets для последующего открытия.
// - На Android возвращает просто filename, SDL_RWFromFile найдёт его в assets/ внутри APK.
// - На других платформах (Linux, Windows, macOS) возвращает "<path_to_exe>/assets/filename".
// Освобождать возвращённую строку нужно через SDL_free().
char* getAssetPath(const char* filename);

/**
 * Возвращает malloc()’ный C‑строку с полным путём к файлу сохранения.
 * Пользователь обязан вызвать free() после использования.
 *
 * @param filename  Относительный путь внутри папки сохранений,
 *                  например "save1.dat" или "saves/save1.dat"
 * @return          Полный путь, например
 *                  - Android: "/data/data/your.pkg.name/files/save1.dat"
 *                  - Windows/Linux: "save1.dat" (то есть relative)
 */
 char* get_save_path(const char* filename);

#endif