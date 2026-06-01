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

    const float lakeNoiseScale = 0.1f;
    const float lakeNoiseThreshold = 0.5f;
    const int   minBasinSize = 2;
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
            std::vector<std::pair<int, int>> basin;
            std::queue<std::pair<int, int>> q;
            q.push({ x, y });
            visited[y][x] = true;
            bool touchesEdge = false;
            bool hasOutlet = false;

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
            if (touchesEdge || hasOutlet || (int)basin.size() < minBasinSize)
                continue;

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

Tile& Map::getTile(int x, int y) const {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        throw std::out_of_range("Tile coordinates out of range.");
    }
    return const_cast<Tile&>(tiles[y * width + x]);
}

std::vector<std::unique_ptr<Structure>>& Map::getStructures() {
    return structures;
}

const std::vector<std::unique_ptr<Structure>>& Map::getStructures() const {
    return structures;
}

int Map::getWidth() const { return width; }
int Map::getHeight() const { return height; }
int Map::getHalfWidth() const { return width / 2; }
int Map::getHalfHeight() const { return height / 2; }

bool Map::canPlaceStructure(int x, int y, int xOffset, int yOffset, bool isPipe) const {
    for (int dy = 0; dy < yOffset; ++dy) {
        for (int dx = 0; dx < xOffset; ++dx) {
            int tileX = x - dx;   // w lewo (oryginalny kierunek)
            int tileY = y - dy;   // w górę
            if (tileX < 0 || tileX >= width || tileY < 0 || tileY >= height) {
                return false;
            }
            const Tile& tile = getTile(tileX, tileY);
            if (tile.isOccupied()) return false;
            if (isPipe) {
                auto slope = tile.getSlopeData();
                bool flat = tile.isFlat();
                bool slope1100 = (slope[0] == 1 && slope[1] == 1 && slope[2] == 0 && slope[3] == 0);
                bool slope1001 = (slope[0] == 1 && slope[1] == 0 && slope[2] == 0 && slope[3] == 1);
                if (!flat && !slope1100 && !slope1001) return false;
            }
            else {
                if (!tile.isFlat()) return false;
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
    int xOff = rawPtr->getXOffset();
    int yOff = rawPtr->getYOffset();

    for (int row = 0; row < yOff; ++row) {
        for (int col = 0; col < xOff; ++col) {
            getTile(x - col, y - row).setStructure(rawPtr);
        }
    }

    structures.push_back(std::move(structure));
    rebuildNetworks();
}

void Map::rebuildNetworks() {
    allNetworks.clear();

    int directionsX[] = { 0,  1,  0, -1 };
    int directionsY[] = { -1, 0,  1, 0 };

    std::vector<Pipe*> allPipes;
    for (const auto& structPtr : structures) {
        if (structPtr && structPtr->isPipe()) {
            Pipe* p = static_cast<Pipe*>(structPtr.get());
            p->network = nullptr;
            p->setConnectionMask(0);
            allPipes.push_back(p);
        }
    }

    for (Pipe* p : allPipes) {
        int px = p->getX();
        int py = p->getY();
        const Tile& tile = getTile(px, py);
        auto slope = tile.getSlopeData();
        bool slope1100 = (slope[0] == 1 && slope[1] == 1 && slope[2] == 0 && slope[3] == 0);
        bool slope1001 = (slope[0] == 1 && slope[1] == 0 && slope[2] == 0 && slope[3] == 1);

        if (slope1100) {
            p->setConnectionMask(0);
            continue;
        }
        else if (slope1001) {
            p->setConnectionMask(1);
            continue;
        }

        int mask = 0;
        for (int i = 0; i < 4; ++i) {
            int nx = px + directionsX[i];
            int ny = py + directionsY[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                Structure* neighbor = getTile(nx, ny).getStructure();
                if (neighbor) {
                    if (neighbor->isPipe()) {
                        mask |= (8 >> i);
                    }
                    else {
                        Direction hisDir = static_cast<Direction>((i + 2) % 4);

                        PortType tilePort = neighbor->getPortAtTile(px, py, hisDir);
                        if (tilePort == PortType::INPUT || tilePort == PortType::OUTPUT) {
                            mask |= (8 >> i);
                        }
                    }
                }
            }
        }

        int popcnt = (mask & 1) + ((mask >> 1) & 1) + ((mask >> 2) & 1) + ((mask >> 3) & 1);
        if (popcnt == 0) {
            mask = 3;
        }
        else if (popcnt == 1) {
            for (int i = 0; i < 4; ++i) {
                if (mask & (8 >> i)) {
                    mask |= (8 >> ((i + 2) % 4));
                    break;
                }
            }
        }

        p->setConnectionMask(mask);
    }

    for (Pipe* p : allPipes) {
        if (p->network != nullptr) continue;

        auto newNet = std::make_unique<PipeNetwork>();
        p->network = newNet.get();
        newNet->pipes.push_back(p);

        std::vector<Pipe*> queue;
        queue.push_back(p);

        while (!queue.empty()) {
            Pipe* current = queue.back();
            queue.pop_back();

            int cx = current->getX();
            int cy = current->getY();

            for (int i = 0; i < 4; ++i) {
                int nx = cx + directionsX[i];
                int ny = cy + directionsY[i];

                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    Structure* neighbor = getTile(nx, ny).getStructure();
                    if (neighbor) {
                        if (neighbor->isPipe()) {
                            Pipe* neighborPipe = static_cast<Pipe*>(neighbor);
                            if (neighborPipe->network == nullptr) {
                                neighborPipe->network = newNet.get();
                                newNet->pipes.push_back(neighborPipe);
                                queue.push_back(neighborPipe);
                            }
                        }
                        else {
                            Direction hisDir = static_cast<Direction>((i + 2) % 4);

                            PortType tilePort = neighbor->getPortAtTile(cx, cy, hisDir);

                            if (tilePort == PortType::OUTPUT) {
                                if (std::find(newNet->producers.begin(), newNet->producers.end(), neighbor) == newNet->producers.end()) {
                                    newNet->producers.push_back(neighbor);
                                }
                            }
                            if (tilePort == PortType::INPUT) {
                                if (std::find(newNet->consumers.begin(), newNet->consumers.end(), neighbor) == newNet->consumers.end()) {
                                    newNet->consumers.push_back(neighbor);
                                }
                            }
                        }
                    }
                }
            }
        }
        allNetworks.push_back(std::move(newNet));
    }
}

void Map::updateNetworks(float dt) {
    for (auto& net : allNetworks) {
        if (net) net->updateNetwork(dt);
    }
}