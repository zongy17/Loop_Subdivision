cmake_minimum_required(VERSION 3.16)

PROJECT (A4)

add_definitions(-DCMAKE_BUILD_TYPE=Release)

#头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
#库文件目录
link_directories(${PROJECT_SOURCE_DIR}/lib)

#项目文件夹下源文件main.cpp和glad.c
aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_ALL_SRC)
add_executable(A4 ${DIR_ALL_SRC})             # 生成可执行文件

target_link_libraries(A4 glfw3.dll)