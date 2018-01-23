cmake_minimum_required(VERSION 3.0)
project(wielomiany C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Nie chcemy oglÄ…daÄ‡ komunikatÃ³w o procentowym postÄ™pie kompilacji.
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

# Chcemy zobaczyÄ‡ polecenia wywoÅ‚ane przez make.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Ustawiamy wspÃ³lne opcje kompilowania dla wszystkich wariantÃ³w projektu.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
# DomyÅ›lne opcje dla wariantÃ³w Release i Debug sÄ… sensowne.
# JeÅ›li to konieczne, ustawiamy tu inne.
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
# set(CMAKE_C_FLAGS_DEBUG "-g")

# Wskazujemy pliki ÅºrÃ³dÅ‚owe.
set(SOURCE_FILES
    src/poly.c
    src/poly.h
	src/calc_poly.c
)

# Wskazujemy plik wykonywalny.
add_executable(calc_poly ${SOURCE_FILES})

# Dodajemy obsÅ‚ugÄ™ Doxygena: sprawdzamy, czy jest zainstalowany i jeÅ›li tak to:
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Wskazujemy lokalizacjÄ… pliku konfiguracyjnego i podajemy jego docelowÄ… lokalizacjÄ™ w folderze, gdzie nastÄ™puje kompilacja.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Dodajemy cel doc: uÅ¼ycie make doc spowoduje wywoÅ‚anie doxygena z odpowiednim plikiem konfiguracyjnym w folderze kompilacji.
    # Na wyjÅ›cie zostanie wypisany odpowiedni komentarz.
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)
