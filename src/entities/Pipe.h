#pragma once
#include "Structure.h"

class PipeNetwork;

class Pipe : public Structure {
private:
    int connectionMask = -1;   // bity: bit3=N, bit2=E, bit1=S, bit0=W
    MaterialType materialType = MaterialType::NONE;

public:
    PipeNetwork* network = nullptr;

    Pipe(int tx, int ty);

    bool isPipe() const override { return true; }

    MaterialType getMaterialType() const { return materialType; }
    void setMaterialType(MaterialType type) { materialType = type; }

    int getConnectionMask() const { return connectionMask; }
    void setConnectionMask(int mask) { connectionMask = mask; }
};