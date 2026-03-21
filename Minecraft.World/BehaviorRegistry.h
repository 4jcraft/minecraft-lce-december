#pragma once

class DispenseItemBehavior;

class BehaviorRegistry {
private:
    std::unordered_map<Item*, DispenseItemBehavior*> storage;
    DispenseItemBehavior* defaultBehavior;

public:
    BehaviorRegistry(DispenseItemBehavior* defaultValue);
    ~BehaviorRegistry();

    DispenseItemBehavior* std::get(Item* key);
    void add(Item* key, DispenseItemBehavior* value);
};