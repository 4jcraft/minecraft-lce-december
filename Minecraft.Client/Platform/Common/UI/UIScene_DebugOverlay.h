#pragma once
#ifdef _DEBUG_MENUS_ENABLED
#include "UIScene.h"
#include "UIControl_ButtonList.h"

class UIScene_DebugOverlay : public UIScene {
private:
    enum eControls {
        eControl_SetCamera,
        eControl_ResetTutorial,
        eControl_Schematic,
        eControl_Thunder,
        eControl_Rain,
        eControl_FOV,
        eControl_SetDay,
        eControl_SetNight,
        eControl_Time,
        eControl_Mobs,
        eControl_Enchantments,
        eControl_Items,
    };

    std::vector<int> m_itemIds;
    std::vector<eINSTANCEOF> m_mobFactories;
    std::vector<std::pair<int, int> > m_enchantmentIdAndLevels;

public:
    UIScene_DebugOverlay(int iPad, void* initData, UILayer* parentLayer);

    virtual EUIScene getSceneType() { return eUIScene_DebugOverlay; }

protected:
    UIControl_ButtonList m_buttonListItems, m_buttonListMobs,
        m_buttonListEnchantments;
    UIControl_Slider m_sliderFov, m_sliderTime;
    UIControl_Button m_buttonRain, m_buttonThunder, m_buttonSchematic,
        m_buttonResetTutorial, m_buttonSetCamera, m_buttonSetDay,
        m_buttonSetNight;
    UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
    UI_MAP_ELEMENT(m_buttonListIt "itemsList")
    UI_MAP_ELEMENT(m_buttonListEnchantme "enchantmentsList")
    UI_MAP_ELEMENT(m_buttonListM "mobList")
    UI_MAP_ELEMENT(m_slider "fov")
    UI_MAP_ELEMENT(m_sliderT "time")
    UI_MAP_ELEMENT(m_buttonSet "setDay")
    UI_MAP_ELEMENT(m_buttonSetNi "setNight")
    UI_MAP_ELEMENT(m_buttonR "rain")
    UI_MAP_ELEMENT(m_buttonThun "thunder")
    UI_MAP_ELEMENT(m_buttonSchema "schematic")
    UI_MAP_ELEMENT(m_buttonResetTutor "resetTutorial")
    UI_MAP_ELEMENT(m_buttonSetCam "setCamera")
    UI_END_MAP_ELEMENTS_AND_NAMES()

    virtual std::wstring getMoviePath();

public:
    virtual void customDraw(IggyCustomDrawCallbackRegion* region);

    publ  // INPUT
        virtual void handleInput(int iPad, int key, bool repeat, bool pressed,
                                 bool released, bool& handled);

protected:
    void handlePress(F64 controlId, F64 childId);
    virtual void handleSliderMove(F64 sliderId, F64 currentValue) #endif