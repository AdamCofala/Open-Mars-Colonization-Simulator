#pragma once
#include <array>
#include "raylib.h"
#include <string>
#include <bitset>
#include <unordered_map>

class TextureManager
{
public:
    std::array<Rectangle, 16> TileTexturesInfo;
    Texture2D tile_atlas = LoadTexture(RESOURCES_PATH "tile_atlas.png");

    std::array<Rectangle, 16> PipeTexturesInfo;
    Texture2D pipe_atlas = LoadTexture(RESOURCES_PATH "pipe_atlas.png");

    std::array<Rectangle, 16> IceTexturesInfo;
    Texture2D ice_atlas = LoadTexture(RESOURCES_PATH "ice_atlas.png");

    std::unordered_map<std::string, Texture2D> StuctureTexturesInfo{
        {"solar_panels",   LoadTexture(RESOURCES_PATH "solar_panels.png")},
        {"ice_melter",     LoadTexture(RESOURCES_PATH "ice_melter.png")},
        {"pipe",           LoadTexture(RESOURCES_PATH "pipe.png")},
        {"water_magazine", LoadTexture(RESOURCES_PATH "water_magazine.png")}  // <- NOWY
    };

    Texture2D cursor = LoadTexture(RESOURCES_PATH "cursor.png");

private:
    void init() {
        load_tiles_textures();
        load_pipes_textures();
        load_ice_textures();
    }

    void load_tiles_textures() {
        TileTexturesInfo = {
            Rectangle{0,   0, 64, 31}, // 0000
            Rectangle{65,  0, 64, 31}, // 0001
            Rectangle{130, 0, 64, 23}, // 0010
            Rectangle{195, 0, 64, 23}, // 0011
            Rectangle{260, 0, 64, 31}, // 0100
            Rectangle{325, 0, 64, 31}, // 0101
            Rectangle{390, 0, 64, 23}, // 0110
            Rectangle{455, 0, 64, 23}, // 0111
            Rectangle{520, 0, 64, 39}, // 1000
            Rectangle{585, 0, 64, 39}, // 1001
            Rectangle{650, 0, 64, 31}, // 1010
            Rectangle{715, 0, 64, 31}, // 1011
            Rectangle{780, 0, 64, 39}, // 1100
            Rectangle{845, 0, 64, 39}, // 1101
            Rectangle{910, 0, 64, 31}, // 1110
            Rectangle{0, 0, 0, 0}     // 1111
        };
    }

    void load_pipes_textures() {
        PipeTexturesInfo.fill(Rectangle{ 0, 0, 0, 0 });
        PipeTexturesInfo[1] = Rectangle{ 715, 0, 64, 42 };
        PipeTexturesInfo[0] = Rectangle{ 780, 0, 64, 42 };
        PipeTexturesInfo[4] = Rectangle{ 845, 0, 64, 35 };
        PipeTexturesInfo[2] = Rectangle{ 910, 0, 64, 35 };
        PipeTexturesInfo[3] = Rectangle{ 0, 0, 64, 35 };
        PipeTexturesInfo[5] = Rectangle{ 65, 0, 64, 35 };
        PipeTexturesInfo[6] = Rectangle{ 130, 0, 64, 31 };
        PipeTexturesInfo[7] = Rectangle{ 195, 0, 64, 35 };
        PipeTexturesInfo[8] = Rectangle{ 715, 0, 64, 42 };
        PipeTexturesInfo[9] = Rectangle{ 260, 0, 64, 35 };
        PipeTexturesInfo[10] = Rectangle{ 325, 0, 64, 35 };
        PipeTexturesInfo[11] = Rectangle{ 390, 0, 64, 35 };
        PipeTexturesInfo[12] = Rectangle{ 455, 0, 64, 35 };
        PipeTexturesInfo[13] = Rectangle{ 520, 0, 64, 35 };
        PipeTexturesInfo[14] = Rectangle{ 585, 0, 64, 35 };
        PipeTexturesInfo[15] = Rectangle{ 650, 0, 64, 35 };
    }

    void load_ice_textures() {
        IceTexturesInfo.fill(Rectangle{ 0, 0, 0, 0 });
        IceTexturesInfo[0] = Rectangle{ 0, 0, 64, 31 };
        IceTexturesInfo[1] = Rectangle{ 65, 0, 64, 31 };
        IceTexturesInfo[4] = Rectangle{ 130, 0, 64, 31 };
        IceTexturesInfo[8] = Rectangle{ 195, 0, 64, 39 };
        IceTexturesInfo[2] = Rectangle{ 260, 0, 64, 23 };
    }

    void UnloadTextures() {
        UnloadTexture(tile_atlas);
        UnloadTexture(pipe_atlas);
        UnloadTexture(ice_atlas);
        UnloadTexture(cursor);
        for (const auto& pair : StuctureTexturesInfo) {
            UnloadTexture(pair.second);
        }
    }

public:
    TextureManager() { init(); }
    ~TextureManager() { UnloadTextures(); }

    const int map_binary_to_index(int data[4]) const {
        return (data[0] << 3) | (data[1] << 2) | (data[2] << 1) | data[3];
    }

    const Rectangle& map_pipe_to_texture(int data[4]) const {
        return PipeTexturesInfo[map_binary_to_index(data)];
    }

    const Rectangle& map_ice_to_texture(int data[4]) const {
        return IceTexturesInfo[map_binary_to_index(data)];
    }

    const Rectangle& map_slope_to_texture(int data[4]) const {
        return TileTexturesInfo[map_binary_to_index(data)];
    }
};