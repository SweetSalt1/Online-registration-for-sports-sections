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

