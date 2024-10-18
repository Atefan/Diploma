#include "serialib.h"
#include "NIST_tests.h"

#include <iostream>
#include <random>
#include <vector>

#define VAR_SIZE sizeof(uint32_t)
#define BLOCK_SIZE 2048

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

    int num_uint32 = size_in_bits / VAR_SIZE;
    random_sequence.reserve(num_uint32);

    for (int i = 0; i < num_uint32; ++i) {
        if (random)
            random_sequence.push_back(dis(gen));
        else
            random_sequence.push_back(~i*37);
    }

    return random_sequence;
}


std::vector<uint32_t> generate_random_sequence_srand(int size_in_bits, bool random) {
    std::vector<uint32_t> random_sequence;

    std::srand(std::time(0));

    int num_uint32 = size_in_bits / VAR_SIZE;
    random_sequence.reserve(num_uint32);

    for (int i = 0; i < num_uint32; ++i) {
        if (random)
            random_sequence.push_back((uint32_t)std::rand);
        else
            random_sequence.push_back(~i * 37);
    }

    return random_sequence;
}

int main() {
    NIST_tests tester;

    std::vector<uint32_t> random_sequence = generate_random_sequence(2048 * VAR_SIZE * 32, true); // (32 blocks or 2048*32 uints)
    
    tester.frequency_monobit_test(random_sequence);
    tester.frequency_within_block_test(random_sequence, BLOCK_SIZE);
    tester.runs_test(random_sequence);
    tester.longest_run_of_ones_in_block_test(random_sequence, BLOCK_SIZE);
    tester.entropy_test(random_sequence);
    return 0;
}
