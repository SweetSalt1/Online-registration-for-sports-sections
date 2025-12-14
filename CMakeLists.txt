cmake_minimum_required(VERSION 3.16)
project(MyDistributedApp VERSION 1.0.0 LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройка путей для заголовочных файлов
include_directories(${CMAKE_SOURCE_DIR})

# Поиск пакетов и библиотек
# Поиск Qt6
find_package(Qt6 COMPONENTS Core Network REQUIRED)

# Поиск MySQL Connector/C++
find_package(MySQL REQUIRED)

# Поиск Crow
find_path(CROW_INCLUDE_DIR crow.h
    PATHS /usr/local/include /usr/include
    PATH_SUFFIXES crow
)

if(NOT CROW_INCLUDE_DIR)
    message(WARNING "Crow not found. Consider using FetchContent or install it.")
endif()

# Поиск CPR
include(FetchContent)
FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

# Настройка для MySQL
if(MySQL_FOUND)
    include_directories(${MYSQL_INCLUDE_DIR})
    link_directories(${MYSQL_LIB_DIR})
endif()

# Создание исполняемых файлов

# Сервер
add_executable(server server2.cpp)
target_include_directories(server PRIVATE
    ${CROW_INCLUDE_DIR}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Network_INCLUDE_DIRS}
)
target_link_libraries(server PRIVATE
    ${MYSQL_LIBRARIES}
    Qt6::Core
    Qt6::Network
)
if(CROW_INCLUDE_DIR)
    target_compile_definitions(server PRIVATE -DHAVE_CROW)
endif()

# Балансировщик
add_executable(balancer balancer.cpp)
target_include_directories(balancer PRIVATE
    ${CROW_INCLUDE_DIR}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Network_INCLUDE_DIRS}
)
target_link_libraries(balancer PRIVATE
    cpr::cpr
    Qt6::Core
    Qt6::Network
    ${CMAKE_THREAD_LIBS_INIT}  # Для многопоточности CPR
)
if(CROW_INCLUDE_DIR)
    target_compile_definitions(balancer PRIVATE -DHAVE_CROW)
endif()

# Копирование заголовочного файла клиента
configure_file(client_a.h ${CMAKE_CURRENT_BINARY_DIR}/client_a.h COPYONLY)

# Дополнительные настройки
if(WIN32)
    # Настройки для Windows
    add_definitions(-D_WIN32_WINNT=0x0A00)
elseif(UNIX)
    # Настройки для Linux
    find_package(Threads REQUIRED)
    target_link_libraries(server PRIVATE Threads::Threads)
    target_link_libraries(balancer PRIVATE Threads::Threads)
endif()

# Установка
install(TARGETS server balancer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)
