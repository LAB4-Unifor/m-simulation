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
set(USE_SSE4_2 ON CACHE BOOL "Use SSE4.2" FORCE)
set(TARGET_UNIT_TESTS OFF CACHE BOOL "Build Unit Tests" FORCE)
set(TARGET_HELLO_WORLD OFF CACHE BOOL "Build Hello World" FORCE)
set(CROSS_PLATFORM_DETERMINISTIC OFF CACHE BOOL "Deterministic" FORCE)

FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
  GIT_TAG v5.0.0
  SOURCE_SUBDIR Build
)
FetchContent_MakeAvailable(JoltPhysics)

# --- 3. Dear ImGui (Compilação Manual) ---
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

# --- 4. rlImGui (Compilação Manual - A CORREÇÃO ESTÁ AQUI) ---
FetchContent_Declare(
  rlImGui
  GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
  GIT_TAG main
)
# Apenas baixa os arquivos, não tenta configurar automaticamente
FetchContent_MakeAvailable(rlImGui) 

# Criamos a biblioteca nós mesmos para garantir que ela exista
add_library(rlImGui STATIC "${rlimgui_SOURCE_DIR}/rlImGui.cpp")

# rlImGui precisa conhecer os headers do ImGui e Raylib
target_link_libraries(rlImGui PUBLIC raylib imgui)
target_include_directories(rlImGui PUBLIC "${rlimgui_SOURCE_DIR}")

# --- 5. Executable ---
add_executable(Yuki 
    src/main.cpp 
    src/robot_model.cpp       
    src/robot_controller.cpp  
    src/gui_manager.cpp       
    src/robot_communication.cpp
)

target_include_directories(Yuki PUBLIC 
    src 
    ${raylib_SOURCE_DIR}/src
    ${imgui_SOURCE_DIR}
    ${rlimgui_SOURCE_DIR}
    ${joltphysics_SOURCE_DIR}
)

target_compile_definitions(Yuki PRIVATE JPH_PROFILE_ENABLED JPH_DEBUG_RENDERER)

target_link_libraries(Yuki PRIVATE 
    rlImGui 
    imgui
    raylib 
    Jolt
)

# Copia assets
add_custom_command(TARGET Yuki POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets..."
)