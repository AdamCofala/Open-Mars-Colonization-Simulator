#pragma once
#include <vector>

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 237);

    float noise(float x, float y) const;

private:
    std::vector<int> p;

    static float fade(float t);
    static float lerp(float a, float b, float t);
    static float grad(int hash, float x, float y);
};