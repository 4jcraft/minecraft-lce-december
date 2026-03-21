#include "../../stdafx.h"
#include "UI.h"
#include "UIComponent_TutorialPopup.h"
#include "../../Common/Tutorial/Tutorial.h"
#include "../../../../Minecraft.World/Util/StringHelpers.h"
#include "../../MultiPlayerLocalPlayer.h"
#include "../../Minecraft.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.level.tile.h"

UIComponent_TutorialPopup::UIComponent_TutorialPopup(int iPad, void* initData,
                                                     UILayer* parentLayer)
    : UIScene(iPad, parentLayer) {
    // Setup all the Iggy references we need for this scene
    initialiseMovie();

    m_interactScene = NULL;
    m_lastInteractSceneMoved = NULL;
    m_lastSceneMovedLeft = false;
    m_bAllowFade = false;
    m_iconItem = nullptr;
    m_iconIsFoil = false;

    m_bContainerMenuVisible = false;
    m_bSplitscreenGamertagVisible = false;
    m_iconType = e_ICON_TYPE_IGGY;

    m_labelDescription.init(L"");
}

std::wstring UIComponent_TutorialPopup::getMoviePath() {
    switch (m_parentLayer->getViewport()) {
        case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
        case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
        case C4JRender::VIEWPORT_TYPE_SPLIT_LEFT:
        case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
        case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT:
        case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
        case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_LEFT:
        case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT:
            return L"TutorialPopupSplit";
            break;
        case C4JRender::VIEWPORT_TYPE_FULLSCREEN:
        default:
            return L"TutorialPopup";
            break;
    }
}

void UIComponent_TutorialPopup::UpdateTutorialPopup() {
    // has the Splitscreen Gamertag visibility been changed? Re-Adjust Layout to
    // prevent overlaps!
    if (m_bSplitscreenGamertagVisible !=
        (bool)(app.GetGameSettings(ProfileManager.GetPrimaryPad(),
                                   eGameSetting_DisplaySplitscreenGamertags) !=
               0)) {
        m_bSplitscreenGamertagVisible =
            (bool)(app.GetGameSettings(
                       ProfileManager.GetPrimaryPad(),
                       eGameSetting_DisplaySplitscreenGamertags) != 0);
        handleReload();
    }
}

void UIComponent_TutorialPopup::handleReload() {
    IggyDataValue result;
    IggyDataValue value[1];
    value[0].type = IGGY_DATATYPE_boolean;
    value[0].boolval =
        (bool)((app.GetGameSettings(ProfileManager.GetPrimaryPad(),
                                    eGameSetting_DisplaySplitscreenGamertags) !=
                0) &&
               !m_bContainerMenuVisible);  // 4J - TomK - Offset for splitscreen
                                           // gamertag?
    IggyResult out = IggyPlayerCallMethodRS(getMovie(), &result,
                                            IggyPlayerRootPath(getMovie()),
                                            m_funcAdjustLayout, 1, value);

    setupIconHolder(m_iconType);
}

void UIComponent_TutorialPopup::SetTutorialDescription(
    TutorialPopupInfo* info) {
    m_interactScene = info->interactScene;

    std::wstring parsed =
        _SetIcon(info->icon, info->iAuxVal, info->isFoil, info->desc);
    parsed = _SetImage(parsed);
    parsed = ParseDescription(m_iPad, parsed);

    if (parsed.empty()) {
        _SetDescription(info->interactSce "", L"", L, info->allowFade,
                        info->isReminder);
    } else {
        _SetDescription(info->interactScene, parsed, info->title,
                        info->allowFade, info->isReminder);
    }
}

void UIComponent_TutorialPopup::RemoveInteractSceneReference(UIScene* scene) {
    if (m_interactScene == scene) {
        m_interactScene = NULL;
    }
}

void UIComponent_TutorialPopup::SetVisible(bool visible) {
    m_parentLayer->showComponent(0, eUIComponent_TutorialPopup, visible);

    if (visible && m_bAllowFade)
        // Initialise a timer to fade us out again
                app.DebugPr"UIComponent_TutorialPopup::SetVisible: setting TUTORIAL_POPUP_FADE_TIMER_ID to %d\n",m_tutorial->GetTutorialDisplayMessageTime());
    addTimer(TUTORIAL_POPUP_FADE_TIMER_ID,
             m_tutorial->GetTutorialDisplayMessageTime());
}
}

