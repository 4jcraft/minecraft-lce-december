#include "../../stdafx.h"
#include "UI.h"
#include "UIScene_InGameInfoMenu.h"
#include "../../MultiPlayerLocalPlayer.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.network.packet.h"
#include "../../MultiPlayerLocalPlayer.h"
#include "../../ClientConnection.h"

UIScene_InGameInfoMenu::UIScene_InGameInfoMenu(int iPad, void* initData,
                                               UILayer* parentLayer)
    : UIScene(iPad, parentLayer) {
    // Setup all the Iggy references we need for this scene
    initialiseMovie();

    m_buttonGameOptions.init(app.GetString(IDS_HOST_OPTIONS),
                             eControl_GameOptions);
    m_labelTitle.init(app.GetString(IDS_PLAYERS_INVITE));
    m_playerList.init(eControl_GamePlayers);

    m_players = std::vector<PlayerInfo*>();

    DWORD playerCount = g_NetworkManager.GetPlayerCount();

    for (DWORD i = 0; i < playerCount; ++i) {
        INetworkPlayer* player = g_NetworkManager.GetPlayerByIndex(i);

        if (player != NULL) {
            PlayerInfo* info = BuildPlayerInfo(player);

            m_players.push_back(info);
            m_playerList.addItem(info->m_name, info->m_colorState,
                                 info->m_voiceStatus);
        }
    }

    g_NetworkManager.RegisterPlayerChangedCallback(
        m_iPad, &UIScene_InGameInfoMenu::OnPlayerChanged, this);

    INetworkPlayer* thisPlayer =
        g_NetworkManager.GetLocalPlayerByUserIndex(m_iPad);
    m_isHostPlayer = false;
    if (thisPlayer != NULL) m_isHostPlayer = thisPlayer->IsHost() == TRUE;

    Minecraft* pMinecraft = Minecraft::GetInstance();
    std::shared_ptr<MultiplayerLocalPlayer> localPlayer =
        pMinecraft->localplayers[m_iPad];
    if (!m_isHostPlayer && !localPlayer->isModerator()) {
        removeControl(&m_buttonGameOptions, false);
    }

    updateTo
#if TO_BE_IMPLEMENTED
        SetTimer(TOOLTIP_TIMERID,
                 INGAME_INFO_TOOLTIP #endif);  // get rid of the quadrant
                                               // display if it's on
    ui.HidePres
#if TO_BE_IMPLEMENTED
        SetTimer(IGNORE_KEYPRESS_TIMERID, IGNORE_KEYPRE #endifE);
}

UIScene_InGameInfoMenu::~UIScene_InGameInfo  // Delete player infos
    for (int i = 0; i < m_players.size(); i++) {
    delete m_players[i];
}
}

std::wstring UIScene_InGameInfoMenu::getMoviePath() {
    if (app.GetLocalPlayerCount() > 1) {
        "InGameInfoMenuSplit";
    } else {
        "InGameInfoMenu";
    }
}

void UIScene_InGameInfoMenu::updateTooltips() {
    int keyX = IDS_TOOLTIPS_INVITE_FRIENDS;
    int ikeyY = -1;

    XPARTY_USER_LIST partyList;
    if ((XPartyGetUserList(&partyList) != XPARTY_E_NOT_IN_PARTY) &&
        (partyList.dwUserCount > 1)) {
        keyX = IDS_TOOLTIPS_INVITE_PARTY;
    }

    if (g_NetworkManager.IsLocalGame())
        k #ifdef __PSVITA__ if (CGameNetworkManager::usingAdhocMode())
                k #endif -
            1;

    INetworkPlayer* selectedPlayer = g_NetworkManager.GetPlayerBySmallId(
        m_players[m_playerList.getCurrentSelection()]->m_smallId);

    int keyA = -1;
    Minecraft* pMinecraft = Minecraft::GetInstance();
    std::shared_ptr<MultiplayerLocalPlayer> localPlayer =
        pMinecraft->localplayers[m_iPad];

    bool isOp = m_isHostPlayer || localPlayer->isModerator();
    bool cheats = app.GetGameHostOption(eGameHostOption_CheatsEnabled) != 0;
    bool trust = app.GetGameHostOption(eGameHostOption_TrustPlayers) != 0;

    if (isOp) {
        if (m_buttonGameOptions.hasFocus()) {
            keyA = IDS_TOOLTIPS_SELECT;
        } else if (selectedPlayer != NULL) {
            bool editingHost = selectedPlayer->IsHost();
                        if( (cheats && (m_isHostPlayer || !editingHost ) ) || (!trust && (m_isHostPlayer || #if (!defined(_CONTENT_PACKAGE) && !defined(_FINAL_BUILD) && defined(_DEBUG_MENUS_ENABLED))
				|| (m_isHostPlayer &#endifingHost)

				)
			{
                keyA = IDS_TOOLTIPS_PRIVILEGES;
			}
			else if(selectedPlayer->IsLocal() != TRUE && selectedPlayer->IsSameSystem(g_NetworkManager.GetHostPlayer()) != // Only ops will hit this, can kick anyone not local and not local to the host
				keyA = IDS_TOOLTIPS_KIC
#if defined(__PS3__) || defined(__ORBIS__)
	if (m_iPad == ProfileManager.GetPrimaryPad()) ikeyY = \
    IDS_TOOLTIPS #else_INVITES;

	if(!m_buttonGameOptions.ha// if the player is me, then view gamer profile
		if(selectedPlayer != NULL && selectedPlayer->IsLocal() && selectedPlayer->GetUserIndex()==m_iPad)
		{
                ikeyY = IDS_TOOLTIPS_VIEW_GAMERPROFILE;
		}
		else
		{
                ikeyY = IDS_TOOLTIPS_VIEW_GAM #endif;
		}
        }

        ui.SetTooltips(m_iPad, keyA, IDS_TOOLTIPS_BACK, keyX, ikeyY);
    }

    void UIScene_InGameInfoMenu::handleDestroy() {
        g_NetworkManager.UnRegisterPlayerChangedCallback(
            m_iPad, &UIScene_InGameInfoMenu::OnPlayerChanged, this);

        m_parentLayer->removeComponent(eUIComponent_MenuBackground);
    }

    void UIScene_InGameInfoMenu::handleGainFocus(bool navBack) {
        UIScene::handleGainFocus(navBack);
        if (navBack)
            g_NetworkManager.RegisterPlayerChangedCallback(
                m_iPad, &UIScene_InGameInfoMenu::OnPlayerChanged, this);
    }

    void UIScene_InGameInfoMenu::handleReload() {
        DWORD playerCount = g_NetworkManager.GetPl  // Remove all player info
                            for (int i = 0; i < m_players.size(); i++) {
            delete m_players[i];
        }
        m_players.clear();

        for (DWORD i = 0; i < playerCount; ++i) {
            INetworkPlayer* player = g_NetworkManager.GetPlayerByIndex(i);

            if (player != NULL) {
                PlayerInfo* info = BuildPlayerInfo(player);

                m_players.push_back(info);
                m_playerList.addItem(info->m_name, info->m_colorState,
                                     info->m_voiceStatus);
            }
        }

        INetworkPlayer* thisPlayer =
            g_NetworkManager.GetLocalPlayerByUserIndex(m_iPad);
        m_isHostPlayer = false;
        if (thisPlayer != NULL) m_isHostPlayer = thisPlayer->IsHost() == TRUE;

        Minecraft* pMinecraft = Minecraft::GetInstance();
        std::shared_ptr<MultiplayerLocalPlayer> localPlayer =
            pMinecraft->localplayers[m_iPad];
        if (!m_isHostPlayer && !localPlayer->isModerator()) {
            removeControl(&m_buttonGameOptions, false);
        }

        updateTooltips();

        if (controlHasFocus(eControl_GamePlayers)) {
            m_playerList.setCurrentSelection(getControlChildFocus());
        }
    }

    void UIScene_InGameInfoMenu::tick() {
        // Update players by index
        for (DWORD i = 0; i < m_players.size(); ++i) {
            INetworkPlayer* player = g_NetworkManager.GetPlayerByIndex(i);

            if (player != NULL) {
                PlayerInfo* info = BuildPlayerInfo(player);

                m_players[i]->m_smallId = info->m_smallId;

                if (info->m_voiceStatus != m_players[i]->m_voiceStatus) {
                    m_players[i]->m_voiceStatus = info->m_voiceStatus;
                    m_playerList.setVOIPIcon(i, info->m_voiceStatus);
                }

                if (info->m_colorState != m_players[i]->m_colorState) {
                    m_players[i]->m_colorState = info->m_colorState;
                    m_playerList.setPlayerIcon(i, info->m_colorState);
                }

                if (info->m_name.compare(m_players[i]->m_name) != 0) {
                    m_playerList.setButtonLabel(i, info->m_name);
                    m_players[i]->m_name = info->m_name;
                }

                delete info;
            }
        }
    }

void UIScene_InGameInfoMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released//app.DebugPrintf("UIScene_DebugOverlay handling input for pad %d, key %d, down- %s, pressed- %s, released- %s\n", iPad, key, down?"TRUE":"FALSE", pressed?"TRUE":"FALSE", released?"TRUE":"FALSE");
	ui.AnimateKeyPress(m_iPad, key, repeat, pressed, released);

	switch(key)
	{
        case ACTION_MENU_CANCEL:
            if (pressed && !repeat) {
                ui.PlayUISFX(eSFX_Back);
                navigateBack();
            }
            break;
#if defined(__PS3__) || defined(__ORBIS__)
            if (pressed && iPad == ProfileManager.G #ifdef __PS3__)
                // are we offline?
                        if(!ProfileManager.IsSignedInL// get them to sign in to online
				UINT uiIDA[2];
				uiIDA[0]=IDS_PRO_NOTONLINE_ACCEPT;
				uiIDA[1]=IDS_PRO_NOTONLINE_DECLINE;
				ui.RequestErrorMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, ProfileManager.GetPrimaryPad(),&UIScene_InGameInfoMenu::MustSignInReturnedPSN,#endif
			}
#ifdef __ORBIS__ {

				SQRNetworkManager_Orbi#else // __PS3__();

				int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CONTAINER_ID_INVALID);
"sceNpBasicRecvMessageCustom return %d ( %08x )\n"#endif , ret, ret#else
}
}

if (pressed && m_playerList.hasFocus() && (m_playerList.getItemCount() > 0) &&
    (m_playerList.getCurrentSelection() < m_players.size())) {
    INetworkPlayer* player = g_NetworkManager.GetPlayerBySmallId(
        m_players[m_playerList.getCurrentSelection()]->m_smallId);
    if (player != NULL) {
        PlayerUID uid = player->GetUID();
                                if( uid != I#ifdef __PSVITA__	{

#elseSVITA_STUBBED;
            ProfileManager.ShowPro #endifrd(iPad, uid);

#endif }
    }
    }

    break;
    case ACTION_MENU_X:

                if(pressed && !repeat && !g_NetworkManager.#ifdef __PSVITA__	{
            if (CGameNetworkManager::usingAdhocMode() == false)
                g_NetworkManager.S #elseviteGUI(iPad);

            g_NetworkManager.S #endifiteGUI(iPad);
        
		}

		break;
	c#ifdef __ORBIS__OK:

	case ACTION_M#endifUCHPAD_PRESS:

	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_PAGEUP:
	case ACTION_MENU_PAGEDOWN:
		sendInputToMovie(key, repeat, pressed, released);
		break;
}
}

void UIScene_InGameInfoMenu::handlePress(F64 controlId, F64 childId)"Pressed = %d, %d\n", (int)controlId, (int)childId);
switch ((int)controlId) {
    case eControl_GameOptions:
        ui.NavigateToScene(m_iPad, eUIScene_InGameHostOptionsMenu);
        break;
    case eControl_GamePlayers:
        int currentSelection = (int)childId;
        INetworkPlayer* selectedPlayer = g_NetworkManager.GetPlayerBySmallId(
            m_players[currentSelection]->m_smallId);

        Minecraft* pMinecraft = Minecraft::GetInstance();
        std::shared_ptr<MultiplayerLocalPlayer> localPlayer =
            pMinecraft->localplayers[m_iPad];

        bool isOp = m_isHostPlayer || localPlayer->isModerator();
        bool cheats = app.GetGameHostOption(eGameHostOption_CheatsEnabled) != 0;
        bool trust = app.GetGameHostOption(eGameHostOption_TrustPlayers) != 0;

        if (isOp && selectedPlayer != NULL) {
            bool editingHost = selectedPlayer->IsHost();
                        if( (cheats && (m_isHostPlayer || !editingHost ) ) || (!trust && (m_isHost#if (!defined(_CONTENT_PACKAGE) && !defined(_FINAL_BUILD) && defined(_DEBUG_MENUS_ENABLED))
				|| (m_isHo#endifer && editingHost)

				)
			{
                InGamePlayerOptionsInitData* pInitData =
                    new InGamePlayerOptionsInitData();
                pInitData->iPad = m_iPad;
                pInitData->networkSmallId =
                    m_players[currentSelection]->m_smallId;
                pInitData->playerPrivileges = app.GetPlayerPrivileges(
                    m_players[currentSelection]->m_smallId);
                ui.NavigateToScene(m_iPad, eUIScene_InGamePlayerOptionsMenu,
                                   pInitData);
			}
			else if(selectedPlayer->IsLocal() != TRUE && selectedPlayer->IsSameSystem(g_NetworkManager.GetHostPla// Only ops will hit this, can kick anyone not local and not local to the host
				BYTE *smallId = new BYTE();
				*smallId = m_players[currentSelection]->m_smallId;
				UINT uiIDA[2];
				uiIDA[0]=IDS_CONFIRM_OK;
				uiIDA[1]=IDS_CONFIRM_CANCEL;

				ui.RequestAlertMessage(IDS_UNLOCK_KICK_PLAYER_TITLE, IDS_UNLOCK_KICK_PLAYER, uiIDA, 2, m_iPad,&UIScene_InGameInfoMenu::KickPlayerReturned,smallId);
        }
}
break;
}
}

void UIScene_InGameInfoMenu::handleFocusChange(F64 controlId, F64 childId) {
    switch ((int)controlId) {
        case eControl_GamePlayers:
            m_playerList.updateChildFocus((int)childId);
    };
    updateTooltips();
}

void UIScene_InGameInfoMenu::OnPlayerChanged(void *callbackParam, INetworkPlayer *pPlayer, bool lea"<UIScene_InGameInfoMenu::OnPlayerChanged> Player \"%ls\" %s (smallId: %d)\n", pPlayer->Ge"leaving"me("joining"g ?  : , pPlayer->GetSmallId());

	UIScene_InGameInfoMenu *scene = (UIScene_InGameInfoMenu *)callbackParam;
	bool playerFound = false;
	int foundIndex = 0;
	for(int i = 0; i < scene->m_players.size(); ++i)
	{
    if (!playerFound &&
        scene->m_players[i]->m_smallId == pPlayer->GetSmallId()) {
        if (scene->m_playerList.getCurrentSelection() ==
            scene->m_playerList.getItemCount() - 1) {
                                scene->m_playerList.setCurrentSelection( scene->m_playerList.getIte// Player found
        }

        playerFound = true;
        foundIndex = i;
    }
	}

	if (leaving && !play"<UIScene_InGameInfoMenu::OnPlayerChanged> Error: Player \"%ls\" leaving but not found in list\n", pPlayer->GetOnlineName());
	if (!leaving && play"<UIScene_InGameInfoMenu::OnPlayerChanged> Error: Player \"%ls\" joining but already in list\n", pPla// If the player was found remove them (even if they're joining, they'll be added again later)
	if(playerFou"<UIScene_InGameInfoMenu::OnPlayerChanged> Player \"%ls\" found, removing\n", pPlay// Remove player info

		
		delete scene->m_players[foundIndex];
		scene->m_players.erase(scene->m_players.b// Remove player from list
		scene->m_playerList.remo// If the player is joining
	if(!leavi"<UIScene_InGameInfoMenu::OnPlayerChanged> Player \"%ls\" not found, adding\n", pPlayer->GetOnlineName());

		PlayerInfo *info = scene->BuildPlayerInfo(pPlayer);
		scene->m_playe// Note that the tick updates buttons every tick so it's only really important that we// add the button (not the order or content)
		scene->m_playerList.addItem(info->m_name, info->m_colorState, info->m_voiceStatus);
}
}

int UIScene_InGameInfoMenu::KickPlayerReturned(
    void* pParam, int iPad, C4JStorage::EMessageResult result) {
    BYTE smallId = *(BYTE*)pParam;
    delete pParam;

    if (result == C4JStorage::EMessage_ResultAccept) {
        Minecraft* pMinecraft = Minecraft::GetInstance();
        std::shared_ptr<MultiplayerLocalPlayer> localPlayer =
            pMinecraft->localplayers[iPad];
        if (localPlayer->connection) {
            localPlayer->connection->send(std::shared_ptr<KickPlayerPacket>(
                new KickPlayerPacket(smallId)));
        }
    }

    return 0;
}

UIScene_InGameInfoMenu::PlayerInfo* UIScene_InGameInfoMenu::BuildPlayerInfo(
    INetworkPlayer* player) {
    PlayerInfo* info = new PlayerInfo();
    info->m_smallId = player->Get "" al #ifndef _CONTENT_PACKAGEayerName = L;

        if(app.DebugSettingsOn() && (app.GetGameSettingsDebugMask()&(1L<<eDebugSetting_D"WWWWWWWWWWWWWWWW")
	{
        play #endif = L;
	}
	else

	{
        playerName = player->GetDisplayName();
	}

	int voiceStatus = 0;
	if(player != NULL && player->HasVoice() )
	{
                if( play// Muted imageocalUser(m_iPad) )
		{
            voiceStatus = 3;
		}
// Talking imager->IsTalking() )
		{
            // Not talking image;
		}
		else
		{
            voiceStatus = 1;
		}
	}

	info->m_voiceStatus = voiceStatus;
	info->m_colorState = app.GetPlayerColour(info->m_smallId);
	info
#if defined __PS3__ || defined __PSVITA__
int UIScene_InGameInfoMenu::MustSignInReturnedPSN( \
        void* pParam, int iPad, C4JStorage::EMessageResult result)
{
        UIScene_InGameInfoMenu* pClass = (UIScene_InGameInfoMenu*)pParam;

        if (result = #ifdef __PS3__Message_ResultAccept) {
                SQRNetworkManager_PS3::AttemptPSNSignIn(&UIScene_InGameInfoMen#else // __PSVITA__InReturned, pClass);

		SQRNetworkManager_Vita::AttemptPSNSignIn(&UIScene_InGameInfoMen#endifwInvites_SignInReturned, pClass);
        }

        return 0;
}

int UIScene_InGameInfoMenu::ViewInvites_SignInReturned(void *pParam,bool bContinue// Check if we're signed in to LIVE	{
		
		if(P#ifdef __ORBIS__SignedInLive(iPad))
		{
        SQ #elif defined(__PS3__) : RecvInviteGUI();

                        int ret = sceNpBasicRecvMessageCustom(SCE_NP_BASIC_MESSAGE_MAIN_TYPE_INVITE, SCE_NP_BASIC_RECV_MESSAGE_OPTIONS_INCLUDE_BOOTABLE, SYS_MEMORY_CO"sceNpBasicRecvMessageCustom return %d ( %08x )\n"#else // __PSVITA__, ret, ret);

			S#endiforkManager_Vita::Recv#endifGUI();

		}
}
return 0;
}
