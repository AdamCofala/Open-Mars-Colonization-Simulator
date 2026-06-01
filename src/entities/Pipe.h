#pragma once
#include "Structure.h"

class PipeNetwork;

class Pipe : public Structure {
private:
    int connectionMask = 0;

public:
    PipeNetwork* network = nullptr;

    Pipe(int tx, int ty);

    bool isPipe() const override { return true; }

    int getConnectionMask() const { return connectionMask; }
    void setConnectionMask(int mask) { connectionMask = mask; }
};