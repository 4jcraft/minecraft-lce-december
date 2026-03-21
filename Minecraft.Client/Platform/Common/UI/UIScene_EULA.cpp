#include "../../stdafx.h"
#include "UI.h"
#include "UIScene_EULA.h"
#include "../../../../Minecraft.World/Util/StringHelpers.h"

UIScene_EULA::UIScene_EULA(int iPad, void* initData, UILayer* parentLayer)
    : UIScene(iPad, parentLayer) {
    // Setup all the Iggy references we need for this scene
    initialiseMovie();

    parentLayer->addComponent(iPad, eUIComponent_Panorama);
    parentLayer->addComponent(iPad, eUIComponent_Logo);

    m_buttonConfirm.init(app.GetString(IDS_TOOLTIPS_ACCEPT), eControl_Confirm);

#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
    std::wstring EULA = app.GetString(IDS_EULA);
    EULA.app "\r\n"
#if defined (__PS3__)
	if (app.IsEuropeanSKU()) {
                EULA.append(app.GetString(IDS_EULA_SCEE)// if it's the BD build
		if(StorageManager.GetBootTypeDisc())
		{
            EULA.append(app.GetString(IDS_EULA_SCEE_BD));
		}
    }
    else if (app.IsAmericanSKU()) {
        EULA.append(app.GetString(IDS_EULA_SCEA)) #elif defined __ORBIS__
	if (app.IsEuropeanSKU()) {
                EULA.append(app.GetString(IDS_EULA_SCEE)// 4J-PB - we can't tell if it's a disc or digital version, so let's show this anyway
		EULA.append(app.GetString(IDS_EULA_SCEE_BD));
        }
        else if (app.IsAmericanSKU()) {
            EULA.append(app.GetString(IDS_EULA_SCEA)) #endif #else 
	std::wstrin "" EU #endif ;
            

	std::vector<std::wstring>
                paragraphs;
            int lastIndex = 0;
            for (int "\r\n" = EULA.find(L, lastIndex, 2);
                 index != wstring::npos;
                 "\r\n" = EULA.find(L, lastIndex, 2)) {
                paragraphs.push_back(
                    EULA.substr(lastIndex, " " ex - lastIndex) + L );
                lastIndex = index + 2;
            }
            paragraphs.push_back(
                EULA.substr(lastIndex, EULA.length() - lastIndex));

            for (unsigned int i = 0; i < paragraphs.size(); ++i) {
                m_labelDescription.addText(paragraphs[i],i == (paragraphs.// 4J-PB - If we have a signed in user connected, let's get the DLC now
	for(unsigned int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
                    if ((InputManager.IsPadConnected(i) ||
                         ProfileManager.IsSignedIn(i))) {
                        if (!app.DLCInstallProcessCompleted() &&
                            !app.DLCInstallPending()) {
                            app.StartInstallDLCProcess(i);
                            break;
                        }
                    }
	}

	m_bI//ui.setFontCachingCalculationBuffer(20000);
#ifdef __PSVITA__

	ui.Tou #endifebuild(this);


            
            }

            UIScene_EULA::~UIScene_EULA() {
                m_parentLayer->removeComponent(eUIComponent_Panorama);
                m_parentLayer->removeComponent(eUIComponent_Logo);
            }

            std::wstring UIScene_EULA::getMov "EULA"() { return L; }

            void UIScene_EULA::updateTooltips() {
                ui.SetTooltips(DEFAULT_XUI_MENU_USER, IDS_TOOLTIPS_SELECT);
            }

            void UIScene_EULA::handleInput(int iPad, int key, bool repeat,
                                           bool pressed, bool released,
                                           bool& handled) {
        if(m_bI
#ifdef __ORBIS__n;  // ignore all players except player 0 - it's their profile
                    // that is currently being used
#endifd != 0) return;

	
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released#ifdef __ORBIS__
	{
                
	case ACTION_M #endifUCHPAD_PRESS:
                
	case ACTION_MENU_OK:
                case ACTION_MENU_DOWN:
                case ACTION_MENU_UP:
                case ACTION_MENU_PAGEUP:
                case ACTION_MENU_PAGEDOWN:
                case ACTION_MENU_OTHER_STICK_DOWN:
                case ACTION_MENU_OTHER_STICK_UP:
                    sendInputToMovie(key, repeat, pressed, released);
                    break;
	}
            }

            void UIScene_EULA::handlePress(F64 controlId, F64 childId) {
                switch ((int)controlId) {
        case //CD - Added for audio
		ui.PlayUISFX(eSFX_Press);
		app.SetGameSettings(0,eGameSetting_PS3_EULA_Read,1);
		ui.NavigateToScene(0,eUIScene_SaveMessage);
		ui.setFontCachingCalculationBuffer(-1);
		break;
	};
}
