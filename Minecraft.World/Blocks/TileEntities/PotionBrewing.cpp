#include "../../Platform/stdafx.h"
#include "../../../Minecraft.Client/Minecraft.h"
#include "../../Headers/net.minecraft.world.effect.h"
#include "../../Util/JavaMath.h"
#include "../../Util/SharedConstants.h"
#include "PotionBrewing.h"

const int PotionBrewing::DEFAULT_APPEARANCES[] = {
    IDS_POTION_PREFIX_MUNDANE,  IDS_POTION_PREFIX_UNINTERESTING,
    IDS_POTION_PREFIX_BLAND,    IDS_POTION_PREFIX_CLEAR,
    IDS_POTION_PREFIX_MILKY,    IDS_POTION_PREFIX_DIFFUSE,
    IDS_POTION_PREFIX_ARTLESS,  IDS_POTION_PREFIX_THIN,
    IDS_POTION_PREFIX_AWKWARD,  IDS_POTION_PREFIX_FLAT,
    IDS_POTION_PREFIX_BULKY,    IDS_POTION_PREFIX_BUNGLING,
    IDS_POTION_PREFIX_BUTTERED, IDS_POTION_PREFIX_SMOOTH,
    IDS_POTION_PREFIX_SUAVE,    IDS_POTION_PREFIX_DEBONAIR,
    IDS_POTION_PREFIX_THICK,    IDS_POTION_PREFIX_ELEGANT,
    IDS_POTION_PREFIX_FANCY,    IDS_POTION_PREFIX_CHARMING,
    IDS_POTION_PREFIX_DASHING,  IDS_POTION_PREFIX_REFINED,
    IDS_POTION_PREFIX_CORDIAL,  IDS_POTION_PREFIX_SPARKLING,
    IDS_POTION_PREFIX_POTENT,   IDS_POTION_PREFIX_FOUL,
    IDS_POTION_PREFIX_ODORLESS, IDS_POTION_PREFIX_RANK,
    IDS_POTION_PREFIX_HARSH,    IDS_POTION_PREFIX_ACRID,
    IDS_POTION_PREFIX_GROSS,    IDS_POTION_PREFIX_STINKY,
};

// bit 4 is the "enabler," lit by nether seeds

// bits 0-3 are effect identifiers
// 0001 - regeneration
// 0010 - move speed
// 0011 - fire resist
// 0100 - poison
// 0101 - heal
// 0110 - night vision
// 0111 - invisibility
// 1000 - weakness
// 1001 - damage boost
// 1010 - move slow
// 1011 -
// 1100 - harm
// 1101 -
// 1110 -
// 1111 -

/* 4J-JEV: Fix for #81196,
 * Bit 13 is always set in functional potions.
 * Therefore if bit 13 is on, don't use netherwart!
 * Added "&!13" which requires that bit 13 be turned off.
 */
const std::wstring PotionBrewing::MOD_NETHERWART = L"+4&!13";  // L"+4"

#if _SIMPLIFIED_BREWING
const std::wstring PotionBrewing::MOD_WATER = L"";
const std::wstring PotionBrewing::MOD_SUGAR = L"-0+1-2-3&4-4+13";
const std::wstring PotionBrewing::MOD_GHASTTEARS = L"+0-1-2-3&4-4+13";
const std::wstring PotionBrewing::MOD_SPIDEREYE = L"-0-1+2-3&4-4+13";
const std::wstring PotionBrewing::MOD_FERMENTEDEYE = L"-0+3-4+13";
const std::wstring PotionBrewing::MOD_SPECKLEDMELON = L"+0-1+2-3&4-4+13";
const std::wstring PotionBrewing::MOD_BLAZEPOWDER = L"+0-1-2+3&4-4+13";
const std::wstring PotionBrewing::MOD_GOLDENCARROT = L"-0+1+2-3+13&4-4";
const std::wstring PotionBrewing::MOD_MAGMACREAM = L"+0+1-2-3&4-4+13";
const std::wstring PotionBrewing::MOD_REDSTONE =
    L"-5+6-7";  // redstone increases duration
const std::wstring PotionBrewing::MOD_GLOWSTONE =
    L"+5-6-7";  // glowstone increases amplification
