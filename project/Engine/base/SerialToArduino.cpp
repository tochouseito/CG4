#include "SerialToArduino.h"
bool SerialToArduino::InitializeSerialPort(const char* portName) {
    hSerial = CreateFileA(
        portName, GENERIC_READ, 0, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening serial port" << std::endl;
        return false;
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error getting device state" << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "Error setting device parameters" << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "Error setting timeouts" << std::endl;
        CloseHandle(hSerial);
        return false;
    }

    return true;
}

std::string SerialToArduino::ReadSerialPort() {
    DWORD bytesRead;
    char buffer[256];
    std::string result = "";

    if (ReadFile(hSerial, buffer, sizeof(buffer), &bytesRead, NULL)) {
        result.append(buffer, bytesRead);
    }

    return result;
}