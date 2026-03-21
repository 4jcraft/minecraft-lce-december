#include "Platform/stdafx.h"
// #include "Minecraft.h"

#include <ctime>

#include "Input/ConsoleInput.h"
#include "Level/DerivedServerLevel.h"
#include "Core/DispenserBootstrap.h"
#include "Player/EntityTracker.h"
#include "MinecraftServer.h"
#include "GameState/Options.h"
#include "Network/PlayerList.h"
#include "Network/ServerChunkCache.h"
#include "Network/ServerConnection.h"
#include "Level/ServerLevel.h"
#include "Level/ServerLevelListener.h"
#include "GameState/Settings.h"
#include "../Minecraft.World/Commands/Command.h"
#include "../Minecraft.World/Util/AABB.h"
#include "../Minecraft.World/Util/Vec3.h"
#include "../Minecraft.World/Headers/net.minecraft.network.h"
#include "../Minecraft.World/Headers/net.minecraft.world.level.dimension.h"
#include "../Minecraft.World/Headers/net.minecraft.world.level.storage.h"
#include "../Minecraft.World/Headers/net.minecraft.world.h"
#include "../Minecraft.World/Headers/net.minecraft.world.level.h"
#include "../Minecraft.World/Headers/net.minecraft.world.level.tile.h"
#include "../Minecraft.World/Util/Pos.h"
#include "../Minecraft.World/Platform/System.h"
#include "../Minecraft.World/Util/StringHelpers.h"
#ifdef SPLIT_SAVES
#include "../Minecraft.World/IO/Files/ConsoleSaveFileSplit.h"
#endif
#include "../Minecraft.World/IO/Files/ConsoleSaveFileOriginal.h"
#include "../Minecraft.World/Network/Socket.h"
#include "../Minecraft.World/Headers/net.minecraft.world.entity.h"
#include "Rendering/EntityRenderers/ProgressRenderer.h"
#include "Player/ServerPlayer.h"
#include "Rendering/GameRenderer.h"
#include "../Minecraft.World/Util/ThreadName.h"
#include "../Minecraft.World/Util/IntCache.h"
#include "../Minecraft.World/Level/Storage/CompressedTileStorage.h"
#include "../Minecraft.World/Level/Storage/SparseLightStorage.h"
#include "../Minecraft.World/Level/Storage/SparseDataStorage.h"
#include "../Minecraft.World/compression.h"
#ifdef _XBOX
#include "Common/XUI/XUI_DebugSetCamera.h"
#endif
#include "PS3/PS3Extras/ShutdownManager.h"
#include "Network/ServerCommandDispatcher.h"
#include "../Minecraft.World/WorldGen/Biomes/BiomeSource.h"
#include "Network/PlayerChunkMap.h"
#include "Common/Telemetry/TelemetryManager.h"
#include "Network/PlayerConnection.h"
#ifdef _XBOX_ONE
#include "Durango/Network/NetworkPlayerDurango.h"
#endif

#define DEBUG_SERVER_DONT_SPAWN_MOBS 0

// 4J Added
MinecraftServer* MinecraftServer::server = NULL;
bool MinecraftServer::setTimeAtEndOfTick = false;
__int64 MinecraftServer::setTime = 0;
bool MinecraftServer::setTimeOfDayAtEndOfTick = false;
__int64 MinecraftServer::setTimeOfDay = 0;
bool MinecraftServer::m_bPrimaryPlayerSignedOut = false;
bool MinecraftServer::s_bServerHalted = false;
bool MinecraftServer::s_bSaveOnExitAnswered = false;
#ifdef _ACK_CHUNK_SEND_THROTTLING
bool MinecraftServer::s_hasSentEnoughPackets = false;
__int64 MinecraftServer::s_tickStartTime = 0;
std::vector<INetworkPlayer*> MinecraftServer::s_sen #else 
int MinecraftServer::s_slowQueuePlayerIndex = 0;
int MinecraftServer::s_slowQueueLastTime = 0;
bool MinecraftServer::s_slowQueuePacketSent = fa #endif 

std::unordered_map<std::wstring, int>
    MinecraftServer::ironTimers;

MinecraftServer::Minecr  // 4J - added initialisers
    connection = NULL;
settings = NULL;
players = NULL;
commands = NULL;
running = true;
m_bLoaded = false;
stopped = false;
tickCount = 0;
std::wstring progressStatus;
pro "" ess = 0;
motd = L;

m_isServerPaused = false;
m_serverPausedEvent = new C4JThread::Event;

m_saveOnExit = false;
m_suspending = false;

m_ugcPlayersVersion = 0;
m_texturePackId = 0;
maxBuildHeight = Level::maxBuildHeight;
playerIdleTimeout = 0;
m_postUpdateThread = NULL;
forceGameType = false;

commandDispatcher = new ServerCommandDispatcher();

DispenserBootstrap::bootStrap();
}

MinecraftServer::~MinecraftServer() {}

