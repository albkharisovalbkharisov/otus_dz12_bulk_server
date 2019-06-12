cmake_minimum_required(VERSION 3.5)

project(bulk_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)
find_package(Boost)
#find_package(Boost COMPONENTS system REQUIRED)
#find_package(Boost COMPONENTS asio REQUIRED)

#set(BOOST_LIBRARYDIR "/usr/lib/x86_64-linux-gnu")
#
#set(Boost_USE_STATIC_LIBS ON) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME ON) 
#find_package(Boost COMPONENTS system REQUIRED) 
#

add_executable(bulk_server bulk_server.cpp)

set_target_properties(bulk_server PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-ggdb"
    # COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-fsanitize=thread"
)

target_link_libraries(bulk_server
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)


install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT albkharisov@gmail.com)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

