#pragma once
#include "Enums.h"
#include <vector>

class Pipe;
class Structure;

class PipeNetwork {
private:
    MaterialType materialType = MaterialType::NONE;

public:
    std::vector<Pipe*> pipes;
    std::vector<Structure*> producers;
    std::vector<Structure*> consumers;

    MaterialType getMaterialType() const { return materialType; }
    void setMaterialType(MaterialType type) { materialType = type; }

    void mergeWith(PipeNetwork* other);
    void updateNetwork(float dt);
};