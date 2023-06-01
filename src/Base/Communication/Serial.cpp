#include "Serial.hpp"

void Serial::WriteData(uint32_t data)
{
    boost::asio::write(m_serialPort, boost::asio::buffer(&data, 4));
}

std::string Serial::ReceiveData()
{
    std::string s;
    char c;
    for(;;)
    {
        boost::asio::read(m_serialPort, boost::asio::buffer(&c, 1));

        switch(c)
        {
            case '\r':
                break;
            case '\n':
                return s;
            default:
                s += c;
        }
    }
}


#ifdef _WIN32
std::vector<std::string>  Serial::CheckPorts()
{

}
#endif
