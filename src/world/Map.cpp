#include "Map.h"
#include "utils/PerlinNoise.h"
#include <stdexcept>
#include <random>
#include <queue>

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

    std::vector<std::vector<int>> vertex(height + 1, std::vector<int>(width + 1));

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

    enforceValidTerrain(vertex, width, height);

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
    enforceValidTerrain(vertex, width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int vN = vertex[y][x];
            int vE = vertex[y][x + 1];
            int vS = vertex[y + 1][x + 1];
            int vW = vertex[y + 1][x];

            int minH = std::min({ vN, vE, vS, vW });
            Tile& tile = tiles[y * width + x];
            tile.setLevel(minH);

            int slope[4] = { vN > minH, vE > minH, vS > minH, vW > minH };
            tile.setSlopeData(slope);
        }
    }

    // Parameters for lake/ice generation
    const float lakeNoiseScale = 0.1f;  // spatial frequency of the lake noise
    const float lakeNoiseThreshold = 0.5f; // basins above this value become ice
    const int   minBasinSize = 2;     // minimum flat-tile count to consider a basin

    // Build a level grid for quick neighbour lookups
    std::vector<std::vector<int>>  level(height, std::vector<int>(width));
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));

    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
            level[y][x] = tiles[y * width + x].getLevel();

    PerlinNoise lakeNoise(randInt(0, 10000));

    const int dx[] = { 0, 0, -1, 1 };
    const int dy[] = { -1, 1,  0, 0 };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (visited[y][x] || !tiles[y * width + x].isFlat())
                continue;

            const int basinLevel = level[y][x];

            // BFS flood-fill to collect all flat tiles at the same elevation
            std::vector<std::pair<int, int>> basin;
            std::queue<std::pair<int, int>> q;
            q.push({ x, y });
            visited[y][x] = true;

            bool touchesEdge = false;
            bool hasOutlet = false; // true if any neighbour is lower (water would drain)

            while (!q.empty()) {
                auto [cx, cy] = q.front(); q.pop();
                basin.push_back({ cx, cy });

                if (cx == 0 || cx == width - 1 || cy == 0 || cy == height - 1)
                    touchesEdge = true;

                for (int i = 0; i < 4; ++i) {
                    int nx = cx + dx[i];
                    int ny = cy + dy[i];

                    if (nx < 0 || nx >= width || ny < 0 || ny >= height) {
                        touchesEdge = true;
                        continue;
                    }

                    if (level[ny][nx] < basinLevel) {
                        hasOutlet = true;
                    }
                    else if (level[ny][nx] == basinLevel &&
                        tiles[ny * width + nx].isFlat() &&
                        !visited[ny][nx]) {
                        visited[ny][nx] = true;
                        q.push({ nx, ny });
                    }
                }
            }

            // Only enclosed, drainless basins of sufficient size become ice
            if (touchesEdge || hasOutlet || (int)basin.size() < minBasinSize)
                continue;

            // Sample lake noise at the basin centroid to vary which basins freeze
            float avgX = 0, avgY = 0;
            for (auto& [bx, by] : basin) { avgX += bx; avgY += by; }
            avgX /= basin.size();
            avgY /= basin.size();

            if (lakeNoise.noise(avgX * lakeNoiseScale, avgY * lakeNoiseScale) > lakeNoiseThreshold) {
                for (auto& [bx, by] : basin)
                    tiles[by * width + bx].setType(TileType::Ice);
            }
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

void Map::addStructure(std::unique_ptr<Structure> structure) {
    if (!structure) return;

    Structure* rawPtr = structure.get();
    int x = rawPtr->getX();
    int y = rawPtr->getY();

    for (int row = 0; row < rawPtr->getYOffset(); ++row) {
        for (int col = 0; col < rawPtr->getXOffset(); ++col) {
            getTile(x + col, y + row).setStructure(rawPtr);
        }
    }

    int directionsX[] = { 1,  0, -1,  0 };
    int directionsY[] = { 0,  1,  0, -1 };

    if (rawPtr->isPipe()) {
        Pipe* rawPipe = static_cast<Pipe*>(rawPtr);

        auto newNet = std::make_unique<PipeNetwork>();
        rawPipe->network = newNet.get();
        newNet->pipes.push_back(rawPipe);
        allNetworks.push_back(std::move(newNet));

        int currentMask = 0;

        for (int i = 0; i < 4; ++i) {
            int nx = x + directionsX[i];
            int ny = y + directionsY[i];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                Structure* neighbor = getTile(nx, ny).getStructure();

                if (neighbor != nullptr && neighbor != rawPipe) {
                    Direction myDir = static_cast<Direction>(i);
                    Direction hisDir = static_cast<Direction>((i + 2) % 4);

                    if (neighbor->isPipe()) {
                        Pipe* neighborPipe = static_cast<Pipe*>(neighbor);

                        currentMask |= (1 << i);
                        neighborPipe->setConnectionMask(neighborPipe->getConnectionMask() | (1 << hisDir));

                        rawPipe->network->mergeWith(neighborPipe->network);
                    }
                    else {
                        PortType tilePort = neighbor->getPortAtTile(nx, ny, hisDir);

                        if (tilePort == PortType::OUTPUT) {
                            rawPipe->network->producers.push_back(neighbor);
                            currentMask |= (1 << i);
                        }
                        else if (tilePort == PortType::INPUT) {
                            rawPipe->network->consumers.push_back(neighbor);
                            currentMask |= (1 << i);
                        }
                    }
                }
            }
        }
        rawPipe->setConnectionMask(currentMask);
    }
    else {
        for (const auto& port : rawPtr->getPorts()) {
            int portX = x + port.offsetX;
            int portY = y + port.offsetY;

            int targetX = portX + directionsX[static_cast<int>(port.dir)];
            int targetY = portY + directionsY[static_cast<int>(port.dir)];

            if (targetX >= 0 && targetX < width && targetY >= 0 && targetY < height) {
                Structure* neighbor = getTile(targetX, targetY).getStructure();

                if (neighbor != nullptr && neighbor->isPipe()) {
                    Pipe* neighborPipe = static_cast<Pipe*>(neighbor);
                    Direction hisDir = static_cast<Direction>((static_cast<int>(port.dir) + 2) % 4);

                    neighborPipe->setConnectionMask(neighborPipe->getConnectionMask() | (1 << hisDir));

                    if (port.type == PortType::OUTPUT) {
                        neighborPipe->network->producers.push_back(rawPtr);
                    }
                    else if (port.type == PortType::INPUT) {
                        neighborPipe->network->consumers.push_back(rawPtr);
                    }
                }
            }
        }
    }

    structures.push_back(std::move(structure));
}

void Map::updateNetworks(float dt) {
    for (auto& net : allNetworks) {
        if (net) net->updateNetwork(dt);
    }
}
