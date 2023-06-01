#pragma once
#include<iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>

#include <boost/asio.hpp>

//Based on simple_serial
class Serial
{
    public:
        Serial() : service(), m_serialPort(service, "/dev/ttyUSB0")
        {
            m_serialPort.set_option(boost::asio::serial_port_base::baud_rate(115200));
        }
        
        void WriteData(uint32_t data);
        std::string ReceiveData();
        
    private:
        boost::asio::io_service service;
        boost::asio::serial_port m_serialPort;
        const std::string linuxESPPort = "/dev/ttyUSB0"; 
};
