#include "SolarPanel.h"
#include <iostream>

void SolarPanel::update(Inventory& globalInventory) {
    globalInventory.addResource(MaterialType::ENERGY, 10);
    std::cout << "Panel sloneczny wyprodukowal 10 energii!\n";
}