#include <SDL.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
    #include <direct.h>  // Для _mkdir
#else
    #include <sys/stat.h>  // Для stat и mkdir
    #include <sys/types.h>
#endif

// Возвращает полное имя файла assets для последующего открытия.
// - На Android возвращает просто filename, SDL_RWFromFile найдёт его в assets/ внутри APK.
// - На других платформах (Linux, Windows, macOS) возвращает "<path_to_exe>/assets/filename".
// Освобождать возвращённую строку нужно через SDL_free().
char* getAssetPath(const char* filename) {
#if defined(__ANDROID__)
    // Для Android внутри APK SDL_RWFromFile ищет сразу в assets/
    return SDL_strdup(filename);
#else
    // Получаем путь до каталога, где находится исполняемый файл
    char* basePath = SDL_GetBasePath();
    if (!basePath) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "SDL_GetBasePath failed: %s", SDL_GetError());
        return NULL;
    }

    // Длина: basePath + "assets/" + filename + '\0'
    size_t needed = SDL_strlen(basePath)
                  + SDL_strlen("assets/")
                  + SDL_strlen(filename)
                  + 1;

    char* fullPath = (char*)SDL_malloc(needed);
    if (!fullPath) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Out of memory building asset path");
        SDL_free(basePath);
        return NULL;
    }

    // Строим путь
    SDL_strlcpy(fullPath, basePath, needed);
    SDL_strlcat(fullPath, "assets/", needed);
    SDL_strlcat(fullPath, filename, needed);

    SDL_free(basePath);
    return fullPath;
#endif
}


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
char* get_save_path(const char* filename) {
    char* fullpath;

    #if defined(__ANDROID__)
        // На Android файлы сохраняются во внутреннюю память приложения
        const char* base = SDL_AndroidGetInternalStoragePath();
        size_t len = strlen(base) + 1 + strlen(filename) + 1;
        fullpath = (char*)malloc(len);
        if (!fullpath) return NULL;
        // base уже оканчивается '/'
        strcpy(fullpath, base);
        strcat(fullpath, filename);

    #else
        // На Windows и на Linux используем просто относительный путь
        // На Windows/Linux — папка "saves/" рядом с исполняемым
        const char* dir = "saves";
        struct stat st = {0};

        // Если папки нет — создаём
        if (stat(dir, &st) != 0) {
        #if defined(_WIN32)
            _mkdir(dir);
        #else
            mkdir(dir, 0755);
        #endif
        }

        // Формируем полный путь "saves/filename"
        size_t len = SDL_strlen(dir) + 1 /* '/' */ + SDL_strlen(filename) + 1;
        fullpath = (char*)SDL_malloc(len);
        if (!fullpath) return NULL;
        SDL_strlcpy(fullpath, dir, len);
        SDL_strlcat(fullpath, "/", len);
        SDL_strlcat(fullpath, filename, len);
    #endif

        return fullpath;
}