#include "Map.h"
#include "utils/PerlinNoise.h"
#include <stdexcept>
#include <random>

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


static bool isTileValid(const Tile& tile) {
	auto data = tile.getSlopeData();
	int min = std::min({ data[0], data[1], data[2], data[3] });
	int max = std::max({ data[0], data[1], data[2], data[3] });
	return (max - min) <= 1;
}



static int randomInt(int min, int max)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}


void Map::generateTerrain()

{
    PerlinNoise perlin(1024);


    const float scale = 0.04f;
    const float warpStrength = 6.0f;
    const int maxHeight = 12;

    std::vector<std::vector<int>> vertex(height + 1,
        std::vector<int>(width + 1));

    // 1. DOMAIN WARPED HEIGHT
    for (int y = 0; y <= height; y++) {
        for (int x = 0; x <= width; x++) {

            float nx = perlin.noise(x * scale, y * scale);
            float ny = perlin.noise((x + 1000) * scale, (y + 1000) * scale);

            float warped = perlin.noise((x + nx * warpStrength) * scale,
                (y + ny * warpStrength) * scale);

			warped = pow(warped, 3.0f);

            vertex[y][x] = (int)(warped * maxHeight);
        }
    }

    // 2. LIGHT SMOOTHING (cheap erosion feel)
    for (int i = 0; i < 2; i++) {
        auto copy = vertex;

        for (int y = 1; y < height; y++) {
            for (int x = 1; x < width; x++) {

                int sum =
                    vertex[y][x] +
                    vertex[y - 1][x] +
                    vertex[y + 1][x] +
                    vertex[y][x - 1] +
                    vertex[y][x + 1];

                copy[y][x] = sum / 5;
            }
        }

        vertex = std::move(copy);
    }
   
	fixSaddles(vertex, width, height);
	clampSlopes(vertex, width, height);

    auto clampEdges = [&](std::vector<std::vector<int>>& v)
        {
            for (int x = 0; x <= width; x++)
            {
                v[0][x] = v[1][x];
                v[height][x] = v[height - 1][x];
            }

            for (int y = 0; y <= height; y++)
            {
                v[y][0] = v[y][1];
                v[y][width] = v[y][width - 1];
            }
        };

	clampEdges(vertex);

    // 3. TILE BUILD (NESW-safe)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int vN = vertex[y][x];
            int vE = vertex[y][x + 1];
            int vS = vertex[y + 1][x + 1];
            int vW = vertex[y + 1][x];

            int minH = std::min({ vN, vE, vS, vW });

            Tile& tile = tiles[y * width + x];
            tile.setLevel(minH);

            int slope[4] = {
                vN > minH,
                vE > minH,
                vS > minH,
                vW > minH
            };

            tile.setSlopeData(slope);
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
