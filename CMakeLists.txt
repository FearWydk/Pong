cmake_minimum_required(VERSION 3.10)
project(Pong CXX)

add_definitions(-DSDL_MAIN_USE_CALLBACKS)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL3 CONFIG REQUIRED)
find_package(SDL3_ttf CONFIG REQUIRED)

add_executable(Pong main.cpp Game.cpp)
 
target_link_libraries(Pong PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)
 
add_custom_command(TARGET Pong POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
           $<TARGET_RUNTIME_DLLS:Pong>
           $<TARGET_FILE_DIR:Pong>
    COMMAND_EXPAND_LISTS
)
