#pragma once

#include "Items/Item.h"

class SimpleFoiledItem : public Item
{
public:
	SimpleFoiledItem(int id);

	bool isFoil(std::shared_ptr<ItemInstance> itemInstance);
};