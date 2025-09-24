-- xmake.lua configuration
set_project("RobotSimulator")
set_version("1.0.0")
add_rules("mode.debug", "mode.release")

target("robot_sim")
    set_kind("binary")
    add_files("src/*.cpp")
    add_headerfiles("include/*.h")
    add_includedirs("include")
    add_packages("opengl", "sdl2", "glew", "glm", "assimp", "imgui")