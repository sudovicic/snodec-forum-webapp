cmake_minimum_required(VERSION 3.5)

project(webappForum LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
        snodec COMPONENTS http-server-express db-mariadb core-socket-stream-legacy net-in-stream-legacy
)

add_executable(main main.cpp utils.cpp utils.h dbsetup.cpp dbsetup.h)

target_link_libraries(main PRIVATE snodec::http-server-express snodec::db-mariadb snodec::core-socket-stream-legacy snodec::net-in-stream-legacy)
