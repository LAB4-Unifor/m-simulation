#include "App.hpp"
#include <raylib.h>

App::App(int wWidth, int wHeight, std::string wName)
{
    Setup(wWidth, wHeight, wName);
    
}

void App::Setup(int wWidth, int wHeight, std::string wName)
{
    InitWindow(wWidth, wHeight, wName.c_str());
    SetTargetFPS(60);
}

void App::Run(Tela *tela)
{
    coordenador.ChangeTela(tela);
    while(coordenador.ShouldQuit() == false)
    {
        coordenador.m_shouldQuit = WindowShouldClose();
        if(coordenador.ShouldQuit() != false)//So that the program doesn´t crash
        {
            break;
        }
        
        coordenador.HandleInput();
        coordenador.Update();
        coordenador.Draw();
    }
    
    CloseWindow();
}
