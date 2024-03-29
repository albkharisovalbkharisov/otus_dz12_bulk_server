cmake_minimum_required(VERSION 3.9)
project(async VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER} DESCRIPTION "async library, performs asynchronius access to bulkmt")
set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)

add_library(async SHARED async.cpp bulkmt.cpp async.h)
add_library(connection SHARED connection.cpp connection.h)
add_executable(cli_server server.cpp)

target_link_libraries(connection
    async
    ${CMAKE_THREAD_LIBS_INIT}
    ${Boost_LIBRARIES}
    )

target_link_libraries(cli_server
    connection
    ${CMAKE_THREAD_LIBS_INIT}
    )

set_target_properties(connection PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
#    COMPILE_DEFINITIONS BOOST_ASIO_ENABLE_HANDLER_TRACKING
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
    VERSION ${PROJECT_VERSION}
    SOVERSION $ENV{TRAVIS_BUILD_NUMBER}
    PUBLIC_HEADER async.h
)

set_target_properties(async PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_DEFINITIONS BOOST_ASIO_ENABLE_HANDLER_TRACKING
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
    VERSION ${PROJECT_VERSION}
    SOVERSION $ENV{TRAVIS_BUILD_NUMBER}
    PUBLIC_HEADER async.h
)

include(GNUInstallDirs)
install(TARGETS async 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCVLUDEDIR})

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT albkharisov@gmail.com)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

