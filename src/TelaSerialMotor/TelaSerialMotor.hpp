#pragma once
#include <Base/Tela.hpp>
#include <boost/asio.hpp>
#include <raygui.h>

//0b1111111111111111111111111111 = 26bit, para velocidade

struct Controle
{
    
};

class TelaSerialMotor : public Tela
{
    public:
        TelaSerialMotor(CoordenadorTelas *coord) {this->m_coord = coord;}
        void Init() override;
        void HandleInput() override;
        void Update() override;
        void Draw() override;
        
        void Pause() override;
        void Resume() override;
        
        void OnExit() override;
        void OnEnter() override;
        
        void CleanUp() override;
    
    private:
        CoordenadorTelas *m_coord = nullptr;
};
