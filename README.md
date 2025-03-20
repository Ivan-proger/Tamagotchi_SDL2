1. # Настройки для компиляции в Vscode

**Компилятор для работы игровой программы: gcc/gdb + SDL2**

## Windows: 
**Для работы требуется MinGv-w64 совместно с MSYS2**
(Для SDL2 также потребуеться этот модуль: **mingw-w64-x86_64-SDL2_gfx**)
<details>
  <summary>tasks.json</summary>

    {
        "version": "2.0.0",
        "tasks": [
        {
            "label": "Build Tamagotchi Game",
            "type": "shell",
            "command": "C:\\msys64\\mingw64\\bin\\gcc.exe",
            "args": [
            "-fdiagnostics-color=always",
            "-g",
            "src/main.c",
            "src/graphics.c",
            "src/scene_manager.c",
            "src/menu_scene.c",
            "src/game_scene.c",
            "src/ui.c",
            "src/pet.c",
            "-o",
            "${workspaceFolder}\\game.exe",
            "-I", "${workspaceFolder}\\include",
            "-I", "C:\\msys64\\mingw64\\include\\SDL2",
            "-L", "C:\\msys64\\mingw64\\lib",
            "-lmingw32",
            "-lSDL2main",
            "-lSDL2",
            "-lSDL2_image",   
            "-lSDL2_gfx",  
            "-lpng16", 
            "-lz", 
            "-ljpeg"     
            ],
            "options": {
            "cwd": "${workspaceFolder}"
            },
            "problemMatcher": ["$gcc"],
            "group": {
            "kind": "build",
            "isDefault": true
            }
        }
        ]
    }            
</details>

<details>
  <summary>c_cpp_properties.json</summary>

    {
        "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "C:/msys64/mingw64/include",
                "C:/msys64/mingw64/include/SDL2"                
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "windowsSdkVersion": "10.0.19041.0",
            "compilerPath": "cl.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-msvc-x64"
        }
    ],
        "version": 4
    }
</details>

## Linux

<details>
  <summary>tasks.json</summary>
  
    {
      "version": "2.0.0",
      "tasks": [
        {
          "label": "Build Tamagotchi Game",
          "type": "shell",
          "command": "gcc",
          "args": [
            "-fdiagnostics-color=always",
            "-g",
            "src/main.c",
            "src/graphics.c",
            "src/scene_manager.c",
            "src/menu_scene.c",
            "src/game_scene.c",
            "src/ui.c",
            "src/pet.c",
            "-o",
            "${workspaceFolder}/game",
            "-I", "${workspaceFolder}/include",
            "-lSDL2main",
            "-lSDL2",
            "-lSDL2_image",  
            "-lSDL2_gfx",
          ],
          "options": {
            "cwd": "${workspaceFolder}"
          },
          "problemMatcher": [
            "$gcc"
          ],
          "group": {
            "kind": "build",
            "isDefault": true
          }
        }
      ]
    }
</details>
