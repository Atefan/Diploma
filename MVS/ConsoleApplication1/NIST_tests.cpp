#include "NIST_tests.h"
#include <iostream>
#include <bitset>
#include <cmath>

bool NIST_tests::frequency_monobit_test(const std::vector<uint32_t>& sequence) {
    int ones_count = 0;
    int total_bits = sequence.size() * 32;


    for (uint32_t num : sequence) {
        ones_count += std::bitset<32>(num).count();
    }

    int zeros_count = total_bits - ones_count;


    double proportion = static_cast<double>(ones_count) / total_bits;
    double statistic = std::abs(ones_count - zeros_count) / std::sqrt(total_bits);


    return (statistic <= 1.96);
}

bool NIST_tests::frequency_within_block_test(const std::vector<uint32_t>& sequence, int block_size) {
    int num_blocks = sequence.size() * 32 / block_size;

    for (int i = 0; i < num_blocks; ++i) {
        int block_ones_count = 0;
        for (int j = i * block_size / 32; j < (i + 1) * block_size / 32; ++j) {
            block_ones_count += std::bitset<32>(sequence[j]).count();
        }

        double proportion = static_cast<double>(block_ones_count) / block_size;
        double statistic = std::abs(proportion - 0.5) * std::sqrt(block_size);

        if (statistic > 1.96) return false;
    }

    return true;
}

bool NIST_tests::runs_test(const std::vector<uint32_t>& sequence) {
    int ones_count = 0, total_bits = sequence.size() * 32;

    for (uint32_t num : sequence) {
        ones_count += std::bitset<32>(num).count();
    }

    double pi = static_cast<double>(ones_count) / total_bits;
    if (std::abs(pi - 0.5) > 2 / std::sqrt(total_bits)) {
        return false;
    }

    int runs_count = 1; // Start with first bit run
    bool current_bit = sequence[0] & 1; // First bit of the sequence

    for (uint32_t num : sequence) {
        for (int i = 0; i < 32; ++i) {
            bool bit = num & (1 << i);
            if (bit != current_bit) {
                runs_count++;
                current_bit = bit;
            }
        }
    }

    double expected_runs = 2 * total_bits * pi * (1 - pi);
    double runs_statistic = std::abs(runs_count - expected_runs) / std::sqrt(2 * total_bits * pi * (1 - pi));

    return (runs_statistic <= 1.96);
}

bool NIST_tests::longest_run_of_ones_in_block_test(const std::vector<uint32_t>& sequence, int block_size) {
    int num_blocks = sequence.size() * 32 / block_size;

    for (int i = 0; i < num_blocks; ++i) {
        int max_run_length = 0, current_run = 0;
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
            return false;
        }
    }

    return true;
}
