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

    std::vector<MaterialType> allMaterials = {
        MaterialType::WATER,
        MaterialType::ENERGY
    };

    for (MaterialType matType : allMaterials) {

        float totalAvailable = 0.0f;
        for (Structure* prod : producers) {
            totalAvailable += prod->getInternalInventory().getAmount(matType);
        }

        if (totalAvailable <= 0.0f) continue;

        float totalDemand = 0.0f;
        for (Structure* cons : consumers) {
            float capacity = cons->getInternalInventory().getMaxCapacity(matType);
            float amount = cons->getInternalInventory().getAmount(matType);
            totalDemand += (capacity - amount);
        }

        if (totalDemand <= 0.0f) continue;

        float amountToTransfer = std::min(totalAvailable, totalDemand);
        float lossFactor = amountToTransfer / totalAvailable;
        float gainFactor = amountToTransfer / totalDemand;

        for (Structure* prod : producers) {
            float currentAmount = prod->getInternalInventory().getAmount(matType);
            float toSub = currentAmount * lossFactor;
            prod->getInternalInventory().subResource(matType, toSub);
        }

        for (Structure* cons : consumers) {
            float capacity = cons->getInternalInventory().getMaxCapacity(matType);
            float amount = cons->getInternalInventory().getAmount(matType);
            float spaceLeft = capacity - amount;

            float toAdd = spaceLeft * gainFactor;
            cons->getInternalInventory().addResource(matType, toAdd);
        }
    }
}