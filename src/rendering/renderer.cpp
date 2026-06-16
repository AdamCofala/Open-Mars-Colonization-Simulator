#include "Renderer.h"
#include "TextureManager.h"
#include "world/World.h"
#include "utils/Math.h"
#include "entities/Pipe.h"
#include "structures/WaterMagazine.h"
#include "raylib.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

void Renderer::init() {
    if (camera)      throw std::runtime_error("Camera is already initialized!");
    if (txt_manager) throw std::runtime_error("TextureManager is already initialized!");
    camera = new GameCamera(IsoToScreen(0, 0));
    txt_manager = new TextureManager();
    HideCursor();
}

void Renderer::update(float dt) {
    camera->update(dt);
}

void Renderer::draw(const World& world) {
    BeginMode2D(camera->getCamera());
    RenderTerrain(world.getMap());
    RenderStructures(world.getMap());
    RenderSelected(world.getMap(), r_selectedTileOffset);
    EndMode2D();
}

void Renderer::setSelectedTile(Vector2 tile, Vector2 offset) {
    r_selectedTile = tile;
    r_selectedTileOffset = offset;
}

void Renderer::setSelectedBuildingType(int typeIndex) {
    r_selectedBuildingType = typeIndex;
}

VisibleTileBounds Renderer::getVisibleTileBounds(const Map& map) const {
    Camera2D cam = camera->getCamera();
    int halfW = map.getHalfWidth();
    int halfH = map.getHalfHeight();
    Vector2 corners[4] = {
        GetScreenToWorld2D({0,0},cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), 0}, cam),
        GetScreenToWorld2D({0,(float)GetScreenHeight()}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, cam)
    };
    int minX = INT_MAX, maxX = INT_MIN;
    int minY = INT_MAX, maxY = INT_MIN;
    for (auto& c : corners) {
        Vector2 iso = ScreenToIso(c, &map);
        minX = std::min(minX, (int)floor(iso.x));
        maxX = std::max(maxX, (int)ceil(iso.x));
        minY = std::min(minY, (int)floor(iso.y));
        maxY = std::max(maxY, (int)ceil(iso.y));
    }
    minX -= 2; maxX += 2;
    minY -= 2; maxY += 2;
    minX = std::max(minX, 0);
    maxX = std::min(maxX, halfW * 2);
    minY = std::max(minY, 0);
    maxY = std::min(maxY, halfH * 2);
    return { minX, maxX, minY, maxY };
}

void Renderer::RenderTerrain(const Map& map) {
    auto [minX, maxX, minY, maxY] = getVisibleTileBounds(map);
    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            const Tile& tile = map.getTile(x, y);
            Color base = { 245, 225, 200, 255 };
            int jitter = calc_jitter(x, y);
            base.r = (unsigned char)std::clamp((int)base.r + jitter, 0, 255);
            base.g = (unsigned char)std::clamp((int)base.g + jitter, 0, 255);
            base.b = (unsigned char)std::clamp((int)base.b + jitter, 0, 255);
            RenderIsoTile(tile, IsoToScreen(x, y, &map), base);
        }
    }
}

void Renderer::RenderStructures(const Map& map) {
    std::vector<const Structure*> sorted;
    for (const auto& s : map.getStructures())
        if (s) sorted.push_back(s.get());

    std::sort(sorted.begin(), sorted.end(), [](const Structure* a, const Structure* b) {
        int dA = (a->getX() - a->getXOffset() / 2 + a->getY() - a->getYOffset() / 2);
        int dB = (b->getX() - b->getXOffset() / 2 + b->getY() - b->getYOffset() / 2);
        return dA < dB;
        });

    const bool placingPipe = (r_selectedBuildingType == 1 && r_selectedTile.x >= 0 && r_selectedTile.y >= 0);
    int selX = (int)r_selectedTile.x;
    int selY = (int)r_selectedTile.y;

    for (const auto* s : sorted) {
        Vector2 pos = IsoToScreen(s->getX(), s->getY(), &map);
        const Tile& baseTile = map.getTile(s->getX(), s->getY());
        if (s->isPipe()) {
            const Pipe& pipe = static_cast<const Pipe&>(*s);
            int mask = pipe.getConnectionMask();
            if (placingPipe && std::abs(s->getX() - selX) + std::abs(s->getY() - selY) == 1)
                mask = map.computePipeConnectionMaskWithVirtual(s->getX(), s->getY(), selX, selY);
            RenderPipe(mask, pos, baseTile, WHITE);
        }
        else {
            RenderStruct(*s, pos, baseTile);
        }
    }
}

