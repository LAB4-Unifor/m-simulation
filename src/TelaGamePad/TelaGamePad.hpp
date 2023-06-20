#pragma once
#include <string>
#include <cstdint>
#include <raylib.h>
#include <raygui.h>
#include <boost/asio.hpp>
#include <Base/Tela.hpp>
#include <Base/Communication/Serial.hpp>

struct ESPPacket
{
    const uint32_t dividendConst = 0b00000011111111111111111111111111;

    //Bits dos motores
    uint32_t motores[6] = {
        0b00010000000000000000000000000000,
        0b00100000000000000000000000000000,
        0b00110000000000000000000000000000,
        0b01000000000000000000000000000000,
        0b01010000000000000000000000000000,
        0b01100000000000000000000000000000
    };

    //bits 5 e 6
    bool stopON = false;
    bool reverseON = false;

    uint32_t waveDivider = 0;
    uint32_t payload = 0;
    uint8_t selectedMotor = 5;//Ponteiro para n sair do array
    
    uint32_t l = 0;
    uint32_t r  = 0;
};

struct PGamePad
{
    bool isAvaible = false;

    bool dpadUp = false;
    bool dpadDown = false;
    bool dpadLeft = false;
    bool dpadRight = false;

    bool btn1 = false;
    bool btn2 = false;
    bool btn3 = false;
    bool btn4 = false;
    bool btn5 = false;
    bool btn6 = false;
    bool l_bumper = false;
    bool r_bumper = false;

    float r_triggerAxis = {0};
    float l_triggerAxis = {0};

    bool Init();
    void Update();
};

class TelaGamePad : public Tela
{
public:
    void Init() override;
    void HandleInput() override;
    void Update() override;
    void Draw() override;

    void Pause() override;
    void Resume() override;

    void OnEnter() override;
    void OnExit() override;

    void CleanUp() override;

    void UpdateESPPacket();
    
    ~TelaGamePad()
    {
        CleanUp();
    }

private:
    ESPPacket m_packetHeader;
    PGamePad m_gamepad;
    Serial serial;
    uint32_t m_lTratado = 0;
    uint32_t m_rTratado = 0;
    uint32_t m_frequencia = 0;
    int m_motorIdx = 0;
    int motorIdxAntigo = 0;
    bool m_temp_start_motor = 1;
    std::string m_controllerConnectMessage = {0};
};
