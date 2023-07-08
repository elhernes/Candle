# for homebrew:
# CMAKE_PREFIX_PATH=/usr/local/Cellar/qt@5/5.15.8_3/ cmake -B n -G Ninja

cmake_minimum_required (VERSION 3.24)

project(rpn-lang)

add_subdirectory(tests)
add_subdirectory(ui)
