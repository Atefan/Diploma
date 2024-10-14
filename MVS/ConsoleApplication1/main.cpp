#include "serialib.h"
#include <iostream>
int main() {
    serialib serial;
    char buffer[128];

    // Open serial port
    if (serial.openDevice("COM7", 9600) != 1) {
        std::cout<<"Error opening port\n";
        return -1;
    }

    // Read data
    while (1)
    {
        int bytesRead = serial.readString(buffer, '!', 128, 5000);
        if (bytesRead > 0) {
            std::cout << "Data read: " << buffer << std::endl;
        }
        else {
            printf("No data read\n");
        }
    }
    

    // Close serial port
    serial.closeDevice();
    return 0;
}
