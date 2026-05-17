#include "Structure.h"

void Structure::init(int startX, int startY, const std::string& texId) {
    x = startX;
    y = startY;
    textureId = texId;
}

void Structure::update() {
    // sprawdzenie czy budynek mo¿e produkowaæ
    bool canOperate = true;
    for (const auto& [material, amountNeeded] : consumeRates) {
        if (internalInventory.getAmount(material) < amountNeeded) {
            canOperate = false;
            break;
        }
    }

    // je¿eli mo¿e to wykonaæ
    if (canOperate) {
        // Konsumpcja
        for (const auto& [material, amountNeeded] : consumeRates) {
            internalInventory.subResource(material, amountNeeded);
        }

        // Produkcja
        for (const auto& [material, amountProduced] : productionRates) {
            internalInventory.addResource(material, amountProduced);
        }
    }
}

const Inventory& Structure::getInternalInventory() const {
    return internalInventory;
}

std::string Structure::getTextureId() const {
    return textureId;
}

int Structure::getX() const {
    return x;
}

int Structure::getY() const {
    return y;
}