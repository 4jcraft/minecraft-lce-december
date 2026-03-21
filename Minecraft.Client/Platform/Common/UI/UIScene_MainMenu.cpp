#include "../../stdafx.h"
#include "../../../../Minecraft.World/Util/Mth.h"
#include "../../../../Minecraft.World/Util/StringHelpers.h"
#include "../../../../Minecraft.World/Util/Random.h"
#include "../../User.h"
#include "../../MinecraftServer.h"
#include "UI.h"
#include "UIScene_MainMenu.h"
#ifdef __ORBIS__
#include <error_dialog.h>
#endif

Random* UIScene_MainMenu::random = new Random();

EUIScene UIScene_MainMenu::eNavigateWhenReady = (EUIScene)-1;

UIScene_MainMenu::UIScene_MainMenu(int iPad, void* initData,
                                   UILayer* parentLayer)
    : UIScene(iPad, parentLayer) {
#ifdef __ORBIS
    // m_ePatchCheckState=ePatchCheck_Idle;
    m_bRunGameChosen = false;
    m_bErrorDialogRunning = false;
#endif

    // Setup all the Iggy references we need for this scene
    initialiseMovie();

    parentLayer->addComponent(iPad, eUIComponent_Panorama);
    parentLayer->addComponent(iPad, eUIComponent_Logo);

    m_eAction = eAction_None;
    m_bIgnorePress = false;

    m_buttons[(int)eControl_PlayGame].init(IDS_PLAY_GAME, eControl_PlayGame);

#ifdef _XBOX_ONE
    if (!ProfileManager.IsFullVersion())
        m_buttons[(int)eControl_PlayGame].setLabel(IDS_PLAY_TRIAL_GAME);
    app.SetReachedMainMenu();
#endif

    m_buttons[(int)eControl_Leaderboards].init(IDS_LEADERBOARDS,
                                               eControl_Leaderboards);
    m_buttons[(int)eControl_Achievements].init((UIString)IDS_ACHIEVEMENTS,
                                               eControl_Achievements);
    m_buttons[(int)eControl_HelpAndOptions].init(IDS_HELP_AND_OPTIONS,
                                                 eControl_HelpAndOptions);
    if (ProfileManager.IsFullVersion()) {
        m_bTrialVersion = false;
        m_buttons[(int)eControl_UnlockOrDLC].init(IDS_DOWNLOADABLECONTENT,
                                                  eControl_UnlockOrDLC);
    } else {
        m_bTrialVersion = true;
        m_buttons[(int)eControl_UnlockOrDLC].init(IDS_UNLOCK_FULL_GAME,
                                                  eControl_UnlockOrDLC);
    }

#ifndef _DURANGO
    m_buttons[(int)eControl_Exit].init(app.GetString(IDS_EXIT_GAME),
                                       eControl_Exit);
#else
    m_buttons[(int)eControl_XboxHelp].init(IDS_XBOX_HELP_APP,
                                           eControl_XboxHelp);
#endif

#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
    // Not allowed to exit from a PS3 game from the game - have to use the PS
    // button
    removeControl(&m_buttons[(int)eControl_Exit], false);
    // We don't have a way to display trophies/achievements, so remove the
    // button
    removeControl(&m_buttons[(int)eControl_Achievements], false);
    m_bLaunchFullVersionPurchase = false;
#endif
#ifdef _DURANGO
    // Allowed to not have achievements in the menu
    removeControl(&m_buttons[(int)eControl_Achievements], false);
    // Not allowed to exit from a Xbox One game from the game - have to use the
    // Home button
    // removeControl( &m_buttons[(int)eControl_Exit], false );
    m_bWaitingForDLCInfo = false;
#endif

    doHorizontalResizeCheck();

    m_splash = L"";

    std::wstring filenam "splashes.txt";
    if (app.hasArchiveFile(filename)) {
        byteArray splashesArray = app.getArchiveFile(filename);
        ByteArrayInputStream bais(splashesArray);
        InputStreamReader isr(&bais);
        BufferedReader br(&isr);

        std::wstrin "" line = L;
        while (!(line = br.readLine()).empty()) {
            line = trimString(line);
            if (line.length() > 0) {
                m_splashes.push_back(line);
            }
        }

        br.close();
    }

    m_bIgnorePress = false;
    m_bLoadTrialOnNetworkManagerReady =  // 4J Stu - Clear out any loaded game
                                         // rules
        app.setLevelGenerationOptions    // 4J Stu - Reset the leaving game flag
                                         // so that we correctly handle signouts
                                         // while in the menus
            g_NetworkManager.ResetLeavi
#if TO_BE_IMPLEMENTED    // Fix for #45154 - Frontend: DLC: Content can only be downloaded from the frontend if you have not joined/exited multiplayer
                XBackgroundDownloadSetMode(
                    XBACKGROUND_DOWNLOAD_MODE_ALWAY #endifW);
    


}

UIScene_MainMenu::~UIScene_MainMenu() {
    m_parentLayer->removeComponent(eUIComponent_Panorama);
    m_parentLayer->removeComponent(eUIComponent_Logo);
}

void UIScene_MainMenu::updateTooltips() {
    int iX = -1;
    int iA = -1;
    if (!m_bIgnorePress) {
        iA = IDS_TOOLTIP
#ifdef _XBOX_ONE
		iX = IDS_TOOLTIPS_CHO #elif defined __PSVITA__
		if (ProfileManager.IsFullVersion())
            {iX = IDS_TOOLTIP_CHANGE_NETWORK_ #endif}

	
    
    }
    ui.SetTooltips(DEFAULT_XUI_MENU_USER, iA, -1, iX);
}

void UIScene_MainMenu::updateComponents() {
    m_parentLayer->showComponent(m_iPad, eUIComponent_Panorama, true);
    m_parentLayer->showComponent(m_iPad, eUIComponent_Logo, true);
}

void UIScene_MainMenu::handleGainFocus(bool navBack) {
    UIScene::handleGainFocus(navBack);
    ui.ShowPlayerDisplayname(false);
    m_bIgnorePress = false;

    if (eNavigateWhenReady >= 0) {
        ret  // 4J-JEV: This needs to come before SetLockedProfile(-1) as it
             // wipes the XbLive contexts.
            if (!navBack) {
                for (int iPad = 0; iPad < MAX_LOCAL_PLAYERS; iPad+// For returning to menus after exiting a game.
			if (ProfileManager.IsSignedIn(iPad) )
			{
                ProfileManager.SetCurrentGameActivity(
                    iPad, CONTEXT_PRESENCE_MENUS, false);
			}
        }
    }
    ProfileManager.SetLockedProfile(-1);

    m_bIgnorePress = false;
        updateTo
#ifdef _DURANGO
	ProfileManager.ClearGam #endif();

		
	if(navBack && ProfileManager.IsFullVersio// Replace the Unlock Full Game with Downloadable Content
		m_buttons[(int)eControl_UnlockOrDLC].setLabel(IDS_DOWNLOADABLECON
#if TO_BE_IMPLEMENTED    // Fix for #45154 - Frontend: DLC: Content can only be downloaded from the frontend if you have not joined/exited multiplayer
	XBackgroundDownloadSetMode(XBACKGROUND_DOWNLOAD_MODE_ALWAYS_ALLOW);
	m_Timer.SetSho#endifE);

	m_controlTimer.setVisible( f// 4J-PB - remove the "hobo humping" message legal say we can't have, and the 1080p one for Vita#ifdef __PSVITA__
	int splashIndex = eSplashRandomStart + 2 + random->nextInt( (int)m_splashes.size() - (eSplashRandomStart#else ); 

	int splashIndex = eSplashRandomStart + 1 + random->nextInt( (int)m_splashes.size() - (eSplashRandomStar#endif );// Override splash text on certain dates
	SYSTEMTIME LocalSysTime;
	GetLocalTime( &LocalSysTime );
	if (LocalSysTime.wMonth == 11 && LocalSysTime.wDay == 9)
	{
        splashIndex = eSplashHappyBirthdayEx;
	}
	else if (LocalSysTime.wMonth == 6 && LocalSysTime.wDay == 1)
	{
        splashIndex = eSplashHappyBirthdayNotch;
	}
	else if (LocalSysTime.wMonth == 12 && LocalSysTime.wD// the Java game shows this on Christmas Eve, so we will too
	{
        splashIndex = eSplashMerryXmas;
	}
	else if (LocalSysTime.wMonth == 1 && LocalSysTime.wDay == 1)
	{
        splashIndex = eSplashHappyNew  // splashIndex = 47; // Very short
                                       // string//splashIndex = 194; // Very
                                       // long string//splashIndex = 295; //
                                       // Coloured//splashIndex = 296; //
                                       // Noise
            m_splash = m_splashes.at(splashIndex);
}

std::wstring UIScene_MainMenu::getMoviePath()
{
        "MainMenu";
}

void UIScene_MainMenu::handleR#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)// Not allowed to exit from a PS3 game from the game - have to use the PS button
	removeControl( &m_buttons[(int)eControl_Exit], // We don't have a way to display trophies/achievements, so remove the button
	removeControl( &m_buttons[(int)eControl_Achievements],#endif #ifdef _DURANGO// Allowed to not have achievements in the menu
	removeControl( &m_buttons[(int)eControl_Achievements],#endif );



}

void UIScene_MainMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &ha//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");
	
	if ( m_bIgnorePress || (eNavigateWhenReady >= 0)
#if defined(__ORBIS__) || \
    defined(              \
        __PSVITA__)    // ignore all players except player 0 - it's their profile that is currently being used
	if(iPad!=0#endifrn;


	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
    case ACTION #ifdef __ORBIS__
	case ACTION_MENU_TOUCHP #endifSS:
        
		if (pressed) {
            ProfileManager.SetPrimaryPad(iPad);
            ProfileManager.SetLockedProfile(-1);
            sendInputToMovie(key, repeat, pressed, released);
        }
#ifdef _XBOX_ONE
	case ACTION_MENU_X:
        if (pressed) {
            m_bIgnorePress = true;
            ProfileManager.RequestSignInUI(false, false, false, false, false,
                                           ChooseUser_SignInReturned, this,
                                           iPad);
        }
#endifa #ifdef __PSVITA__
	case ACTION_MENU_X:
        if (pressed && ProfileManager.IsFullVersion()) {
            UINT uiIDA[2];
            uiIDA[0] = IDS__NETWORK_PSN;
            uiIDA[1] = IDS_NETWORK_ADHOC;
            ui.RequestErrorMessage(
                IDS_SELECT_NETWORK_MODE_TITLE, IDS_SELECT_NETWORK_MODE_TEXT,
                uiIDA, 2, XUSER_INDEX_ANY,
                &UIScene_MainMenu::SelectNetworkModeReturned, this);
        }
#endifak;
    

	case ACTION_MENU_UP:
    case ACTION_MENU_DOWN:
        sendInputToMovie(key, repeat, pressed, released);
        break;
	}
}

void UIScene_MainMenu::handlePress(F64 controlId, F64 childId) {
        int primaryPad = ProfileManager.GetPrim
#ifdef _XBOX_ONE
	int (*signInReturnedFunc)(LPVOID, const bool, const int iPad, \
                                  const int iController #elseULL;

	int (*signInReturnedFunc) (LPVOID,const bool, const int iPad#endifLL;


	switch((int)controlId)
	{
        case eControl_#ifdef __ORBIS__
		{
			m_bIgnorePress=//CD - Added for audio
			ui.PlayUISFX(eSFX_Press);

			ProfileManager.RefreshChatAndContentRestrictions(RefreshChatAndContentRestrictionsReturned_PlayGame, t#else
		}

		m_eAction=eAction_R//CD - Added for audio
		ui.PlayUISFX(eSFX_Press);

		signInReturnedFunc = &UIScene_MainMenu::CreateLoad_SignIn#endif		;

		break;
	case eControl_Leader//CD - Added for audio
		ui.PlayUISFX(eSF#ifdef __ORBIS__
		ProfileManager.RefreshChatAndContentRestrictions(RefreshChatAndContentRestrictionsReturned_Leaderboard#elseis);

		m_eAction=eAction_RunLeaderboards;
		signInReturnedFunc = &UIScene_MainMenu::Leaderboards_SignIn#endifed;

		break;
	case eControl_Achiev//CD - Added for audio
		ui.PlayUISFX(eSFX_Press);

		m_eAction=eAction_RunAchievements;
		signInReturnedFunc = &UIScene_MainMenu::Achievements_SignInReturned;
		break;
	case eControl_HelpAndO//CD - Added for audio
		ui.PlayUISFX(eSFX_Press);

		m_eAction=eAction_RunHelpAndOptions;
		signInReturnedFunc = &UIScene_MainMenu::HelpAndOptions_SignInReturned;
		break;
	case eControl_Unloc//CD - Added for audio
		ui.PlayUISFX(eSFX_Press);

		m_eAction=eAction_RunUnlockOrDLC;
		signInReturnedFunc = &UIScene_MainMenu::UnlockFullGame_SignInReturned;#if defined _XBOX
	case eControl_Exit:
            if (ProfileManager.IsFullVersion()) {
                UINT uiIDA[2];
                uiIDA[0] = IDS_CANCEL;
                uiIDA[1] = IDS_OK;
                ui.RequestErrorMessage(
                    IDS_WARNING_ARCADE_TITLE, IDS_WARNING_ARCADE_TEXT, uiIDA, 2,
                    XUSER_INDEX_ANY, &UIScene_MainMenu::ExitGameReturned, this);
            }
#ifdef _XBOX_ONE
				ui.ShowPlayerDisplayna #endife);
            
			ui.NavigateToScene(primaryPad,
                                           eUIScene_TrialExitUpsell);
		}#endifa
#ifdef _DURANGO
	case eControl_XboxHelp:
		ui.PlayUISFX(eSFX_Press);

		m_eAction=eAction_RunXboxHelp;
		signInReturnedFunc = &UIScene_MainMenu::XboxHelp_SignInReturned;#endifak;


	default:	__debugbreak();
}

bool confirmUser =  // Note: if no sign in returned func, assume this isn't
                    // required
    if (signInReturnedFunc != NULL) {
    if (ProfileManager.IsSignedIn(primaryPad)) {
        if (confirmUser) {
            ProfileManager.RequestSignInUI(false, false, true, false, true,
                                           signInReturnedFunc, this,
                                           primaryPad);
        } else {
            RunAction(primaryPad);
        }
    }
    el  // Ask user to sign in
        UINT uiIDA[2];
    uiIDA[0] = IDS_CONFIRM_OK;
    uiIDA[1] = IDS_CONFIRM_CANCEL;
    ui.RequestErrorMessage(IDS_MUST_SIGN_IN_TITLE, IDS_MUST_SIGN_IN_TEXT, uiIDA,
                           2, primaryPad, &UIScene_MainMenu::MustSignInReturned,
                           this);
    // Run current action
    void UIScene_MainMenu::RunAction(int iPad) {
        switch (m_eAction) {
            case eAction_RunGame:
                RunPlayGame(iPad);
                break;
            case eAction_RunLeaderboards:
                RunLeaderboards(iPad);
                break;
            case eAction_RunAchievements:
                RunAchievements(iPad);
                break;
            case eAction_RunHelpAndOptions:
                RunHelpAndOptions(iPad);
                break;
            case eAction_RunUnlockOrDLC:
                RunUnlockOrDLC(iPad);
#ifdef _DURANGO
	case eAction_RunXb  // 4J: Launch the dummy xbox help
                            // application.
                WXS::User ^ user =
                    ProfileManager.GetUser(ProfileManager.GetPrimaryPad());
                Windows::Xbox::ApplicationModel::Help::Show(user);
#endifak;
                
	
        }
    }

    void UIScene_MainMenu::customDraw(IggyCustomDrawCallbackRegion * region) {
        if (wcscmp((wchar_t*)regi "Splash", L) == 0) {
                    PIXBeginNam "Custom draw splash");
                    customDrawSplash(region);
                    PIXEndNamedEvent();
        }
    }

    void UIScene_MainMenu::customDrawSplash(IggyCustomDrawCallbackRegion *
                                            region) {
        Minecraft* pMinecraft =
            Minecraft::GetInst  // 4J Stu - Move this to the ctor when
                                // the main menu is not the first scene
                                // we navigate to
                ScreenSizeCalculator ssc(pMinecraft->options,
                                         pMinecraft->width_phys,
                                         pMinecraft->height_phys);
        m_fScreenWidth = (float)pMinecraft->width_phys;
        m_fRawWidth = (float)ssc.rawWidth;
        m_fScreenHeight = (float)pMinecraft->height_phys;
        m_fRawHeight = (float)ssc.rawH  // Setup GDraw, normal game render
                                        // states and matrices
                       CustomDrawData* customDrawRegion =
            ui.setupCustomDraw(this, region);
        delete customDrawRegion;

        Font* font =
            pMinecraft  // build and render with the game call
                glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);

        glPushMatrix();

        float width = region->x1 - region->x0;
        float height = region->y1 - region->y0;
        float xo = width / 2;
        float yo = height;

        glTranslatef(xo, yo, 0);

        glRotatef(-17, 0, 0, 1);
        float sss = 1.8f - Mth::abs(Mth::sin(System::currentTimeMillis() %
                                             1000 / 1000.0f * PI * 2) *
                                    0.1f);
        sss *= (m_fScreenWidth / m_fRawWidth);

        sss = sss * 100 / (font->width(m_splash) + 8 * 4); glScalef(
                    sss, ss  // drawCenteredString(font, splash, 0, -8,
                             // 0xffff00);
                                 font->drawShadow(
                                     m_splash, 0 - (font->width(m_splash)) / 2,
                                     -8, 0xffff00);
                    glPopMatrix();

                    glDisable(GL_RESCALE_NORMAL);

                    glEnable(GL_DEPTH_  // Finish GDraw and anything else that
                                        // needs to be finalised
                                            ui.endCustomDraw(region);
    }

    int UIScene_MainMenu::MustSignInReturned(
        void* pParam, int iPad, C4JStorage::EMessageResult result) {
        UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

                        if (result ==
                            C4JStorage::
                                EMessage_ResultAcce  // we need to specify local
                                                     // game here to display
                                                     // local and LIVE profiles
                                                     // in the list
                            switch (pClass->m_eAction) {
            case eAction_RunGame:
                ProfileManager.RequestSignInUI(
                    false, true, false, false, true,
                    &UIScene_MainMenu::CreateLoad_SignInReturned, pClass, iPad);
                break;
            case eAction_RunHelpAndOptions:
                ProfileManager.RequestSignInUI(
                    false, false, true, false, true,
                    &UIScene_MainMenu::HelpAndOptions_SignInReturned, pClass,
                    iPad);
                break;
            case eAction_RunLeaderboards:
                ProfileManager.RequestSignInUI(
                    false, false, true, false, true,
                    &UIScene_MainMenu::Leaderboards_SignInReturned, pClass,
                    iPad);
                break;
            case eAction_RunAchievements:
                ProfileManager.RequestSignInUI(
                    false, false, true, false, true,
                    &UIScene_MainMenu::Achievements_SignInReturned, pClass,
                    iPad);
                break;
            case eAction_RunUnlockOrDLC:
                ProfileManager.RequestSignInUI(
                    false, false, true, false, true,
                    &UIScene_MainMenu::UnlockFullGame_SignInReturned, pClass,
                    iPad);
                break;
#ifdef _DURANGO						 					  
		case eAction_RunXboxHelp : ProfileManager.RequestSignInUI( \
    false, false, true, false, true,                                       \
    &UIScene_MainMenu::XboxHelp_SignInReturned, pClass, iPad #endifak;
                
		
        }
    }
    else {
        pClass
            ->m_bIgnorePress  // unlock the profile
                ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
            ProfileManager.SetCurrentGameActivity(i, CONTEXT_PRESENCE_MENUS,
                                                  false);
			}
    }
}

re
#if defined(__PS3__) || defined(__PSVITA__) ||                      \
    defined(__ORBIS__)
int UIScene_MainMenu::MustSignInReturnedPSN(void* pParam, int iPad, \
                                            C4JStorage::EMessageResult result)
{
    UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

        if(result==C4JStorage::EMessage_ResultAc#ifdef __PS3__// we need to specify local game here to display local and LIVE profiles in the list
		switch(pClass->m_eAction)
		{
        case eAction_RunLeaderboardsPSN:
            SQRNetworkManager_PS3::AttemptPSNSignIn(
                &UIScene_MainMenu::Leaderboards_SignInReturned, pClass);
            break;
        case eAction_RunGamePSN:
            SQRNetworkManager_PS3::AttemptPSNSignIn(
                &UIScene_MainMenu::CreateLoad_SignInReturned, pClass);
            break;
        case eAction_RunUnlockOrDLCPSN:
            SQRNetworkManager_PS3::AttemptPSNSignIn(
                &UIScene_MainMenu::UnlockFullGame_SignInReturned, pClass);
            b #elif defined __PSVITA__
		switch (pClass->m_eAction) {
                case eAction_RunLeaderboar//CD - Must force Ad-Hoc off if they want leaderboard PSN sign-in//Save settings change
			app.SetGameSettings(0, eGameSetting_PSVita_NetworkModeAdho//Force off
			CGameNetworkManager::setAdhocMode(f//Now Sign-in
			SQRNetworkManager_Vita::AttemptPSNSignIn(&UIScene_MainMenu::Leaderboards_SignInReturned, pClass);
			break;
		case eAction_RunGamePSN:
                    if (CGameNetworkManager::usingAdhocMode()) {
                        SQRNetworkManager_AdHoc_Vita::AttemptAdhocSignIn(
                            &UIScene_MainMenu::CreateLoad_SignInReturned,
                            pClass);
                    } else {
                        SQRNetworkManager_Vita::AttemptPSNSignIn(
                            &UIScene_MainMenu::CreateLoad_SignInReturned,
                            pClass);
                    }
                    break;
                case eAction_RunUnlockOrD//CD - Must force Ad-Hoc off if they want commerce PSN sign-in//Save settings change
			app.SetGameSettings(0, eGameSetting_PSVita_NetworkModeAdho//Force off
			CGameNetworkManager::setAdhocMode(f//Now Sign-in
			SQRNetworkManager_Vita::AttemptPSNSignIn(&UIScene_MainMenu::UnlockFullGame_SignInReturned, pClass);
			b#else
		}

		switch(pClass->m_eAction)
		{
		case eAction_RunLeaderboardsPSN:
                    SQRNetworkManager_Orbis::AttemptPSNSignIn(
                        &UIScene_MainMenu::Leaderboards_SignInReturned, pClass,
                        true, iPad);
                    break;
                case eAction_RunGamePSN:
                    SQRNetworkManager_Orbis::AttemptPSNSignIn(
                        &UIScene_MainMenu::CreateLoad_SignInReturned, pClass,
                        true, iPad);
                    break;
                case eAction_RunUnlockOrDLCPSN:
                    SQRNetworkManager_Orbis::AttemptPSNSignIn(
                        &UIScene_MainMenu::UnlockFullGame_SignInReturned,
                        pClass, true, iPad);
                    b
#endif }

	
            
            }
            else {
                if (pClass->m_eAction == eAction_RunGamePSN) {
                    if (result == C4JStorage::EMessage_Cancelled)
                        CreateLoad_SignInReturned(pClass, false, 0);
                    else
                        CreateLoad_SignInReturned(pClass, true, 0);
                } else {
                    pClass->m_bIgnorePress = false;
                }
            }

            re #endif;
#ifdef _XBOX_ONE
int UIScene_MainMenu::HelpAndOptions_SignInReturned( \
    void* pParam, bool bContinue, int iPad, int iCo #elseler)
            
int UIScene_MainMenu::HelpAndOptions_SignInReturned(void* pParam,
                                                    bool bContinue, #endifad)

{
                UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

        if(bConti// 4J-JEV: Don't we only need to update rich-presence if the sign-in status changes.
		ProfileManager.SetCurrentGameActivity(iPad, CONTEXT_PRESENCE_MENUS
#if TO_BE_IMPLEMENTED
 		if (app.GetTMSDLCIn #endif())

 		{
                    ProfileManager.SetLockedProfile(
                        ProfileManager
                            .GetPrima #ifdef _XBOX_ONE
			ui.ShowPlayerDisplayna #endife);

			proceedToScene(iPad, eUIScene_HelpAndOptionsMe#if TO_BE_IMPLEMENTED
 		else// Changing to async TMS calls
 			app.SetTMSAction(iPad,eTMSAction_TMSPP_RetrieveFiles_HelpAndOption// block all input
 			pClass->m_bIgnorePress=// We want to hide everything in this scene and display a timer until we get a completion for the TMS files
 			for(int i=0;i<BUTTONS_MAX;i++)
 			{
                        pClass->m_Buttons[i].SetShow(FALSE);
 			}
 
 			pClass->updateTooltips();
 
 			pClass->m_Timer.SetShow(TR#endif		}

	
            
            }
            else {
                pClass
                    ->m_bIgnorePress  // unlock the profile
                        ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
                    ProfileManager.SetCurrentGameActivity(
                        i, CONTEXT_PRESENCE_MENUS, false);
			}
            }
	}	

	re
#ifdef _XBOX_ONE
int UIScene_MainMenu::ChooseUser_SignInReturned(void* pParam, bool bContinue, \
                                                int iPad, int iController)
{
        UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;
        pClass->m_bIgnorePress = false;
        pClass->updateTooltips();
        re #endif;
#ifdef _XBOX_ONE
int UIScene_MainMenu::CreateLoad_SignInReturned(void* pParam, bool bContinue, \
                                                int iPad, int iCo #elseler)
        
int UIScene_MainMenu::CreateLoad_SignInReturned(void* pParam, bool bContinue,
#endifad)

{
            UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

            if (
                bConti  // 4J-JEV: We only need to update rich-presence if the
                        // sign-in status changes.
                    ProfileManager.SetCurrentGameActivity(
                        iPad, CONTEXT_PRESENCE_MENUS, false);

                UINT uiIDA[1] = {IDS_OK};

                if (ProfileManager.IsGuest(ProfileManager.GetPrimaryPad())) {
                    pClass->m_bIgnorePress = false;
                    ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE,
                                           IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
                } else {
                        ProfileManager.SetLockedProfile(ProfileManager.GetPrimaryPad// change the minecraft player name
			Minecraft::GetInstance()->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad()));

			if(ProfileManager.IsFullVersion())
			{
                    bool bSignedInLive =
                        ProfileManager.IsSignedInLi #ifdef __PSVITA__
				if (CGameNetworkManager::usingAdhocMode()) {
                        if (SQRNetworkManager_AdHoc_Vita::GetAdhocStatus()) {
                            bSignedInLive = true;
                        } else
                            // adhoc mode, but we didn't make the connection,
                            // turn off adhoc mode, and just go with whatever
                            // the regular online status is
                            CGameNetworkManager::setAdhocMode(false);
                        bSignedInLive = ProfileManager.IsSignedInLive(iPad);
#endif }
                            // Check if we're signed in to LIVE
                            if (bSignedInLive)
                            // 4J-PB - Need to check for installed DLC
                            if (!app.DLCInstallProcessCompleted())
                                app.StartInstallDLCProcess(iPad);

                        if (ProfileManager.IsGuest(iPad)) {
                            pClass->m_bIgnorePress = false;
                            ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE,
                                                   IDS_PRO_GUESTPROFILE_TEXT,
                                                   uiIDA, 1);
                        } else
                            // 4J Stu - Not relevant to PS3#ifdef _XBOX_ONE//
                            // if(app.GetTMSDLCInfoRead() &&
                            // app.GetBanListRead(iPad))
                            if (app.GetBanListRead(iPad)) {
                                Minecraft* pMinecraft =
                                    Minecraft::GetInstance();
                                pMinecraft->user->name =
                                    convStringToWstring(
                                        ProfileManager.GetGamertag(
                                            ProfileManager
                                                .GetPrimaryPad()))  // ensure
                                                                    // we've
                                                                    // applied
                                                                    // this
                                                                    // player's
                                                                    // settings
                                    app.ApplyGameSettingsChang
#ifdef _XBOX_ONE
							ui \
    .ShowPlayerDisplayna #endife);

							proceedToScene(
                                        ProfileManager.GetPrimaryPad(),
                                        eUIScene_LoadOrJoinMenu);
                            } else {
                                app.SetTMSAction(
                                       iPad,
                                       eTMSAction_TMSPP_RetrieveFiles_RunPlayGame)  // block all input
                                    pClass->m_bIgnorePress =
                                    tru  // We want to hide everything in this
                                         // scene and display a timer until we
                                         // get a completion for the TMS
                                         // files//
                                         // for(int
                                         // i=0;i<eControl_Count;i++)//
                                         // {	//
                                         // m_buttons[i].set(false);//
                                         // }	

                                        pClass->updateTooltips();

                                pClass->m_controlTimer.setVisible(
                                    true) #endif #if TO_BE_IMPLEMENTED    // check if all the TMS files are loaded
                                    if (app.GetTMSDLCInfoRead() &&
                                        app.GetTMSXUIDsFileRead() &&
                                        app.GetBanListRead(iPad)) {
                                    if (StorageManager.SetSaveDevice(
                                            &UIScene_MainMenu::
                                                DeviceSelectReturned,
                                            pClass) == true)
                                        // save device already
                                        // selected// ensure we've
                                        // applied this player's
                                        // settings
                                        app.ApplyGameSettingsChanged(
                                               ProfileManager
                                                   .GetPrimaryPad())  // check
                                                                      // for
                                                                      // DLC//
                                                                      // start
                                                                      // timer
                                                                      // to
                                                                      // track
                                                                      // DLC
                                                                      // check
                                                                      // finished
                                            pClass->m_Timer.SetShow(TRUE);
                                    XuiSetTimer(
                                        pClass->m_hObj, DLC_INSTALLED_TIMER_ID,
                                        DLC_INSTALLED_TIMER_TIME)  // app.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_MultiGameJoinLoad);
                                }
                            }
                        else
                            // Changing to async TMS calls
                            app.SetTMSAction(
                                   iPad,
                                   eTMSAction_TMSPP_RetrieveFiles_RunPlayGame)  // block all input
                            pClass->m_bIgnorePress =
                            tru  // We want to hide everything in this scene and
                                 // display a timer until we get a completion
                                 // for the TMS files
                            for (int i = 0; i < BUTTONS_MAX; i++) {
                            pClass->m_Buttons[i].SetShow(FALSE);
                        }

                        updateTooltips();

                        pClass->m_Timer.SetShow(TRUE) #else
                    }
                    
						Minecraft* pMinecraft =
                        Minecraft::GetInstance();
                                                pMinecraft->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad())// ensure we've applied this player's settings
						app.ApplyGameSettingsChang
#ifdef _XBOX_ONE
						ui \
    .ShowPlayerDisplayna #endife);

						proceedToScene(ProfileManager.GetPrimaryPad(), eUIScene_LoadOrJ#endifu);

					}
                } e #if TO_BE_IMPLEMENTED    // offline
                ProfileManager.DisplayOfflineProfile(
                    &CScene_Main::CreateLoad_OfflineProfileReturned, pClass,
                    ProfileManager.GetPrimar #else))
                ;

					app.De"Offline Profile returned not implemented\n"#ifdef _XBOX_ONE
					ui.ShowPlayerDisplayna#endife);

					proceedToScene(
    ProfileManager.GetPrimaryPad(), eUIScene_LoadOrJ #endifu);

				
        }
			}
			else// 4J-PB - if this is the trial game, we can't have any networking// Can't apply the player's settings here - they haven't come back from the QuerySignInStatud call above yet.// Need to let them action in the main loop when they come in// ensure we've applied this player's settings//app.ApplyGameSettingsChanged(iPad);
#if defined(__PS3__) || defined(__ORBIS__) || \
    defined(                                  \
        __PSVITA__)    // ensure we've applied this player's settings - we do have them on PS3
				app.ApplyGameSettingsChang#endifd
#ifdef __ORBIS__
				if (!g_NetworkManager.IsReadyToPlayOrIdle())
				{
        pClass->m_bLoadTrialOnNetworkManagerReady = true;
        ui.NavigateToScene(iPad, eUIScene_Timer);
				}#endiflse

	// go straight in to the trial level
					LoadTrial();
}
}
}
}
else {
    pClass->m_bIgnorePress =  // unlock the profile
        ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
        ProfileManager.SetCurrentGameActivity(i, CONTEXT_PRESENCE_MENUS, false);
			}
}
}
re
#ifdef _XBOX_ONE
int UIScene_MainMenu::Leaderboards_SignInReturned( \
    void* pParam, bool bContinue, int iPad, int iCo #elseler)

int UIScene_MainMenu::Leaderboards_SignInReturned(void* pParam, bool bContinue,
#endifad)

{
    UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

        if(bConti// 4J-JEV: We only need to update rich-presence if the sign-in status changes.
		ProfileManager.SetCurrentGameActivity(iPad, CONTEXT_PRESENCE_MENUS, false);

		UINT uiIDA[1] = {
        IDS  // guests can't look at leaderboards
            if (ProfileManager.IsGuest(ProfileManager.GetPrimaryPad())) {
            pClass->m_bIgnorePress = false;
            ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE,
                                   IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
        }
        else if (!ProfileManager.IsSignedInLive(
                     ProfileManager.GetPrimaryPad())) {
            pClass->m_bIgnorePress = false;
            ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE,
                                   IDS_PRO_XBOXLIVE_NOTIFICATION, uiIDA, 1);
        }
        else {
            bool bContentRestrict #if defined (__PS3__) ||
                defined(__PSVITA__)
			ProfileManager.GetChatAndContentRestrictions(
                    iPad, true, NULL, &bContentRestrict #endifL);
            
			if (bContentRestricted) {
                pClass->m_bIgnorePre #if !(
                    defined(_XBOX) || defined(_WINDOWS64) ||
                    defined(_XBOX_ONE))  // 4J Stu - Temp to get the win build
                                         // running, but so we check this for
                                         // other platforms// you can't see
                                         // leaderboards
                    UINT uiIDA[1];
                uiIDA[0] = IDS_CONFIRM_OK;
                                ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrima#endif));
                                
			
            
            }
            else {
                ProfileManager.SetLockedProfile(ProfileManager
                                                    .GetPrima #ifdef _XBOX_ONE
				ui.ShowPlayerDisplayna #endife);
                
				proceedToScene(ProfileManager.GetPrimaryPad(),
                                               eUIScene_LeaderboardsMenu);
            }
        }
	}
	else
	{
        pClass
            ->m_bIgnorePress  // unlock the profile
                ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
            ProfileManager.SetCurrentGameActivity(i, CONTEXT_PRESENCE_MENUS,
                                                  false);
			}
		}
}
re
#ifdef _XBOX_ONE
int UIScene_MainMenu::Achievements_SignInReturned( \
    void* pParam, bool bContinue, int iPad, int iCo #elseler)

int UIScene_MainMenu::Achievements_SignInReturned(void* pParam, bool bContinue,
#endifad)

{
    UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

    if (bContinue) {
        pClass
            ->m_bIgnorePress  // 4J-JEV: We only need to update rich-presence if
                              // the sign-in status changes.
                ProfileManager.SetCurrentGameActivity(
                    iPad, CONTEXT_PRESENCE_MENUS, false);

        XShowAchievementsUI(ProfileManager.GetPrimaryPad());
    } else {
        pClass
            ->m_bIgnorePress  // unlock the profile
                ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
            ProfileManager.SetCurrentGameActivity(i, CONTEXT_PRESENCE_MENUS,
                                                  false);
			}
    }
}
re
#ifdef _XBOX_ONE
int UIScene_MainMenu::UnlockFullGame_SignInReturned( \
    void* pParam, bool bContinue, int iPad, int iCo #elseler)

int UIScene_MainMenu::UnlockFullGame_SignInReturned(void* pParam,
                                                    bool bContinue, #endifad)

{
    UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

        if (bConti// 4J-JEV: We only need to update rich-presence if the sign-in status changes.
		ProfileManager.SetCurrentGameActivity(iPad, CONTEXT_PRESENCE_MENUS, false);

		pClass->RunUnlockOrDLC(iPad);
}
else {
    pClass
        ->m_bIgnorePress  // unlock the profile
            ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
        ProfileManager.SetCurrentGameActivity(i, CONTEXT_PRESENCE_MENUS, false);
			}
}
}

re
#ifdef _XBOX_ONE
int UIScene_MainMenu::XboxHelp_SignInReturned(void* pParam, bool bContinue, \
                                              int iPad, int iController)
{
    UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

        if (bConti// 4J: Launch the dummy xbox help application.
		WXS::User^ user = ProfileManager.GetUser(ProfileManager.GetPrimaryPad());
		Windows::Xbox::ApplicationModel::Help::Show(user);
}
// unlock the profile
ProfileManager.SetLockedProfile(-1);
                for(int i=0;i<XUSER_MAX_COUNT;i+// if the user is valid, we should set the presence
			if(ProfileManager.IsSignedIn(i))
			{
    ProfileManager.SetCurrentGameActivity(i, CONTEXT_PRESENCE_MENUS, false);
			}
                }
                }

                re #endif;
                }


int UIScene_MainMenu::ExitGameReturned(void *pParam,int iPad,C4JStorage::EMessageResult r//UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;// buttons reversed on this
	if(result==C4JStorage::EMessage_ResultDecli//XLaunchNewImage(XLAUNCH_KEYWORD_DASH_ARCADE, 0);
		app.ExitGame();
}

re
#ifdef __ORBIS__
void UIScene_MainMenu::RefreshChatAndContentRestrictionsReturned_PlayGame( \
    void* pParam)
{
    int primaryPad = ProfileManager.GetPrimaryPad();

    UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

    int (*signInReturnedFunc)(
        LPVOID, const bool,
        const int
            iPad)  // 4J-PB - Check if there is a patch for the game
        pClass->m_errorCode =
        ProfileManager.getNPAvailability(ProfileManager.GetPrimaryPad());

    bool bPatchAvailable;
    switch (pClass->m_errorCode) {
        case SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST:
        case SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED:
            bPatchAvailable = true;
            break;
        default:
            bPatchAvailable = false;
            break;
    }

    if (!bPatchAvailable) {
        pClass->m_eAction = eAction_RunGame;
        signInReturnedFunc = &UIScene_MainMenu::CreateLoad_SignInReturned;
    } else {
        pClass->m_bRunGameChosen = true;
        pClass->m_bErrorDialogRunning = true;
        int32_t ret = sceErrorDialogInitialize();
        if (ret == SCE_OK) {
            SceErrorDialogParam param;
                        sceErrorDialogParamInitialize( &pa// 4J-PB - We want to display the option to get the patch now
			param.errorCode = SCE_NP_ERROR_LATEST_PATCH_PKG_D//pClass->m_errorCode;
			ret = sceUserServiceGetInitialUser( &param.userId );
			if ( ret == SCE_OK ) 
			{
                ret = sceErrorDialogOpen(&param);
			}
			ret// 		UINT uiIDA[1];// 		uiIDA[0]=IDS_OK;// 		ui.RequestMessageBox(IDS_PATCH_AVAILABLE_TITLE, IDS_PATCH_AVAILABLE_TEXT, uiIDA, 1, XUSER_INDEX_ANY,NULL,pClass);// Check if PSN is unavailable because of age restriction
	if (pClass->m_errorCode == SCE_NP_ERROR_AGE_RESTRICTION)
	{
                UINT uiIDA[1];
                uiIDA[0] = IDS_PRO_NOTONLINE_DECLINE;
                ui.RequestErrorMessage(
                    IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1,
                    ProfileManager.GetPrimaryPad(),
                    &UIScene_MainMenu::PlayOfflineReturned, pClass);

                return;
	}

	bool confirmUser =// Note: if no sign in returned func, assume this isn't required
	if (signInReturnedFunc != NULL)
	{
                if (ProfileManager.IsSignedIn(primaryPad)) {
                    if (confirmUser) {
                        ProfileManager.RequestSignInUI(
                            false, false, true, false, true, signInReturnedFunc,
                            pClass, primaryPad);
                    } else {
                        pClass->RunAction(primaryPad);
                    }
                }
                el  // Ask user to sign in
                    UINT uiIDA[2];
                uiIDA[0] = IDS_CONFIRM_OK;
                uiIDA[1] = IDS_CONFIRM_CANCEL;
                ui.RequestErrorMessage(
                    IDS_MUST_SIGN_IN_TITLE, IDS_MUST_SIGN_IN_TEXT, uiIDA, 2,
                    primaryPad, &UIScene_MainMenu::MustSignInReturned, pClass);
		}
        }
    }

    void
    UIScene_MainMenu::RefreshChatAndContentRestrictionsReturned_Leaderboards(
        void* pParam) {
        int primaryPad = ProfileManager.GetPrimaryPad();

        UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

        int (*signInReturnedFunc)(
            LPVOID, const bool, const int iPad)  // 4J-PB - Check if there is a
                                                 // patch for the game
            pClass->m_errorCode =
            ProfileManager.getNPAvailability(ProfileManager.GetPrimaryPad());

        bool bPatchAvailable;
        switch (pClass->m_errorCode) {
            case SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST:
            case SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED:
                bPatchAvailable = true;
                break;
            default:
                bPatchAvailable = false;
                break;
        }

        if (!bPatchAvailable) {
            pClass->m_eAction = eAction_RunLeaderboards;
            signInReturnedFunc = &UIScene_MainMenu::Leaderboards_SignInReturned;
        } else {
            int32_t ret = sceErrorDialogInitialize();
            pClass->m_bErrorDialogRunning = true;
            if (ret == SCE_OK) {
                SceErrorDialogParam param;
                        sceErrorDialogParamInitialize( &pa// 4J-PB - We want to display the option to get the patch now
			param.errorCode = SCE_NP_ERROR_LATEST_PATCH_PKG_D//pClass->m_errorCode;
			ret = sceUserServiceGetInitialUser( &param.userId );
			if ( ret == SCE_OK ) 
			{
                    ret = sceErrorDialogOpen(&param);
                    // 		UINT uiIDA[1];//
                    // uiIDA[0]=IDS_OK;//
                    // ui.RequestMessageBox(IDS_PATCH_AVAILABLE_TITLE,
                    // IDS_PATCH_AVAILABLE_TEXT, uiIDA, 1,
                    // XUSER_INDEX_ANY,NULL,pClass);
	}

	bool confirmUser =// Update error code
	pClass->m_errorCode = ProfileManager.getNPAvailability(ProfileManager.GetPrimary// Check if PSN is unavailable because of age restriction
	if (pClass->m_errorCode == SCE_NP_ERROR_AGE_RESTRICTION)
	{
                    UINT uiIDA[1];
                    uiIDA[0] = IDS_CONFIRM_OK;
                    ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE,
                                           IDS_CONTENT_RESTRICTION, uiIDA, 1,
                                           ProfileManager.GetPrimaryPad(),
                                           nullptr, pClass);

                    ret  // Note: if no sign in returned func, assume this isn't
                         // required
                        if (signInReturnedFunc != NULL) {
                        if (ProfileManager.IsSignedIn(primaryPad)) {
                            if (confirmUser) {
                                ProfileManager.RequestSignInUI(
                                    false, false, true, false, true,
                                    signInReturnedFunc, pClass, primaryPad);
                            } else {
                                pClass->RunAction(primaryPad);
                            }
                        }
                        el  // Ask user to sign in
                            UINT uiIDA[2];
                        uiIDA[0] = IDS_CONFIRM_OK;
                        uiIDA[1] = IDS_CONFIRM_CANCEL;
                        ui.RequestErrorMessage(
                            IDS_MUST_SIGN_IN_TITLE, IDS_MUST_SIGN_IN_TEXT,
                            uiIDA, 2, primaryPad,
                            &UIScene_MainMenu::MustSignInReturned, pClass);
                    }
	}
            }

            int UIScene_MainMenu::PlayOfflineReturned(
                void* pParam, int iPad, C4JStorage::EMessageResult result) {
                UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

                if (result == C4JStorage::EMessage_ResultAccept) {
                    if (pClass->m_eAction == eAction_RunGame) {
                        CreateLoad_SignInReturned(pClass, true, 0);
                    } else {
                        pClass->m_bIgnorePress = false;
                    }
                } else {
                    pClass->m_bIgnorePress = false;
                }

                re #endif;
            }
            

void UIScene_MainMenu::RunPlayGame(int iPad) {
                Minecraft* pMinecraft =
                    Minecraft::GetInst  // clear the remembered signed in users
                                        // so their profiles get read
                                        // again
                        app.ClearSignInChangeUsersMask();

                app.ReleaseSaveThumbnail();

                if (ProfileManager.IsGuest(iPad)) {
                    UINT uiIDA[1];
                    uiIDA[0] = IDS_OK;

                    m_bIgnorePress = false;
                    ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE,
                                           IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
                } else {
                ProfileManager.SetLockedProfile(// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
		ProfileManager.QuerySigninSta// 4J-PB - Need to check for installed DLC
		if(!app.DLCInstallProcessCompleted()) app.StartInstallDLCProcess(iPad);

		if(ProfileManager.IsFullVersion(// are we offline?
			bool bSignedInLive = ProfileManager.IsSignedInLi#ifdef __PSVITA__
			if(app.GetGameSettings(ProfileManager.GetPrimaryPad(),eGameSetting_PSVita_NetworkModeAdhoc) == true)
			{
                        CGameNetworkManager::setAdhocMode(true);
                        bSignedInLive =
                            SQRNetworkManager_AdHoc_Vita::GetAdhocStatus();
                                app.De"Adhoc mode signed in : %s\n", bSigne"true"e ?"false" : );
			}
			else
			{
                        CGameNetworkManager::setAdhocMode(false);
                                app.De"PSN mode signed in : %s\n", bSigne"true"e ?"false" : 
#endif //__PSVITA__

			if(!bSignedInL#if defined(__PS3__) || defined __PSVITA__// enable input again
				m_bIgnorePress=fa// Not sure why 360 doesn't need this, but leaving as __PS3__ only for now until we see that it does. Without this, on a PS3 offline game, the primary player just gets the default Player1234 type name
				pMinecraft->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad()));

				m_eAction=eAction_RunGam// get them to sign in to online
				UINT uiIDA[2];
				uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
				uiIDA[1]=IDS_PRO_NOTONLINE
#ifdef __PSVITA__
				if (CGameNetworkManager::usingAdhocMode())
				{
                            uiIDA[0] = IDS_NETWORK_ADHOC;  // this should be
                                                           // "Connect to adhoc
                                                           // network"
                            ui.RequestErrorMessage(
                                IDS_PRO_NOTADHOCONLINE_TITLE,
                                IDS_PRO_NOTADHOCONLINE_TEXT, uiIDA, 2,
                                ProfileManager.GetPrimaryPad(),
                                &UIScene_MainMenu::MustSignInReturnedPSN, this);
				}
				else
	/* 4J-PB - Add this after release
					// Determine why they're not "signed in live"
					if (ProfileManager.IsSignedInPSN(iPad))
					{
						m_eAction=eAction_RunGame;
						// Signed in to PSN but not connected (no internet access)

						UINT uiIDA[1];
						uiIDA[0] = IDS_PRO_NOTONLINE_DECLINE;
						ui.RequestMessageBox( IDS_ERROR_NETWORK_TITLE, IDS_ERROR_NETWORK, uiIDA, 1, iPad, UIScene_MainMenu::PlayOfflineReturned, this, app.GetStringTable());
					}
					else
					{		
						m_eAction=eAction_RunGamePSN;
						// Not signed in to PSN
						ui.RequestMessageBox( IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, iPad, &UIScene_MainMenu::MustSignInReturnedPSN, this, app.GetStringTable());
						return;
					}	*/
					ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, ProfileManager.GetPrimaryPad(),&UIScene_MainMenu::MustSignInReturnedPSN,this);
#else }


				ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, iPad, &UIScene_MainMenu::MustSignInReturnedPS#endifs
#elif defined \
    __ORBIS__    // Determine why they're not "signed in live"
				if (ProfileManager.isSignedInPSN(iPad))
				{
					m_eAction=eAction_RunG// Signed in to PSN but not connected (no internet access)
					assert(!ProfileManager.isConnectedToPSN(iPad));

					UINT uiIDA[1];
					uiIDA[0] = IDS_PRO_NOTONLINE_DECLINE;
					ui.RequestErrorMessage( IDS_ERROR_NETWORK_TITLE, IDS_ERROR_NETWORK, uiIDA, 1, iPad, UIScene_MainMenu::PlayOfflineReturned, this);
				}
				else
				{		
					m_eAction=eAction_RunGame// Not signed in to PSN
					UINT uiIDA[2];
					uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
					uiIDA[1] = IDS_PRO_NOTONLINE_DECLINE;
					ui.RequestAlertMessage( IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, iPad, &UIScene_MainMenu::MustSignInReturnedPSN, this);
					retu#else			}

				ProfileManager.SetLockedProfi#ifdef _XBOX_ONE
				ui.ShowPlayerDisplayna#endife);

				proceedToScene(ProfileManager.GetPrimaryPad(), eUIScene_LoadOrJ#endifu);

			}
#ifdef _XBOX_ONE
				if (!app.GetBanListRead(iPad))
				{
					app.SetTMSAction(iPad,eTMSAction_TMSPP_RetrieveFiles_RunPlayGam// block all input
					m_bIgnorePress=t// We want to hide everything in this scene and display a timer until we get a completion for the TMS files// 					for(int i=0;i<eControl_Count;i++)// 					{
// 						m_buttons[i].set(false);// 					}


					updateTooltips();

					m_controlTimer.setVisible( true#endif	#if TO_BE_IMPLEMENTED// Check if there is any new DLC
				app.ClearNewDLCAvailable();
				StorageManager.GetAvailableDLCCount(iP// check if all the TMS files are loaded
				if(app.GetTMSDLCInfoRead() && app.GetTMSXUIDsFileRead() && app.GetBanListRead(iPad))
				{
					if(StorageManager.SetSaveDevice(&CScene_Main::DeviceSelectReturned,this)==true)
			// change the minecraft player name
						pMinecraft->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad()// save device already selected// ensure we've applied this player's settings
						app.ApplyGameSettingsChanged(iPa// check for DLC// start timer to track DLC check finished
						m_Timer.SetShow(TRUE);
						XuiSetTimer(m_hObj,DLC_INSTALLED_TIMER_ID,DLC_INSTALLED_TIMER_TIM//app.NavigateToScene(iPad,eUIScene_MultiGameJoinLoad);
					}
				}
				else
	// Changing to async TMS calls
					app.SetTMSAction(iPad,eTMSAction_TMSPP_RetrieveFiles_RunPlayGam// block all input
					m_bIgnorePress=t// We want to hide everything in this scene and display a timer until we get a completion for the TMS files
					for(int i=0;i<BUTTONS_MAX;i++)
					{
						m_Buttons[i].SetShow(FALSE);
					}

					updateTooltips();

					m_Timer.SetShow(TRU#else			}

				pMinecraft->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad(// ensure we've applied this player's settings
				app.ApplyGameSettingsChang
#ifdef _XBOX_ONE
				ui.ShowPlayerDisplayna #endife);

				proceedToScene(ProfileManager.GetPrimaryPad(), eUIScene_LoadOrJ#endifu);

			}
		}
		el// 4J-PB - if this is the trial game, we can't have any networking// go straight in to the trial level// change the minecraft player name
			Minecraft::GetInstance()->user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad// Can't apply the player's settings here - they haven't come back from the QuerySignInStatud call above yet.// Need to let them action in the main loop when they come in// ensure we've applied this player's settings//app.ApplyGameSettingsChanged(iPad);
#if defined(__PS3__) || defined(__ORBIS__) || \
    defined(                                  \
        __PSVITA__)    // ensure we've applied this player's settings - we do have them on PS3
			app.ApplyGameSettingsChang#endifd
#ifdef __ORBIS__
			if (!g_NetworkManager.IsReadyToPlayOrIdle())
			{
				m_bLoadTrialOnNetworkManagerReady = true;
				ui.NavigateToScene(iPad, eUIScene_Timer);
#endiflse

			{
				LoadTrial();
			}
		}
	}
}

void UIScene_MainMenu::RunLeaderboards(int iPad)
{
	UINT uiIDA[1];
	uiIDA[0]=I// guests can't look at leaderboards
	if(ProfileManager.IsGuest(iPad))
	{
		ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
	}
	else if(!ProfileManager.IsSignedInLive(#if defined __PS3__ || defined __PSVITA__
		m_eAction=eAction_RunLeaderboa// get them to sign in to online
		UINT uiIDA[1];
		uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
		ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),&UIScene_MainMenu::MustSignInReturnedPS/* 4J-PB - Add this after release
#elif defined __PSVITA__
		m_eAction=eAction_RunLeaderboardsPSN;
		// Determine why they're not "signed in live"
		if (ProfileManager.IsSignedInPSN(iPad))
		{
			// Signed in to PSN but not connected (no internet access)
			UINT uiIDA[1];
			uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
			ui.RequestMessageBox(IDS_PRO_CURRENTLY_NOT_ONLINE_TITLE, IDS_PRO_PSNOFFLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(), &UIScene_MainMenu::MustSignInReturnedPSN, this, app.GetStringTable());
		}
		else
		{		
			// Not signed in to PSN
			UINT uiIDA[1];
			uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
			ui.RequestMessageBox(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(), &UIScene_MainMenu::MustSignInReturnedPSN, this, app.GetStringTable());
			return;
		}*/#elif defined __ORBIS__
		m_eAction=eAction_RunLeaderboa// Determine why they're not "signed in live"
		if (ProfileManager.isSignedInPSN(iPad// Signed in to PSN but not connected (no internet access)
			assert(!ProfileManager.isConnectedToPSN(iPad));

			UINT uiIDA[1];
			uiIDA[0] = IDS_OK;
			ui.RequestErrorMessage( IDS_ERROR_NETWORK_TITLE, IDS_ERROR_NETWORK, uiIDA, 1, iPad);
		}
		else// Not signed in to PSN
			UINT uiIDA[1];
			uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
			ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(), &UIScene_MainMenu::MustSignInReturnedPSN, this);
			re#else
		}

		ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, u#endif1);

	}
	e// we're supposed to check for parental control restrictions before showing leaderboards// The title enforces the user's NP parental control setting for age-based content//restriction in network communications.// If age restrictions are in place and the user's age does not meet// the age restriction of the title's online service content rating (CERO, ESRB, PEGI, etc.), then the title must//display a message such as the following and disallow online service for this user.

		bool bContentRestrict#if defined(__PS3__) || defined(__PSVITA__)
		ProfileManager.GetChatAndContentRestrictions(iPad,true,NULL,&bContentRestrict#endifL);

		if(bContentRestri#if !(defined(_XBOX) || defined(_WINDOWS64) || defined(_XBOX_ONE)) // 4J Stu - Temp to get the win build running, but so we check this for other platforms// you can't see leaderboards
			UINT uiIDA[1];
			uiIDA[0]=IDS_CONFIRM_OK;
			ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrimaryPad(),NU#endifs);

		}
		else
		{
			ProfileManager.SetLockedProfile(// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
			ProfileManager.QuerySignin
#ifdef _XBOX_ONE
			ui.ShowPlayerDisplayna #endife);

			proceedToScene(iPad, eUIScene_LeaderboardsMenu);
		}
	}
}
void UIScene_MainMenu::RunUnlockOrDLC(int iPad)
{
	UINT uiIDA[1];
	uiIDA[0]=// Check if this means downloadable content
	if(ProfileManager.IsFullVers#ifdef __ORBIS__// 4J-PB - Check if there is a patch for the game
		m_errorCode = ProfileManager.getNPAvailability(ProfileManager.GetPrimaryPad());

		bool bPatchAvailable;
		switch(m_errorCode)
		{
		case SCE_NP_ERROR_LATEST_PATCH_PKG_EXIST:
		case SCE_NP_ERROR_LATEST_PATCH_PKG_DOWNLOADED:
			bPatchAvailable=true;
			break;
		default:
			bPatchAvailable=false;
			break;
		}

		if(bPatchAvailable)
		{
			m_bIgnorePress=false;

			int32_t ret=sceErrorDialogInitialize();
			m_bErrorDialogRunning=true;
			if (  ret==SCE_OK ) 
			{
				SceErrorDialogParam param;
				sceErrorDialogParamInitialize( &par// 4J-PB - We want to display the option to get the patch now
				param.errorCode = SCE_NP_ERROR_LATEST_PATCH_PKG_D//pClass->m_errorCode;
				ret = sceUserServiceGetInitialUser( &param.userId );
				if ( ret == SCE_OK ) 
				{
					ret=sceErrorDialogOpen( &param );
		// 			UINT uiIDA[1];// 			uiIDA[0]=IDS_OK;// 			ui.RequestMessageBox(IDS_PATCH_AVAILABLE_TITLE, IDS_PATCH_AVAILABLE_TEXT, uiIDA, 1, XUSER_INDEX_ANY,NULL,this);
			retur// Check if PSN is unavailable because of age restriction
		if (m_errorCode == SCE_NP_ERROR_AGE_RESTRICTION)
		{
			m_bIgnorePress=false;
			UINT uiIDA[1];
			uiIDA[0] = IDS_OK;
			ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrimaryPad(), nullptr, this);

			re#endif		}// downloadable content
		if(ProfileManager.IsSignedInLive(iPad))
		{
			if(ProfileManager.IsGuest(iPad))
			{
				m_bIgnorePress=false;
				ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
			}
			else
// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
				ProfileManager.QuerySignin
#if defined _XBOX_ONE
				if (app.GetTMSDLCIn #endif())

				{
					bool bContentRestrict#if defined(__PS3__) || defined(__PSVITA__)
					ProfileManager.GetChatAndContentRestrictions(iPad,true,NULL,&bContentRestrict#endifL);

					if(bContentRestricted)
					{
						m_bIgnorePre#if !(defined(_XBOX) || defined(_WINDOWS64) || defined(_XBOX_ONE)) // 4J Stu - Temp to get the win build running, but so we check this for other platforms// you can't see the store
						UINT uiIDA[1];
						uiIDA[0]=IDS_CONFIRM_OK;
						ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrimaryPad(),NU#endifs);

					}
					else
					{
						ProfileManager.SetLockedProfi#ifdef _XBOX_ONE
						ui.ShowPlayerDisplayna#endife);

						proceedToScene(ProfileManager.GetPrimaryPad(), eUIScene_DLCMainMenu);
#if defined _XBOX_ONE
				else
	// Changing to async TMS calls
					app.SetTMSAction(iPad,eTMSAction_TMSPP_RetrieveFiles_DLCMai// block all input
					m_bIgnorePress=t// We want to hide everything in this scene and display a timer until we get a completion for the TMS files// 					for(int i=0;i<BUTTONS_MAX;i++)// 					{
// 						m_Buttons[i].SetShow(FALSE);// 					}


					updateTooltips();

					m_controlTimer.setVisible( true );
					m_bWaitingForDLCInfo=tr#endif		}
// read the DLC info from TMS/*app.ReadDLCFileFromTMS(iPad);*/// We want to navigate to the DLC scene, but block input until we get the DLC file in from TMS// Don't navigate - we might have an uplink disconnect//app.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_DLCMainMenu);

			}
		}
#if defined(__PS3__) || defined(__PSVITA__)
			m_eAction = \
    eAction_RunUnlockOrD  // get them to sign in to online
			UINT uiIDA[1];
			uiIDA[0]=IDS_PRO_NOTONLINE_A//uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
			ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),&UIScene_MainMenu::MustSignInReturnedP#elif defined __ORBIS__
			m_eAction=eAction_RunUnlockOrD// Determine why they're not "signed in live"
			if (ProfileManager.isSignedInPSN(iPad))
			{
				m_bIgnorePress=f// Signed in to PSN but not connected (no internet access)
				assert(!ProfileManager.isConnectedToPSN(iPad));

				UINT uiIDA[1];
				uiIDA[0] = IDS_OK;
				ui.RequestErrorMessage( IDS_ERROR_NETWORK_TITLE, IDS_ERROR_NETWORK, uiIDA, 1, iPad);
			}
			else
	// Not signed in to PSN
				UINT uiIDA[1];
				uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
				ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(), &UIScene_MainMenu::MustSignInReturnedPSN, this);
				ret#else			}

			UINT uiIDA[1];
			uiIDA[0]=IDS_OK;
			ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, u#endif1);

		}
	}
	// guests can't buy the game
		if(ProfileManager.IsGuest(iPad))
		{
			m_bIgnorePress=false;
			ui.RequestErrorMessage(IDS_UNLOCK_TITLE, IDS_UNLOCK_GUEST_TEXT, uiIDA, 1,iPad);
		}
		else if(!ProfileManager.IsSignedInLive(i#if defined(__PS3__) || defined(__PSVITA__)
			m_eAction=eAction_RunUnlockOrD// get them to sign in to online
			UINT uiIDA[1];
			uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
			ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(),&UIScene_MainMenu::MustSignInReturnedP#elif defined __ORBIS__
			m_eAction=eAction_RunUnlockOrD// Determine why they're not "signed in live"
			if (ProfileManager.isSignedInPSN(iPad))
			{
				m_bIgnorePress=f// Signed in to PSN but not connected (no internet access)
				assert(!ProfileManager.isConnectedToPSN(iPad));

				UINT uiIDA[1];
				uiIDA[0] = IDS_OK;
				ui.RequestErrorMessage( IDS_ERROR_NETWORK_TITLE, IDS_ERROR_NETWORK, uiIDA, 1, iPad);
			}
			else
	// Not signed in to PSN
				UINT uiIDA[1];
				uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
				ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 1, ProfileManager.GetPrimaryPad(), &UIScene_MainMenu::MustSignInReturnedPSN, this);
				ret#else			}

			UINT uiIDA[1];
			uiIDA[0]=IDS_OK;
			ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_XBOXLIVE_NOTIFICATION, u#endif1);


		}
		el// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
			ProfileManager.QuerySigninStat// check that the commerce is in the right state to be able to display the full version purchase - if the user is fast with the trial version, it can still be retrieving the product list#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
			m_bIgnorePress=true;
			m_bLaunchFullVersionPurch
#elseue;

			TelemetryManager->RecordUpsellPresented(iPad, eSen_UpsellID_Full_Version_Of_Game, app.m_dwOfferID);
			ProfileManager.DisplayFullVersionPurchase(false,iPad,eSen_UpsellID_Full_Version_#endife);

		}
	}
}

void UIScene_MainMenu::tick()
{
	UIScene::tick();

	if ( (eNavigateWhenReady >= 0) )
	{
		
		int lockedProfile = ProfileManager.GetLockedP
#ifdef _DURANGO			    // 4J-JEV:	DLC menu contains text localised to system language which we can't change.// 			We need to switch to this language in-case it uses a different font.
		if (eNavigateWhenReady == eUIScene_DLCMainMenu) setLanguageOverride(false);

		bool isSignedIn;
		C4JStorage::eOptionsCallback status;
		bool pendingFontChange;
		if (lockedProfile >= 0)
		{
			isSignedIn = ProfileManager.IsSignedIn(lockedProfile);
			status = app.GetOptionsCallbackStatus(lockedProfile);
			pendingFontChange = ui.PendingFontChange();

			if(status == C4JStorage::eOptions_Callback_Idle)// make sure the TMS banned list data is ditched - the player may have gone in to help & options, backed out, and signed out
				app.InvalidateBannedList(lockedProfi// need to ditch any DLCOffers info
				StorageManager.ClearDLCOffers();
				app.ClearAndResetDLCDownloadQueue();
				app.ClearDLCInstalled();
			}
		}

		if (		(lockedProfile >= 0)
				&&	isSignedIn
				&&	((status == C4JStorage::eOptions_Callback_Read)||(status == C4JStorage::eOptions_Callback_Write))
				&&	!pendingFontCha#endif		)

		{
			app.De"[MainMenu] Navigating away from MainMenu.\n");
			ui.NavigateToScene(lockedProfile, eNavigateWhenReady);
			eNavigateWhenReady = (EUIScene#ifdef _DURANGO
		else
		{
			app.De"[MainMenu] Delaying navigation: lockedProfile=%i, %s, status=%ls, %s.\n", 
				lockedProfile,
				isS"SignedIn""SignedOut",
				app.toStringOptionsStatus(status).c_str(),
				pendingFon"Pending font change""font OK" #endif		}

#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__)
	if (m_bLaunchFullVersionPurchase)
	{
		int iCommerceState=app.GetCommerceS// 4J-PB - if there's a commerce error - store down, player can't access store - let the DisplayFullVersionPurchase show the error
		if((iCommerceState==CConsoleMinecraftApp::eCommerce_State_Online) || (iCommerceState==CConsoleMinecraftApp::eCommerce_State_Error))
		{
			m_bLaunchFullVersionPurchase=false;
			m_bIgnorePress=false;
			updateToolti// 4J-PB - need to check this user can access the store
			bool bContentRestricted=false;
			ProfileManager.GetChatAndContentRestrictions(ProfileManager.GetPrimaryPad(),true,NULL,&bContentRestricted,NULL);
			if(bContentRestricted)
			{
				UINT uiIDA[1];
				uiIDA[0]=IDS_CONFIRM_OK;
				ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, ProfileManager.GetPrimaryPad());
			}
			else
			{
				TelemetryManager->RecordUpsellPresented(ProfileManager.GetPrimaryPad(), eSen_UpsellID_Full_Version_Of_Game, app.m_dwOfferID);
				ProfileManager.DisplayFullVersionPurchase(false,ProfileManager.GetPrimaryPad(),eSen_UpsellID_Full_Version_Of_Game);
			}// 4J-PB - check for a trial version changing to a full version
	if(m_bTrialVersion)
	{
		if(ProfileManager.IsFullVersion())
		{
			m_bTrialVersion=false;
			m_buttons[(int)eControl_UnlockOrDLC].init(app.GetString(IDS_DOWNLOADABLECONTENT),eControl_UnlockOrDLC#endif

#if defined _XBOX_ONE
	if (m_bWaitingForDLCInfo)
	{	
		if(app.GetTMSDLCInfoRead())
		{		
			m_bWaitingForDLCInfo=false;
			ProfileManager.SetLockedProfile(m_iPad);
			proceedToScene(ProfileManager.GetPrimaryPad(), eUIScene_DLCMainMenu);
		}
	}

	if(g_NetworkManager.ShouldMessageForFullSession())
	{
		UINT uiIDA[1];
		uiIDA[0]=IDS_CONFIRM_OK;
		ui.RequestErrorMessage( IDS_CONNECTION_FAILED, IDS_IN_PARTY_SESSION_FULL, uiIDA,1,ProfileManager.GetPrimaryP#endif

#ifdef __ORBIS__    // process the error dialog (for a patch being available)// SQRNetworkManager_Orbis::tickErrorDialog also runs the error dialog, so wrap this so this doesn't terminate a signin dialog
	if(m_bErrorDialogRunning)
	{	
		SceErrorDialogStatus stat = sceErrorDialogUpdateStatus();
		if( stat == SCE_ERROR_DIALOG_STATUS_FINISHED ) 
		{
			sceErrorDialogTermin// if m_bRunGameChosen is true, we're here after selecting play game, and we should let the user continue with an offline game
			if(m_bRunGameChosen)
			{
				m_bRunGameChosen=false;
				m_eAction = eAction_RunG// give the option of continuing offline
				UINT uiIDA[1];
				uiIDA[0]=IDS_PRO_NOTONLINE_DECLINE;
				ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION_PATCH_AVAILABLE, uiIDA, 1, ProfileManager.GetPrimaryPad(), &UIScene_MainMenu::PlayOfflineReturned, this);

			}
			m_bErrorDialogRunning=false;
		}
	}

	if(m_bLoadTrialOnNetworkManagerReady && g_NetworkManager.IsReadyToPlayOrIdle())
	{
		m_bLoadTrialOnNetworkManagerReady = false;
		LoadTr
#endif }

}

void UIScene_MainMenu::RunAchievements(in#if TO_BE_IMPLEMENTED
	UINT uiIDA[1];
	uiIDA[0]=// guests can't look at achievements
	if(ProfileManager.IsGuest(iPad))
	{
		ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
	}
	else
	{
		XShowAchievementsUI( i#endif
	}

}

void UIScene_MainMenu::RunHelpAndOptions(int iPad)
{
	if(ProfileManager.IsGuest(iPad))
	{
		UINT uiIDA[1];
		uiIDA[0]=IDS_OK;
		ui.RequestErrorMessage(IDS_PRO_GUESTPROFILE_TITLE, IDS_PRO_GUESTPROFILE_TEXT, uiIDA, 1);
	}
	// If the player was signed in before selecting play, we'll not have read the profile yet, so query the sign-in status to get this to happen
		ProfileManager.QuerySignin
#if TO_BE_IMPLEMENTED    // 4J-PB - You can be offline and still can go into help and options
		if(app.GetTMSDLCInfoRead() || !ProfileManager.IsSignedInLi#endifd))

		{
			ProfileManager.SetLockedProfi#ifdef _XBOX_ONE
			ui.ShowPlayerDisplayna#endife);

			proceedToScene(iPad, eUIScene_HelpAndOptionsM#if TO_BE_IMPLEMENTED
		el// Changing to async TMS calls
			app.SetTMSAction(iPad,eTMSAction_TMSPP_RetrieveFiles_HelpAndOpti// block all input
			m_bIgnorePress// We want to hide everything in this scene and display a timer until we get a completion for the TMS files
			for(int i=0;i<BUTTONS_MAX;i++)
			{
				m_Buttons[i].SetShow(FALSE);
			}

			updateTooltips();

			m_Timer.SetShow(T#endif		}

	}
}

void UIScene_MainMenu::LoadTrial(void)
{		
	app.SetTutorialMode( // clear out the app's terrain features list
	app.ClearTerrainFeaturePosition();

	StorageManager.ResetSave// Need to set the mode as trial
	ProfileManager.StartTrial// No saving in the trial
	StorageManager.SetSaveDisabled(true);
	app.SetGameHostOption(eGameHostOption_WasntSaveOwner, f// Set the global flag, so that we don't disable saving again once the save is complete
	app.SetGameHostOption(eGameHostOption_DisableSaving, 1);

	StorageManager.SetS"Tutorial"// Reset the autosave time
	app.SetAutosaveTimer// not online for the trial game
	g_NetworkManager.HostGame(0,false,true,MINECRAFT_NET_MAX_PL
#ifndef _XBOX
	g_NetworkManager.FakeLocalPlayer #endif();


	NetworkGameInitData *param = new NetworkGameInitData();
	param->seed = 0;
	param->saveData = NULL;
	param->settings = app.GetGameHostOption( eGameHostOption_Tutorial ) | app.GetGameHostOption(eGameHostOption_DisableSaving);

	std::vector<LevelGenerationOptions *> *generators = app.getLevelGenerators();
	param->levelGen = generators->at(0);

	LoadingInputParams *loadingParams = new LoadingInputParams();
	loadingParams->func = &CGameNetworkManager::RunNetworkGameThreadProc;
	loadingParams->lpParam = (LPVOID)param;

	UIFullscreenProgressCompletionData *completionData = new UIFullscreenProgressCompletionData();
	completionData->bShowBackground=TRUE;
	completionData->bShowLogo=TRUE;
	completionData->type = e_ProgressCompletion_CloseAllPlayersUIScenes;
	completionData->iPad = ProfileManager.GetPrimaryPad();
	loadingParams->completionData = completionData;

	ui.ShowTri
#ifdef _XBOX_ONE
	ui.ShowPlayerDisp #endife(true);

	ui.NavigateToScene(ProfileManager.GetPrimaryPad(),eUIScene_FullscreenProgress, loadingParams);
}

void UIScene_MainMenu::handleUnlockFullVersion()
{
	m_buttons[(int)eControl_UnlockOrDLC].setLabel(IDS_DOWNLOADABLECO

#ifdef __PSVITA__
int UIScene_MainMenu::SelectNetworkModeReturned( \
    void* pParam, int iPad, C4JStorage::EMessageResult result)
{
	UIScene_MainMenu* pClass = (UIScene_MainMenu*)pParam;

	if(result==C4JStorage::EMessage_ResultAccept) 
	{
		a"Setting network mode to PSN\n");
		app.SetGameSettings(0, eGameSetting_PSVita_NetworkModeAdhoc, 0);
	}
	else if(result==C4JStorage::EMessage_ResultDecline) 
	{
		a"Setting network mode to Adhoc\n");
		app.SetGameSettings(0, eGameSetting_PSVita_NetworkModeAdhoc, 1);
	}
	pClass->updateTooltips()#endif //__PSVITA__
