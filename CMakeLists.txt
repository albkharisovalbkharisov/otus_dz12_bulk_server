cmake_minimum_required(VERSION 3.5)

project(bulk_server VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

set(CMAKE_CXX_STANDARD 14)

find_package(Threads)

add_executable(bulk_server bulk_server.cpp)

set_target_properties(bulk_server PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-ggdb"
    # COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra;-fsanitize=thread"
)

target_link_librariesi(bulk_server
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

