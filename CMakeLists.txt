cmake_minimum_required(VERSION 3.10)

project(Online-registration-for-sports-sections VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDART 17)
set(CMAKE_CXX_STANDART_REQUIRED ON)

# Переключатели для частей программы
option(BUILD_CLIENT "Build client application" ON)
option(BUILD_SERVER "Build server application" ON)
option(BUILD_BALANCER "Build balancer application" OFF)

# Где появятся исполняемые файлы
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINIRY_DIR}/bin)
# Где будут динамические библиотеки
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# Где будут статические библиотеки
set(CMAKE_ARCHIVE_OUTPUT_DIRECTOR ${CMAKE_BINARY_DIR}/lib)

# Собираем библиотеки
add_subdirectory(libraries)

# Собираем все осальное
if(BUILD_CLIENT)
	add_subdirectory(client)
endif()

if(BUILD_SERVER)
	add_subdirectory(server)
endif()

if(BUILD_BALANCER)
	add_subdirectory(balancer)
endif()
