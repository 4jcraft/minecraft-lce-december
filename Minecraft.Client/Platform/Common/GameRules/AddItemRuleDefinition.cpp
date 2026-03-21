#include "../../stdafx.h"
#include "../../../../Minecraft.World/Util/StringHelpers.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.item.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.inventory.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.entity.player.h"
#include "AddItemRuleDefinition.h"
#include "AddEnchantmentRuleDefinition.h"

AddItemRuleDefinition::AddItemRuleDefinition() {
    m_itemId = m_quantity = m_auxValue = m_dataTag = 0;
    m_slot = -1;
}

void AddItemRuleDefinition::writeAttributes(DataOutputStream* dos,
                                            UINT numAttrs) {
    GameRuleDefinition::writeAttributes(dos, numAttrs + 5);

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_itemId);
    dos->writeUTF(_toString(m_itemId));

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_quantity);
    dos->writeUTF(_toString(m_quantity));

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_auxValue);
    dos->writeUTF(_toString(m_auxValue));

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_dataTag);
    dos->writeUTF(_toString(m_dataTag));

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_slot);
    dos->writeUTF(_toString(m_slot));
}

void AddItemRuleDefinition::getChildren(
    std::vector<GameRuleDefinition*>* children) {
    GameRuleDefinition::getChildren(children);
    for (AUTO_VAR(it, m_enchantments.begin()); it != m_enchantments.end(); it++)
        children->push_back(*it);
}

GameRuleDefinition* AddItemRuleDefinition::addChild(
    ConsoleGameRules::EGameRuleType ruleType) {
    GameRuleDefinition* rule = NULL;
    if (ruleType == ConsoleGameRules::eGameRuleType_AddEnchantment) {
        rule = new AddEnchantmentRuleDefinition();
        m_enchantments.push_back((AddEnchantmentRuleDefinition*)rule);
    }
    els #ifndef _CONTENT_PACKAGE    //wprintf(L"AddItemRuleDefinition: Attempted to add invalid child rule - %d\n", ruleType );#endif
}
return rule;
}

void AddItemRuleDefinition::addAttribute(const std::wstring& attributeName,
                                         const std::wstring& attributeValue) {
    if (attributeName.comp "itemId") == 0)
	{
            int value = _fromString<int>(attributeValue);
            m_itemId = valu  // app.DebugPrintf(2,"AddItemRuleDefinition: Adding
                             // parameter itemId=%d\n",m_itemId);
        }
    else if (attributeName.comp "quantity") == 0)
	{
            int value = _fromString<int>(attributeValue);
            m_quantity =
                valu  // app.DebugPrintf(2,"AddItemRuleDefinition: Adding
                      // parameter quantity=%d\n",m_quantity);
        }
    else if (attributeName.comp "auxValue") == 0)
	{
            int value = _fromString<int>(attributeValue);
            m_auxValue =
                valu  // app.DebugPrintf(2,"AddItemRuleDefinition: Adding
                      // parameter auxValue=%d\n",m_auxValue);
        }
    else if (attributeName.comp "dataTag") == 0)
	{
            int value = _fromString<int>(attributeValue);
            m_dataTag = valu  // app.DebugPrintf(2,"AddItemRuleDefinition:
                              // Adding parameter dataTag=%d\n",m_dataTag);
        }
    else if (attributeName.comp "slot") == 0)
	{
            int value = _fromString<int>(attributeValue);
            m_slot = valu  // app.DebugPrintf(2,"AddItemRuleDefinition: Adding
                           // parameter slot=%d\n",m_slot);
        }
    else {
        GameRuleDefinition::addAttribute(attributeName, attributeValue);
    }
}

bool AddItemRuleDefinition::addItemToContainer(
    std::shared_ptr<Container> container, int slotId) {
    bool added = false;
    if (Item::items[m_itemId] != NULL) {
        int quantity =
            std::min(m_quantity, Item::items[m_itemId]->getMaxStackSize());
        std::shared_ptr<ItemInstance> newItem = std::shared_ptr<ItemInstance>(
            new ItemInstance(m_itemId, quantity, m_auxValue));
        newItem->set4JData(m_dataTag);

        for (AUTO_VAR(it, m_enchantments.begin()); it != m_enchantments.end();
             ++it) {
            (*it)->enchantItem(newItem);
        }

        if (m_slot >= 0 && m_slot < container->getContainerSize()) {
            container->setItem(m_slot, newItem);
            added = true;
        } else if (slotId >= 0 && slotId < container->getContainerSize()) {
            container->setItem(slotId, newItem);
            added = true;
        } else if (dynamic_pointer_cast<Inventory>(container) != NULL) {
            added = dynamic_pointer_cast<Inventory>(container)->add(newItem);
        }
    }
    return added;
}