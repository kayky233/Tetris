cmake_minimum_required(VERSION 3.26)
project(ccode C)

set(CMAKE_C_STANDARD 99)

# 设置SDL2的路径
set(SDL2_PATH "${CMAKE_SOURCE_DIR}/SDL2")
set(SDL_ttf_PATH "${CMAKE_SOURCE_DIR}/SDL2ttf")

# 包含SDL2的头文件目录
include_directories(${SDL2_PATH}/include)

# 链接SDL2的库文件目录
link_directories(${SDL2_PATH}/lib)

include_directories(${SDL_ttf_PATH}/include)
link_directories(${SDL_ttf_PATH}/lib)

add_executable(ccode main.c)
# 添加-municode到编译参数
target_compile_options(ccode PRIVATE -municode)
# 链接SDL2库到您的项目
target_link_libraries(ccode mingw32 SDL2main SDL2 SDL2_ttf)
