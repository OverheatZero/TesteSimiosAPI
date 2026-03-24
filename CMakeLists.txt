cmake_minimum_required(VERSION 3.31)
project(TesteSimiosAPI)

set(CMAKE_CXX_STANDARD 17)

add_executable(TesteSimiosAPI main.cpp)
find_package(Crow)
target_link_libraries(TesteSimiosAPI PUBLIC Crow::Crow)
