#pragma once
#include <array>
#include "raylib.h"
#include <string>
#include <unordered_map>

class TextureManager
{
public:
    std::array<Rectangle, 16> TileTexturesInfo;
    std::array<Rectangle, 16> PipeTexturesInfo;
    std::array<Rectangle, 16> IceTexturesInfo;

    Texture2D tile_atlas{};
    Texture2D pipe_atlas{};
    Texture2D ice_atlas{};
    std::unordered_map<std::string, Texture2D> StuctureTexturesInfo;
    Texture2D cursor{};

    TextureManager() { init(); }
    ~TextureManager() { unloadTextures(); }

    // Rule of Five 
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&& other) noexcept
        : TileTexturesInfo(other.TileTexturesInfo)
        , PipeTexturesInfo(other.PipeTexturesInfo)
        , IceTexturesInfo(other.IceTexturesInfo)
        , tile_atlas(other.tile_atlas)
        , pipe_atlas(other.pipe_atlas)
        , ice_atlas(other.ice_atlas)
        , StuctureTexturesInfo(std::move(other.StuctureTexturesInfo))
        , cursor(other.cursor)
    {
        other.tile_atlas = {};
        other.pipe_atlas = {};
        other.ice_atlas = {};
        other.cursor = {};
    }
    TextureManager& operator=(TextureManager&& other) noexcept {
        if (this != &other) {
            unloadTextures();
            TileTexturesInfo = other.TileTexturesInfo;
            PipeTexturesInfo = other.PipeTexturesInfo;
            IceTexturesInfo = other.IceTexturesInfo;
            tile_atlas = other.tile_atlas;
            pipe_atlas = other.pipe_atlas;
            ice_atlas = other.ice_atlas;
            StuctureTexturesInfo = std::move(other.StuctureTexturesInfo);
            cursor = other.cursor;
            other.tile_atlas = {};
            other.pipe_atlas = {};
            other.ice_atlas = {};
            other.cursor = {};
        }
        return *this;
    }

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

private:
    void init() {
        tile_atlas = LoadTexture(RESOURCES_PATH "tile_atlas.png");
        pipe_atlas = LoadTexture(RESOURCES_PATH "pipe_atlas.png");
        ice_atlas = LoadTexture(RESOURCES_PATH "ice_atlas.png");
        cursor = LoadTexture(RESOURCES_PATH "cursor.png");

        StuctureTexturesInfo["solar_panels"] = LoadTexture(RESOURCES_PATH "solar_panels.png");
        StuctureTexturesInfo["ice_melter"] = LoadTexture(RESOURCES_PATH "ice_melter.png");
        StuctureTexturesInfo["pipe"] = LoadTexture(RESOURCES_PATH "pipe.png");
        StuctureTexturesInfo["water_magazine"] = LoadTexture(RESOURCES_PATH "water_magazine.png");

        load_tiles_textures();
        load_pipes_textures();
        load_ice_textures();
    }

    void load_tiles_textures() {
        TileTexturesInfo = {
            Rectangle{0,   0, 64, 31},
            Rectangle{65,  0, 64, 31},
            Rectangle{130, 0, 64, 23},
            Rectangle{195, 0, 64, 23},
            Rectangle{260, 0, 64, 31},
            Rectangle{325, 0, 64, 31},
            Rectangle{390, 0, 64, 23},
            Rectangle{455, 0, 64, 23},
            Rectangle{520, 0, 64, 39},
            Rectangle{585, 0, 64, 39},
            Rectangle{650, 0, 64, 31},
            Rectangle{715, 0, 64, 31},
            Rectangle{780, 0, 64, 39},
            Rectangle{845, 0, 64, 39},
            Rectangle{910, 0, 64, 31},
            Rectangle{0, 0, 0, 0}
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

    void unloadTextures() {
        if (tile_atlas.id != 0) UnloadTexture(tile_atlas);
        if (pipe_atlas.id != 0) UnloadTexture(pipe_atlas);
        if (ice_atlas.id != 0)  UnloadTexture(ice_atlas);
        if (cursor.id != 0)     UnloadTexture(cursor);
        for (auto& pair : StuctureTexturesInfo) {
            if (pair.second.id != 0) UnloadTexture(pair.second);
        }
        StuctureTexturesInfo.clear();
    }
};