#pragma once

#include <vector>
#include <cstdint>

class NIST_tests {
public:
    bool frequency_monobit_test(const std::vector<uint32_t>& sequence);
    bool frequency_within_block_test(const std::vector<uint32_t>& sequence, int block_size);
    bool runs_test(const std::vector<uint32_t>& sequence);
    bool longest_run_of_ones_in_block_test(const std::vector<uint32_t>& sequence, int block_size);
    bool entropy_test(const std::vector<uint32_t>& sequence);
};


