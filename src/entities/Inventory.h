#include <unordered_map>
#include "MaterialType.h"

class Inventory {
private:
    std::unordered_map<ResourceType, int> items;
    int maxCapacity;

public:
    void init(int capacity = 100);

    void addResource(ResourceType type, int amount);
    bool hasResource(ResourceType type, int amount) const;

    int getAmount(ResourceType type) const;
    int getMaxCapacity() const;
    int getCurrentLoad() const;

    void clear();
};