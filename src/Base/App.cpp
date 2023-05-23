#include "App.hpp"
#include <raylib.h>

App::App(int wWidth, int wHeight, std::string wName, Tela *tela)
{
    Setup(wWidth, wHeight, wName);
    coordenador.ChangeTela(tela);
}

void App::Setup(int wWidth, int wHeight, std::string wName)
{
    InitWindow(wWidth, wHeight, wName.c_str());
    SetTargetFPS(60);
}

void App::Run()
{
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
