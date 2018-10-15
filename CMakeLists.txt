cmake_minimum_required(VERSION 3.6.2)
project(Modem)

set(CMAKE_CXX_STANDARD 11)

add_executable(Modem src/main.cpp src/ModemInterface.hpp src/MiniModem/MiniModem.cpp src/MiniModem/MiniModem.hpp src/MiniModem/StreamException.hpp src/Exception.hpp src/ModemResolver.cpp src/ModemResolver.hpp src/MiniModem/PTT.cpp src/MiniModem/PTT.hpp src/WRCP/WRCP.cpp src/WRCP/WRCP.hpp src/WRCP/WRCPController.cpp src/WRCP/WRCPController.hpp src/WRCP/WRCPReceiver.cpp src/WRCP/WRCPReceiver.hpp src/ThreadInterface.hpp src/WRCP/WRCPTransmitter.cpp src/WRCP/WRCPTransmitter.hpp)
target_link_libraries(Modem wiringPi pthread)