#include "Renderer.h"

#include "TextureManager.h"
#include "world/World.h"
#include "utils/Math.h"

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

VisibleTileBounds Renderer::getVisibleTileBounds(const Map& map) const {
    Camera2D cam = camera->getCamera();
    int halfW = map.getHalfWidth();
    int halfH = map.getHalfHeight();

    // Get screen corners in world coordinates
    Vector2 corners[4] = {
        GetScreenToWorld2D({0,                      0},                      cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), 0},                      cam),
        GetScreenToWorld2D({0,                      (float)GetScreenHeight()}, cam),
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

    return {
        minX,
        maxX,
        minY,
        maxY
    };
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
    auto structures = map.getStructures();

    // Painter's algorithm: sort items by isometric depth (y + x) safely
    std::sort(structures.begin(), structures.end(), [](const Structure& a, const Structure& b) {
        return (a.getX() + a.getY()) < (b.getX() + b.getY());
    });

    for (const auto& s : structures) {
        Vector2 pos = IsoToScreen(s.getX(), s.getY(), &map);

        const Tile& baseTile = map.getTile(s.getX(), s.getY());
        int n_raised = baseTile.getSlopeData()[0];

        // For a flat tile, n_raised is 0. Its bottom point (south vertex) is at +31 relative to its drawn Y.
        float tileBottomY = pos.y - n_raised * HEIGHT_OFFSET - baseTile.getLevel() * HEIGHT_OFFSET + 31.0f;

        // Center the structure image horizontally, and set its bottom at tileBottomY
        float drawX = pos.x - txt_manager->solar_panels.width / 2.0f;
        float drawY = tileBottomY - txt_manager->solar_panels.height + 1.0f; // +1 if to overlap perfectly

        DrawTexture(txt_manager->solar_panels, (int)drawX, (int)drawY, WHITE);
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

	auto [visMinX, visMaxX, visMinY, visMaxY] = getVisibleTileBounds(map);

	// Clip to visible tile bounds to avoid unnecessary rendering calculations
	minX = std::max(minX, visMinX);
	minY = std::max(minY, visMinY);
	maxX = std::min(maxX, visMaxX - 1);
	maxY = std::min(maxY, visMaxY - 1);

	std::vector<std::pair<Tile, Vector2>> selectedTiles;

	bool isSelectionOnly = (offset.x == 1 && offset.y == 1);
	bool valid_placement = map.canPlaceStructure(startX, startY, (int)offset.x, (int)offset.y);

	if (!isSelectionOnly) {
		Vector2 pos = IsoToScreen(startX, startY, &map);
		const Tile& baseTile = map.getTile(startX, startY);
		int n_raised = baseTile.getSlopeData()[0];

		// Calculate the bottom point of the structure based on the tile elevation
		float tileBottomY = pos.y - n_raised * HEIGHT_OFFSET - baseTile.getLevel() * HEIGHT_OFFSET + 31.0f;

		float drawX = pos.x - txt_manager->solar_panels.width / 2.0f;
		float drawY = tileBottomY - txt_manager->solar_panels.height + 1.0f;

		Color structureTint = valid_placement ? Fade(WHITE, 0.5f) : Fade(RED, 0.5f);
		DrawTexture(txt_manager->solar_panels, (int)drawX, (int)drawY, structureTint);
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

    const Rectangle& rect = txt_manager->map_slope_to_texture(slope_vec.data());
    float drawX = pos.x - rect.width / 2.0f;
    float drawY = pos.y
        - n_raised * HEIGHT_OFFSET
        - tile.getLevel() * HEIGHT_OFFSET;

    DrawTextureRec(txt_manager->tile_atlas, rect, { drawX, drawY }, tint);
}

void Renderer::drawCursor() const {
    if (txt_manager && txt_manager->cursor.id != 0) {
        if (IsCursorOnScreen()) {
            Vector2 mousePos = GetMousePosition();
            DrawTexture(txt_manager->cursor, (int)mousePos.x, (int)mousePos.y, WHITE);
        }
    }
}