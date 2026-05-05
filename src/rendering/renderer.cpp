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

    // TODO : Add culling render - rendering only what we see, huge optimization!
   
    int halfW = map.getWidth() / 2;
    int halfH = map.getHeight() / 2;

    for (int y = -halfH; y < halfH; y++) {
        for (int x = -halfW; x < halfW; x++) {

            const Tile& tile = map.getTile(x + halfW, y + halfH);

            Vector2 pos = IsoToScreen(x, y); 

            DrawIsoTile(tile, pos);

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

void Renderer::DrawIsoTile(const Tile& tile, Vector2 pos) const {
    auto slope_vec = tile.getSlopeData();
    Texture2D& txt = txt_manager.map_slope_to_texture(slope_vec.data());

    // N vertex tile'a zawsze na pos.y
    // Ka¿da tekstura ma N na swoim top pikselu (y=0)
    // Ale N vertex w world space zale¿y od slope - ile rogów jest "wysoko"
    // slope_data[0]=N, ile z N/W/E/S rogów jest wy¿ej
    auto& s = slope_vec;
    int n_raised = s[0]; // czy N róg jest wy¿ej o 1 level

    int drawX = (int)pos.x - txt.width / 2;
    int drawY = (int)pos.y
        - n_raised * HEIGHT_OFFSET          // N wy¿ej = przesuñ w górê
        - tile.getLevel() * HEIGHT_OFFSET;

    DrawTexture(txt, drawX, drawY, WHITE);
}