cmake_minimum_required(VERSION 3.15)
project(PngTuberApp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "C:/SFML/SFML-2.6.1/lib/cmake/SFML")

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

add_executable(PngTuberApp main.cpp)

target_link_libraries(PngTuberApp PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

