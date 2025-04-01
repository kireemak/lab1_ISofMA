cmake_minimum_required(VERSION 3.12)

project("FileAccessManager" 
    VERSION 1.0
    LANGUAGES CXX
    DESCRIPTION "Tool for packaging files with their access permissions"
)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(inc)

set(SOURCES
    src/main.cpp
    src/file_access.cpp
    src/archive_engine.cpp
    src/gui.cpp
)

find_package(libzip REQUIRED)
find_package(FLTK CONFIG REQUIRED fltk fltk_images fltk_gl)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    fltk
    fltk_images
    fltk_gl
    libzip::zip
)