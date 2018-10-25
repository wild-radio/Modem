cmake_minimum_required(VERSION 3.2)

project(robot36_encode)
project(robot36_decode)
find_package(SDL REQUIRED)

include_directories(${SDL_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)


add_executable(robot36_encode encode.c mmap_file.c pcm.c wav.c alsa.c yuv.c img.c ppm.c sdl.c)
add_executable(robot36_decode decode.c mmap_file.c pcm.c wav.c alsa.c window.c ddc.c buffer.c yuv.c img.c ppm.c sdl.c)

target_link_libraries(robot36_encode -lm -lasound ${SDL_LIBRARY})
target_link_libraries(robot36_decode -lm -lasound ${SDL_LIBRARY})