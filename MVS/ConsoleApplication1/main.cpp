#include "serialib.h"
#include "NIST_tests.h"

#include <iostream>
#include <random>
#include <vector>

void test()
{
    serialib serial;
    char buffer[128];
    int cnt = 0;

    // Open serial port
    if (serial.openDevice("COM7", 9600) != 1) {
        std::cout << "Error opening port\n";
        return;
    }

    // Read data
    while (1)
    {
        if (++cnt == 10)
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
}


std::vector<uint32_t> generate_random_sequence(int size_in_bits, bool random) {
    std::vector<uint32_t> random_sequence;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF); // [0, 2^32 - 1]

    int num_uint32 = size_in_bits / 32;
    random_sequence.reserve(num_uint32);

    for (int i = 0; i < num_uint32; ++i) {
        if (random)
            random_sequence.push_back(dis(gen));
        else
            random_sequence.push_back(~i*37);
    }

    return random_sequence;
}

int main() {
    NIST_tests tester;

    std::vector<uint32_t> random_sequence1 = generate_random_sequence(2048, false); // (64 uint32_t numbers)
    std::vector<uint32_t> random_sequence = {
    0xAAAAAAAA,  // Alternating bits: 10101010...
    0x55555555,  // Alternating bits: 01010101...
    0xFFFFFFFF,  // All bits set to 1
    0x00000000,  // All bits set to 0
    0x12345678,  // Sequential pattern
    0x87654321,  // Reverse sequential pattern
    0xFFFFFF00,  // Upper 24 bits set to 1, lower 8 bits set to 0
    0x0000FFFF,  // Lower 16 bits set to 1, upper 16 bits set to 0
    0xCCCCCCCC,  // Alternating pair of 1s and 0s: 11001100...
    0x33333333,  // Alternating pair of 0s and 1s: 00110011...
    0x7FFFFFFF,  // All bits set to 1 except the most significant bit
    0x80000000,  // Only the most significant bit set to 1
    0xF0F0F0F0,  // Alternating nibbles of 1s and 0s: 11110000...
    0x0F0F0F0F,  // Alternating nibbles of 0s and 1s: 00001111...
    0xAAAA5555,  // Half bits alternating: upper 16 bits different from lower 16
    0x5555AAAA   // Reverse of the previous number
    };

    if (tester.frequency_monobit_test(random_sequence)) 
        std::cout << "Monobit test passed." << std::endl;
    else 
        std::cout << "Monobit test failed." << std::endl;


    if (tester.frequency_within_block_test(random_sequence)) 
        std::cout << "Frequency within block test passed." << std::endl;
    else 
        std::cout << "Frequency within block test failed." << std::endl;


    if (tester.runs_test(random_sequence)) 
        std::cout << "Runs test passed." << std::endl;
    else 
        std::cout << "Runs test failed." << std::endl;


    if (tester.longest_run_of_ones_in_block_test(random_sequence)) 
        std::cout << "Longest run of ones in block test passed." << std::endl;
    else 
        std::cout << "Longest run of ones in block test failed." << std::endl;

    return 0;
}
