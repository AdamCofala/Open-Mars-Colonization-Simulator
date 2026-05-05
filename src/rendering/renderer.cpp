#include "Renderer.h"
#include "raylib.h"
#include "TextureManager.h"
#include <omp.h>

void Renderer::init() {
    // Load any textures, shaders, etc. here
}

void Renderer::update(float dt) {
	camera.update(dt);
}

void Renderer::draw(const Map& map) {
    BeginMode2D(camera.getCamera());


    Vector2 center = IsoToScreen(0, 0);

    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            DrawIsoTile(map.getTile(x, y));
        }
    }

	EndMode2D();

    DrawText("Mars TTD - Isometric Prototype", 10, 10, 20, WHITE);
    DrawFPS(10, 40);

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

void Renderer::DrawIsoTile(const Tile& tile) const {

	auto slope_vec= tile.getSlopeData();
	int* slope_data = new int[4];
    for (int i = 0; i < 4; ++i) {
        slope_data[i] = slope_vec[i];
	}

	Texture2D txt = txt_manager.map_slope_to_texture(slope_data);
    Vector2 pos = IsoToScreen(tile.getX(), tile.getY());
    DrawTexture(txt, pos.x, pos.y - tile.getLevel()*16, WHITE);

    delete[] slope_data;
}