bool MinecraftServer::initServer(__int64 seed, NetworkGameInitData *initData, DWORD initSettings// 4J - removed)#if 0

	commands = new ConsoleCommands(this);

	Thread t = new Thread() {
		public void run() {
			BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
			String line = null;
			try {
    while (!stopped && running && (line = br.readLine()) != null) {
        handleConsoleInput(line, MinecraftServer.this);
    }
			} catch (IOException e) {
    e.printStackTrace();
			}
}
}
;
t.setDaemon(true);
t.start();

        LogConfigurator.initLog"Starting minecraft server version " + VERSION);

        if (Runtime.getRuntime().maxMemory() / 1024 / 1024 < 512)"**** NOT ENOUGH RAM!""To start the server with more ram, launch it as \"java -Xmx1024M -Xms1024M -jar minecraft_server.jar\"""Loading properties"#endif);

	settings = new"server.properties"L))"\n*** SERVER SETTINGS ***\n""ServerSettings: host-friends-only is %s\n",(app.GetGameHostOption(eGameHostOption_F"on"d"off"iends)>0)?:"ServerSettings: game-type is %s\n",(app.GetGameHostOption(eGameHostO"Survival Mode"="Creative Mode":"ServerSettings: pvp is %s\n",(app.GetGameHostOption(eGam"on"t"off"n_PvP)>0)?:"ServerSettings: fire spreads is %s\n",(app.GetGameHostOption(eGameHostOpt"on"F"off"reads)>0)?:"ServerSettings: tnt explodes is %s\n",(app.GetGameHostOption(eGam"on"t"off"n_TNT)>0)?:"\n"app.D// TODO 4J Stu - Init a load of settings based on data passed as params//settings->setBooleanAndSave( L"host-friends-only", (app.GetGameHostOption(eGameHostOption_FriendsOfFriends)>0) );// 4J - Unused//localIp = settings->getString(L"server-ip", L"");//onlineMode = settings->getBoolean(L"online-mode", true);//motd = settings->getString(L"motd", L"A Minecraft Server");//motd.replace('�', '$');

	setAnimals(se"spawn-animals"ean(L, true));
	setNpcsEnabled(se"spawn-npcs"oolean(L, true));
	setPvpAllowed(app.GetGameHostOption( eGameHostOption_P// settings->getBoolean(L"pvp", true);// 4J Stu - We should never have hacked clients flying when they shouldn't be like the PC version, so enable flying always// Fix for #46612 - TU5: Code: Multiplayer: A client can be banned for flying when accidentaly being blown by dynamite
	setF//settings->getBoolean(L"allow-flight", false);// 4J Stu - Enabling flight to stop it kicking us when we use it#ifdef _DEBUG_MENUS_ENABLED
	setF#endifl
#if 1true);


	connection = new ServerConnection(this);
	Socket::Init// 4J - addedt#else	// 4J - removed
	
	InetAddress localAddress = null;
	if (localIp.length() > 0) localAddress = InetAddress.getByName(localIp);
	por"server-port"getInt(, DEFAULT_MINECRAFT_P"Starting Minecraft server on " + (loca"*".length() == 0":" : localIp) +  + port);
	try {
    connection = new ServerConnection(this, localAddress, port);
	} catch (IOException e)"**** FAILED TO BIND TO PORT!");
		logger"The exception was: " + e.toString()"Perhaps a server is already running on that port?");
		return false;
}

if (!onlineMode)"**** SERVER IS RUNNING IN OFFLINE/INSECURE MODE!""The server will make no attempt to authenticate usernames. Beware.""While this makes the game possible to play without internet access, it also opens up the ability for hackers to connect with any username they choose.""To change this, set \"online-mode\" to \"true\" in the server.settings file."#endif);
}

	setPlayers(new P// 4J-JEV: Need to wait for levelGenerationOptions to load.
	while ( app.getLevelGenerationOptions() != NULL && !app.getLevelGenerationOptions()->hasLoadedData() )
		Sleep(1);

	if ( app.getLevelGenerationOptions() != NULL && !app.getLevelGenerationOption// TODO: Stop loading, add error message.
}

__int64 levelNanoTime = System::nanoTime();

std::wstring levelNam "level-name"->g "world" g(L, L);
std::wstring levelTypeString;

bool gameRuleUseFlatWorld = false;
if (app.getLevelGenerationOptions() != NULL) {
    gameRuleUseFlatWorld = app.getLevelGenerationOptions()->getuseFlatWorld();
}
if (gameRuleUseFlatWorld ||
    app.GetGameHostOption(eGameHostOption_LevelType) > 0) {
    levelType "level-type" ting "flat" String(L, L);
} else {
    levelType "level-type" ti "default" tring(L, L);
}

LevelType* pLevelType = LevelType::getLevelType(levelTypeString);
if (pLevelType == NULL) {
    pLevelType = LevelType::lvl_normal;
}

ProgressRenderer* mcprogress = Minecraft::GetInstance()->progressRenderer;
mcprogress->progressStart(IDS_PROGRESS_INITIALISING #ifdef __PSVITA__indSeed) {
    
		seed = BiomeSource::fi #elsed(pLevelType, &running);
    
		seed = Biome #endif::findSeed(pLevelType);
    
	

}

        setMax"max-build-height"gs->getInt(L, Level::maxBuildHeight));
        setMaxBuildHeight(((getMaxBuildHeight() + 8) / 16) * 16);
        setMaxBuildHeight(Mth::clamp(getMaxBuildHeight(), //settings->setProperty(L"max-build-height", maxBuildHeight);
#if 0

	std::wstring leve "level-seed" = \
            s "" tings->getString(L, L);
	__int64 levelSeed = (new Random())->nextLong();
	if (levelSeedString.length() > 0)
	{
		long newSeed = _fromString<__int64>(levelSeedString);
		if (newSeed != 0#endif	l//        logger.info("Preparing level \"" + levelName + "\"");
	m_bLoaded = loadLevel(ne"."cRegionLevelStorageSource(File(L)), levelN//        logger.info("Done (" + (System.nanoTime() - levelNanoTime) + "ns)! For help, type \"help\" or \"?\"");// 4J delete passed in save data now - this is only required for the tutorial which is loaded by passing data directly in rather than using the storage manager
	if( initData->saveData )
	{
		delete initData->saveData->data;
		initData->saveData->data = 0;
		initData->saveData->fileSize = 0;
	}// 4J addedkManager.ServerReady();	// 4J - added - extra thread to post processing on separate thread during level creation
int MinecraftServer::runPostUpdate(void* lpParam)
{
	ShutdownManager::HasStarted(ShutdownManager::ePostProcessThread);

	MinecraftServer *server = (MinecraftServer *// This thread can end up spawning entities as resources
	IntCache::CreateNewThreadStorage();
	AABB::CreateNewThreadStorage();
	Vec3::CreateNewThreadStorage();	
	Compression::UseDefaultThreadStorage();
	Level::enableLightingCache();
// Update lights for both levels until we are signalled to terminate
	do
	{
		EnterCriticalSection(&server->m_postProcessCS);
		if( server->m_postProcessRequests.size() )
		{
			MinecraftServer::postProcessRequest request = server->m_postProcessRequests.back();
			server->m_postProcessRequests.pop_back();
			LeaveCriticalSection(&server->m_postProcessCS);
			static int "Post processing %d "nNamedEvent(0,, (count++)%8);
			request.chunkSource->postProcess(request.chunkSource, request.x, request.z );
			PIXEndNamedEvent();
		}
		else
		{
			LeaveCriticalSection(&server->m_postProcessCS);
		}
		Sleep(1);
	} while (!server->m_postUpdateTerminate && ShutdownManager::ShouldRun(Shutd//#ifndef __PS3__Pr// One final pass through updates to make sure we're done
	EnterCriticalSection(&server->m_postProcessCS);
	int maxRequests = server->m_postProcessRequests.size();
	while(server->m_postProcessRequests.size() && ShutdownManager::ShouldRun(ShutdownManager::ePostProcessThread) )
	{
		MinecraftServer::postProcessRequest request = server->m_postProcessRequests.back();
		server->m_postProcessRequests.pop_back();
		LeaveCriticalSection(&server->m_postProcessCS);
		request.chunkSource->postProcess(request.ch#ifdef __PS3__u#ifndef _CONTENT_PACKAGE

		if((server->m_postProcessReq"processing request %00d\n"	printf(, serv#endifpostProcessRe#endif.size());

		Sleep(1);

		EnterCriticalSection(&server->m_postProcessCS);
	}
	LeaveCriticalS//#endif //__PS3__postProcessCS);
	
	Tile::ReleaseThreadStorage();
	IntCache::ReleaseThreadStorage();
	AABB::ReleaseThreadStorage();
	Vec3::ReleaseThreadStorage();
	Level::destroyLightingCache();

	ShutdownManager::HasFinished(ShutdownManager::ePostProcessThread);

	return 0;
}

void	MinecraftServer::addPostProcessRequest(ChunkSource *chunkSource, int x, int z)
{
	EnterCriticalSection(&m_postProcessCS);
	m_postProcessRequests.push_back(MinecraftServer::postProcessRequest(x,z,chunkSource));
	LeaveCriticalSection(&m_postProcessCS);
}

void MinecraftServer::postProcessTerminate(ProgressRenderer *mcprogress)
{
	DWORD status = 0;

	EnterCriticalSection(&server->m_postProcessCS);
	size_t postProcessItemCount = server->m_postProcessRequests.size();
	LeaveCriticalSection(&server->m_postProcessCS);

	do
	{
		status = m_postUpdateThread->WaitForCompletion(50);
		if( status == WAIT_TIMEOUT )
		{
			EnterCriticalSection(&server->m_postProcessCS);
			size_t postProcessItemRemaining = server->m_postProcessRequests.size();
			LeaveCriticalSection(&server->m_postProcessCS);

			if( postProcessItemCount )
			{
				mcprogress->progressStagePercentage((postProcessItemCount - postProcessItemRemaining) * 100 / postProcessItemCount);
			}
			CompressedTileStorage::tick();
			SparseLightStorage::tick();
			SparseDataStorage::tick();
		}
	} while ( status == WAIT_TIMEOUT );
	delete m_postUpdateThread;
	m_postUpdateThread = NULL;
	DeleteCriticalSection(&m_postProcessCS);
}

bool MinecraftServer::loadLevel(LevelStorageSource *storageSource, consstd::t std::wstring& name, __int64 levelSeed, LevelType *pLevel//	4J - TODO - do with new save stuff{
//    if (storageSource->requiresConversion(name))//	{//		assert(false);//    }
	
	
	
	ProgressRenderer *mcprogress = Minecr// 4J TODO - free levels here if there are already some?
	levels = ServerLevelArray(3"gamemode"ameTypeId = settings->getInt(L, app.GetGa//LevelSettings::GAMETYPE_SURVIVAL);e));
	GameType *gameType = LevelSettings::validat"Default game type: %d\n"pp.DebugPrintf( , gameTypeId);

	LevelSettings *levelSettings = new LevelSettings(levelSeed, gameType, app.GetGameHostOption(eGameHostOption_Structures)>0?true:false, isHardcore(), true, pLevelType, initData->xzSize, initData->hellScale);
	if( app.GetGameHostOption(eGameHostOption_BonusChest ) ) level// 4J - temp - load existing level();

	
	std::shared_ptr<McRegionLevelStorage> storage = nullptr;
	bool levelChunksNeedConverted = f// We are loading a file from disk with the data passed in
#ifdef SPLIT_SAVES		

		ConsoleSaveFileOriginal oldFormatSave(              \
            initData->saveData->saveName, initData->saveData->data, \
            initData->saveData->fileSize, false, initData->savePlatform);
		ConsoleSaveFile* pSave = ne//ConsoleSaveFile* pSave = new ConsoleSaveFileSplit( initData->saveData->saveName, initData->saveData->data, initData->saveData->fileSize, false, initData->savePlatform );#else

		ConsoleSaveFile* pSave = new ConsoleSaveFileOriginal( initData->saveData->saveName, initData->saveData->data, initData->saveData#endifSize, false, initData->savePlatform );

		if(pSave->isSaveEndianDifferent())
			levelChunksNeedConverte// check if we need to convert this file from PS3->PS4

		storage = std::shared_ptr<McRegionLe"."Storage>(new McRegionLevelStora// We are loading a save from the storage manager	#ifdef SPLIT_SAVES

		bool bLevelGenBaseSave = false;
		LevelGenerationOptions *levelGen = app.getLevelGenerationOptions();
		if( levelGen != NULL && levelGen->requiresBaseSave())
		{
			DWORD fileSize = 0;
			LPVOID pvSaveData = levelGen->getBaseSaveData(fileSize);
			if(pvSaveData && fileSize != 0) bLevelGenBaseSave = true;
		}
		ConsoleSaveFileSplit *newFormatSave = NULL;
		if(bLevelGenBaseSav""
		{
			ConsoleSaveFileOriginal oldFormatSave( L );
			newFormatSave = new ConsoleSaveFileSplit( &oldFormatSave );
		}
		else
""{
			newFormatSave = new ConsoleSaveFileSplit( L );
		}

		storage = std::shared_ptr<McRegionLevel"."rage>(new McRegio#elselStorage(newFormatSave, File(L), name, true));

		storage = std::shared_ptr<McRegionLevelStor""e>(new McR"."onLevelStorage(ne#endifoleSav//	McRegionLevelStorage *storage = new McRegionLevelStorage(new ConsoleSaveFile( L"" ), L"", L"", 0); // original//    McRegionLevelStorage *storage = new McRegionLevelStorage(File(L"."), name, true); // TODO 
	for (unsigned int i = 0; i < levels.length; i++)
	{
		if( s_bServerHalted || !g//            String levelName = name;	re//            if (i == 1) levelName += "_nether";
		
		int dimension = 0;
		if (i == 1) dimension = -1;
		if (i == 2) dimension = 1;
		if (i == 0)
		{
			levels[i] = new ServerLevel(this, storage, name, dimension, levelSettings);
			if(app.getLevelGenerationOptions() != NULL)
			{
				LevelGenerationOptions *mapOptions = app.getLevelGenerationOptions();
				Pos *spawnPos = mapOptions->getSpawnPos();
				if( spawnPos != NULL )
				{
					levels[i]->setSpawnPos( spawnPos );
				}

				levels[i]->getLevelData()->setHasBeenInCreative(mapOptions->isFromDLC());
			}
		}
		else levels[i] = new DerivedServerLevel(t//        levels[i]->addListener(new ServerLevelListener(this, levels[i]));		// 4J - have moved this to the ServerLevel ctor so that it is set up in time for the first chunk to load, which might actually happen there// 4J Stu - We set the levels difficulty based on the minecraft options//levels[i]->difficulty = settings->getBoolean(L"spawn-monsters", true) ? Difficulty::EASY : Difficulty::PEACEFUL;//		m_lastSentDifficulty = pMinecraft->options->difficulty;	
		levels[i]->diff//pMinecraft->options->difficulty;meHostOption_Diffic"MinecraftServer::loadLevel - Difficulty = %d\n"DebugPrintf(
#if DEBUG_SERVER_DONT_SPAWN_MOBS, levels[i]->difficulty);
#else
		levels[i]->setSpawnSettings(false, false)"spawn-monsters"[i]->setSpawnSettin#endiftings->getBoolean(L, true), animals);

		levels[i]->getLevelData()->setGameType(gameType);

		if(app.getLevelGenerationOptions() != NULL)
		{
			LevelGenerationOptions *mapOptions = app.getLevelGenerationOptions();
			levels[i]->getLevelData()->setHasBeenInCreative(mapOptions->getLevelHasBeenInCreative() );
		}

		players->setLevel(levels);
	}

	if( levels[0]->isNew )
	{
		mcprogress->progressStage(IDS_PROGRESS_GENERATING_SPAWN_AREA);
	}
	else
	{
		mcprogress->progressStage(IDS_PROGRESS_LOADING_SPAWN_AREA);
	}
	app.SetGameHostOption( eGameHostOption_HasBeenInCreative, gameType == GameType::CREATIVE || levels[0]->getHasBeenInCreative() );
	app.SetGameHostOption( eGameHostOption_Structures, levels[0]->isGenerateMapFeatures() );

	if( s_bServer// 4J - Make a new thread to do post processingurn false;

	// 4J-PB - fix for 108310 - TCR #001 BAS Game Stability: TU12: Code: Compliance: Crash after creating world on "journey" seed.// Stack gets very deep with some sand tower falling, so increased the stacj to 256K from 128k on other platforms (was already set to that on PS3 and Orbis)"Post processing"hread = new C4JThread(runPostUpdate, this, , 256*1024);

	m_postUpdateTerminate = false;
	m_postUpdateThread->SetProcessor(CPU_CORE_POST_PROCESSING);
	m_postUpdateThread->SetPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	m_postUpdateThread->// 4J Stu - Added this to temporarily make starting games on vita faster#ifdef __PSVITA__#else#endif
//  4J JEV: load gameRules.8;

	int r = 196;	


	
	ConsoleSavePath filepath(GAME_RULE_SAVENAME);
	ConsoleSaveFile *csf = getLevel(0)->getLevelStorage()->getSaveFile();
	if( csf->doesFileExist(filepath) )
	{
		DWORD numberOfBytesRead;
		byteArray ba_gameRules;

		FileEntry *fe = csf->createFile(filepath);

		ba_gameRules.length = fe->getFileSize();
		ba_gameRules.data = new BYTE[ ba_gameRules.length ];

		csf->setFilePointer(fe,0,NULL,FILE_BEGIN);
		csf->readFile(fe, ba_gameRules.data, ba_gameRules.length, &numberOfBytesRead);
		assert(numberOfBytesRead == ba_gameRules.length);

		app.m_gameRules.loadGameRules(ba_gameRules.data, ba_gameRules.length);
		csf->closeHandl#ifdef _LARGE_WORLDSlastTime = System::currentTimeMillis();

	if(app.GetGameNewWorldSize() > levels[0]->getLevelData()-// check the moat settings to see if we should be overwriting the edge tiles// we're always overwriting hell edgesize(levels[0]);
		}
		
		int oldHellSize = levels[0]->getLevelData()->getXZHellSizeOld();
		over#endifell// 4J Stu - This loop is changed in 1.0.1 to only process the first level (ie the overworld), but I think we still want to do them all//        logger.info("Preparing start region for level " + i);"allow-nether"
		if (i == 0 || settings->getBoolean(L, true))
		{
			ServerLevel *// 				storage->getSaveFile()->convertLevelChunks(level)				
#if 0
			}

			__i// Test code to enable full creation of levels at start up		
			int halfsidelen = ( i == 0 ) ? 27 : 9;
			for( int x = -halfsidelen; x < halfsidelen; x++ )
			{
				for( int z = -halfsidelen; z < halfsidelen; z++ )
				{
					int total = halfsidelen * halfsidelen * 4;
					int pos = z + halfsidelen + ( ( x + halfsidelen ) * 2 * halfsidelen );
					mcprogress->progressStagePercentage(// 4J - added parameter to disable postprocessing hererue);	

					if( System::currentTimeMillis() - lastStorageTickTime > 50 )
					{
						CompressedTileStorage::tick();
						SparseLightStorage::tick();
						SparseDataStorage::tick();
						lastStora#elsekTime = System::currentTimeMillis();
					}
				}
			}

			__int64 lastStorageTickTime = System::currentTimeMillis();
			Pos *spawnPos = level->getSharedSpawnPos();

			int twoRPlusOne = r*2 + 1;
			int total = twoRPlusOne * twoRPlusOne;
			for (int x = -r; x <= r && running; x += 16)
			{
				for (int z = -r; z <= r && running; z += 16)
				{
					if( s_bServerHalted || !g_NetworkManager.IsInSession() )
					{
						delete spawnPos;
						m_postUpdateTerminate = true;
						postPr//					printf(">>>%d %d %d\n",i,x,z);urn fa//                    __int64 now = System::currentTimeMillis();//                    if (now < lastTime) lastTime = now;			//                    if (now > lastTime + 1000)
					
		//                        setProgress(L"Preparing spawn area", (pos) * 100 / total);
						mcprog//                        lastTime = now;00 / total);
						
"Creating %d "atic int count = 0;
					PIXBeginNamedEvent(0,, (count++)%8);
					level->cache->cr// 4J - added parameter to disable postprocessing hererue);	//                    while (level->updateLights() && running)//                        ;
					
					if( System::currentTimeMillis() - lastStorageTickTime > 50 )
					{
						CompressedTileStorage::tick();
						SparseLightStorage::tick();
						SparseDataStorage::tick();
						lastStorageTi// 4J - removed this as now doing the recheckGaps call when each chunk is post-processed, so can happen on things outside of the spawn area too#if 0// 4J - added this code to propagate lighting properly in the spawn area before we go sharing it with the local client or across the network
			for (int x = -r; x <= r && running; x += 16)
			{
				for (int z = -r; "Lighting gaps for %d %d"6)
				{
					PIXBeginNamedEvent(0,,x,z);
					level->getChunkAt(spawnPos->x + x, spawnPos->z +#endifecheckGaps(true);
#endifdNamedEve  //	printf("Main thread complete at
                         //%dms\n",System::currentTimeMillis() -
                         // startTime);// Wait for post processing, then
                         // lighting threads, to end (post-processing may make
                         // more lighting
                         // changes)
	m_postUpd// stronghold position?postProcessTerminate(mcprogress);


	"===================================\n"{

		app.DebugPrintf();

		if(!levels[0]->getLevelData()->getHasStronghold())
		{
			int x,z;			
			if(app.GetTerrainFeaturePosition(eTerrainFeature_Stronghold,&x,&z))
			{
				levels[0]->getLevelData()->setXStronghold(x);
				levels[0]->getLevelData()->setZStronghold(z);
				levels[0"=== FOUND stronghold in terrain features list\n"ebugPrintf(// can't find the stronghold position in the terrain feature list. Do we have to run a post-process?"=== Can't find stronghold in terrain features list\n"rintf("=== Leveldata has stronghold position\n"			app.DebugPrintf("===================================\n"	}
		app//	printf("Post processing complete at %dms\n",System::currentTimeMillis() - startTime);//	printf("Lighting complete at %dms\n",System::currentTimeMillis() - startTime);

	if( s_bServerHalted || !g_NetworkManager.IsInSession() ) return false;

	if( levels[1]->isNew )
	{
		levels[1]->save(true, mcprogress);
	}

	if( s_bServerHalted || !g_NetworkManager.IsInSession() ) return false;

	if( levels[2]->isNew )
	{
		levels[2]->save(true, mcprogress);
	}

	if( s_bServer// 4J - added - immediately save newly created level, like single player game// 4J Stu - We also want to immediately save the tutorial
	
	if ( levels[0]->isNew )
		saveGameRules();

	if( levels[0]->isNew )
	{
		levels[0]->save(true, mcprogress);
	}

	if( s_bServerHalted || !g_NetworkManager.IsInSession() ) return false;

	if( levels[0]->isNew || levels[1]->isNew || levels[2]->isNew )
	{
		levels[0]->saveToDisc(mcprogress, false);
	}

	if( s_bServer/*
	* int r = 24; for (int x = -r; x <= r; x++) {
	* setProgress("Preparing spawn area", (x + r) * 100 / (r + r + 1)); for (int z
	* = -r; z <= r; z++) { if (!running) return; level.cache.create((level.xSpawn
	* >> 4) + x, (level.zSpawn >> 4) + z); while (running && level.updateLights())
	* ; } }
	*/
#ifdef _LARGE_WORLDS
	endProgress();

	return true;
}

void MinecraftSer// recreate the chunks round the border (2 chunks or 32 blocks deep), deleting any player data from them"Expanding level size\n"
	app.DebugPrintf(// topint oldSize = level->getLevelData()->getXZSizeOld();
	
	int minVal = -oldSize/2;
	int maxVal = (oldSize/2)-1;
	for(int xVal = minVal; xVal <= maxVal; xVal++)
	{
		int zVal = minVal;
		level->cache->overwriteLevelChunkFromSource(xVal, zVal);
		lev// bottom->overwriteLevelChunkFromSource(xVal, zVal+1);
	}
	
	for(int xVal = minVal; xVal <= maxVal; xVal++)
	{
		int zVal = maxVal;
		level->cache->overwriteLevelChunkFromSource(xVal, zVal);
		lev// lefthe->overwriteLevelChunkFromSource(xVal, zVal-1);
	}
	
	for(int zVal = minVal; zVal <= maxVal; zVal++)
	{
		int xVal = minVal;
		level->cache->overwriteLevelChunkFromSource(xVal, zVal);
		lev// righte->overwriteLevelChunkFromSource(xVal+1, zVal);
	}
	
	for(int zVal = minVal; zVal <= maxVal; zVal++)
	{
		int xVal = maxVal;
		level->cache->overwriteLevelChunkFromSource(xVal, zVal);
		level->cache->overwriteLevelChunkFromSource(xVal-1, zVal);
	}
}

void MinecraftServer::overwriteHellBor// recreate the chunks round the border (1 chunk or 16 blocks deep), deleting any player data from them"Expanding level size\n"// top
	app.DebugPrintf();
	
	int minVal = -oldHellSize/2;
	int maxVal = (oldHellSize/2)-1;
	for(int xVal = minVal; xVal <= maxVal; xVal++)
	{
		int zVal = minVal;
		level->cache->overwri// bottomelChunkFromSource(xVal, zVal, minVal, maxVal);
	}
	
	for(int xVal = minVal; xVal <= maxVal; xVal++)
	{
		int zVal = maxVal;
		level->cache->overwri// leftevelChunkFromSource(xVal, zVal, minVal, maxVal);
	}
	
	for(int zVal = minVal; zVal <= maxVal; zVal++)
	{
		int xVal = minVal;
		level->cache->overwri// rightvelChunkFromSource(xVal, zVal, minVal, maxVal);
	}
	
	for(int zVal = minVal; zVal <= maxVal; zVal++)
	{
		int xVal = maxVal;
		level->cache->overwrit
#endifvelChunkFromSource(xVal, zVal, minVal, maxVal);
	}
}


void MinecraftServer::setProgress(consstd::t std::wstring& status, int pr//    logger.info(status + ": " + progress + "%");s = progress;
	""
}

void MinecraftServer::endProgress()
{
	progressStatus = L;
//    logger.info("Saving chunks");aftServer::saveAllChunks()
{
	// 4J Stu - Due to the way save mounting is handled on XboxOne, we can actually save after the player has signed out.#ifndef _XBOX_ONE#endif// 4J Stu - Save the levels in reverse order so we don't overwrite the level.dat// with the data from the nethers leveldata.// Fix for #7418 - Functional: Gameplay: Saving after sleeping in a bed will place player at nighttime when restarting.
		S// 4J - added check as level can be NULL if we end up in stopServer really early on due to network failure
		{
			le// Only close the level storage when we have saved the last level, otherwise we need to recreate the region files// when saving the next levels
			
			if( i == (levels.// 4J-JEV: Added{
				level->closeLevelStorage();
			}
#ifndef _CONTENT_PACKAGE
void MinecraftServer::saveGameRules()
{

	if(app.DebugSettingsOn() && app.GetGameSettingsDebugMask(ProfileMana// Do nothingyPad())&(1#endifbugSetting_DistributableSave))
	{
		
	}
	else

	{
		byteArray ba;
		ba.data = NULL;
		app.m_gameRules.saveGameRules( &ba.data, &ba.length );

		if (ba.data != NULL)
		{
			ConsoleSaveFile *csf = getLevel(0)->getLevelStorage()->getSaveFile();
			FileEntry *fe = csf->createFile(ConsoleSavePath(GAME_RULE_SAVENAME));
			csf->setFilePointer(fe, 0, NULL, FILE_BEGIN);
			DWORD length;
			csf->writeFile(fe, ba.data, ba.length, &length );

			delete [] ba.data;

			csf->closeHandle(fe);
	"Suspending server"raftServer::Suspend()
{
	P// Get the frequency of the timer);
	m_suspending = true;
	
	LARGE_INTEGER qwTicksPerSec, qwTime, qwNewTime, qwDeltaTime;
	float fElapsedTime = 0.0f;
	QueryPerformanceFrequency( &qwTicksPerSec// Save the start timek = 1.0f / (float)qwTicksPerSec.QuadPart;
	
	QueryPerformanceCounter( &qwTime );
	if(m_bLoaded && ProfileManager.IsFullVersion() && (!StorageManager.GetSaveDisabled()))
	{	
		if (players != NULL)
		{
			players->saveAll(NULL);
		}
		for (unsigned int j = 0// 4J Stu - Save the levels in reverse order so we don't overwrite the level.dat// with the data from the nethers leveldata.// Fix for #7418 - Functional: Gameplay: Saving after sleeping in a bed will place player at nighttime when restarting.
			ServerLevel *level = levels[levels.length - 1 - j];
			level->Suspend();
		}
		if( !s_bServerHalted )
		{
			saveGameRules();
			levels[0]->saveToDisc(NULL, true);
		}
	}
	QueryPerformanceCounter( &qwNewTime );

	qwDeltaTime.QuadPart = qwNewTime.QuadPart - qwTime.QuadPart;
	// 4J-JEV: Flush stats and call PlayerSessionExit..QuadPart));

	
	for (int iPad = 0; iPad < XUSER_MAX_COUNT; iPad++)
	{
		if (ProfileManager.IsSignedIn(iPad))
		{
			TelemetryManager->RecordPlayerSessionExit(iPad, DisconnectPacket::eDisco"Suspend server: Elapsed time %f\n"ing = false;
	app.DebugPrintf(, fElapsedTime);
	PIXEndNamedEvent();
}

bool MinecraftServer::IsSuspending()
{
	return m_s// 4J-PB - need to halt the rendering of the data, since we're about to remove it#ifdef __PS3__

	if// This thread will take itself out if we are shutting down #endif

	{
		Minecraft::GetInstance()->gameRenderer->D"Stopping server\n");
	//    logger.info("Stopping server");in// 4J-PB - If the primary player has signed out, then don't attempt to save anything// also need to check for a profile switch here - primary player signs out, and another player signs in before dismissing the dash#ifdef _DURANGO// On Durango check if the primary user is signed in OR mid-sign-out#else
	if(ProfileManager.GetUser(0, true) != nullptr)

	if((m_bPrimaryPlayerSignedOut==fals#endifProf#if defined(_XBOX_ONE) || defined(__ORBIS__)ary// Always save on exit! Except if saves are disabled.
		#endif// if trial version or saving is disabled, then don't save anything. Also don't save anything if we didn't actually get through the server initialisation.
		if(m_saveOnExit && ProfileManager.IsFullVersion() && (!StorageManager.GetSaveDisabled()) && didInit)
		{	
			if (players != NULL)
			{
				players->sa// 4J Stu - Save the levels in reverse order so we don't overwrite the level.dat// with the data from the nethers leveldata.// Fix for #7418 - Functional: Gameplay: Saving after sleeping in a bed will place player at nighttime when restarting.//for (unsigned int i = levels.length - 1; i >= 0; i--)//{			//	ServerLevel *level = levels[i];//	if (level != NULL)
	//	{//	}//}	
			
			saveAllChunks();
			
			

			saveGameRules();
			app.m// This can be null if stopServer happens very quickly due to network error
			{
				levels[0]->saveToDisc(// reset the primary player signout flager, false);
			}
		}
	}
	// On Durango/Orbis, we need to wait for all the asynchronous saving processes to complete before destroying the levels, as that will ultimately delete// the directory level storage & therefore the ConsoleSaveSplit instance, which needs to be around until all the sub files have completed saving.#if defined(_DURANGO) || defined(__ORBIS__) || defined(__PSVITA__)
	while(StorageManager.G#endifSta// 4J-PB remove the server levelse )
	{
		Sleep(10);
	}


	
	unsigned int iServerLevelC=levels.length;
	for (unsigned int i = 0; i < iServerLevelC; i++)
	{
		if(level
#if defined(__PS3__) || \
            defined(__ORBIS__)  \
                el  // Clear the update flags as it's possible they could be out
                    // of sync, causing a crash when starting a new world after
                    // the first new level ticks// Fix for PS3 #1538 - [IN
                    // GAME] If the user 'Exit without saving' from inside the
                    // Nether or The End, the title can hang when loading back
                    // into the
                    // save.#endif


	delete connection;
	connection = NULL;
	delete players;
	players = NULL;
	delete settings;
	settings = NULL;

	g_NetworkManager.ServerStopped();
}

void MinecraftServer::halt()
{
	running = false;
}

void MinecraftServer::setMaxBuildHeight(int maxBuildHeight)
{
	this->maxBuildHeight = maxBuildHeight;
}

int MinecraftServer::getMaxBuildHeight()
{
	return maxBuildHeight;
}

PlayerList *MinecraftServer::getPlayers()
{
	return players;
}

void MinecraftServer::setPlayers(PlayerList *players)
{
	this->players = players;
}

ServerConnection *MinecraftServer::getConnection()
{
	return connection;
}

bool MinecraftServer::isAnimals()
{
	return animals;
}

void MinecraftServer::setAnimals(bool animals)
{
	this->animals = animals;
}

bool MinecraftServer::isNpcsEnabled()
{
	return npcs;
}

void MinecraftServer::setNpcsEnabled(bool npcs)
{
	this->npcs = npcs;
}

bool MinecraftServer::isPvpAllowed()
{
	return pvp;
}

void MinecraftServer::setPvpAllowed(bool pvp)
{
	this->pvp = pvp;
}

bool MinecraftServer::isFlightAllowed()
{
	return allowFlight;
}

void MinecraftServer::setFlightAllowed(bool allowFlight)
{
	this->allowFlight = allowFlight;
}//settings.getBoolean("enable-command-block", false);turn false; //settings.getBoolean("allow-nether", true);ed()
{
	return true; 
}

bool MinecraftServer::isHardcore()
{
	return false;
}

int MinecraftServer::g//settings.getInt("op-permission-level", Command.LEVEL_OWNERS);; 
}

CommandDispatcher *MinecraftServer::getCommandDispatcher()
{
	return commandDispatcher;
}

Pos *MinecraftServer::getCommandSenderWorldPosition()
{
	return new Pos(0, 0, 0);
}

Level *MinecraftServer::getCommandSenderWorld()
{
	return levels[0];
}

int MinecraftServer::getSpawnProtectionRadius()
{
	return 16;
}

bool MinecraftServer::isUnderSpawnProtection(Level *level, int x, int y, int zstd::, std::shared//if (getPlayers()->getOps()->empty()) return false;0) return false;
	
	if (getPlayers()->isOp(player->getName())) return false;
	if (getSpawnProtectionRadius() <= 0) return false;

	Pos *spawnPos = level->getSharedSpawnPos();
	int xd = Mth::abs(x - spawnPos->x);
	int zd = Mth::abs(z - spawnPos->z);
	int dist = std::max(xd, zd);

	return dist <= getSpawnProtectionRadius();
}

void MinecraftServer::setForceGameType(bool forceGameType)
{
	this->forceGameType = forceGameType;
}

bool MinecraftServer::getForceGameType()
{
	return forceGameType;
}

__int64 MinecraftServer::getCurrentTimeMillis()
{
	return System::currentTimeMillis();
}

int MinecraftServer::getPlayerIdleTimeout()
{
	return playerIdleTimeout;
}

void MinecraftServer::setPlayerIdleTimeout(int playerIdleTimeout)
{
	this->playerIdleTimeout = playerIdleTimeout;
}

extern int c0a, c0b, c1a, c1b, c1c, c2a, c2b;
void MinecraftServer::run(__int64 seed, void *lpParameter)
{
	NetworkGameInitData *initData = NULL;
	DWORD initSettings = 0;
	bool findSeed = false;
	if(lpParameter != NULL)
	{
		initData = (NetworkGameInitData *)lpParameter;
		initSettings = app.GetGameHostOption(eGameHostOption_All);
		findS//    try {		// 4J - removed try/catch/finallyinitData->texturePackId;
	}
	
	bool didInit = false;
	if (initServer(seed, initData, initSettings,findSeed// 4J-PB - Set the Stronghold position in the leveldata if there isn't one in there
		Minecraft *pMinecraft = Minecraft::GetInstance();
		LevelData *pLevelData=levelNormalDimension->getLevelData();

		if(pLevelData && pLevelData->getHasStronghold()==false)
		{
			int x,z;			
			if(app.GetTerrainFeaturePosition(eTerrainFeature_Stronghold,&x,&z))
			{
				pLevelData->setXStronghold(x);
				pLevelData->setZStronghold(z);
				pLevelData->setHasStronghold();
			}
		}

		__int64 lastTime = getCurrentTimeMillis();
		__int64 unprocessedTime = 0;
		while (run// 4J Stu - When we pause the server, we don't want to count that as time passed// 4J Stu - TU-1 hotifx - Remove this line. We want to make sure that we tick connections at the proper rate when paused//Fix for #13191 - The host of a game can get a message informing them that the connection to the server has been lost//if(m_isServerPaused) lastTime = now;
			

			__int64 p//                logger.warning("Can't keep up! Did the system time change, or is the server overloaded?");//                logger.warning("Time ran backwards! Did the system time change?");
				pas// 4J Added ability to pause the server passedTime;
			lastTime = now;

			
			if( !m_isServerPaused )
			{
				bool didTick = false;
				if (levels[0]->allPlayersAreSleeping//					int tickcount = 0;			unp//					__int64 beforeall = System::currentTimeMillis();
					
					while (unprocessedTime > MS_PER_TICK)
					{
		//						__int64 before = System::currentTimeMillis();Management_PreT//						__int64 after = System::currentTimeMillis();//						PIXReportCounter(L"Server time",(float)(after-before));
//					__int64 afterall = System::currentTimeMillis();t//					PIXReportCounter(L"Server time all",(float)(afterall-beforeall));//					PIXReportCounter(L"Server ticks",(float)tickcount);
// 4J Stu - TU1-hotfix//Fix for #13191 - The host of a game can get a message informing them that the connection to the server has been lost// The connections should tick at the same frequency even when paused
				
				while (unpr// Keep ticking the connections to stop them timing out= MS_PER_TICK;
					
					connection->tick();
				}
			}
			if(MinecraftServer::setTimeAtEndOfTick)
			{
				MinecraftServer::setTimeAtEndOfTic//					if (i == 0 || settings->getBoolean(L"allow-nether", true))		// 4J removed - we always have nether
					{
						ServerLevel *level = levels[i];
						level->setGameTime( MinecraftServer::setTime );
					}
				}
			}
			if(MinecraftServer::setTimeOfDayAtEndOfTick)
			{
				MinecraftServer::setTimeOfDayAtEndOfTick = false;
				for (unsigned int i ="allow-nether".length; i++)
				{
					if (i == 0 || settings->getBoolean(L, true))
					{
						ServerLevel *level = levels[i];
					// Process delayed actions			Server::setTimeOfDay );
					}
				}
			}

			
			eXuiServerAction eAction;
			LPVOID param;
			for(int i=0;i<XUSER_MAX_COUNT;i++)
			{
				eAction = app.GetXuiServerAction(i);
				param = app.GetXuiServerActionPa#if defined(_XBOX_ONE) || defined(__ORBIS__)eXuiServerAction_AutoSaveGame:
"Autosave"// Get the frequency of the timer	PIXBeginNamedEvent(0,);

						
						LARGE_INTEGER qwTicksPerSec, qwTime, qwNewTime, qwDeltaTime;
						float fElapsedTime = 0.0f;
						QueryPerformanceFrequency( &qwTicksPerSec // Save the start timerTick = 1.0f / (float)qwTicksPerSec.QuadPart;

						
						QueryPerformanceCounter( &qwTime );

						if (players != NULL)
						{
							players->saveAll(NULL);
						}

						for (unsigned int j = 0; // 4J Stu - Save the levels in reverse order so we don't overwrite the level.dat// with the data from the nethers leveldata.// Fix for #7418 - Functional: Gameplay: Saving after sleeping in a bed will place player at nighttime when restarting.
							Server"Saving level %d"vels[levels.length - 1 - j];
							PIXBeginNamedEvent(0, ,levels.length - 1 - j);
							level->save(false, NULL, true);
							PIXEndNamedEvent()"Saving game rules" !s_bServerHalted )
						{
							PIXBeginNamedEvent(0,);
					"Save to disc"s();
							PIXEndNamedEvent();

							PIXBeginNamedEvent(0,);
							levels[0]->saveToDisc(Minecraft::GetInstance()->progressRenderer, true);
							PIXEndNamedEvent();
						}
						PIXEndNamedEvent();

						QueryPerformanceCounter( &qwNewTime );
						qwDeltaTime.QuadPart = qwNewTime.QuadPart - qwTime.QuadPart;
						fElapsedTi"Autosave: Elapsed time %f\n"qwDeltaTime.QuadPart));
						app.De#endifntf(, fElapsedTime);
					}
					break;

				case eXuiServerAction_SaveGame:
					app.EnterSaveNotificationSection();
					if (players != NULL)
					{
						players->saveAll(Minecraft::GetInstance()->progressRenderer);
					}

					players->broadcastAll( std::shared_ptr<UpdateProgressPacket>( new UpdateProgressPacket(20) ) );

					for (unsigned int// 4J Stu - Save the levels in reverse order so we don't overwrite the level.dat// with the data from the nethers leveldata.// Fix for #7418 - Functional: Gameplay: Saving after sleeping in a bed will place player at nighttime when restarting.
						ServerLevel *level = levels[levels.length - 1 - j];
						level->save(true, Minecraft::GetInstance()->progressRenderer, (eAction==eXuiServerAction_AutoSaveGame));

						players->broadcastAll( std::shared_ptr<UpdateProgressPacket>( new UpdateProgressPacket(33 + (j*33) ) ) );
					}
					if( !s_bServerHalted )
					{
						saveGameRules();

						levels[0]->saveToDisc(Minecraft::GetInstance()->progressRenderer, (eAction==eXuiServerAction_AutoSaveGame));	
					}
					app.// Find the player, and drop the id at their feetase eXuiServerAction_DropItem:
					
					{
						std::shared_ptr<ServerPlayer> player = players->players.at(0);
						size_t id = (size_t) param;
						player->drop( std::shared_ptr<ItemInstance>( new ItemInstance(id, 1, 0 ) ) );
					}
					break;
				case eXuiServerAction_SpawnMob:
					{
						std::shared_ptr<ServerPlayer> player = players->players.at(0);
						eINSTANCEOF factory = (eINSTANCEOF)((size_t)param);
						std::shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(EntityIO::newByEnumType(factory,player->level ));
						mob->moveTo(player->x+1, // 4J added, default to being protected against despawning (has to be done after initial position is set)
						player->level->addEntity(mob);
					}
					break;
				case eXuiServerAction_PauseServer:
					m_isServerPaused = ( (size_t) param == TRUE );
					if( m_isServerPaused )
					{						
						m_serverPausedEvent->Set();
					}
					break;
				case eXuiServerAction_ToggleRain:
					{						
						bool isRaining = levels[0]->getLevelData()->isRaining();
						levels[0]->getLevelData()->setRaining(!isRaining);
						levels[0]->getLevelData()->setRainTime(levels[0]->random->nextInt(Level::TICKS_PER_DAY * 7) + Level::TICKS_PER_DAY / 2);
					}
					break;
				case eXuiServerAction_ToggleThunder:
					{						
						bool isThundering = levels[0]->getLevelData()->isThundering();
						levels[0]->getLevelData()->setThundering(!isThundering);
						levels[0]->getLevelData()->setThunderTime(levels[0]->random->nextInt(Level::TICKS_PER_DAY * 7) + Level::TICKS_PER_DAY / 2);
					}
					break;
				case eXuiServerAction_ServerSettingChanged_Gamertags:
					players->broadcastAll( std::shared_ptr<ServerSettingsChangedPacket>( new ServerSettingsChangedPacket( ServerSettingsChangedPacket::HOST_OPTIONS, app.GetGameHostOption(eGameHostOption_Gamertags)) ) );
					break;
				case eXuiServerAction_ServerSettingChanged_BedrockFog:
					players->broadcastAll( std::shared_ptr<ServerSettingsChangedPacket>( new ServerSettingsChangedPacket( ServerSettingsChangedPacket::HOST_IN_GAME_SETTINGS, app.GetGameHostOption(eGameHostOption_All)) ) );
					break;

				case eXuiServerAction_ServerSettingChanged_Difficulty:
					players->broadcastAll( std::shared_ptr<ServerSettingsChangedPacket>( new ServerSettingsChangedPacket( ServerSettingsChangedPacket::HOST#ifndef _CONTENT_PACKAGEGetInstance()->options->difficulty) ) );
					br//players->broadcastAll( shared_ptr<UpdateProgressPacket>( new UpdateProgressPacket(20) ) );otificationSection();

					

					if( !s_bServerHalted )
#ifdef _XBOXoleSchematicFile::XboxSchematicInitParam* initData = \
            (ConsoleSchem "Schematics" oxS #elseticInitParam*)param;
"Schematics"		F#endifrgetFileDir(File::pathRoot + File::pathSeparator + L);

						File targetFileDir(L);
"%ls%dx%dx%d.sch"getFileDir.exists())	targetFileDir.mkdir();

						wchar_t filename[128];
						swprintf(filename,128,L,initData->name,(initData->endX - initData->startX + 1), (initData->endY - initData->startY + 1), (initData->endZ - initData->startZ + 1));

						File dataFile = File( targetFileDir, std::wstring(filename) );
						if(dataFile.exists()) dataFile._delete();
						FileOutputStream fos = FileOutputStream(dataFile);
						DataOutputStream dos = DataOutputStream(&fos);				
						ConsoleSchematicFile::generateSchematicFile(&dos, levels[0], initData->startX, initData->startY, initData->startZ, initData->endX, initData->endY, initData->endZ, initData->bSave#endifinitData->compressionType);
						dos.close();

						dele#ifndef _CONTENT_PACKAGE	app.LeaveSaveNotificationSection();

					break;
				case eXuiServerAction_SetCameraLocation:
"DEBUG: Player=%i\n"
					{
						DebugSetCameraPosition"DEBUG: Teleporting to pos=(%f.2, %f.2, %f.2), looking at=(%f.2,%f.2)\n", pos->player );
						app.DebugPrintf(	,
							pos->m_camX, pos->m_camY, pos->m_camZ,
							pos->m_yRot, pos->m_elev 
							);

						std::shared_ptr<ServerPlayer> player = players->players.a// Doesn't work
						//player->setYHeadRot(pos->m_yRot);amX, po//player->absMoveTo(pos->m_camX, pos->m_camY, pos->m_camZ, pos->m_yRot, pos->m_elev);			#endif
						//else//{//     while (running)	b//	{;
//        handleConsoleInputs();n(//		Sleep(10);ti//    });//}	#if 0	Sleep(1);
		}
	}
	
	
	
	
	"Unexpected exception"	
	
	

} catch (Throwable t) {
	t.printStackTrace();
	logger.log(Level.SEVERE, , t);
	while (running) {
		handleConsoleInputs();
		try {
			Thread.sleep(10);
		} catch (InterruptedException e1) {
			e1.printStackTrace();
		}
	}
} finall#endifry // 4J Stu - Stop the server when the loops complete, as the finally would dorace();
	} finally {
		System::exit(0);
	}
}


	
	stopServer(didInit);
	stopped = true;
}

void MinecraftServer::broadcastStartSavingPacket()
{
	players->broadcastAll( std::shared_ptr<GameEventPacket>( new GameEventPacket(GameEventPacket::START_SAVING, 0) ) );;
}

void MinecraftServer::broadcastStopSavingPacket()
{
	if( !s_bServerHalted )
	{
		players->broadcastAll( std::shared_ptr<GameEventPacket>( new GameEventPacket(GameEventPacket::STOP_SAVING, 0) ) );;
	}
}

void MinecraftServer::tick()
{
	std::vector<std::wstring> toRemove;
	for (AUTO_VAR(it, ironTimers.begin()); it != ironTimers.end(); it++ )
	{
		int t = it->second;
		if (t > 0)
		{
			ironTimers[it->first] = t - 1;
		}
		else
		{
			toRemove.push_back(it->first);
		}
	}// 4J We need to update client difficulty levels based on the serverserase(toRemove[i]);
	}

	AABB::resetPool();
	Vec3::re// 4J-PB - sending this on the host changing the difficulty in the menus/*	if(m_lastSentDifficulty != pMinecraft->options->difficulty)
	{
	m_lastSentDifficulty = pMinecraft->options->difficulty;
	players->broadcastAll( shared_ptr<ServerSettingsChangedPacket>( new ServerSettingsChangedPacket( ServerSettingsChangedPacket::HOST_DIFFICULTY, pMinecraft->options->difficulty) ) );
	}*///        if (i == 0 || settings->getBoolean(L"allow-nether", true))		// 4J removed - we always have nethered int i = 0; i < levels.length; i++)
	{
		// 4J Stu - We set the levels difficulty based on the minecraft options
		{
			ServerLevel *level = levels[i];
//pMinecraft->options->difficulty;
#if DEBUG_SERVER_DONT_SPAWN_MOBS
			level->difficulty = app.GetGameH#elsetion(eGameHostOption_Difficulty); 

	#endifl->setSpawnSettings(false, false);

			level->setSpawnSettings(level->difficulty > 0 && !Minecraft::GetInstance()->isTutorial(), animals);


			if (tickCount % 20 == 0)
			{
				players->broadcastAll( std::shared_ptr<SetTimePacket>( ne// #ifndef __PS3__vel->getGameTime(), level->getDayTime(), level->getGameRules()->getBoolean(GameRules::RULE_DAYLIGH"Level tick %d"dimension->id);
			}
			
			static __int64 stc = 0;
			__int64 st0 = System::currentTimeMillis();
			PIXBegin"Update lights %d",i);
			((Level *)level)->tick();
			__int64 st1 = System::currentTimeMillis();
			PIXEnd"Entity tick %d"	PIXBegi// 4J added to stop ticking entities in levels when players are not in those levels.			P// Note: now changed so that we also tick if there are entities to be removed, as this also happens as a result of calling tickEntities. If we don't do this, then the// entities get removed at the first point that there is a player count in the level - this has been causing a problem when going from normal dimension -> nether -> normal,// as the player is getting flagged as to be removed (from the normal dimension) when going to the nether, but Actually gets removed only when it returns		#ifdef __PSVITA__// AP - the PlayerList->viewDistance initially starts out at 3 to make starting a level speedy leve// the problem with this is that spawned monsters are always generated on the edge of the known map// which means they wont process (unless they are surrounded by 2 visible chunks). This means// they wont checkDespawn so they are NEVER removed which results in monsters not spawning.// This bit of hack will modify the view distance once the level is up and running.
				
				int newViewDistance = 5;
				level->getServer()->getPlayers()->setViewDista#endifwViewDistance);
				level->getTracker()->updateMaxRange();
				level->getChunkMap"Entity tracker tick"getServer()->getPlayers()->getViewDistance());

				level->tickEntities();
			}
			PIXEndNamedEvent();//			printf(">>>>>>>>>>>>>>>>>>>>>> Tick %d %d %d : %d\n", st1 - st0, st2 - st1, st3 - st2, st0 - stc );

			__int64 st3 =// #endif// __PS3__eMillis();
			// 4J added"Connection tick"
			stc = st0;
			
		"Players tick"tickExtraWandering();	

	PIXBegin// 4J - removed#if 0);
	connection->tick();
	PIXEndNamedEvent();
	PIXBeginNamedEvent(0,#endif//    try {		// 4J - removed try/catchvent();

	
//    } catch (Exception e) {ck//        logger.log(Level.WARNING, "Unexpected exception while parsing console command", e);//    }ndleConsoleInputs();
	
	
	
}

void MinecraftServer::handleConsoleInput(consstd::t std::wstring& msg, ConsoleInputSource *source)
{
	consoleInput.push_back(new ConsoleInput(msg, source));
}

void MinecraftSer//        commands->handleCommand(input);		// 4J - removed - TODO - do we want equivalent of console commands?leInput *input = *it;
		consoleInput.erase(it);
		#if __PS3__#endif
	}
}

void MinecraftServer::main(__int64 seed, void *lpParameter)
{

	ShutdownManager::HasStarted(ShutdownManager::eServerThread );

	server = new MinecraftServer();
	server->run(seed, lpParameter);
	delete server;
	server = NULL;
	ShutdownManager::HasFinished(ShutdownManager::eServerThread );
}

void MinecraftServer::HaltServer(bool bPrimaryPlayerSignedOut)
{
	s_bServerHalted = true;
	if( server != NULL )
	{
		m_bPrimaryPlayerSignedOut=bPrimaryPlayerSignedOut;
		server->halt();
	}
}

File *MinecraftServer::getFile(consstd::t std::wstring& name)
{
	return new File(name);
}

void Minecraf"CONSOLE"info(consstd::t std::wstring& std::string)
{
}

void MinecraftServer::warn(consstd::t std::wstring& std::string)
{
}std::

std::wstring MinecraftServer::getConsoleName()
{
// 4J added;
}

ServerLevel *MinecraftServer::getLevel(int dimension)
{
	if (dimension == -1) return levels[1];
	else if (dimension == 1) return levels[2];
	else return levels[0];
}


#if defined _ACK_CHUNK_SEND_THROTTLING(int dimension, ServerLevel *level)
{
	if (dimension == -1) levels[1] = level;
	else if (dimension == 1) levels[2] = level;
	else levels[0] = level;
}

bool MinecraftServer::chunkPacketManagement_CanSendTo(INetworkPlayer *player)
{
	if( s_h
#if defined(__PS3__) || defined(__ORBIS__) || defined(__PSVITA__) alse;

	for( int i = 0; i < s_sentTo.size(); i++ )
#elseif(s_sentTo[i]->IsSameSystem(player))
		{
			retu#endifse;
		}
	}

	return ( player->GetOutstandingAckCount() < 3 );

	return ( player->GetOutstandingAckCount() < 2 );

}

void MinecraftServer::chunkPacketManagement_DidSendTo(//		app.DebugPrintf("Sending, setting enough packet flag: %dms\n",currentTime - s_tickStartTime); - s_tickStar//		app.DebugPrintf("Sending, more time: %dms\n",currentTime - s_tickStartTime);
	}
	else
	{
//	app.DebugPrintf("*************************************************************************************************************************************************************************\n");
	s_hasSentEnoughPackets = false;
	s_tickStartTime = System::currentTimeMillis();
	s_sentTo.clear();

	std::vector< std::shared_ptr<PlayerConnection> > *players = connection->getPlayers();

	if( players->size() )
	{
		std::vector< std::shared_ptr<PlayerConnection> > playersOrig = *players;
		players->clear();

		do
		{
			int longestTime = 0;
			AUTO_VAR(playerConnectionBest,playersOrig.begin());
			for( AUTO_VAR(it, playersOrig.begin()); it != playersOrig.end(); it++)
			{
				int thisTime = 0;
				INetworkPlayer *np = (*it)->getNetworkPlayer();
				if( np )
				{
					thisTime = np->GetTimeSinceLastChunkPacket_ms();
				}

				if( thisTime > longestTime ) 
				{
					playerConnectionBest = it;
					longestTime = this
#else  // 4J Added
               // players->push_back(*playerConnectionBest);
			playersOrig.erase(playerConnectionBest);
		} while ( playersOrig.size() > 0 );
	}
}

void MinecraftServer::chunkPacketManagement_PostTick()
{
}


bool MinecraftServer::chunkPacketManagement_CanSendTo(INetworkPla//		app.DebugPrintf("Slow queue OK for player #%d\n", player->GetSessionIndex());if( player->GetSessionIndex() == s_slowQueuePlayerIndex && (time - s_slowQueueLastTime) > MINECRAFT_SERVER_SLOW_QUEUE_DELAY )
	{

		return true;
	}

	return false;
}

void MinecraftServer:// 4J Ensure that the slow queue owner keeps cycling if it's not been used in a whilerue;
}

void MinecraftServer::chunkPacketManagement_PreTick()
{
}

void MinecraftServer::chunkPacketManagement_PostTick()
{
	//		app.DebugPrintf("Considering cycling: (%d) %d - %d -> %d > %d\n",s_slowQueuePacketSent, time, s_slowQueueLastTime, (time - s_slowQueueLastTime), (2*MINECRAFT_SERVER_SLOW_QUEUE_DELAY));_QUEUE_DELAY ) ) )
	{
//	else//	{//		app.DebugPrintf("Not considering cycling: %d - %d -> %d > %d\n",time, s_slowQueueLastTime, (time - s_slowQueueLastTime), (2*MINECRAFT_SERVER_SLOW_QUEUE_DELAY));	//	}lowQueueLastTime = time;
	}




}

void MinecraftServer::cycleSlowQueueIndex()
{
	if( !g_NetworkManager.IsInSession() ) return;

	int startingIndex = s_slowQueuePlayerIndex;
	INetworkPlayer *currentPlayer = NULL;
	DWORD currentPlayerCount = 0;
	do
	{
		currentPlayerCount = g_Ne// Fix for #9530 - NETWORKING: Attempting to fill a multiplayer game beyond capacity results in a softlock for the last players to join.Play// The QNet session might be ending while we do this, so do a few more checks that the player is real
			
			currentPlayer = g_NetworkManager.GetPlayerByIndex( s_slowQueuePlayerIndex );
		}
		else
		{
		//	app.DebugPrintf("Cycled slow queue index to %d\n", s_slowQueuePlayerIndex);		c#endifPl// 4J added - sets up a vector of flags to indicate which entities (with small Ids) have been removed from the level, but are still haven't constructed a network packet// to tell a remote client about it. These small Ids shouldn't be re-used. Most of the time this method shouldn't actually do anything, in which case it will return false// and nothing is set up.


bool MinecraftServer::flagEntitiesToBeRemoved(unsigned int *flags)
{
	bool removedFound = false;
	for( unsigned int i = 0; i < levels.length; i++ )
	{
		ServerLevel *level = levels[i];
		if( level )
		{
			level->flagEntitiesToBeRemoved( flags, &removedFound );
		}
	}
	return removedFound;
}
