#include "Renderer.h"

#include "TextureManager.h"
#include "world/World.h"
#include "utils/Math.h"
#include "entities/Pipe.h"
#include "raylib.h"

#include <stdexcept>
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

void Renderer::setSelectedTile(Vector2 tile, Vector2 offset)
{
    r_selectedTile = tile;
    r_selectedTileOffset = offset;
}

void Renderer::setSelectedBuildingType(int typeIndex)
{
	r_selectedBuildingType = typeIndex;
}

VisibleTileBounds Renderer::getVisibleTileBounds(const Map& map) const {
    Camera2D cam = camera->getCamera();
    int halfW = map.getHalfWidth();
    int halfH = map.getHalfHeight();

    // Get screen corners in world coordinates
    Vector2 corners[4] = {
        GetScreenToWorld2D({0,0},cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), 0}, cam),
        GetScreenToWorld2D({0,(float)GetScreenHeight()}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, cam)
    };

    int minX = INT_MAX, maxX = INT_MIN;
    int minY = INT_MAX, maxY = INT_MIN;

    // Find min/max bounds in isometric grid coordinates
    for (auto& c : corners) {
        Vector2 iso = ScreenToIso(c, &map);
        minX = std::min(minX, (int)floor(iso.x));
        maxX = std::max(maxX, (int)ceil(iso.x));
        minY = std::min(minY, (int)floor(iso.y));
        maxY = std::max(maxY, (int)ceil(iso.y));
    }

    // Margin to prevent cutting tiles near edges
    minX -= 2; maxX += 2;
    minY -= 2; maxY += 2;

    // Constrain visible range to actual map bounds
    minX = std::max(minX, 0);
    maxX = std::min(maxX, halfW * 2);
    minY = std::max(minY, 0);
    maxY = std::min(maxY, halfH * 2);

    return {minX, maxX, minY, maxY};
}


void Renderer::RenderTerrain(const Map& map) {
    auto [minX, maxX, minY, maxY] = getVisibleTileBounds(map);

    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            const Tile& tile = map.getTile(x, y);

            if (tile.isOccupied()) {
                DrawIsoTile(tile, IsoToScreen(x, y, &map), Fade(RED, 0.5f));
                continue;
            }

            Color base = { 245, 225, 200, 255 };
            int jitter = calc_jitter(x, y);

            base.r = (unsigned char)std::clamp((int)base.r + jitter, 0, 255);
            base.g = (unsigned char)std::clamp((int)base.g + jitter, 0, 255);
            base.b = (unsigned char)std::clamp((int)base.b + jitter, 0, 255);

            DrawIsoTile(tile, IsoToScreen(x, y, &map), base);
        }
    }
}

void Renderer::RenderStructures(const Map& map) {
    std::vector<const Structure*> sortedStructures;
    for (const auto& structurePtr : map.getStructures()) {
        if (structurePtr != nullptr) {
            sortedStructures.push_back(structurePtr.get());
        }
    }

    std::sort(sortedStructures.begin(), sortedStructures.end(), [](const Structure* a, const Structure* b) {
        return (a->getX() + a->getY()) < (b->getX() + b->getY());
        });

    // Czy jesteśmy w trybie podglądu rury?
    const bool placingPipe = (r_selectedBuildingType == 1 &&
        r_selectedTile.x >= 0 && r_selectedTile.y >= 0);
    int selX = (int)r_selectedTile.x;
    int selY = (int)r_selectedTile.y;

    for (const auto* s : sortedStructures) {
        Vector2 pos = IsoToScreen(s->getX(), s->getY(), &map);
        const Tile& baseTile = map.getTile(s->getX(), s->getY());

        if (s->isPipe()) {
            const Pipe& pipe = static_cast<const Pipe&>(*s);
            int mask = pipe.getConnectionMask();

            if (placingPipe && std::abs(s->getX() - selX) + std::abs(s->getY() - selY) == 1) {
                mask = map.computePipeConnectionMaskWithVirtual(s->getX(), s->getY(), selX, selY);
            }

            Rectangle sourceRec = txt_manager->PipeTexturesInfo[mask];
            if (sourceRec.width == 0) {
                sourceRec = txt_manager->PipeTexturesInfo[0];
            }

            float drawX = pos.x - sourceRec.width / 2.0f;
            float drawY = pos.y - baseTile.getLevel() * HEIGHT_OFFSET - (sourceRec.height - 31.0f);

            Color tint = WHITE;
            DrawTextureRec(txt_manager->pipe_atlas, sourceRec, { drawX, drawY }, tint);
        }
        else {
            RenderStruct(*s, pos, baseTile);
        }
    }
}

