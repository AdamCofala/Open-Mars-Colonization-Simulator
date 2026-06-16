#include "PipeNetwork.h"
#include "Pipe.h"
#include "Structure.h"
#include <algorithm>

void PipeNetwork::mergeWith(PipeNetwork* other) {
    if (this == other || other == nullptr) return;

    for (Pipe* p : other->m_pipes) {
        p->network = this;
        this->m_pipes.push_back(p);
    }
    this->m_producers.insert(this->m_producers.end(), other->m_producers.begin(), other->m_producers.end());
    this->m_consumers.insert(this->m_consumers.end(), other->m_consumers.begin(), other->m_consumers.end());
}

void PipeNetwork::updateNetwork(float dt) {
    if (m_producers.empty() || m_consumers.empty()) return;

    MaterialType matType = materialType;
    if (matType == MaterialType::NONE) return;

    float totalAvailable = 0.0f;
    for (Structure* prod : m_producers)
        totalAvailable += prod->getOutputInventory().getAmount(matType);

    if (totalAvailable <= 0.0f) return;

    float totalDemand = 0.0f;
    for (Structure* cons : m_consumers) {
        float cap = cons->getInputInventory().getMaxCapacity(matType);
        float cur = cons->getInputInventory().getAmount(matType);
        totalDemand += (cap - cur);
    }

    if (totalDemand <= 0.0f) return;

    float amountToTransfer = std::min(totalAvailable, totalDemand);
    float lossFactor = amountToTransfer / totalAvailable;
    float gainFactor = amountToTransfer / totalDemand;

    for (Structure* prod : m_producers) {
        float cur = prod->getOutputInventory().getAmount(matType);
        prod->getOutputInventory().subResource(matType, cur * lossFactor);
    }
    for (Structure* cons : m_consumers) {
        float cap = cons->getInputInventory().getMaxCapacity(matType);
        float cur = cons->getInputInventory().getAmount(matType);
        float space = cap - cur;
        cons->getInputInventory().addResource(matType, space * gainFactor);
    }
}