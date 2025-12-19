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