void Renderer::RenderStruct(const Structure& structure, Vector2 pos, const Tile& baseTile, Color tint) {
    Texture2D sourceRec = txt_manager->StuctureTexturesInfo[structure.getTextureId()];
    float tileBottomY = pos.y - baseTile.getLevel() * HEIGHT_OFFSET + 31.0f;
    float drawX = pos.x - sourceRec.width / 2.0f;
    float drawY = tileBottomY - sourceRec.height;
    DrawTexture(sourceRec, (int)drawX, (int)drawY, tint);
}

void Renderer::RenderPipe(int mask, Vector2 pos, const Tile& baseTile, Color tint) {
    Rectangle sourceRec = txt_manager->PipeTexturesInfo[mask];
    if (sourceRec.width == 0) {
        sourceRec = txt_manager->PipeTexturesInfo[0];
    }
    float drawX = pos.x - sourceRec.width / 2.0f;
    float drawY = pos.y - baseTile.getLevel() * HEIGHT_OFFSET - (sourceRec.height - 31.0f);
    DrawTextureRec(txt_manager->pipe_atlas, sourceRec, { drawX, drawY }, tint);
}

int Renderer::getPipeConnectionBit(int fromX, int fromY, int toX, int toY) const {
    int dx = toX - fromX;
    int dy = toY - fromY;
    int i = -1;
    if (dx == 0 && dy == -1) i = (int)Direction::NORTH_EAST;
    else if (dx == 1 && dy == 0)  i = (int)Direction::SOUTH_EAST;
    else if (dx == 0 && dy == 1)  i = (int)Direction::SOUTH_WEST;
    else if (dx == -1 && dy == 0) i = (int)Direction::NORTH_WEST;
    if (i >= 0) return (8 >> i);
    return 0;
}

void Renderer::RenderSelected(const Map& map, Vector2 offset, Color tint) {
    if (r_selectedTile.x < 0 || r_selectedTile.y < 0) return;
    int startX = (int)r_selectedTile.x;
    int startY = (int)r_selectedTile.y;

    // Mapowanie indeksu z GUI na StructureType
    StructureType stype = StructureType::Pipe; // domyślnie, gdyby coś poszło nie tak
    switch (r_selectedBuildingType) {
    case 0: stype = StructureType::SolarPanel; break;
    case 1: stype = StructureType::Pipe;       break;
    case 2: stype = StructureType::IceMelter;  break;
    case 3: stype = StructureType::WaterMagazine; break;
    default: break;
    }

    if (r_selectedTool == Gui::SelectedTool::Demolish) {
        const Tile& tile = map.getTile(startX, startY);
        const Structure* structure = tile.getStructure();
        if (structure) {
            Vector2 pos = IsoToScreen(structure->getX(), structure->getY(), &map);
            const Tile& baseTile = map.getTile(structure->getX(), structure->getY());
            if (structure->isPipe()) {
                const Pipe* pipe = static_cast<const Pipe*>(structure);
                int mask = pipe->getConnectionMask();
                RenderPipe(mask, pos, baseTile, Fade(RED, 0.6f));
            }
            else {
                RenderStruct(*structure, pos, baseTile, Fade(RED, 0.6f));
            }
        }
        return;
    }

    bool valid_placement = map.canPlaceStructure(startX, startY, (int)offset.x, (int)offset.y, stype);

    if (r_selectedBuildingType >= 0) {
        Vector2 pos = IsoToScreen(startX, startY, &map);
        const Tile& baseTile = map.getTile(startX, startY);
        Color structureTint;

        if (r_selectedBuildingType == 1) {
            int mask = map.computePipeConnectionMask(startX, startY);
            std::vector<int>slope = baseTile.getSlopeData();
            bool special_valic_placement = valid_placement ||
                slope == std::vector<int>{1, 1, 0, 0} ||
                slope == std::vector<int>{1, 0, 0, 1} ||
                slope == std::vector<int>{0, 1, 1, 0} ||
                slope == std::vector<int>{0, 0, 1, 1};

            structureTint = special_valic_placement ? Fade(WHITE, 0.5f) : Fade(RED, 0.5f);
            RenderPipe(mask, pos, baseTile, structureTint);
        }
        else {
            std::unique_ptr<Structure> tempStructure;
            switch (r_selectedBuildingType) {
            case 0: tempStructure = std::make_unique<SolarPanel>(0, 0); break;
            case 2: tempStructure = std::make_unique<IceMelter>(0, 0);  break;
            case 3: tempStructure = std::make_unique<WaterMagazine>(0, 0); break;
            default: break;
            }
            if (tempStructure) {
                structureTint = valid_placement ? Fade(WHITE, 0.5f) : Fade(RED, 0.5f);
                RenderStruct(*tempStructure, pos, baseTile, structureTint);
            }
        }
    }
}

