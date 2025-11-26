cmake_minimum_required(VERSION 3.16)
project(RaylibJoltDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Raylib
FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG master # Use the master branch
)
FetchContent_MakeAvailable(raylib)


# Fetch JoltPhysics
FetchContent_Declare(
  JoltPhysics
  GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
  GIT_TAG v4.0.2 # Use a more recent version
  SOURCE_SUBDIR Build
)
FetchContent_MakeAvailable(JoltPhysics)


add_executable(raylib-jolt-demo src/main.cpp)

target_link_libraries(raylib-jolt-demo PRIVATE raylib Jolt)
