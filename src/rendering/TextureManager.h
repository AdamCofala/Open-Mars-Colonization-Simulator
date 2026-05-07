#pragma once
#include <array>
#include "raylib.h"
#include <string>
#include <bitset>

class TextureManager
{
private:
    std::array<Texture2D, 16> TileTextures;


    void init() {
		load_tiles_textures();
    }

    void load_tiles_textures() {
        TileTextures[0b0000] = LoadTexture(RESOURCES_PATH "Tiles/tile_0000.png");
        TileTextures[0b1000] = LoadTexture(RESOURCES_PATH "Tiles/tile_1000.png");
        TileTextures[0b0100] = LoadTexture(RESOURCES_PATH "Tiles/tile_0100.png");
        TileTextures[0b0010] = LoadTexture(RESOURCES_PATH "Tiles/tile_0010.png");
        TileTextures[0b0001] = LoadTexture(RESOURCES_PATH "Tiles/tile_0001.png");
        TileTextures[0b1100] = LoadTexture(RESOURCES_PATH "Tiles/tile_1100.png");
        TileTextures[0b0110] = LoadTexture(RESOURCES_PATH "Tiles/tile_0110.png");
        TileTextures[0b0011] = LoadTexture(RESOURCES_PATH "Tiles/tile_0011.png");
        TileTextures[0b1001] = LoadTexture(RESOURCES_PATH "Tiles/tile_1001.png");
        TileTextures[0b1110] = LoadTexture(RESOURCES_PATH "Tiles/tile_1110.png");
        TileTextures[0b0111] = LoadTexture(RESOURCES_PATH "Tiles/tile_0111.png");
        TileTextures[0b1011] = LoadTexture(RESOURCES_PATH "Tiles/tile_1011.png");
        TileTextures[0b1101] = LoadTexture(RESOURCES_PATH "Tiles/tile_1101.png");
        TileTextures[0b1010] = LoadTexture(RESOURCES_PATH "Tiles/tile_1010.png");
		TileTextures[0b0101] = LoadTexture(RESOURCES_PATH "Tiles/tile_0101.png");
    }

    void UnloadTextures() {
        for (const auto& texture : TileTextures) {
            UnloadTexture(texture);
        }
	}


public:

    TextureManager() { init(); }

    ~TextureManager() {
        UnloadTextures();
    }

    const Texture2D& map_slope_to_texture(int data[4]) const
    {
        int index =
            (data[0] << 3) |
            (data[1] << 2) |
            (data[2] << 1) |
            data[3];

        return TileTextures[index];
    }
 };