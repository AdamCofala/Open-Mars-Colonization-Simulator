#pragma once
#include "Enums.h"
#include <vector>

class Pipe;
class Structure;

class PipeNetwork {
private:
    MaterialType materialType = MaterialType::NONE;
    std::vector<Pipe*> m_pipes;
    std::vector<Structure*> m_producers;
    std::vector<Structure*> m_consumers;

public:
    MaterialType getMaterialType() const { return materialType; }
    void setMaterialType(MaterialType type) { materialType = type; }

    const std::vector<Pipe*>& getPipes() const { return m_pipes; }
    const std::vector<Structure*>& getProducers() const { return m_producers; }
    const std::vector<Structure*>& getConsumers() const { return m_consumers; }

    void addPipe(Pipe* pipe) { m_pipes.push_back(pipe); }
    void addProducer(Structure* producer) { m_producers.push_back(producer); }
    void addConsumer(Structure* consumer) { m_consumers.push_back(consumer); }

    void mergeWith(PipeNetwork* other);
    void updateNetwork(float dt);
};