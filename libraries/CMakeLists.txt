# Cmake для сторонних библиотек

# Настройка путей

set(LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR})		# {CMAKE_CURRENT_SOURCE_DIR - libraries

# CPR
if(EXISTS ${LIBRARIES_DIR}/cpr/CMakeLists.txt)
	add_subdirectory(${LIBRARIES_DIR}/cpr)
	set(CPR_FOUND TRUE)
else()
	message(WARNING "CPR не найден в ${LIBRARIES_DIR}/cpr")
	set(CPR_FOUND FALSE)
endif()

# JSON
if(EXISTS ${LIBRARIES_DIR}/json/include/nlohmann/json.hpp)
	add_library(nlohmann_json INTERFACE)
	target_include_directories(nlohmann_json INTERFACE ${LIBRARIES_DIR}/json/include)
	set(JSON_FOUND TRUE)
else()
	message(WARNING "nlohmann/json не найден в ${LIBRARIES_DIR}/json/include")
	set(JSON_FOUND FALSE)
endif()

# MySQL Connector
find_package(mysql-concpp REQUIRED)
#set(MYSQL_CONNECTOR_DIR ${LIBRARIES_DIR}/mysql-connector-c++-9.5.0-src)

#if(EXISTS ${MYSQL_CONNECTOR_DIR}/include/jdbc/mysql/jdbc.h)
	# создание интерфейсной цели
#	add_library(mysql_connector_cpp INTERFACE)

#	target_include_directories(mysql_connector_cpp INTERFACE ${MYSQL_CONNECTOR_DIR}/include ${MYSQL_CONNECTOR_DIR}/include/jdbc)

#	if(WIN32)
#		target_link_libraries(mysql_connector_cpp INTERFACE ${MYSQL_CONNECTOR_DIR}/lib/mysqlcppconn.lib)
#	else()
#		find_library(MYSQLCPPCONN_LIB
#			NAMES mysqlcppconn libmysqlcppconn
#			PATH ${MYSQL_CONNECTOR_DIR}/lib64 ${MYSQL_CONNECTOR_DIR}/lib /usr/lib /usr/local/lib
#			NO_DEFAULT_PATH
#			)
#		if(MYSQLCPPCONN_LIB)
#			target_link_libraries(mysql_connector_cpp INTERFACE ${MYSQLCPPCONN_LIB})
#		else()
#			message(WARNING "MySQL Connector не найдена.")
#		endif()
#	endif()
#
#	set(MYSQL_CONNECTOR_FOUND TRUE)
#else()
#	message(WARNING "MySQL Connector не найдена в ${MYSQL_CONNECTOR_DIR}")
#	set(MYSQL_CONNECTOR_FOUND FALSE)
#endif()

# CROW

add_library(crow INTERFACE)

if(EXISTS ${LIBRARIES_DIR}/crow/include/crow_all.h)
	target_include_directories(crow INTERFACE ${LIBRARIES_DIR}/crow/include)

	target_compile_definitions(crow INTERFACE CROW_MAIN)
	set(CROW_FOUND TRUE)
	message(STATUS "Найден crow_all.h")
elseif(EXISTS ${LIBRARIES_DIR}/crow/include/crow.h)
	target_include_directories(crow INTERFACE ${LIBRARIES_DIR}/crow/include)
	set(CROW_FOUND TRUE)
	message(STATUS "Найден crow.h")
else()
	message(WARNING "Crow не найден")
	set(CROW_FOUND FALSE)
endif()

# Системные библиотеки
# BOOST для crow
find_package(Boost 1.66 QUIET COMPONENTS system thread)
if(Boost_FOUND)
	message(STATUS "Boost найден: ${Boost_VERSION}")
else()
	message(WARNING "Boost не найден => CROW не будет работать.")
endif()

#libcurl для CPR
find_package(CURL QUIET)
if(CURL_FOUND)
	message(STATUS "libcurl найден")
else()
	message(WARNING "libcurl не найден => CPR не будет работать.")
endif()

# MySQL Client для MySQL Connector
find_package(MySQL QUIET)
if(MySQL_FOUND)
	message(STATUS "MySQL Client найден")
else()
	# вручную
	find_path(MYSQL_INCLUDE_DIR mysql.h
		PATHS /usr/include/mysql
			/usr/local/include/mysql
	)
	find_library(MYSQL_LIBRARY NAMES mysqlclient
		PATHS /usr/lib
			/usr/local/lib
	)

	if(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
		set(MySQL_FOUND TRUE)
		add_library(mysql_client UNKNOWN IMPORTED)
		set_target_properties(mysql_client PROPERTIES
			IMPORTED_LOCATION ${MYSQL_LIBRARY}
			INTERFACE_INCLUDE_DIRECTORIES ${MYSQL_INCLUDE_DIR}
		)
		message(STATUS "MySQL client найден вручную.")
	else()
		message(WARNING "MySQL Client не найден => NySQL Connector не будет работать.")
	endif()
endif()

# Threads для std::mutex
find_package(Threads REQUIRED)


# Создание целей для использования
add_library(libraries_all INTERFACE)

if(CPR_FOUND)
	target_link_libraries(libraries_all INTERFACE cpr::cpr)
endif()
if(JSON_FOUND)
	target_link_libraries(libraries_all INTERFACE nlohmann_json)
endif()
if(CROW_FOUND)
	target_link_libraries(libraries_all INTERFACE crow)
endif()
target_link_libraries(libraries_all INTERFACE mysql::concpp)
if(Boost_FOUND)
	target_link_libraries(libraries_all INTERFACE Boost::system Boost::thread)
endif()
if(CURL_FOUND)
	target_link_libraries(libraries_all INTERFACE CURL::libcurl)
endif()
if(Threads_FOUND)
	target_link_libraries(libraries_all INTERFACE Threads::Threads)
endif()
if(MySQL_FOUND)
	target_link_libraries(libraries_all INTERFACE mysql_client)
endif()

find_package(OpenSSL REQUIRED)

set(CPR_FOUND ${CPR_FOUND} PARENT_SCOPE)
set(JSON_FOUND ${JSON_FOUND} PARENT_SCOPE)
set(MYSQL_CONNECTOR_FOUND ${MYSQL_CONNECTOR_FOUND} PARENT_SCOPE)
set(CROW_FOUND ${CROW_FOUND} PARENT_SCOPE)
