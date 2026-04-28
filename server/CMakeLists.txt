cmake_minimum_required(VERSION 3.18)
project(sport_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDAD_REQUIRED ON)

include(FetchContent)

# crow
FetchContent_Declare(crow
	GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
	GIT_TAG v1.3.2
)

FetchContent_MakeAvailable(crow)

# многопоточность
find_package(Threads REQUIRED)

# чтобы не было 1000 зависимостей качаем mysql-connector через пакетный менеджер
# команда в терминале: sudo apt install libmysqlcppconn-dev libmysqlclient-dev
#find_package(mysql-concpp REQUIRED)

# Mysql Connector ищу вручную (он был установлен через терминал)
find_path(MYSQL_CPPCONN_INCLUDE_DIR
	NAMES mysql_driver.h
	PATHS /usr/include/mysql-cppconn/jdbc
)

find_library(MYSQL_CPPCONN_LIBRARY
	NAMES mysqlcppconn mysqlcppconn8
	PATHS /usr/lib/x86_64-linux-gnu
)

add_executable(sport_server
	server.cpp
	database.cpp
	session_manager.cpp
	auth_manager.cpp
	main.cpp
)

target_include_directories(sport_server PRIVATE ${MYSQL_CPPCONN_INCLUDE_DIR})
target_link_libraries(sport_server
	PRIVATE
	Threads::Threads
#	mysql::concpp-jdbc
	${MYSQL_CPPCONN_LIBRARY}
	Crow::Crow
)
