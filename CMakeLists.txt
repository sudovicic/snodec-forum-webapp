cmake_minimum_required(VERSION 3.5)

project(forum-webapp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(MARIADB mariadb)

if(MARIADB_FOUND)

    find_package(
        snodec COMPONENTS express core-legacy net-in db-mariadb
    )

    add_executable(forum-webapp main.cpp)

    target_link_libraries(forum-webapp PRIVATE snodec::express snodec::core-legacy snodec::net-in snodec::db-mariadb)

    target_include_directories(
        forum-webapp
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${MARIADB_INCLUDE_DIRS}
               "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
               "$<INSTALL_INTERFACE:include/snode.c>"
    )

    install(TARGETS forum-webapp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

endif(MARIADB_FOUND)
