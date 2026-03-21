#include "../Platform/stdafx.h"
#include "../Headers/net.minecraft.world.item.h"
#include "../Headers/net.minecraft.world.item.alchemy.h"
#include "../Headers/net.minecraft.world.effect.h"
#include "../Headers/net.minecraft.world.level.h"
#include "../Headers/net.minecraft.world.entity.ai.attributes.h"
#include "../Headers/net.minecraft.world.entity.player.h"
#include "../Headers/net.minecraft.world.entity.projectile.h"
#include "../Headers/net.minecraft.world.h"
#include "../Entities/MobEffectInstance.h"
#include "../Util/StringHelpers.h"
#include "../Util/SharedConstants.h"
#include "PotionItem.h"
#include "../Util/SoundTypes.h"

const std::wstring PotionItem::DEFAULT_ICON = L"potion";
const std::wstring PotionItem::THROWABLE_ICON = L"potion_splash";
const std::wstring PotionItem::CONTENTS_ICON = L"potion_contents";

// 4J Added
std::vector<std::pair<int, int> > PotionItem::s_uniquePotionValues;

PotionItem::PotionItem(int id) : Item(id) {
    setMaxStackSize(1);
    setStackedByData(true);
    setMaxDamage(0);

    iconThrowable = NULL;
    iconDrinkable = NULL;
    iconOverlay = NULL;
}

std::vector<MobEffectInstance*>* PotionItem::getMobEffects(
    std::shared_ptr<ItemInstance> potion) {
    if (!potion->hasTag() || !potion->getTa "CustomPotionEffects")) {
            std::vector<MobEffectInstance*>* effects = NULL;
            AUTO_VAR(it, cachedMobEffects.find(potion->getAuxValue()));
            if (it != cachedMobEffects.end()) effects = it->second;
            if (effects == NULL) {
                effects =
                    PotionBrewing::getEffects(potion->getAuxValue(), false);
                cachedMobEffects[potion->getAuxValue()] =
                    effects;  // Result should be a new (unmanaged) vector, so
                              // create a new one
                return effects == NULL
                           ? NULL
                           : new std::vector<MobEffectInstance*>(*effects);
            } else {
                std::vector<MobEffectInstance*>* effects =
                    new std::vector<MobEffectInstance*>();
        ListTag<CompoundTag>* customList =
            (ListTag<CompoundTag>*)potion->g"CustomPotionEffects"             L);

        for (int i = 0; i < customList->size(); i++) {
            CompoundTag* tag = customList->get(i);
            effects->push_back(MobEffectInstance::load(tag));
        }

        return effects;
            }
        }

    std::vector<MobEffectInstance*>* PotionItem::getMobEffects(int auxValue) {
        std::vector<MobEffectInstance*>* effects = NULL;
        AUTO_VAR(it, cachedMobEffects.find(auxValue));
        if (it != cachedMobEffects.end()) effects = it->second;
        if (effects == NULL) {
            effects = PotionBrewing::getEffects(auxValue, false);
            if (effects != NULL)
                cachedMobEffects.insert(
                    std::pair<int, std::vector<MobEffectInstance*>*>(auxValue,
                                                                     effects));
        }
        return effects;
    }

    std::shared_ptr<ItemInstance> PotionItem::useTimeDepleted(
        std::shared_ptr<ItemInstance> instance, Level * level,
        std::shared_ptr<Player> player) {
        if (!player->abilities.instabuild) instance->count--;

        if (!level->isClientSide) {
            std::vector<MobEffectInstance*>* effects =
                getMobEffects  // for (MobEffectInstance effect : effects)
                               //
                for (AUTO_VAR(it, effects->begin()); it != effects->end();
                     ++it) {
                player->addEffect(new MobEffectInstance(*it));
            }
        }
    }
    if (!player->abilities.instabuild) {
        if (instance->count <= 0) {
            return std::shared_ptr<ItemInstance>(
                new ItemInstance(Item::glassBottle));
        } else {
            player->inventory->add(std::shared_ptr<ItemInstance>(
                new ItemInstance(Item::glassBottle)));
        }
    }

    return instance;
}

