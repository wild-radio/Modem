cmake_minimum_required(VERSION 3.6.2)
project(Modem)

set(CMAKE_CXX_STANDARD 11)

add_executable(
        Modem src/main.cpp
        src/ModemInterface.hpp
        src/MiniModem/MiniModem.cpp
        src/MiniModem/MiniModem.hpp
        src/MiniModem/StreamException.hpp
        src/Exception.hpp
        src/ModemResolver.cpp
        src/ModemResolver.hpp
        src/MiniModem/PTT.cpp
        src/MiniModem/PTT.hpp
        src/WRCP/WRCP.cpp
        src/WRCP/WRCP.hpp
        src/WRCP/WRCPController.cpp
        src/WRCP/WRCPController.hpp
        src/WRCP/WRCPReceiver.cpp
        src/WRCP/WRCPReceiver.hpp
        src/ThreadInterface.hpp
        src/WRCP/WRCPTransmitter.cpp
        src/WRCP/WRCPTransmitter.hpp
        src/MessageQueue/MessageQueue.hpp
        src/test/TestModem.cpp
        src/test/TestModem.hpp
        src/WRCP/WRCPTimeoutException.hpp
        src/WRCP/WRCPPermissionException.hpp
        src/test/FileModem.cpp
        src/test/FileModem.hpp
        src/SSTV/buffer.cpp
        src/SSTV/buffer.hpp
        src/SSTV/ddc.cpp
        src/SSTV/ddc.hpp
        src/SSTV/Image.cpp
        src/SSTV/Image.hpp
        src/SSTV/mmap_file.cpp
        src/SSTV/mmap_file.hpp
        src/SSTV/pcm.cpp
        src/SSTV/pcm.hpp
        src/SSTV/ppm.cpp
        src/SSTV/ppm.hpp
        src/SSTV/Pulse.cpp
        src/SSTV/Pulse.hpp
        src/SSTV/Robot36Encoder.cpp
        src/SSTV/Robot36Encoder.hpp
        src/SSTV/Robot36Decoder.cpp
        src/SSTV/Robot36Decoder.hpp
        src/SSTV/utils.cpp
        src/SSTV/utils.hpp
        src/SSTV/wav.cpp
        src/SSTV/wav.hpp
        src/SSTV/window.cpp
        src/SSTV/window.hpp
        src/SSTV/yuv.cpp
        src/SSTV/yuv.hpp
        src/SSTV/Robot36.cpp
        src/SSTV/Robot36.hpp
        src/Notification/CameraConfigurationsEventMonitor.cpp
        src/Notification/CameraConfigurationsEventMonitor.hpp
        src/Notification/Notification.cpp
        src/Notification/Notification.hpp
        src/SendPhotoToServer/SendPhotoToServer.cpp
        src/SendPhotoToServer/SendPhotoToServer.hpp
        src/Base64/base64.hpp
        src/Base64/base64.cpp
        src/Notification/CameraConfigurations.cpp
        src/Notification/CameraConfigurations.hpp
        src/Utils/FileUtils.cpp
        src/Utils/FileUtils.hpp
        src/Notification/PhotosEventMonitor.cpp
        src/Notification/PhotosEventMonitor.hpp
        src/ControlRecordAccess.cpp
        src/ControlRecordAccess.hpp
        src/WRCP/WRCPOtherPacketException.hpp src/MiniModem/PTTResolver.cpp src/MiniModem/PTTResolver.hpp)
target_link_libraries(Modem wiringPi pthread m pulse-simple rt curl)

project(decoder36)

add_executable( decoder36
        src/decoder_main.cpp
        src/SSTV/buffer.cpp
        src/SSTV/buffer.hpp
        src/SSTV/ddc.cpp
        src/SSTV/ddc.hpp
        src/SSTV/Image.cpp
        src/SSTV/Image.hpp
        src/SSTV/mmap_file.cpp
        src/SSTV/mmap_file.hpp
        src/SSTV/pcm.cpp
        src/SSTV/pcm.hpp
        src/SSTV/ppm.cpp
        src/SSTV/ppm.hpp
        src/SSTV/Pulse.cpp
        src/SSTV/Pulse.hpp
        src/SSTV/Robot36Decoder.cpp
        src/SSTV/Robot36Decoder.hpp
        src/SSTV/utils.cpp
        src/SSTV/utils.hpp
        src/SSTV/wav.cpp
        src/SSTV/wav.hpp
        src/SSTV/window.cpp
        src/SSTV/window.hpp
        src/SSTV/yuv.cpp
        src/SSTV/yuv.hpp
        src/SendPhotoToServer/SendPhotoToServer.cpp
        src/SendPhotoToServer/SendPhotoToServer.hpp
        src/Utils/FileUtils.cpp
        src/Utils/FileUtils.hpp
        src/Base64/base64.hpp
        src/Base64/base64.cpp
        )
target_link_libraries(decoder36 pthread m pulse-simple rt curl)