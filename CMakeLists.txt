cmake_minimum_required(VERSION 3.16)
project(RaylibJoltDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. Raylib ---
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

# --- 2. Jolt Physics ---
# Opções para desativar testes e exemplos pesados
set(USE_SSE4_2 ON CACHE BOOL "Use SSE4.2" FORCE)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "Build Unit Tests" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "Build Hello World" FORCE)
set(TARGET_PERFORMANCE_TEST OFF CACHE BOOL "Build Performance Test" FORCE)
set(CROSS_PLATFORM_DETERMINISTIC OFF CACHE BOOL "Deterministic" FORCE)

FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
  GIT_TAG v5.0.0
  SOURCE_SUBDIR Build 
)
FetchContent_MakeAvailable(JoltPhysics)

# --- 3. Dear ImGui ---
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})

# --- 4. rlImGui ---
FetchContent_Declare(
  rlImGui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG main
)
FetchContent_MakeAvailable(rlImGui)

# --- 5. exe ---
add_executable(Robot_yuki 
    src/main.cpp 
    src/robot_arm.cpp 
    src/robot_communication.cpp
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp 
)

# Diretórios de Include
target_include_directories(Robot_yuki PUBLIC 
    src 
    ${raylib_SOURCE_DIR}/src
    ${joltphysics_SOURCE_DIR} 
    ${rlimgui_SOURCE_DIR}
)

# Linkagem
target_link_libraries(Robot_yuki PRIVATE 
    raylib 
    imgui
    Jolt 
)

# Copiar Assets
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})