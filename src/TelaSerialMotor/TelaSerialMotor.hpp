#pragma once
#include <cstring>
#include <Base/Tela.hpp>
#include <boost/asio.hpp>
#include <raygui.h>

//0b1111111111111111111111111111 = 26bit, para velocidade

//TODO: Vejam no extern os exemplos do raygui

//Struct para o estado dos widgets da tela
struct SerialGUILayout
{
    bool TextBox001EditMode;
    char TextBox001Text[128];//mudar para string
    bool CheckBoxEx004Checked;
    bool CheckBoxEx006Checked;
    bool CheckBoxEx007Checked;
    bool Toggle008Active;
    bool Toggle009Active;
    bool Button010Pressed;
    bool TextmultiBox011EditMode;
    char TextmultiBox011Text[128];//mudar para string
    bool CheckBoxEx009Checked;
    bool CheckBoxEx010Checked;
    bool CheckBoxEx011Checked;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

    void InitGUI();
    void DrawGUI();
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