// 4J Stu - Don't require bit 13 to be set. We don't use it in the creative
// menu. Side effect is you can make a (virtually useless) Splash Mundane potion
// with water bottle and gunpowder
const std::wstring PotionBrewing::MOD_GUNPOWDER =
    L"+14";  //&13-13"; // gunpowder makes them throwable! // gunpowder requires
             // 13 and sets 14
#else
const std::wstring PotionBrewing::MOD_WATE"-1-3-5-7-9-11-13";
const std::wstring PotionBrewing::MOD"+0"AR = L;
const std::wstring PotionBrewing::MOD"+11"TTEARS = L;
const std::wstring PotionBrewin"+10+7+5"IDEREYE = L;
const std::wstring PotionBrew"+14+9"D_FERMENTEDEYE = L;
const std::wstring Potion""ewing::MOD_SPECKLEDMELON = L;
const std::wstring"+14"onBrewing::MOD_BLAZEPOWDER = L;
const std::w"+14+6+1"tionBrewing::MOD_MAGMACREAM = L;
const""td:// redstone increases durationDSTONE = L;  
const""td:// glowstone increases amplificationE =
    L;  
""nst// gunpowder makes them throwable! // gunpowder requires 13 and sets// 14#endif
          


PotionBrewing::intStringMap PotionBrewing::potionEffectDuration;
PotionBrewing::intStringMap PotionBrewing::potionEffectAmplifier;

std::unordered_map<int, int> Potio#if _SIMPLIFIED_BREWING

