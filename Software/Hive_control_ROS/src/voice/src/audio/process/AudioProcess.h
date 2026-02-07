#pragma once
#include <vector>
#include <cstdint>

class AudioProcess {
public:
    static std::vector<int16_t> removeNoise(const std::vector<int16_t>& audio);
    static std::vector<int16_t> normalize(const std::vector<int16_t>& audio);
    static bool detectSpeech(const std::vector<int16_t>& audio);
};
