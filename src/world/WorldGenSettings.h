#pragma once
#include <string>

struct WorldGenSettings {
    int   mapWidth = 128;
    int   mapHeight = 128;
    float noiseScale = 0.05f;
    float warpStrength = 6.0f;
    int   maxHeight = 10;
    float lakeThreshold = 0.5f;   // 0 = no lakes, 1 = lakes everywhere
    int   seed = -1;     // -1 = random

    // Display helpers
    static const char* mapSizeLabel(int idx) {
        switch (idx) {
        case 0: return "Small  (64x64)";
        case 1: return "Medium (128x128)";
        case 2: return "Large  (192x192)";
        default: return "Medium (128x128)";
        }
    }
    static void applyMapSizePreset(int idx, WorldGenSettings& s) {
        switch (idx) {
        case 0: s.mapWidth = s.mapHeight = 64;  break;
        case 1: s.mapWidth = s.mapHeight = 128; break;
        case 2: s.mapWidth = s.mapHeight = 192; break;
        }
    }
};