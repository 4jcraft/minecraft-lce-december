#include "../Platform/stdafx.h"
#include "PlayerList.h"
#include "PlayerChunkMap.h"
#include "../MinecraftServer.h"
#include "../GameState/Settings.h"
#include "../Level/ServerLevel.h"
#include "ServerChunkCache.h"
#include "../Player/ServerPlayer.h"
#include "../Player/ServerPlayerGameMode.h"
#include "ServerConnection.h"
#include "PendingConnection.h"
#include "PlayerConnection.h"
#include "../Player/EntityTracker.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.storage.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.dimension.h"
#include "../../Minecraft.World/Util/ArrayWithLength.h"
#include "../../Minecraft.World/Headers/net.minecraft.network.packet.h"
#include "../../Minecraft.World/Headers/net.minecraft.network.h"
#include "../../Minecraft.World/Util/Pos.h"
#include "../../Minecraft.World/Util/ProgressListener.h"
#include "../../Minecraft.World/WorldGen/Sources/HellRandomLevelSource.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.phys.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.item.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.storage.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.saveddata.h"
#include "../../Minecraft.World/Util/JavaMath.h"
#include "../../Minecraft.World/Level/Storage/EntityIO.h"
#ifdef _XBOX
#include "Xbox/Network/NetworkPlayerXbox.h"
#elif defined(__PS3__) || defined(__ORBIS__)
#include "Common/Network/Sony/NetworkPlayerSony.h"
#endif

// 4J - this class is fairly substantially altered as there didn't seem any
// point in porting code for banning, whitelisting, ops etc.

PlayerList::PlayerList(MinecraftServer* server) {
    playerIo = NULL;

    this->server = server;

    sendAllPlayerInfoIn = 0;
    overrideGameMode = NULL;
    allowCheatsForAllPlayers = false;

#ifdef __PSVITA__
    viewDistance = 3;
#elif defined _LARGE_WORLDS
    viewDistance = 16;
#else
    viewDistance = 10;
#endif

    // int viewDistance = server->settings->getInt(L"view-distance", 10);

    maxPlayers = server->settings->getInt(L"max-players", 20);
    doWhiteList = false;

    InitializeCriticalSection(&m_kickPlayersCS);
    InitializeCriticalSection(&m_closePlayersCS);
}

PlayerList::~PlayerList() {
    for (AUTO_VAR(it, players.begin()); it < players.end(); it++) {
        (*it)->connection = nullptr;  // Must remove reference to connection, or
                                      // else there is a circular dependency
        delete (*it)->gameMode;  // Gamemode also needs deleted as it references
                                 // back to this player
        (*it)->gameMode = NULL;
    }

    DeleteCriticalSection(&m_kickPlayersCS);
    DeleteCriticalSection(&m_closePlayersCS);
}

