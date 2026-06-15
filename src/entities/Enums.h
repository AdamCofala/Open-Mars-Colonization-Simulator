#pragma once

enum class MaterialType {
    NONE,
    ENERGY,
    WATER
};

enum class PortType {
    NONE,
    INPUT,
    OUTPUT
};

enum Direction {
    NORTH_EAST = 0,
    SOUTH_EAST = 1,
    SOUTH_WEST = 2,
    NORTH_WEST = 3
};

struct StructurePort {
    int offsetX = 0;
    int offsetY = 0;
    Direction dir;
    PortType type;
    MaterialType material = MaterialType::NONE;
};