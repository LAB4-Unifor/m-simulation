cmake_minimum_required(VERSION 3.15)
project(yuki)

if (UNIX)
    find_package(raylib 4.5 REQUIRED) # Requires at least version 4.5
    find_package(Boost 1.82)
else()
    
endif()

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/)
file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.h")

add_executable(${PROJECT_NAME} ${SIM_SRC})

target_link_libraries(${PROJECT_NAME} raylib  ${Boost_LIBRARIES} -pthread -fpic -lm -ldl -lrt)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif() 
