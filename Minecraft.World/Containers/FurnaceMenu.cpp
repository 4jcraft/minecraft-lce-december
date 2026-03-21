#include "../Platform/stdafx.h"
#include "../Headers/net.minecraft.world.entity.player.h"
#include "../Headers/net.minecraft.world.item.h"
#include "../Headers/net.minecraft.world.level.tile.entity.h"
#include "FurnaceResultSlot.h"
#include "Slot.h"
#include "../Stats/GenericStats.h"
#include "FurnaceMenu.h"
#include "../Recipes/FurnaceRecipes.h"

FurnaceMenu::FurnaceMenu(std::shared_ptr<Inventory> inventory,
                         std::shared_ptr<FurnaceTileEntity> furnace)
    : AbstractContainerMenu() {
    tc = 0;
    lt = 0;
    ld = 0;

    this->furnace = furnace;

    addSlot(new Slot(furnace, 0, 52 + 4, 13 + 4));
    addSlot(new Slot(furnace, 1, 52 + 4, 49 + 4));
    addSlot(new FurnaceResultSlot(
        dynamic_pointer_cast<Player>(inventory->player->shared_from_this()),
        furnace, 2, 112 + 4, 31 + 4));

    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 9; x++) {
            addSlot(
                new Slot(inventory, x + y * 9 + 9, 8 + x * 18, 84 + y * 18));
        }
    }
    for (int x = 0; x < 9; x++) {
        addSlot(new Slot(inventory, x, 8 + x * 18, 142));
    }
}

void FurnaceMenu::addSlotListener(ContainerListener* listener) {
    AbstractContainerMenu::addSlotListener(listener);
    listener->setContainerData(this, 0, furnace->tickCount);
    listener->setContainerData(this, 1, furnace->litTime);
    listener->setContainerData(this, 2, furnace->litDuration);
}

void FurnaceMenu::broadcastChanges() {
    AbstractContainerMenu::broadcastChanges();

    AUTO_VAR(itEnd, containerListeners.end());
    for (AUTO_VAR(it, containerListeners.begin()); it != itEnd; it++) {
        ContainerListener* listener = *it;  // containerListeners->at(i);
        if (tc != furnace->tickCount) {
            listener->setContainerData(this, 0, furnace->tickCount);
        }
        if (lt != furnace->litTime) {
            listener->setContainerData(this, 1, furnace->litTime);
        }
        if (ld != furnace->litDuration) {
            listener->setContainerData(this, 2, furnace->litDuration);
        }
    }

    tc = furnace->tickCount;
    lt = furnace->litTime;
    ld = furnace->litDuration;
}

void FurnaceMenu::setData(int id, int value) {
    if (id == 0) furnace->tickCount = value;
    if (id == 1) furnace->litTime = value;
    if (id == 2) furnace->litDuration = value;
}

bool FurnaceMenu::stillValid(std::shared_ptr<Player> player) {
    return furnace->stillValid(player);
}

std::shared_ptr<ItemInstance> FurnaceMenu::quickMoveStack(
    std::shared_ptr<Player> player, int slotIndex) {
    std::shared_ptr<ItemInstance> clicked = nullptr;
    Slot* slot = slots.at(
        slotIndex);  // Slot *IngredientSlot = slots->at(INGREDIENT_SLOT);

    bool charcoalUsed = furnace->wasCharcoalUsed();

    if (slot != NULL && slot->hasItem()) {
        std::shared_ptr<ItemInstance> std::stack = slot->getItem();
        clicked = std::stack->copy();

        if (slotIndex == RESULT_SLOT) {
            if (!moveItemStackTo(std::stack, INV_SLOT_START, USE_ROW_SLOT_END,
                                 true)) {
                return nullptr;
            }
            slot->onQuickCraft(std::// 4J-JEV, hook for Durango achievement 'Renewable Energy'.#ifdef _EXTENDED_ACHIEVEMENTS

            if (charcoalUsed && std::stack->getItem()->id == Item::coal_Id &&
                std::stack->getAuxValue() == CoalItem::CHAR_COAL)
                player->awardStat(GenericStats::renewableEnergy(),
#endifcStats::param_renewableEnergy());

        } else if (slotIndex == FUEL_SLOT || slotIndex == INGREDIENT_SLOT) {
            if (!moveItemStackTo(std::stack, INV_SLOT_START, USE_ROW_SLOT_END,
                                 false)) {
                return nullptr;
            }
        } else if (FurnaceRecipes::getInstance()->getResult(
                       std::stack->getItem()->id) != NULL) {
            if (!moveItemStackTo(std::stack, INGREDIENT_SLOT,
                                 INGREDIENT_SLOT + 1, false)) {
                return nullptr;
            }
        } else if (FurnaceTileEntity::isFuel(std::stack)) {
            if (!moveItemStackTo(std::stack, FUEL_SLOT, FUEL_SLOT + 1, false)) {
                return nullptr;
            }
        } else if (slotIndex >= INV_SLOT_START && slotIndex < INV_SLOT_END) {
            if (!moveItemStackTo(std::stack, USE_ROW_SLOT_START,
                                 USE_ROW_SLOT_END, false)) {
                return nullptr;
            }
        } else if (slotIndex >= USE_ROW_SLOT_START &&
                   slotIndex < USE_ROW_SLOT_END) {
            if (!moveItemStackTo(std::stack, INV_SLOT_START, INV_SLOT_END,
                                 false)) {
                return nullptr;
            }
        }
        if (std::stack->count == 0) {
            slot->set(nullptr);
        } else {
            slot->setChanged();
        }
        if (std::stack->count == clicked->count) {
            return nullptr;
        } else {
            slot->onTake(player, std::stack);
        }
    }
    return clicked;
}

std::shared_ptr<ItemInstance> FurnaceMenu::clicked(
    int s  // 4J Added looped param int clickType, std::shared_ptr<Player>
           // player,
    bool looped) {
    bool charcoalUsed = furnace->wasCharcoalUsed();

    std::shared_ptr<ItemInstance> o
#ifdef _EXTENDED_ACHIEVEMENTSicked(
        slotIndex, buttonNum, clickType, player, looped);

    if (charcoalUsed && (out != nullptr) &&
        (buttonNum == 0 || buttonNum == 1) && clickType == CLICK_PICKUP &&
        out->getItem()->id == Item::coal_Id &&
        out->getAuxValue() == CoalItem::CHAR_COAL) {
        player->awardStat(GenericSta #endifnewableEnergy(),
                          GenericStats::param_renewableEnergy());
    }

    return out;
}