void Renderer::RenderStruct(const Structure& structure, Vector2 pos, const Tile& baseTile, Color tint)
{
    Texture2D sourceRec = txt_manager->StuctureTexturesInfo[structure.getTextureId()];
    float tileBottomY = pos.y - baseTile.getLevel() * HEIGHT_OFFSET + 31.0f;
    float drawX = pos.x - sourceRec.width / 2.0f;
    float drawY = tileBottomY - sourceRec.height;
    DrawTexture(sourceRec, (int)drawX, (int)drawY, tint);
}

void Renderer::RenderPipe(const Pipe& p, Vector2 pos, const Tile& baseTile, Color tint) {
    int mask = p.getConnectionMask();
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
    if (dx == 0 && dy == -1) i = (int)Direction::NORTH_EAST; // 0
    else if (dx == 1 && dy == 0)  i = (int)Direction::SOUTH_EAST; // 1
    else if (dx == 0 && dy == 1)  i = (int)Direction::SOUTH_WEST; // 2
    else if (dx == -1 && dy == 0) i = (int)Direction::NORTH_WEST; // 3

    if (i >= 0) return (8 >> i);
    return 0;
}

void Renderer::RenderSelected(const Map& map, Vector2 offset, Color tint) {
    if (r_selectedTile.x < 0 || r_selectedTile.y < 0) return;

    int startX = (int)r_selectedTile.x;
    int startY = (int)r_selectedTile.y;

    bool valid_placement = map.canPlaceStructure(startX, startY, (int)offset.x, (int)offset.y);

    if (r_selectedBuildingType >= 0) {
        std::unique_ptr<Structure> tempStructure;

        switch (r_selectedBuildingType) {
			case 0: tempStructure = std::make_unique<SolarPanel>(0, 0); break;
			case 1: tempStructure = std::make_unique<Pipe>(0, 0);       break;
			case 2: tempStructure = std::make_unique<IceMelter>(0, 0);  break;
			default: break;
        }

        if (tempStructure) {
            Vector2 pos = IsoToScreen(startX, startY, &map);
            const Tile& baseTile = map.getTile(startX, startY);
            Color structureTint = valid_placement ? Fade(WHITE, 0.5f) : Fade(RED, 0.5f);

            if (tempStructure->isPipe()) {
                Pipe* tempPipe = static_cast<Pipe*>(tempStructure.get());
                int mask = map.computePipeConnectionMask(startX, startY);
                tempPipe->setConnectionMask(mask);
                RenderPipe(*tempPipe, pos, baseTile, structureTint);
            }
            else {
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
    if (camera) {
        delete camera;
        camera = nullptr;
    }
    if (txt_manager) {
        delete txt_manager;
        txt_manager = nullptr;
    }
}

Vector2 Renderer::IsoToScreen(float x, float y, const Map* map) const {
    // If a map is provided, adjust coordinates to center the grid based on the map's dimensions
    if (map) {
        x -= map->getHalfWidth();
        y -= map->getHalfHeight();
    }

    return {
        (x - y) * (tileSize / 2.0f),
        (x + y) * (tileSize / 4.0f)
    };
}

Vector2 Renderer::ScreenToIso(Vector2 pos, const Map* map) const {
    float isoX = (pos.x / (tileSize / 2.0f) + pos.y / (tileSize / 4.0f)) / 2.0f;
    float isoY = (pos.y / (tileSize / 4.0f) - pos.x / (tileSize / 2.0f)) / 2.0f;

    // If a map is provided, convert back to grid coordinates by adding half offsets
    if (map) {
        isoX += map->getHalfWidth();
        isoY += map->getHalfHeight();
    }

    return { isoX, isoY };
}

void Renderer::DrawIsoTile(const Tile& tile, Vector2 pos, Color tint) const {
    auto slope_vec = tile.getSlopeData();
    int n_raised = slope_vec[0];

    Rectangle rect;
    Texture2D atlas;

    if (tile.getType() == TileType::Ice) {
        rect = txt_manager->map_ice_to_texture(slope_vec.data());
        atlas = txt_manager->ice_atlas;
    } else {
        rect = txt_manager->map_slope_to_texture(slope_vec.data());
        atlas = txt_manager->tile_atlas;
    }

    float drawX = pos.x - rect.width / 2.0f;
    float drawY = pos.y
        - n_raised * HEIGHT_OFFSET
        - tile.getLevel() * HEIGHT_OFFSET;

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