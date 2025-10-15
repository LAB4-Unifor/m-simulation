add_rules("mode.debug", "mode.release")

add_requires("libsdl2")
add_requires("glew")
add_requires("glm")
add_requires("assimp")
add_requires("imgui v1.89.9-docking", {configs = {sdl2 = true, opengl3 = true}})
add_requires("implot v0.16")  -- Try forcing a specific version or newer

target("robot_simulator")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("include/*.h")
    add_includedirs("include")
    add_packages("libsdl2", "glew", "glm", "assimp", "imgui", "implot")