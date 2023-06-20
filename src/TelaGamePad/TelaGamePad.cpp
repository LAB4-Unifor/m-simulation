#include <iostream>
#include <bitset>
#include <TelaGamePad/TelaGamePad.hpp>


bool PGamePad::Init()
{
    isAvaible = IsGamepadAvailable(0);
    return isAvaible;
}

void PGamePad::Update()
{
    l_triggerAxis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER);
    r_triggerAxis = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER);
    l_bumper = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1);
    r_bumper = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1);
}

void TelaGamePad::Init()
{
    std::cout << m_packetHeader.dividendConst << std::endl;
    if(m_gamepad.Init())
    {
        m_controllerConnectMessage = "Controller conected!";
    }
    else
    {
        m_controllerConnectMessage = "Controller not conected!";
    }
}

void TelaGamePad::UpdateESPPacket()
{
    m_packetHeader.r =  m_rTratado;//+ 30;//TODO:trocar esse 30 por um valor variavel(30+)
    m_packetHeader.l = m_lTratado;
    
    uint32_t valor_min = 25;//TODO:input do usuario
    uint32_t valor_max = 55;//TODO:input do usuario
    uint32_t max = valor_max - valor_min;
    
    if(m_packetHeader.l == 0 || m_packetHeader.r == 0)
    {
        m_packetHeader.stopON = true;
        m_packetHeader.reverseON = false;
    }
    
    if(m_packetHeader.l > 0)
    {
        if(m_packetHeader.l  < 25)
        {
            m_packetHeader.l = 15;
        }
        else if(m_packetHeader.l  >= 25 && m_packetHeader.l  < 50)
        {
            m_packetHeader.l  = 25;
        }
        else if(m_packetHeader.l  >= 50 && m_packetHeader.l  < 75)
        {
            m_packetHeader.l  = 50;
        }
        else if(m_packetHeader.l  >= 75 && m_packetHeader.l  < 100)
        {
            m_packetHeader.l  = 75;
        }
        m_packetHeader.stopON = false;
        m_packetHeader.reverseON = true;
    }
    
    if(m_packetHeader.r > 0)
    {
        if(m_packetHeader.r < 25)
        {
            m_packetHeader.r = 15;
        }
        if(m_packetHeader.r >= 25 && m_packetHeader.r < 50)
        {
            m_packetHeader.r = 25;
        }
        else if(m_packetHeader.r >= 50 && m_packetHeader.r < 75)
        {
            m_packetHeader.r = 50;
        }
        else if(m_packetHeader.r >= 75 && m_packetHeader.r < 100)
        {
            m_packetHeader.r = 75;
        }
        m_packetHeader.stopON = false;
    }
    
    if(m_packetHeader.l > 0 && m_packetHeader.r > 0)
    {
        m_packetHeader.stopON = true;
    }
    

    if(m_packetHeader.reverseON == false)
    {
        m_packetHeader.waveDivider = (uint32_t)(50000000 / (256 * (((max * m_packetHeader.r)/100) + valor_min)));
    }
    else
    {
        m_packetHeader.waveDivider = (uint32_t)(50000000 / (256 * (((max * m_packetHeader.l)/100) + valor_min)));
    }
    //m_packetHeader.waveDivider = (float)m_packetHeader.dividendConst * (1 / d);
    //std::cout << m_packetHeader.waveDivider  << std::endl;
    //std::cout << d << std::endl;
     
    uint32_t breakHelper = 0b00001000000000000000000000000000;
    uint32_t reverseHelper = 0b00000100000000000000000000000000;
    
    m_packetHeader.payload = m_packetHeader.motores[m_motorIdx] | 
                             m_packetHeader.waveDivider;
                            
    if(m_packetHeader.stopON == true)
    {
        m_packetHeader.payload |= breakHelper; 
    }
    
    if(m_packetHeader.reverseON == true)
    {
        m_packetHeader.payload |= reverseHelper;
    }
    
    //std::bitset<32> x(m_packetHeader.payload);
    //std::cout <<  x << std::endl;
    serial.WriteData(m_packetHeader.payload);
    std::string dataFromEsp = serial.ReceiveData();
    if(dataFromEsp.length() > 1)
    //{
    //    std::cout << "bits do esp: " << dataFromEsp << std::endl;
    //}
    
    std::cout << "bits do esp: " <<dataFromEsp << std::endl;
}

void TelaGamePad::HandleInput() 
{
    m_gamepad.Update();
}

void TelaGamePad::Update()
{
    m_lTratado = (((uint32_t)((1 + m_gamepad.l_triggerAxis) * 100)) / 2);
    m_rTratado = (((uint32_t)((1 + m_gamepad.r_triggerAxis) * 100)) / 2);
    if(m_packetHeader.stopON) {
        m_frequencia = 0;
    } else {
        m_frequencia = 50000000/(256*m_packetHeader.waveDivider + 1);
    }
    
    if(m_gamepad.r_bumper == true)
    {
        m_motorIdx++;
        if(m_motorIdx > 5)
        {
            m_motorIdx = 0;
        }
    }
    if(m_gamepad.l_bumper == true)
    {
        m_motorIdx--;
        if(m_motorIdx < 0)
        {
            m_motorIdx = 5;
        }
    }
    
    UpdateESPPacket();
}

void TelaGamePad::Draw()
{
    //DrawText(TextFormat("GATILHO L: %.02f", 1, m_gamepad.l_triggerAxis), 20, 70 + 20, 75, DARKGRAY);
    //DrawText(TextFormat("GATILHO R: %.02f", 1, m_gamepad.r_triggerAxis), 20, 200 + 20, 75, DARKGRAY);
    //DrawText(TextFormat("GATILHO L: %d", m_lTratado), 20, 70 + 20, 75, DARKGREEN);
    //DrawText(TextFormat("GATILHO R: %d", m_rTratado), 20, 200 + 20, 75, DARKGREEN);
    DrawText(TextFormat("GATILHO L: %d", m_packetHeader.l), 20, 70 + 20, 75, DARKGREEN);
    DrawText(TextFormat("GATILHO R: %d", m_packetHeader.r), 20, 200 + 20, 75, DARKGREEN);
    DrawText(TextFormat("ID DO MOTOR: %d", m_motorIdx + 1), 20, 350 + 20, 50, PURPLE);
    DrawText(TextFormat("REVERSE: %s", m_packetHeader.reverseON ? "true" : "false"), 20, 400 + 20, 50, PURPLE);
    DrawText(TextFormat("BREAK: %s", m_packetHeader.stopON ? "true" : "false"), 20, 450 + 20, 50, RED);
    DrawText(TextFormat("DIVISOR EM DECIMAL: %d", m_packetHeader.waveDivider), 20, 550 + 20, 50, DARKPURPLE);
    DrawText(TextFormat("FREQUÊNCIA: %d Hz", m_frequencia), 20, 600 + 20, 50, DARKPURPLE);
}

void TelaGamePad::Pause()
{
}

void TelaGamePad::Resume()
{
}

void TelaGamePad::OnEnter()
{
}

void TelaGamePad::OnExit()
{
}

void TelaGamePad::CleanUp()
{
    std::bitset<32> x (m_packetHeader.payload);
    std::cout << "bits do divisor: " << x << std::endl;
    std::cout << "divisor em decimal: " << m_packetHeader.waveDivider << std::endl;
}
