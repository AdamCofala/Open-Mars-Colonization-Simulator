#include <unordered_map>
#include "MaterialType.h"

class Inventory {
private:
    std::unordered_map<MaterialType, int> items;
    int maxCapacity;

public:
    void init(int capacity = 100);

    void addResource(MaterialType type, int amount);
    bool hasResource(MaterialType type, int amount) const;

    int getAmount(MaterialType type) const;
    int getMaxCapacity() const;
    int getCurrentLoad() const;

    void clear();
};