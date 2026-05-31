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

    std::unordered_map<std::string, Rectangle> StuctureTexturesInfo;
    Texture2D structure_atlas = LoadTexture(RESOURCES_PATH "structure_atlas.png");
    Texture2D solar_panels = LoadTexture(RESOURCES_PATH "solar_panels.png");
    Texture2D cursor = LoadTexture(RESOURCES_PATH "cursor.png");

private:
    void init() {
		load_tiles_textures();
        load_pipes_textures();
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

			Rectangle{0, 0, 0, 0} // 1111 unused
        };
    }

    void load_pipes_textures() {
        PipeTexturesInfo.fill(Rectangle{0, 0, 0, 0});
        
        PipeTexturesInfo[0] = Rectangle{715, 0, 64, 42};  // 0000
        PipeTexturesInfo[1] = Rectangle{780, 0, 64, 42};  // 0001 
        PipeTexturesInfo[3] = Rectangle{0, 0, 64, 35};    // 0011
        PipeTexturesInfo[5] = Rectangle{65, 0, 64, 35};   // 0101
        PipeTexturesInfo[6] = Rectangle{130, 0, 64, 31};  // 0110
        PipeTexturesInfo[7] = Rectangle{195, 0, 64, 35};  // 0111
        PipeTexturesInfo[9] = Rectangle{260, 0, 64, 35};  // 1001
        PipeTexturesInfo[10] = Rectangle{325, 0, 64, 35}; // 1010
        PipeTexturesInfo[11] = Rectangle{390, 0, 64, 35}; // 1011
        PipeTexturesInfo[12] = Rectangle{455, 0, 64, 35}; // 1100
        PipeTexturesInfo[13] = Rectangle{520, 0, 64, 35}; // 1101
        PipeTexturesInfo[14] = Rectangle{585, 0, 64, 35}; // 1110
        PipeTexturesInfo[15] = Rectangle{650, 0, 64, 35}; // 1111
    }

    void UnloadTextures() {
        UnloadTexture(tile_atlas);
        UnloadTexture(pipe_atlas);
        UnloadTexture(structure_atlas);
        UnloadTexture(solar_panels);
        UnloadTexture(cursor);
    }


public:

    TextureManager() { init(); }

    ~TextureManager() {
        UnloadTextures();
    }

    const Rectangle& map_pipe_to_texture(int data[4]) const
    {
        int index =
            (data[0] << 3) |
            (data[1] << 2) |
            (data[2] << 1) |
            data[3];

        return PipeTexturesInfo[index];
    }

    const Rectangle& map_slope_to_texture(int data[4]) const
    {
        int index =
            (data[0] << 3) |
            (data[1] << 2) |
            (data[2] << 1) |
            data[3];

        return TileTexturesInfo[index];
    }
 };