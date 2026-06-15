#include "PipeNetwork.h"
#include "Pipe.h"
#include "Structure.h"
#include <algorithm>

void PipeNetwork::mergeWith(PipeNetwork* other) {
    if (this == other || other == nullptr) return;

    for (Pipe* p : other->pipes) {
        p->network = this;
        this->pipes.push_back(p);
    }
    this->producers.insert(this->producers.end(), other->producers.begin(), other->producers.end());
    this->consumers.insert(this->consumers.end(), other->consumers.begin(), other->consumers.end());
}

void PipeNetwork::updateNetwork(float dt) {
    if (producers.empty() || consumers.empty()) return;

    MaterialType matType = materialType;
    if (matType == MaterialType::NONE) return;

    float totalAvailable = 0.0f;
    for (Structure* prod : producers)
        totalAvailable += prod->getOutputInventory().getAmount(matType);

    if (totalAvailable <= 0.0f) return;

    float totalDemand = 0.0f;
    for (Structure* cons : consumers) {
        float cap = cons->getInputInventory().getMaxCapacity(matType);
        float cur = cons->getInputInventory().getAmount(matType);
        totalDemand += (cap - cur);
    }

    if (totalDemand <= 0.0f) return;

    float amountToTransfer = std::min(totalAvailable, totalDemand);
    float lossFactor = amountToTransfer / totalAvailable;
    float gainFactor = amountToTransfer / totalDemand;

    for (Structure* prod : producers) {
        float cur = prod->getOutputInventory().getAmount(matType);
        prod->getOutputInventory().subResource(matType, cur * lossFactor);
    }
    for (Structure* cons : consumers) {
        float cap = cons->getInputInventory().getMaxCapacity(matType);
        float cur = cons->getInputInventory().getAmount(matType);
        float space = cap - cur;
        cons->getInputInventory().addResource(matType, space * gainFactor);
    }
}