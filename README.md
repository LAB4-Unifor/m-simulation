Se não tiver o raylib e o boost instalados:
1.Instale o package manager de c++ conan
2.conan . --output-folder=build --build=missing
3.mkdir build && cd build
4.cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
5.cmake --build .