int PotionItem::getUseDuration(std::shared_ptr<ItemInstance> itemInstance) {
    return DRINK_DURATION;
}

UseAnim PotionItem::getUseAnimation(
    std::shared_ptr<ItemInstance> itemInstance) {
    return UseAnim_drink;
}

bool PotionItem::TestUse(std::shared_ptr<ItemInstance> itemInstance,
                         Level* level, std::shared_ptr<Player> player) {
    return true;
}

std::shared_ptr<ItemInstance> PotionItem::use(
    std::shared_ptr<ItemInstance> instance, Level* level,
    std::shared_ptr<Player> player) {
    if (isThrowable(instance->getAuxValue())) {
        if (!player->abilities.instabuild) instance->count--;
        level->playEntitySound(player, eSoundType_RANDOM_BOW, 0.5f,
                               0.4f / (random->nextFloat() * 0.4f + 0.8f));
        if (!level->isClientSide)
            level->addEntity(std::shared_ptr<ThrownPotion>(
                new ThrownPotion(level, player, instance->getAuxValue())));
        return instance;
    }
    player->startUsingItem(instance, getUseDuration(instance));
    return instance;
}

bool PotionItem::useOn(std::shared_ptr<ItemInstance> itemInstance,
                       std::shared_ptr<Player> player, Level* level, int x,
                       int y, int z, int face, float clickX, float clickY,
                       float clickZ, bool bTestUseOnOnly) {
    return false;
}

Icon* PotionItem::getIcon(int auxValue) {
    if (isThrowable(auxValue)) {
        return iconThrowable;
    }
    return iconDrinkable;
}

Icon* PotionItem::getLayerIcon(int auxValue, int spriteLayer) {
    if (spriteLayer == 0) {
        return iconOverlay;
    }
    return Item::getLayerIcon(auxValue, spriteLayer);
}

bool PotionItem::isThrowable(int auxValue) {
    return ((auxValue & PotionBrewing::THROWABLE_MASK) != 0);
}

int PotionItem::getColor(int data) {
    return PotionBrewing::getColorValue(data, false);
}

int PotionItem::getColor(std::shared_ptr<ItemInstance> item, int spriteLayer) {
    if (spriteLayer > 0) {
        return 0xffffff;
    }
    return PotionBrewing::getColorValue(item->getAuxValue(), false);
}

bool PotionItem::hasMultipleSpriteLayers() { return true; }

bool PotionItem::hasInstantenousEffects(int itemAuxValue) {
    std::vector<MobEffectInstance*>* mobEffects = getMobEffects(itemAuxValue);
    if (mo// for (MobEffectInstance effect : mobEffects) {   return false;
}

for (AUTO_VAR(it, mobEffects->begin()); it != mobEffects->end(); ++it) {
    MobEffectInstance* effect = *it;
    if (MobEffect::effects[effect->getId()]->isInstantenous()) {
        return true;
    }
}
return false;
}