GameCamera& Renderer::getGameCamera() const {
    if (!camera) throw std::runtime_error("Camera is not initialized!");
    return *camera;
}

void Renderer::shutdown() {
    if (camera) { delete camera; camera = nullptr; }
    if (txt_manager) { delete txt_manager; txt_manager = nullptr; }
}

Vector2 Renderer::IsoToScreen(float x, float y, const Map* map) const {
    if (map) {
        x -= map->getHalfWidth();
        y -= map->getHalfHeight();
    }
    return { (x - y) * (tileSize / 2.0f), (x + y) * (tileSize / 4.0f) };
}

Vector2 Renderer::ScreenToIso(Vector2 pos, const Map* map) const {
    float isoX = (pos.x / (tileSize / 2.0f) + pos.y / (tileSize / 4.0f)) / 2.0f;
    float isoY = (pos.y / (tileSize / 4.0f) - pos.x / (tileSize / 2.0f)) / 2.0f;
    if (map) {
        isoX += map->getHalfWidth();
        isoY += map->getHalfHeight();
    }
    return { isoX, isoY };
}

void Renderer::RenderIsoTile(const Tile& tile, Vector2 pos, Color tint) const {
    auto slope_vec = tile.getSlopeData();
    int n_raised = slope_vec[0];
    Rectangle rect;
    Texture2D atlas;
    if (tile.getType() == TileType::Ice) {
        rect = txt_manager->map_ice_to_texture(slope_vec.data());
        if (rect.width == 0) {
            rect = txt_manager->map_slope_to_texture(slope_vec.data());
            atlas = txt_manager->tile_atlas;
        }
        else {
            atlas = txt_manager->ice_atlas;
        }
    }
    else {
        rect = txt_manager->map_slope_to_texture(slope_vec.data());
        atlas = txt_manager->tile_atlas;
    }
    float drawX = pos.x - rect.width / 2.0f;
    float drawY = pos.y - n_raised * HEIGHT_OFFSET - tile.getLevel() * HEIGHT_OFFSET;
    DrawTextureRec(atlas, rect, { drawX, drawY }, tint);
}

void Renderer::drawCursor() const {
    if (txt_manager && txt_manager->cursor.id != 0) {
        if (IsCursorOnScreen()) {
            Vector2 mousePos = GetMousePosition();
            DrawTexture(txt_manager->cursor, (int)mousePos.x, (int)mousePos.y, WHITE);
        }
    }
}