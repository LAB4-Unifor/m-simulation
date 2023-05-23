cmake_minimum_required(VERSION 3.15)
project(yuki)

set(CMAKE_C_STANDARD 11) # Requires C11 standard
set(CMAKE_CXX_STANDARD 17) #Requires C++ 17 standard

include(ExternalProject)

if(UNIX)
    if(raylib AND Boost)

        #Set folder for download
        set(EXTERNAL_INSTALL_LOCATION ${PROJECT_SOURCE_DIR}/extern/)

        #raylib
        ExternalProject_Add(raylib
        GIT_REPOSITORY    https://github.com/raysan5/raylib.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION})

        #raygui
        ExternalProject_Add(raygui
        GIT_REPOSITORY    https://github.com/raysan5/raygui.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION})

        #boost from conan
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup()

        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/)
        file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" 
        "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/extern/raygui/src/*.h")
        
        add_executable(${PROJECT_NAME} ${SIM_SRC})
        
        target_link_libraries(${PROJECT_NAME} raylib ${CONAN_LIBS} -pthread -fpic -lm -ldl -lrt)

    else()

        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/)
        file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/src/*.h")
        
        add_executable(${PROJECT_NAME} ${SIM_SRC})
        
        target_link_libraries(${PROJECT_NAME} raylib  ${Boost_LIBRARIES} -pthread -fpic -lm -ldl -lrt)

    endif()
else()
    if(raylib AND Boost)

        #Set folder for download
        set(EXTERNAL_INSTALL_LOCATION ${PROJECT_SOURCE_DIR}/extern)

        #raylib
        ExternalProject_Add(raylib
        GIT_REPOSITORY    https://github.com/raysan5/raylib.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION})

        #raygui
        ExternalProject_Add(raylib
        GIT_REPOSITORY    https://github.com/raysan5/raygui.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_INSTALL_LOCATION})

        #conan for boost
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup()

        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/)
        include_directories(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/extern/)
        file(GLOB_RECURSE SIM_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp" 
        "${PROJECT_SOURCE_DIR}/src/*.hpp" "${PROJECT_SOURCE_DIR}/extern/raygui/src/*.h")
        
        add_executable(${PROJECT_NAME} ${SIM_SRC})
        
        target_link_libraries(${PROJECT_NAME} raylib ${CONAN_LIBS})
    endif()
endif()

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif() 
