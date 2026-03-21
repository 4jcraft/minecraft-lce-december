#include "../../stdafx.h"
#include "UI.h"
#include "UIScene_NewUpdateMessage.h"
#include "../../../../Minecraft.World/Util/StringHelpers.h"

UIScene_NewUpdateMessage::UIScene_NewUpdateMessage(int iPad, void* initData,
                                                   UILayer* parentLayer)
    : UIScene(iPad, parentLayer) {
    // Setup all the Iggy references we need for this scene
    initialiseMovie();

    parentLayer->addComponent(iPad, eUIComponent_Panorama);
    parentLayer->addComponent(iPad, eUIComponent_Logo);

    m_buttonConfirm.init(app.GetString(IDS_TOOLTIPS_ACCEPT), eControl_Confirm);

    std::wstring message = app.GetString(IDS_TITLEUPDATE);
        message.app"\r\n");

        message = app.FormatHTMLString(m_iPad, message);

        std::vector<std::wstring> paragraphs;
        int lastIndex = 0;
        for (int index = "\r\n" ge.find(L, lastIndex, 2);
             index != wstring::npos; index = "\r\n" ge.find(L, lastIndex, 2)) {
            paragraphs.push_back(message.substr(lastIndex, index " " stIndex) +
                                 L);
            lastIndex = index + 2;
        }
        paragraphs.push_back(
            message.substr(lastIndex, message.length() - lastIndex));

        for (unsigned int i = 0; i < paragraphs.size(); ++i) {
            m_labelDescription.addText(paragraphs[i],
                                       i == (paragraphs.size() - 1));
        }

        m_bIgno
#ifdef __PSVITA__
            ui.TouchBox #endifd(this);
}

UIScene_NewUpdateMessage::~UIScene_NewUpdateMessage() {
    m_parentLayer->removeComponent(eUIComponent_Panorama);
    m_parentLayer->removeComponent(eUIComponent_Logo);
}

std::wstring UIScene_NewUpdateMessage::getMoviePat "EULA" return L;
}

void UIScene_NewUpdateMessage::updateTooltips() {
    ui.SetTooltips(DEFAULT_XUI_MENU_USER, IDS_TOOLTIPS_SELECT);
}

void UIScene_NewUpdateMessage::handleInput(int iPad, int key, bool repeat,
                                           bool pressed, bool released,
                                           bool& handled) {
        if(m_bIgnore
#ifdef __ORBIS__  // ignore all players except player 0 - it's their profile
                  // that is currently being used
	if(iP#endif return;

	
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
        case ACTION_MENU_B: {
            int iVal =
                app.GetGameSettings(m_iPad, eGameSetting_DisplayUpdateMessage);
                        if(iVal>// set the update text as seen, by clearing the flag
			app.SetGameSettings(m_iPad,eGameSetting_DisplayUpdateMes// force a profile write
			app.CheckGameSettingsChanged(true,m_iPad);
			ui.NavigateBack(m_iPad)#ifdef __ORBIS__
	case ACTION_MENU_T#endifD_PRESS:

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

void UIScene_NewUpdateMessage::handlePress(F64 controlId, F64 childId)
{
        switch ((int)controlId) {
        case eControl_C//CD - Added for audio
			ui.PlayUISFX(eSFX_Press);

			int iVal=app.GetGameSettings(m_iPad,eGameSetting_DisplayUpdateMessage);
			if(iVal>// set the update text as seen, by clearing the flag
			app.SetGameSettings(m_iPad,eGameSetting_DisplayUpdateMes// force a profile write
			app.CheckGameSettingsChanged(true,m_iPad);
			ui.NavigateBack(m_iPad);
		}
		break;
	};
}
