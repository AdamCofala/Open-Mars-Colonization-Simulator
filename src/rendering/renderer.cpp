#include "Renderer.h"
#include "raylib.h"
#include "TextureManager.h"
#include "world/World.h"
#include <stdexcept>

void Renderer::init() {
    // Load any textures, shaders, etc. here

	if (camera) throw std::runtime_error("Camera is already initialized!");
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
       
    EndMode2D();

    DrawFPS(10, 40);
}

void Renderer::RenderTerrain(const Map& map) {

	// Render only tiles that are visible on the screen, based on camera position and zoom
    Camera2D cam = camera->getCamera();
    int halfW = map.getWidth() / 2;
    int halfH = map.getHeight() / 2;

    Vector2 corners[4] = {
        GetScreenToWorld2D({0, 0}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), 0}, cam),
        GetScreenToWorld2D({0, (float)GetScreenHeight()}, cam),
        GetScreenToWorld2D({(float)GetScreenWidth(), (float)GetScreenHeight()}, cam)
    };

    int minX = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
    for (auto& c : corners) {
        Vector2 iso = ScreenToIso(c);
        minX = std::min(minX, (int)floor(iso.x));
        maxX = std::max(maxX, (int)ceil(iso.x));
        minY = std::min(minY, (int)floor(iso.y));
        maxY = std::max(maxY, (int)ceil(iso.y));
    }

    minY -= 2; minX -= 2;
    maxY += 2; maxX += 2;

    minX = std::max(minX, -halfW);
    maxX = std::min(maxX, halfW);
    minY = std::max(minY, -halfH);
    maxY = std::min(maxY, halfH);

    for (int y = minY; y < maxY; y++) {
        for (int x = minX; x < maxX; x++) {
            const Tile& tile = map.getTile(x + halfW, y + halfH);
            Vector2 pos = IsoToScreen(x, y);
            DrawIsoTile(tile, pos);
        }
    }
}

void Renderer::shutdown() {
}

Vector2 Renderer::IsoToScreen(int x, int y) const {
    Vector2 out;
    out.x = (x - y) * (tileSize / 2.0f);
    out.y = (x + y) * (tileSize / 4.0f);
    return out;
}

Vector2 Renderer::ScreenToIso(Vector2 pos) const {
    float x = pos.x;
    float y = pos.y;

    float isoX = (x / (tileSize / 2.0f) + y / (tileSize / 4.0f)) / 2.0f;
    float isoY = (y / (tileSize / 4.0f) - (x / (tileSize / 2.0f))) / 2.0f;

    return { isoX, isoY };
}

void Renderer::DrawIsoTile(const Tile& tile, Vector2 pos) const {
    auto slope_vec = tile.getSlopeData();
    const Texture2D* txt = &txt_manager->map_slope_to_texture(slope_vec.data());

    // N vertex tile'a zawsze na pos.y
    // Ka¿da tekstura ma N na swoim top pikselu (y=0)
    // Ale N vertex w world space zale¿y od slope - ile rogów jest "wysoko"
    // slope_data[0]=N, ile z N/W/E/S rogów jest wy¿ej
    auto& s = slope_vec;
    int n_raised = s[0]; // czy N róg jest wy¿ej o 1 level

    int drawX = (int)pos.x - txt->width / 2;
    int drawY = (int)pos.y
        - n_raised * HEIGHT_OFFSET          // N wy¿ej = przesuñ w górê
        - tile.getLevel() * HEIGHT_OFFSET;

    DrawTexture(*txt, drawX, drawY, WHITE);
}