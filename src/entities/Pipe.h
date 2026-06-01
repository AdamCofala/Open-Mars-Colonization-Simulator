#pragma once
#include "Structure.h"

class PipeNetwork;

class Pipe : public Structure {
private:
    int connectionMask = 3;   // bity: bit3=N, bit2=E, bit1=S, bit0=W

public:
    PipeNetwork* network = nullptr;

    Pipe(int tx, int ty);

    bool isPipe() const override { return true; }

    int getConnectionMask() const { return connectionMask; }
    void setConnectionMask(int mask) { connectionMask = mask; }
};