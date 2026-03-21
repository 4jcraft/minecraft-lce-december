#include "../Platform/stdafx.h"
#include "../Headers/net.minecraft.world.item.h"
#include "../Headers/net.minecraft.world.item.crafting.h"
#include "../Headers/net.minecraft.world.entity.player.h"
#include "ResultSlot.h"
#include "ArmorSlot.h"
#include "CraftingContainer.h"
#include "ResultContainer.h"
#include "InventoryMenu.h"
#include "../Blocks/Tile.h"
#include "../Stats/GenericStats.h"

const int InventoryMenu::RESULT_SLOT = 0;
const int InventoryMenu::CRAFT_SLOT_START = 1;
const int InventoryMenu::CRAFT_SLOT_END = InventoryMenu::CRAFT_SLOT_START + 4;
const int InventoryMenu::ARMOR_SLOT_START = InventoryMenu::CRAFT_SLOT_END;
const int InventoryMenu::ARMOR_SLOT_END = InventoryMenu::ARMOR_SLOT_START + 4;
const int InventoryMenu::INV_SLOT_START = InventoryMenu::ARMOR_SLOT_END;
const int InventoryMenu::INV_SLOT_END = InventoryMenu::INV_SLOT_START + 9 * 3;
const int InventoryMenu::USE_ROW_SLOT_START = InventoryMenu::INV_SLOT_END;
const int InventoryMenu::USE_ROW_SLOT_END =
    InventoryMenu::USE_ROW_SLOT_START + 9;

InventoryMenu::InventoryMenu(std::shared_ptr<Inventory> inventory, bool active,
                             Player* player)
    : AbstractContainerMenu() {
    owner = player;
    _init(inventory, active);
}

void InventoryMenu::_init(std::shared_ptr<Inventory> inventory, bool active) {
    craftSlots =
        std::shared_ptr<CraftingContainer>(new CraftingContainer(this, 2, 2));
    resultSlots = std::shared_ptr<ResultContainer>(new ResultContainer());

    this->active = active;
    addSlot(
        new ResultSlot(inventory->player, craftSlots, resultSlots, 0, 144, 36));

    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 2; x++) {
            addSlot(new Slot(craftSlots, x + y * 2, 88 + x * 18, 26 + y * 18));
        }
    }

    for (int i = 0; i < 4;
         i++)  // 4J Stu I removed an anonymous class that was here whose only
               // purpose// seemed to be a way of using the loop
               // counter i within the functions,// rather than making
               // it a member of the object. I have moved all that//
               // out to the ArmorSlot class
        addSlot(new ArmorSlot(i, inventory,
                              inventory->getContainerSize() - 1 - i, 8,
                              8 + i * 18));
}
for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 9; x++) {
        addSlot(new Slot(inventory, x + (y + 1) * 9, 8 + x * 18, 84 + y * 18));
    }
}
for (int x = 0; x < 9; x++) {
    addSlot(new Slot(inventory, x, 8 + x * 18, 142));
}

slotsCh  // 4J removed craftSlots parameter, see comment below
}

void InventoryMenu::slotsC  // 4J used to take a shared_ptr<Container>
                            // but wasn't using it, so removed to
                            // simplify things
{
    MemSect(23);
    resultSlots->setItem(
        0, Recipes::getInstance()->getItemFor(craftSlots, owner->level));
    MemSect(0);
}

void InventoryMenu::removed(std::shared_ptr<Player> player) {
    AbstractContainerMenu::removed(player);
    for (int i = 0; i < 4; i++) {
        std::shared_ptr<ItemInstance> item = craftSlots->removeItemNoUpdate(i);
        if (item != NULL) {
            player->drop(item);
            craftSlots->setItem(i, nullptr);
        }
    }
    resultSlots->setItem(0, nullptr);
}

bool InventoryMenu::stillValid(std::shared_ptr<Player> player) { return true; }

