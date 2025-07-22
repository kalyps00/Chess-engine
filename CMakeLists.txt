# Minimalna wersja CMake - sprawdza czy system ma wystarczająco nową wersję
cmake_minimum_required(VERSION 3.16)

# Nazwa projektu - tworzy zmienną PROJECT_NAME z wartością "ChessEngine"
project(ChessEngine VERSION 1.0.0 LANGUAGES CXX)

# Standard C++ - wymusza użycie C++17 (nowoczesne funkcje języka)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Flagi kompilacji - włącza wszystkie ostrzeżenia i informacje debug
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Automatycznie wykryj tryb budowania jeśli nie podano
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Ścieżki do katalogów z plikami źródłowymi
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets")

# Lista wszystkich plików źródłowych (.cpp)
set(SOURCES
    ${SRC_DIR}/main.cpp
    # Model - logika gry
    # View - interfejs graficzny
    # Controller - kontrola gry
)

# Lista plików nagłówkowych (.h) - dla IDE i IntelliSense
set(HEADERS
    # Model headers
    # View headers  
    # Controller headers
)

# Tworzenie pliku wykonywalnego z podanych źródeł
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Dodanie katalogów z nagłówkami do ścieżki include
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SRC_DIR}
    ${SRC_DIR}/model
    ${SRC_DIR}/view  
    ${SRC_DIR}/controller
)

# ========== SFML CONFIGURATION ==========

# Wyszukiwanie biblioteki SFML w systemie
find_package(PkgConfig REQUIRED)
pkg_check_modules(SFML REQUIRED sfml-all>=2.5)

# Jeśli SFML znalezione przez pkg-config
if(SFML_FOUND)
    message(STATUS "SFML found via pkg-config")
    target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PRIVATE ${SFML_INCLUDE_DIRS})
    target_compile_options(${PROJECT_NAME} PRIVATE ${SFML_CFLAGS_OTHER})
else()
    # Fallback - szukanie przez Find modules CMake
    find_package(sfml-graphics REQUIRED)
    find_package(sfml-window REQUIRED) 
    find_package(sfml-system REQUIRED)
    find_package(sfml-audio REQUIRED)
    
    # Linkowanie modułów SFML
    target_link_libraries(${PROJECT_NAME} 
        sfml-graphics 
        sfml-window 
        sfml-system 
        sfml-audio
    )
endif()

# Kopiowanie assetów do folderu build (opcjonalne)
file(COPY ${ASSETS_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# Custom target dla czyszczenia buildu
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
)