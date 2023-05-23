#pragma once
#include <string>
#include <raylib.h>
#include <Base/Tela.hpp>

//TODO:ler um .INI para configurar logo de cara
class App
{
    public:
        App(int wWidth, int wHeight, std::string wName, Tela *tela);
        
        //Loop
        void Run();
        
    private:
        CoordenadorTelas coordenador;
        
        void Setup(int wWidth, int wHeight, std::string wName);
};
