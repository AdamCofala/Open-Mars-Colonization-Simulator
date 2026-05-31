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


static bool invalidTile(const std::vector<std::vector<int>>& vertex, int x, int y) {
    int vN = vertex[y][x];
    int vE = vertex[y][x + 1];
    int vS = vertex[y + 1][x + 1];
    int vW = vertex[y + 1][x];
    int minH = std::min({ vN, vE, vS, vW });
    int maxH = std::max({ vN, vE, vS, vW });
    return (maxH - minH > 1);
}


static void flattenTile(std::vector<std::vector<int>>& vertex, int x, int y) {
    int& vN = vertex[y][x];
    int& vE = vertex[y][x + 1];
    int& vS = vertex[y + 1][x + 1];
    int& vW = vertex[y + 1][x];
    int minH = std::min({ vN, vE, vS, vW });
    vN = vE = vS = vW = minH + 1;
}

static void enforceValidTerrain(std::vector<std::vector<int>>& vertex, int width, int height) {
    constexpr int MAX_ITER = 100;
    for (int iter = 0; iter < MAX_ITER; ++iter) {
        bool changed = false;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (invalidTile(vertex, x, y)) {
                    flattenTile(vertex, x, y);
                    changed = true;
                }
            }
        }
        if (!changed) break;
    }
}

static int randInt(int min, int max) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

void Map::generateTerrain() {
    PerlinNoise perlin(randInt(0, 10000));

    // Perlin noise parameters for initial terrain generation
    const float scale = 0.05f;
    const float warpStrength = 6.0f;
    const int maxHeight = 10;

    // We generate an array of vertices. A 2D grid of tiles corresponds to width+1 by height+1 vertices.
    std::vector<std::vector<int>> vertex(height + 1,
        std::vector<int>(width + 1));

    for (int y = 0; y <= height; y++) {
        for (int x = 0; x <= width; x++) {
            // Calculate base noise
            float nx = perlin.noise(x * scale, y * scale);
            float ny = perlin.noise((x + 1000) * scale, (y + 1000) * scale);

            // Apply domain warping to create more natural, less grid-like terrain
            float warped = perlin.noise((x + nx * warpStrength) * scale,
                (y + ny * warpStrength) * scale);

            // Sharpen valleys and peaks by cubing the noise result
            warped = pow(warped, 3.0f);
            vertex[y][x] = (int)(warped * maxHeight);
        }
    }

    // Ensure the terrain slopes are valid for isometric rendering (no excessive steepness)
    enforceValidTerrain(vertex, width, height);

    // Make map edges flat by copying adjacent vertex heights
    auto clampEdges = [&](std::vector<std::vector<int>>& v) {
       for (int x = 0; x <= width; x++) {
            v[0][x] = v[1][x];
            v[height][x] = v[height - 1][x];
       }

       for (int y = 0; y <= height; y++) {
         v[y][0] = v[y][1];
         v[y][width] = v[y][width - 1];
       }
    };

    clampEdges(vertex);

    // Re-validate the terrain shape after edge adjustments
    enforceValidTerrain(vertex, width, height);

    // Convert vertex heights into tile representations (level and slopes)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int vN = vertex[y][x];
            int vE = vertex[y][x + 1];
            int vS = vertex[y + 1][x + 1];
            int vW = vertex[y + 1][x];

            // The tile's base elevation is simply the lowest of its 4 corner vertices
            int minH = std::min({ vN, vE, vS, vW });
            Tile& tile = tiles[y * width + x];
            tile.setLevel(minH);

            // A corner is considered 'sloped' if it is higher than the minimum elevation
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

std::vector<std::unique_ptr<Structure>>& Map::getStructures()
{
    return structures;
}

const std::vector<std::unique_ptr<Structure>>& Map::getStructures() const
{
    return structures;
}

int Map::getWidth() const
{
	return width;
}

int Map::getHeight() const
{
	return height;
}

int Map::getHalfWidth() const
{
	return width / 2;
}

int Map::getHalfHeight() const
{
	return height / 2;
}

bool Map::canPlaceStructure(int x, int y, int xOffset, int yOffset) const {
	for (int dy = 0; dy < yOffset; ++dy) {
		for (int dx = 0; dx < xOffset; ++dx) {
			int tileX = x - dx;
			int tileY = y - dy;
			if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height) {
				return false;
			}
			if (!getTile(tileX, tileY).isFlat() || getTile(tileX, tileY).isOccupied()) {
				return false;
			}
		}
	}
	return true;
}

void Map::addStructure(std::unique_ptr<Structure> structure)
{
    if (!structure) {
        return;
    }

    int x = structure->getX();
    int y = structure->getY();
    int xOffset = structure->getXOffset();
    int yOffset = structure->getYOffset();

    if (!canPlaceStructure(x, y, xOffset, yOffset)) {
        return;
    }

    Structure* rawStructurePtr = structure.get();

    for (int dy = 0; dy < yOffset; ++dy) {
        for (int dx = 0; dx < xOffset; ++dx) {
            int tileX = x - dx;
            int tileY = y - dy;

            Tile& tile = getTile(tileX, tileY);
            tile.setOccupied(true);

            tile.setStructure(rawStructurePtr);
        }
    }

    structures.push_back(std::move(structure));
}
