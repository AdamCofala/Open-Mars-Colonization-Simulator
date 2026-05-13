#pragma once
#include <array>
#include "raylib.h"
#include <string>
#include <bitset>

class TextureManager
{
public:
    std::array<Rectangle, 16> TileTexturesInfo;
	Texture2D tile_atlas = LoadTexture(RESOURCES_PATH "tile_atlas.png");
    Image tile_atlas_image = LoadImage(RESOURCES_PATH "tile_atlas.png");

private:
    void init() {
		load_tiles_textures();
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
    void UnloadTextures() {
        UnloadTexture(tile_atlas);
    }


public:

    TextureManager() { init(); }

    ~TextureManager() {
        UnloadTextures();
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