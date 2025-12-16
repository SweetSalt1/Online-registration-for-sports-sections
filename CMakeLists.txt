cmake_minimum_required(VERSION 3.24)
project(OnlineRegistrationForSportsSections)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Сначала ищем системные библиотеки
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(mysql-concpp REQUIRED)

# 2. Для Crow и CPR используем FetchContent
include(FetchContent)

# Crow: если не найден в системе, скачиваем
# Не используем find_package для Crow, так как он не устанавливает cmake-конфиг
message(STATUS "Загружаем Crow через FetchContent...")
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG        v1.2.1.2
)
FetchContent_MakeAvailable(crow)

# CPR: если не найден в системе, скачиваем
message(STATUS "Загружаем CPR через FetchContent...")
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG        1.11.0
)
# Отключаем поиск libpsl для curl (входит в CPR)
set(CPR_USE_SYSTEM_LIB_PSL OFF)
FetchContent_MakeAvailable(cpr)

add_subdirectory(client)
add_subdirectory(server)
