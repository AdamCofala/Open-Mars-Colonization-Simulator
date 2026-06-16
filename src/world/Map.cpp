#include "Map.h"
#include "utils/PerlinNoise.h"
#include <stdexcept>
#include <random>
#include <queue>
#include <fstream>
#include "../structures/SolarPanel.h"
#include "../entities/Pipe.h"
#include "../structures/IceMelter.h"
#include "../structures/WaterMagazine.h"

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


void Map::generateTerrain(const WorldGenSettings& settings) {
    unsigned int seed = (settings.seed >= 0) ? (unsigned int)settings.seed
        : (unsigned int)randInt(0, 100000);

    PerlinNoise perlin(seed);
    const float scale = settings.noiseScale;
    const float warpStrength = settings.warpStrength;
    const int   maxHeight = settings.maxHeight;

    std::vector<std::vector<int>> vertex(height + 1, std::vector<int>(width + 1));
    for (int y = 0; y <= (int)height; y++) {
        for (int x = 0; x <= (int)width; x++) {
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
        for (int x = 0; x <= (int)width; x++) {
            v[0][x] = v[1][x];
            v[height][x] = v[height - 1][x];
        }
        for (int y = 0; y <= (int)height; y++) {
            v[y][0] = v[y][1];
            v[y][width] = v[y][width - 1];
        }
        };

    clampEdges(vertex);
    enforceValidTerrain(vertex, width, height);

    for (int y = 0; y < (int)height; y++) {
        for (int x = 0; x < (int)width; x++) {
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

    // Ice deposits
    const float lakeNoiseScale = 0.1f;
    const float lakeNoiseThreshold = settings.lakeThreshold;
    const int   minBasinSize = 2;

    std::vector<std::vector<int>>  level(height, std::vector<int>(width));
    std::vector<std::vector<bool>> visited(height, std::vector<bool>(width, false));
    for (int y = 0; y < (int)height; ++y)
        for (int x = 0; x < (int)width; ++x)
            level[y][x] = tiles[y * width + x].getLevel();

    PerlinNoise lakeNoise(randInt(0, 10000));
    const int dx[] = { 0, 0, -1, 1 };
    const int dy[] = { -1, 1,  0, 0 };

    for (int y = 0; y < (int)height; ++y) {
        for (int x = 0; x < (int)width; ++x) {
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
                if (cx == 0 || cx == (int)width - 1 || cy == 0 || cy == (int)height - 1)
                    touchesEdge = true;
                for (int i = 0; i < 4; ++i) {
                    int nx = cx + dx[i];
                    int ny = cy + dy[i];
                    if (nx < 0 || nx >= (int)width || ny < 0 || ny >= (int)height) {
                        touchesEdge = true;
                        continue;
                    }
                    if (level[ny][nx] < basinLevel) {
                        hasOutlet = true;
                    }
                    else if (level[ny][nx] == basinLevel && !visited[ny][nx]) {
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
                bool slope0110 = (slope[0] == 0 && slope[1] == 1 && slope[2] == 1 && slope[3] == 0);
                bool slope0011 = (slope[0] == 0 && slope[1] == 0 && slope[2] == 1 && slope[3] == 1);
                if (!flat && !slope1100 && !slope1001 && !slope0110 && !slope0011) return false;
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
            p->setMaterialType(MaterialType::NONE);
            allPipes.push_back(p);
        }
    }

    // --- FAZA 0: USTALANIE TYPU RURY ---
    bool changed = true;
    while (changed) {
        changed = false;
        for (Pipe* p : allPipes) {
            if (p->getMaterialType() != MaterialType::NONE) continue;

            int px = p->getX();
            int py = p->getY();

            MaterialType detectedType = MaterialType::NONE;
            bool conflict = false;

            for (int i = 0; i < 4; ++i) {
                int nx = px + directionsX[i];
                int ny = py + directionsY[i];
                if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

                Structure* neighbor = getTile(nx, ny).getStructure();
                if (!neighbor) continue;

                MaterialType mat = MaterialType::NONE;
                if (neighbor->isPipe()) {
                    mat = static_cast<Pipe*>(neighbor)->getMaterialType();
                }
                else {
                    Direction hisDir = static_cast<Direction>((i + 2) % 4);
                    mat = neighbor->getMaterialAtPort(px, py, hisDir);
                }

                if (mat == MaterialType::NONE) continue;

                if (detectedType == MaterialType::NONE) {
                    detectedType = mat; // pierwszy znaleziony typ
                }
                else if (detectedType != mat) {
                    conflict = true; // dwa różne typy z różnych stron
                    break;
                }
            }

            // Ustaw typ tylko jeśli nie ma konfliktu
            if (!conflict && detectedType != MaterialType::NONE) {
                p->setMaterialType(detectedType);
                changed = true;
            }
        }
    }

    // --- FAZA 1: OBLICZANIE MASKI POŁĄCZEŃ ---
    for (Pipe* p : allPipes) {
        int px = p->getX();
        int py = p->getY();
        const Tile& tile = getTile(px, py);
        auto slope = tile.getSlopeData();
        bool slope1100 = (slope[0] == 1 && slope[1] == 1 && slope[2] == 0 && slope[3] == 0);
        bool slope1001 = (slope[0] == 1 && slope[1] == 0 && slope[2] == 0 && slope[3] == 1);
        bool slope0110 = (slope[0] == 0 && slope[1] == 1 && slope[2] == 1 && slope[3] == 0);
        bool slope0011 = (slope[0] == 0 && slope[1] == 0 && slope[2] == 1 && slope[3] == 1);

        if (slope1100) { p->setConnectionMask(0); continue; }
        else if (slope1001) { p->setConnectionMask(1); continue; }
        else if (slope0110) { p->setConnectionMask(2); continue; }
        else if (slope0011) { p->setConnectionMask(4); continue; }

        int mask = 0;
        for (int i = 0; i < 4; ++i) {
            int nx = px + directionsX[i];
            int ny = py + directionsY[i];
            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                Structure* neighbor = getTile(nx, ny).getStructure();
                if (neighbor) {
                    if (neighbor->isPipe()) {
                        Pipe* neighborPipe = static_cast<Pipe*>(neighbor);
                        MaterialType myType = p->getMaterialType();
                        MaterialType hisType = neighborPipe->getMaterialType();

                        // Obaj muszą mieć typ I muszą być zgodne
                        if (myType != MaterialType::NONE &&
                            hisType != MaterialType::NONE &&
                            myType == hisType) {
                            mask |= (8 >> i);
                        }
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
            mask = 10;
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

    // --- FAZA 2: BUDOWANIE LOGICZNYCH SIECI ---
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
                            // Łącz logicznie tylko jeśli typy są zgodne
                            if (neighborPipe->network == nullptr) {
                                MaterialType myType = current->getMaterialType();
                                MaterialType hisType = neighborPipe->getMaterialType();

                                if ((myType == MaterialType::NONE && hisType == MaterialType::NONE) ||
                                    (myType != MaterialType::NONE && myType == hisType)) {
                                    neighborPipe->network = newNet.get();
                                    newNet->pipes.push_back(neighborPipe);
                                    queue.push_back(neighborPipe);
                                }
                            }
                        }
                        else {
                            Direction hisDir = static_cast<Direction>((i + 2) % 4);
                            PortType tilePort = neighbor->getPortAtTile(cx, cy, hisDir);
                            MaterialType portMaterial = neighbor->getMaterialAtPort(cx, cy, hisDir);

                            if (tilePort == PortType::OUTPUT) {
                                if (newNet->getMaterialType() == MaterialType::NONE)
                                    newNet->setMaterialType(portMaterial);
                                if (newNet->getMaterialType() == portMaterial) {
                                    if (std::find(newNet->producers.begin(), newNet->producers.end(), neighbor)
                                        == newNet->producers.end())
                                        newNet->producers.push_back(neighbor);
                                }
                            }
                            if (tilePort == PortType::INPUT) {
                                if (newNet->getMaterialType() == MaterialType::NONE)
                                    newNet->setMaterialType(portMaterial);
                                if (newNet->getMaterialType() == portMaterial) {
                                    if (std::find(newNet->consumers.begin(), newNet->consumers.end(), neighbor)
                                        == newNet->consumers.end())
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

int Map::computePipeConnectionMask(int px, int py) const {
    if (px < 0 || px >= width || py < 0 || py >= height) return 0;

    int directionsX[4] = { 0,  1,  0, -1 };
    int directionsY[4] = { -1, 0,  1,  0 };

    const Tile& tile = tiles[py * width + px];
    auto slope = tile.getSlopeData();
    bool slope1100 = (slope[0] == 1 && slope[1] == 1 && slope[2] == 0 && slope[3] == 0);
    bool slope1001 = (slope[0] == 1 && slope[1] == 0 && slope[2] == 0 && slope[3] == 1);
    bool slope0110 = (slope[0] == 0 && slope[1] == 1 && slope[2] == 1 && slope[3] == 0);
    bool slope0011 = (slope[0] == 0 && slope[1] == 0 && slope[2] == 1 && slope[3] == 1);

    if (slope1100) return 0;
    else if (slope1001) return 1;
    else if (slope0110) return 2;
    else if (slope0011) return 4;

    // Najpierw ustal typ wirtualnej rury (tak jak Faza 0)
    MaterialType virtualType = MaterialType::NONE;
    for (int i = 0; i < 4; ++i) {
        int nx = px + directionsX[i];
        int ny = py + directionsY[i];
        if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
        const Structure* neighbor = getTile(nx, ny).getStructure();
        if (!neighbor) continue;

        MaterialType mat = MaterialType::NONE;
        if (neighbor->isPipe()) {
            mat = static_cast<const Pipe*>(neighbor)->getMaterialType();
        }
        else {
            Direction hisDir = static_cast<Direction>((i + 2) % 4);
            mat = neighbor->getMaterialAtPort(px, py, hisDir);
        }
        if (mat != MaterialType::NONE) {
            if (virtualType == MaterialType::NONE) virtualType = mat;
            else if (virtualType != mat) { virtualType = MaterialType::NONE; break; }
        }
    }

    int mask = 0;
    for (int i = 0; i < 4; ++i) {
        int nx = px + directionsX[i];
        int ny = py + directionsY[i];
        if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
            const Structure* neighbor = getTile(nx, ny).getStructure();
            if (neighbor) {
                if (neighbor->isPipe()) {
                    const Pipe* neighborPipe = static_cast<const Pipe*>(neighbor);
                    MaterialType hisType = neighborPipe->getMaterialType();
                    if (virtualType != MaterialType::NONE &&
                        hisType != MaterialType::NONE &&
                        virtualType == hisType) {
                        mask |= (8 >> i);
                    }
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
    if (popcnt == 0) mask = 10;
    else if (popcnt == 1) {
        for (int i = 0; i < 4; ++i) {
            if (mask & (8 >> i)) { mask |= (8 >> ((i + 2) % 4)); break; }
        }
    }
    return mask;
}

int Map::computePipeConnectionMaskWithVirtual(int px, int py, int vx, int vy) const {
    if (px < 0 || px >= width || py < 0 || py >= height) return 0;

    int directionsX[4] = { 0,  1,  0, -1 };
    int directionsY[4] = { -1, 0,  1,  0 };

    const Tile& tile = tiles[py * width + px];
    auto slope = tile.getSlopeData();
    if (slope[0] == 1 && slope[1] == 1 && slope[2] == 0 && slope[3] == 0) return 0;
    if (slope[0] == 1 && slope[1] == 0 && slope[2] == 0 && slope[3] == 1) return 1;
    if (slope[0] == 0 && slope[1] == 1 && slope[2] == 1 && slope[3] == 0) return 2;
    if (slope[0] == 0 && slope[1] == 0 && slope[2] == 1 && slope[3] == 1) return 4;

    // Typ tej istniejącej rury
    const Structure* self = getTile(px, py).getStructure();
    MaterialType myType = MaterialType::NONE;
    if (self && self->isPipe())
        myType = static_cast<const Pipe*>(self)->getMaterialType();

    // Typ wirtualnej rury (z computePipeConnectionMask)
    MaterialType virtualType = computeVirtualPipeType(vx, vy);

    int mask = 0;
    for (int i = 0; i < 4; ++i) {
        int nx = px + directionsX[i];
        int ny = py + directionsY[i];
        if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;

        bool isVirtual = (nx == vx && ny == vy);

        if (isVirtual) {
            // Łącz z wirtualną rurą tylko jeśli typy są zgodne
            if (myType != MaterialType::NONE &&
                virtualType != MaterialType::NONE &&
                myType == virtualType) {
                mask |= (8 >> i);
            }
        }
        else {
            const Structure* neighbor = getTile(nx, ny).getStructure();
            if (!neighbor) continue;
            if (neighbor->isPipe()) {
                const Pipe* neighborPipe = static_cast<const Pipe*>(neighbor);
                MaterialType hisType = neighborPipe->getMaterialType();
                if (myType != MaterialType::NONE &&
                    hisType != MaterialType::NONE &&
                    myType == hisType) {
                    mask |= (8 >> i);
                }
            }
            else {
                Direction hisDir = static_cast<Direction>((i + 2) % 4);
                PortType tilePort = neighbor->getPortAtTile(px, py, hisDir);
                if (tilePort == PortType::INPUT || tilePort == PortType::OUTPUT)
                    mask |= (8 >> i);
            }
        }
    }

    int popcnt = (mask & 1) + ((mask >> 1) & 1) + ((mask >> 2) & 1) + ((mask >> 3) & 1);
    if (popcnt == 0) mask = 3;
    else if (popcnt == 1) {
        for (int i = 0; i < 4; ++i) {
            if (mask & (8 >> i)) { mask |= (8 >> ((i + 2) % 4)); break; }
        }
    }
    return mask;
}

MaterialType Map::computeVirtualPipeType(int px, int py) const {
    int directionsX[4] = { 0,  1,  0, -1 };
    int directionsY[4] = { -1, 0,  1,  0 };
    MaterialType detected = MaterialType::NONE;

    for (int i = 0; i < 4; ++i) {
        int nx = px + directionsX[i];
        int ny = py + directionsY[i];
        if (nx < 0 || nx >= width || ny < 0 || ny >= height) continue;
        const Structure* neighbor = getTile(nx, ny).getStructure();
        if (!neighbor) continue;

        MaterialType mat = MaterialType::NONE;
        if (neighbor->isPipe()) {
            mat = static_cast<const Pipe*>(neighbor)->getMaterialType();
        }
        else {
            Direction hisDir = static_cast<Direction>((i + 2) % 4);
            mat = neighbor->getMaterialAtPort(px, py, hisDir);
        }
        if (mat == MaterialType::NONE) continue;
        if (detected == MaterialType::NONE) detected = mat;
        else if (detected != mat) return MaterialType::NONE; // konflikt
    }
    return detected;
}

void Map::removeStructureAt(int tileX, int tileY) {
    Tile& tile = getTile(tileX, tileY);
    Structure* structPtr = tile.getStructure();
    if (!structPtr) return; // nic do wyburzenia

    int baseX = structPtr->getX();
    int baseY = structPtr->getY();
    int xOff = structPtr->getXOffset();
    int yOff = structPtr->getYOffset();

    auto it = std::find_if(structures.begin(), structures.end(),
        [structPtr](const std::unique_ptr<Structure>& s) {
            return s.get() == structPtr;
        });
    if (it != structures.end()) {
        structures.erase(it);
    }

    for (int row = 0; row < yOff; ++row) {
        for (int col = 0; col < xOff; ++col) {
            getTile(baseX - col, baseY - row).setStructure(nullptr);
        }
    }

    rebuildNetworks();
}

void Map::save(std::ofstream& file) const {
    file.write((char*)&width, sizeof(width));
    file.write((char*)&height, sizeof(height));

    size_t tileCount = tiles.size();
    file.write((char*)&tileCount, sizeof(tileCount));

    for (const auto& tile : tiles) {
        tile.save(file);
    }

    size_t count = structures.size();
    file.write((char*)&count, sizeof(count));

    for (const auto& s : structures) {
        StructureType type = s->getType();
        file.write((char*)&type, sizeof(type));

        s->save(file);
    }
}

void Map::load(std::ifstream& file) {
    file.read((char*)&width, sizeof(width));
    file.read((char*)&height, sizeof(height));

    size_t tileCount;
    file.read((char*)&tileCount, sizeof(tileCount));

    tiles.resize(tileCount);

    for (auto& tile : tiles) {
        tile.load(file);
    }

    size_t count;
    file.read((char*)&count, sizeof(count));

    structures.clear();

    for (size_t i = 0; i < count; i++) {
        StructureType type;
        file.read((char*)&type, sizeof(type));

        std::unique_ptr<Structure> s;

        switch (type) {
        case StructureType::SolarPanel:
            s = std::make_unique<SolarPanel>(0, 0);
            break;
        case StructureType::Pipe:
            s = std::make_unique<Pipe>(0, 0);
            break;
        case StructureType::IceMelter:
            s = std::make_unique<IceMelter>(0, 0);
            break;
        case StructureType::WaterMagazine:
            s = std::make_unique<WaterMagazine>(0, 0);
            break;
        default:
            continue;
        }

        s->load(file);
        structures.push_back(std::move(s));
    }

    for (auto& tile : tiles) {
        tile.setStructure(nullptr);
        tile.setOccupied(false);
    }

    for (auto& s : structures) {
        int x = s->getX();
        int y = s->getY();

        Tile& tile = getTile(x, y);
        tile.setStructure(s.get());
        tile.setOccupied(true);
    }

    rebuildNetworks();
}