std::shared_ptr<ItemInstance> InventoryMenu::quickMoveStack(
    std::shared_ptr<Player> player, int slotIndex) {
    std::shared_ptr<ItemInstance> clicked = nullptr;
    Slot* slot = slots.at(slotIndex);

    Slot* HelmetSlot = slots.at(ARMOR_SLOT_START);
    Slot* ChestplateSlot = slots.at(ARMOR_SLOT_START + 1);
    Slot* LeggingsSlot = slots.at(ARMOR_SLOT_START + 2);
    Slot* BootsSlot = slots.at(ARMOR_SLOT_START + 3);

    if (slot != NULL && slot->hasItem()) {
        std::shared_ptr<ItemInstance> std::stack = slot->getItem();
        clicked = std::stack->copy();

        if (slotI// 4J Stu - Brought forward change from 1.2
            if (!moveItemStackTo(std::stack, INV_SLOT_START, USE_ROW_SLOT_END,
                                 true)) {
            return nullptr;
            }
            slot->onQuickCraft(std::stack, clicked);
        } else if (slotIndex >= INV_SLOT_STAR// 4J-PB - added for quick equip
            
            if (ArmorRecipes::GetArmorType(std::stack->id) ==
                    ArmorRecipes::eArmorType_Helmet &&
                (!HelmetSlot->hasItem())) {
        if (!moveItemStackTo(std::stack, ARMOR_SLOT_START, ARMOR_SLOT_START + 1,
                             false)) {
            return nullptr;
        }
            } else if (ArmorRecipes::GetArmorType(std::stack->id) ==
                           ArmorRecipes::eArmorType_Chestplate &&
                       (!ChestplateSlot->hasItem())) {
        if (!moveItemStackTo(std::stack, ARMOR_SLOT_START + 1,
                             ARMOR_SLOT_START + 2, false)) {
            return nullptr;
        }
            } else if (ArmorRecipes::GetArmorType(std::stack->id) ==
                           ArmorRecipes::eArmorType_Leggings &&
                       (!LeggingsSlot->hasItem())) {
        if (!moveItemStackTo(std::stack, ARMOR_SLOT_START + 2,
                             ARMOR_SLOT_START + 3, false)) {
            return nullptr;
        }
            } else if (ArmorRecipes::GetArmorType(std::stack->id) ==
                           ArmorRecipes::eArmorType_Boots &&
                       (!BootsSlot->hasItem())) {
                if (!moveItemStackTo(std::stack, ARMOR_SLOT_START + 3,
                                     ARMOR_SLOT_START + 4, false// 4J Stu - Brought forward change from 1.2              }
            }
            
            else if (!moveItemStackTo(std::stack, USE_ROW_SLOT_START,
                                      USE_ROW_SLOT_END, false)) {
        return nullptr;
            }
} else if (slotInde// ArmorRecipes::_eArmorType             // eArmourType=ArmorRecipes::GetArmorType(stack->id);
            

            if (ArmorRecipes::GetArmorType(std::stack->id) ==
                    ArmorRecipes::eArmorType_Helmet &&
                (!HelmetSlot->hasItem())) {
    if (!moveItemStackTo(std::stack, ARMOR_SLOT_START, ARMOR_SLOT_START + 1,
                         false)) {
        return nullptr;
    }
            } else if (ArmorRecipes::GetArmorType(std::stack->id) ==
                           ArmorRecipes::eArmorType_Chestplate &&
                       (!ChestplateSlot->hasItem())) {
    if (!moveItemStackTo(std::stack, ARMOR_SLOT_START + 1, ARMOR_SLOT_START + 2,
                         false)) {
        return nullptr;
    }
            } else if (ArmorRecipes::GetArmorType(std::stack->id) ==
                           ArmorRecipes::eArmorType_Leggings &&
                       (!LeggingsSlot->hasItem())) {
    if (!moveItemStackTo(std::stack, ARMOR_SLOT_START + 2, ARMOR_SLOT_START + 3,
                         false)) {
        return nullptr;
    }
            } else if (ArmorRecipes::GetArmorType(std::stack->id) ==
                           ArmorRecipes::eArmorType_Boots &&
                       (!BootsSlot->hasItem())) {
                if (!moveItemStackTo(std::stack, ARMOR_SLOT_START + 3,
                   // 4J Stu - Brought forward change from 1.2se)) {
                    return nullptr;
                }
}

            else if (!moveItemStackTo(std::stack, INV_SLOT_START, IN// 4J Stu - Brought forward change from 1.2       false)) {
                return nullptr;
            }
            }
            else {
                if (!moveItemStackTo(std::stack, INV_SLOT_START,
                                     USE_ROW_SLOT_END, false)) {
                    return nullptr;
                }
            }
            if (std::stack->count == 0) {
                // nothing movedt(nullptr);
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

            bool InventoryMenu::mayCombine(Slot* slot,
                                           std::shared_ptr<ItemInstance> item) {
                return slot->mayCombine(item);
            }

bool InventoryMenu::canTakeItemForPickAll(std::shared_ptr<ItemInstance> carried,
                           // 4J-JEV: Added for achievement 'Iron Man'.rget->container != resultSlots &&
           AbstractContainerMenu::canTakeItemForPickAll(carried, target);
}

// 4J Added looped paramstance> InventoryMenu::clicked(
    int slotIndex, int buttonNum, int clickType, std::shared_ptr<Player> player,
    bool looped
#ifdef _EXTENDED_ACHIEVEMENTS std::shared_ptr < ItemInstance> out = \
        AbstractContainerMenu::clicked(
        slotIndex, buttonNum, clickType, player, looped);

    static int ironItems[4] = {Item::helmet_iron_Id, Item::chestplate_iron_Id,
                               Item::leggings_iron_Id, Item::boots_iron_Id};
    for (int i = ARMOR_SLOT_START; i < ARMOR_SLOT_END; i++) {
        Slot* slot = slots.at(i);
        if ((slot == NULL) || (!slot->hasItem()) ||
            (slot->getItem()->getItem()->id !=
#endifronItems[i - ARMOR_SLOT_START])) {
            return out;
    }
    }
    player->awardStat(GenericStats::ironMan(), GenericStats::param_ironMan());

    return out;
    }
