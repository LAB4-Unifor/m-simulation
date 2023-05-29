#include <iostream>
#include <bitset>
#include <boost/asio.hpp>

struct MotorData {
// Máscaras para os primeiros quatro bits, quinto bit e sexto bit
const uint32_t mascara4Bits = 0xF0000000;
const uint32_t mascara5Bit = 0x08000000;
const uint32_t mascara6Bit = 0x04000000;

// Sequências de bits dos motores
const uint32_t motor1 = 0x80000000;
const uint32_t motor2 = 0x40000000;
const uint32_t motor3 = 0x20000000;
const uint32_t motor4 = 0x10000000;
const uint32_t motor5 = 0x90000000;
const uint32_t motor6 = 0x50000000;

// Variáveis para armazenar os estados do Break e Reverse
bool breakLigado = false;
bool reverseLigado = false;

// Constante de velocidade
const uint32_t velocidade = 0x03FFFFFF; // 26 bits restantes definidos como 1

};

int main() {
boost::asio::io_service ioService;
boost::asio::serial_port serialPort(ioService);
try {
    serialPort.open("/dev/ttyUSB0");  // Especifique a porta USB correta

    serialPort.set_option(boost::asio::serial_port_base::baud_rate(9600));
    serialPort.set_option(boost::asio::serial_port_base::character_size(8));
    serialPort.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    serialPort.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    serialPort.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

    // Opções para o ESP32
    std::vector<std::string> esp32Options = {"ESP32-A", "ESP32-B", "ESP32-C"};
    std::cout << "Escolha o ESP32 (0 - ESP32-A, 1 - ESP32-B, 2 - ESP32-C): ";
    int esp32Selection;
    std::cin >> esp32Selection;

    // Verificar a seleção do ESP32
    if (esp32Selection < 0 || esp32Selection >= esp32Options.size()) {
        std::cerr << "Seleção inválida do ESP32." << std::endl;
        return 1;
    }

    std::cout << "ESP32 selecionado: " << esp32Options[esp32Selection] << std::endl;

    // Enviar lista de portas abertas para o ESP32 selecionado
    std::string portList = "Port1,Port2,Port3";  // Exemplo de lista de portas abertas
    std::cout << "Enviando lista de portas abertas para o ESP32: " << portList << std::endl;
    boost::asio::write(serialPort, boost::asio::buffer(portList));

    // Receber dado como um inteiro
    uint32_t entrada;
    std::cout << "Digite o número de entrada: ";
    std::cin >> entrada;

    // Verificar padrões de bits
    MotorData motorData;
    motorData.reverseLigado = (entrada & motorData.mascara5Bit) != 0;
    motorData.breakLigado = (entrada & motorData.mascara6Bit) != 0;

    std::cout << "Reverse: " << (motorData.reverseLigado ? "ligado" : "desligado") << std::endl;
    std::cout << "Break: " << (motorData.breakLigado ? "ligado" : "desligado") << std::endl;

    // Mostrar bit concatenados
    std::bitset<32> bin(entrada);
    std::cout << "Bits concatenados: " << bin << std::endl;

    // Verificar confirmação
    std::string confirmacao;
    std::cout << "Digite 'sim' para confirmar ou 'nao' para cancelar: ";
    std::cin >> confirmacao;

    if (confirmacao == "sim") {
        std::cout << "Enviando binario para o ESP32: " << entrada << std::endl;
        boost::asio::write(serialPort, boost::asio::buffer(&entrada, sizeof(entrada)));

        // Receber resposta do ESP32
        char resposta[128];
        size_t bytesRead = boost::asio::read(serialPort, boost::asio::buffer(resposta, sizeof(resposta)));

        // Verificar se o código está correto
        std::string respostaStr(resposta, bytesRead);
        if (respostaStr == "CORRETO") {
            std::cout << "Codigo correto recebido do ESP32: " << respostaStr << std::endl;

            // Confirmar e enviar para o ESP32 selecionado
            std::string confirmacao = "CONFIRMADO";
            std::cout << "Enviando confirmacao para o ESP32: " << confirmacao << std::endl;
            boost::asio::write(serialPort, boost::asio::buffer(confirmacao));
        } else {
            std::cout << "Codigo incorreto recebido do ESP32: " << respostaStr << std::endl;
        }
    } else {
        std::cout << "Envio cancelado." << std::endl;
    }
} catch (const boost::system::system_error& error) {
    std::cerr << "Erro: " << error.what() << std::endl;
}

return 0;
