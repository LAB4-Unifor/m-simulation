{
    depfiles_format = "gcc",
    depfiles = "performance_monitor.o: src/performance_monitor.cpp  include/performance_monitor.h\
",
    values = {
        "/usr/bin/g++",
        {
            "-m64",
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-Iinclude",
            "-D_GNU_SOURCE=1",
            "-D_REENTRANT",
            "-DGLEW_NO_GLU",
            "-isystem",
            "/usr/include/SDL2",
            "-isystem",
            "/home/alexandre/.xmake/packages/g/glew/2.2.0/2ac0c6940a2140f7b6506fcd8240f406/include",
            "-isystem",
            "/home/alexandre/.xmake/packages/g/glm/1.0.1/1781f3ac6d8141628505a9ae557cd017/include",
            "-isystem",
            "/home/alexandre/.xmake/packages/a/assimp/v6.0.2/286073ee6ddc428ab65174ef0fabe045/include",
            "-isystem",
            "/usr/include/minizip",
            "-isystem",
            "/home/alexandre/.xmake/packages/i/imgui/v1.92.1/3ed829247db04c62b786163b4c20f6c8/include",
            "-isystem",
            "/home/alexandre/.xmake/packages/i/imgui/v1.92.1/3ed829247db04c62b786163b4c20f6c8/include/imgui",
            "-isystem",
            "/home/alexandre/.xmake/packages/i/imgui/v1.92.1/3ed829247db04c62b786163b4c20f6c8/include/backends",
            "-isystem",
            "/home/alexandre/.xmake/packages/i/imgui/v1.92.1/3ed829247db04c62b786163b4c20f6c8/include/misc/cpp",
            "-DNDEBUG"
        }
    },
    files = {
        "src/performance_monitor.cpp"
    }
}