@echo off
echo Rozpoczynam kompilacje...
g++ -Iinclude src/main.cpp src/core/Game.cpp src/input/Controller.cpp src/graphics/Renderer.cpp src/entities/Obstacle.cpp src/entities/ObstacleManager.cpp -o game.exe -lsetupapi -lhid

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Kompilacja przebiegla pomyslnie!
    echo Aby uruchomic gre, wpisz: game.exe
) else (
    echo.
    echo Wystapil blad podczas kompilacji. Upewnij sie, ze gra nie jest aktualnie wlaczona.
)
pause
