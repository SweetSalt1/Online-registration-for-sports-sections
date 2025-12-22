cmake_minimum_required(VERSION 3.10)

project(Online-registration-for-sports-sections VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

option(BUILD_CLIENT "Build client application" ON)
option(BUILD_SERVER "Build server application" ON)
option(BUILD_BALANCER "Build balancer application" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINIRY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTOR ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(libraries)

if(BUILD_CLIENT)
	add_subdirectory(client)
endif()

if(BUILD_SERVER)
	add_subdirectory(server)
endif()

if(BUILD_BALANCER)
	add_subdirectory(balancer)
endif()
