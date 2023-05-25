cmake_minimum_required(VERSION 3.15)
project(yuki)

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_CXX_STANDARD 17) #Requires C++ 17 standard

include(FetchContent)

if(UNIX)
    if(NOT raylib AND NOT Boost)
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        )

        #boost from conan
        find_package(Boost)

        FetchContent_MakeAvailable(raylib)

        execute_process(COMMAND git clone https://github.com/raysan5/raygui.git extern/raygui)

        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/src/)
        file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" 
        "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/build/extern/raygui/src/*.h")
        
        add_executable(${PROJECT_NAME} ${SIM_SRC})
        
        target_link_libraries(${PROJECT_NAME} raylib ${Boost_LIBRARIES} -pthread -fpic -lm -ldl -lrt)

    else()
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/)
        file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.hpp" 
        "${PROJECT_SOURCE_DIR}/extern/raygui/*.h")
        
        add_executable(${PROJECT_NAME} ${SIM_SRC})
        
        target_link_libraries(${PROJECT_NAME} raylib  ${Boost_LIBRARIES} -pthread -fpic -lm -ldl -lrt)

    endif()
else()
    if(NOT raylib AND NOT Boost)

        #TODO:Get raygui by fetch
        FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        )

        FetchContent_MakeAvailable(raylib)

        find_package(Boost)

        execute_process(COMMAND git clone https://github.com/raysan5/raygui.git extern/raygui)

        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/src/)
        file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" 
        "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/build/extern/raygui/src/*.h")
        
        add_executable(${PROJECT_NAME} ${SIM_SRC})
        
        target_link_libraries(${PROJECT_NAME} raylib ${Boost_LIBRARIES})
    endif()
endif()

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif() 
