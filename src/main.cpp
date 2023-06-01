#include <iostream>
#include <raylib.h>
#include <Base/App.hpp>
#include <Base/Communication/Serial.hpp>

#define RAYGUI_IMPLEMENTATION//Definitions for UI
//#include <TelaSerialMotor/TelaSerialMotor.hpp>
#include <TelaGamePad/TelaGamePad.hpp>


//Serial example
/*
int main()
{
  // create an asio io_service object
  boost::asio::io_service io_service;

  // create a serial port object
  boost::asio::serial_port serial_port(io_service);

  // open the serial port
  serial_port.open("/dev/ttyUSB0");

  // set serial port options
  serial_port.set_option(boost::asio::serial_port_base::baud_rate(9600));
  serial_port.set_option(boost::asio::serial_port_base::character_size(8));
  serial_port.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
  serial_port.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
  serial_port.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

  // create a buffer to store the received data
  char buffer[sizeof(uint32_t)];

  // read data from the serial port
  uint32_t n = boost::asio::read(serial_port, boost::asio::buffer(buffer, sizeof(uint32_t)));
  
  uint32_t b = 'a';
  uint32_t *a = &b;
  boost::asio::write(serial_port, boost::asio::buffer(a, sizeof(uint32_t)));

  // convert the received data to binary
  std::string binary_data;
  for (size_t i = 0; i < n; i++) {
    for (int j = 31; j >= 0; j--) {
      if (buffer[i] & (1 << j)) {
        binary_data.push_back('1');
      } else {
        binary_data.push_back('0');
      }
    }
  }

  // print the binary data
  std::cout << "Received data in binary format: " << binary_data << std::endl;

  // close the serial port
  serial_port.close();
  
  return 0;
}*/

int main() 
{
    //Serial serial;
    App app(980, 720, "Yuki");
    app.Run(new TelaGamePad());
    
    return 0;
}
