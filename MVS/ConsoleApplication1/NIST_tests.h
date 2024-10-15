#pragma once

#include <vector>
#include <cstdint>

class NIST_tests {
public:
    // Frequency (Monobit) Test
    bool frequency_monobit_test(const std::vector<uint32_t>& sequence);

    // Frequency Test Within A Block
    bool frequency_within_block_test(const std::vector<uint32_t>& sequence, int block_size = 2048);

    // Runs Test
    bool runs_test(const std::vector<uint32_t>& sequence);

    // Longest Run of Ones in a Block Test
    bool longest_run_of_ones_in_block_test(const std::vector<uint32_t>& sequence, int block_size = 2048);
};


