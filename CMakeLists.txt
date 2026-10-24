cmake_minimum_required(VERSION 3.19)
project(server_for_messenger LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Network )

qt_standard_project_setup()

qt_add_executable(server_for_messenger
    main.cpp
    chat_server/chat_server.h
    chat_server/chat_server.cpp
)

target_link_libraries(server_for_messenger
    PRIVATE
        Qt::Core
        Qt6::Network
)

install(TARGETS server_for_messenger
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
