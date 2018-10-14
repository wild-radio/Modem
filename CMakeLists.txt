cmake_minimum_required(VERSION 3.9)
project(Modem)

set(CMAKE_CXX_STANDARD 17)

add_executable(Modem src/main.cpp src/ModemInterface.hpp src/MiniModem/MiniModem.cpp src/MiniModem/MiniModem.hpp src/MiniModem/StreamException.hpp src/Exception.hpp src/ModemResolver.cpp src/ModemResolver.hpp)