#include <unordered_map>

class Inventory {
private:
    std::unordered_map<ResourceType, int> items;
    int maxCapacity;

public:
    Inventory(int capacity);

    void init(int capacity = 100);

    void addResource(ResourceType type, int amount);
    bool hasResource(ResourceType type, int amount) const;

    int getAmount(ResourceType type) const;
    int getMaxCapacity() const;
    int getCurrentLoad() const;

    void clear();
};