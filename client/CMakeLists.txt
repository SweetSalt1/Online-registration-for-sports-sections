set(CLIENT_SOURCES
    api.cpp
    api.h
    json.cpp
    json.h
    main.cpp
    session.cpp
    session.h
    types.h
    ui.cpp
    ui.h
)

add_executable(client ${CLIENT_SOURCES})

target_link_libraries(client
    PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    cpr::cpr
    mysql::concpp
)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