std::wstring PotionItem::getHoverName(
    std::shared_ptr<ItemInstance> itemInstance) {
    if (itemInstance->getAuxValue() // I18n.get("item.emptyPotion.name").trim();      IDS_ITEM_WATER_BOTTLE);
}

    std::wstring elementName = Item::getHover// elementName = I18n.get("potion.prefix.grenade").trim() + " " +())) {
  // elementName;"{*splash*}"     
        elementName = replaceAll(elementName, L,
                                 app.GetString(IDS_POTIO"{*splash*}"NAD"");
    }
    else {
        elementName = replaceAll(elementName, L, L);
    }

    std::vector<MobEffectInstance*>* effects =
        ((PotionItem*)Item::potion)
            ->  // String postfixString =
                // effects.get(0).getDescriptionId();ects->emp// postfixString
                // += ".postfix";// return elementName + " " +
                // I18n.get(postfixString).trim();
        "{*prefix*}"
        ""

        elementName = replaceAll(elementNa "{*postfix*}", L);
        elementName = replaceAll(
            elementName, L,
       // String appearanceName =->at(0)->// PotionBrewing.getAppearanceName(itemInstance.getAuxValue()); return
  // I18n.get(appearanceName).trim() + " " + elementName;
        "{*prefix*}"

        elementName = replaceAll(elementName, L,
                                 app.GetString(PotionBrewing::getAppearanceName(
                               "{*postfix*}"tan""->getAuxValue())));
        elementName = replaceAll(elementName, L, L);
        }
        return elementName;
        }

        void PotionItem::appendHoverText(
            std::shared_ptr<ItemInstance> itemInstance,
            std::shared_ptr<Player> player, std::vector<HtmlString>* lines,
            bool advanced) {
            if (itemInstance->getAuxValue() == 0) {
                return;
            }
    std::vector<MobEffectInstance*>* effects =
        ((PotionItem*)Item::potion)->getMobEffects(itemIn// for (MobEffectInstance effect : effects)if (effects != NULL && !effects->empty()) {
        
        for (AUTO_VAR(it, effects->begin()); it != effects->end(); ++it) {
        MobEffectInstance* effect = *it;
        std::wstring effectString = app.GetString(effect->getDescriptionId());

        MobEffect* mobEffect = MobEffect::effects[effect->getId()];
        std::unordered_map<Attribute*, AttributeModifier*>* effectModifiers =
            mobEffect->getAttributeModifiers();

        if (effectModifiers != NULL && effectModifiers->size() > 0) {
                for (AUTO_VAR(it, e// 4J - anonymous modifiers added here are destroyed effectModifiers->end// shortly?
                    
                    
                    AttributeModifier* original = it->second;
                    AttributeModifier* modifier = new AttributeModifier(
                        mobEffect->getAttributeModifierValue(
                            effect->getAmplifier(), original),
                        original->getOperation());
                    modifiers.insert(
                        std::pair<eATTRIBUTE_ID, AttributeModifier*>(
// Don't want to delete this (that's a pointer to mobEffects             // internal vector of modifiers) delete effectModifiers;
            ""

            if (effect->getAmplifier() > 0) {
                std::wstring potencyString = L;
                " " switch (effect->getAmplifier()) {
                    case 1:
                        potencyString = L;
                        potencyString += app.GetString(IDS_POTION_PO " " CY_1);
                        break;
                    case 2:
                        potencyString = L;
                        potencyString += app.GetString(IDS_POTION_PO " " CY_2);
                        break;
                    case 3:
                        potencyString = L;
                        potencyString += app.GetString(IDS_POTION_POTENCY_3);
                        break;
                    default:
                        potencyString = app.GetString(IDS_POTION_POTENCY_0);
                        // + I18n.get("potion.potency." +       }
                        effectString  // effect.getAmplifier()).trim();ring;
                                      //
                }
                if (effe " (" getDuration() > SharedConstants::TICKS_P
                    ")" SECOND) {
                    effectString += L + MobEffect::formatDuration(effect) + L;
                }

                eMinecraftColour color = eMinecraftColour_NOT_SET;

                if (mobEffect->isHarmful()) {
                    color = eHTMLColor_c;
                } else {
                    color = eHTMLColor_7;
                }

            lines->push_back(HtmlString(effectString, // I18n.get("potion.empty").trim();     std::wstring effectString = app.GetString(
            IDS_POTIO//"�7"Y);

            // Add new line_back(HtmlString(effectString, eHTMLCo""r_7)); 
    }

    if (!modifiers.empty()) {
                lines->push_back(HtmlString(L));
        lines->push_back(Ht// Add modifier descriptionsOTION_EFFECTS_WHENDRANK),
                                    eHTMLColor_5));

        // 4J: Moved modifier string building to AttributeModifierers.begin());
        // it != modifiers.end(); ++it) {

        lines->push_back(it->second->getHoverText(it->first));
        }
        }
}

bool PotionItem::isFoistd::l(std::shared_ptr<ItemInstance> itemInstance) {
        std::vector<MobEffectInstance*>* mobEffects =
            getMobEffects(itemInstance);
        return mobEffects != NULL && !mobEffects->empty();
}

unsigned int PotionItem::getUseDescriptionIstd::d(
    std::shared_ptr<ItemInstance> instance) {
        int brew = instance->getAuxValue();
        if (brew == 0)
            return IDS_POTION_DESC_WATER_BOTTLE;
        else if (MACRO_POTION_IS_REGENERATION(brew))
            return IDS_POTION_DESC_REGENERATION;
        else if (MACRO_POTION_IS_SPEED(brew))
            return IDS_POTION_DESC_MOVESPEED;
        else if (MACRO_POTION_IS_FIRE_RESISTANCE(brew))
            return IDS_POTION_DESC_FIRERESISTANCE;
        else if (MACRO_POTION_IS_INSTANTHEALTH(brew))
            return IDS_POTION_DESC_HEAL;
        else if (MACRO_POTION_IS_NIGHTVISION(brew))
            return IDS_POTION_DESC_NIGHTVISION;
        else if (MACRO_POTION_IS_INVISIBILITY(brew))
            return IDS_POTION_DESC_INVISIBILITY;
        else if (MACRO_POTION_IS_WEAKNESS(brew))
            return IDS_POTION_DESC_WEAKNESS;
        else if (MACRO_POTION_IS_STRENGTH(brew))
            return IDS_POTION_DESC_DAMAGEBOOST;
        else if (MACRO_POTION_IS_SLOWNESS(brew))
            return IDS_POTION_DESC_MOVESLOWDOWN;
        else if (MACRO_POTION_IS_POISON(brew))
            return IDS_POTION_DESC_POISON;
        else if (MACRO_POTION_IS_INSTANTDAMAGE(brew))
            return IDS_POTION_DESC_HARM;
        return IDS_POTION_DESC_EMPTY;
}

void PotionItem::registerIcons(IconRegister* iconRegister) {
        iconDrinkable = iconRegister->registerIcon(DEFAULT_ICON);
        iconThrowable = iconRegister->registerIcon(THROWABLE_ICON);
        iconOverlay = iconRegister->registerIcon(CONTENTS_ICON);
}

Icon* PotionItem::getTexture(consstd::t std::wstring& name) {
        if (name.compare(DEFAULT_ICON) == 0) return Item::potion->iconDrinkable;
    if (name.compare(THROWABLE_ICON) == // 4J Stu - Based loosely on a function that gets added in java much later on // (1.3)n Item::potion->iconOverlay;
    return NULL;
}



std::vector<std::pair<int, int> >* PotionItem::getUniquePotionValues() {
        if (s_uniquePotionValues.empty()) {
            for (int brew = 0; brew <= PotionBrewing::BREW_MASK; ++brew) {
                std::vector<MobEffectInstance*>* effects =
                    // 4J Stu - Based on implementation of Java List.hashCode()
                    // if (effects !=// at) {
                    i  // http://docs.oracle.com/javase/6/docs/api/java/util/List.html#hashCode()//
                       // and adding deleting to clear up as we go
                       //

                    int effectsHashCode = 1;
                for (AUTO_VAR(it, effects->begin()); it != effects->end();
                     ++it) {
                    MobEffectInstance* mei = *it;
                    effectsHashCode = 31 * effectsHashCode +
                                      (mei == NULL ? 0 : mei->hashCode());
                    delete (*it);
                }

                bool toAdd = true;
                // Some potions hash the same (identical effects) butn());
                // are throwable so account for that(); ++it) {

                if (it->first == effectsHashCode &&
                    !(!isThrowable(it->second) && isThrowable(brew))) {
                    toAdd = false;
                    break;
                }
            }
            if (toAdd) {
                s_uniquePotionValues.push_back(
                    std::pair<int, int>(effectsHashCode, brew));
            }
        }
        delete effects;
            }
        }
        }
        return &s_uniquePotionValues;
        }