#include "Renderer.h"
#include "raylib.h"
#include "TextureManager.h"
#include "world/World.h"
#include <stdexcept>
#include <algorithm>

void Renderer::init() {
    if (camera)      throw std::runtime_error("Camera is already initialized!");
    if (txt_manager) throw std::runtime_error("TextureManager is already initialized!");
    camera = new GameCamera(IsoToScreen(0, 0));
    txt_manager = new TextureManager();
}

void Renderer::update(float dt) {
    camera->update(dt);
}

void Renderer::draw(const World& world) {
    BeginMode2D(camera->getCamera());

    RenderTerrain(world.getMap());
    RenderSelected(world.getMap());

    EndMode2D();
    DrawFPS(10, 40);
}

VisibleTileBounds Renderer::getVisibleTileBounds(int mapWidth, int mapHeight) const {
    Camera2D cam = camera->getCamera();
    int halfW = mapWidth / 2;
    int halfH = mapHeight / 2;

    Vector2 corners[4] = {
        GetScreenToWorld2D({0,                      0},                      cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), 0},                      cam),
        GetScreenToWorld2D({0,                      (float)GetScreenHeight()}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, cam)
    };

    int minX = INT_MAX, maxX = INT_MIN;
    int minY = INT_MAX, maxY = INT_MIN;
    for (auto& c : corners) {
        Vector2 iso = ScreenToIso(c);
        minX = std::min(minX, (int)floor(iso.x));
        maxX = std::max(maxX, (int)ceil(iso.x));
        minY = std::min(minY, (int)floor(iso.y));
        maxY = std::max(maxY, (int)ceil(iso.y));
    }

    // Margines ¿eby nie ucinaæ kafelków przy krawêdzi
    minX -= 2; maxX += 2;
    minY -= 2; maxY += 2;

    // Przytnij do obszaru mapy (iso space: [-halfW, halfW])
    minX = std::max(minX, -halfW);
    maxX = std::min(maxX, halfW);
    minY = std::max(minY, -halfH);
    maxY = std::min(maxY, halfH);

    // Przelicz na grid (dodaj half offset)
    return {
        minX + halfW,
        maxX + halfW,
        minY + halfH,
        maxY + halfH
    };
}

void Renderer::RenderTerrain(const Map& map) {
    int halfW = map.getWidth() / 2;
    int halfH = map.getHeight() / 2;
    auto [minX, maxX, minY, maxY] = getVisibleTileBounds(map.getWidth(), map.getHeight());

    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            const Tile& tile = map.getTile(x, y);
            DrawIsoTile(tile, IsoToScreen(x - halfW, y - halfH));
        }
    }
}

void Renderer::RenderSelected(const Map& map) {
    if (m_selectedTile.x >= 0 && m_selectedTile.y >= 0) {
        Vector2 isoPos = IsoToScreen(
            m_selectedTile.x - map.getWidth() / 2,
            m_selectedTile.y - map.getHeight() / 2
        );
        const Tile& tile = map.getTile(m_selectedTile.x, m_selectedTile.y);
        DrawIsoTile(tile, isoPos, Fade(SKYBLUE, 0.5f));
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

Vector2 Renderer::IsoToScreen(int x, int y) const {
    return {
        (x - y) * (tileSize / 2.0f),
        (x + y) * (tileSize / 4.0f)
    };
}

Vector2 Renderer::ScreenToIso(Vector2 pos) const {
    float isoX = (pos.x / (tileSize / 2.0f) + pos.y / (tileSize / 4.0f)) / 2.0f;
    float isoY = (pos.y / (tileSize / 4.0f) - pos.x / (tileSize / 2.0f)) / 2.0f;
    return { isoX, isoY };
}

void Renderer::DrawIsoTile(const Tile& tile, Vector2 pos, Color tint) const {
    auto slope_vec = tile.getSlopeData();
    int n_raised = slope_vec[0];

    const Rectangle& rect = txt_manager->map_slope_to_texture(slope_vec.data());
    float drawX = pos.x - rect.width / 2.0f;
    float drawY = pos.y
        - n_raised * HEIGHT_OFFSET
        - tile.getLevel() * HEIGHT_OFFSET;

    DrawTextureRec(txt_manager->tile_atlas, rect, { drawX, drawY }, tint);
}