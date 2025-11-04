add_rules("mode.debug", "mode.release")

add_requires("libsdl2", "glew", "glm", "assimp")

target("robot_simulator")
    set_kind("binary")
    set_languages("c++17")
    
    -- Add source files
    add_files("src/*.cpp")
    
    -- Add ImGui and ImPlot source files
    add_files("vendor/imgui/*.cpp")
    add_files("vendor/implot/*.cpp")
    
    -- Add backend implementations
    add_files("vendor/imgui/backends/imgui_impl_sdl2.cpp")
    add_files("vendor/imgui/backends/imgui_impl_opengl3.cpp")
    
    add_headerfiles("include/*.h")
    add_headerfiles("vendor/imgui/*.h")
    add_headerfiles("vendor/implot/*.h")
    add_headerfiles("vendor/imgui/backends/imgui_impl_sdl2.h")
    add_headerfiles("vendor/imgui/backends/imgui_impl_opengl3.h")
    
    add_includedirs("include")
    add_includedirs("vendor/imgui")
    add_includedirs("vendor/implot")
    add_includedirs("vendor/imgui/backends")
    
    add_packages("libsdl2", "glew", "glm", "assimp")
    
    -- Copy shaders AND assets to build directory
    after_build(function (target)
        local targetdir = target:targetdir()
        print("Copying shaders to: " .. targetdir)
        os.cp("shaders", targetdir)
        print("Copying assets to: " .. targetdir)
        os.cp("assets", targetdir)
    end)
    
    -- Also copy on configuration to ensure they're available
    after_config(function (target)
        local targetdir = target:targetdir()
        os.cp("shaders", targetdir)
        os.cp("assets", targetdir)
    end)
    
    if is_plat("linux") then
        add_syslinks("dl", "pthread", "GL", "m")
    end
    
    if is_plat("windows") then
        add_syslinks("opengl32", "gdi32", "user32", "kernel32")
    end