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
        {"solar_panels", LoadTexture(RESOURCES_PATH "solar_panels.png")},
        { "ice_melter", LoadTexture(RESOURCES_PATH "ice_melter.png") },
        {"pipe", LoadTexture(RESOURCES_PATH "pipe.png")}
    };
    //Texture2D solar_panels = LoadTexture(RESOURCES_PATH "solar_panels.png");
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

			Rectangle{0, 0, 0, 0} // 1111 
        };
    }

    void load_pipes_textures() {
        PipeTexturesInfo.fill(Rectangle{ 0, 0, 0, 0 });
        PipeTexturesInfo[1] = Rectangle{ 715, 0, 64, 42 };  // 0000
        PipeTexturesInfo[0] = Rectangle{ 780, 0, 64, 42 };  // 0001 (W)
        PipeTexturesInfo[2] = Rectangle{ 715, 0, 64, 42 };  // 0010 (S) – fallback do pustej
        PipeTexturesInfo[3] = Rectangle{ 0, 0, 64, 35 };    // 0011 (S+W)
        PipeTexturesInfo[4] = Rectangle{ 715, 0, 64, 42 };  // 0100 (E) – fallback
        PipeTexturesInfo[5] = Rectangle{ 65, 0, 64, 35 };   // 0101 (E+W)
        PipeTexturesInfo[6] = Rectangle{ 130, 0, 64, 31 };  // 0110 (E+S)
        PipeTexturesInfo[7] = Rectangle{ 195, 0, 64, 35 };  // 0111 (E+S+W)
        PipeTexturesInfo[8] = Rectangle{ 715, 0, 64, 42 };  // 1000 (N) – fallback
        PipeTexturesInfo[9] = Rectangle{ 260, 0, 64, 35 };  // 1001 (N+W)
        PipeTexturesInfo[10] = Rectangle{ 325, 0, 64, 35 };  // 1010 (N+S)
        PipeTexturesInfo[11] = Rectangle{ 390, 0, 64, 35 };  // 1011 (N+S+W)
        PipeTexturesInfo[12] = Rectangle{ 455, 0, 64, 35 };  // 1100 (N+E)
        PipeTexturesInfo[13] = Rectangle{ 520, 0, 64, 35 };  // 1101 (N+E+W)
        PipeTexturesInfo[14] = Rectangle{ 585, 0, 64, 35 };  // 1110 (N+E+S)
        PipeTexturesInfo[15] = Rectangle{ 650, 0, 64, 35 };  // 1111
    }

    void load_ice_textures() {
        IceTexturesInfo.fill(Rectangle{0, 0, 0, 0});
        IceTexturesInfo[0] = Rectangle{0, 0, 64, 31};  // 0000
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

    const Rectangle& map_ice_to_texture(int data[4]) const
    {
        int index =
            (data[0] << 3) |
            (data[1] << 2) |
            (data[2] << 1) |
            data[3];

        return IceTexturesInfo[index];
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