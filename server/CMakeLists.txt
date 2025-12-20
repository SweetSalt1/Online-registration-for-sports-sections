cmake_minimum_required(VERSION 3.10)
project(SportServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(
    /usr/include/mysql
    /usr/local/include
    /home/sweet-salt/mysql-connector-c++-9.5.0-linux-glibc2.28-aarch64/mysql-connector-c++-9.5.0-linux-glibc2.28-aarch64/include/jdbc
)

link_directories(
    /home/user/mysql-connector-c++-8.0.xx/lib64
)

set(SOURCES
    main.cpp
    server.cpp
    database.cpp
    session_manager.cpp
    auth_manager.cpp
)

add_executable(sport_server ${SOURCES})

target_link_libraries(sport_server
    PRIVATE
    Threads::Threads
    ssl
    crypto
    mysqlcppconn
    pthread
    mysqlcppconn
)
