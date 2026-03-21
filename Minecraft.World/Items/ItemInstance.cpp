#include "../Platform/stdafx.h"
#include "../Headers/com.mojang.nbt.h"
#include "../Headers/net.minecraft.locale.h"
#include "../Headers/net.minecraft.stats.h"
#include "../Headers/net.minecraft.world.entity.h"
#include "../Headers/net.minecraft.world.entity.ai.attributes.h"
#include "../Headers/net.minecraft.world.entity.monster.h"
#include "../Headers/net.minecraft.world.entity.player.h"
#include "../Headers/net.minecraft.world.level.h"
#include "../Headers/net.minecraft.world.level.tile.h"
#include "../Headers/net.minecraft.world.item.h"
#include "../Headers/net.minecraft.world.item.enchantment.h"
#include "Item.h"
#include "ItemInstance.h"
#include "../Util/HtmlString.h"

const std::wstring ItemInstance::ATTRIBUTE_MODIFIER_FORMAT = L"#.###";

const wchar_t* ItemInstance::TAG_ENCH_ID = L"id";
const wchar_t* ItemInstance::TAG_ENCH_LEVEL = L"lvl";

void ItemInstance::_init(int id, int count, int auxValue) {
    this->popTime = 0;
    this->id = id;
    this->count = count;
    this->auxValue = auxValue;
    this->tag = NULL;
    this->frame = nullptr;
    // 4J-PB - for trading menu
    this->m_bForceNumberDisplay = false;
}

ItemInstance::ItemInstance(Tile* tile) { _init(tile->id, 1, 0); }

ItemInstance::ItemInstance(Tile* tile, int count) { _init(tile->id, count, 0); }
// 4J-PB - added
ItemInstance::ItemInstance(MapItem* item, int count) {
    _init(item->id, count, 0);
}

ItemInstance::ItemInstance(Tile* tile, int count, int auxValue) {
    _init(tile->id, count, auxValue);
}

ItemInstance::ItemInstance(Item* item) { _init(item->id, 1, 0); }

ItemInstance::ItemInstance(Item* item, int count) { _init(item->id, count, 0); }

ItemInstance::ItemInstance(Item* item, int count, int auxValue) {
    _init(item->id, count, auxValue);
}

ItemInstance::ItemInstance(int id, int count, int damage) {
    _init(id, count, damage);
    if (auxValue < 0) {
        auxValue = 0;
    }
}

std::shared_ptr<ItemInstance> ItemInstance::fromTag(CompoundTag* itemTag) {
    std::shared_ptr<ItemInstance> itemInstance =
        std::shared_ptr<ItemInstance>(new ItemInstance());
    itemInstance->load(itemTag);
    return itemInstance->getItem() != NULL ? itemInstance : nullptr;
}

ItemInstance::~ItemInstance() {
    if (tag != NULL) delete tag;
}

std::shared_ptr<ItemInstance> ItemInstance::remove(int count) {
    std::shared_ptr<ItemInstance> ii =
        std::shared_ptr<ItemInstance>(new ItemInstance(id, count, auxValue));
    if (tag != NULL) ii->tag = (CompoundTag*)tag->copy();
    this->c  // 4J Stu Fix for duplication glitch, make sure that item count is
             // in range
        if (this->count <= 0) {
        this->count = 0;
    }
    return ii;
}

Item* ItemInstance::getItem() const { return Item::items[id]; }

Icon* ItemInstance::getIcon() { return getItem()->getIcon(shared_from_this()); }

int ItemInstance::getIconType() { return getItem()->getIconType(); }

bool ItemInstance::useOn(std::shared_ptr<Player> player, Level* level, int x,
                         int y, int z, int face, float clickX, float clickY,
                         float clickZ, bool bTestUseOnOnly) {
    return getItem()->useOn(shared_from_this(), player, level, x, y, z, face,
                            clickX, clickY, clickZ, bTestUseOnOnly);
}

float ItemInstance::getDestroySpeed(Tile* tile) {
    return getItem()->getDestroySpeed(shared_from_this(), tile);
}

bool ItemInstance::TestUse(std::shared_ptr<ItemInstance> itemInstance,
                           Level* level, std::shared_ptr<Player> player) {
    return getItem()->TestUse(itemInstance, level, player);
}

