#include <windows.h>
#include <iostream>
#include <string>

class SerialToArduino {



public:
    bool InitializeSerialPort(const char* portName);

    std::string ReadSerialPort();
private:
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };

};

