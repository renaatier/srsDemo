cmake_minimum_required(VERSION 3.20)
project(SRSDemoDaemon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
else()
    message(FATAL_ERROR "VCPKG_ROOT environment variable not set. Please set it to the path of your vcpkg installation.")
endif()

set(SOURCES
    srsDemoDaemon.cpp
    databaseManager.cpp
)

set(HEADERS
    databaseManager.h
)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(uwebsockets CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(tinyxml2 CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)

target_link_libraries(${PROJECT_NAME}
    PRIVATE uwebsockets::uwebsockets
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE tinyxml2::tinyxml2
    PRIVATE SQLite::SQLite3
)

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES) #Windows mathematics
elseif(APPLE)
    #Macintosh computer company...
    find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CORE_FOUNDATION})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
