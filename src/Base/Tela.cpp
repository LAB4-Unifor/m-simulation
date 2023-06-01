#include <Base/Tela.hpp>
#include <raylib.h>

void CoordenadorTelas::PushTela(Tela* tela)
{
    if(!this->telas.empty())
    {
        this->telas.top()->Pause();
    }
    
    this->telas.push(tela);
    this->telas.top()->Init();
}

void CoordenadorTelas::ChangeTela(Tela* tela)
{
    if(!this->telas.empty())
    {
        PopTela();
    }
    
    PushTela(tela);
}

Tela *CoordenadorTelas::PeekTela()
{
    if(this->telas.empty()) {return nullptr;}
    return this->telas.top();
}

void CoordenadorTelas::HandleInput()
{
    PeekTela()->HandleInput();
}

void CoordenadorTelas::Update()
{
    PeekTela()->Update();
}

void CoordenadorTelas::Draw()
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    PeekTela()->Draw();
    EndDrawing();
}

void CoordenadorTelas::PopTela()
{
    delete this->telas.top();
    this->telas.pop();
}

CoordenadorTelas::~CoordenadorTelas()
{
    while(!this->telas.empty())
    {
        PopTela();
    }
}
