<<<<<<< HEAD
cmake_minimum_required(VERSION 3.10)
project(SportClient)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    api.cpp
    json.cpp
    ui.cpp
    session.cpp
)

add_executable(sport_client ${SOURCES})

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(cpr REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(/usr/local/include)
lin_directories(/usr/local/lib)
target_include_directories(sport_client
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sport_client
	PRIVATE
	${CPR_LIBRARIES}
	cpr
	nlohmann_json::nlohmann_json
	OpenSSL::SSL
	OpenSSL::Crypto
	Threads::Threads
)

=======
# cmake для клиента

# получаем исходные файлы
file(GLOB_RECURSE CLIENT_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

# получаем заголовочные файлы
file(GLOB_RECURSE CLIENT_HEADERS
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
	${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
)

add_executable(client_app ${CLIENT_SOURCES} ${CLIENT_HEADERS})

set_target_properties(client_app PROPERTIES
	OUTPUT_NAME "client"
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_include_directories(client_app PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# подключение библиотек
target_link_libraries(client_app PRIVATE libraries_all)
target_compile_features(client_app PRIVATE cxx_std_17)

if(MSVC)
	target_compile_options(client_app PRIVATE /W4 /WX)
else()
	target_compile_options(client_app PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(NOT CPR_FOUND)
	message(WARNING "Клиент требует CPR (он не найден)")
endif()
if(NOT JSON_FOUND)
	message(WARNING "Клиент требует JSON (он не найден)")
endif()
>>>>>>> be0b338162bd95eb542f2c5cd77c708fde8e5d2b
