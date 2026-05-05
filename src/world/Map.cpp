#include "Map.h"
#include "utils/PerlinNoise.h"
#include <stdexcept>

void Map::init(int w, int h) {

    if (w <= 0 || h <= 0) {
        throw std::invalid_argument("Map dimensions must be positive integers.");
    }

    width = w;
    height = h;

    tiles.resize(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = y * width + x;
            tiles[i].init(x, y, nullptr);
        }
    }
}


static void clampSlopes(std::vector<std::vector<int>>& vertex, int width, int height) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (int y = 0; y <= height; y++) {
            for (int x = 0; x <= width; x++) {
                int dx[] = { 1, -1, 0, 0 };
                int dy[] = { 0, 0, 1, -1 };
                for (int d = 0; d < 4; d++) {
                    int nx = x + dx[d], ny = y + dy[d];
                    if (nx < 0 || nx > width || ny < 0 || ny > height) continue;
                    if (vertex[y][x] - vertex[ny][nx] > 1) {
                        vertex[y][x] = vertex[ny][nx] + 1;
                        changed = true;
                    }
                }
            }
        }
    }
}

static void fixSaddles(std::vector<std::vector<int>>& vertex, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int& vN = vertex[y][x];
            int& vE = vertex[y][x + 1];
            int& vS = vertex[y + 1][x + 1];
            int& vW = vertex[y + 1][x];

            int minH = std::min({ vN, vE, vS, vW });
            int mask = ((vN > minH) << 3) | ((vE > minH) << 2) |
                ((vS > minH) << 1) | (vW > minH);

            if (mask == 0b0101) { vN = minH + 1; vS = minH + 1; }
            if (mask == 0b1010) { vE = minH + 1; vW = minH + 1; }
        }
    }
}

void Map::generateTerrain() {
    PerlinNoise perlin(67);
    const float scale = 0.04f;
    const int maxLevel = 10;

    std::vector<std::vector<int>> vertex(height + 1, std::vector<int>(width + 1));
    for (int y = 0; y <= (int)height; y++)
        for (int x = 0; x <= (int)width; x++)
            vertex[y][x] = (int)(perlin.noise(x * scale, y * scale) * maxLevel);

    clampSlopes(vertex, width, height);
    fixSaddles(vertex, width, height);
    clampSlopes(vertex, width, height);

    for (int y = 0; y < (int)height; y++) {
        for (int x = 0; x < (int)width; x++) {
            int vN = vertex[y][x], vE = vertex[y][x + 1];
            int vS = vertex[y + 1][x + 1], vW = vertex[y + 1][x];
            int minH = std::min({ vN, vE, vS, vW });

            int slopeData[4] = {
                vN > minH ? 1 : 0,
                vE > minH ? 1 : 0,
                vS > minH ? 1 : 0,
                vW > minH ? 1 : 0
            };

            Tile& tile = getTile(x, y);
            tile.setLevel(minH);
            tile.setSlopeData(slopeData);
        }
    }
}

Tile& Map::getTile(int x, int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Tile coordinates out of range.");
    }
    return const_cast<Tile&>(tiles[y * width + x]);
}

int Map::getWidth() const
{
	return width;
}

int Map::getHeight() const
{
	return height;
}
