cmake_minimum_required(VERSION 3.10)

project(<<<<<Nom du projet>>>>>)

file(GLOB PROJ_SRC
    "sources/*.cpp"
    "includes/*.h"
)
if ((NOT DEFINED VCPKG_TARGET_TRIPLET) AND ("${CMAKE_CXX_COMPILER}" MATCHES  g\\+\\+\\.exe) AND (${CMAKE_SIZEOF_VOID_P} EQUAL 4))
    message("Configuiration pour MinGW 32")
    set(SFML_DIR "${CMAKE_SOURCE_DIR}/dependencies/SFML/lib/cmake/SFML" CACHE FILEPATH "The directory containing a CMake configuration file for SFML.")
endif()

add_executable(<<<<<Nom du projet>>>>> ${PROJ_SRC})
target_include_directories(<<<<<Nom du projet>>>>> PRIVATE ${CMAKE_SOURCE_DIR}/includes)

find_package(SFML COMPONENTS system window graphics REQUIRED)
target_link_libraries(<<<<<Nom du projet>>>>> PRIVATE     
        sfml-system
        sfml-main
        sfml-window
        sfml-graphics
    )
target_include_directories(<<<<<Nom du projet>>>>> PRIVATE ${SFML_INCLUDE_DIR})
