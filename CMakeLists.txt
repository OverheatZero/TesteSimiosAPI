cmake_minimum_required(VERSION 3.22)
project(TesteSimiosAPI)

set(CMAKE_CXX_STANDARD 17)

add_executable(TesteSimiosAPI main.cpp)

include(FetchContent)
FetchContent_Declare(
        crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG master
)
FetchContent_MakeAvailable(crow)

target_link_libraries(TesteSimiosAPI PRIVATE Crow::Crow)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

target_link_libraries(TesteSimiosAPI PRIVATE pqxx pq)