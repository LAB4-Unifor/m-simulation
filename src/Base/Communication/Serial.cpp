#include "Serial.hpp"

#ifndef __linux__
std::vector<std::string>  Serial::CheckPorts()
{
    std::vector<std::string> port_List;
    boost::filesystem::path path("/dev/tty");//Set path as string

    return port_List;
}
#endif

#ifdef _WIN32
std::vector<std::string>  Serial::CheckPorts()
{

}
#endif
