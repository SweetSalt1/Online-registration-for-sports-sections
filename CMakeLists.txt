cmake_minimum_required(VERSION 3.24)
project(OnlineRegistrationForSportsSections)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем системные библиотеки
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(mysql-concpp REQUIRED)

include(FetchContent)

# Crow
message(STATUS "Загружаем Crow через FetchContent...")
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG        v1.2.1.2
)

# CPR
message(STATUS "Загружаем CPR через FetchContent...")
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.11.0
)

# Отключаем поиск libpsl для curl (входит в CPR)
set(CPR_USE_SYSTEM_LIB_PSL OFF)

# nlohmann/json
message(STATUS "Загружаем nlohmann/json через FetchContent...")
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

FetchContent_MakeAvailable(crow cpr nlohmann_json)

add_subdirectory(client)
add_subdirectory(server)
