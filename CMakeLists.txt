cmake_minimum_required(VERSION 3.10)

# Set the project name
project(YDIBDGTCH)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(source/win)
include_directories(source/lib/engine)

set(SOURCE_DIRS "source/win" "source/lib/engine")

set(SOURCES "")
set(HEADERS "")
foreach(DIR ${SOURCE_DIRS})
    file(GLOB_RECURSE DIR_SOURCES "${CMAKE_SOURCE_DIR}/${DIR}/*.cpp")
    file(GLOB_RECURSE DIR_HEADERS "${CMAKE_SOURCE_DIR}/${DIR}/*.h")
    file(GLOB_RECURSE DIR_HEADERS_HPP "${CMAKE_SOURCE_DIR}/${DIR}/*.hpp")

    # Append files to the overall list
    list(APPEND SOURCES ${DIR_SOURCES})
    list(APPEND HEADERS ${DIR_HEADERS})
    list(APPEND HEADERS ${DIR_HEADERS_HPP})

    # Create source groups based on the directory name
    get_filename_component(SUBGROUP_NAME ${DIR} NAME)
    source_group("Source Files\\${SUBGROUP_NAME}" FILES ${DIR_SOURCES} ${DIR_HEADERS} ${DIR_HEADERS_HPP})
endforeach()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})