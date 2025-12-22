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
file(GLOB_RECURSE SERVER_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

file(GLOB_RECURSE SERVER_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_executable(server_app
	${SERVER_SOURCES}
	${SERVER_HEADERS}
)

set_target_properties(server_app PROPERTIES
	OUTPUT_NAME "server"
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(server_app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(server_app PRIVATE
	linbraries_all
)

target_compile_features(server_app PRIVATE cxx_std_17)

if(MSVC)
	target_compile_options(server_app PRIVATE /W4 /WX)
else()
	target_compile_options(server_app PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(NOT CROW_FOUND)
	message(WARNING "Сервер требует Crow (он не найден)")
endif()

#if(NOT MYSQL_CONNECTOR_FOUND)
#	message(WARNING "Сервер требует MySQL Connector (он не найден)")
#endif()

#if(NOT MySQL_FOUND)
#	message(WARNING "Сервер требует MySQL client (он не найден)")
#endif()
