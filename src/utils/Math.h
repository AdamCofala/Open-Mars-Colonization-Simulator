#pragma once
#include <cmath>

inline float calc_jitter(int x, int y) {
    int noise = ((x * 1619 + y * 31337) ^ (x + y * 7)) & 0xFF;
    return static_cast<float>((noise % 13) - 6);
}