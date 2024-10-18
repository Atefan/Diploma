#include "NIST_tests.h"
#include <iostream>
#include <bitset>
#include <cmath>
#include <map>

bool NIST_tests::frequency_monobit_test(const std::vector<uint32_t>& sequence) {
    int ones_count = 0;
    int total_bits = sequence.size() * 32;

    for (uint32_t num : sequence) {
        ones_count += std::bitset<32>(num).count();
    }

    int zeros_count = total_bits - ones_count;
    double proportion = static_cast<double>(ones_count) / total_bits;
    double statistic = std::abs(ones_count - zeros_count) / std::sqrt(total_bits);

    if (statistic > 1.96) {
        std::cout << "Monobit test failed: Statistic " << statistic << " is greater than 1.96. Zeros to Onces "<< zeros_count  << " : " << ones_count << std::endl;
        return false;
    }

    std::cout << "Monobit test passed: Statistic " << statistic << " is within bounds. Zeros to Onces " << zeros_count << " : " << ones_count << std::endl;
    return true;
}

bool NIST_tests::frequency_within_block_test(const std::vector<uint32_t>& sequence, int block_size) {
    int num_blocks = sequence.size() * 32 / block_size;
    bool success = true;
    for (int i = 0; i < num_blocks; ++i) {
        int block_ones_count = 0;
        for (int j = i * block_size / 32; j < (i + 1) * block_size / 32; ++j) {
            block_ones_count += std::bitset<32>(sequence[j]).count();
        }

        double proportion = static_cast<double>(block_ones_count) / block_size;
        double statistic = std::abs(proportion - 0.5) * std::sqrt(block_size);

        if (statistic > 1.96) {
            std::cout << "Frequency within block test failed: Block " << i << " has statistic " << statistic << " greater than 1.96. Proporition of Onces to Size: "<< block_ones_count << " : " << block_size << std::endl;
            success = false;
        }
    }
    if(success)
        std::cout << "Frequency within block test passed: All block statistics are within bounds." << std::endl;
    return success;
}

bool NIST_tests::runs_test(const std::vector<uint32_t>& sequence) {
    bool success = true;
    int ones_count = 0, total_bits = sequence.size() * 32;

    for (uint32_t num : sequence) {
        ones_count += std::bitset<32>(num).count();
    }

    double pi = static_cast<double>(ones_count) / total_bits;
    if (std::abs(pi - 0.5) > 2 / std::sqrt(total_bits)) {
        std::cout << "Runs test failed: Pi " << pi << " is too far from 0.5. Continuing anyways!" << std::endl;
        success = false;
    }

    int runs_count = 1;
    bool current_bit = sequence[0] & 1;

    for (uint32_t num : sequence) {
        for (int i = 0; i < 32; ++i) {
            bool bit = num & (1 << i);
            if (bit != current_bit) {
                runs_count++;
                current_bit = bit;
            }
        }
    }

    size_t expected_runs = 2 * total_bits * pi * (1 - pi);
    size_t runs_statistic = std::abs((double)runs_count - expected_runs) / std::sqrt(2 * total_bits * pi * (1 - pi));

    if (runs_statistic > 1.96) {
        std::cout << "Runs test failed: Runs statistic " << runs_statistic << " is greater than 1.96. Total runs: "<< runs_count << std::endl;
        success = false;
    }
    if(success)
        std::cout << "Runs test passed: Runs statistic " << runs_statistic << " is within bounds.Runs Count ot statistical runs : " << runs_count << ":"<< expected_runs  << std::endl;
    return success;
}

bool NIST_tests::longest_run_of_ones_in_block_test(const std::vector<uint32_t>& sequence, int block_size) {
    int num_blocks = sequence.size() * 32 / block_size, max_run_length;

    for (int i = 0; i < num_blocks; ++i) {
        int  current_run = 0;
        max_run_length = 0;
        for (int j = i * block_size / 32; j < (i + 1) * block_size / 32; ++j) {
            for (int bit = 0; bit < 32; ++bit) {
                if (sequence[j] & (1 << bit)) {
                    current_run++;
                    max_run_length = std::max(max_run_length, current_run);
                }
                else {
                    current_run = 0;
                }
            }
        }

        if (max_run_length > 34) { // Example threshold value for a 2048-bit block
            std::cout << "Longest run of ones in block test failed: Block " << i << " has max run length " << max_run_length << " greater than 34." << std::endl;
            return false;
        }
    }

    std::cout << "Longest run of ones in block test passed: All blocks have acceptable run lengths. Max run was: "<< max_run_length << std::endl;
    return true;
}

bool NIST_tests::entropy_test(const std::vector<uint32_t>& sequence)
{
    std::map<uint32_t, uint32_t> entropy;

    for (uint32_t number : sequence) 
        entropy[number]++;

    std::cout << "Entropy distribution (value : count):" << std::endl;
    for (const auto& pair : entropy) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }

    return true;
}
