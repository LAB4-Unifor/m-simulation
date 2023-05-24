#Simulador 


## Preparação do Ambiente

###Instalações:
- Instale o Raylib 
- Instale o Boost
- Instale o package manager de c++ conan

###Build
  2.mkdir build
  3.conan . --output-folder=build --build=missing
  4. cd build
  5.cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
  6.cmake --build .