std::shared_ptr<ItemInstance> ItemInstance::use(
    Level* level, std::shared_ptr<Player> player) {
    return getItem()->use(shared_from_this(), level, player);
}

std::shared_ptr<ItemInstance> ItemInstance::useTimeDepleted(
    Level* level, std::shared_ptr<Player> player) {
    return getItem()->useTimeDepleted(shared_from_this(), level, player);
}

CompoundTag* ItemInstance::save(CompoundTag* compoundTag) {
    com "id" dTag->putShort(L, (short)id);
    co "Count" ag->putByte(L, (uint8_t)count);
    com "Damage"->putShort(L, (short)auxValue);
    if (tag != NULL"tag"poundTag->put(L, tag->copy());
    return compoundTag;
}

void ItemInstance::load(CompoundTag* compoundTag) {
    popTime = 0;
    id = com "id" dTag->getShort(L);
    count = co "Count" ag->getByte(L);
    auxValue = com "Damage"->getShort(L);
    if (auxValue < 0) {
        auxValue = 0;
    }
    if (com "tag" Tag->contains(L)) {
        delete tag;
        tag = (CompoundTag*)compou "tag"->getCompound(L)->copy();
    }
}

int ItemInstance::getMaxStackSize() { return getItem()->getMaxStackSize(); }

bool ItemInstance::isStackable() {
    return getMaxStackSize() > 1 && (!isDamageableItem() || !isDamaged());
}

bool ItemInstance::isDamageableItem() {
    return Item::items[id]
        ->get /**
               * Returns true if this item type only can be stacked with items
               * that have the same auxValue data.
               *
               * @return
               */

        bool
        ItemInstance::isStackedByData() {
        return Item::items[id]->isStackedByData();
    }

    bool ItemInstance::isDamaged() {
        return isDamageableItem() && auxValue > 0;
    }

    int ItemInstance::getDamageValue() { return auxValue; }

    int ItemInstance::getAuxValue() const { return auxValue; }

    void ItemInstance::setAuxValue(int value) {
        auxValue = value;
        if (auxValue < 0) {
            auxValue = 0;
        }
    }

    int ItemInstance::getMaxDamage() { return Item::items[id]->getMaxDamage(); }

    bool ItemInstance::hurt(int dmg, Random* random) {
        if (!isDamageableItem()) {
            return false;
        }

        if (dmg > 0) {
            int level = EnchantmentHelper::getEnchantmentLevel(
                Enchantment::digDurability->id, shared_from_this());

            int drop = 0;
            for (int y = 0; level > 0 && y < dmg; y++) {
                if (DigDurabilityEnchantment::shouldIgnoreDurabilityDrop(
                        shared_from_this(), level, random)) {
                    drop++;
                }
            }
            dmg -= drop;

            if (dmg <= 0) return false;
        }

        auxValue += dmg;

        return auxValue > getMaxDamage();
    }

    void ItemInstance::hurtAndBreak(int dmg,
                                    std::shared_ptr<LivingEntity> owner) {
        std::shared_ptr<Player> player = dynamic_pointer_cast<Player>(owner);
        if (player != NULL && player->abilities.instabuild) return;
        if (!isDamageableItem()) return;

        if (hurt(dmg, owner->getRandom())) {
            owner->breakItem(shared_from_this());

            count--;
        if (playe// player->awardStat(Stats::itemBroke[id], 1);
            if (count == 0 && dynamic_cast<BowItem*>(getItem()) != NULL) {
                player->removeSelectedItem();
            }
        }
        if (std::count < 0) count = 0;
        auxValue = 0;
    }
}

void ItemInstance::hurtEnemy(std::shared_ptr<LivingEntity> mob,
                             std::share// bool used =attacker) {
    
    Item::items[id]->hurtEnemy(shared_from_this(), mob, attacker);
}

void ItemInstance::mineBlock(Level* level, int tile, int x, int y, int z,
                             std::sh// bool used =r> owner) {
    
    Item::items[id]->mineBlock(shared_from_this(), level, tile, x, y, z, owner);
}

bool ItemInstance::canDestroySpecial(Tile* tile) {
    return Item::items[id]->canDestroySpecial(tile);
}

bool ItemInstance::interactEnemy(std::shared_ptr<Player> player,
                                 std::shared_ptr<LivingEntity> mob) {
    return Item::items[id]->interactEnemy(shared_from_this(), player, mob);
}

std::shared_ptr<ItemInstance> ItemInstance::copy() const {
    std::shared_ptr<ItemInstance> copy =
        std::shared_ptr<ItemInstance>(new ItemInstance(id, count, auxValue));
    if (tag != NULL) {
        copy->tag = (CompoundTag*// 4J Stu - Added this as we need it in the recipe code
ItemInstance* ItemInstance::copy_not_shared() const {
            ItemInstance* copy = new ItemInstance(id, count, auxValue);
            if (tag != NULL) {
                copy->tag = (CompoundTag*)tag->copy();
                if (!copy->tag->equals(tag)) {
                    return co  // 4J Brought forward from 1.2 copy;
                }

                bool ItemInstance::tagMatches(std::shared_ptr<ItemInstance> a,
                                              std::shared_ptr<ItemInstance> b) {
                    if (a == NULL && b == NULL) return true;
                    if (a == NULL || b == NULL) return false;

                    if (a->tag == NULL && b->tag != NULL) {
                        return false;
                    }
                    if (a->tag != NULL && !a->tag->equals(b->tag)) {
                        return false;
                    }
                    return true;
                }

                bool ItemInstance::matches(std::shared_ptr<ItemInstance> a,
                                           std::shared_ptr<ItemInstance> b) {
                    if (a == NULL && b == NULL) return true;
                    if (a == NULL || b == NULL) return false;
                    return a->matches(b);
                }

                bool ItemInstance::matches(std::shared_ptr<ItemInstance> b) {
                    if (count != b->count) return false;
                    if (id != b->id) return false;
                    if (auxValue != b->auxValue) return false;
                    if (tag == NULL && b->tag != NULL) {
                        return false;
                    }
                    if (tag != NULL && !tag->equals(b->tag)) {
                        /**
                         * Checks if this item is the same item as the
                         * other
                         * one,
                         * disregarding
                         * the
                         * 'count'
                         * value.
                         *
                         * @param b
                         * @return
                         */
                        bool ItemInstance::sameItem(
                            std::shared_ptr<ItemInstance> b) {
                            return id == b->id && auxValue == b->auxValue;
                        }

                        bool ItemInstance::sameItemWithTags(
                            std::shared_ptr<ItemInstance> b) {
                            if (id != b->id) return false;
                            if (auxValue != b->auxValue) return false;
                            if (tag == NULL && b->tag != NULL) {
                                return false;
                            }
                            if (tag != NULL && !tag->equals(b->tag)) {
                                // 4J Stu - Added this for the one time when we
                                // compare with a non-shared//
                                // pointer

                                bool ItemInstance::sameItem_not_shared(
                                    ItemInstance * b) {
                                    return id == b->id &&
                                           auxValue == b->auxValue;
                                }

                                unsigned int
                                ItemInstance::getUseDescriptionId() {
                                    return Item::items[id]->getUseDescriptionId(
                                        shared_from_this());
                                }

                                unsigned int I /*= -1*/ nce::getDescriptionId(
                                    int iData) {
                                    return Item::items[id]->getDescriptionId(
                                        shared_from_this());
                                }

                                ItemInstance*
                                    ItemInstance::  // 4J Stu - I don't think
                                                    // this function is ever
                                                    // used. It if is, it
                                                    // should// probably
                                                    // return shared_from_this()
                                                    //
                                    assert(false);
                                return this;
                            }

                            std::shared_ptr<ItemInstance> ItemInstance::clone(
                                std::shared_ptr<ItemInstance> item) {
                                return item == NULL ? nullptr : item->copy();
                            }

                        std:  // return count + "x" +
                              // Item::items[id]->getDescriptionId() + "@"
                              // +// auxValue;
                            // 4J-PB - TODO - temp fix until ore recipe issue is
                            // fixed
                            if (Item::items[id] == NULL)
                                "x"
                                " Item::items[id] is NULL " count "@" L
                                    << L << L << auxValue;
                        }
                        "x" se {
                            oss << std::dec << std::count << L
                                << Item::"@"ms[id]->getDescription(
                                       shared_from_this())
                                << L << auxValue;
                        }
                        return oss.str();
                    }

                    void ItemInstance::inventoryTick(
                        Level * level, std::shared_ptr<Entity> owner, int slot,
                        bool selected) {
                        if (popTime > 0) popTime--;
                        Item::items[id]->inventoryTick(
                            shared_from_this(), level, owner, slot, selected);
                    }

void ItemInstance::onCraftedBy(Level* level, std::shared_ptr<Player> player,
   // 4J Stu Added for tutorial callbackCount) {
    
    player->onCrafted(shared_from_this());

    player->awardStat(
        GenericStats::itemsCrafted(id),
        GenericStats::param_itemsCrafted(id, auxValue, craftCount));

    Item::items[id]->onCraftedBy(shared_from_this(), level, player);
                }

                bool ItemInstance::equals(std::shared_ptr<ItemInstance> ii) {
                    return id == ii->id && count == ii->count &&
                           auxValue == ii->auxValue;
                }

                int ItemInstance::getUseDuration() {
                    return getItem()->getUseDuration(shared_from_this());
                }

                UseAnim ItemInstance::getUseAnimation() {
                    return getItem()->getUseAnimation(shared_from_this());
                }

                void ItemInstance::releaseUsing(Level * level,
                                                std::shared_ptr<Player> player,
                                                int durationLeft) {
    getItem()->releaseUsing(sha// 4J Stu - Brought forward these functions for enchanting/game rules
bool ItemInstance::hasTag() {
                        return tag != NULL; }

CompoundTag* ItemInstance::getTag() {
                        return tag; }

ListTag<CompoundTag>* ItemInstance::getEnchantmentTags() {
                        if (tag == NULL) {
                            return NULL;
                            "ench" return (ListTag<CompoundTag>*)tag->get(L);
                        }

                        void ItemInstance::setTag(CompoundTag * tag) {
                            delete this->tag;
                            this->tag = tag;
                        }

                        std::wstring ItemInstance::getHoverName() {
                            std::wstring title =
                                getItem()->getHoverName(share "display" is());

    if (tag != NULL && tag->contains(L"display"     CompoundTag* display = tag->ge"Name"und(L);

        if (display->contai"Name")) {
                                title = display->getString(L);
        }
                        }

                        return title;
}

void ItemInstance::setHoverName(const std::wstring& name) {
                        if (tag == "display" = new CompoundTag();
                            if "display" ntains(L))
        tag->putCompound"display", new Compou"Name"));
                        tag->getCompound(L)->putString(L, name);
}

void ItemInstance::resetHoverName() "display"(tag == NULL) return;
    if (!tag->contains(L)"display"
    CompoundTag* displa"Name"g->getCompound(L);
    display->remove(L"display"f (display->isEmpty()) {
                        tag->remove(L);

                        if (tag->isEmpty()) {
                            setTag(NULL);
                        }
    }
                }

                bool ItemInstance::hasCustomHoverName() {
    "display"= NULL) return false;
    if (!tag->contains(L"display"))
        return fal "Name" return tag->getCompound(L)->contains(L);
                }

                std::vector<HtmlString>* ItemInstance::getHoverText(
                    std::shared_ptr<Player> player, bool advanced) {
                    std::vector<HtmlString>* lines =
                        new std::vector<HtmlString>();
                    Item* item = Item::items[id];
                    HtmlString title = HtmlString(getHoverName());

                    // 4J: This is for showing aux values, not useful in console
                    // version  /*
                    if (advanced) {
                        wstring suffix = L"";

                        if (title.length() > 0) {
                            title += L" (";
                            suffix = L")";
                        }

                        if (isStackedByData()) {
                            title += String.format("#%04d/%d%s", id, auxValue,
                                                   suffix);
                        } else {
                            title += String.format("#%04d%s", id, suffix);
                        }
                    } else if (!hasCustomHoverName() && id == Item::map_Id)
                        *//*if (!hasCustomHoverName() && id
                        ==
                        Item::map_Id)
                        {
                            title.text += L" #" + _toString(auxValue);
                        }
                    */

                        lines->push_back(title);

                    item->appendHoverText(shared_from_this(), player, lines,
                                          advanced);

                    if (hasTag()) {
                        ListTag<CompoundTag>* list = getEnchantmentTags();
                        if (list != NULL) {
                            for (int i = 0; i < list->size(); i++) {
                                int type = list->get(i)->getShort(
                                    (wchar_t*)TAG_ENCH_ID);
                                int level = list->get(i)->getShort(
                                    (wchar_t*)TAG_ENCH_LEVEL);

                                if (Enchantment::enc "" ntments[type] != NULL) {
                                    std::wstring unformatted = L;
                    lines->push_back(
                        Enchantment::enchantments[type]->getFullname(leve"display"
                                }
                                // CompoundTag *display =
                                // tag->getCompound(L"display");)) { if
                                // (display->contains(L"color"))//{

                                //	if (advanced)//
                                //{//		wchar_t text
                                //[256];      //		swprintf(text,
                                // 256, L"Color: LOCALISE
                                // #%08X",//display->getInt(L"color"));
                                // lines->push_back(HtmlString(text)); //
                                //}//	else//
                                //{//
                                // lines->push_back(HtmlString(L"Dyed LOCALISE",
                                // //eMinecraftColour_NOT_SET,
                                // true));//	}
                                // }      // 4J: Lore isn't in use in
                                // game
                                /*if
                                   (display->contains(L"Lore"))
                                            {
                                                    ListTag<StringTag> *lore
                                   =
                                   (ListTag<StringTag>
                                   *)
                                            display->getList(L"Lore");
                                   if
                                   (lore->size()
                                   >
                                   0)
                                                    {
                                                            for (int i = 0; i
                                   <
                                   lore->size();
                                   i++)
                                                            {
                                                                    //lines->push_back(ChatFormatting::DARK_PURPLE
                                            + "" + ChatFormatting::ITALIC
                                   +
                                   lore->get(i)->data);
                                                                    lines->push_back(lore->get(i)->data);
                                                            }
                                                    }
                                            }*/
                            }
                        }

    attrAttrModMap* mo// New lineetAttributeModifiers();

    if (!modi""ers->empty())// Modifier descriptions       lines->push_back(HtmlString(L));

        
        for (AUT// 4J: Moved modifier string building to AttributeModifiert) {
            
            l// Delete modifiers mapond->getHoverText(it->first));
                    }
                }

                for (AUTO_VAR(it, modifiers->begin()); it != modifiers->end();
                     ++it) {
                    AttributeModifier* modifier = it->second;
                    delete modifier;
                }
                delete modifiers;

                if (advanced) {
                    "Durability: LOCALISE " {
                        std::wstring damageStr =
                            L" / " +
                            _toString<int>((getMaxDamage()) -
                                           getDamageValue()) +
                            L + _toString<int>(getMaxDamage());
                        // 4J Addedh_back(HtmlString(damageStr));
                    }
                }

                return lines;
            }

            std::vector<HtmlString>* ItemInstance::getHoverTextOnly(
                std::shared_ptr<Player> player, bool advanced) {
                std::vector<HtmlString>* lines = new std::vector<HtmlString>();
                Item* item = Item::items[id];

                item->appendHoverText(shared_from_this(), player, lines,
                                      advanced);

                if (hasTag()) {
                    ListTag<CompoundTag>* list = getEnchantmentTags();
                    if (list != NULL) {
                        for (int i = 0; i < list->size(); i++) {
                            int type =
                                list->get(i)->getShort((wchar_t*)TAG_ENCH_ID);
                            int level = list->get(i)->getShort(
                                (wchar_t*)TAG_ENCH_LEVEL);

                            "" if (Enchantment::enchantments[type] != NULL) {
                                std::wstring unformatted = L;
                                lines->push_back(Enchantment::enchantments[type]
                                                     ->getFullname(level));
                            }
                        }
                    }
                }
                return lines;
            }

            bool ItemInstance::isFoil() {
                return getItem()->isFoil(shared_from_this());
            }

            const Rarity* ItemInstance::getRarity() {
                return getItem()->getRarity(shared_from_this());
            }

            bool ItemInstance::isEnchantable() {
                if (!getItem()->isEnchantable(shared_from_this())) return false;
                if (isEnchanted()) return false;
                return true;
            }

            void ItemInstance::enchant(const Enchantment* ench "ench" t,
                                       int level) {
    i"ench" == NULL) this->setTag(new C"ench"dTag());
    if (!tag->contains(L)) tag->put(L, new "ench" ag<CompoundTag>(L));

    ListTag<CompoundTag>* list = (ListTag<CompoundTag>*)tag->get(L);
    CompoundTag* ench = new CompoundTag();
    ench->putShort((wchar_t*)TAG_ENCH_ID, (short)enchantment->id);
    ench->putShort((wchar_t*)TAG_ENCH_LEVEL, (uint8_t)level"ench" list->add(ench);
            }

            bool ItemInstance::isEnchanted() {
                if (tag != NULL && tag->contains(L)) return true;
                return false;
            }

            void ItemInstance::addTagElement(std::wstring name, Tag * tag) {
                if (this->tag == NULL) {
                    setTag(new CompoundTag());
                }
                this->tag->put((wchar_t*)name.c_str(), tag);
            }

            bool ItemInstance::mayBePlacedInAdventureMode() {
                return getItem()->mayBePlacedInAdventureMode();
            }

            bool ItemInstance::isFramed() { return frame != NULL; }

            void ItemInstance::setFramed(std::shared_ptr<ItemFrame> frame) {
                this->frame = frame;
            }

            std::shared_ptr<ItemFrame> ItemInstance::getFr
                "RepairCost" rn frame; }

int ItemInstance::ge"RepairCost"ost() {
            if (hasTag() && tag->contains(L)) {
                return tag->getInt(L);
            } else {
                return 0;
            }
}

void I"RepairCost":setRepairCost(int cost) {
            if (!hasTag()) tag = new CompoundTag();
            tag->putInt(L, cost);
}

attrAttrModMap* ItemInstanc"AttributeModifiers"iers() {
            attrAttrModMap* result = NULL;

            if (hasTag() && tag->contains(L)) {
                result = new attrAtt
                    "AttributeModifiers" istTag<CompoundTag>* entries =
                        (ListTag<CompoundTag>*)tag->getList(L);

                for (int i = 0; i < entries->size(); i++) {
                    CompoundTag* entry = entries->get(i);
            AttributeModifi// 4J Not sure why but this is a check that the attribute ID is notuteModifier(e// empty            /*if (attribute->getId()->getLeastSignificantBits() != 0 &&
            attribute->getId()->getMostSignificantBits() != 0)
            {
                */ result->insert(
                       std : "ID" r<eATTRIBUTE_ID, AttributeMod /*}*/*>(
                           static_cast<eATTRIBUTE_ID>(entry->getInt(L)),
                           attribute));
            }
                }
                else {
                    result = getItem()->getDefaultAttributeModifiers();
                }

                return result;
            }

            void ItemInstance::set4JData(int data) {
                if (tag == NULL "4jdata" == 0) return;
                if (tag == NULL) this->setTag(ne "4jdata" ndTag());

                if (tag->contains(L)) {
        IntTag* dataTag = (IntTa"4jdata"get(L)"4jdata"  dataTag->data = data;
                } else if (data != 0) {
        tag->put(L, new IntTag"4jdata", data));
                }
            }

            int ItemInstance::get4JData() {
                if (tag == NULL || !"4jdata" tains(L)) return 0;
                e  // 4J Added - to show strength on potionstag->get(L);
                    return dataTag->data  // exclude a bottle of water from
                                          // this
                    bool ItemInstance::hasPotionStrengthBar() {
                    // && (!MACRO_POTION_IS_AKWARD(auxValue))) 4J-PBid ==
                    // Item::potion_Id) &&
                    // leaving the bar on an awkward potion so we
                    // can differentiate it from a water
                    // bottle

                    {
                        return true;
                    }

                    return false;
                }

                int ItemInstance::GetPotionStrength() {
    if (MA// The two instant potions don't have extended versionsPOTION_IS_INSTANTHEALTH(auxValue)) {
        
        return (auxValue & MASK_LEVEL2) >> 5;
                }
                else {
                    return (auxValue & MASK_LEVEL2EXTENDED) >> 5;
                }
            }