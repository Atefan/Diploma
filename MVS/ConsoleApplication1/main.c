#include <windows.h>
#include <stdio.h>

int main() {
    HANDLE hComm;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    char readBuffer[256] = { 0 };
    DWORD bytesRead;

    // Open the COM port
    hComm = CreateFile("\\\\.\\COM4", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error opening COM port\n");
        return 1;
    }

    // Set port parameters
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hComm, &dcbSerialParams)) {
        printf("Error getting COM state\n");
        CloseHandle(hComm);
        return 1;
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hComm, &dcbSerialParams)) {
        printf("Error setting COM state\n");
        CloseHandle(hComm);
        return 1;
    }

    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hComm, &timeouts)) {
        printf("Error setting timeouts\n");
        CloseHandle(hComm);
        return 1;
    }

    // Read data
    if (!ReadFile(hComm, readBuffer, sizeof(readBuffer), &bytesRead, NULL)) {
        printf("Error reading from COM port\n");
    }
    else {
        printf("Read %d bytes: %s\n", bytesRead, readBuffer);
    }

    // Close the COM port
    CloseHandle(hComm);
    return 0;
}
