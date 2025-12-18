set(SERVER_SOURCES
    Authentication.cpp
    Secretary.cpp
    Section.cpp
    Startserver.cpp
    Student.cpp
    WorkWithJson.cpp
)

add_executable(server ${SERVER_SOURCES})

target_link_libraries(server
    PRIVATE
    crow
    /usr/include/mysql-cppconn/jdbc
)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

