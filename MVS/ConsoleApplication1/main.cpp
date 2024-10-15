#include "serialib.h"
#include <iostream>
int main() {
    serialib serial;
    char buffer[128];
    int cnt = 0;

    // Open serial port
    if (serial.openDevice("COM7", 9600) != 1) {
        std::cout<<"Error opening port\n";
        return -1;
    }

    // Read data
    while (1)
    {
        if(++cnt == 10)
        {
            serial.writeChar('A');
            cnt = 0;
        }
        else
        {
            int bytesRead = serial.readString(buffer, '\n', 128, 5000);
            if (bytesRead > 0) 
                std::cout << "Data read: " << buffer << std::endl;
            else 
                printf("No data read\n");

        }
    }
    

    // Close serial port
    serial.closeDevice();
    return 0;
}
