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
    RenderSelected(world.getMap(), r_selectedTileOffset);

    EndMode2D();
}

void Renderer::setSelectedTile(Vector2 tile, Vector2 offset)
{
    r_selectedTile = tile;
    r_selectedTileOffset = offset;
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

    // Margines żeby nie ucinać kafelków przy krawędzi
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

            if (tile.isOccupied()) {
                DrawIsoTile(tile, IsoToScreen(x - halfW, y - halfH), Fade(RED, 0.5f));
                continue;
            }

            Color base = { 245, 225, 200, 255};   

            // Jitter ±6
            int noise = ((x * 1619 + y * 31337) ^ (x + y * 7)) & 0xFF;
            int jitter = (noise % 13) - 6;

            base.r = (unsigned char)std::clamp((int)base.r + jitter, 0, 255);
            base.g = (unsigned char)std::clamp((int)base.g + jitter, 0, 255);
            base.b = (unsigned char)std::clamp((int)base.b + jitter, 0, 255);
            DrawIsoTile(tile, IsoToScreen(x - halfW, y - halfH), base);
        }
    }
}

void Renderer::RenderSelected(const Map& map, Vector2 offset, Color tint) {
    if (r_selectedTile.x < 0 || r_selectedTile.y < 0) return;

    int startX = (int)r_selectedTile.x;
    int startY = (int)r_selectedTile.y;
    int endX = startX - ((int)offset.x - 1);
    int endY = startY - ((int)offset.y - 1);

    int minX = std::min(startX, endX);
    int maxX = std::max(startX, endX);
    int minY = std::min(startY, endY);
    int maxY = std::max(startY, endY);

    auto [visMinX, visMaxX, visMinY, visMaxY] = getVisibleTileBounds(map.getWidth(), map.getHeight());


    // Przytnij do widocznych granic
    minX = std::max(minX, visMinX);
    minY = std::max(minY, visMinY);
    maxX = std::min(maxX, visMaxX - 1);
    maxY = std::min(maxY, visMaxY - 1);

	std::vector<std::pair<Tile, Vector2>> selectedTiles;

	bool valid_placement = map.canPlaceStructure(startX, startY, (int)offset.x, (int)offset.y);

	for (int y = minY; y <= maxY; y++) {
		for (int x = minX; x <= maxX; x++) {
			Vector2 isoPos = IsoToScreen(x - map.getWidth() / 2, y - map.getHeight() / 2);
			const Tile& tile = map.getTile(x, y);
			selectedTiles.push_back({tile, isoPos});
		}
	}

	tint = valid_placement ? Fade(GREEN, 0.5f) : Fade(RED, 0.5f);

    for (const auto& [tile, pos] : selectedTiles) {
        DrawIsoTile(tile, pos, tint);
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