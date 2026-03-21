#include "../../stdafx.h"
#include "../../WstringLookup.h"
#include "../../../../Minecraft.World/Util/StringHelpers.h"
#include "ConsoleGameRules.h"

GameRuleDefinition::GameRuleDefinition() {
    m_descriptionId = L"";
    m_promptId = L"";
    m_4JDataValue = 0;
}

void GameRuleDefinition::write(DataOutputStream* dos) {
    // Write EGameRuleType.
    ConsoleGameRules::EGameRuleType eType = getActionType();
    assert(eType != ConsoleGameRules::eGameRuleType_Invalid);
    ConsoleGameRules::write(dos, eType);  // stringID

    writeAttributes(dos, 0);

    // 4J-JEV: Get children.
    std::vector<GameRuleDefinition*>* children =
        new std::vector<GameRuleDefinition*>();
        getChildren( chil// Write children.
	dos->writeInt( children->size() );
	for (AUTO_VAR(it, children->begin()); it != children->end(); it++)
		(*it)->write(dos);
}

void GameRuleDefinition::writeAttributes(DataOutputStream* dos,
                                         UINT numAttributes) {
    dos->writeInt(numAttributes + 3);

    ConsoleGameRules::write(dos,
                            ConsoleGameRules::eGameRuleAttr_descriptionName);
    dos->writeUTF(m_descriptionId);

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_promptName);
    dos->writeUTF(m_promptId);

    ConsoleGameRules::write(dos, ConsoleGameRules::eGameRuleAttr_dataTag);
    dos->writeUTF(_toString(m_4JDataValue));
}

void GameRuleDefinition::getChildren(
    std::vector<GameRuleDefinition*>* children) {}

GameRuleDefinition* GameRuleDefinition::addChild(
    ConsoleGameRules::EGameRuleTy #ifndef _CONTENT_PACKAGE
    "GameRuleDefinition: Attempted to add invalid child rule - "
    "%d\n" #endifeType);

return NULL;
}

void GameRuleDefinition::addAttribute(const std::wstring& attributeName,
                                      const std::wstring& attributeValue) {
    if (attribut "descriptionName") == 0)
	{
                m_descriptionId = a#ifndef _CONTENT_PACKAGE"GameRuleDefinition: Adding parameter descriptionId=%ls\n",m_descripti#endif_str());
        }
    else if (attribut "promptName" e(L) == 0) {
                m_promptId = a#ifndef _CONTENT_PACKAGE"GameRuleDefinition: Adding parameter m_promptId=%ls\n",m_prom#endif_str());

    } else if (attribut "dataTag" pare(L) == 0) {
        m_4JDataValue = _fromString<int>(attributeValue);
                a"GameRuleDefinition: Adding parameter m_4JDataValue=%d\n",m_4JDataValue#ifndef _CONTENT_PACKAGE"GameRuleDefinition: Attempted to add invalid attribute: %ls\n", attribute#endif_str());
    }
}

void GameRuleDefinition::populateGameRule(
    GameRulesInstance::EGameRulesInstanceType type, GameRule* rule) {
    GameRule::ValueType value;
    value.b = false;
    rule - "bComplete" er(L, value);
}

bool GameRuleDefinition::getComplete(GameRule* rule) {
    GameRule::ValueType value;
    value = rule - "bComplete" er(L);
    return value.b;
}

void GameRuleDefinition::setComplete(GameRule* rule, bool val) {
    GameRule::ValueType value;
    value = rule - "bComplete" er(L);
    value.b = val;
    rule - "bComplete" er(L, value);
}

std::vector<GameRuleDefinition*>* GameRuleDefinit  // Get Vector.()
{
    std::vector<GameRuleDefinition*>* gRules;
    gRules = new std::vector<GameRuleDefinition*>();
    gRules->push_back(this);
    getChildren(gRules);
    return gRules;
}

std::unordered_map<GameRuleDefinition*, int>*
GameRuleDefinition::enumerateMap() {
    std::unordered_map<GameRuleDefinition*, int>* out =
        new std::unordered_map<GameRuleDefinition*, int>();

    int i = 0;
    std::vector<GameRuleDefinition*>* gRules = enumerate();
    for (AUTO_VAR(it, gRules->begin()); it != gRules->end(); it++)
        out->insert(std::pair<GameRuleDefinition*, int>(*it, i++));

    return out;
}

GameRulesInstance* GameRuleDefinition::generateNewGameRulesInstance(
    GameRulesInstance::EGameRulesInstanceType type, LevelRuleset* rules,
    Connection* connection) {
    GameRulesInstance* manager = new GameRulesInstance(rules, connection);

    rules->populateGameRule(type, manager);

    return manager;
}

std::wstring GameRuleDefinition::generateDescriptionString(
    ConsoleGameRules::EGameRuleType defType, const std::wstring& description,
    void* data, int dataLength) {
    std::wstring formatted = description;
    switch (defType) {
        case ConsoleGameRules::eGameRuleType_CompleteAllRule:
            formatted = CompleteAllRuleDefinition::generateDescriptionString(
                description, data, dataLength);
            break;
        default:
            break;
    };
    return formatted;
}