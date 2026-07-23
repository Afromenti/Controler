cmake_minimum_required(VERSION 3.10)
project(ControlerGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Katalogi z nagłówkami
include_directories(
    include
    include/core
    include/input
    include/graphics
    include/entities
)

# Wszystkie pliki źródłowe projektu pogrupowane w moduły
set(SOURCES
    src/main.cpp
    src/core/Game.cpp
    src/input/Controller.cpp
    src/graphics/Renderer.cpp
    src/entities/Obstacle.cpp
    src/entities/ObstacleManager.cpp
)

# Utworzenie pliku wykonywalnego
add_executable(game ${SOURCES})

# Przeniesienie gotowego pliku game.exe do głównego katalogu
set_target_properties(game PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Dołączenie niezbędnych bibliotek systemowych Windows
target_link_libraries(game setupapi hid)
