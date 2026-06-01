#pragma once
#include <vector>

class Pipe;
class Structure;

class PipeNetwork {
public:
    std::vector<Pipe*> pipes;
    std::vector<Structure*> producers;
    std::vector<Structure*> consumers;

    void mergeWith(PipeNetwork* other);
    void updateNetwork(float dt);
};