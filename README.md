#Simulador 


## Preparação do Ambiente

### Requisitos
- CMake()
- Conan

### Build Linux
  1. Crie uma pasta para a build:

  `mkdir build`

  2. Instale as bibliotecas necessarias do conan:

  `conan install . --output-folder=build --build=missing`

  3.Entre na paste da build:

  `cd build`
  
  5. Configure o projeto com CMake:

  `cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release`
  
  6. Compile o projeto:

  `cmake --build .`

### Build Windows
TODO