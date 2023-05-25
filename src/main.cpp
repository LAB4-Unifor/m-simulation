#include <iostream>
#include <raylib.h>
#include <boost/asio.hpp>
#include <Base/App.hpp>
#define RAYGUI_IMPLEMENTATION
#include <TelaSerialMotor/TelaSerialMotor.hpp>


/*Serial example
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
  char buffer[1024];

  // read data from the serial port
  size_t n = boost::asio::read(serial_port, boost::asio::buffer(buffer, 1024));

  // convert the received data to binary
  std::string binary_data;
  for (size_t i = 0; i < n; i++) {
    for (int j = 7; j >= 0; j--) {
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
  serial_port.close();*/

int main() 
{
    App app(980, 720, "Yuki", new TelaSerialMotor(&app.coordenador));
    app.Run();

    return 0;
}