void PlayerList::placeNewPlayer(Connection* connection,
                                std::shared_ptr<ServerPlayer> player,
                                std::shared_ptr<LoginPacket> packet) {
    CompoundTag* playerTag = load(player);

    bool newPlayer = playerTag == NULL;

    player->setLevel(server->getLevel(player->dimension));
    player->gameMode->setLevel((ServerLevel*)player->level);

    // Make sure these privileges are always turned off for the host player
    INetworkPlayer* networkPlayer = connection->getSocket()->getPlayer();
    if (networkPlayer != NULL && networkPlayer->IsHost()) {
        player->enableAllPlayerPrivileges(true);
        player->setPlayerGamePrivilege(Player::ePlayerGamePrivilege_HOST, 1);
    }

#if defined(__PS3__) || defined(__ORBIS__)
    // PS3 networking library doesn't automatically assign PlayerUIDs to the
    // network players for anything remote, so need to tell it what to set from
    // the data in this packet now
    if (!g_NetworkManager.IsLocalGame()) {
        if (networkPlayer != NULL) {
            ((NetworkPlayerSony*)networkPlayer)->SetUID(packet->m_onlineXuid);
        }
    }
#endif

    // 4J Stu - TU-1 hotfix
    // Fix for #13150 - When a player loads/joins a game after saving/leaving in
    // the nether, sometimes they are spawned on top of the nether and cannot
    // mine down
    validatePlayerSpawnPosition(player);

    //        logger.info(getName() + " logged in with entity id " +
    //        playerEntity.entityId + " at (" + playerEntity.x + ", " +
    //        playerEntity.y + ", " + playerEntity.z + ")");

    ServerLevel* level = server->getLevel(player->dimension);

    DWORD playerIndex = 0;
    {
        bool usedIndexes[MINECRAFT_NET_MAX_PLAYERS];
        ZeroMemory(&usedIndexes, MINECRAFT_NET_MAX_PLAYERS * sizeof(bool));
        for (AUTO_VAR(it, players.begin()); it < players.end(); ++it) {
            usedIndexes[(int)(*it)->getPlayerIndex()] = true;
        }
        for (unsigned int i = 0; i < MINECRAFT_NET_MAX_PLAYERS; ++i) {
            if (!usedIndexes[i]) {
                playerIndex = i;
                break;
            }
        }
    }
    player->setPlayerIndex(playerIndex);
    player->setCustomSkin(packet->m_playerSkinId);
    player->setCustomCape(packet->m_playerCapeId);

    // 4J-JEV: Moved this here so we can send player-model texture and geometry
    // data.
        std::shared_ptr<PlayerConnection> playerConnection = std::shared_ptr<PlayerConnection>(new PlayerConnection(server, connection, p//player->connection = playerConnection;	// Used to be assigned in PlayerConnection ctor but moved out so we can use shared_ptr

	if(newPlayer)
	{
        int mapS #ifdef _LARGE_WORLDS
		int scale = MapItemSavedData::MAP_SIZE * 2 * (1 << mapScale);
        int centreXC = (int)(Math::round(player->x / scale) * scale);
        int centreZC = (int)(Math::round(
            player->z /
            scale) #elsele)  // 4J-PB - for Xbox maps, we'll centre them on the
                             // origin of the world, since we can fit the whole
                             // world in our map
            int centreXC = 0;
        int cent #endif 0;  // 4J Added - Give every player a map the first time
                            // they join a server
        player->inventory->setItem(
            9, std::shared_ptr<ItemInstance>(new ItemInstance(
                   Item::map_Id, 1,
                   level->getAuxValueForMap(player->getXuid(), 0, centreXC,
                                            centreZC, mapScale))));
        if (app.getGameRuleDefinitions() != NULL) {
            app.getGameRuleDefinitions()->postProcessPlayer(player);
        }
	}

	if(!player->customTextureUrl.empty() && player->customTextureUrl.substr"def".compare(L) != 0 && !app.IsFileInMemoryTextures(player->customTextureUrl))
	{
                if(	server->getConnection()->addPendingTextureRequest(player->customTe#ifndef _CONTENT_PACKAGE"Sending texture packet to get custom skin %ls from player %ls\n",player->customTextureUrl.c_str(), player->#endif_str());

			playerConnection->send(std::shared_ptr<TextureAndGeometryPacket>( new TextureAndGeometryPacket(player->customTextureUrl,NULL,0) ) );
		}
}
        else if(!player->customTextureUrl.empty() && app.IsFileInMemoryTextures(player->customT// Update the ref count on the memory texture data
		app.AddMemoryTextureFile(player->customTextureUrl,NULL,0);
        }

        if (!player->customTextureUrl2.empty() &&
            player->customTextureUrl2.s "def"(0, 3).compare(L) != 0 &&
            !app.IsFileInMemoryTextures(player->customTextureUrl2)) {
                if(	server->getConnection()->addPendingTextureRequest(player->cust#ifndef _CONTENT_PACKAGE"Sending texture packet to get custom skin %ls from player %ls\n",player->customTextureUrl2.c_str(), pla#endifame.c_str());

			playerConnection->send(std::shared_ptr<TexturePacket>( new TexturePacket(player->customTextureUrl2,NULL,0) ) );
        }
        }
        else if(!player->customTextureUrl2.empty() && app.IsFileInMemoryTextures(player->cus// Update the ref count on the memory texture data
		app.AddMemoryTextureFile(player->customTextureUrl2,NULL,0);
        }

        player->setIsGuest(packet->m_isGuest);

        Pos* spawnPos = level->getSharedSpawnPos();

        updatePlayerGameMode(pl// Update the privileges with the correct game mode
	GameType *gameType = Player::getPlayerGamePrivilege(player->getAllPlayerGamePrivileges(),Player::ePlayerGamePrivilege_CreativeMode) ? GameType::CREATIVE : GameType::SURVIVAL;
	gameType = LevelSettings::validateGameType(gameType->getId());
	if (player->gameMode->getGameModeForPlayer() != gameType)
	{
                player->setPlayerGamePrivilege(Player::ePlayerGamePrivilege_CreativeMode,player->gameMode->getGameModeForP//shared_ptr<PlayerConnection> playerConnection = shared_ptr<PlayerConnection>(new PlayerConnection(server, connection, player));
	player->connec// Used to be assigned in PlayerConnection ctor but moved out so we can use shared_ptr// 4J Added to store UGC settings
	playerConnection->m_friendsOnlyUGC = packet->m_friendsOnlyUGC;
	playerConnection->m_offlineXUID = packet->m_offlineXuid;
	playerConnection->m_onlineXUID =// This player is now added to the list, so incrementing this value invalidates all previous PreLogin packets
	if(packet->m_friendsOnlyUGC) ++server->m_ugcPlayersVersion;

	addPlayerToReceiving( player );

	playerConnection->send( std::shared_ptr<L""inPacket>( new LoginPacket(L, player->entityId, level->getLevelData()->getGenerator(), level->getSeed(), player->gameMode->getGameModeForPlayer()->getId(),
		(uint8_t) level->dimension->id, (uint8_t) level->getMaxBuildHeight(), (uint8_t) getMaxPlayers(),
		level->difficulty, TelemetryManager->GetMultiplayerInstanceID(), (BYTE)playerIndex, level->useNewSeaLevel(), player->getAllPlayerGamePrivileges(),
		level->getLevelData()->getXZSize(), level->getLevelData()->getHellScale() ) ) );
	playerConnection->send( std::shared_ptr<SetSpawnPositionPacket>( new SetSpawnPositionPacket(spawnPos->x, spawnPos->y, spawnPos->z) ) );
	playerConnection->send( std::shared_ptr<PlayerAbilitiesPacket>( new PlayerAbilitiesPacket(&player->abilities)) );
	playerConnection->send( std::shared_ptr<SetCarriedItemPacket>( new SetCarriedItemPacket(player->inventory->selected)));
	delete spawnPos;

	updateEntireScoreboard((ServerScoreboard *) level->getScoreboard()// 4J-PB - removed, since it needs to be localised in the language the client is in//server->players->broadcastAll( shared_ptr<ChatPacket>( new ChatPacket(L"�e" + playerEntity->name + L" joined the game.") ) );
	broadcastAll( std::shared_ptr<ChatPacket>( new ChatPacket(player->name, ChatPacket::e_ChatPlayerJoinedGame) ) );

	MemSect(14);
	add(player);
// 4J - added - force sending of the nearest chunk before the player is teleported, so we have somewhere to arrive on...
	playerConnection->teleport(player->x, player->y, player->z, player->yRot, player->xRot);

	server->getConnection()->addPlayerConnection(playerConnection);
	playerConnection->send( std::shared_ptr<SetTimePacket>( new SetTimePacket(level->getGameTime(), level->getDayTime(), level->getGameRules()->getBoolean(GameRules::RULE_DAYLIGHT)) ) );

	AUTO_VAR(activeEffects, player->getActiveEffects());
	for(AUTO_VAR(it, activeEffects->begin()); it != activeEffects->end(); ++it)
	{
        MobEffectInstance* effect = *it;
        playerConnection->send(std::shared_ptr<UpdateMobEffectPacket>(
            new UpdateMobEffectPacket(player->entityId, effect)));
	}

	player->initMenu();

	if (playerT// this player has been saved with a mount tagNG_TAG))
	{
        
		std::shared_ptr<Entity>
            mount = EntityIO::loadStatic(
                playerTag->getCompound(Entity::RIDING_TAG), level);
        if (mount != NULL) {
            mount->forcedLoading = true;
            level->addEntity(mount);
            // If we are joining at the same time as someone in the end on this
            // system is travelling through the win portal,// then we should
            // set our wonGame flag to true so that respawning works when the
            // EndPoem is
            // closed
            INetworkPlayer* thisPlayer = player->connection->getNetworkPlayer();
            if (thisPlayer != NULL) {
                for (AUTO_VAR(it, players.begin()); it != players.end(); ++it) {
                    std::shared_ptr<ServerPlayer> servPlayer = *it;
                    INetworkPlayer* checkPlayer =
                        servPlayer->connection->getNetworkPlayer();
                    if (thisPlayer != checkPlayer && checkPlayer != NULL &&
                        thisPlayer->IsSameSystem(checkPlayer) &&
                        servPlayer->wonGame) {
                        player->wonGame = true;
                        break;
                    }
                }
            }
        }

void PlayerList::updateEntireScoreboard(S//unordered_set<Objective *> objectives;har//for (PlayerTeam team : scoreboard->getPlayerTeams())//{//	player->connection->send( shared_ptr<SetPlayerTeamPacket>(new SetPlayerTeamPacket(team, SetPlayerTeamPacket::METHOD_ADD)));//}//for (int slot = 0; slot < Scoreboard::DISPLAY_SLOTS; slot++)//{
	//	Objective objective = scoreboard->getDisplayObjective(slot);//	if (objective != NULL && !objectives->contains(objective))//	{//		vector<shared_ptr<Packet> > *packets = scoreboard->getStartTrackingPackets(objective);//		for (Packet packet : packets)//		{//			player->connection->send(packet);//		}//		objectives->add(objective);//	}//}
	

	
	
	
}

void PlayerList::setLevel(ServerLevelArray levels)
{
        playerIo = levels[0]->getLevelStorage()->getPlayerIO();
}

void PlayerList::changeDimensiostd::n(std::shared_ptr<ServerPlayer> player, ServerLevel *from)
{
        ServerLevel* to = player->getLevel();

        if (from != NULL) from->getChunkMap()->remove(player);
        to->getChunkMap()->add(player);

        to->cache->create(((int)player->x) >> 4, ((int)player->z) >> 4);
}

int PlayerList::getMaxRange()
{
        return PlayerChunkMap::convertChunkRangeToBlock(getViewDistance());
}

CompoundTag *PlayerList::loastd::d(std::shared_ptr<ServerPlayer> player)
{
        return playerIo->load(player);
}

void // 4J Stu - TU-1 hotifxt// Add this function to take some of the code from the PlayerList::add function with the fixes// for checking spawn area, especially in the nether. These needed to be done in a different order from before// Fix for #13150 - When a player loads/joins a game after saving/leaving in the nether, sometimes they are spawned on top of the nether and cannot mine down// 4J Stu - Some adjustments to make sure the current players position is correctr>// Make sure that the player is on the ground, and in the centre x/z of the current column
	"Original pos is %f, %f, %f in dimension %d\n"
	app.DebugPrintf(, player->x, player->y, player->z, player->dimension);

	bool spawnForced = player->isRespawnForced();

	double targetX = 0;
	if(player->x < 0) targetX = Mth::ceil(player->x) - 0.5;
	else targetX = Mth::floor(player->x) + 0.5;

	double targetY = floor(player->y);

	double targetZ = 0;
	if(player->z < 0) targetZ = Mth::ceil(player->z) - 0.5;
	else targetZ"New pos is %f, %f, %f in dimension %d\n">setPos(targetX, targetY, targetZ);

	app.DebugPrintf(, player->x, player->y, player->z, player->dimension);

	ServerLevel *level = server->getLevel(player->dimension);
	while (level->getCubes(player, player->b"Final pos is %f, %f, %f in dimension %d\n"->x, player->y + 1, player->z);
	}
	app.DebugPrintf(// 4J Stu - If we are in the nether and the above while loop has put us above the nether then we have a problem// Finding a valid, safe spawn point is potentially computationally expensive (may have to hunt through a large part// of the nether) so move the player to their spawn position in the overworld so that they do not lose their inventory// 4J Stu - We also use this mechanism to force a spawn point in the overworld for players who were in the save when the reset nether option was applied"Player in the nether tried to spawn at y = %f, moving to overworld\n"25)
	{
    app.DebugPrintf(,
                    player->y);
    player->setLevel(server->getLevel(0));
    player->gameMode->setLevel(server->getLevel(0));
    player->dimension = 0;

    level = server->getLevel(player->dimension);

    Pos* levelSpawn = level->getSharedSpawnPos();
    player->setPos(levelSpawn->x, levelSpawn->y, levelSpawn->z);
    delete levelSpawn;

    Pos* bedPosition = player->getRespawnPosition();
    if (bedPosition != NULL) {
        Pos* respawnPosition = Player::checkBedValidRespawnPosition(
            server->getLevel(player->dimension), bedPosition, spawnForced);
        if (respawnPosition != NULL) {
            player->moveTo(respawnPosition->x + 0.5f, respawnPosition->y + 0.1f,
                           respawnPosition->z + 0.5f, 0, 0);
            player->setRespawnPosition(bedPosition, spawnForced);
        }
        delete bedPosition;
    }
    while (level->getCubes(player, player->bb)
                   ->s "Updated pos is %f, %f, %f in dimension %d\n" player->y +
               1,
           player->z);
		}

		app.DebugPrintf(, player->x, player->y, play//broadcastAll(shared_ptr<PlayerInfoPacket>( new PlayerInfoPacket(player->name, true, 1000) ) );
{
    
	if (player->connection->getNetworkPlayer()) {
                broadcastAll(// 4J Added_ptr<PlayerInfoPacket>( new Player// Ensure the area the player is spawning in is loaded!);

	
	addPlayerToReceiving(player);

	// 4J Stu - TU-1 hotfix// Fix for #13150 - When a player loads/joins a game after saving/leaving in the nether, sometimes they are spawned on top of the nether and cannot mine down// Some code from here has been moved to the above validatePlayerSpawnPosition function// 4J Stu - Swapped these lines about so that we get the chunk visiblity packet way ahead of all the add tracked entity packets// Fix for #9169 - ART : Sign text is replaced with the words �Awaiting approval�.
	
	changeDimension(player, NULL);
	level->addEntity//player->connection->send(shared_ptr<PlayerInfoPacket>( new PlayerInfoPacket(op->name, true, op->latency) ) );
		if( op->connection->getNetworkPlayer() )
		{
            player->connection->send(
                std::shared_ptr<PlayerInfoPacket>(new PlayerInfoPacket(op)));
		}
    }

    if (level->isAtLeastOnePlayerSleeping()) {
        std::shared_ptr<ServerPlayer> firstSleepingPlayer = nullptr;
        for (unsigned int i = 0; i < players.size(); i++) {
            std::shared_ptr<ServerPlayer> thisPlayer = players[i];
            if (thisPlayer->isSleeping()) {
                if (firstSleepingPlayer == NULL)
                    firstSleepingPlayer = thisPlayer;
                thisPlayer->connection->send(
                    std::shared_ptr<ChatPacket>(new ChatPacket(
                        thisPlayer->name, ChatPacket::e_ChatBedMeSleep)));
            }
        }
        player->connection->send(std::shared_ptr<ChatPacket>(new ChatPacket(
            firstSleepingPlayer->name, ChatPacket::e_ChatBedPlayerSleep)));
    }
}

void PlayerList:std:::move(std::shared_ptr<ServerPlayer> pla//4J Stu - We don't want to save the map data for guests, so when we are sure that the player is gone delete the mapr> player)
{
    save(player);
    // remove mount first because the player unmounts when beingpla// removed, also remove mount because it's saved in the player'sing// save tag{
		
		"removing player mount"
		
		level->removeEntityImmediately(player->riding);
    app.DebugPrintf();
	}
	level->removeEntity(player);
	level->//broadcastAll(shared_ptr<PlayerInfoPacket>( new PlayerInfoPacket(player->name, false, 9999) ) );	( it != players.end() )
	{
    players.erase(it);
	}
	// Must remove reference to connection, or else there is a circular dependency

	removePlayerFromReceivi// Gamemode also needs deleted as it references back to this player// 4J Stu - Save all the players currently in the game, which will also free up unused map id slots if required, and remove old playersMode = NULL;

	
	saveAll(NULL,falsestd::);
        }

        std::shared_ptr<ServerPlayer> PlayerList::getPlayerForLogin(
            PendingConnection* pendingConnection,
            std::const std::wstring& userName, PlayerUID xuid,
            PlayerUID onlineXuid) {
            if (players.size() >= maxPlayers) {
                pendingConnection->disconnect(
                    DisconnectPacket::eDisconnect_ServerFull);
                return std::shared_ptr<ServerPlayer>();
            }

            std::shared_ptr<ServerPlayer>
                player    // 4J added as had to remove this assignment from
                          // ServerPlayer ctoretLevel(0), userName, new S// 4J
                          // AddedGameMode(server->getLevel(0)) ));
                    play  // 4J Addede->// Work out the base server
                          // player
                          // settings
                        player->setXuid(xuid);
            
	player->setOnlineXuid(onlineXuid);
            

	
	INetworkPlayer* networkPlayer =
                pendingConnection->connection->getSocke  // 4J Addedyer();
                if (networkPlayer != NULL && !networkPlayer->IsHost()) {
                player->enableAllPlayerPrivileges(
                    app.GetGameHostOption(eGameHostOption_TrustPlayers) > 0);
            }

            
	LevelRuleset* serverRuleDefs = app.getGameRuleDefinitions();
            if (serverRuleDefs != NULL) {
                player->gameMode->setGameRules( GameRuleDefinition::generateNewGameRulesInstance(GameRulesInstance::eGameRulesInstanceType_ServerPlayer, serverRuleDefs, pendingConnection->conne// How we handle the entity tracker depends on whether we are the primary player currently, and whether there will be any player in the same system in the same dimension once we finish respawning. // Is this the primary player in its current dimension?
	bool isPrimary = canRecei// We're not emptying this dimension on this machine if this player is going back into the same dimensionerv// Also consider if there is another player on this machine which is in the same dimension and can take over as primary player

	
	if( isEmptying )
	{
            INetworkPlayer* thisPlayer =
                serverPlayer->connection->getNetworkPlayer();

            for (unsigned int i = 0; i < players.size(); i++) {
                std::shared_ptr<ServerPlayer> ep = players[i];
                if (ep == serverPlayer) continue;
                        if( ep-// There's another player here in the same dimension - we're not the last one outnnection->getNetworkPlayer();
			if( ot// Now we know where we stand, the actions to take are as follows:		// (1) if this isn't the primary player, then we just need to remove it from the entity trackerin// (2) if this Is the primary player then://		(a) if isEmptying is true, then remove the player from the tracker, and send "remove entity" packets for anything seen (this is the original behaviour of the code)//		(b) if isEmptying is false, then we'll be transferring control of entity tracking to another player"Emptying this dimension\n"

"Transferring... storing flags\n"ing )
		{
                    app.DebugPrintf();
                        serverPlayer->getLevel()->getTracker()->clear("Not primary player\n"lse
		{
                        app.DebugPrintf();
                        serverPlayer->getLevel()->getTracker()->removeEntity(
                            serverPlayer);
		}
	}
	else
	{
                    app.DebugPrintf();
                    serverPlayer->getLevel()->getTracker()->removeEntity(
                        serverPlayer);
	}

	serverPlayer->getLevel()->getChunkMap()->remove(serverPlayer);
	AUTO_VAR(it, std::find(players.begin(),players.end(),serverPlayer));
	if( it != players.end() )
	{
                    players.erase(it);
	}
	server->getLevel(serverPlayer->dimension)->removeEntityImmediately(serverPlayer);

	Pos *bedPosition = serverPlayer->getRespawnPosition();
	bool spawnForced = serverPlayer->isRespawnForced();

	removePlayerFromReceiving(serverPlayer);
	serverPlayer->dimension = targetDimension;

	EDefaultSkins skin = serverPlayer->getPlayerDefaultSkin();
	DWORD playerIndex = serverPlayer->getPlayerIndex();

	PlayerUID playerXuid = serverPlayer->getXuid();
	PlayerUID playerOnlineXuid = serverPlayer->getOnlineXuid();

	std::shared_ptr<ServerPlayer> player = std::shared_ptr<ServerPlayer>(new ServerPlayer(server, server->getLevel(serverPlayer->dimension), serverPlayer->getName()// Fix for #81759 - TU9: Content: Gameplay: Entering The End Exit Portal replaces the Player's currently held item with the first one from the Quickbaryer, keepAllPlayerData);
	if (keepAllPlayerData)
	{
                        // 4J added as had to remove this assignment from ServerPlayer ctor
                        // 4J Addedventory->selected =
                        // serverPlayer->inventory->// 4J Added} 4J Stu - Don't
                        // reuse the id. If we do, then the player can be
                        // re-added after being removed, but the add packet gets
                        // sent before the remove packetpl//player->entityId =
                        // serverPlayer->entityId;

	
	

	player->setPlayerDefaultSkin(skin);
                    player->setIsGuest(serverPlayer->isGuest());
                    player->setPlayerIndex(playerIndex);
                    player->setCustomSkin(serverPlayer->getCustomSkin());
                    player->setCustomCape(serverPlayer->getCustomCape());
                    player->setPlayerGam  // 4J Stu - Added this as we need to
                                          // know earlier if the player is the
                                          // player for this connection so
                                          // thatde// we can work out if they
                                          // are the primary for the system and
                                          // can receive all packetsDimension;

	
	    // reset the player's game mode (first pick from old, then copy level ifco// necessary)Player( player );

                        addPlayerToReceiving(player);

                    ServerLevel* level =
                        server->getLevel(serverPlayer->dimension);

                        // If the player is still alive and respawning to the same dimension, they are just being added back from someone else viewing the Win screenimension == oldDimension && serverPlayer->getHealth() > 0)
                    {
                
		player->moveTo(ser// Fix for #81759 - TU9: Content: Gameplay: Entering The End Exit Portal replaces the Player's currently held item with the first one from the QuickbardPosition, spawnForced);
			delete bedPosition;
                    }
                    
		player->inventory->selected = serverPlayer->inventory->selected;
	}
	else if (bedPosition != NULL)
	{
                    Pos* respawnPosition = Player::checkBedValidRespawnPosition(
                        server->getLevel(serverPlayer->dimension), bedPosition,
                        spawnForced);
                    if (respawnPosition != NULL) {
                        player->moveTo(respawnPosition->x + 0.5f,
                                       respawnPosition->y + 0.1f,
                                       respawnPosition->z + 0.5f, 0, 0);
                        player
                            ->setRespawnPositi  // Ensure the area the player
                                                // is spawning in is
                                                // loaded!>connection->send(
                                                // std::shared_ptr<GameEventPacket>(
                                                // new
                                                // GameEventPacket(GameEventPacket::NO_RESPAWN_BED_AVAILABLE,
                                                // 0) ) );
                    }
                    delete bedPosition;
	}

	
	level->cache->create(((int) player->x) >> 4, ((int) player->z) >> 4);

	while (!level->getCubes(player, player->bb)->empty())
	{
                    player->setPos(player->x, player->y + 1, player->z);
	}

	player->connection->send( std::shared_ptr<RespawnPacket>( new RespawnPacket((char) player->dimension, player->level->getSeed(), player->level->getMaxBuildHeight(),
		player->gameMode->getGameModeForPlayer(), level->difficulty, level->getLevelData()->getGenerator(),
		player->level->useNewSeaLevel(), player->entityId, level->getLevelData()->getXZSize(), level->getLevelData()->getHellScale()) ) );
	player->connection->teleport(player->x, player->y, player->z, player->yRot, player->xRot);
	player->connection->send( std::shared_ptr<SetExperiencePacket>( new SetExperiencePacket(player->experienceProgress, player->totalExperience, player->experienceLevel)) );

	if(keepAllPlayerData)
	{
                    std::vector<MobEffectInstance*>* activeEffects =
                        player->getActiveEffects();
                    for (AUTO_VAR(it, activeEffects->begin());
                         it != activeEffects->end(); ++it) {
                        MobEffectInstance* effect = *it;

                        player->connection->send(
                            std::shared_ptr<UpdateMobEffectPacket>(
                                new UpdateMobEffectPacket(player->entityId,
                                                          effect)));
                    }
                    delete activeEffects;
                    player->getEntityData()
                        ->  // 4J-JEV - Dying before this point in the tutorial
                            // is pretty annoying,;
                        // making sure to remove health/hunger and give you back
                        // your meat.ayers.push_back(player);

                        player->initMenu();
                    player->setHealth(player->getHealth());

                    
	
	if (Minecraft::GetInstance()->isTutorial() &&
            (!Minecraft::GetInstance()
                  ->gameMode->getTutorial()
                  ->isStateCompleted(e_Tutorial_State_Food_Bar))) {
                        app.getGameRuleDefinitions()->postProcessPlayer(player);
                    }

                    if (
                        oldDimension ==
                            1 &&player
                                ->di  // How we handle the entity tracker
                                      // depends on whether we are the primary
                                      // player currently, and whether there
                                      // will be any player in the same system
                                      // in the same dimension once we finish
                                      // respawning. t lastDimension =
                                      // player->dimension;
	    // Is this the primary player in its current dimension?// Also consider if there is another player on this machine which is in the same dimension and can take over as primary playereceiveAllPackets(player);	
                            bool isEmptying = true;

	
	INetworkPlayer *thisPlayer = player->connection->getNetworkPlayer();

                        for (unsigned int i = 0; i < players.size(); i++) {
                            std::shared_ptr<ServerPlayer> ep =// There's another player here in the same dimension - we're not the last one out ) continue;

		INetworkPlayer * o// Now we know where we stand, the actions to take are as follows: !// (1) if this isn't the primary player, then we just need to remove it from the entity tracker// (2) if this Is the primary player then:		//		(a) if isEmptying is true, then remove the player from the tracker, and send "remove entity" packets for anything seen (this is the original behaviour of the code)//		(b) if isEmptying is false, then we'll be transferring control of entity tracking to another player"Toggle... Emptying this dimension\n"
	"Toggle...  transferring\n"

	if( isPrimary )
	{
                                if (isEmptying) {
                        app.DebugPrintf("Toggle...  Not primary player\n"	player->getLevel()->getTracker()->clear(player);
                                } else {
                                    app.DebugPrintf();
                                    player
                                        ->getL  // 4J Stu - Do this much earlier
                                                // so we don't end up unloading
                                                // chunks in the wrong
                                                // dimension);
                                            player->getLevel()
                                        ->getTracker()
                                        ->removeEntity(player);
                                }

                                ServerLevel* oldLevel =
                                    server->getLevel(player->dimension);

                                
	player->getLevel()
                                    ->getChunkMap()
                                    ->remove(player);

                                if (player->dimension != 1 &&
                                    targetDimension == 1) {
                                    player
                                        ->display  // 4J Stu - Fix for #46423 -
                                                   // TU5: Art: Code: No burning
                                                   // animation visible after
                                                   // entering The Nether while
                                                   // burningS_PLAYER_LEFT_END);
                                }
                                // Stop burning if travelling through a
                                // portalrLe// 4J Stu Added so that we remove
                                // entities from the correct level, after the
                                // respawn packet we will be in the wrong
                                // level
                                player->clearFire();
                                

	
	player->flushEntitiesToRemove();

        player->connection->send( std::shared_ptr<RespawnPacket>( new RespawnPacket((char) player->dimension, newLevel->getSeed(), newLevel->getMaxBuildHeight(),
		player->gameMode->getGameModeForPlayer(), newLevel->difficulty, newLevel->getLevelData()->getGenerator(),
		newLevel->useNewSeaLevel(), player->entityId, newLevel->getLevelData()->getXZSize(), newLevel->getLevelData(// Resend the teleport if we haven't yet sent the chunk they will land onyer->removed = false;

	repositionAcrossDimension(player, lastDimension, oldLevel, newLevel);
	changeDimension(player, oldLevel);

	player->gameMode->setLevel(newLevel);

	
	if( !g_NetworkManager.SystemFlagGet(player->con// Force sending of the current chunkyer::getFlagIndexForChunk( ChunkPos(player->xChunk,player->zChunk), player->level->dimension->id ) ) )
	{
		player->connection->telepor// 4J Stu - Fix for #64683 - Customer Encountered: TU7: Content: Gameplay: Potion effects are removed after using the Nether Portale, true, true);
	}

	player->connection->teleport(player->x, player->y, player->z, player->yRot, player->xRot);

	
	std::vector<MobEffectInstance *> *activeEffects = player->getActiveEffects();
	for(AUTO_VAR(it, activeEffects->begin()); it != activeEffects->end(); ++it)
	{
                                MobEffectInstance* effect = *it;

                                player->connection->send(
                                    std::shared_ptr<UpdateMobEffectPacket>(
                                        new UpdateMobEffectPacket(
                                            player->entityId, effect)));
	}
	delete activeEffects;
	player->getEntityData()->markDirty(Mob::DATA_EFFECT_COLOR_ID);

	sendLevelInfo(player, newLevel);
	sendAllPlayerInfo(player);
                            }

void PlayerList::repositionAcrossDimestd::nsion(std::shared_ptr<Entity> entity, int lastDimension, ServerLevel *oldLevel, // 4J Scale was 8 but this is all we can fit in;
	double zt = entity->z;
	double xOriginal = entity->x;
	double yOriginal = entity->y;
	double zOriginal = entity->z;
	float yRotOriginal = entity->yRot;
	double scale = newLevel->getLevelData()->getHellScale(); 
	if (entity->dimension == -1)
	{
                            xt /= scale;
                            zt /= scale;
                            entity->moveTo(xt, entity->y, zt, entity->yRot,
                                           entity->xRot);
                            if (entity->isAlive()) {
                                oldLevel->tick(entity, false);
                                // Coming from the end>dimension == 0)
                                {
                                    xt *= scale;
                                    zt *= scale;
                                    entit  // Going to the endy->y, zt,
                                           // entity->yRot, entity->xRot);
                                        if (entity->isAlive()) {
                                        oldLevel->tick(entity, false);
                                    }
                                }
                                else {
                                    Pos* p;

                                    if (lastDimension == 1) {
                                        
			p = newLevel->getSharedSpawnPos();
                                    } else {
                                        
			p = newLevel->getDimensionSpecificSpawn();
                                    }

                                    xt = p->x;
                                    entity->y = p->y;
                                    zt = p->z;
                                    delete p;
                                    entity->moveTo(xt, entity->y, zt, 90, 0);
                                    if (entity->isAlive()) {
                                        oldLevel->tick(entity, false);
                                    }
                                }

                                if (entity->GetType() == eTYPE_SERVERPLAYER) {
                                    std::shared_ptr<ServerPlayer> player =
                                        dynamic_pointer_cast<ServerPlayer>(
                                            entity);
                                    removePlayerFromReceiving(player, false,
                                                              lastDimension);
                                    addPlayerToReceiving(player);
                                }

                                if (lastDimension != 1) {
                                    xt = (double)Mth::clamp(
                                        (int)xt, -Level::MAX_LEVEL_SIZE + 128,
                                        Level::MAX_LEVEL_SIZE - 128);
                                    zt = (double)Mth::clamp(
                                        (int)zt, -Level::MAX_LEVEL_SIZE + 128,
                                        Level::MAX_LEVEL_SIZE - 128);
                                    if (entity->isAlive()) {
                                        newLevel->addEntity(entity);
                                        entity->moveTo(xt, entity->y, zt,
                                                       entity->yRot,
                                                       entity->xRot);
                                        newLev  // 4J - brought changes to how
                                                // often this is sent forward
                                                // from 1.2.3ewLevel->getPortalForcer()->force(entity,
                                                // xOriginal, yOriginal,
                                                // zOriginal, yRotOriginal);
                                            newLevel->cache->autoCreate = false;
                                    }
                                }

                                entity->setLevel(newLevel);
                            }

                            void PlayerList::tick() {
                                    //broadcastAll(shared_ptr<PlayerInfoPacket>( new PlayerInfoPacket(op->name, true, op->latency) ) );		YER_INFO_INTERVAL)
                                {
                                    sendAllPlayerInfoIn = 0;
                                }

                                if (sendAllPlayerInfoIn < players.size()) {
                                    std::shared_ptr<ServerPlayer> op =
                                        players[sendAllPlayerInfoIn];
                                    
		if (op->connection->getNetworkPlayer()) {
                                        broadcastAll(
                                            std::shared_ptr<PlayerInfoPacket>(
                                                new PlayerInfoPacket(op)));
                                    }
                                }

        EnterCriticalSection(&m_closePlay// 4J Stu - May be being a bit overprotective with all the NULL checks, but adding late in TU7 so want to be safe_front();

		std::shared_ptr<ServerPlayer> player = nullptr;

		for(unsigned int i = 0; i < players.size(); i++)
		{
                                    std::shared_ptr<ServerPlayer> p =
                                        players.at(i);
                                    
			if (p != NULL && p->connection != NULL &&
                            p->connection->connection != NULL &&
                            p->connection->connection->getSocket() != NULL &&
                            p->connection->connection->getSocket()
                                    ->getSmallId() == smallId) {
                                        player = p;
                                        break;
                                    }
		}

		if (player != NULL)
		{
                                    player->connection->disconnect(
                                        DisconnectPacket::eDisconnect_Closed);
		}
                            }
                            LeaveCriticalSection(&m_closePlayersCS);

        EnterCriticalSection(&m_kickPlayers//#ifdef _XBOX_smallIdsToKick.empty())
	{
                                BYTE smallId =
                                    m_small  // Kick this player from the
                                             // gameKick.pop_front();
                                    INetworkPlayer* selectedPlayer =
                                        g_NetworkManager.GetPlayerBySmallId(
                                            smallId);
                                if (selectedPlayer != NULL) {
                                    if (selectedPlayer->IsLocal() != TRUE) {
                                        
				PlayerUID xuid = selectedPlayer->GetUID();
                                        
				std::shared_ptr<ServerPlayer>
                                            player = nullptr;

                                        for (unsigned int i = 0;
                                             i < players.size(); i++) {
                                            std::shared_ptr <
                                                Serv  // 4J Stu - If we have
                                                      // kicked a player, make
                                                      // sure that they have no
                                                      // privileges if they
                                                      // later try to join the
                                                      // world when trust
                                                      // players is off, xuid )
                                                      // )
                                            {
                                                player = p;
                                                break;
                                            }
                                        }

                                        if (player != NULL) {
                                            m_bannedXuids.push_back(
                                                player->getOnlineXuid());
                                                //#endif// Check our receiving players, and if they are dead see if we can replace them->setWasKicked();
                                                player->connection->send(
                                                    std::shared_ptr<
                                                        DisconnectPacket>(
                                                        new DisconnectPacket(
                                                            DisconnectPacket::
                                                                eDisconnect_Kicked)));
                                        }
                                        
			
                                    
                                    }
                                }
	}
	LeaveCriticalSection(&m_kickPlayersCS);

	
	for(unsigned int dim = 0; dim < 2; ++dim)
	{
                for(unsigned int i = 0; i < receiveAllPlayers[dim].si"Replacing primary player %ls with %ls in dimension %d\n"ayer  = receiveAllPlayers[dim][i];
			if(currentPlayer->removed)
			{
                                    std::shared_ptr<ServerPlayer> newPlayer =
                                        findAlivePlayerOnSystem(currentPlayer);
                                    if (newPlayer != NULL) {
                                        receiveAllPlayers[dim][i] = newPlayer;
                                        // 4J added - make sure that any tile
                                        // updates for the chunk at this
                                        // location get prioritised for
                                        // sending(), newPlayer->name.c_str(),
                                        // dim);
                                    }
			}
		}
	}
                        }

                        bool PlayerList::isTrackingTile(int x, int y, int z,
                                                        int dimension) {
                            return server->getLevel(dimension)
                                ->getChunkMap()
                                ->isTrackingTile(x, y, z);
                        }


void PlayerList::prioritiseTileChanges(int x, int y, int z, int dimension) {
                            server->getLevel(dimension)
                                ->getChunkMap()
                                ->prioritiseTileChanges(x, y, z);
                        }

                        void PlayerList::broadcastd::stAll(
                            std::shared_ptr<Packet> packet) {
                            for (unsigned int i = 0; i < players.size(); i++) {
                                std::shared_ptr<ServerPlayer> player =
                                    players[i];
                                player->connection->send(packet);
                            }
                        }

                        void PlayerList::broadcastd::stAll(
                            std::shared_ptr<Packet> packet, int d ", " sion) {
                            for (unsigned int i = 0; i < players.size(); i++) {
                                std::shared_ptr<ServerPlayer> player =
                                    players[i];
                                if (player->dimension == dimension)
                                    player->connection->send(packet);
                                std::
                            }
                        }

                        std::wstring PlayerList::getPlayerNames() {
                            std::wstring msg;
                            for (unsigned int i = 0; i < players.size(); i++) {
                                if (i > 0)
                                    ms #ifdef _DEBUG_MENUS_ENABLEDs[i]->name;
                            }
                            return msg;
                        }

                        bool PlayerList::isWhiteListed #endifconst std::wstring
                            &name) {
                        return true;
                    }

                    bool PlayerList::isOp(std::const std::wstring& name) {
                        return false;
                    }

                    bool PlayerList
                        : std:: : isOp(std::shared_ptr<ServerPlayer> player) {
                        bool cheatsEnabled = app.GetGameHostOption(
                            eGameHostOption_CheatsEnabled);
                        
	cheatsEnabled = cheatsEnabled || app.GetUseDPadForDebug();
                        
	INetworkPlayer* networkPlayer =
                            player->conn  // 4J - used to be case insensitive
                                          // (using equalsIgnoreCase) - imagine
                                          // we'll be shifting to XUIDs anywayLL
                                          // && networkPlayer->IsHost()));
                            return isOs  // 4J std::Addedhared_ptr<ServerPlayer>
                                         // PlayerList::getPlayer(std::const
                                         // std::wstring& name)
                        {
                            for (unsigned int i = 0; i < players.size(); i++) {
                                std::shared_ptr<ServerPlayer> p = players[i];
                                if (p->name == name)
                                        // 4J - used to be case insensitive (using equalsIgnoreCase) - imagine we'll be shifting to XUIDs anyway p;
                            }
                        }
                        return nullptr;
                    }

                    
std::shared_ptr<ServerPlayer>
                    PlayerList::getPlayer(PlayerUID uid) {
                        for (unsigned int i = 0; i < players.size(); i++) {
                            std::shared_ptr<ServerPlayer> p = players[i];
                            if (p->getXuid() == uid ||
                                p->getOnlineXuid() == uid)
                                
		{
                                    return p;
                                }
                        }
                        return nullptstd::r;
                    }

                    std::shared_ptr<ServerPlayer> PlayerList::getNearestPlayer(
                        Pos * position, int range) {
                        if (players.empty()) return nullptr;
                        if (position == NULL) return players.at(0);
                        std::shared_ptr<ServerPlayer> current = nullptr;
                        double dist = -1;
                        int rangeSqr = range * range;

                        for (int i = 0; i < players.size(); i++) {
                            std::shared_ptr<ServerPlayer> next = players.at(i);
                            double newDist = position->distSqr(
                                next->getCommandSenderWorldPosition());

                            if ((dist == -1 ||
                                 newDist < d "getPlayers NOT IMPLEMENTED!" t <=
                                     rangeSqr)) { /*if (players.empty()) return
                                         NULL; vector<shared_ptr<ServerPlayer> >
                                         result = new
                                         vector<shared_ptr<ServerPlayer> >();
                                         bool reverse = count < 0;
                                         bool playerNameNot =
                                         !playerName.empty() &&
                                         playerName.startsWith("!"); bool
                                         teamNameNot = !teamName.empty() &&
                                         teamName.startsWith("!"); int
                                         rangeMinSqr = rangeMin * rangeMin; int
                                         rangeMaxSqr = rangeMax * rangeMax;
                                         count = Mth.abs(count);

                                         if (playerNameNot) playerName =
                                         playerName.substring(1); if
                                         (teamNameNot) teamName =
                                         teamName.substring(1);

                                         for (int i = 0; i < players.size();
                                         i++) { ServerPlayer player =
                                         players.get(i);

                                         if (level != null && player.level !=
                                         level) continue; if (playerName !=
                                         null) { if (playerNameNot ==
                                         playerName.equalsIgnoreCase(player.getAName()))
                                         continue;
                                         }
                                         if (teamName != null) {
                                         Team team = player.getTeam();
                                         String actualName = team == null ? "" :
                                         team.getName(); if (teamNameNot ==
                                         teamName.equalsIgnoreCase(actualName))
                                         continue;
                                         }

                                         if (position != null && (rangeMin > 0
                                         || rangeMax > 0)) { float distance =
                                         position.distSqr(player.getCommandSenderWorldPosition());
                                         if (rangeMin > 0 && distance <
                                         rangeMinSqr) continue; if (rangeMax > 0
                                         && distance > rangeMaxSqr) continue;
                                         }

                                         if (!meetsScoreRequirements(player,
                                         scoreRequirements)) continue;

                                         if (mode != GameType.NOT_SET.getId() &&
                                         mode !=
                                         player.gameMode.getGameModeForPlayer().getId())
                                         continue; if (levelMin > 0 &&
                                         player.experienceLevel < levelMin)
                                         continue; if (player.experienceLevel >
                                         levelMax) continue;

                                         result.add(player);
                                         }

                                         if (position != null)
                                         Collections.sort(result, new
                                         PlayerDistanceComparator(position)); if
                                         (reverse) Collections.reverse(result);
                                         if (count > 0) result =
                                         result.subList(0, Math.min(count,
                                         result.size()));

                                         return result;*/
                                "meetsScoreRequirements NOT IMPLEMENTED!"//if (scoreRequirements == null || scoreRequirements.size() == 0) return true;//for (Map.Entry<String, Integer> requirement : scoreRequirements.entrySet()) {//	String name = requirement.getKey();//	boolean min = false; Pl//	if (name.endsWith("_min") && name.length() > 4) {r<//		min = true; s//		name = name.substring(0, name.length() - 4);em//	})
{//	Scoreboard scoreboard = player.getScoreboard();//	Objective objective = scoreboard.getObjective(name);//	if (objective == null) return false;//	Score score = player.getScoreboard().getPlayerScore(player.getAName(), objective);//	int value = score.getScore();//	if (value < requirement.getValue() && min) {//		return false;//	} else if (value > requirement.getValue() && !min) {//		return false;//	}//}
                                    // return
                                    // true;
                                    
	
	
	

	
	
	
	
	
	

	

                                
                                }

                                void PlayerList::sendMessage(
                                    std::const std::wstring& name,
                                    std::const std::wstring& message) {
        std::shared_ptr<ServerPlayer> player = getPlayer(// 4J - altered so that we don't send to the same machine more than once. Add the source player to the machines we have "sent" to as it doesn't need to go to thatub// machine eitherouble range, int dimenstd::sion, std::shared_ptr<Packet> packet)
{
                                        broadcast(nullptr, x, y, z, range,
                                                  dimension, packet);
}

void PlayerList::broastd::dcast(std::shared_ptr<Player> except, double x, double y, double z, double range, int dimenstd::sion, std::shared_ptr<Packet> packet)
{
                                            // 4J - don't send to the same machine more than once
	
	std::vector<std::shared_ptr<ServerPlayer> >
                                            sentTo;
                                        if (except != NULL) {
                                            sentTo.push_back(
                                                dynamic_pointer_cast<
                                                    ServerPlayer>(except));
                                        }

                                        for (unsigned int i = 0;
                                             i < players.size(); i++) {
                                            std::shared_ptr<ServerPlayer> p =
                                                players[i];
                                            if (p == except) continue;
                                            if (p->dimension != dimension)
                                                continue;

                                            
		bool dontSend = false;
                                            if (sentTo.size()) {
                                                INetworkPlayer* thisPlayer =
                                                    p->connection
                                                        ->getNetworkPlayer();
                                                if (thisPlayer == NULL) {
                                                    dontSend = true;
                                                }
                                                els #if 0  // _DEBUGunsigned int
                                                           // j = 0; j <
                                                           // sentTo.size(); j++
                                                           // )
                                                {
                                                    std::shared_ptr<
                                                        ServerPlayer>
                                                        player2 = sentTo[j];
                                                    INetworkPlayer* otherPlayer
                                                        "---broadcast - "
                                                        "eSoundType_[%d]"
                                                        " " kPlayer();
                                                    if (otherPlayer != NULL &&
                                                        thisPlayer
                                                            ->IsSameSystem(
                                                                otherPlayer)) {
                                                        dontSend = true;
                                                    }
                                                }
                                            }
                                            "\n" if (do #endif) { continue; }

                                            double xd = x - p->x;
                                            double yd = y - p->y;
                                            double zd = z - p->z;
                                            if (xd * xd + yd * yd + zd * zd <
                                                range * range) {
/*= false*/
			std::shared_ptr<LevelSoundPacket> SoundPacket= dynamic_pointer_cast<LevelSoundPacket>(pac// 4J - playerIo can be NULL if we have have to exit a game really early on due to network failure>getSound());
				OutputDebugStringW(ConsoleSoundEngine::wchSoundNames[SoundPacket->getSound()]);
				app.DebugPrintf();
                                            }
                                            
			p -  // 4J Stu - We don't want to save the map data for
                                            // guests, so when we are sure that
                                            // the player is gone delete the
                                            // mapistener, bool bDeleteGuestMaps
                                            // )
                                            {
                                                if (progressListener != NULL)
                                                    progressListener->progressStart(
                                                        IDS_PROGRESS_SAVING_PLAYERS);
                                                
	if (playerIo) {
                                                    playerIo
                                                        ->saveAllCachedData();
                                                    for (unsigned int i = 0;
                                                         i < players.size();
                                                         i++) {
                                                        playerIo->save(
                                                            players[i]);

                                                        
			if (bDeleteGuestMaps && players[i]->isGuest()) playerIo
                                                            ->deleteMapFilesForPlayer(
                                                                players[i]);

                                                        if (progressListener !=
                                                            NULL)
                                                            progressListener
                                                                ->progressStagePercentage(
                                                                    (i * 100) /
                                                                    ((int)players
                                                                         .size()));
                                                    }
                                                    playerIo
                                                        ->clearOldPlayerFiles();
                                                    playerIo->saveMapIdLookup();
                                                }
                                            }

                                            void PlayerList::whiteList(
                                                std::const std::wstring&
                                                    playerName) {}

void PlayerList::blackList(std::con// 4J Stu - Fix for #44836 - Customer Encountered: Out of Sync Weather [A-10]d P// If it was raining when the player left the level, and is now not raining we need to make sure that state is updatedtd::shared_ptr<SetTimePacket>( new SetTimePacket(level->getGameTime(), level->getDayTime(), level->getGameRules()->getBool// send the stronghold position if there is one isRaining())
	{
                                                player->connection->send(
                                                    std::shared_ptr<
                                                        GameEventPacket>(
                                                        new GameEventPacket(
                                                            GameEventPacket::
                                                                START_RAINING,
                                                            0)));
	}
	else
	{
                                                
		
		player->connection->send(std::shared_ptr<GameEventPacket>(
                                                    new GameEventPacket(
                                                        GameEventPacket::
                                                            STOP_RAINING,
                                                        0)));
	}

	
	if((level->dimension->id==0) && level->getLevelData()->getHasStronghold())
	{
                                                player->connection->send(
                                                    std::shared_ptr<
                                                        XZPacket>(new XZPacket(
                                                        XZPacket::STRONGHOLD,
                                                        level->getLevelData()
                                                            ->getXStronghold(),
                                                        level->getLevelData()
                                                            ->getZStronghold())));
	}
                                        }

                                        void PlayerList::sendAllPlayestd::rInfo(
                                            std::shared_ptr<ServerPlayer>
                                                player) {
                                            player->refreshContainer(
                                                player->inventoryMenu);
                                            player->resetSentInfo();
                                            player->connection->send(
                                                std::shared_ptr<
                                                    SetCarriedItemPacket>(
                                                    new SetCarriedItemPacket(
                                                        player->inventory
                                                            ->selected)));
                                        }

                                        int PlayerList::getPlayerCount() {
                                            return (int)players.size();
                                        }

                                        int PlayerList::getPlayerCount(
                                            ServerLevel * level){
                                            int count =
                                                0  // reset the player's game
                                                   // mode (first pick from old,
                                                   // then copy level iff(//
                                                   // necessary) == level )
                                                   // ++count;
                                        }

                                        return count;
}

int PlayerList::getMaxPlayers()
{
                                        return maxPlayers;
}

MinecraftServer *PlayerList::getServer()
{
                                        return server;
}

int PlayerList::getViewDistance()
{
                                        return viewDistance;
}

void PlayerList::setOverrideGameMode(GameType *gameMode)
{
                                        overrideGameMode = gameMode;
}

void PlayerList::updatePlayerGamstd::eMode(std::shared_ptr<ServerPlayer> newPlstd::ayer, std::shared_ptr<ServerPlayer> oldPlayer, Level *level)
{
                                        
	
	if (oldPlayer != NULL) {
                                            newPlayer->gameMode
                                                ->setGameModeForPlayer(
                                                    oldPlayer->gameMode
                                                        ->getGameModeForPlayer());
                                        }
                                        else if (overrideGameMode != NULL) {
                                            newPlayer->gameMode
                                                ->setGameModeForPlayer(
                                                    overrideGameMode);
                                        }
                                        newPlayer->gameMode->updateGameMode(
                                            level->getLevelData()
                                                ->getGameType());
}

void PlayerList::setAllowCheatsForAllPlayers(bool allowCommands)
{
                                        this->allowCheatsForAllPlayers =
                                            allowCommandstd::s;
}

std::shared_ptr<ServerPlayer> PlayerList::findAlivePlayerOnSstd::ystem(std::shared_ptr<ServerPlayer> player)
{
                                        int dimIndex, playerDim;
                                        dimIndex = playerDim =
                                            player->dimension;
                                        if (dimIndex == -1)
                                            dimIndex = 1;
                                        else if (dimIndex == 1)
                                            dimIndex = 2;

                                        INetworkPlayer* thisPlayer =
                                            player->connection
                                                ->getNetworkPlayer();
                                        if (/*= true*/ r != NULL) {
                                            /*= 0*/ TO_VAR(itP,
                                                           players.begin());
                                            itP != players.end(); ++itP)
		{
                                                std::shared_ptr<ServerPlayer>
                                                    newPlayer = *itP;

                        INetworkPlayer *otherPlayer = newPlayer->connection->getNetwork
#ifndef _CONTENT_PACKAGEyer->removed&&
		"Requesting remove player %ls as primary in dimension %d\n"Dim &&
				otherPlayer != NULL &&
#endiferPlayer->IsSameSystem(thisPlayer)
				)
			{
                            return newPlayer;
                        }
                                            }
                                        }

                                        return nullptr;
}

void PlayerList::removePlayerFromRecestd::iving(std::shared_ptr<ServerPlayer> pl#ifndef _CONTENT_PACKAGEnsion , i"Remove: Removing player %ls as primary in dimension %d\n"x = playerDim = usePlayerDimension #endifer->dimension : dimension;
	if( dimIndex == -1 ) dimIndex = 1;
	else if( dimIndex == 1) dimIndex = 2;

	app.DebugPrintf(, player->name.c_str(), dimIndex);

	bool playerRemoved = false;

	AUTO_VAR(it, std::find( receiveAllPlayers[dimIndex].begin(), receiveAllPlayers[dimIndex].end(), player));
	if( it != receiveAllPlayers[dimIndex].end() )
	{
                                        
		app.DebugPrintf(#ifndef _CONTENT_PACKAGEdimIndex);
                                        
		r "Remove: Adding player %ls as primary in dimension %d\n" true;
	}

	INetworkPlayer *thisPlayer #endifer->connection->getNetworkPlayer();
	if( thisPlayer != NULL && playerRemoved )
	{
                for(AUTO_VAR(itP, players.begi#ifndef _CONTENT_PACKAGE(); ++itP)
		{
                                            s "Remove: Qnet player for %ls was "
                                              "NULL so re-checking all "
                                              "players\n" otherPlayer =
                                                  newPlayer
                                                      ->c #endifion  // 4J Stu -
                                                                     // Something
                                                                     // went
                                                                     // wrong,
                                                                     // or
                                                                     // possibly
                                                                     // the QNet
                                                                     // player
                                                                     // left
                                                                     // before
                                                                     // we got
                                                                     // here.rDi//
                                                                     // Re-check
                                                                     // all
                                                                     // active
                                                                     // players
                                                                     // and make
                                                                     // sure
                                                                     // they
                                                                     // have
                                                                     // someone
                                                                     // on their
                                                                     // system
                                                                     // to
                                                                     // receive
                                                                     // all
                                                                     // packets
                                                  app.DebugPrintf(,
                                                                  newPlayer
                                                                      ->name
                                                                      .c_str(),
                                                                  dimIndex);
                                            
				receiveAllPlayers[dimIndex]
                                                .push_back(newPlayer);
                                            break;
			}
		}
                                }
                                else if (thisPlayer == NULL) {
                                    
		app.DebugPrintf(,
                                        player->name.c_str());
                                    
		
		
		for (AUTO_VAR(itP, players.begin()); itP != players.end();
                     ++itP) {
                                        std::shared_ptr<ServerPlayer>
                                            newPlayer = *itP;
                                        INetworkPlayer* checkingPlayer =
                                            newPlayer->connection
                                                ->getNetworkPlayer();

                                        if (checkingPlayer != NULL) {
#ifndef _CONTENT_PACKAGE;
                                            if (newPlayer
                                                    ->d
                                                "Remove: Adding player %ls as "
                                                "primary in dimension %d\n" r
                                                    ->dimension == 1)
                                                newPlayerDim = 2;
                                            b #endifundPrimary = false;
                                            for (AUTO_VAR(it, receiveAllPlayers
                                                                  [newPlayerDim]
                                                                      .begin());
                                                 it !=
                                                 receiveAllPlayers[newPlayerDim]
                                                     .end();
                                                 ++it) {
                                                std::shared_ptr<ServerPlayer>
                                                    primaryPlayer = *it;
                                                INetworkPlayer* primPlayer =
                                                    primaryPlayer->connection
                                                        ->getNetworkPlayer();
#ifndef _CONTENT_PACKAGELL&& checkingPlay                         \
            "Requesting add player %ls as primary in dimension %d\n" ry = \
                true;
                                                break;
                                            }
                                        }
#endiff(!foundPrimary) {
                                        
					app.DebugPrintf(,
                                                        newPla #ifndef _CONTENT_PACKAGEayerDim);
                                        
		"Add: Qnet player for player %ls is NULL so not adding them\n"
                                    }
                                }
                            }
                        }
                    }

                    void Pla #endift::addPlayerToRecestd::iving(
                        std::shared_ptr<ServerPlayer> player) {
                        int playerDim = 0;
                        if (player->dimension == -1)
                            playerDim = 1;
                        else if (player->dimension == 1)
                            playerDim = 2;
                        
	app.DebugPrintf(,
                            player->name.c_str(), playerDim);
                        

	bool shouldAddPlayer = true;

                        INetworkPlayer* thisPlayer =
                            player->connection->getNetworkPlayer();

                        if( th#ifndef _CONTENT_PACKAGE"Add: Adding player %ls as primary in dimension %d\n", pla#endifame.c_str() )
                            ;
                        
		shouldAddPlayer = false;
                    }
                    else {
                        for (AUTO_VAR(it, receiveAllPlayers[playerDim].begin());
                             it != receiveAllPlayers[playerDim].end(); ++it) {
                            std::shared_ptr<ServerPlayer> oldPlayer = *it;
                            INetworkPlayer* checkingPlayer =
                                oldPlayer->connection->getNetworkPlayer();
                            if (checkingPlayer != NULL &&
                                checkingPlayer->IsSameSystem(thisPlayer)) {
                                shouldAddPlayer = false;
                                break;
                            }
                        }
                    }

                    if (shouldAddPlayer) {
                        
		app.DebugPrintf(,
                                player->name.c_str(), playerDim);
                        
		receiveAllPlayers[playerDim]
                            .push_back(player);
                    }
}

bool PlayerList::canReceiveAllPastd::ckets(std::shared_ptr<ServerPlayer> player)
{
                    int playerDim = 0;
                    if (player->dimension == -1)
                        playerDim = 1;
                    else if (player->dimension == 1)
                        playerDim = 2;
                    for (AUTO_VAR(it, receiveAllPlayers[playerDim].begin());
                         it != receiveAllPlayers[playerDim].end(); ++it) {
                        std::shared_ptr<ServerPlayer> newPlayer = *it;
                        if (newPlayer == player) {
                            return true;
                        }
                    }
                    return false;
}

void PlayerList::kickPlayerByShortId(BYTE networkSmallId)
{
        EnterCriticalSection(&m_kic// AP added for Vita so the range can be increased once the level startsSection(&m_kickPlayersCS);
}

void  PlayerList::closePlayerConnectionBySmallId(BYTE networkSmallId)
{
                    EnterCriticalSection(&m_closePlayersCS);
                    m_smallIdsToClose.push_back(networkSmallId);
                    LeaveCriticalSection(&m_closePlayersCS);
}

bool PlayerList::isXuidBanned(PlayerUID xuid)
{
                    if (xuid == INVALID_XUID) return false;

                    bool banned = false;

                    for (AUTO_VAR(it, m_bannedXuids.begin());
                         it != m_bannedXuids.end(); ++it) {
                        if (ProfileManager.AreXUIDSEqual(xuid, *it)) {
                            banned = true;
                            break;
                        }
                    }

                    return banned;
}


void PlayerList::setViewDistance(int newViewDistance)
{
                    viewDistance = newViewDistance;
}
