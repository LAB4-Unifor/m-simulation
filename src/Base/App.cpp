#include <Base/App.hpp>
#include <raylib.h>

void App::Setup()
{
    InitWindow(800, 450, "Test window");
    SetTargetFPS(60);
}

void App::Input()
{
}

void App::Update()
{
}

void App::Draw()
{
    BeginDrawing();
        ClearBackground(GRAY);
    EndDrawing();
}

void App::Run()
{
    Setup();
    while(!WindowShouldClose())
    {
        Input();
        Update();
        Draw();
    }
    CloseWindow();
}