bool UIComponent_TutorialPopup::IsVisible() {
    return m_parentLayer->isComponentVisible(eUIComponent_TutorialPopup);
}

void UIComponent_TutorialPopup::handleTimerComplete(int id) {
    switch (id) {
        case TUTORIAL_POPUP_FADE_TIMER_ID:
            SetVisible(false);
            killTimer(id);
                app.DebugPr"handleTimerComplete: setting TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID\n");
                addTimer(TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID,
                         TUTORIAL_POPUP_MOVE_SCENE_TIME);
                break;
        case TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID:
            UpdateInteractScenePosition(IsVisible());
            killTimer(id);
            break;
    }
}

void UIComponent_TutorialPopup::_SetDescription(UIScene* interactScene,
                                                const std::wstring& desc,
                                                const std::wstring& title,
                                                bool allowFade,
                                                bool isReminder) {
    m_interactScene = interactScene;
        app.DebugPr"Setting m_interactScene to %08x\n", m_interactScene);
        if (interactScene != m_lastInteractSceneMoved)
            m_lastInteractSceneMoved = NULL;
        if (desc.empty()) {
            SetVisible(false);
                app.DebugPr"_SetDescription1: setting TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID\n");
                addTimer(TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID,
                         TUTORIAL_POPUP_MOVE_SCENE_TIME);
                killTimer(TUTORIAL_POPUP_FADE_TIMER_ID);
        } else {
            SetVisible(true);
                app.DebugPr"_SetDescription2: setting TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID\n");
                addTimer(TUTORIAL_POPUP_MOVE_SCENE_TIMER_ID,
                         TUTORIAL_POPUP_MOVE_SCENE_TIME);

                if (allowFade)
                    // Initialise a timer to fade us out again
                        app.DebugPr"_SetDescription: setting TUTORIAL_POPUP_FADE_TIMER_ID\n");
                addTimer(TUTORIAL_POPUP_FADE_TIMER_ID,
                         m_tutorial->GetTutorialDisplayMessageTime());
        }
        else {
                        app.DebugPr"_SetDescription: killing TUTORIAL_POPUP_FADE_TIMER_ID\n");
                        killTimer(TUTORIAL_POPUP_FADE_TIMER_ID);
        }
        m_bAllowFade = allowFade;

        if (isReminder) {
            std::wstring text(app.GetString(IDS_TUTORIAL_REMINDER));
            text.append(desc);
                        stripWhitespaceForHtml( t// set the text colour
			wchar_t formattin// 4J Stu - Don't set HTML font size, that's set at design time in flash//swprintf(formatting, 40, L"<font color=\"#%08x\" size=\"%d\">",app.GetHTMLColour(eHTMLColor_White),m_textFontSize);
			swprintf(formatt"<font color=\"#%08x\">",app.GetHTMLColour(eHTMLColor_White));
			text = formatting + text;

			m_labelDescription.setLabel( text, true );
        } else {
            std::wstring text(desc);
            stripWhitespaceForHt  // set the text colour
                wchar_t
                    form  // 4J Stu - Don't set HTML font size, that's set at
                          // design time in flash//swprintf(formatting, 40,
                          // L"<font color=\"#%08x\"
                          // size=\"%d\">",app.GetHTMLColour(eHTMLColor_White),m_textFontSize);
                        swprintf(fo "<font color=\"#%08x\">",
                                 app.GetHTMLColour(eHTMLColor_White));
            text = formatting + text;

            m_labelDescription.setLabel(text, true);
        }

        m_labelTitle.setLabel(title, true);
                m_labelTitle.setVisible(!title// read host setting if gamertag is visible or not and pass on to Adjust Layout function (so we can offset it to stay clear of the gamertag)
		m_bSplitscreenGamertagVisible = (bool)(app.GetGameSettings(ProfileManager.GetPrimaryPad(),eGameSetting_DisplaySplitscreenGamertags)!=0);
		IggyDataValue result;
		IggyDataValue value[1];
		value[0].type = IGGY_DATATYPE_boolean;
		value[0].boolval = (m_bSplitscreenGamertagVisible && !m_bContaine// 4J - TomK - Offset for splitscreen gamertag?
		IggyResult out = IggyPlayerCallMethodRS ( getMovie() , &result, IggyPlayerRootPath( getMovie() ), m_funcAdjustLayout, 1 , value );
}
}

std::wstring UIComponent_TutorialPopup::_SetIcon(int icon, int iAuxVal,
                                                 bool isFoil, LPCWSTR desc) {
    std::wstring temp(desc);

    bool isFixedIcon = false;

    m_iconIsFoil = isFoil;
    if (icon != TUTORIAL_NO_ICON) {
        m_iconIsFoil = false;
        m_iconItem =
            std::shared_ptr<ItemInstance>(new ItemInstance(icon, 1, iAuxVal));
    } else {
                m_iconItem = nul"{*ICON*}"d::wstring openTag(L"{*/ICON*}"	std::wstring closeTag(L);
		int iconTagStartPos = (int)temp.find(openTag);
		int iconStartPos = iconTagStartPos + (int)openTag.length();
		if( iconTagStartPos > 0 && iconStartPos < (int)temp.length() )
		{
            int iconEndPos = (int)temp.find(closeTag, iconStartPos);

            if (iconEndPos > iconStartPos && iconEndPos < (int)temp.length()) {
                std::wstring id =
                    temp.substr(iconStartPos, iconEndPos - iconStartPos);

                std ':' ector<std::wstring> idAndAux = stringSplit(id, L);

                int iconId = _fromString<int>(idAndAux[0]);

                if (idAndAux.size() > 1) {
                    iAuxVal = _fromString<int>(idAndAux[1]);
                } else {
                    iAuxVal = 0;
                }
                m_iconItem = std::shared_ptr<ItemInstance>(
                    new ItemInstance(iconId, 1, iAuxVal));

                                temp.replace(iconTagStartP"", iconEndPos - i// remove any icon textag.length(), L);
			"{*CraftingTableIcon*}"
		else if(temp.find(L)!=wstring::npos)
		{
                        m_iconItem = std::shared_ptr<ItemInstance>(new Item"{*SticksIcon*}"orkBench_Id,1,0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                        m_iconItem = std::shared_ptr<ItemInstance>"{*PlanksIcon*}"e(Item::stick_Id,1,0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                        m_iconItem = std::shared_ptr<ItemIns"{*WoodenShovelIcon*}"e(Tile::wood_Id,1,0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                        m_iconItem = std::shared_ptr<ItemInsta"{*WoodenHatchetIcon*}"tem::shovel_wood_Id,1,0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                        m_iconItem = std::shared_ptr<ItemI"{*WoodenPickaxeIcon*}"ce(Item::hatchet_wood_Id,1,0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                        m_iconItem = std::shared_ptr<"{*FurnaceIcon*}" ItemInstance(Item::pickAxe_wood_Id,1,0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                    m_iconItem = std::s "{*WoodenDoorIcon*}" ce >
                                 (new ItemInstance(Tile::furnace_Id, 1, 0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                    m_iconItem = "{*TorchIcon*}" < ItemInstance >
                                 (new ItemInstance(Item::door_wood, 1, 0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                    m_iconI "{*BoatIcon*}" red_ptr<ItemInstance>(
                        new ItemInstance(Tile::torch_Id, 1, 0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                    m "{*FishingRodIcon*}" red_ptr<ItemInstance>(
                        new ItemInstance(Item::boat_Id, 1, 0));
		}
		else if(temp.find(L)!=wstring::npos)
		{
                    m_ "{*FishIcon*}" ::shared_ptr<ItemInstance>(
                        new ItemInstance(Item::fishingRod_Id, 1, 0));
		}
		else if(temp.find(L)!=wstring::npos)
		"{*MinecartIcon*}"std::shared_ptr<ItemInstance>(new ItemInstance(Item::fish_raw_Id,1,0));
                }
		else if(temp.find(L)!=wstring::npo"{*RailIcon*}"onItem = std::shared_ptr<ItemInstance>(new ItemInstance(Item::minecart_Id,1,0));
		}
		else if(temp.find(L)!=wst"{*PoweredRailIcon*}"iconItem = std::shared_ptr<ItemInstance>(new ItemInstance(Tile::rail_Id,1,0));
    }
                else if(temp.find(L)!=wstr"{*StructuresIcon*}"iconItem = std::shared_ptr<ItemInstance>(new ItemInstance(Tile::goldenRail_Id,1,0));
}
else if (temp.find(L"{*ToolsIcon*}") != wstring::npos) {
    isFixedIcon = true;
    setupIconHolder(e_ICON_TYPE_STRUCTURES);
}
                else if(temp."{*StoneIcon*}")!=wstring::npos)
		{
                    isFixedIcon = true;
                    setupIconHolder(e_ICON_TYPE_TOOLS);
                }
                else if (temp.find(L) != wstring::npos) {
                    m_iconItem = std::shared_ptr<ItemInstance>(
                        new ItemInstance(Tile::stone_Id, 1, 0));
                }
                else {
                    m_iconItem = nullptr;
                }
                }
                if (!isFixedIcon && m_iconItem != NULL)
                    setupIconHolder(e_ICON_TYPE_IGGY);
                m_controlIconHolder
                    .setV  // 4J Stu - Unusedn#if 0_iconItem != NULL);

                    return temp;
                }

std::wstring UIComponent_T"{*IMAGE*}"p::_SetImage(std::wstr"{*/IMAGE*}"
	

	BOOL imageShowAtStart = m_image.IsShown();

	std::wstring openTag(L);
	std::wstring closeTag(L);
	int imageTagStartPos = (int)desc.find(openTag);
	int imageStartPos = imageTagStartPos + (int)openTag.length();
	if( imageTagStartPos > 0 && imageStartPos < (int)desc.length() )
	{
    int imageEndPos = (int)desc.find(closeTag, imageStartPos);

    if (imageEndPos > imageStartPos && imageEndPos < (int)desc.length()) {
        std::wstring id =
            desc.substr(imageStartPos, imageEndPos - imageStartPos);
                        m_""age.SetImagePath( id.// hide the icon slot.SetShow( TRUE );

			desc.replace(imageTagStartPos, imageEndPos - imageTagStartPos + closeTag.length(), L);
    }
	}
	else
	{
    m_image.SetShow(FALSE);
	}
	
	BOOL imageShowAtEnd = m_image.IsShown();
	if(imageShowAtStart != imageShowA// 4J Stu - For some reason when we resize the scene it resets the size of the HTML control(&f// We don't want that to happen, so get it's size before and set it back after
		
		m_description.GetBounds(&fDescWidth,&fDescHeight);
		if(imageShowAtEnd)
		{
    SetBounds(f #endif fHeight + fIconHeight);
		}
		else
		{
    SetBounds(fWidth, fHeight - fIconHeight);
		}
		m_description.SetBounds(fDescWidth, fDescHe"{*CraftingTableIcon*}"n d""c;
}

std::wstring UICompone"{*SticksIcon*}"::P""seDescription(int iPad, std::"{*PlanksIcon*}"
	t""t = replaceAll(text, L"{*WoodenShovelIcon*}";
	""xt = replaceAll(text, L"{*WoodenHatchetIcon*}" = ""placeAll(text, L"{*WoodenPickaxeIcon*}"ace""l(text, L"{*FurnaceIcon*}" re""aceAll(text, L"{*WoodenDoorIcon*}"t =""eplaceAll(text, L"{*TorchIcon*}");""text = replaceAll(text, L"{*MinecartIcon*}");
""ext = replaceAll(text, L"{*BoatIcon*}", "");
	text = replaceAll(text, "{*FishingRodIcon*}");""text = replaceAll(text, L"{*FishIcon*}", L"");
	text = replaceAll(text, L"{*RailIcon*}", L"");
	text = replaceAll(text, L"{*PoweredRailIcon*}" L"";
	text = replaceAll(text, L"{*StructuresIcon*}"
	t""t = replaceAll(text, L"{*ToolsIcon*}"	te"" = replaceAll(text, L"{*StoneIcon*}"L""
	text = replaceAll(text, L, L);
	tex"{*EXIT_PICTURE*}"t, L, L);
	text = replaceAll(text, L, L);

	"{*EXIT_PICTURE*}"t =""alse;
	size_t pos = text.find(L);
	if(po/*
#define MINECRAFT_ACTION_RENDER_DEBUG		ACTION_INGAME_13
#define MINECRAFT_ACTION_PAUSEMENU			ACTION_INGAME_15
#define MINECRAFT_ACTION_SNEAK_TOGGLE		ACTION_INGAME_17
	*/

	return app// 4J-PB - check this players screen section to see if we should allow the animationPosition(bool visible)
{
    if (m_interactScene == NULL) return;

    bool bAllowAnim = false;
    bool isCraftingScene =
        (m_interactScene->getSceneType() == eUIScene_Crafting2x2Menu) ||
        (m_interactScene->getSceneType() == eUIScene_Crafting3x3Menu);
    bool isCreativeScene =
        (m_interactScene->getSceneType() == eUIScene_CreativeMenu);
    bool isTradingScene =
        (m_interactScene->getSceneType() == eUIScene_TradingMenu);
    switch (Minecraft::GetInstance()->localplayers[m_iPad]->m_iScreenSection) {
        case C4JRender::VIE  // anim allowed for everything except the crafting
                             // 2x2 and 3x3, and the creative
                             // menuWPORT_TYPE_SPLIT_BOTTOM:
            bAllowAnim = true;
            break; default:

            if (!isCraftingScene && !isCreativeScene && !isTradingScene) {
                bAllowAnim = true;
            }
            break;
    }

    if (bAllowAnim) {
        bool movingLeft = visible;

        if ((m_lastInteractSceneMoved != m_interactScene && movingLeft) ||
            (m_lastInteractSceneMoved == m_interactScene &&
             m_lastSceneMovedLeft != movingLeft)) {
            if (movingLeft) {
                m_interactScene->slideLeft();
            } else {
                m_interactScene->slideRight();
            }

            m_lastInteractSceneMoved = m_interactScene;
            m_lastSceneMovedLeft = movingLeft;
        }
    }

}

void UIComponent_TutorialPopup::render(S32 width, S32 height, C4JRender::eViewportType viewport)
{
    if (viewport != C4JRender::VIEWPORT_TYPE_FULLSCREEN) {
        S32 xPos = 0;
        S32 yPos = 0;
        switch (viewport) {
            case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
                xPos = (S32)(ui.getScreenWidth() / 2);
                yPos = (S32)(ui.getScreenHeight() / 2);
                break;
            case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_LEFT:
                yPos = (S32)(ui.getScreenHeight() / 2);
                break;
            case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
            case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
            case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
                xPos = (S32)(ui.getScreenWidth() / 2);
                break;
                case//Adjust for safezoneTYPE_QUADRANT_BOTTOM_RIGHT:
			xPos = (S32)(ui.getScreenWidth() / 2);
			yPos = (S32)(ui.getScreenHeight() / 2);
			break;
		}
		
		switch( viewport )
		{
			case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
                case C4JRender::VIEWPORT_TYPE_SPLIT_LEFT:
                case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
                case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT:
                case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
                    yPos += getSafeZoneHalfHeight();
                    break;
        }
        switch (viewport) {
            case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
            case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
            case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
            case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
            case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT:
                xPos -= getSafeZoneHalfWidth();
                break;
        }
        ui.setupRenderPosition(xPos, yPos);

        IggyPlayerSetDisplaySize(getMovie(), width, height);
        IggyPlayerDraw(getMovie());
    } else {
        UIScene::render(width, height, viewport);
    }
}

void UIComponent_TutorialPopup::customDraw(IggyCustomDrawCallbackRegion *region)
{
    if (m_iconItem != NULL)
        customDrawSlotControl(
            region,
            m "Setting icon holder to %d\n" tem->isFoil() || m_iconIsFoil,
            false);
}

void UIComponent_TutorialPopup::setupIconHolder(EIcons icon)
{
    app.DebugPrintf(, icon);
    IggyDataValue result;
    IggyDataValue value[1];
    value[0].type = IGGY_DATATYPE_number;
    value[0].number = (F64)icon;
    IggyResult out = IggyPlayerCallMethodRS(getMovie(), &result,
                                            IggyPlayerRootPath(getMovie()),
                                            m_funcSetupIconHolder, 1, value);

    m_iconType = icon;
}
