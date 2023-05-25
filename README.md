Se não tiver o raylib e o boost instalados:
1.Instale o package manager de c++ conan
2.mkdir build
3.conan . --output-folder=build --build=missing
4.cd build
5.cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
6.cmake --build .