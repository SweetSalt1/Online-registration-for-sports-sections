cmake_minimum_required(VERSION 3.18)
project(SportClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# качаем cpr + libcurl (должны установиться вместе)
FetchContent_Declare(cpr
	GIT_REPOSITORY https://github.com/libcpr/cpr.git
	GIT_TAG 1.14.2
)

FetchContent_MakeAvailable(cpr)

# качаем nlohmann_json
FetchContent_Declare(nlohmann
	URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)

FetchContent_MakeAvailable(nlohmann)

# закидываем в сборку все .cpp файлы (вручную)
add_executable(sport_client
	api.cpp
	json.cpp
	session.cpp
	ui.cpp
	main.cpp
)

target_link_libraries(sport_client
	PRIVATE
		cpr::cpr
		nlohmann_json::nlohmann_json
)