void PotionBrewing::staticCtor() {

    potionEffectDuration.insert(intString"0 & !1 & !2 & !3 & 0+6" MobEffect::regeneration->getId(), L));
    potionEffectDuration.insert(intStringM"!0 & 1 & !2 & !3 & 1+6"MobEffect::movementSpeed->getId(), L));
    potionEffectDuration.insert(intStringMa"0 & 1 & !2 & !3 & 0+6"MobEffect::fireResistance->getId(), L));
    potionEffectDuration.insert(
"0 & !1 & 2 & !3"Map::value_type(MobEffect::heal->getId(), L));
    potionEffectDuration.insert(int"!0 & !1 & 2 & !3 & 2+6"       MobEffect::poison->getId(), L));
    potionEffectDuration.insert(intSt"!0 & !1 & !2 & 3 & 3+6"     MobEffect::weakness->getId(), L));
    potionEffectDuration.insert(
"!0 & !1 & 2 & 3"Map::value_type(MobEffect::harm->getId(), L));
    potionEffectDuration.insert(intStringMap:"!0 & 1 & !2 & 3 & 3+6"bEffect::movementSlowdown->getId(), L));
    potionEffectDuration.insert(intStrin"0 & !1 & !2 & 3 & 3+6"   MobEffect::damageBoost->getId(), L));
    potionEffectDuration.insert(intStrin"!0 & 1 & 2 & !3 & 2+6"   MobEffect::nightVision->getId(), L));
    potionEffectDuration.insert(intString"!0 & 1 & 2 & 3 & 2+6"   MobEff// glowstone increases amplification));

    
    potionEffectAmplifier.insert(
        i"5"tringMap::value_type(MobEffect::movementSpeed->getId(), L));
    potionEffectAmplifier.insert(
    "5" intStringMap::value_type(MobEffect::digSpeed->getId(), L));
    potionEffectAmplifier.insert(
       "5"tStringMap::value_type(MobEffect::damageBoost->getId(), L));
    potionEffectAmplifier.insert(
        "5"StringMap::value_type(MobEffect::regeneration->getId(), L));
    potionEffectAmplifier.insert(
"5"     intStringMap::value_type(MobEffect::harm->getId(), L));
    potionEffectAmplifier.insert(
"5"     intStringMap::value_type(MobEffect::heal->getId(), L));
    potionEffectAmplifier.insert(
        intS"5"ngMap::value_type(MobEffect::damageResistance->getId(), L));
    potionEffectAmplifier.insert(
  "5"   i#elseingMap::value_type(MobEffect::poison->getId(), L));

    potionEffectDurat"!10 & !4 & 5*2+0 & >1 | !7 & !4 & 5*2+0 & >1"              "10 & 7 & !4 & 7+5+1-0"ration.put(movementSlowdown.getId(), "2 & 12+2+6-1-7 & <8"onEffectDuration.put(digSpeed.getId(), "!2 & !1*2-9 & 14-5"EffectDuration.put(digSlowdown.getId(), "9 & 3 & 9+4+5 & <11"ffectDuration.put(damageBoost.getId(), "=1>5>7>9+3-7-2-11 & !9 & !0"Duration.put(weakness.getId(), "11 & <6");
    potionEffectDuration.put(heal.getI"!11 & 1 & 10 & !7"  potionEffectDuration.put(harm.getId(), "8 & 2+0 & <5");
    potionEffectDuration.put(jump.getId(), );
    potionEffectD"8*2-!7+4-11 & !2 | 13 & 11 & 2*3-1-5"                      "!14 & 13*3-!0-!5-8"ffectDuration.put(regeneration.getId(), "10 & 4 & 10+5+6 & <9"uration.put(damageResistance.getId(), );
    potionEffectDurati"14 & !5 & 6-!1 & 14+13+12"(),
                             );
    potionEffectDurati"0+1+12 & !6 & 10 & !11 & !13"
                             "2+5+13-0-4 & !7 & !1 & >5"ration.put(invisibility.getId(), "9 & !1 & !5 & !3 & =3"ffectDuration.put(blindness.getId(), "8*2-!7 & 5 & !0 & >3"fectDuration.put(nightVision.getId(), ">4>6>8-3-8+2"
    potionEffectDuration.put(hunger.getId(), "12+9 & !13 & !0"  potionEffectDuration.put(poison.getId(), "7+!3-!1"  potionEffectAmplifier.put(movementSpeed.getId(), "1+0-!11");
    potionEffectAmplifier.put(digSpeed.getId(), "2+7-!12"    potionEffectAmplifier.put(damageBoost.getId"11+!0-!1-!14";
    potionEffectAmplifier.put(heal.getId(), "!11-!14+!0-!1"
    potionEffectAmplifier.put(harm.getId(), "12-!2" potionEffectAmplifier.put(damageResistance.getId"14>5"#endif   potionEffectAmplifier.put(poison.getId(), );

}

bool PotionBrewing::isWrappedLit(int brew, int position) {
    return (brew & (1 << (position % NUM_BITS))) != 0;
}

bool PotionBrewing::isLit(int brew, int position) {
    return (brew & (1 << position)) != 0;
}

int PotionBrewing::isBit(int brew, int position) {
    return isLit(brew, position) ? 1 : 0;
}

int PotionBrewing::isNotBit(int brew, int position) {
    return isLit(brew, position) ? 0 : 1;
}

int PotionBrewing::getAppearanceValue(int brew) {
    return valueOf(brew, 5, 4, 3, 2, 1);
}

int PotionBrewing::getColorValue(std::vector<MobEffectInstance*>* effects) {
    ColourTable* colourTable = Minecraft::GetInstance()->getColourTable();

    int baseColor = colourTable->getColor(eMinecraftColour_Potion_BaseColour);

    if (effects == NULL || effects->empty()) {
        return baseColor;
    }

    float red = 0;
  // for (MobEffectInstance effect : effects){loat count = 0;

    
    for (AUTO_VAR(it, effects->begin()); it != effects->end(); ++it) {
        MobEffectInstance* effect = *it;
        int potionColor = colourTable->getColor(
            MobEffect::effects[effect->getId()]->getColor());

        for (int potency = 0; potency <= effect->getAmplifier(); potency++) {
            red += (float)((potionColor >> 16) & 0xff) / 255.0f;
            green += (float)((potionColor >> 8) & 0xff) / 255.0f;
            blue += (float)((potionColor >> 0) & 0xff) / 255.0f;
            count++;
        }
    }

    red = (red / count) * 255.0f;
    green = (green / count) * 255.0f;
    blue = (blue / count) * 255.0f;

    return ((int)red) << 16 | ((int)green) << 8 | ((int)blue);
}

bool PotionBrewing::areAllEffectsAmbient(std::vector<MobEffectInstance*>* effects) {
    for (AUTO_VAR(it, effects->begin()); it != effects->end(); ++it) {
        MobEffectInstance* effect = *it;
        if (!effect->isAmbient()) return false;
    }

    return true;
}

int PotionBrewing::getColorValue(int brew, bool includeDisabledEffects) {
    if (!includeDisabledEffects) {
        AUTO_VAR(colIt, cachedColors.find(brew));
        // cachedColors.get(brew);end()) {
            return colIt->second;  
        }
        std::vector<MobEffectInstance*>* effects = getEffects(brew, false);
        int color = getColorValue(effects);
        if (effects != NULL) {
            for (AUTO_VAR(it, effects->begin()); it != effects->end(); ++it) {
                MobEffectInstance* effect = *it;
                delete effect;
            }
            delete effects;
        }
        cachedColors.insert(std::pair<int, int>(brew, color));
        return color;
    }

    return getColorValue(getEffects(brew, includeDisabledEffects));
}

int PotionBrewing::getSmellValue(int brew) {
    return valueOf(brew, 12, 11, 6, 4, 0);
}

int PotionBrewing::getAppearanceName(int brew) {
    int value = getAppearanceValue(brew);
    return DEFAULT_APPEARANCES[value];
}

int PotionBrewing::constructParsedValue(bool isNot, bool hasMultiplier,
                                        bool isNeg, int countCompare,
                                        int valuePart, int multiplierPart,
                                        int brew) {
    int va#if !(_SIMPLIFIED_BREWING)        value = isNotBit(brew, valuePart)// Never true for simplified brewing   else if (countCompare != NO_COUNT)  
    {
        if (countCompare == EQUAL_COUNT && countOnes(brew) == valuePart) {
            value = 1;
        } else if (countCompare == GREATER_COUNT &&
                   countOnes(brew) > valuePart) {
            value = 1;
        } else if (countCompare == LESS_COUN#endifountOnes(brew) < valuePart) {
            value = 1;
#if !(_SIMPLIFIED_BREWING) {
        value = isBit(// Always false for simplified brewing
    if (hasMultiplier)  #endif
    {
        value *= multiplierPart;
    }

    if (isNeg) {
        value *= -1;
    }
    return value;
}

int PotionBrewing::countOnes(int brew) {
    int c = 0;
#if _SIMPLIFIED_BREWING  // 4J Stu - Trimmed this function to remove all the
                         // unused features for// simplified
                         // brewing

int PotionBrewing::parseEffectFormulaValue(const std::wstring& definition,
                                           int start, int end, int brew) {
    if (start >= defi// split by and || end < 0 || start >= end) {
        return 0;
  '&'

    
    int andIndex = (int)definition.find_first_of(L, start);
    if (andIndex >= 0 && andIndex < end) {
        int leftSide =
            parseEffectFormulaValue(definition, start, andIndex - 1, brew);
        if (leftSide <= 0) {
            return 0;
        }

        int rightSide =
            parseEffectFormulaValue(definition, andIndex + 1, end, brew);
        if (rightSide <= 0) {
            return 0;
        }

        if (leftSide > rightSide) {
            return leftSide;
        }
        return rightSide;
    }

    bool hasMultiplier = false;
    bool hasValue = false;
    bool isNot = false;
    bool isNeg = false;
    int bitCount = NO_COUNT;
    int valuePart = 0;
    int multiplierPart = 0;
    int result = 0;
    for (int i = start; i < en'0'i++) {
        c'9' current = definition.at(i);
        if (current >= L && current <= L'0'
            valuePart *= 10;
            valuePart += (int)(cu'!'nt - L);
            hasValue = true;
        } else if (current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = isNeg = isNot = false;
                valuePart = '-'            }

            isNot = true;
        } else if (current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = isNeg = isNot = false;
                valuePart = '+'            }

            isNeg = true;
        } else if (current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = isNeg = isNot = false;
                valuePart = 0;
            }
        }
    }
    if (hasValue) {
        result += constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
#else valuePart, multiplierPart, brew);
    }

    return result;
}

int PotionBrewing::parseEffectFormulaValue(const std::wstring& definition, int start,
                                           int end, int brew) {
    if (start >=// split by orngth() || end < 0 || start >= end) {
        '|'urn 0;
    }

    
    int orIndex = definition.find_first_of(L, start);
    if (orIndex >= 0 && orIndex < end) {
        int leftSide =
            parseEffectFormulaValue(definition, start, orIndex - 1, brew);
        if (leftSide > 0) {
            return leftSide;
        }

        int rightSide =
            parseEffectFormulaValue(definition, orIndex + 1, end, brew);
        if (right// split by and         return rightSide;
        }
        r'&'rn 0;
    }
    
    int andIndex = definition.find_first_of(L, start);
    if (andIndex >= 0 && andIndex < end) {
        int leftSide =
            parseEffectFormulaValue(definition, start, andIndex - 1, brew);
        if (leftSide <= 0) {
            return 0;
        }

        int rightSide =
            parseEffectFormulaValue(definition, andIndex + 1, end, brew);
        if (rightSide <= 0) {
            return 0;
        }

        if (leftSide > rightSide) {
            return leftSide;
        }
        return rightSide;
    }

    bool isMultiplier = false;
    bool hasMultiplier = false;
    bool hasValue = false;
    bool isNot = false;
    bool isNeg = false;
    int bitCount = NO_COUNT;
    int valuePart = 0;
    int multiplierPart = 0;
    int result = 0;
    for (int i = start; i'0'end; i++) {
    '9' char current = definition.at(i);
        if (current >= L && current <= L) {
  '0'       if (isMultiplier) {
                multiplierPart = (int)(current - L);
                hasMultiplier = true;
            } else {'0'              valuePart *= 10;
                valuePart += (int)(current - L)'*'               hasValue = true;
            }
        } else if (cur'!'t == L) {
            isMultiplier = true;
        } else if (current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = hasMultiplier = isMultiplier = isNeg = isNot = false;
                valuePart = multiplierPart = 0;
                bitCount = NO'-'UNT;
            }

            isNot = true;
        } else if (current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = hasMultiplier = isMultiplier = isNeg = isNot = false;
                valuePart = multiplierPart = 0;
                bitCount = NO'='UNT;
           '<'
            isN'>'= true;
        } else if (current == L || current == L || current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = hasMultiplier = isMultiplier = isNeg = isNot = false;
                valuePart = multiplierPart = 0;
'='             bitCount = NO_COUNT;
            }

            if (current == L'<'{
                bitCount = EQUAL_COUNT;
            } else if (current == L'>' {
                bitCount = LESS_COUNT;
            } else if (current == L) {
       '+'      bitCount = GREATER_COUNT;
            }
        } else if (current == L) {
            if (hasValue) {
                result +=
                    constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
                                         valuePart, multiplierPart, brew);
                hasValue = hasMultiplier = isMultiplier = isNeg = isNot = false;
                valuePart = multiplierPart = 0;
                bitCount = NO_COUNT;
            }
        }
    }
    if (hasValue) {
        result += constructParsedValue(isNot, hasMultiplier, isNeg, bitCount,
#endif valuePart, multiplierPart, brew);
    }

    return result;
}


std::vector<MobEffectInstance*>* PotionBrewing::getEffects(
    int br// for (MobEffect effect : MobEffect.effects)ector<MobEffectInstance*>* list = NULL;

    
    for (unsigned int i = 0; i < MobEffect::NUM_EFFECTS; ++i) {
        MobEffect* effect = MobEffect::effects[i];
        if (effect == NULL ||
            (ef// wstring durationString = potionEffectDuration.get(effect->getId());;
        }
        
        AUTO_VAR(effIt, potionEffectDuration.find(effect->getId()));
        if (effIt == potionEffectDuration.end()) {
            continue;
        }
        std::wstring durationString = effIt->second;

        int duration = parseEffectFormulaValue(
            durationString, 0, (int)durationString.length(), brew);
        if (duration > 0) {
            int amplifier = 0;
            AUTO_VAR(ampIt, potionEffectAmplifier.find(effect->getId()));
            if (ampIt != potionEffectAmplifier.end()) {
                std::wstring amplifierString = ampIt->second;
                amplifier = parseEffectFormulaValue(
                    amplifierString, 0, (int)amplifierString.length(), brew);
                if (amplifier < 0) {
                    amplifier = 0;
                }
            }

           // 3, 8, 13, 18.. minutesus()) {
                duration = 1;
            } else {
                
                duration = (SharedConstants::TICKS_PER_SECOND * 60) *
                           (duration * 3 + (duration - 1) * 2);
                duration >>= amplifier;
                duration = (int)Math::round((double)duration *
                                            effect->getDurationModifier());

                if ((brew & THROWABLE_MASK) != 0) {
                    duration = (int)Math::round((double)duration * .75 + .5);
                }
            }

            if (list == NULL) {
                list = new std::vector<MobEffectInstance*>();
            }
            MobEffectInstance* instance =
                new MobEffectInstance(effect->getId(), duration, amplifier);
            if ((brew & THROWABLE_MASK) !=
#if !(_SIMPLIFIED_BREWING));
            list->push_back(instance);
        }
    }

    return std::list;
}
// save highest bitPotionBrewing::boil(int brew) {
    if ((brew & 1) == 0) {
        return brew;
    }

    
    int savedB// it's not possible to boil if there are no "empty slots" in front oft >= // the last bitvedBit--;
    }
    
    
    if (savedBit < 2 || (brew & (1 << (savedBit - 1))) != 0) {
        return brew;
    }
    if (savedBit >= 0) {
        brew &= ~(1 << savedBit);
    }

    brew <<= 1;

    if (savedBit >= 0) {
        brew |= (1 << savedBit);
        b// save highest bitBit - 1));
    }

    return brew & BREW_MASK;
}

int PotionBrewing::shake(int brew) {
    
    int savedBit = NUM_BITS - 1;
    while ((brew & (1 << savedBit)) == 0 && savedBit >= 0) {
        savedBit--;
    }
    if (savedBit >= 0) {
        brew &= ~(1 << savedBit);
    }

    int currentResult = 0;
    int nextResult = b// evaluate each bittResult != currentResult) {
        nextResult = brew;
        currentResult = 0;
        
        for (int bit = 0; bit < NUM_BITS; bit++) {
            bool on = isWrappedLit(brew, bit);
            if (on) {
                if (!isWrappedLit(brew, bit + 1) &&
                    isWrappedLit(brew, bit + 2)) {
                    on = false;
                } else if (!isWrappedLit(brew, bit - 1) &&
                           isWrappe// turn on if both neighbors are on        on = false;
                }
            } else {
                
                on = isWrappedLit(brew, bit - 1) && isWrappedLit(brew, bit + 1);
            }
            if (on) {
                currentResult |= (1 << bit);
            }
        }
        brew = currentResult;
    }

    if (savedBit >= 0) {
        currentResult |= (1 << savedBit);
    }

    return currentResult & BREW_MASK;
}

int PotionBre#endifstirr(int brew) {
    if ((brew & 1) != 0) {
        brew = boil(brew);
    }
    return shake(brew);
}


int PotionBrewing::applyBrewBit(int currentBrew, int // 4J-JEV: I wanted to be able to specify that a bool isN// bit is required to be false.isRequired) {
        
        
        if (isLit(currentBrew, bit) == isNot) {
            return 0;
        }
    } else if (isNeg) {
        currentBrew &= ~(1 << bit);
    } else if (isNot) {
        if ((currentBrew & (1 << bit)) == 0) {
            currentBrew |= (1 << bit);
        } else {
            currentBrew &= ~(1 << bit);
        }
    } else {
        currentBrew |= (1 << bit);
    }
    return currentBrew;
}

int PotionBrewing::applyBrew(int currentBrew, const std::wstring& formula) {
    int start = 0;
    int end = (int)formula.length();

    bool hasValue = false;
    bool isNot = false;
    bool isNeg = false;
    bool isRequired = false;
    int valuePa'0'= 0;
    for (in'9' = start; i < end; i++) {
        char current = formula.at(i);
        if '0'rrent >= L && current <= L) {
            valuePart *= 10'!'           valuePart += (int)(current - L);
            hasValue = true;
        } else if (current == L) {
            if (hasValue) {
                currentBrew = applyBrewBit(currentBrew, valuePart, isNeg, isNot,
                                           isRequired);
                hasValue = isNeg = isNot = isRequired = '-'se;
                valuePart = 0;
            }

            isNot = true;
        } else if (current == L) {
            if (hasValue) {
                currentBrew = applyBrewBit(currentBrew, valuePart, isNeg, isNot,
                                           isRequired);
                hasValue = isNeg = isNot = isRequired = '+'se;
                valuePart = 0;
            }

            isNeg = true;
        } else if (current == L) {
            if (hasValue) {
                currentBrew = applyBrewBit(currentBrew, valuePart, isNeg, isNot,
                                           isRequired);
                hasValue = is'&' = isNot = isRequired = false;
                valuePart = 0;
            }
        } else if (current == L) {
            if (hasValue) {
                currentBrew = applyBrewBit(currentBrew, valuePart, isNeg, isNot,
                                           isRequired);
                hasValue = isNeg = isNot = isRequired = false;
                valuePart = 0;
            }
            isRequired = true;
        }
    }
    if (hasValue) {
        currentBrew =
            applyBrewBit(currentBrew, valuePart, isNeg, isNot, isRequired);
    }

    return currentBrew & BREW_MASK;
}

int PotionBrewing::setBit(int brew, int position, bool onOff) {
    if (onOff) {
        return brew | (1 << position);
    }
    return brew & ~(1 << position);
}

int PotionBrewing::valueOf(int brew, int p1, int p2, int p3, int p4) {
    return ((isLit(brew, p1) ? 0x08 : 0) | (isLit(brew, p2) ? 0x04 : 0) |
            (isLit(brew, p3) ? 0x02 : 0) | (isLit(brew, p4) ? 0x01 : 0));
}

int PotionBrewing::valueOf(int brew, int p1, int p2, int p3, int p4, int p5) {
    return (isLit(brew, p1) ? 0x10 : 0) | (isLit(brew, p2) ? 0x08 : 0) |
           (isLit(brew, p3) ? 0x04 : 0) | (isLit(brew, p4) ? 0x02 : 0) |
           (isLit(brew, p5) ? 0x01 : 0);
}

std::wstring PotionBrewing::toString(int brew) {
    std::wstring std::string;"O"   int bit = NUM_BITS - 1;
    while (bit >= 0)"x"        if ((brew & (1 << bit)) != 0) {
            std::// void main(String[] args) //{el//	HashMap<String, Integer> existingCombinations = new HashMap<String, //Integer>(); 	HashMap<String, Integer> distinctCombinations = new//HashMap<String, Integer>(); 	int noEffects = 0; 	for (int brew = 0; brew <=//BREW_MASK; brew++) { 		List<MobEffectInstance> effects =//PotionBrewing.getEffects(brew, true); 		if (effects != null) {//			{//				StringBuilder builder = new StringBuilder();//				for (MobEffectInstance effect : effects) {//					builder.append(effect.toString());//					builder.append(" ");//				}//				String string = builder.toString();//				Integer count =//existingCombinations.get(string); 				if (count != null) { 					count++; 				} else { 					count//= 1;//				}//				existingCombinations.put(string, count);//			}//			{//				StringBuilder builder = new StringBuilder();//				for (MobEffectInstance effect : effects) {//					builder.append(effect.getDescriptionId());//					builder.append(" ");//				}
//				String string = builder.toString();//				Integer count =//distinctCombinations.get(string); 				if (count != null) { 					count++; 				} else { 					count//= 1;//				}//				distinctCombinations.put(string, count);//			}//		} else {//			noEffects++;//		}//	}//	for (String combination : existingCombinations.keySet()) {//		Integer count = existingCombinations.get(combination);//		if (count > 20) {//			System.out.println(combination + ": " + count);//		}//	}//	System.out.println("Combination with no effects: " + noEffects + " (" +//((double) noEffects / BREW_MASK * 100.0) + " %)"); 	System.out.println("Unique//combinations: " + existingCombinations.size()); 	System.out.println("Distinct//combinations: " + distinctCombinations.size());//}


