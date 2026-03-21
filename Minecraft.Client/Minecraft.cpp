#include "Platform/stdafx.h"
#include "Minecraft.h"
#include "GameState/GameMode.h"
#include "Utils/Timer.h"
#include "Rendering/EntityRenderers/ProgressRenderer.h"
#include "Rendering/LevelRenderer.h"
#include "Rendering/Particles/ParticleEngine.h"
#include "Player/MultiPlayerLocalPlayer.h"
#include "Player/User.h"
#include "Textures/Textures.h"
#include "Rendering/GameRenderer.h"
#include "Rendering/Models/HumanoidModel.h"
#include "GameState/Options.h"
#include "Textures/Packs/TexturePackRepository.h"
#include "GameState/StatsCounter.h"
#include "Rendering/EntityRenderers/EntityRenderDispatcher.h"
#include "Rendering/EntityRenderers/TileEntityRenderDispatcher.h"
#include "GameState/SurvivalMode.h"
#include "Rendering/Chunk.h"
#include "GameState/CreativeMode.h"
#include "Level/DemoLevel.h"
#include "Level/MultiPlayerLevel.h"
#include "Player/MultiPlayerLocalPlayer.h"
#include "GameState/DemoUser.h"
#include "Rendering/Particles/GuiParticles.h"
#include "UI/Screen.h"
#include "UI/Screens/DeathScreen.h"
#include "UI/Screens/ErrorScreen.h"
#include "UI/Screens/TitleScreen.h"
#include "UI/Screens/InventoryScreen.h"
#include "UI/Screens/InBedChatScreen.h"
#include "UI/Screens/AchievementPopup.h"
#include "Input/Input.h"
#include "Rendering/FrustumCuller.h"
#include "Rendering/Camera.h"

#include "..\Minecraft.World\MobEffect.h"
#include "..\Minecraft.World\Difficulty.h"
#include "..\Minecraft.World\net.minecraft.world.level.h"
#include "..\Minecraft.World\net.minecraft.world.entity.h"
#include "..\Minecraft.World\net.minecraft.world.entity.player.h"
#include "..\Minecraft.World\net.minecraft.world.entity.item.h"
#include "..\Minecraft.World\net.minecraft.world.phys.h"
#include "..\Minecraft.World\File.h"
#include "..\Minecraft.World\net.minecraft.world.level.storage.h"
#include "..\Minecraft.World\net.minecraft.h"
#include "..\Minecraft.World\net.minecraft.stats.h"
#include "..\Minecraft.World\System.h"
#include "..\Minecraft.World\ByteBuffer.h"
#include "..\Minecraft.World\net.minecraft.world.level.tile.h"
#include "..\Minecraft.World\net.minecraft.world.level.chunk.h"
#include "..\Minecraft.World\net.minecraft.world.level.dimension.h"
#include "..\Minecraft.World\net.minecraft.world.item.h"
#include "..\Minecraft.World\Minecraft.World.h"
#include "Network/ClientConnection.h"
#include "..\Minecraft.World\HellRandomLevelSource.h"
#include "..\Minecraft.World\net.minecraft.world.entity.animal.h"
#include "..\Minecraft.World\net.minecraft.world.entity.monster.h"
#include "..\Minecraft.World\StrongholdFeature.h"
#include "..\Minecraft.World\IntCache.h"
#include "..\Minecraft.World\Villager.h"
#include "..\Minecraft.World\SparseLightStorage.h"
#include "..\Minecraft.World\SparseDataStorage.h"
#include "..\Minecraft.World\ChestTileEntity.h"
#include "Textures/TextureManager.h"
#ifdef _XBOX
#include "Xbox\Network\NetworkPlayerXbox.h"
#endif
#include "Common\UI\IUIScene_CreativeMenu.h"
#include "Common\UI\UIFontData.h"
#include "Textures/Packs/DLCTexturePack.h"

#ifdef __ORBIS__
#include "Orbis\Network\PsPlusUpsellWrapper_Orbis.h"
#endif

// #define DISABLE_SPU_CODE
// 4J Turning this on will change the graph at the bottom of the debug overlay
// to show the number of packets of each type added per fram
// #define DEBUG_RENDER_SHOWS_PACKETS 1
// #define SPLITSCREEN_TEST

// If not disabled, this creates an event queue on a seperate thread so that the
// Level::tick calls can be offloaded from the main thread, and have longer to
// run, since it's called at 20Hz instead of 60
#define DISABLE_LEVELTICK_THREAD

Minecraft* Minecraft::m_instance = NULL;
__int64 Minecraft::frameTimes[512];
__int64 Minecraft::tickTimes[512];
int Minecraft::frameTimePos = 0;
__int64 Minecraft::warezTime = 0;
File Minecraft::workDir = File(L"");

#ifdef __PSVITA__

TOUCHSCREENRECT QuickSelectRect[3] = {
    {560, 890, 1360, 980},
    {450, 840, 1449, 960},
    {320, 840, 1600, 970},
};

int QuickSelectBoxWidth[3] = {89, 111, 142};

// 4J - TomK ToDo: these really shouldn't be magic numbers, it should read the
// hud position from flash.
int iToolTipOffset = 85;

#endif

ResourceLocation Minecraft::DEFAULT_FONT_LOCATION =
    ResourceLocation(TN_DEFAULT_FONT);
ResourceLocation Minecraft::ALT_FONT_LOCATION = ResourceLocation(TN_ALT_FONT);

Minecraft::Minecraft(Component* mouseComponent, Canvas* parent,
                     MinecraftApplet* minecraftApplet, int width, int height,
                     bool fullscreen) {
    // 4J - added this block of initialisers
    gameMode = NULL;
    hasCrashed = false;
    timer = new Timer(SharedConstants::TICKS_PER_SECOND);
    oldLevel = NULL;  // 4J Stu added
    level = NULL;
    levels = MultiPlayerLevelArray(3);  // 4J Added
    levelRenderer = NULL;
    player = nullptr;
    cameraTargetPlayer = nullptr;
    particleEngine = NULL;
    user = NULL;
    parent = NULL;
    pause = false;
    textures = NULL;
    font = NULL;
    screen = NULL;
    localPlayerIdx = 0;
    rightClickDelay = 0;

    // 4J Stu Added
    InitializeCriticalSection(&ProgressRenderer::s_progress);
    InitializeCriticalSection(&m_setLevelCS);
    // m_hPlayerRespawned = CreateEvent(NULL, FALSE, FALSE, NULL);

    progressRenderer = NULL;
    gameRenderer = NULL;
    bgLoader = NULL;

    ticks = 0;
    // 4J-PB - moved into the local player
    // missTime = 0;
    // lastClickTick = 0;
    // isRaining = false;
    // 4J-PB - end

    orgWidth = orgHeight = 0;
    achievementPopup = new AchievementPopup(this);
    gui = NULL;
    noRender = false;
    humanoidModel = new HumanoidModel(0);
    hitResult = 0;
    options = NULL;
    soundEngine = new SoundEngine();
    mouseHandler = NULL;
    skins = NULL;
    workingDirectory = File(L"");
    levelSource = NULL;
    stats[0] = NULL;
    stats[1] = NULL;
    stats[2] = NULL;
    stats[3] = NULL;
    connectToPort = 0;
    workDir = File(L"");
    // 4J removed
    // wasDown = false;
    lastTimer = -1;

    // 4J removed
    // lastTickTime = System::currentTimeMillis();
    recheckPlayerIn = 0;
    running = true;
    unoccupiedQuadrant = -1;

    Stats::init();

    orgHeight = height;
    this->fullscreen = fullscreen;
    this->minecraftApplet = NULL;

    this->parent = parent;
    // 4J - Our actual physical frame buffer is always 1280x720 ie in a 16:9
    // ratio. If we want to do a 4:3 mode, we are telling the original minecraft
    // code that the width is 3/4 what it actually is, to correctly present a
    // 4:3 image. Have added width_phys and height_phys for any code we add that
    // requires to know the real physical dimensions of the frame buffer.
    if (RenderManager.IsWidescreen()) {
        this->width = width;
    } else {
        this->width = (width * 3) / 4;
    }
    this->height = height;
    this->width_phys = width;
    this->height_phys = height;

    this->fullscreen = fullscreen;

    appletMode = false;

    Minecraft::m_instance = this;
    TextureManager::createInstance();

    for (int i = 0; i < XUSER_MAX_COUNT; i++) {
        m_pendingLocalConnections[i] = NULL;
        m_connectionFailed[i] = false;
        localgameModes[i] = NULL;
    }

    animateTickLevel = NULL;   // 4J added
    m_inFullTutorialBits = 0;  // 4J Added
    reloadTextures = false;

    // initialise the audio before any textures are loaded - to avoid the
    // problem in win64 of the Miles audio causing the codec for textures to be
    // unloaded

    // 4J-PB - Removed it from here on Orbis due to it causing a crash with the
    // network init. We should work out why...
#ifndef __ORBIS__
    this->soundEngine->init(NULL);
#endif

#ifndef DISABLE_LEVELTICK_THREAD
    levelTickEventQueue =
        new C4JThread::EventQueue(levelTickUpdateFunc, levelTickThreadInitFunc,
                                  "LevelTick_EventQueuePoll");
    levelTickEventQueue->setProcessor(3);
    levelTickEventQueue->setPriority(THREAD_PRIORITY_NORMAL);
#endif  // DISABLE_LEVELTICK_THREAD
}

void Minecraft::clearConnectionFailed() {
    for (int i = 0; i < XUSER_MAX_COUNT; i++) {
        m_connectionFailed[i] = false;
        m_connectionFailedReason[i] = DisconnectPacket::eDisconnect_None;
    }
    app.SetDisconnectReason(DisconnectPacket::eDisconnect_None);
}

void Minecraft::connectTo(const std::wstring& server, int port) {
    connectToIp = server;
    connectToPort = port;
}

void Minecraft::init() {
#if 0  // 4J - removed
	if (parent != null)
	{
		Graphics g = parent.getGraphics();
		if (g != null) {
			g.setColor(Color.BLACK);
			g.fillRect(0, 0, width, height);
			g.dispose();
		}
		Display.setParent(parent);
	} else {
		if (fullscreen) {
			Display.setFullscreen(true);
			width = Display.getDisplayMode().getWidth();
			height = Display.getDisplayMode().getHeight();
			if (width <= 0) width = 1;
			if (height <= 0) height = 1;
		} else {
			Display.setDisplayMode(new DisplayMode(width, height));
		}
	}

	Display.setTitle("Minecraft " + VERSION_STRING);
	try {
		Display.create();
		/*
		* System.out.println("LWJGL version: " + Sys.getVersion());
		* System.out.println("GL RENDERER: " +
		* GL11.glGetString(GL11.GL_RENDERER));
		* System.out.println("GL VENDOR: " +
		* GL11.glGetString(GL11.GL_VENDOR));
		* System.out.println("GL VERSION: " +
		* GL11.glGetString(GL11.GL_VERSION)); ContextCapabilities caps =
		* GLContext.getCapabilities(); System.out.println("OpenGL 3.0: " +
		* caps.OpenGL30); System.out.println("OpenGL 3.1: " +
		* caps.OpenGL31); System.out.println("OpenGL 3.2: " +
		* caps.OpenGL32); System.out.println("ARB_compatibility: " +
		* caps.GL_ARB_compatibility); if (caps.OpenGL32) { IntBuffer buffer
		* = ByteBuffer.allocateDirect(16 *
		* 4).order(ByteOrder.nativeOrder()).asIntBuffer();
		* GL11.glGetInteger(GL32.GL_CONTEXT_PROFILE_MASK, buffer); int
		* profileMask = buffer.get(0); System.out.println("PROFILE MASK: "
		* + Integer.toBinaryString(profileMask));
		* System.out.println("CORE PROFILE: " + ((profileMask &
		* GL32.GL_CONTEXT_CORE_PROFILE_BIT) != 0));
		* System.out.println("COMPATIBILITY PROFILE: " + ((profileMask &
		* GL32.GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0)); }
		*/
	} catch (LWJGLException e) {
		// This COULD be because of a bug! A delay followed by a new attempt
		// is supposed getWorkingDirectoryto fix it.
		e.printStackTrace();
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e1) {
		}
		Display.create();
	}

	if (Minecraft.FLYBY_MODE) {
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
#endif

    // glClearColor(0.2f, 0.2f, 0.2f, 1);

    workingDirectory = File(L"");  // getWorkingDirectory();
    levelSource =
        new McRegionLevelStorageSource(File(workingDirectory, L"saves"));
    //        levelSource = new MemoryLevelStorageSource();
    options = new Options(this, workingDirectory);
    skins = new TexturePackRepository(workingDirectory, this);
    skins->addDebugPacks();
    textures = new Textures(skins, options);
    // renderLoadingScreen();

    font =
        new Font(options, L"font/Default.png", textures, false,
                 &DEFAULT_FONT_LOCATION, 23, 20, 8, 8, SFontData::Codepoints);
    altFont = new Font(options, L"font/alternate.png", textures, false,
                       &ALT_FONT_LOCATION, 16, 16, 8, 8);

    // if (options.languageCode != null) {
    //	Language.getInstance().loadLanguage(options.languageCode);
    //	//
    //font.setEnforceUnicodeSheet("true".equalsIgnoreCase(I18n.get("language.enforceUnicode")));
    //	font.setEnforceUnicodeSheet(Language.getInstance().isSelectedLanguageIsUnicode());
    //	font.setBidirectional(Language.isBidirectional(options.languageCode));
    // }

    // 4J Stu - Not using these any more
    // WaterColor::init(textures->loadTexturePixels(L"misc/watercolor.png"));
    // GrassColor::init(textures->loadTexturePixels(L"misc/grasscolor.png"));
    // FoliageColor::init(textures->loadTexturePixels(L"misc/foliagecolor.png"));

    gameRenderer = new GameRenderer(this);
    EntityRenderDispatcher::instance->itemInHandRenderer =
        new ItemInHandRenderer(this, false);

    for (int i = 0; i < 4; ++i) stats[i] = new StatsCounter();

    /*		4J - TODO, 4J-JEV: Unnecessary.
    Achievements::openInventory->setDescFormatter(NULL);
    Achievements.openInventory.setDescFormatter(new DescFormatter(){
    public String format(String i18nValue) {
    return String.format(i18nValue, Keyboard.getKeyName(options.keyBuild.key));
    }
    });
    */

    // 4J-PB - We'll do this in a xui intro
    // renderLoadingScreen();

    // Keyboard::create();
    Mouse::create();
#if 0  // 4J - removed
	mouseHandler = new MouseHandler(parent);
	try {
		Controllers.create();
	} catch (Exception e) {
		e.printStackTrace();
	}
#endif

    MemSect(31);
    checkGlError(L"Pre startup");
    MemSect(0);

    // width = Display.getDisplayMode().getWidth();
    // height = Display.getDisplayMode().getHeight();

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);
    glCullFace(GL_BACK);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    MemSect(31);
    checkGlError(L"Startup");
    MemSect(0);

    //    openGLCapabilities = new OpenGLCapabilities();	// 4J - removed

    levelRenderer = new LevelRenderer(this, textures);
    // textures->register(&TextureAtlas::LOCATION_BLOCKS, new
    // TextureAtlas(Icon::TYPE_TERRAIN, TN_TERRAIN));
    // textures->register(&TextureAtlas::LOCATION_ITEMS, new
    // TextureAtlas(Icon::TYPE_ITEM, TN_GUI_ITEMS));
    textures->stitch();

    glViewport(0, 0, width, height);

    particleEngine = new ParticleEngine(level, textures);

    MemSect(31);
    checkGlError(L"Post startup");
    MemSect(0);
    gui = new Gui(this);

    if (connectToIp != L"")  // 4J - was NULL comparison
    {
        //        setScreen(new ConnectScreen(this, connectToIp,
        //        connectToPort));		// 4J TODO - put back in
    } else {
        setScreen(new TitleScreen());
    }
    progressRenderer = new ProgressRenderer(this);

    RenderManager.CBuffLockStaticCreations();
}

void Minecraft::renderLoadingScreen() {
    // 4J Unused
    // testing stuff on vita just now
#ifdef __PSVITA__
    ScreenSizeCalculator ssc(options, width, height);

    // xxx
    RenderManager.StartFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (float)ssc.rawWidth, (float)ssc.rawHeight, 0, 1000, 3000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -2000);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);

    Tesselator* t = Tesselator::getInstance();

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    // xxx
    glBindTexture(GL_TEXTURE_2D, textures->loadTexture(TN_MOB_PIG));
    t->begin();
    t->color(0xffffff);
    t->vertexUV((float)(0), (float)(height), (float)(0), (float)(0),
                (float)(0));
    t->vertexUV((float)(width), (float)(height), (float)(0), (float)(0),
                (float)(0));
    t->vertexUV((float)(width), (float)(0), (float)(0), (float)(0), (float)(0));
    t->vertexUV((float)(0), (float)(0), (float)(0), (float)(0), (float)(0));
    t->end();

    int lw = 256;
    int lh = 256;
    glColor4f(1, 1, 1, 1);
    t->color(0xffffff);
    blit((ssc.getWidth() - lw) / 2, (ssc.getHeight() - lh) / 2, 0, 0, lw, lh);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.1f);

    Display::swapBuffers();
    // xxx
    RenderManager.Present();
#endif
}

void Minecraft::blit(int x, int y, int sx, int sy, int w, int h) {
    float us = 1 / 256.0f;
    float vs = 1 / 256.0f;
    Tesselator* t = Tesselator::getInstance();
    t->begin();
    t->vertexUV((float)(x + 0), (float)(y + h), (float)(0),
                (float)((sx + 0) * us), (float)((sy + h) * vs));
    t->vertexUV((float)(x + w), (float)(y + h), (float)(0),
                (float)((sx + w) * us), (float)((sy + h) * vs));
    t->vertexUV((float)(x + w), (float)(y + 0), (float)(0),
                (float)((sx + w) * us), (float)((sy + 0) * vs));
    t->vertexUV((float)(x + 0), (float)(y + 0), (float)(0),
                (float)((sx + 0) * us), (float)((sy + 0) * vs));
    t->end();
}

LevelStorageSource* Minecraft::getLevelSource() { return levelSource; }

void Minecraft::setScreen(Screen* screen) {
    if (this->screen != NULL) {
        this->screen->removed();
    }

    // 4J Gordon: Do not force a stats save here
    /*if (dynamic_cast<TitleScreen *>(screen)!=NULL)
    {
    stats->forceSend();
    }
    stats->forceSave();*/

    if (screen == NULL && level == NULL) {
        screen = new TitleScreen();
    } else if (player != NULL && !ui.GetMenuDisplayed(player->GetXboxPad()) &&
               player->getHealth() <= 0) {
        // screen = new DeathScreen();

        // 4J Stu - If we exit from the death screen then we are saved as being
        // dead. In the Java game when you load the game you are still dead, but
        // this is silly so only show the dead screen if we have died during
        // gameplay
        if (ticks == 0) {
            player->respawn();
        } else {
            ui.NavigateToScene(player->GetXboxPad(), eUIScene_DeathMenu, NULL);
        }
    }

    if (dynamic_cast<TitleScreen*>(screen) != NULL) {
        options->renderDebug = false;
        gui->clearMessages();
    }

    this->screen = screen;
    if (screen != NULL) {
        //        releaseMouse();	// 4J - removed
        ScreenSizeCalculator ssc(options, width, height);
        int screenWidth = ssc.getWidth();
        int screenHeight = ssc.getHeight();
        screen->init(this, screenWidth, screenHeight);
        noRender = false;
    } else {
        //        grabMouse();	// 4J - removed
    }

    // 4J-PB - if a screen has been set, go into menu mode
    // it's possible that player doesn't exist here yet
    /*if(screen!=NULL)
    {
    if(player && player->GetXboxPad()!=-1)
    {
    InputManager.SetMenuDisplayed(player->GetXboxPad(),true);
    }
    else
    {
    // set all
    //InputManager.SetMenuDisplayed(XUSER_INDEX_ANY,true);
    }
    }
    else
    {
    if(player && player->GetXboxPad()!=-1)
    {
    InputManager.SetMenuDisplayed(player->GetXboxPad(),false);
    }
    else
    {
    //InputManager.SetMenuDisplayed(XUSER_INDEX_ANY,false);
    }
    }*/
}

void Minecraft::checkGlError(const std::wstring& std::string// 4J - TODO     
}

void Minecraft::destroy(//4J Gordon: Do not force a stats save here  /*stats->forceSend();
	stats->forceSave();*/   //    try {     
	setLevel(NUL//    } catch (Throwable e) {  //    }   //    try {     
	MemoryTracker::release//    } catch (Throwable e) {  //    }     

	soundEngine->destroy//} finally {     
	Display::destroy//    if (!hasCrashed) System.exit(0);	//4J - removed  //}
// System.gc();	// 4J - removed    // 4J-PB - splitting this function into 3
// parts, so we can call the middle part from our xbox game loop 
#if 0     
void Minecraft::run()
{
	running = tr//    try {	// 4J - removed try/catch     
	init//    } catch (Exception e) {  //        e.printStackTrace();  //       crash(new CrashReport("Failed to start game", e));  //        return;  //    }  //    try {	// 4J - removed try/catch     
	if (Minecraft::FLYBY_MODE)
	{
		generateFlyby();
		return;
	}

	__int64 lastTime = System::currentTimeMillis();
	int frames = 0;

	while (running)
//        try {	// 4J - removed try/catch   //            if (minecraftApplet != null && !minecraftApplet.isActive()) break;	// 4J - removed     
		AABB::resetPool();
		Vec3::resetPool()//            if (parent == NULL && Display.isCloseRequested()) {		// 4J - removed   //                stop();   //            }     

		if (pause && level != NULL)
		{
			float lastA = timer->a;
			timer->advanceTime();
			timer->a = lastA;
		}
		else
		{
			timer->advanceTime();
		}

		__int64 beforeTickTime = System::nanoTime();
		for (int i = 0; i < timer->ticks; i++)
		{
			ticks++//            try {		// 4J - try/catch removed     
			tick()//            } catch (LevelConflictException e) {    //                this.level = null;    //                setLevel(null);    //                setScreen(new LevelConflictScreen());    //            }     
		}
		__int64 tickDuraction = System::nanoTime() - beforeTickTime;
		checkGlEr"Pre render"     );

		TileRenderer::fancy = options->fancyGraphics// if (pause) timer.a = 1;     

		soundEngine->update(player, timer->a);

		glEnable(GL_TEXTURE_2D);
		if (level != NULL) level->updateLights()//        if (!Keyboard::isKeyDown(Keyboard.KEY_F7)) Display.update();		// 4J - removed     

		if (player != NULL && player->isInWall()) options->thirdPersonView = false;
		if (!noRender)
		{
			if (gameMode != NULL) gameMode->render(timer->a);
			gameRenderer->render(timer->a);
		/*	4J - removed
		if (!Display::isActive())
		{
		if (fullscreen)
		{
		this->toggleFullScreen();
		}
		Sleep(10);
		}
		*/     

		if (options->renderDebug)
		{
			renderFpsMeter(tickDuraction);
		}
		else
		{
			lastTimer = System::nanoTime();
		}

		achievementPopup->render();

		Sleep// 4J - was Thread.yield()    //        if (Keyboard::isKeyDown(Keyboard::KEY_F7)) Display.update();	// 4J - removed condition     
		Display::update()//        checkScreenshot();	// 4J - removed    /* 4J - removed
		if (parent != NULL && !fullscreen)
		{
		if (parent.getWidth() != width || parent.getHeight() != height)
		{
		width = parent.getWidth();
		height = parent.getHeight();
		if (width <= 0) width = 1;
		if (height <= 0) height = 1;

		resize(width, height);
		}
		}
		*/     
		checkGlEr"Post render"     );
		frames++;
		pause = !isClientSide() && screen != NULL && screen->isPauseScreen();

		while (System::currentTimeMillis() >= lastTime + 1000)
		{
			fpsString = _toString<int>(frames" fps, "      + _toString<int>(Chunk::updates" chunk updates"     ;
			Chunk::updates = 0;
			lastTime += 1000;
			frames = 0;
	/*
		} catch (LevelConflictException e) {
		this.level = null;
		setLevel(null);
		setScreen(new LevelConflictScreen());
		} catch (OutOfMemoryError e) {
		emergencySave();
		setScreen(new OutOfMemoryScreen());
		System.gc();
		}
		*/     /*
	} catch (StopGameException e) {
	} catch (Throwable e) {
	emergencySave();
	e.printStackTrace();
	crash(new CrashReport("Unexpected error", e));
	} finally {
	destroy();
	}
	*/     
	destroy(#endif     

void Minecraft::run()
{
	running = tr//    try {	// 4J - removed try/catch     
	init//    } catch (Exception e) {  //        e.printStackTrace();  //       crash(new CrashReport("Failed to start game", e));  //        return;  //    }  //    try {	// 4J - removed try/catch     // 4J added - Selects which local player is currently active for processing by the existing minecraft code     
bool Minecraft::setLocalPlayerIdx(int idx)
{
	localPlayerIdx = i// If the player is not null, but the game mode is then this is just a temp player  // whose only real purpose is to hold the viewport position     
	if( localplayers[idx] == NULL || localgameModes[idx] == NULL ) return false;

	gameMode = localgameModes[idx];
	player = localplayers[idx];
	cameraTargetPlayer = localplayers[idx];
	gameRenderer->itemInHandRenderer = localitemInHandRenderers[idx];
	level = getLevel( localplayers[idx]->dimension );
	particleEngine->setLevel( level );

	return true;
}

int Minecraft::getLocalPlayerIdx()
{
	return localPlayerIdx;
}

void Minecraft::updatePlayerViewportAssignments()
{
	unoccupiedQuadrant = // Find out how many viewports we'll be needing     
	int viewportsRequired = 0;
	for( int i = 0; i < XUSER_MAX_COUNT; i++ )
	{
		if( localplayers[i] != NULL ) viewportsRequired++;
	}
	if( viewportsRequired == 3 ) viewportsRequired = // Allocate away...     
	if( viewportsRequired == 1 )
// Single viewport     
		for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		{
			if( localplayers[i] != NULL ) localplayers[i]->m_iScreenSection = C4JRender::VIEWPORT_TYPE_FULLSCREEN;
		}
	}
	else if( viewportsRequired == 2 )
// Split screen - TODO - option for vertical/horizontal split     
		int found = 0;
		for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		{
			if( localplayers[i] != NULL )
			{// Primary player settings decide what the mode is     
				if(app.GetGameSettings(ProfileManager.GetPrimaryPad(),eGameSetting_SplitScreenVertical))
				{
					localplayers[i]->m_iScreenSection = C4JRender::VIEWPORT_TYPE_SPLIT_LEFT + found;
				}
				else
				{
					localplayers[i]->m_iScreenSection = C4JRender::VIEWPORT_TYPE_SPLIT_TOP + found;
				}
				found++;
			}
		}
	}
	else if( viewportsRequired >= 3 )
// Quadrants - this is slightly more complicated. We don't want to move viewports around if we are going from 3 to 4, or 4 to 3 players,   // so persist any allocations for quadrants that already exist.     
		bool quadrantsAllocated[4] = {false,false,false,false};

		for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		{
			if( localplayers[i] != NULL )
			{
// 4J Stu - If the game hasn't started, ignore current allocations (as the players won't have seen them)     // This fixes an issue with the primary player being the 4th controller quadrant, but ending up in the 3rd viewport.     
				if(app.GetGameStarted())
				{
					if( ( localplayers[i]->m_iScreenSection >= C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT ) &&
						( localplayers[i]->m_iScreenSection <= C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT ) )
					{
						quadrantsAllocated[localplayers[i]->m_iScreenSection - C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT] = true;
					}
				}
				else
				{
// Reset the viewport so that it can be assigned in the next loop     
					localplayers[i]->m_iScreenSection = C4JRender::VIEWPORT_TYPE_FULLSCREEN;
				}
			}
		// Found which quadrants are currently in use, now allocate out any spares that are required     
		for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		{
			if( localplayers[i] != NULL )
			{
				if( ( localplayers[i]->m_iScreenSection < C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT ) ||
					( localplayers[i]->m_iScreenSection > C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT ) )
				{
					for( int j = 0; j < 4; j++ )
					{
						if( !quadrantsAllocated[j] )
						{
							localplayers[i]->m_iScreenSection = C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT + j;
							quadrantsAllocated[j] = true;
							break;
						}
					}
				}
			}
	// If there's an unoccupied quadrant, record which one so we can clear it to black when rendering     
		for( int i = 0; i < XUSER_MAX_COUNT; i++ )
		{
			if( quadrantsAllocated[i] == false )
			{
				unoccupiedQuadrant = i;
			}
		}
// 4J Stu - If the game is not running we do not want to do this yet, and should wait until the task  // that caused the app to not be running is finished     
	if(app.GetGameStarted())ui.UpdatePlayerBasePositions()// Add a temporary player so that the viewports get re-arranged, and add the player to the game session     
bool Minecraft::addLocalPlayer(int idx//int iLocalPlayerC=app.GetLocalPlayerCount();     
	if( m_pendingLocalConnections[idx] != NULL )
// 4J Stu - Should we ever be in a state where this happens?     
		assert(false);
		m_pendingLocalConnections[idx]->close();
	}
	m_connectionFailed[idx] = false;
	m_pendingLocalConnections[idx] = NULL;

	bool success=g_NetworkManager.AddLocalPlayerByUserIndex(idx);

	if(success)
	{
		app.DebugPr"Adding temp local player on pad %d\n"     , idx);
		localplayers[idx] = std::shared_ptr<MultiplayerLocalPlayer>( new MultiplayerLocalPlayer(this, level, user, NULL ) );
		localgameModes[idx] = NULL;

		updatePlayerViewportAssig
#ifdef _XBOX     // tell the xui scenes a splitscreen player joined          
		XUIMessage xuiMsg;
		CustomMessage_Splitscreenplayer_Struct myMsgData;
		CustomMessage_Splitscreenplayer( &xuiMsg, &myMsgData, // send the message          
		for(int i=0;i<XUSER_MAX_COUNT;i+//if((i!=idx) && (localplayers[i]!=NULL))          
			{
				XuiBroadcastMessage( CXuiSceneBase::GetPlayerBaseScene(i), &xuiMsg );#endif		}
      

		ConnectionProgressParams *param = new ConnectionProgressParams();
		param->iPad = idx;
		param->stringId = IDS_PROGRESS_CONNECTING;
		param->showTooltips = true;
		param->setFailTimer = true;
		param->timerTime = CONNECTING_PROGRESS_CHECK// Joining as second player so always the small progress          
		ui.NavigateToScene(idx, eUIScene_ConnectingProgress, param);

	}
	else
	{
		app.De"g_NetworkManager.AddLocalPlayerByUserIndex failed\n"   #ifdef _DURANGO          
		ProfileManager.RemoveGamepadFromG#endifx);
      
	}

	return success;
}

void Minecraft::addPendingLocalConnection(int idx, ClientConnection *connection)
{
	m_pendingLocalConnections[idx] = connection;
}

std::shared_ptr<MultiplayerLocalPlayer> Minecraft::createExtraLocalPlayer(int idx, const std::wstring& name, int iPad, int iDimension, ClientConnection *clientC/*= NULL*/          ,MultiPlayerLevel *levelpassedin)
{
	if( clientConnection == NULL) return nullptr;

	if( clientConnection == m_pendingLocalConnections[idx] )
	{
		int tempScreenSection = C4JRender::VIEWPORT_TYPE_FULLSCREEN;
		if( localplayers[idx] != NULL && localgameModes[idx] == NULL// A temp player displaying a connecting screen          
			tempScreenSection = localplayers[idx]->m_iScreenSection;
		}
		std::wstring prevname = user->name;
		user->n// Don't need this any more               
		m_pendingLocalConnections[i// Add the connection to the level which will now take responsibility for ticking it   // 4J-PB - can't use the dimension from localplayers[idx], since there may be no localplayers at this point   //MultiPlayerLevel *mpLevel = (MultiPlayerLevel *)getLevel( localplayers[idx]->dimension );               

		MultiPlayerLevel *mpLevel;

		if(levelpassedin)
		{
			level=levelpassedin;
			mpLevel=levelpassedin;
		}
		else
		{
			level=getLevel( iDimension );
			mpLevel = getLevel( iDimension );
			mpLevel->addClientConnection( clientConnection );
		}

		if( app.GetTutorialMode() )
		{
			localgameModes[idx] = new FullTutorialMode(idx, this, clientConn// check if we're in the trial version               
		else if(ProfileManager.IsFullVersion()==false)
		{
			localgameModes[idx] = new TrialMode(idx, this, clientConnection);
		}
		else
		{
			localgameModes[idx] = new ConsoleGameMode(idx, this, clientConne// 4J-PB - can't do this here because they use a render context, but this is running from a thread.   // Moved the creation of these into the main thread, before level launch   //localitemInHandRenderers[idx] = new ItemInHandRenderer(this);               
		localplayers[idx] = localgameModes[idx]->createPlayer(level);

		PlayerUID playerXUIDOffline = INVALID_XUID;
		PlayerUID playerXUIDOnline = INVALID_XUID;
		ProfileManager.GetXUID(idx,&playerXUIDOffline,false);
		ProfileManager.GetXUID(idx,&playerXUIDOnline,true);
		localplayers[idx]->setXuid(playerXUIDOffline);
		localplayers[idx]->setOnlineXuid(playerXUIDOnline);
		localplayers[idx]->setIsGuest(ProfileManager.IsGuest(idx));

		localplayers[idx]->m_displayName = ProfileManager.GetDisplayName(idx);

		localplayers[idx]->m_iScreenSection = tempScreenSection;

		if( levelpassedin == NULL)	level->addEntity(local// Don't add if we're passing the level in, we only do this from the client connection & we'll be handling adding it ourselves               

		localplayers[idx]->SetXboxPad(iPad);

		if( localplayers[idx]->input != NULL ) delete localplayers[idx]->input;
		localplayers[idx]->input = new Input();

		localplayers[idx]->resetPos();

		levelRenderer->setLevel(idx, level);
		localplayers[idx]->level = level;

		user->name = prevname;

		updatePlayerViewportAss// Fix for #105852 - TU12: Content: Gameplay: Local splitscreen Players are spawned at incorrect places after re-joining previously saved and loaded "Mass Effect World".   // Move this check to ClientConnection::handleMovePlayer //		// 4J-PB - can't call this when this function is called from the qnet thread (GetGameStarted will be false) //		if(app.GetGameStarted()) //		{ //			ui.CloseUIScenes(idx); //		}         
     
	}

	return localpl// on a respawn of the local player, just store them               
void Minecraft::storeExtraLocalPlayer(int idx)
{
	localplayers[idx] = player;

	if( localplayers[idx]->input != NULL ) delete localplayers[idx]->input;
	localplayers[idx]->input = new Input();

	if(ProfileManager.IsSignedIn(idx))
	{
		localplayers[idx]->name = convStringToWstring( ProfileManager.GetGamertag(idx) );
	}
}

void Minecraft::removeLocalPlayerIdx(int idx)
{
	bool updateXui = true;
	if(localgameModes[idx] != NULL)
	{
		if( getLevel( localplayers[idx]->dimension )->isClientSide )
		{
			std::shared_ptr<MultiplayerLocalPlayer> mplp = localplayers[idx];
			( (MultiPlayerLevel *)getLevel( localplayers[idx]->dimension ) )->removeClientConnection(mplp->connection, true);
			delete mplp->connection;
			mplp->connection = NULL;
			g_NetworkManager.RemoveLocalPlayerByUserIndex(idx);
		}
		getLevel( localplayers[idx]->dimension )->removeEntity(
#ifdef _XBOXidx  // 4J Stu - Fix for #12368 - Crash: Game crashes when saving
                 // then exiting and selecting to save                    
		app.TutorialScen#endifateB// 4J Stu - Fix for #13257 - CRASH: Gameplay: Title crashed after exiting the tutorial   // It doesn't matter if they were in the tutorial already                    
		playerLeftTutorial( idx );

		delete localgameModes[idx];
		localgameModes[idx] = NULL;
	}
	else if( m_pendingLocalConnections[idx] != NULL )
	{
		m_pendingLocalConnections[idx]->sendAndDisconnect( std::shared_ptr<DisconnectPacket>( new DisconnectPacket(DisconnectPacket::eDisconnect_Quitting) ) );;
		delete m_pendingLocalConnections[idx];
		m_pendingLocalConnections[idx] = NULL;
		g_NetworkManager.RemoveLocalPlayerByUserI// Not sure how this works on qnet, but for other platforms, calling RemoveLocalPlayerByUserIndex won't do anything if there isn't a local user to remove   // Now just updating the UI directly in this case #ifdef _XBOX   // 4J Stu - A signout early in the game creation before this player has connected to the game server                      #endifupd// 4J Stu - Adding this back in for exactly the reason my comment above suggests it was added in the first place #if defined(_XBOX_ONE) || defined(__ORBIS__)                         
		g_NetworkManager.RemoveLoca#endifrByUserIndex(idx);
      
	}
	localplayers[idx] = nullptr;

	if( idx == ProfileManage// We should never try to remove the Primary player in this way                    /*
		// If we are removing the primary player then there can't be a valid gamemode left anymore, this
		// pointer will be referring to the one we've just deleted
		gameMode = NULL;
		// Remove references to player
		player = NULL;
		cameraTargetPlayer = NULL;
		EntityRenderDispatcher::instance->cameraEntity = NULL;
		TileEntityRenderDispatcher::instance->cameraEntity = NULL;
		*/                         
	}
	else if( updateXui )
	{
		gameRenderer->DisableUpdateThread();
		levelRenderer->setLevel(idx, NULL);
		gameRenderer->EnableUpdateThread();
		ui.CloseUIScenes(idx,true);
		updatePlayerVie// We only create these once ever so don't delete it here  //delete localitemInHandRenderers[idx];                         
}

void Minecraft::createPrimaryLocalPlayer(int iPad)
{
	localgameModes[iPad] = gameMode;
	local//gameRenderer->itemInHandRenderer = localitemInHandRenderers[iPad];  // Give them the gamertag if they're signed in                         
	if(ProfileManager.IsSignedIn(ProfileManager.GetPrimaryPad()))
	{
		user->name = convStringToWstring( ProfileManager.GetGamertag(ProfileManager.GetPrimaryPad()) );
	}
}

void Minecraft::run_middle()
{
	static __int64 lastTime = 0;
	static bool bFirstTimeIntoGame = true;
	static bool bAutosaveTimerSet=false;
	static unsigned int uiAutosaveTimer=0;
	static int iFirstTimeCountdown=60;
	if( lastTime == 0 ) lastTime = System::nanoTime();
	static int frames = 0;

	EnterCriticalSection(&m_setLevelCS);

	if(running)
	{
		if (reloadTextures)
		{
			reloadTextures = false;
			textu//while (running)
		}

		//        try {	// 4J - removed try/catch    //            if (minecraftApplet != null && !minecraftApplet.isActive()) break;	// 4J - removed                         
			AABB::resetPool();
	//            if (parent == NULL && Display.isCloseRequested()) {		// 4J - removed    //                stop();    //            }     // 4J-PB - AUTOSAVE TIMER - only in the full game and if the player is the host                         
			if(level!=NULL && ProfileManager.IsFullVersion() && g_NetworkMa/*if(!bAutosaveTimerSet)
				{
				// set the timer
				bAutosaveTimerSet=true;

				app.SetAutosaveTimerTime();
				}
				else*/            // if the pause menu is up for the primary player, don't autosave      // If saving isn't disabled, and the main player has a app action running , or has any crafting or containers open, don't autosave                         
					if(!StorageManager.GetSaveDisabled() && (app.GetXuiAction(ProfileManager.GetPrimaryPad())==eAppAction_Idle) )
					{
						if(!ui.IsPauseMenuDisplayed(ProfileManager.GetPrimaryPad()) && !ui.IsIgnoreAutosaveMenuDisplayed(ProfileManager.GetPrima// check if the autotimer countdown has reached zero                         
							unsigned char ucAutosaveVal=app.GetGameSettings(ProfileManager.GetPrimaryPad(),eGameSetting_Autosave);
							bool bTrialTexturepack=false;
							if(!Minecraft::GetInstance()->skins->isUsingDefaultSkin())
							{
								TexturePack *tPack = Minecraft::GetInstance()->skins->getSelected();
								DLCTexturePack *pDLCTexPack=(DLCTexturePack *)tPack;

								DLCPack *pDLCPack=pDLCTexPack->getDLCInfoParentPack();

								if( pDLCPack )
								{
									if(!pDLCPack->hasPurchasedFile( DLCMana""r::e_DLCType_Texture, L   ))
									{			
										bTrialTexturepack=true;
									}
		// If the autosave value is not zero, and the player isn't using a trial texture pack, then check whether we need to save this tick                         
							if((ucAutosaveVal!=0) && !bTrialTexturepack)
							{
								if(app.AutosaveD// disable the autosave countdown                         
									ui.ShowAutosaveCountdow// Need to save now						                   
"+++++++++++\n"bugPrintf(               );
"+++Autosave\n"bugPrintf(               );
"+++++++++++\n"bugPrintf(               );
									app.SetAction(ProfileManager.GetPrimaryPad(),eAppAction_Aut//app.SetAutosaveTimerTime(); #ifndef _CONTENT_PACKAGE
                     // print the time								                 
										SYSTEMTIME UTCSysTime;
										GetSystemTime( //char szTime[15];							                  

	"%02d:%02d:%02d\n"Printf(                  ,UTCSysTime.wHour,UTCSysTime.wMinute,UTCSysT#endifecond);
									}
      
								}
								else
								{
									unsigned int uiTimeToAutosave=app.SecondsToAutosave();

									if(uiTimeToAutosave<6)
									{
										ui.ShowAutosaveCountdownTimer(true);
										ui.UpdateAutosaveCountdownTimer(uiTimeToAutosave);
									}
								}
							}
						}
	// disable the autosave countdown                         
							ui.ShowAutosaveCountdownTimer(false);
					// 4J-PB - Once we're in the level, check if the players have the level in their banned list and ask if they want to play it                         
			for( int i = 0; i < XUSER_MAX_COUNT; i++ )
			{
				if( localplayers[i] && (app.GetBanListCheck(i)==false) && !Minecraft::GetInstance()->isTutorial() && ProfileManager.IsSignedInLive(i) && !ProfileManager// If there is a sys ui displayed, we can't display the message box here, so ignore until we can                         
					if(!ProfileManager.IsSystemUIDisplayed())
					{
						app.SetBan// 4J-PB - check if the level is in the banned level list       // get the unique save name and xuid from whoever is the host #if defined _XBOX || defined _XBOX_ONE                         
						INetworkPlayer *pHostPlayer = g_Network
#ifdef _XBOXstPlayer();
             
						PlayerUID xuid=((NetworkPlayerXbox *#elsetPlayer)->GetUID();
     
						PlayerUID xui#endiftPlayer->GetUID();
      

						if(app.IsInBannedLevelList(i,xuid,app.GetUniqueMa// put up a message box asking if the player would like to unban this level                        "This level is banned\n"(         // set the app action to bring up the message box to give them the option to remove from the ban list or exit the level                         
							app.SetAction(i,eAppAction_LevelInBanLevelLis#endifd *)TRUE);
						}
      
					}
				}
			}

			if(!ProfileManager.IsSystemUIDisplayed() && app.DLCInstallProcessCompleted() && !app.DLCInstallPending() && app.m_dlcManager.NeedsCorruptCheck() )
			{
				app.m_dlcManager.checkForCorrup// When we go into the first loaded level, check if the console has active joypads that are not in the game, and bring up the quadrant display to remind them to press start (if the session has space)                         
			if(level!=NULL && bFirstTimeIntoGame && g_NetworkManager.Ses// have a short delay before the display                         
				if(iFirstTimeCountdown==0)
				{
					bFirstTimeIntoGame=false;

					if(app.IsLocalMultiplayerAvailable())
					{
						for( int i = 0; i < XUSER_MAX_COUNT; i++ )
						{
							if((localplayers[i] == NULL) && InputManager.IsPadConnected(i))
							{
								if(!ui.PressStartPlaying(i))
								{
									ui.ShowPressStart(i);
								}
							}
						}
					}
				}
				else iFirst// 4J-PB - store any button toggles for the players, since the minecraft::tick may not be called if we're running fast, and a button press and release will be missed                         

			for( int i = 0; i < XUS#ifdef __ORBIS__+ )
			{
                
				if ( m_pPsPlusUpsell != NULL && m_pPsPlusUpsell->hasResponse() && m_pPsPlusUpsell->m_userIndex == i )
				{
					delete m_pPsPlusUpsell;
					m_pPsPlusUpsell = NULL;
								
					if ( ProfileManager.HasPlayStationPlus(i) )
				"<Minecraft.cpp> Player_%i is now authorised for PsPlus.\n"                         , i);
						if (!ui.PressStartPlaying(i)) ui.ShowPressStart(i);
					}
					else
					{
						UINT uiIDA[1] = { IDS_OK };
						ui.RequestErrorMessage( IDS_CANTJOIN_TITLE, IDS_NO_PLAYSTATIONPLUS, uiIDA, 1, i#endif		}
				}
				else
      
					if(loc// 4J-PB - add these to check for coming out of idle                         
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_JUMP))				localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_JUMP;
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_USE))					localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_USE;

					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_INVENTORY))				localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_INVENTORY;
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_ACTION))					localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_ACTION;
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_CRAFTING))				localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_CRAFTING;
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_PAUSEMENU))
					{
						localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_PAUSEMEN"PAUSE PRESSED - ipad = %d, Storing press\n"            #ifdef _DURANGO);
					}
               
					if(InputManager.ButtonPressed(i, ACTION_MENU_GTC_PAUSE))					localplayers[i]->ullButtonsPressed|=1LL#endifON_MENU_GTC_PAUSE;
      
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_DROP))					localplayers[i]->ullButtonsPressed|=1LL<<MINE// 4J-PB - If we're flying, the sneak needs to be held on to go down                         
					if(localplayers[i]->abilities.flying)
					{
						if(InputManager.ButtonDown(i, MINECRAFT_ACTION_SNEAK_TOGGLE))			localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_SNEAK_TOGGLE;
					}
					else
					{
						if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_SNEAK_TOGGLE))		localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_SNEAK_TOGGLE;
					}
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_RENDER_THIRD_PERSON))		localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_RENDER_THIRD_PERSON;
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_GAME_INFO))				localplayers[i]->ullButtonsPressed|=1LL<<MIN
#ifndef _FINAL_BUILDFO;
                     
					if( app.DebugSettingsOn() && app.GetUseDPadForDebug() )
					{
						localplayers[i]->ullDpad_last = 0;
						localplayers[i]->ullDpad_this = 0;
						localplayers[i]->ullDpad_filtered = 0;
						if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_DPAD_RIGHT))			localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_CHANGE_SKIN;
						if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_DPAD_UP))				localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_FLY_TOGGLE;
						if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_DPAD_DOWN))			localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_RENDER_DEBUG;
						if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_DPAD_LEFT))			localplayers[i]->ullButtonsPressed|=1LL<<MINECRAFT_ACTION_SPAWN_C#endif;
					}
					// Movement on DPAD is stored ulimately into ullDpad_filtered - this ignores any diagonals pressed, instead reporting the last single direction - otherwise       // we get loads of accidental diagonal movements                         

						localplayers[i]->ullDpad_this = 0;
#ifndef __PSVITA__ = 0;
                   
						if(InputManager.ButtonDown(i, MINECRAFT_ACTION_DPAD_LEFT))					{ localplayers[i]->ullDpad_this|=1LL<<MINECRAFT_ACTION_DPAD_LEFT;		dirCount++; }
						if(InputManager.ButtonDown(i, MINECRAFT_ACTION_DPAD_RIGHT))					{ localplayers[i]->ullDpad_this|=1LL<<MINECRAFT_ACTION_DPAD_RIGHT;		dirCount++; }
						if(InputManager.ButtonDown(i, MINECRAFT_ACTION_DPAD_UP))					{ localplayers[i]->ullDpad_this|=1LL<<MINECRAFT_ACTION_DPAD_UP;			dirCount++; }
						if(InputManager.ButtonDown(i, MINECRAFT_ACTION_DPAD_DOWN))					{ localplayers[i]->ullDpad_this|=1LL<<MINECRAFT_ACTION_D#endifWN;		dirCount++; }
      

						if( dirCount <= 1 )
						{
							localplayers[i]->ullDpad_last = localplayers[i]->ullDpad_this;
							localplayers[i]->ullDpad_filtered = localplayers[i]->ullDpad_this;
						}
						else
						{
							localplayers[i]->ullDpad_filtered = localplayers[i]->ullDpad_la// for the opacity timer	                        
					if(InputManager.ButtonPressed(i, MINECRAFT_ACTION_LEFT_SCROLL) || InputManager.ButtonPressed(i, MINECRAFT_ACT//InputManager.ButtonPressed(i, MINECRAFT_ACTION_USE) || InputManager.ButtonPressed(i, MINECRAFT_ACTION_ACTION))                         
					{
						app.SetOpacityTimer(i);
					}
	// 4J Stu - This doesn't make any sense with the way we handle XboxOne users #ifndef _DURANGO      // did we just get input from a player who doesn't exist? They'll be wanting to join the game then                         
					bool tryJoin = !pause && !ui.IsIgnorePlayerJoinMenuDisplayed(ProfileManager.GetPrimaryPad()) && g_NetworkManager.SessionHasSpace() && RenderManager.IsHiDef() && InputM#ifdef __ORBIS__ssed(i// Check for remote play	                        
					tryJoin = tryJoin && InputManager.IsLocalMulti// 4J Stu - Check that content restriction information has been received                         
					if( !g_NetworkManager.IsLocalGame() )
					{
						tryJoin = tryJoin && ProfileManager.GetChatAndContentRestrictions(i,true#endifNULL,NULL);
					}
      
					if(tryJoin)
					{
						if(!ui.Press#ifdef __ORBIS__
						{// Don't let player start joining until their PS Plus check has finished                         
							if (g_NetworkManager.IsLocalGame() || !ProfileManager.Reque#endiflaystationPlus(i))
      
							{
								ui.ShowPressStart(i);
							}
						}
	// did we just get input from a player who doesn't exist? They'll be wanting to join the game then #ifdef __ORBIS__        
                
							if(InputManager.ButtonPr#else(i, ACTION_MENU_A))
     
							if(InputManager.ButtonPressed(i, MINE#endifACTION_PAUSEMENU))// Let them join
								 // are they signed in?			                      
								if(ProfileManager.IsSignedI// if this is a local game, then the player just needs to be signed in                         
									if( g_NetworkManager.IsLocalGame() || (ProfileManager.IsSignedInLive(i) && ProfileManager.AllowedToPlayMulti#ifdef __ORBIS__							{
                
										bool contentRestricted = false;
										ProfileManager.GetChatAndContentRestrictions(i,false,NULL,&// TODO!estricted,NULL);         

										if (!g_NetworkManager.IsLocalGame() && contentRestricted)
										{
											ui.RequestContentRestrictedMessageBox(IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_CONTENT_RESTRICTION, i);
										}
										else if(!g_NetworkManager.IsLocalGame() && !ProfileManager.HasPlayStationPlus(i))
										{
											m_pPsPlusUpsell = new PsPlusUpsellWrapper(i);
											m_pPsPlusUpsell->displayUpsell();
#endif }
										else
      
										if( level->isClientSide )
										{
											bool success=addLocalPlayer(i);

											if(!success)
											{
			"Bringing up the sign in ui\n"                         );
												ProfileManager.RequestSignInUI(false, g_NetworkManager.IsLocalGame(), true, false,true,&Minecraft::InGame_SignInReturned, this,i);
											}
#ifdef __ORBIS__ {
                
												if(g_NetworkManager.IsLocalGame() == false)
												{
													bool chatRestricted = false;
													ProfileManager.GetChatAndContentRestrictions(i,false,&chatRestricted,NULL,NULL);
													if(chatRestricted)
													{
														ProfileManager.DisplaySystemMessage( SCE_MSG_DIALOG_SYSMSG_TYPE_TRC_PSN_CHAT_RESTRICTION, i );
#endif }
												}
      
											}
										}
										els// create the localplayer                         
											std::shared_ptr<Player> player = localplayers[i];
											if( player == NULL)
											{
												player = createExtraLocalPlayer(i, (convStringToWstring( ProfileManager.GetGamertag(i) )).c_str(), i, level->dimension->id);
											}
										}
									}
									else
									{
										if( ProfileManager.IsSignedInLive(ProfileManager.GetPrimaryPad()) && !ProfileManager.AllowedToPlayMultiplayer(i) )
										{
											ProfileManager.RequestConvertOfflineToGuestUI( &Minecraft::InGame_SignIn// 4J Stu - Don't allow converting to guests as we don't allow any guest sign-in while in the game            // Fix for #66516 - TCR #124: MPS Guest Support ; #001: BAS Game Stability: TU8: The game crashes when second Guest signs-in on console which takes part in Xbox LIVE multiplayer session.            //ProfileManager.RequestConvertOfflineToGuestUI( &Minecraft::InGame_SignInReturned, this,i); 
#ifndef _XBOX               
              
		#endif
#ifdef __ORBIS__rt();
      
                 
											int npAvailability = ProfileManager.getN// Check if PSN is unavailable because of age restriction                              
											if (npAvailability == SCE_NP_ERROR_AGE_RESTRICTION)
											{
												UINT uiIDA[1];
												uiIDA[0] = IDS_OK;
												ui.RequestErrorMessage(IDS_ONLINE_SERVICE_TITLE, IDS_CONTENT_RESTRICTION, uiIDA, 1, i);
											}
											else if (ProfileManager.IsSignedIn(i) && !ProfileManager.IsSignedInLive// You're not signed in to PSN!                              
												UINT uiIDA[2];
												uiIDA[0] = IDS_PRO_NOTONLINE_ACCEPT;
												uiIDA[1] = IDS_CANCEL;
												ui.RequestAlertMessage(IDS_PRO_NOTONLINE_TITLE, IDS_PRO_NOTONLINE_TEXT, uiIDA, 2, i,&Minecraft::MustSignInReturnedPSN, this);#endif						}
											else
      
											{
												UINT uiIDA[1];
												uiIDA[0]=IDS_CONFIRM_OK;
												ui.RequestErrorMessage(IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_JOIN_TEXT, uiIDA, 1, i);
					//else}
										}
										// player not signed in to live            // bring up the sign in dialog                            "Bringing up the sign in ui\n"                              );
											ProfileManager.RequestSignInUI(false, g_NetworkManager.IsLocalGame(), true, false,true,&Minecraft::InGame_SignInReturned, this,i);
										}
									}
								}
		// bring up the sign in dialog                          "Bringing up the sign in ui\n"                              );
									ProfileManager.RequestSignInUI(false, g_NetworkManager.IsLocalGame(), true, false,true,&Minecraft::InGame_SignInReturned, this,i);
#endif  // _DURANGO			}
					}

#ifdef _DURANGO  
	  // did we just get input from a player who doesn't exist? They'll be wanting to join the game then                              
			if(!pause && !ui.IsIgnorePlayerJoinMenuDisplayed(ProfileManager.GetPrimaryPad()) && g_NetworkManager.SessionHasSpace() && RenderManager.IsHiDef() )
			{
				int firstEmptyUser = 0;
				for( int i = 0; i < XUSER_MAX_COUNT; i++ )
				{
					if(localplayers[i] == NULL)
					{
						firstEmptyUser = i;
	// For durango, check for unmapped controllers                              
				for(unsigned int iPad = XUSER_MAX_COUNT; iPad < (XUSER_MAX_COUNT + InputManager.MAX_GAMEPADS); ++iPad)
				{
					bool isPadLocked = InputManager.IsPadLocked(iPad), isPadConnected = InputManager.IsPadConnected(iPad), buttonPressed = InputManager.ButtonPressed(iPad);
					if (isPadLocked || !isPadConnected || !buttonPressed) continue;

					if(!ui.PressStartPlaying(firstEmptyUser))
					{
						ui.ShowPressStart(firstEmptyUser);
// did we just get input from a player who doesn't exist? They'll be wanting to join the game then                              
						if(InputManager.ButtonPressed(iPad, MINECRAFT_ACTI// bring up the sign in dialog                        "Bringing up the sign in ui\n"                              );
							ProfileManager.RequestSignInUI(false, g_NetworkManager.IsLocalGame(), true, false,true,&Minecraft::InGame_SignIn// 4J Stu - If we are joining a pad here, then we don't want to try and join any others                              
							bre#endif				}
					}
				}
			}
      

			if (pause && level != NULL)
			{
				float lastA = timer->a;
				timer->advanceTime();
				timer->a = lastA;
			}
			else
			{
				t//__int64 beforeTickTime = System::nanoTime();                              
			for (int i = 0; i < timer->ticks; i++)
			{
				bool bLastTimerTick = ( i =// 4J-PB - the tick here can run more than once, and this is a problem for our input, which would see the a key press twice with the same time - let's tick the inputmanager again                              
				if(i!=0)
				{
					InputManager.Tick();
					app.HandleButtonPress//            try {		// 4J - try/catch removed                              
				bool bFirst = true;
				for( int idx = 0; idx < XUSER_// 4J - If we are waiting for this connection to do something, then tick it here.      // This replaces many of the original Java scenes which would tick the connection while showing that scene                              
					if( m_pendingLocalConnections[idx] != NULL )
					{
						m_pendingLocalConnectio// reset the player inactive tick                              
					if(localpla// any input received?{
						                      
						if((localplayers[idx]->ullButtonsPressed!=0) || InputManager.GetJoypadStick_LX(idx,false)!=0.0f ||
							InputManager.GetJoypadStick_LY(idx,false)!=0.0f || InputManager.GetJoypadStick_RX(idx,false)!=0.0f ||
							InputManager.GetJoypadStick_RY(idx,false)!=0.0f )
						{
							localplayers[idx]->ResetInactiveTicks();
						}
						else
						{
							localplayers[idx]->IncrementInactiveTicks();
						}

						if(localplayers[idx]->GetInactiveTicks()>200)
						{
							if(!localplayers[idx]->isIdle() && localplayers[idx]->onGround)
							{
								localplayers[idx]->setIsIdle(true);
							}
						}
						else
						{
							if(localplayers[idx]->isIdle())
							{
								localplayers[idx]->setIsIdle(false);
							}
						}
					}

					if( setLocalPlayerIdx(idx) )
					{
						tick(bFirst, bLastTimerTick)// clear the stored button downs since the tick for this player will now have actioned them                              
						player->ullButtonsPressed=0LL;
					}
				}

				ui.HandleGameTick();

				setLocalPlayerIdx(ProfileM// 4J - added - now do the equivalent of level::animateTick, but taking into account the positions of all our players                              

				for( int l = 0; l < levels.length; l++ )
				{
					if( levels[l] )
					{
						levels[l]->animateTi//            } catch (LevelConflictException e) {     //                this.level = null;     //                setLevel(null);     //                setScreen(new LevelConflictScreen());     //            } // 				SparseLightStorage::tick();	// 4J added // 				CompressedTileStorage::tick();	// 4J added // 				SparseDataStorage::tick();		// 4J added         //__int64 tickDuraction = System::nanoTime() - beforeTickTime;                              
			"Pre render"
			checkGlError(L            );
			MemSect(0);

			TileRenderer::fancy // if (pause) timer.a = 1;
			                      "Sound engine update"dEvent(0,                     );
			soundEngine->tick((std::shared_ptr<Mob> *)localplayers, timer->a);
			PIXEndName"Light update"PIXBeginNamedEvent(0,              );

			glEnable(GL_TEXTUR//        if (!Keyboard::isKeyDown(Keyboard.KEY_F7)) Display.update();		// 4J - removed     // 4J-PB - changing this to be per player    //if (player != NULL && player->isInWall()) options->thirdPersonView = false;                                   
			if (player != NULL && player->isInWall()) player->SetThirdPersonView(0);

			if (!noRender)
			{
				bool bFirst = true;
				int iPrimaryPad=ProfileManager.GetPrimaryPad();
				for( int i = 0; i < XUSER_MAX_COUNT; i++ )
				{
					if( setLocalPlayerIdx(i) )"Game render player idx %d"Event(0,                           ,i);
						RenderManager.StateSetViewport((C4JRender::eViewportType)player->m_iScreenSection);
						gameRenderer->render(timer->a, bFirst);
						bFirst = false;
						PIXEndNamedEvent();#ifdef __ORBIS__rimaryPa// PS4 does much of the screen-capturing for every frame, to simplify the synchronisation when we actually want a capture. This call tells it the point in the frame to do it.                                   
							Rend#endifger.Inte// check to see if we need to capture a screenshot for the save game thumbnail                                   
							switch(app.GetXuiAction(i))
							{
							case eAppAction_ExitWorldCapturedThumbnail:
							case eAppAction_SaveGameCapturedThumbnail:
							case eAppAction_Autosave// capture the save thumbnail						                             
								app.CaptureSaveThumbnail();
								break;// If there's an unoccupied quadrant, then clear that to black                                   
				if( un// render a logo > -1 )
				{
					                
					RenderManager.StateSetViewport((C4JRender::eViewportType)(C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT + unoccupiedQuadrant));
					glClearColor(0, 0, 0, 0);
					glClear(GL_COLOR_BUFFER_BIT);

					ui.SetEmptyQuadrantLogo(C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT + unoccupiedQuadrant);
				}
				setLocalPlayerIdx(iPrimaryPad);
				RenderManager.StateSetViewport(C4J
#ifdef _XBOXORT_T  // Do we need to capture a screenshot for a social
                   // post?                                   
				for( int i = 0; i < XUSER_MAX_COUNT; i++ )
				{
					if(app.GetXuiAction(i)==eAppAction_SocialPostScreenshot)
					{
						app.#endifeScreenshot(i);
					}
	/*	4J - removed
			if (!Display::isActive())
			{
			if (fullscreen)
			{
			this->toggleFullScreen();
			}
			Sleep(10);
			}
			*/ 
#if PACKET_ENABLE_STAT_TRACKING  
                                
#endifket::updatePacketStatsPIX();
      

			//renderFpsMeter(tickDuraction);
#if DEBUG_RENDER_SHOWS_PACKETS   
   // To show data for only one packet type     //Packet::renderPacketStats(31);			   // To show data for all packet types selected as being renderable in the Packet:static_ctor call to Packet::map                                   
#elsecket:  // To show the size of the QNet queue in bytes and
            // messages                                   
				g_#endifkManager.renderQueueMeter();
      
			}
			else
			{
				lastTimer = System::nanoTime();
			}

			achievementPopup->"Sleeping"
			PIXBeginName// 4J - was Thread.yield()leep(0);	                   //        if (Keyboard::isKeyDown(Keyboard::KEY_F7)) Display.update();	// 4J - removed condition                         "Display update"XBeginNamedEvent(0,                );
			Display::up//        checkScreenshot();	// 4J - removed     /* 4J - removed
			if (parent != NULL && !fullscreen)
			{
			if (parent.getWidth() != width || parent.getHeight() != height)
			{
			width = parent.getWidth();
			height = parent.getHeight();
			if (width <= 0) width = 1;
			if (height <= 0) height = 1;

			resize(width, height);
			}
			}
			*/                                  "Post render"31);
			checkGlError(L            //pause = !isClientSide() && screen != NULL && screen->isPauseScreen();    //pause = g_NetworkManager.IsLocalGame() && g_NetworkManager.GetPlayerCount() == 1 && app.IsPauseMenuDisplayed(ProfileManager.GetPrimaryPad());                               
#ifndef _CONTENT_PACKAGEPaused();
                         
			while (System::nanoTime() >= lastTime + 1000000000)
			{
				MemSect(31);
				fps" fps, " _toString<int>(frames) + L         +" chunk updates"Chunk::updates) + L                ;
				MemSect(0);
				Chunk::updates = 0;
				lastTime +#endif0000/*
			} catch (LevelConflictException e) {
			this.level = null;
			setLevel(null);
			setScreen(new LevelConflictScreen());
			} catch (OutOfMemoryError e) {
			emergencySave();
			setScreen(new OutOfMemoryScreen());
			System.gc();
			}
			*/       /*
		} catch (StopGameException e) {
		} catch (Throwable e) {
		emergencySave();
		e.printStackTrace();
		crash(new CrashReport("Unexpected error", e));
		} finally {
		destroy();
		}
		*/                                   
	}
	LeaveCriticalSection(&m_setLevelCS);
}

void Minecraft::run_end()
{
	destroy();
}

// 4J - lots of try/catches removed here, and garbage collector things                                   
	levelRenderer->clear();
	AABB::clearPool();
	Vec3::clearPool();
	setLevel(NULL);
}

void Minecraft::renderFpsMeter(__int64 tickTime)
{
	int nsPer60Fps = 1000000000l / 60;
	if (lastTimer == -1)
	{
		lastTimer = System::nanoTime();
	}
	__int64 now = System::nanoTime();
	Minecraft::tickTimes[(Minecraft::frameTimePos) & (Minecraft::frameTimes_length - 1)] = tickTime;
	Minecraft::frameTimes[(Minecraft::frameTimePos++) & (Minecraft::frameTimes_length - 1)] = now - lastTimer;
	lastTimer = now;

	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_COLOR_MATERIAL);
	glLoadIdentity();
	glOrtho(0, (float)width, (float)height, 0, 1000, 3000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -2000);

	glLineWidth(1);
	glDisable(GL_TEXTURE_2D);
	Tesselator *t = Tesselator::getInstance();
	t->begin(GL_QUADS);
	int hh1 = (int) (nsPer60Fps / 200000);
	t->color(0x20000000);
	t->vertex((float)(0), (float)( height - hh1), (float)( 0));
	t->vertex((float)(0), (float)( height), (float)( 0));
	t->vertex((float)(Minecraft::frameTimes_length), (float)( height), (float)( 0));
	t->vertex((float)(Minecraft::frameTimes_length), (float)( height - hh1), (float)( 0));

	t->color(0x20200000);
	t->vertex((float)(0), (float)( height - hh1 * 2), (float)( 0));
	t->vertex((float)(0), (float)( height - hh1), (float)( 0));
	t->vertex((float)(Minecraft::frameTimes_length), (float)( height - hh1), (float)( 0));
	t->vertex((float)(Minecraft::frameTimes_length), (float)( height - hh1 * 2), (float)( 0));

	t->end();
	__int64 totalTime = 0;
	for (int i = 0; i < Minecraft::frameTimes_length; i++)
	{
		totalTime += Minecraft::frameTimes[i];
	}
	int hh = (int) (totalTime / 200000 / Minecraft::frameTimes_length);
	t->begin(GL_QUADS);
	t->color(0x20400000);
	t->vertex((float)(0), (float)( height - hh), (float)( 0));
	t->vertex((float)(0), (float)( height), (float)( 0));
	t->vertex((float)(Minecraft::frameTimes_length), (float)( height), (float)( 0));
	t->vertex((float)(Minecraft::frameTimes_length), (float)( height - hh), (float)( 0));
	t->end();
	t->begin(GL_LINES);
	for (int i = 0; i < Minecraft::frameTimes_length; i++)
	{
		int col = ((i - Minecraft::frameTimePos) & (Minecraft::frameTimes_length - 1)) * 255 / Minecraft::frameTimes_length;
		int cc = col * col / 255;
		cc = cc * cc / 255;
		int cc2 = cc * cc / 255;
		cc2 = cc2 * cc2 / 255;
		if (Minecraft::frameTimes[i] > nsPer60Fps)
		{
			t->color(0xff000000 + cc * 65536);
		}
		else
		{
			t->color(0xff000000 + cc * 256);
		}

		__int64 time = Minecraft::frameTimes[i] / 200000;
		__int64 time2 = Minecraft::tickTimes[i] / 200000;

		t->vertex((float)(i + 0.5f), (float)( height - time + 0.5f), (float)( 0));
		t->vertex((float)(i + 0.5f), (float)// if (Minecraft.frameTimes[i]>nsPer60Fps) {                                   
		t->color(0xff000000 + // } else {+ c// t.color(0xff808080 + cc/2 * 256);   // }                            
		    
		t->vertex((float)(i + 0.5f), (float)( height - time + 0.5f), (float)( 0));
		t->vertex((float)(i + 0.5f), (float)( height - (time - time2) + 0.5f), (float)( 0));
	}
	t->end();

	glEnable(GL_TEXTURE_2D);
}

void Mine// keepPolling = false;g = false;
	                       
}

void Minecraft::pauseGame(//    setScreen(new PauseScreen());	// 4J - TODO put back in                                   
}

void Minecraft::resize(int width, int height)
{
	if (width <= 0) width = 1;
	if (height <= 0) height = 1;
	this->width = width;
	this->height = height;

	if (screen != NULL)
	{
		ScreenSizeCalculator ssc(options, width, height);
		int screenWidth = ssc.getWidth();
		int//        screen->init(this, screenWidth, screenHeight);	// 4J - TODO - put back in                                   /* 4J - TODO
	new Thread() {
	public void run() {
	try {
	HttpURLConnection huc = (HttpURLConnection) new URL("https://login.minecraft.net/session?name=" + user.name + "&session=" + user.sessionId).openConnection();
	huc.connect();
	if (huc.getResponseCode() == 400) {
	warezTime = System.currentTimeMillis();
	}
	huc.disconnect();
	} catch (Exception e) {
	e.printStackTrace();
	}
	}
	}.start();
	*/                                   
}




void Minecraft::levelTickUpdateFunc(void* pParam)
{
	Level* pLevel = (Level*)pParam;
	pLevel->tick();
}

void Minecraft::levelTickThreadInitFunc()
{
	AABB::CreateNewThreadStorage();
	Vec3::CreateNewThreadStorage();
	IntCache::CreateNewThreadStorage();	
	Compressi// 4J - added bFirst parameter, which is true for the first active viewport in splitscreen // 4J - added bUpdateTextures, which is true if the actual renderer textures are to be updated - this will be true for the last time this tick runs with bFirst true                                   
void Minecraft::tick(bool bFirst, bool bUpdateTextures)
//OutputDebugString("Minecraft::tick\n");   //4J-PB - only tick this player's stats                             // Tick the opacity timer (to display the interface at default opacity for a certain time if the user has been navigating it)                                // 4J addedckOpacityTimer(iPad);

	           
	if( bFirst ) levelRenderer->destroyedTileManager->tick();

	gu#if 0ck// 4J - removed - we don't use ChunkCache anymore                                   
	if (player != NULL)
	{
		ChunkSource *cs = level->getChunkSource();
		if (dynamic_cast<ChunkCache *>(cs) != NULL)
		{
			ChunkC// 4J - there was also Mth::floors on these ints but that seems superfluous                                   
			int xt = ((int) player->x) >> 4;
			int zt = ((int) player->z) >> 4;#endifcc-// soundEngine.playMusicTick(); 

	                               

	if (!pause && level != NULL) gameMode->tick();
	MemSect(31);
	glBindTexture(GL_TEXTURE_2D, t//L"/terrain.png"));(TN_TERRAIN));                     
	MemSect(0);
	if( "Texture tick"PIXBeginNamedEvent(0,              );
		if (!pause) textures->tick(bUpdateTex/*
	* if (serverConnection != null && !(screen instanceof ErrorScreen)) {
	* if (!serverConnection.isConnected()) {
	* progressRenderer.progressStart("Connecting..");
	* progressRenderer.progressStagePercentage(0); } else {
	* serverConnection.tick(); serverConnection.sendPosition(player); } }
	*/                                   
	if (screen == NULL && player != NULL )
	{
		if (player->getHealth() <= 0 && !ui.GetMenuDisplayed(iPad) )
		{
			setScreen(NULL);
		}
		else if (player->isSleeping() && level != N//            setScreen(new InBedChatScreen());		// 4J - TODO put back in                                   
		}
	}
	else if (screen != NULL && (dynamic_cast<InBedChatScreen *>(screen)!=NULL) && !player->isSleeping())
	{
		setScreen(NULL);
	}

	if (screen != NULL)
	{
		player->missTime = 10000;
		player->lastClickTick[0] = ticks + 10000;
		player->lastClickTick[1] = ticks + 10000;
	}

	if (screen != NULL)
	{
		screen->updateEvents();
		if (screen != NULL)
		{
			screen->particles->tick();
			screen->tick();
		}
	}

	if (screen == NULL &&// 4J-PB - add some tooltips if required                                   
		int iA=-1, iB=-1, iX, iY=IDS_CONTROLS_INVENTORY, iLT=-1, iRT=-1, iLB=-1, iRB=-1, iLS=-1, iRS=-1;

		if(player->abilities.instabuild)
		{
			iX=IDS_TOOLTIPS_CREATIVE;
		}
		else
		{
// control scheme remapping can move the Action button, so we need to check this                                   
		int *piAction;
		int *piJump;
		int *piUse;
		int *piAlt;

		unsigned int uiAction = InputManager.GetGameJoypadMaps(InputManager.GetJoypadMapVal( iPad ) ,MINECRAFT_ACTION_ACTION );
		unsigned int uiJump = InputManager.GetGameJoypadMaps(InputManager.GetJoypadMapVal( iPad ) ,MINECRAFT_ACTION_JUMP );
		unsigned int uiUse = InputManager.GetGameJoypadMaps(InputManager.GetJoypadMapVal( iPad ) ,MINECRAFT_ACTION_USE );
		unsigned int uiAlt = InputManager.GetGameJoypadMaps(InputManager.GetJoypadMapVal( iPad ) ,M// Also need to handle PS3 having swapped triggers/bumpers                                   
		switch(uiAction)
		{
		case _360_JOY_BUTTON_RT:
			piAction=&iRT;
			break;
		case _360_JOY_BUTTON_LT:
			piAction=&iLT;
			break;
		case _360_JOY_BUTTON_LB:
			piAction=&iLB;
			break;
		case _360_JOY_BUTTON_RB:
			piAction=&iRB;
			break;
		case _360_JOY_BUTTON_A:
		default:
			piAction=&iA;
			break;
		}

		switch(uiJump)
		{
		case _360_JOY_BUTTON_LT:
			piJump=&iLT;
			break;
		case _360_JOY_BUTTON_RT:
			piJump=&iRT;
			break;
		case _360_JOY_BUTTON_LB:
			piJump=&iLB;
			break;
		case _360_JOY_BUTTON_RB:
			piJump=&iRB;
			break;
		case _360_JOY_BUTTON_A:
		default:
			piJump=&iA;
			break;
		}

		switch(uiUse)
		{
		case _360_JOY_BUTTON_LB:
			piUse=&iLB;
			break;
		case _360_JOY_BUTTON_RB:
			piUse=&iRB;
			break;
		case _360_JOY_BUTTON_LT:
			piUse=&iLT;
			break;
		case _360_JOY_BUTTON_RT:
		default:
			piUse=&iRT;
			break;
		}

		switch(uiAlt)
		{
		default:
		case _360_JOY_BUTTON_LSTICK_R//TODO			piAlt=&iRS;
			break;

			      
		}

		if (player->isUnderLiquid(Material::water))
		{
			*piJump=IDS_TOOLTIPS_SWIMUP;
		}
		else
		{
			*piJump=-1;
		}

		*piUse=-// 4J-PB another special case for when the player is sleeping in a bed                                   
		if (player->isSleeping() && (level != NULL) && level->isClientSide)
		{
			*piUse=IDS_TOOLTIPS_WAKEUP;
		}
		else
		{
			if (player->isRiding())
			{
				std::shared_ptr<Entity> mount = player->riding;

				if ( mount->instanceof(eTYPE_MINECART) || mount->instanceof(eTYPE_BOAT) )
				{
					*piAlt = IDS_TOOLTIPS_EXIT;
				}
				else
				{
					*piAlt // no hit result, but we may have something in our hand that we can do something with                                        
			std::shared_ptr<ItemInstance> itemInst// 4J-JEV: Moved all this here to avoid having it in 3 different places.                               // 4J-PB - very special case for boat and empty bucket and glass bottle and more                                             
				bool bUseItem = gameMode->useItem(player, level, itemInstance, true);

				swi// foodemInstance->getItem()->id)
				{
					       
				case Item::potatoBaked_Id:
				case Item::potato_Id:
				case Item::pumpkinPie_Id:
				case Item::potatoPoisonous_Id:
				case Item::carrotGolden_Id:
				case Item::carrots_Id:
				case Item::mushroomStew_Id:
				case Item::apple_Id:
				case Item::bread_Id:
				case Item::porkChop_raw_Id:
				case Item::porkChop_cooked_Id:
				case Item::apple_gold_Id:
				case Item::fish_raw_Id:
				case Item::fish_cooked_Id:
				case Item::cookie_Id:
				case Item::beef_cooked_Id:
				case Item::beef_raw_Id:
				case Item::chicken_cooked_Id:
				case Item::chicken_raw_Id:
				case Item::melon_Id:
				case Item::rotten// Check that we are actually hungry so will eat this item                                             
					{
						FoodItem *food = (FoodItem *)itemInstance->getItem();
						if (food != NULL && food->canEat(player))
						{
							*piUse=IDS_TOOLTIPS_EAT;
						}
					}
					break;

				case Item::bucket_milk_Id:
					*piUse=IDS_TOOLTIPS_DRINK// use	break;

				case Item::fishingRod_Id:	      
				case Item::emptyMap_Id:
					*piUse=IDS_TOOLTIP// throw				break;

				case Item::egg_Id:			        
				case Item::snowBall_Id:
					*piUse=IDS_TOOLTIP// draw or releasek;

				case Item::bow_Id:	                  
					if ( player->abilities.instabuild || player->inventory->hasResource(Item::arrow_Id) )
					{
						if (player->isUsingItem())	*piUse=IDS_TOOLTIPS_RELEASE_BOW;
						else						*piUse=IDS_TOOLTIPS_DRAW_BOW;
					}
					break;

				case Item::sword_wood_Id:
				case Item::sword_stone_Id:
				case Item::sword_iron_Id:
				case Item::sword_diamond_Id:
				case Item::sword_gold_Id:
					*piUse=IDS_TOOLTIPS_BLOCK;
					break;

				case Item::bucket_empty_Id:
				case Item::glassBottle_Id:
					if (bUseItem) *piUse=IDS_TOOLTIPS_COLLECT;
					break;

				case Item::bucket_lava_Id:
				case Item::bucket_water_Id:
					*piUse=IDS_TOOLTIPS_EMPTY;
					break;

				case Item::boat_Id:
				case Tile::waterLily_Id:
					if (bUseItem) *piUse=IDS_TOOLTIPS_PLACE;
					break;

				case Item::potion_Id:
					if (bUseItem)
					{
						if (MACRO_POTION_IS_SPLASH(itemInstance->getAuxValue()))	*piUse=IDS_TOOLTIPS_THROW;
						else														*piUse=IDS_TOOLTIPS_DRINK;
					}
					break;

				case Item::enderPearl_Id:
					if (bUseItem) *piUse=IDS_TOOLTIPS_THROW;
			// This will only work if there is a stronghold in this dimension                                             
					if ( bUseItem && (level->dimension->id==0) && level->getLevelData()->getHasStronghold() )
					{
						*piUse=IDS_TOOLTIPS_THROW;
					}
					break;

				case Item::expBottle_Id:
					if (bUseItem) *piUse=IDS_TOOLTIPS_THROW;
					break;
				}
			}

			if (hitResult!=NULL)
			{
				switch(hitResult->type)
				{
				case HitResult::TILE:
					{
						int x,y,z;
						x=hitResult->x;
						y=hitResult->y;
						z=hitResult->z;
						int face = hitResult->f;

						int iTileID=level->getTile(x,y ,z );
						int iData = level->getData(x, y, z);

						if( gameMode != NULL && game// 4J Stu - For the tutorial we want to be able to record what items we look at so that we can give hints                                             
							gameMode->getTutoria// 4J-PB - Call the useItemOn with the TestOnly flag set                                             
						bool bUseItemOn=gameMode->useItemOn(player, level, itemInstance, /* 4J-Jev:
						 *	Moved this here so we have item tooltips to fallback on
						 *	for noteblocks, enderportals and flowerpots in case of non-standard items.
						 *	(ie. ignite behaviour)
						 */                                             
						if (bUseItemOn && itemInstance!=NULL)
						{
							switch (itemInstance->getItem()->id)
							{
							case Tile::mushroom_brown_Id:
							case Tile::mushroom_red_Id:
							case Tile::tallgrass_Id:
							case Tile::cactus_Id:
							case Tile::sapling_Id:
							case Tile::reeds_Id:
							case Tile::flower_Id:
							case Tile::rose_Id:
								*piUse// Things to USEANT;
								break;

								                
							case Item::hoe_wood_Id:
							case Item::hoe_stone_Id:
							case Item::hoe_iron_Id:
							case Item::hoe_diamond_Id:
							case Item::hoe_gold_Id:
								*piUse=IDS_TOOLTIPS_TILL;
								break;

							case Item::seeds_wheat_Id:
							case Item::netherwart_seeds_Id:
								*piUse=IDS_TOOLTIPS_PLANT;
								brea// bonemeal grows various plants_Id:
								                                
								if (itemInstance->getAuxValue() == DyePowderItem::WHITE)
								{
									switch(iTileID)
									{
									case Tile::sapling_Id:
									case Tile::wheat_Id:
									case Tile::grass_Id:
									case Tile::mushroom_brown_Id:
									case Tile::mushroom_red_Id:
									case Tile::melonStem_Id:
									case Tile::pumpkinStem_Id:
									case Tile::carrots_Id:
									case Tile::potatoes_Id:
										*piUse=IDS_TOOLTIPS_GROW;
										break;
									}
								}
								break;

							case Item::painting_Id:
								*piUse=IDS_TOOLTIPS_HANG;
								break;

							case Item::flintAndSteel_Id:
							case Item::fireball_Id:
								*piUse=IDS_TOOLTIPS_IGNITE;
								break;

							case Item::fireworks_Id:
								*piUse=IDS_TOOLTIPS_FIREWORK_LAUNCH;
								break;

							case Item::lead_Id:
								*piUse=IDS_TOOLTIPS_ATTACH;
								break;

							default:
								*piUse=IDS_TOOLTIPS_PLACE;
								break;
							}
						}

						switch(iTileID)
						{
						case Tile::anvil_Id:
						case Tile::enchantTable_Id:
						case Tile::brewingStand_Id:
						case Tile::workBench_Id:
						case Tile::furnace_Id:
						case Tile::furnace_lit_Id:
						case Tile::door_wood_Id:
						case Tile::dispenser_Id:
						case Tile::lever_Id:
						case Tile::button_stone_Id:
						case Tile::button_wood_Id:
						case Tile::trapdoor_Id:
						case Tile::fenceGate_Id:
						case Tile::beacon_Id:
							*piAction=IDS_TOOLTIPS_MINE;
							*piUse=IDS_TOOLTIPS_USE;
							break;

						case Tile::chest_Id:
							*piAction = IDS_TOOLTIPS_MINE;
							*piUse = (Tile::chest->getContainer(level,x,y,z) != NULL) ? IDS_TOOLTIPS_OPEN : -1;
							break;

						case Tile::enderChest_Id:
						case Tile::chest_trap_Id:
						case Tile::dropper_Id:
						case Tile::hopper_Id:
							*piUse=IDS_TOOLTIPS_OPEN;
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::activatorRail_Id:
						case Tile::goldenRail_Id:
						case Tile::detectorRail_Id:
						case Tile::rail_Id:
							if (bUseItemOn) *piUse=IDS_TOOLTIPS_PLACE;
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::bed_Id:
							if (bUseItemOn)	*piUse=IDS_TOOLTIPS_SLEEP;
							*piAction=IDS_TOOLTIPS_MINE;
							b// if in creative mode, we will mine:
							                                    
							if (player->abilities.instabuild)	*piAction=IDS_TOOLTIPS_MINE;
							else								*piAction=IDS_TOOLTIPS_PLAY;
							*piUse=IDS_TOOLTIPS_CHANGEPITCH;
							break;

						case Tile::sign_Id:
							*piAction=IDS_TOOLTIPS_MINE;
							// special case for a cauldron of water and an empty bottle                                             
							if (itemInstance)
							{
								int iID=itemInstance->getItem()->id;
								int currentData = level->getData(x, y, z);
								if ((iID==Item::glassBottle_Id) && (currentData > 0))
								{
									*piUse=IDS_TOOLTIPS_COLLECT;
								}
							}
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::cake_// if in creative mode, we will minetabuild)                                     
							{
								*piAction=IDS_TOOLTIPS_MINE;
							}
							else
							{
				// 4J-JEV: Changed from healthto hunger.() )                                         
								{
									*piAction=IDS_TOOLTIPS_EAT;
									*piUse=IDS_TOOLTIPS_EAT;
								}
								else
								{
									*piAction=IDS_TOOLTIPS_MINE;
								}
							}
							break;

						case Tile::jukebox_Id:
							if (!bUseItemOn && itemInstance!=NULL)
							{
								int iID=itemInstance->getItem()->id;
								if ( (iID>=Item::record_01_Id) && (iID<=Item::record_12_Id) )
								{
									*piUse=IDS_TOOLTIPS_PLAY;
								}
								*piAction=IDS_TOOLTIPS_MINE;
							}
							else
							{
								if (Til// means we can ejectevel, x, y, z, player))                      
								{
									*piUse=IDS_TOOLTIPS_EJECT;
								}
								*piAction=IDS_TOOLTIPS_MINE;
							}
							break;

						case Tile::flowerPot_Id:
							if ( !bUseItemOn && (itemInstance != NULL) && (iData == 0) )
							{
								int iID = itemI// is it a tile?()->id;
								if (iID<256)                 
								{
									switch(iID)
									{
									case Tile::flower_Id:
									case Tile::rose_Id:
									case Tile::sapling_Id:
									case Tile::mushroom_brown_Id:
									case Tile::mushroom_red_Id:
									case Tile::cactus_Id:
									case Tile::deadBush_Id:
										*piUse=IDS_TOOLTIPS_PLANT;
										break;

									case Tile::tallgrass_Id:
										if (itemInstance->getAuxValue() != TallGrass::TALL_GRASS) *piUse=IDS_TOOLTIPS_PLANT;
										break;
									}
								}
							}
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::comparator_off_Id:
						case Tile::comparator_on_Id:
							*piUse=IDS_TOOLTIPS_USE;
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::diode_off_Id:
						case Tile::diode_on_Id:
							*piUse=IDS_TOOLTIPS_USE;
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::redStoneOre_Id:
							if (bUseItemOn)	*piUse=IDS_TOOLTIPS_USE;
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						case Tile::door_iron_Id:
							if(*piUse==IDS_TOOLTIPS_PLACE)
							{
								*piUse = -1;
							}
							*piAction=IDS_TOOLTIPS_MINE;
							break;

						default:
							*piAction=IDS_TOOLTIPS_MINE;
							break;
						}
					}
					break;

				case HitResult::ENTITY:
					eINSTANCEOF entityType = hitResult->entity->GetType();

					if ( (gameMode != NULL) && (gam// 4J Stu - For the tutorial we want to be able to record what items we look at so that we can give hints                                             
						gameMode->getTutorial()->onLookAtEntity(hitResult->entity);
					}

					std::shared_ptr<ItemInstance> heldItem = nullptr;
					if (player->inventory->IsHeldItem())
					{
						heldItem = player->inventory->getSelected();		
					}
					int heldItemId = heldItem != NULL ? heldItem->getItem()->id : -1;
					
					switch(entityType)
					{
					case eTYPE_CHICKEN:
						{
							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;

							std::shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(hitResult->entity);

							if (animal->isLeashed() && animal->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
								break;
							}

							switch(heldItemId)
							{
							case Item::nameTag_Id:
								*piUse=IDS_TOOLTIPS_NAME;
								break;

							case Item::lead_Id:
								if (!animal->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
								break;

							default:
								{
									if(!animal->isBaby() && !animal->isInLove() && (animal->getAge() == 0) && animal->isFood(heldItem))
									{
										*piUse=IDS_TOOLTIPS_LOVEMODE;
					// 4J-JEV: Empty hand.		break;

							case -1: break;                       
							}
						}
						break;

					case eTYPE_COW:
						{
							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;
						
							std::shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(hitResult->entity);

							if (animal->isLeashed() && animal->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
								br// Things to USE						switch (heldItemId)
							{
									                
							case Item::nameTag_Id:
								*piUse=IDS_TOOLTIPS_NAME;
								break;
							case Item::lead_Id:
								if (!animal->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
								break;
							case Item::bucket_empty_Id:
									*piUse=IDS_TOOLTIPS_MILK;
									break;
								default:
									{
										if(!animal->isBaby() && !animal->isInLove() && (animal->getAge() == 0) && animal->isFood(heldItem))
										{
											*piUse=IDS_TOOLTIPS_LOVEMODE;
			// 4J-JEV: Empty hand.							break;

							case -1: break;                       
								}
					// 4J-PB - Fix for #13081 - No tooltip is displayed for hitting a cow when you have nothing in your hand                                                            
							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;

							std::shared_ptr<Animal> animal = dynamic_pointer_cast<Animal>(hitResult->entity);

							if (animal->isLeashed() && animal->getLeashHolder() == player)
							{
					// It's an itemOOLTIPS_UNLEASH;
								break;
							}

		// Things to USE     
							switch(heldItemId)
							{
								                
							case Item::nameTag_Id:
								*piUse=IDS_TOOLTIPS_NAME;
								break;

							case Item::lead_Id:
								if (!animal->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
								break;
// You can milk a mooshroom with either a bowl (mushroom soup) or a bucket (milk)!                                                                 
									*piUse=IDS_TOOLTIPS_MILK;
									break;
								case Item::shears_Id:
									{								
										if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;
										if(!animal->isBaby()) *piUse=IDS_TOOLTIPS_SHEAR;
									}
									break;
								default:
									{
										if(!animal->isBaby() && !animal->isInLove() && (animal->getAge() == 0) && animal->isFood(heldItem))
										{
											*piUse=IDS_TOOLTIPS_LOVEMODE// 4J-JEV: Empty hand.	}
									break;

							case -1: break;                       
							}
						}
						break;

					case eTYPE_BOAT:
						*piAction=IDS_TOOLTIPS_MINE;
						*piUse=IDS_TOOLTIPS_SAIL;
						break;

					case eTYPE_MINECART_RIDEABLE:
						// are we in the minecart already? - 4J-JEV: Doesn't matter anymore.                                                                 
						break;
						
					case eTYPE_MIN// if you have coal, it'll go. Is there an object in hand?
						                                                          
						if (heldItemId == Item::coal_Id) *piUse=IDS_TOOLTIPS_USE;
						break;

					case eTYPE_MINECART_CHEST:
					case eTYPE_MINECART_HOPPER:
						*piAction = IDS_TOOLTIPS_MINE;
						*piUse = IDS_TOOLTIPS_OPEN;
						break;

					case eTYPE_MINECART_SPAWNER:
					case eTYPE_MINECART_TNT:
						*piUse = IDS_TO// can dye a sheep	break;

					case eTYPE_SHEEP:
						{
							                  
							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;

							std::shared_ptr<Sheep> sheep = dynamic_pointer_cast<Sheep>(hitResult->entity);

							if (sheep->isLeashed() && sheep->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
								break;
							}

							switch(heldItemId)
							{
							case Item::nameTag_Id:
								*piUse=IDS_TOOLTIPS_NAME;
								break;

							case Item::lead_Id:
								if (!sheep->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH// convert to tile-based color value (0 is white instead of black)				                                                                  
									int newColor = Colore// can only use a dye on sheep that haven't been sheared));

									                                                        
									if(!(sheep->isSheared() && sheep->getColor() != newColor))
									{
										*piUse=IDS_TOOLTIPS_DYE;
									}
			// can only shear a sheep that hasn't been sheared
								{
									                                                  
									if ( !sheep->isBaby() && !sheep->isSheared() )
									{
										*piUse=IDS_TOOLTIPS_SHEAR;
									}
								}

								break;
							default:
								{
									if(!sheep->isBaby() && !sheep->isInLove() && (sheep->getAge() == 0) && sheep->isFood(heldItem))
									{
										*piUse=IDS_TOOLTIPS_// 4J-JEV: Empty hand.							}
								break;

							case -1: break;                     // can ride a pig	}
						break;

					case eTYPE_PIG:
						{
							                 
							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;

							std::shared_ptr<Pig> pig = dynamic_pointer_cast<Pig>(hitResult->entity);
							
							if (pig->isLeashed() && pig->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
							}
							else if (heldItemId == Item::lead_Id)
							{
								if (!pig->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
							}
							else if (heldItemId == Item::nameTag_Id)
							{
				// does the pig have a saddle?;
							}
							else if (pig->hasSaddle())                               
							{
								*piUse=IDS_TOOLTIPS_MOUNT;
							}
							else if (!pig->isBaby())
							{
								if(player->inventory->IsHeldItem())
								{
									switch(heldItemId)
									{
									case Item::saddle_Id:
										*piUse=IDS_TOOLTIPS_SADDLE;
										break;

									default:
										{
											if (!pig->isInLove() && (pig->getAge() == 0) && pig->isFood(heldItem))
											{
												*piUse=IDS_TOOLTIPS_LOVEMODE;
											}
										}
										break;
					// can be tamed, fed, and made to sit/stand, or enter love modeWOLF:
						                                                               
						{
							std::shared_ptr<Wolf> wolf = dynamic_pointer_cast<Wolf>(hitResult->entity);

							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;

							if (wolf->isLeashed() && wolf->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
								break;
							}

							switch(heldItemId)
							{
							case Item::nameTag_Id:
								*piUse=IDS_TOOLTIPS_NAME;
								break;

							case Item::lead_Id:
								if (!wolf->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
								break;

							case Item::bone_Id:
								if (!wolf->isAngry() && !wolf->isTame())
								{
									*piUse=IDS_TOOLTIPS_TAME;
								}
								else if (equalsIgnoreCase(player->getUUID(), wolf->getOwnerUUID()))
								{
									if(wolf->isSitting())
									{
										*piUse=IDS_TOOLTIPS_FOLLOWME;
									}
									else
									{
										*piUse=IDS_TOOLTIPS_SIT// Use is throw, so don't change the tips for the wolfm::enderPearl_Id:
								                                                      
								break;
							case Item::dye_powder_Id:
								if (wolf->isTame())
								{
									if (ColoredTile::getTileDataForItemAuxValue(heldItem->getAuxValue()) != wolf->getCollarColor())
									{
										*piUse=IDS_TOOLTIPS_DYECOLLAR;
									}
									else if (wolf->isSitting())
									{
										*piUse=IDS_TOOLTIPS_FOLLOWME;
									}
									else
									{
										*piUse=IDS_TOOLTIPS_SIT;
									}
								}
								break;
							default:
								if(wolf->isTame())
								{
									if(wolf->isFood(heldItem))
									{
										if(wolf->GetSynchedHealth() < wolf->getMaxHealth())
										{
											*piUse=IDS_TOOLTIPS_HEAL;
										}
										else
										{
											if(!wolf->isBaby() && !wolf->isInLove() && (wolf->getAge() == 0))
										// break out hereiUse=IDS_TOOLTIPS_LOVEMODE;
											}
										}
										                 
										break;
									}

									if (equalsIgnoreCase(player->getUUID(), wolf->getOwnerUUID()))
									{
										if(wolf->isSitting())
										{
											*piUse=IDS_TOOLTIPS_FOLLOWME;
										}
										else
										{
											*piUse=IDS_TOOLTIPS_SIT;
										}
									}
								}
								break;
							}
						}
						break;
					case eTYPE_OCELOT:
						{
							std::shared_ptr<Ocelot> ocelot = dynamic_pointer_cast<Ocelot>(hitResult->entity);

							if(player->isAllowedToAttackAnimals()) *piAction=IDS_TOOLTIPS_HIT;
						
							if (ocelot->isLeashed() && ocelot->getLeashHolder() == player)
							{
								*piUse = IDS_TOOLTIPS_UNLEASH;
							}
							else if (heldItemId == Item::lead_Id)
							{
								if (!ocelot->isLeashed()) *piUse = IDS_TOOLTIPS_LEASH;
							}
							else if (heldItemId == Item::nameTag_Id)
							{
								*p// 4J-PB - if you have a raw fish in your hand, you will feed the ocelot rather than have it sit/follow                                                                                     
								if(ocelot->isFood(heldItem))
								{
									if(!ocelot->isBaby())
									{
										if(!ocelot->isInLove())
										{
											if(ocelot->getAge() == 0)
											{
												*piUse=IDS_TOOLTIPS_LOVEMODE;
											}
										}
										else
										{
											*piUse=IDS_TOOLTIPS_FEED;									
										}
									}

								}
								else if (equalsIgnoreCase(player->getUUID(), ocelot->getOwnerUUID()) && !ocelot->isSittingOnTile() )
								{
									if(ocelot->isSitting())
									{
										*piUse=IDS_TOOLTIPS_FOLLOWME;
									}
									else
									{
										*piUse=IDS_TOOLTIPS_SIT;
									}
								}
							}
							else if(heldItemId >= 0)
							{
								if (ocelot->isFood(heldItem)) *piUse=IDS_TOOLTI// Fix for #58576 - TU6: Content: Gameplay: Hit button prompt is available when attacking a host who has "Invisible" option turned on                                                                                     
							std::shared_ptr<Player> TargetPlayer = dynamic_po// This means they are invisible, not just that they have the privilegeisiblePrivilege())                                                                        
							{
								if( app.GetGameHostOption(eGameHostOption_PvP) && player->isAllowedToAttackPlayers())
								{
									*piAction=IDS_TOOLTIPS_HIT;
								}
							}
						}
						break;

					case eTYPE_ITEM_FRAME:
						{
							std:// is the frame occupied?emFrame = dynamic_pointer_cast<ItemFrame>(hitResult->ent// rotate the item                     
							if(itemFrame->getItem()!=NULL)
							{
						// is there an object in hand?piUse=IDS_TOOLTIPS_ROTATE;
							}
							else
							{
								                              
								if(heldItemId >= 0) *piUse=IDS_TOOLTIPS_PLACE;
							}

			// 4J-JEV: Cannot leash villagers.				}
						break;

					case eTYPE_VILLAGER:
						{
							                                  

							std::shared_ptr<Villager> villager = dynamic_pointer_cast<Villager>(hitResult->entity);
							if (!villager->isBaby())
							{
								*piUse=IDS_TOOLTIPS_TRADE;
							}
							*piAction=IDS_TOOLTIPS_HIT;
						}
						break;

					case eTYPE_ZOMBIE:
						{
							std::shared_ptr<Zombie> zomb = dynamic_pointer_cast<Zombie>(hitR//zomb->hasEffect(MobEffect::weakness) - not present on client.denAppleItem *) Item::apple_gold;

							                                                               
							if ( zomb->isVillager() && zomb->isWeakened() && (heldItemId == Item::apple_gold_Id) && !goldapple->isFoil(heldItem) )
							{
								*piUse=IDS_TOOLTIPS_CURE;
							}
							*piAction=IDS_TOOLTIPS_HIT;
						}
						break;

					case eTYPE_HORSE:
						{
							std::shared_ptr<EntityHorse> horse = dynamic_pointer_cast<EntityHorse>(hitResult->entity);
							
							bool heldItemIsFood = false, heldItemIsLove = false, heldItemIsArmour = false;
							
							switch( heldItemId )
							{
								case Item::wheat_Id:
								case Item::sugar_Id:
								case Item::bread_Id:
								case Tile::hayBlock_Id:
								case Item::apple_Id:
									heldItemIsFood = true;
									break;
								case Item::carrotGolden_Id:
								case Item::apple_gold_Id:
									heldItemIsLove = true;
									heldItemIsFood = true;
									break;
								case Item::horseArmorDiamond_Id:
								case Item::horseArmorGold_Id:
								case Item::horseArmorMetal_Id:
									heldItemIsArmour = true;
									break;
							}

							if (horse->isLeashed() && horse->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
							}
							else if ( heldItemId == Item::lead_Id)
							{
								if (!horse->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
							}
							else if (heldItemI// 4J-JEV: Can't ride baby horses due to morals. IDS_TOOLTIPS_NAME;
							}
							else if (horse->isBab// 4j - Can feed foles to speed growth.              
							{
								if (heldItemIsFood)
								{
									                                       
									*piUse = IDS_TOOLTIPS_FEED;
				// 4j - Player not holding anything, ride and attempt to break untamed horse.ItemId == -1)
								{
									                                                                   // 4j - Attempt to make it like you more by feeding it.	}
								else if (heldItemIsFood)
								{
									                                                       
									*piUse = IDS_TOOLTIPS_FEED;
								}
							}
							else if (	player->isSneaking() 
									||	(heldI// 4j - Access horses inventory				||	(horse->canWearArmor() && heldItemIsArmour)
									)
							{
								                               
								if (*piUse == -1) *piUse = IDS_TOOLTIPS_OPEN;
							}
							else if (	horse->canW// 4j - Attach saddle-bags (chest) to donkey or mule.				&&	(heldItemId == Tile::chest_Id) )
							{
								                                                     
								*piUse = IDS_TOOLTIPS// 4j - Different food to mate horses.se->isReadyForParenting()
									&&	heldItemIsLove )
							{
								                                      
								*piUse = IDS_TOOLTIPS_LOVEMODE// 4j - Horse is damaged and can eat held item to healgetHealth() < horse->getMaxHealth()) )
							{
								                   // 4j - Ride tamed horse.          
								*piUse = IDS_TOOLTIPS_HEAL;
							}
							else
							{
								                         
								*piUse = IDS_TOOLTIPS_MOUNT;
							}
							
							if (player->isA// 4J-JEV: Enderdragon cannot be named.TOOLTIPS_HIT;
						}
						break;

					case eTYPE_ENDERDRAGON:
						                                       
						*piAction = IDS_TOOLTIPS_HIT;
						break;

					case eTYPE_LEASHFENCEKNOT:
						*piAction = IDS_TOOLTIPS_UNLEASH;
						if (heldItemId == Item::lead_Id && LeashItem::bindPlayerMobsTest(player, level, player->x, player->y, player->z))
						{
							*piUse = IDS_TOOLTIPS_ATTACH;
						}
						else
						{
							*piUse = IDS_TOOLTIPS_UNLEASH;
						}
						break;

					default:
						if (  hitResult->entity->instanceof(eTYPE_MOB) )
						{
							std::shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(hitResult->entity);
							if (mob->isLeashed() && mob->getLeashHolder() == player)
							{
								*piUse=IDS_TOOLTIPS_UNLEASH;
							}
							else if (heldItemId == Item::lead_Id)
							{
								if (!mob->isLeashed()) *piUse=IDS_TOOLTIPS_LEASH;
							}
							else if (heldItemId == Item::nameTag_Id)
							{
								*piUse=IDS_TO// 4J-JEV: Don't set tooltips when we're reloading the skin, it'll crash.ak;	
					}
					break;
				}
			}
		}

		                                                                         
		if (!ui.IsReloadingSkin()) ui.SetTooltips( iPad, iA, iB, iX, iY, iLT, iRT, iLB, iRB, iLS, iRS);

		int wheel = 0;
		if (InputManager.GetValue(iPad, MINECRAFT_ACTION_LEFT_SCROLL, true) > 0 && gameMode->isInputAllowed(MINECRAFT_ACTION_LEFT_SCROLL) )
		{
			wheel = 1;
		}
		else if (InputManager.GetValue(iPad, MINECRAFT_ACTION_RIGHT_SCROLL,true) > 0 && gameMode->isInputAllowed(MINECRAFT_ACTION_RIGHT_SCROLL) )
		{
			wheel = -1;
		}
		if (wheel != 0)
// 4J Stu - For the tutorial we want to be able to record what items we are using so that we can give hints		{
				                                                                                        // Update presence 
				gameMode->getTutorial()->onSelectedItemChanged(player->inventory->getSelected());
			}

			                  
			player->updateRichPresence();

			if (options->isFlying)
			{
				if (wheel > 0) wheel = 1;
				if (wheel < 0) wheel = -1;

				options->flySpeed += wheel * .25f;
			}
		}

		if( gameMode->i//if(InputManager.ButtonPressed(iPad, MINECRAFT_ACTION_ACTION) )uttonsPres//printf("MINECRAFT_ACTION_ACTION ButtonPressed");                                                       
			{
				                                                  
				player->handleMouseClick(0);
				player->lastClickTick[0] = ticks;
			}

			if (InputManager.Bu//printf("MINECRAFT_ACTION_ACTION ButtonDown");s - player->lastClickTick[0] >= timer->ticksPerSecond / 4)
			{
				                                               
				player->handleMouseClick(0);
				player->lastClickTick[0] = ticks;
			}

			if(InputManager.ButtonDown(iPad, MINECRAFT_ACTION_ACTION) )// 4J Stu - This is how we used to handle the USE action. It has now been replaced with the block below which is more like the way the Java game does it,   // however we may find that the way we had it previously is more fun to play.   /*
		if ((InputManager.GetValue(iPad, MINECRAFT_ACTION_USE,true)>0) && gameMode->isInputAllowed(MINECRAFT_ACTION_USE) )
		{
		handleMouseClick(1);
		lastClickTick = ticks;
		}
		*/                                                                                                                   
		if( player->isUsingItem() )
		{
			if(!InputManager.ButtonDown(iPad, MINECRAFT_ACTION_USE)) gameMode->releaseUsingItem(player);
// 4J - attempt to handle click in special creative mode fashion if possible (used for placing blocks at regular intervals)                                                                                                                   
				// If this handler has put us in lastClick_oldRepeat mode then it is because we aren't placing blocks - behave largely as the code used to                                                                                 // If we've already handled the click in creativeModeHandleMouseClick then just record the time of this click
					                                                                                            // Otherwise just the original game code for handling autorepeatlickTick[1] = ticks;
					}
					else
					{
						                                                                
						if (InputManager.ButtonDown(iPad, MINECRAFT_ACTION_USE) && ticks - player->lastClickTick[1] >= timer->ticksPerSecond / 4)
						{
					// Consider as a click if we've had a period of not pressing the button, or we've reached auto-repeat time since the last time     // Auto-repeat is only considered if we aren't riding or sprinting, to avoid photo sensitivity issues when placing fire whilst doing fast things     // Also disable repeat when the player is sleeping to stop the waking up right after using the bed            
				                                                                                                  
				bool firstClick = ( player->lastClickTick[1] == 0 );
				bool autoRepeat = ticks - player->lastClickTick[1] >= timer->ticksPerSecond / 4;
				if ( player->isRiding() || player->isSprinting() || p// If the player has just exited a bed, then delay the time before a repeat key is allowed without releasing{
					                                                                                                            
					if(player->isSleeping() ) player->lastClickTick[1] = ticks + (timer->ticksPerSecond * 2);
					if( firstClic// If the player has just exited a bed, then delay the time before a repeat key is allowed without releasing
						                                                                                                            
						if(wasSleeping) player->lastClickTick[1] = ticks + (timer->ticksPerSecond * 2);
						else player->lastClickTick[1] = ticks;
					}
				}
				else
				{
					player->lastClickTick[1] = 0;
				}
			}
		}

		if(app.DebugSettingsOn())
		{
			if (player->ullButtonsPressed & ( 1LL << MINECRAFT_AC
#ifdef _DEBUG_MENUS_ENABLED player->ChangePlayerSkin();
			}#ifndef __PSVITA__->mi// 4J-PB - debugoverlay for primary player only             
		if(app.DebugSettingsOn())
		{
                  
			                                               
			if(iPad==Pr#ifndef _CONTENT_PACKAGEad())
			{
				if((player->ullButtonsPressed&(1LL<<MI#ifdef _XBOXON_RENDER_DEBUG)) )
				{
                        

					#elsens->re// 4J Stu - The xbox uses a completely different way of navigating to this sceneons->renderDebug,iPad);
     
					                                       #endif #endif                            
					ui.NavigateToScene(0, eUIScene_DebugOverlay, NULL, eUILayer_Debug);
      
      
				}

				//shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(Creeper::_class->newInstance( level ));tackEn//shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(Wolf::_class->newInstance( level ));                       
					                                                                                      
					std::shared_ptr<Mob> mob = dynamic_pointer_cast<Mob>(std::shared_ptr<Spider>(new Spider( level )));
					mob->moveTo(player->x+1, player->y, player->z+1, level->random->nextFloat() * 360, 0);
					level->addEntity(mob);
				}
			}

			if( (player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_FLY_TOGGLE)) )
			{
				pl#endif // PSVITA		bugfl#endif !player->abilities.debugflying;
				player->abilities.flying = !player->abilities.flying;
			}
                  
		}
      

		if((player->ullButtonsPr// 4J-PB - changing this to be per player_PERSON)) && gameMode->isInputAllowed(MINECRAFT_ACTION_RENDER_THIRD_//options->thirdPersonView = !options->thirdPersonView;  
			player->SetThirdPersonView((player->ThirdPersonView()+1)%3);
			                                                       
		}

		if((player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_GAME_INFO)) && gameMode->isInputAllowed(MINECRAFT_ACTION_GAME_INFO))
		{
			ui.NavigateToScene(iPad,eUIScene_InGameInfoMenu);
			ui.PlayUISFX(eSFX_Press);
		}

		if((player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_INVENTORY)) && gameMode->isInputAllowed(MINECRAFT_ACTION_INVENTORY))
		{
			std::shared_ptr<MultiplayerLocalPlayer> player = Minecraft::GetInstance()->player;
			ui.PlayUISFX(eSFX_Press);

			if(gameMode->isServerControlledInventory())
			{
				player->sendOpenInventory();
			}
			else
			{
				app.LoadInventoryMenu(iPad,player);
			}
		}

		if((player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_CRAFTING)) && gameMode->is// 4J-PB - reordered the if statement so creative mode doesn't bring up the crafting tableayer// Fix for #39014 - TU5:  Creative Mode:  Pressing X to access the creative menu while looking at a crafting table causes the crafting menu to display                                              // Creative mode                                                                         
			if// 4J-PB - Microsoft request that we use the 3x3 crafting if someone presses X while at the workbenchreativeMenu(iPad,player);
			}
			                                                                                                     
			else if ((hitResult!=NULL)//ui.PlayUISFX(eSFX_Press);Resul//app.LoadXuiCrafting3x3Menu(iPad,player,hitResult->x, hitResult->y, hitResult->z);Bench_Id))
			{
				                           
				                                                                                   
				bool usedItem = false;
				gameMode->useItemOn(player, level, nullptr, hitResult->x, hitResult->y, hitResult->z, 0, hitResult->pos, false, &usedItem);
			}
			else
			{
				ui.PlayUISFX#ifdef _DURANGO			app.LoadCrafting2x2Menu(iPad,player);
			}
		}

		if ( (playe#endifButtonsPressed&(1LL<<MINECRAF"PAUSE PRESS PROCESSING - ipad = %d, NavigateToScene\n"ButtonsPressed&(1LL<<ACTION_MENU_GTC_PAUSE))
      
			)
		{
			app.DebugPrintf(                                                       ,player->GetXboxPad());
			ui.PlayUISFX(eSFX_Press);
			ui.NavigateToScene(iPad, eUIScene_PauseMenu, NULL, eUILayer_Scene);
		}

		if((player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_DROP)) && gameMode->isInputAllowed(M#ifdef __PSVITA__ROP// 4J-PB - use the touchscreen for quickselectuttonsPressed=player->ullButtonsPressed;
		
		bool selected = false;
                 
		                                              
		SceTouchData* pTouchData = InputManager.GetTouchPadData(iPad,false);

		if(pTouchData->reportNum==1)
		{
			int iHudSize=app.GetGameSettings(iPad,eGameSetting_UISize);
			int iYOffset = (app.GetGameSettings(ProfileManager.GetPrimaryPad(),eGameSetting_Tooltips) == 0) ? iToolTipOffset : 0;
			if((pTouchData->report[0].x>QuickSelectRect[iHudSize].left)&&(pTouchData->report[0].x<QuickSelectRect[iHudSize].right) &&
				(pTouchData->report[0].y>QuickSelectRect[iHudSize].top+iYOffset)&&(pTouchData->report[0].y<QuickSelectRect[iHudSize].bottom+iYOffset))
			{
				player->inventory->sel"Touch %d\n"hData->report[0].x-QuickSelectRect[iHudS#endifeft)/QuickSelectBoxWidth[iHudSize];
				selected = true;
				app.DebugPrintf(            ,player->inventory->selected);""		}
		}
      
		if( selected || wheel != 0 || (player->ullButtonsPressed&(// Dropping items happens over network, so if we only have one then assume that we dropped it and should hide the itemyer->getSelectedItem();
			                                                                                                                      
			int iCount=0;
			
			if(selectedItem != NULL) iCount=selectedItem->GetCount();
			if(selectedItem != NULL && !( (player->ullButtonsPressed&(1LL<<MINECRAFT_ACTION_DROP)) && selectedItem->GetCount() == 1))
			{
				itemName = selectedItem->getHoverName();
			}
			if( !(player->ullButtonsPressed&(1// 4J-PBCRAFT_ACTION_DROP)) || (selectedItem != NULL && selectedItem->GetCount() <= 1) ) ui.SetSelectedItem( iPad, itemName );
		}
	}
	else
	{
		    // monitor for keyboard inputlu// #ifndef _CONTENT_PACKAGE >// 	if(!(ui.GetMenuDisplayed(iPad)))N_// 	{CA// 		WCHAR wchInput;ee// 		if(InputManager.InputDetected(iPad,&wchInput))  // 		{  // 			printf("Input Detected!\n");  //  // 			// see if we can react to this  // 			if(app.GetXuiAction(iPad)==eAppAction_Idle)  // 			{	 // 				app.SetAction(iPad,eAppAction_DebugText,(LPVOID)wchInput);  // 			}  // 		} 
// 	}  // #endif 
#if 0    // 4J - TODO - some replacement for input handling...                                                
	       
	      
	     
	         
      
	                                                     
	if (screen == NULL || screen.passEvents)
	{
		while (Mouse.next())
		{
			long passedTime = System.currentTimeMillis() - lastTickTime;
			if (passedTime > 200) continue;

			int wheel = Mouse.getEventDWheel();
			if (wheel != 0) {
				player->inventory.swapPaint(wheel);

				if (options.isFlying) {
					if (wheel > 0) wheel = 1;
					if (wheel < 0) wheel = -1;

					options.flySpeed += wheel * .25f;
				}
			}

			if (screen == null) {
				if (!mouseGrabbed && Mouse.getEventButtonState()) {
					grabMouse();
				} else {
					if (Mouse.getEventButton() == 0 && Mouse.getEventButtonState()) {
						handleMouseClick(0);
						lastClickTick = ticks;
					}
					if (Mouse.getEventButton() == 1 && Mouse.getEventButtonState()) {
						handleMouseClick(1);
						lastClickTick = ticks;
					}
					if (Mouse.getEventButton() == 2 && Mouse.getEventButtonState()) {
						handleGrabTexture();
					}
				}
			} else if (screen != null) {
				screen.mouseEvent();
			}
		}

		if (missTime > 0) missTime--;

		while (Keyboard.next()) {
			player->setKey(Keyboard.getEventKey(), Keyboard.getEventKeyState()/*
				* if (Keyboard.getEventKey() == Keyboard.KEY_F4) { new
				* PortalForcer().createPortal(level, player); continue; }
				*/nue;
	/*
				* if (Keyboard.getEventKey() == Keyboard.KEY_RETURN) {
				* level.pathFind(); continue; }
				*/                                   

				                                                                                                        

				if (screen != null) {
					screen.keyboardEvent();
				} else {
					if (Keyboard.getEventKey() == Keyboard.KEY_ESCAPE) {
						pa//                        if (Keyboard.getEventKey() == Keyboard.KEY_P) {eyboar//                            gameMode = new DemoMode(this);
					//                            selectLevel(CreateWorldScreen.findAvailableFolderName(getLevelSource(), "Demo"), "Demo World", 0L);      //                            setScreen(null);      //      //                        }                                                    
					                                              
					  
					                           

					if (Keyboard.getEventKey() == Keyboard.KEY_F1) {
						options.hideGui = !options.hideGui;
					}
					if (Keyboard.getEventKey() == Keyboard.KEY_F3) {
						options.renderDebug = !options.renderDebug;
					}
					if (Keyboard.getEventKey() == Keyboard.KEY_F5) {
						options.thirdPersonView = !options.thirdPersonView;
					}
					if (Keyboard.getEventKey() == Keyboard.KEY_F8) {
						options.smoothCamera = !options.smoothCamera;
					}
					if (DEADMAU5_CAMERA_CHEATS) {
						if (Keyboard.getEventKey() == Keyboard.KEY_F6) {
							options.isFlying = !options.isFlying;
						}
						if (Keyboard.getEventKey() == Keyboard.KEY_F9) {
							options.fixedCamera = !options.fixedCamera;
						}
						if (Keyboard.getEventKey() == Keyboard.KEY_ADD) {
							options.cameraSpeed += .1f;
						}
						if (Keyboard.getEventKey() == Keyboard.KEY_SUBTRACT) {
							options.cameraSpeed -= .1f;
							if (options.cameraSpeed < 0) {
								options.cameraSpeed = 0;
							}
						}
					}

					if (Keyboard.getEventKey() == options.keyBuild.key) {
						setScreen(new InventoryScreen(player));
					}

					if (Keyboard.getEventKey() == options.keyDrop.key) {
						player->drop();
					}
					if (isClientSide() && Keyboard.getEventKey() == options.keyChat.key) {
						setScreen(new ChatScreen());
					}
				}

				for (int i = 0; i < 9; i++) {
					if (Keyboard.getEventKey() == Keyboard.KEY_1 + i) player->inventory.selected = i;
				}
				if (Keyboard.getEventKey() == options.keyFog.key) {
					options.toggle(Options.Option.RENDER_DISTANCE, Keyboard.isKeyDown(Keyboard.KEY_LSHIFT) || Keyboard.isKeyDown(Keyboard.KEY_RSHIFT) ? -1 : 1);
				}
			}
		}

		if (screen == null) {
			if (Mouse.isButtonDown(0) && ticks - lastClickTick >= timer.ticksPerSecond / 4 && mouseGrabbed) {
				handleMouseClick(0);
				lastClickTick = ticks;
			}
			if (Mouse.isButtonDown(1) && ticks - lastClickTick >= timer.ticksPerSecond / 4 && mouseGrabbed) {#endifandleMouseClick(1);
				lastClickTick = ticks;
			}
		}

		handleMouseDown(0, screen == null && Mouse.isButtonDown(0) && mouseGrabbed);
	}
      

	if (level != NULL)
	{
		if (// 4J Changed - We are setting the difficulty the same as the server so that leaderboard updates work correctly->e//level->difficulty = options->difficulty;   //if (level->isClientSide) level->difficulty = Difficulty::HARD;                                 
		 //app.DebugPrintf("Minecraft::tick - Difficulty = %d",options->difficulty);                                 
		if( !level->isClientSide )
		{
			   "Game renderer tick"                                                    
			level->difficul// 4J - we want to tick each level once only per frame, and do it when a player that is actually in that level happens to be active.dEv// This is important as things that get called in the level tick (eg the levellistener) eventually end up working out what the current   // level is by determing it from the current player. Use flags here to make sure each level is only ticked the once.                          
		                                                          
#ifndef DISABLE_LEVELTICK_THREAD                         "levelTickEventQueue waitForFinish"Flags;
		if( bFirst )
		{
			levelsTickedFlags = 0;
               #endif // DISABLE_LEVELTICK_THREADdEvent(0,                       // 4J added );
			levelTickEventQueue->waitFor// 4J added		PIXEndNamedEvent();
          // 4J added             
			SparseLightStorage::tick();	           
			CompressedTileStorage::tick();	           
			// Don't tick if the current player isn't in this levelgned // 4J - this doesn't fully tick the animateTick here, but does register this player's position. The actual    // work is now done in Level::animateTickDoWork() so we can take into account multiple players in the one level.                             
			                                                                                                                
			if (!pause && levels[i] != NULL)// Don't tick further if we've already ticked this level this frame), Mth::floor(player->z));

			if( levelsTickedFlags & ( 1 <"Level renderer tick"                                                                
		// if (!pause && player!=null) {
			// if (player != null && !level.entities.contains(player)) {leve// level.addEntity(player);dNam// }ent(// }		                                
			                                                            
			                           
			    
			    
"Level entity tick" NULL )
			{
				if (!pause)
				{
					if (levels[i]->skyFlashTime > 0// optimisation to set the culling off early, in parallel with other stuff)#if defined __PS3__ && !defined DISABLE_SPU_CODEedEve// kick off the culling for all valid players in this level                                
                                                
				                                                           
				int currPlayerIdx = getLocalPlayerIdx();
				for( int idx = 0; idx < XUSER_MAX_COUNT; idx++ )
				{
					if(localplayers[idx]!=NULL)
					{
						if( localplayers[idx]->level == levels[i] )
						{
							setLocalPlayerIdx(idx);
							gameRenderer->setupCamera(timer->a, i);
							Camera::prepare(localplayers[idx], localplayers[idx]->ThirdPersonView() == 2);
							std::shared_ptr<LivingEntity> cameraEntity = cameraTargetPlayer;
							double xOff = cameraEntity->xOld + (cameraEntity->x - cameraEntity->xOld) * timer->a;
							double yOff = cameraEntity->yOld + (cameraEntity->y - cameraEntity->yOld) * timer->a;
							double zOff = cameraEntity->zOld + (cameraEntity->z - cameraEntity->zOld) * timer->a;
							FrustumCuller frustObj;
							Culler *frustum = &frustObj;
							MemSect(0);
#endif   // __PS3__epare(// 4J Stu - We are always online, but still could be
         // pausedtum, 0);
						}
// || isClientSide())ocalPlayerId//app.DebugPrintf("Minecraft::tick spawn settings - Difficulty = %d",options->difficulty);          
				if (!pause)                      
				{
					                              "Level tick"   #ifdef DISABLE_LEVELTICK_THREAD              
					level#else>setSpawnSettings(level->difficulty > 0, true);
#endif   // DISABLE_LEVELTICK_THREAD   );
                               
					levels[i]->tick();
     
					levelTickEve"Particle tick"ent(levels[i]);
                                  
					PIXEndNamedEvent// 4J Stu - Keep ticking the connections if paused so that they don't time out);
			if (!pause) particleEngine->tick(// player->tick();vent#ifdef __PS3__  // 	while(!g_tickLevelQueue.empty())  // 	{ // 		Level* pLevel = g_tickLevelQueue.front();c// 		g_tickLevelQueue.pop(); // 		pLevel->tick(); // 	}; 
#endif      // if (Keyboard.isKeyDown(Keyboard.KEY_NUMPAD7) ||  // Keyboard.isKeyDown(Keyboard.KEY_Q)) rota++;  // if (Keyboard.isKeyDown(Keyboard.KEY_NUMPAD9) ||  // Keyboard.isKeyDown(Keyboard.KEY_E)) rota--;  // 4J removed  //lastTickTime = System::currentTimeMillis();                                     //    System.out.println("FORCING RELOAD!");		// 4J - removed     
	                                             
}

void Minecraft::reloadSound()
{
	                                                             
	soundEngine = new SoundEngine();
	soundEngine->init(options);
	bgLoader->forceReload();
}

bool Minecraft::isClientSide()
{
	return level != NULL && level->isClientSide;
}

void Minecraft::selectLevel(ConsoleSaveFile *saveFile, const std::wstring& levelId, const std::wstring& levelName, LevelSettings *levelSettings)
{
	}

bool Minecraft::saveSlot(int slot, const std::wstring& name)
//this->level = NULL;ool Minecraft::loadSlot(const // 4J Stu - This code was within setLevel, but I moved it out so that I can call it at a better
// time when exiting from an online game message);
}

                              //4J Gordon: Force a stats save                                 //4J Gordon: If the player is signed in, save the leaderboardsceStatsSave(int idx)
{
	                               
	stats[idx]->save(idx, true);

	                                                              
	if( ProfileManager.IsSignedInLive(idx) )
	{
		int tempLockedProfile = ProfileManager.GetL// 4J Addede();
		ProfileManager.SetLockedProfile(idx);
		stats[idx]->saveLeaderboards();
		ProfileManager.SetLockedProfile(tempLockedProfile);
	}
}

           
MultiPlayerLeve// 4J Stu - Removed as redundant with default values in params.e//void Minecraft::setLevel(Level *level, bool doForceStatsSave /*= true*/)v//{[//	setLevel(level, -1, NULL, doForceStatsSave); //}  // Also causing ambiguous call for some reason // as it is matching shared_ptr<Player> from the func below with bool from this one //void Minecraft::setLevel(Level *level, const wstring& message, bool doForceStatsSave /*= true*/) //{ //	setLevel(level, message, NULL, doForceStatsSave); //}                                                                                         
   
                                                    
   

void Minecraft::forceaddLevel(MultiPlayerLevel *level)
{
	int dimId = level->dimension->id;
	if (dimId == -1)/*=-1*/[1] = level;
	else if(dimId == 1) levels[2] /*=NULL*/	else levels[0] = level;/*=true*/Minecraft::setLevel(MultiPlayer/*=false*/el, int message        , std::shared_ptr<Player> forceInsertPlayer          , bool doForceStatsSave          , bool bPrimaryPlayerSignedOut           )
{
	EnterCriticalSection(&m_setLevelCS);
	bool playerAdded = false;
	this->cameraTargetPl// 4J-PB - since we now play music in the menu, just let it keep playinger//soundEngine->playStreaming(L"", 0, 0, 0, 0, 0);r->// 4J - stop update thread from processing this level, which blocks until it is safe to move on - will be re-enabled if we set the level to be non-NULL                                                                                              // 4J We only need to save out in multiplayer is we are setting the level to NULLdat// If we ever go back to making single player only then this will not work properly!                                                  // 4J Stu - This is really only relevant for single player (ie not what we do at the moment)        
		if (levels[i] != NULL && level == NULL)
		{
			                                      // 4J Stu - Added these for the case when we exit a level so we are setting the level to NULLer!=// The level renderer needs to have it's stored level set to NULL so that it doesn't break next time we set one                                   
			                                                                                                               
			if (levelRenderer != NULL)
			{
				for(DWORD p = 0; p <// 4J If we are setting the level to NULL then we are exiting, so delete the levelsf (particleEngine != NULL) particleEngine->setLevel(NULL);
		}
	}
	                              // Both level share the same savedDataStorage        
	if( level == NULL )
	{
		if(levels[0]!=NULL)
		{
			delete levels[0];
			levels[0] = NULL;

			                                             
			if(levels[1]!=NULL) levels[1]->savedDataStorage = NULL;
	// Delete all the player objectsdelete levels[1];
			levels[1] = NULL;
		}
		if(levels[2]!=NULL)
		{
			delete levels[2];
			levels[2] = NULL;
		}

		                                
		for(unsigned int idx = 0; idx < XUSER_MAX_COUNT; ++idx)
		{
			std::shared_ptr<MultiplayerLocalPlayer> mplp = localplayers[idx];
			if(mplp != NULL && mplp->connection != NULL )
			{
				delete mplp->connection;
				mplp->connection = NULL;
			}

			if( localgameModes[idx] != NULL )
			{
				delete localgameModes[idx];
				localgameModes[idx] = NULL;
			}

			if( m_pendingLocalConnect// If we are removing the primary player then there can't be a valid gamemode left anymore, thiss[i// pointer will be referring to the one we've just deleted                      // Remove references to player                                          
		                                                          
		gameMode = NULL;
		                              
		player = nullptr;
		cameraTargetPlayer = nullptr;
		EntityRenderDispatcher::instance->cameraEntity = nullptr;
		TileEntityRenderDispatcher::instance->cameraEntity = nullptr;
	}
	this->level = level;

	if (level !// If no player has been set, then this is the first level to be set this game, so set upse // a primary player & initialise some other things = level;

		                                                                                         
		                                                  
		if (player == NULL)
		{
			int iPrimaryPlayer = ProfileManager.GetPrimaryPad();

			player = gameMode->createPlayer(level);

			PlayerUID playerXUIDOffline = INVALID_XUID;
			PlayerUID playerXUIDOn#ifdef __PSVITA__ID;
			ProfileManager.GetXUID(iPrimaryPlayer,&playerXUIDOffline,false);
			ProfileManager.GetXUID// player doesn't have an online UID, set it from the player nameCGameNetworkManager::usingAdhocMode() && p#endifUIDOnline.getOnlineID()[0] == 0)
			{
				                                                                 
				playerXUIDOnline.setForAdhoc();
			}
      
			player->setXuid(playerXUIDOffline);
			player->setOnlineXuid(playerXUIDOnline);

			player->m_displayName = ProfileManager.GetDisplayName(iPrimaryPlayer);



			player->resetPos();
			gameMode->initPlayer(player);
			
			player->SetXboxPad(iPrimaryPlayer);

			for(int i=0;i<XUSER_MAX_COUNT;i++)
			{
				// gameMode.initPlayer(player);NULL;
				if( i != iPrimaryPlayer ) localgameModes[i] = NULL;
			}
		}

		if (player != NULL)
		{
			player->resetPos();
			                               
			if (level != NULL)
			{
				level->addEntity(player);
				playerAdded = true;
			}
		}

		if(player->input != NULL) delete player->input;
		player->input = new I
#if 0;

// 4J - removed - we don't use ChunkCache anymorevel(player->GetXboxPad(), level);
		if (particleEngine != NULL) particleEngine->setLevel(level);
      
		                                  // 4J - these had a Mth::floor which seems unrequiredurce();
		if (dynamic_cast<ChunkCache *>(cs) != NULL)
		{
			ChunkCache *spcc = (ChunkCache *)cs;

			   #endif                                            
			int xt = ((int) player->x) >> 4;
			int zt = ((int) player->z) >> 4;

			spcc->centerOn(xt, zt);
		}
      
		gameMode->adjustPlayer(player);

		for(in// 4J - allow update thread to start processing the level now both it & the player should be okportAssignments();

		this->cameraTargetPlayer = player;

		                                         // Clear all players if the new level is NULL         
		gameRenderer->EnableUpdateThread();
	}
	else
	{
		levelSource->clearAll();
		player = nullptr;

		                                             
		for(int i=0;i<XUSER_MAX_COUNT;i++)
		{
			if( m_pendingLocalConnections[i] !=//    System.gc();	// 4J - removedi]// 4J removed	m//this->lastTickTime = 0;i] = NULL;
			localplayers[i] = nullptr;
			localgameModes[i] = NULL;
		}
	}

	                                  
	             
	                         
	LeaveCriticalSection(&m_setLevelCS);
}

void Minecraft::prepareLevel(int title)
{
	if(progressRenderer != NULL)
	{
		this->progressRenderer->progressStart(title);
		this->progressRenderer->progressStage(IDS_PROGRESS_BUILDING_TERRAIN);
	}
	int r = 128;
	if (gameMode->isCutScene()) r = 64;
	int pp = 0;
	int max = r * 2 / 16 + 1;
	max = max * max;
	ChunkSource *cs = level->ge
#if 0So  // 4J - removed - we don't use ChunkCache anymores();
	if (player != NULL)
	{
		spawnPos->x = (int) player->x;
		spawnPos->z = (int) player->z;
	}
      
	                                       #endif    
	if (dynamic_cast<ChunkCache *>(cs)!=NULL)
	{
		ChunkCache *spcc = (ChunkCache *) cs;

		spcc->centerOn(spawnPos->x >> 4, spawnPos->z >> 4);
	}
      

	for (int x = -r; x <= r; x += 16)
	{
		for (int z = -r; z <= r; z += 16)
		{
			if(progressRenderer != NULL) this->progressRenderer->progressStagePercentage((pp++) * 100 / max);
			level->getTile(spawnPos->x + x, 64, spawnPos->z + z);
			if (!gameMode->isCutScene()) {
			}
		}
	}
	delete spawnPos;
	if (!gameMode->isCutScene())
	{
		if(pr//return levelRenderer->gatherStats1();enderer->p"Time to autosave: "GRESS_SIMULATING_WORLD);
		max = 2000;
}
}

std::wstring "s"ecraft::gatherStats1()
{
	                                       
	return L            //return levelRenderer->gatherStats2();.SecondsToAutosave() ) + L   ;
}

std::wstring Minecraft::gatherStats2()
{
	return g_Netw//return L"P: " + particleEngine->countParticles() + L". T: " + level->gatherStats();necraft::gatherStats3()
{
	return g_NetworkManager.GatherRTTStats();
	                                                                                     
}

std::wstring Minecraft::gatherStats4()
{
	// 4J - don't do updating whilst we are adjusting the player & localplayer arrayd, int dimension, int newEntityId)
{
	gameRenderer->DisableUpdateThread();                                                                                 
	std::shared_ptr<MultiplayerLocalPlayer> localPlayer = localplayers[iPad];

	level->validateSpawn();
	level->removeAllPendingEntity// 4J-PB - copy and set the players xbox padlevel->removeEntity(localPlayer);
	}

	std::shared_ptr<Player> oldPlayer = localPlayer;
	cameraTargetPlayer = nullptr;

	                                            
	int iTempPad=localPlayer->GetXboxPad();
	int iTempScreenSection = localPlayer->m_iScreenSection;
	EDefaultSkins skin = localPlayer->getPlayerDefaultSkin();
	player = localgameModes[iPad]->createPlayer(level);

	PlayerUID playerXUIDOffline = INVALID_XUID;
	PlayerUID playerXUIDOnline = INVALID_XUID;
	ProfileManager.GetXUID(iTempPad,&playerXUIDOffline,false);
	ProfileManager.GetXUID(iTempPad,&playerXUIDOnline,true);
	player->setXuid(playerXUIDOffline);
	player->setOnlineXuid(playerXUIDOnline);
	player->setIsGuest( ProfileManager.IsGuest(iTempPad) );

	player->m_displayName = ProfileManager.GetDisplayName(iPad);

	player->SetXboxPad(iTempPad);

	player->m_iScreenSection = iTempScreenSection;
	player->setPlayerIndex( localPlayer->getPlayerIndex() );
	player->setCustomSkin(localPlayer->getCustomSkin());
	player->setPlayerDefaultSkin( skin );
	player->setCustomCape(localPlayer->getCustomCape());
	player->m_sessionTimeStart = localPlayer->m_sessionTimeStart;
	player->m_dimensionTimeStart = localPlayer->m_dimensionTimeStart;
	player// Fix for #63021 - TU7: Content: UI: Travelling from/to the Nether results in switching currently held item to another.dP// Fix for #81759 - TU9: Content: Gameplay: Entering The End Exit Portal replaces the Player's currently held item with the first one from the Quickbar            
	                                                                                                                        // Set the animation override if the skin has one>getHealth() > 0 && localPlayer->y > -64)
	{
		player->inventory->selected = localPlayer->inventory->selected;
	}

	                                                 
	DWORD dwSkinID=app.getSkinIdFromPath(player->customTextureUrl);
	if(GET_IS_DLC_SKIN_FROM_BITMA// 4J-PB - are we the primary player or a local player?r->getSkinAnimOverrideBitmask(dwSkinID));
	}

	player->dimension = dimension;
	came// update the debugoptions	                                                       
	if(iPad==ProfileManager.GetPrimaryPad())
	{
		createPrimaryLocalPlayer(iPad);

		                          
		app.SetGameSettingsDebugMask(ProfileManager.GetPrimaryPad(),app.GetGameSettingsDebugMask(-1,true));
	}
	else
	{
		storeExtraLocalPlayer(iPad);
	}

	player->setShowOnMaps(app.GetGameHostOption(eGameHostOption_Gamertags)!=0?true:false);

	player->resetPos();
	level->addEntity(player);
	gameMode->initPlayer(player);

	if(playe// 4J - added isClientSide check heret;
	player->input = new Input();
	player->entityId = newEntityId;
	player->an// 4J Added for multiplayer. At this point we know everything is ready to run again  //SetEvent(m_hPlayerRespawned);	{
		prepareLevel(IDS_PROGRESS_RESPAWNING);
	}

	                                                                                   
	                               
	player->SetPlayerRespawned(true);

	if (dynamic_cast<DeathScreen *>(screen) != NULL)""etScreen(NULL);

	gameRenderer->EnableUpdateThread();
}

void Minecraft::start(const std::wstring& name, const std::wstring& sid)
{
	startAndConnectTo(name, sid, L  );
}

v/* 4J - removed window handling things here
	final Frame frame = new Frame("Minecraft");
	Canvas canvas = new Canvas();
	frame.setLayout(new BorderLayout());

	frame.add(canvas, BorderLayout.CENTER);

	// OverlayLayout oll = new OverlayLayout(frame);
	// oll.addLayoutComponent(canvas, BorderLayout.CENTER);
	// oll.addLayoutComponent(new JLabel("TEST"), BorderLayout.EAST);

	canvas.setPreferredSize(new Dimension(854, 480));
	frame.pack();
	frame.setLocationRelativeTo(null);
	*/                          // 4J - was new Minecraft(frame, canvas, NULL, 854, 480, fullScreen);                                                                         /* - 4J - removed
	{
	@Override
	public void onCrash(CrashReport crashReport) {
	frame.removeAll();
	frame.add(new CrashInfoPanel(crashReport), BorderLayout.CENTER);
	frame.validate();
	}
	}; */   /* 4J - removed
	final Thread thread = new Thread(minecraft, "Minecraft main thread");
	thread.setPriority(Thread.MAX_PRIORITY);
	*/                             "www.minecraft.net"    // 4J Stu - We never want the player to be DemoUser, we always want them to have their gamertag displayed  //if (ProfileManager.IsFullVersion())= L                   ;""	           ""  // 4J - username & side were compared with NULL rather than empty strings                 
	                                     
	{
		if (userName != L   && sid != L  )	"Player"                                                        ""       
		{//elsene//{ft//	minecraft->user = new DemoUser();}
//}lse/* 4J - TODO
	if (url != NULL)
	{
	String[] tokens = url.split(":");
	minecraft.connectTo(tokens[0], Integer.parseInt(tokens[1]));
	}
	*/   /* 4J - removed
	frame.setVisible(true);
	frame.addWindowListener(new WindowAdapter() {
	public void windowClosing(WindowEvent arg0) {
	minecraft.stop();
	try {
	thread.join();
	} catch (InterruptedException e) {
	e.printStackTrace();
	}
	System.exit(0);
	}
	});
	*/  // 4J - TODO - consider whether we need to actually create a thread here                                                                                                
	                       // 4J-PB - so we can access this from within our xbox game loopun();
}

ClientConnection *Minecraft::getConnection(int iPad)
{
	return localplayers[iPad]->connection;
}

                                                              //g_iMainThreadId = GetCurrentThreadId();return m_instance;
}

bool useLomp = false;

int g_iMainThreadId;

void Minecraft::main()
{
	std::wstring name;
	std::wstring sessionId;

	                                         

	useLomp = true;

	MinecraftWorld_RunStaticCtors();
	Enti
#ifdef _LARGE_WORLDStaticCtor();
	TileEntityRenderD#endifher// 4J Stu - This block generates XML for the game rules schema	#if 0rTable::staticCtor();
	app.loadDefaultGameRules();
                     
	LevelRenderer::staticCtor();
      
"<xs:enumeration value=\"%d\"><xs:annotation><xs:documentation>%ls</xs:documentation></xs:annotation></xs:enumeration>\n"+i)
	{
		if(Item::items[i] != NULL)
		{
			app.DebugPrintf(                         "\n\n\n\n\n"                                                                                    , i, app.GetStr"<xs:enumeration value=\"%d\"><xs:annotation><xs:documentation>%ls</xs:documentation></xs:annotation></xs:enumeration>\n"+i)
	{
		if(Tile::tiles[i] != NULL)
		{
			app.DebugPrintf(                        #endif   // 4J-PB - Can't call this for the first 5 seconds of a game - MS rule  //if (ProfileManager.IsFullVersion())ile::tiles[i]-"Player"riptionId() ));
		}
	}
	__debugbreak();
      

	                         "-"    /* 4J - TODO - get a session ID from somewhere?
		if (args.length > 0) name = args[0];
		sessionId = "-";
		if (args.length > 1) sessionId = args[1];
		*/0);
		// Common for all platforms                                         // On PS4, we call Minecraft::Start from another thread, as this has been timed taking ~2.5 seconds and we need to do some basic  // rendering stuff so that we don't break the TRCs on SubmitDone calls #ifndef __ORBIS__                                     #endif                                               
	                                                                      
                 
	Minecraft::start(name, sessionId);
      
}

bool Minecraft::renderNames()
{
	if (m_instance == NULL || !m_instance->options->hideGui)
	{
		return true;
	}
	return false;
}

bool Minecraft::useFancyGraphics()
{
	return (m_instance != NULL && m_instance->options->fancyGraphics);
}

bool Minecraft::useAmbientOcclusion()
{
	return (m_instance != NULL && m_instance->options->ambientOcclusion != Options::AO_OFF);
}

bool Minecraft::renderDebug()
{
	return (m_instance != NUL// 4J Force valueoptions->renderDe//for (int texSize = 16384; texSize > 0; texSize >>= 1) {d://	GL11.glTexImage2D(GL11.GL_PROXY_TEXTURE_2D, 0, GL11.GL_RGBA, texSize, texSize, 0, GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE, (ByteBuffer) null);  //	final int width = GL11.glGetTexLevelParameteri(GL11.GL_PROXY_TEXTURE_2D, 0, GL11.GL_TEXTURE_WIDTH);  //	if (width != 0) {  //		return texSize;  //	}  //}  //return -1;        
	                                                                                                      
	                    
	           //(Sys.getTime() * 1000) / Sys.getTimerResolution();:del/*void Minecraft::handleMouseDown(int button, bool down)
{
if (gameMode->instaBuild) return;
if (!down) missTime = 0;
if (button == 0 && missTime > 0) return;

if (down && hitResult != NULL && hitResult->type == HitResult::TILE && button == 0)
{
int x = hitResult->x;
int y = hitResult->y;
int z = hitResult->z;
gameMode->continueDestroyBlock(x, y, z, hitResult->f);
particleEngine->crack(x, y, z, hitResult->f);
}
else
{
gameMode->stopDestroyBlock();
}
}

void Minecraft::handleMouseClick(int button)
{
if (button == 0 && missTime > 0) return;
if (button == 0)
{
app.DebugPrintf("handleMouseClick - Player %d is swinging\n",player->GetXboxPad());
player->swing();
}

bool mayUse = true;

//	* if (button == 1) { ItemInstance item =
//	* player.inventory.getSelected(); if (item != null) { if
//	* (gameMode.useItem(player, item)) {
//	* gameRenderer.itemInHandRenderer.itemUsed(); return; } } }

// 4J-PB - Adding a special case in here for sleeping in a bed in a multiplayer game - we need to wake up, and we don't have the inbedchatscreen with a button

if(button==1 && (player->isSleeping() && level != NULL && level->isClientSide))
{
shared_ptr<MultiplayerLocalPlayer> mplp = dynamic_pointer_cast<MultiplayerLocalPlayer>( player );

if(mplp) mplp->StopSleeping();

// 4J - TODO
//if (minecraft.player instanceof MultiplayerLocalPlayer)
//{
//    ClientConnection connection = ((MultiplayerLocalPlayer) minecraft.player).connection;
//    connection.send(new PlayerCommandPacket(minecraft.player, PlayerCommandPacket.STOP_SLEEPING));
//}
}

if (hitResult == NULL)
{
if (button == 0 && !(dynamic_cast<CreativeMode *>(gameMode) != NULL)) missTime = 10;
}
else if (hitResult->type == HitResult::ENTITY)
{
if (button == 0)
{
gameMode->attack(player, hitResult->entity);
}
if (button == 1)
{
gameMode->interact(player, hitResult->entity);
}
}
else if (hitResult->type == HitResult::TILE)
{
int x = hitResult->x;
int y = hitResult->y;
int z = hitResult->z;
int face = hitResult->f;

//	* if (button != 0) { if (hitResult.f == 0) y--; if (hitResult.f ==
//	* 1) y++; if (hitResult.f == 2) z--; if (hitResult.f == 3) z++; if
//	* (hitResult.f == 4) x--; if (hitResult.f == 5) x++; }

// if (isClientSide())
// {
// return;
// }

if (button == 0)
{
gameMode->startDestroyBlock(x, y, z, hitResult->f);
}
else
{
shared_ptr<ItemInstance> item = player->inventory->getSelected();
int oldCount = item != NULL ? item->count : 0;
if (gameMode->useItemOn(player, level, item, x, y, z, face))
{
mayUse = false;
app.DebugPrintf("Player %d is swinging\n",player->GetXboxPad());
player->swing();
}
if (item == NULL)
{
return;
}

if (item->count == 0)
{
player->inventory->items[player->inventory->selected] = NULL;
}
else if (item->count != oldCount)
{
gameRenderer->itemInHandRenderer->itemPlaced();
}
}
}

if (mayUse && button == 1)
{
shared_ptr<ItemInstance> item = player->inventory->getSelected();
if (item != NULL)
{
if (gameMode->useItem(player, level, item))
{
gameRenderer->itemInHandRenderer->itemUsed();
}
}
}
}
*/  // 4J-PB                                                                                                                         /*if( gameMode != NULL && gameMode->isTutorial() )
	{
	return true;
	}
	else
	{
	return false;
	}*/rn screen;
}

bool Minecraft::isTutorial()
{
	return m_// If the app doesn't think we are in a tutorial mode then just ignore this add                                              
}

void Minecraft::playerStartedTutorial(int iPad)
{
	                                         // 4J Stu - Fix for bug that was flooding Sentient with LevelStart eventsul// If the tutorial bits are already 0 then don't need to update anything:playerLeftTutorial(int iPad)
{
	                                                                         
	                                                                        
	if(m_inFullTutorialBits == 0)
	// 4J Stu -This telemetry event means something different on XboxOne, so we don't call it for simple state changes like thisu#ifndef _XBOX_ONE	{
		app.SetTutorialMode( false );

		                                                                                                                            
                 
		for(DWORD idx = 0; idx < XUSER_MAX_COUNT; ++idx)
		{
			if(localplayers[idx] != NULL)
			{
				TelemetryManager->RecordLevelStart(idx, eSen_Fr#endifMatch_
#ifdef _DURANGOited_Friends, eSen_CompeteOrCoop_Coop_and_Competitive, \
    level->difficulty, app.GetLocalPlayerCount(),                     \
    g_NetworkManager.GetOnlinePlayerCount());
			}
		}
      
	}
}
                
void Minecraft::inGameSignInCheckAllPrivilegesCallback(LPVOID lpParam, bool hasPrivileges, int iPad)
{
	Minecraft* pClass = (Minecraft*)lpParam;

	if(!hasPrivileges)
	{
		ProfileManager.RemoveGamepadFromGame(iPad);
	}
	else
	{
		if( !g_NetworkManager.SessionHasSpace() )
		{
			UINT uiIDA[1];
			uiIDA[0]=IDS_OK;
			ui.RequestErrorMessage(IDS_MULTIPLAYER_FULL_TITLE, IDS_MULTIPLAYE// create the local player for the iPader.RemoveGamepadFromGame(iPad);
		}
		else if( ProfileManager.IsSignedInLive(iPad) && ProfileManager.AllowedToPlayMultiplayer(iPad) )
		{
			                                       
			std::shar// create the local player for the iPadayers[iPad];
			if( player == NULL)
			{
				if( pClass->level->isClientSide )
				{
					pClass->addLocalPlayer(iPad);
				}
				else
				{
					                                       
					std::shared_ptr<Player> player = pClass->localplayers[iPad];
					if( player == NULL)
#endif

#ifdef _XBOX_ONElass->createExtraLocalPlayer(                               \
    iPad, (convStringToWstring(ProfileManager.GetGamertag(iPad))).c #else), \
    iPad, pClass->level->dimension->id);
					}
				}
			}
		}
	}
}
      
   #endif        
int Minecraft::InGame_SignInReturned(void *pParam,bool bContinue, int iPad, int iContr// 4J Stu - There seems to be a bug in the signin ui call that enables guest sign in. We never allow this within game, so make sure that it's disabledif(// Fix for #66516 - TCR #124: MPS Guest Support ; #001: BAS Game Stability: TU8: The game crashes when second Guest signs-in on console which takes part in Xbox LIVE multiplayer session. 
		               "Disabling Guest Signin\n"                                     // If sign in succeded, we're in game and this player isn't already playing, continue                       
		app.DebugPrintf(                          );
		XEnableGuestSignin(FALSE);
	}

	   // It's possible that the player has not signed in - they can back out or choose no for the converttoguestg_NetworkManager.IsInSession() && pMinecraf#ifdef _DURANGOayers[iPad] == NULL)
	{
		                                                                                                          
		if(ProfileManager.IsSignedIn(iPad))
		{
               
			if(!g_NetworkManager.IsLocalGame() && ProfileManager.IsSignedInLive(i#endif& ProfileManager.AllowedToPlayMultiplayer(iPad))
			{
				ProfileManager.CheckMultiplayerPrivileges(iPad, true, &inGameSignInCheckAllPrivilegesCallback, pMinecraftClass);
			}
			else
#ifdef _DURANGOg_NetworkManager.SessionHasSpace())
			{
				UIN#endifA[1];
			// if this is a local game then profiles just need to be signed inTITLE, IDS_MULTIPLAYER_FULL_TEXT, uiIDA, 1);
               
				ProfileManager.RemoveGamepadFromGame(iPad);
      
			}
			                 #ifdef __ORBIS__                                 
			else if( g_NetworkManager.IsLocalGame() || (ProfileManager.IsSignedInLive(iPad) && ProfileM// TODO!llowedToPlayMultiplayer(iPad)) )
			{
                
				bool contentRestricted = false;
				ProfileManager.GetChatAndContentRestrictions(iPad,false,NULL,&contentRestricted,NULL);         

				if (!g_NetworkManager.IsLocalGame() && contentRestricted)
				{
					ui.RequestContentRestrictedMessageBox(IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_CONTENT_RESTRICTION, iPad);
				}
				else if(!g_NetworkManager.IsLocalGame() && #endifleManager.HasPlayStationPlus(iPad))
				{
					pMinecraftClass->m_pPsPlusUpsell = new PsPlusUpsellWrapper(iPad);
					pMinecra// create the local player for the iPad();
				}
				else
      
					if( pMinecraftClass->level->isClientSide )
					{
					pMinecraftClass->addLocalPlayer(iPad);
				}
				else
				{
					                                       
					std::shared_ptr<Player> player = pMinecraftClass->localplayers[iPad];
					if( player == NULL)
					{
						player = pMinecraftClass->createExtraLocalPlayer(iPad, (convStringToWstring( ProfileManager.GetGamertag(iPad) )).c_str(), iPad// 4J Stu - Don't allow converting to guests as we don't allow any guest sign-in while in the gameive(P// Fix for #66516 - TCR #124: MPS Guest Support ; #001: BAS Game Stability: TU8: The game crashes when second Guest signs-in on console which takes part in Xbox LIVE multiplayer session.    
//ProfileManager.RequestConvertOfflineToGuestUI( &Minecraft::InGame_SignInReturned, pMinecraftClass,iPad);                                                                                    
				                                                                               #ifdef _DURANGO            
				UINT uiIDA[1];
				uiIDA[0]=IDS_#endifM_OK;
				ui.RequestErrorMessage( IDS_NO_MULTIPLAYER_PRIVILEGE_TITLE, IDS_NO_MULTIPLAYER_PRIVILEGE_JOIN_TEXT, uiIDA,1,iPad);
               
				ProfileManager.RemoveGamepadFromGame(iPad);
      
			}
		}
	}
	return 0;
}

void Minecraft::tickAllConnections()
{
	int oldIdx = getLocalPlayerIdx();
	for(unsigned int i = 0; i < XUSER_MAX_COUNT; i++ )
	{
		std::shared_ptr<MultiplayerLocalPlayer> mplp = localplayers[i];
		if( mplp && mplp->connection)
		{
			setLocalPlayerIdx(i);
			mplp->connection->tick();
		}
	}
	setLocalPlayerIdx(oldIdx);
}

bool Minecraft::addPendingClientTextureRequest(const std::wstring &textureName)
{
	AUTO_VAR(it, std::find( m_pendingTextureRequests.begin(), m_pendingTextureRequests.end(), textureName));
	if( it == m_pendingTextureRequests.end() )
	{
		m_pendingTextureRequests.push_back(textureName);
		return true;
	}
	return false;
}

void Minecraft::handleClientTextureReceived(const std::wstring &textureName)
{
	AUTO_VAR(it, std::find( m_pendingTextureRequests.begin(), m_pendingTextureRequests.end(), textureName));
	if( it != m_pendingTextureRequests.end() )
	{
		m_pendingTextureRequests.erase(it);
	}
}

unsigned int Minecraft::getCurrentTexturePackId()
{
	return skins->getSelected()->getId();
}

ColourTable *Minecraft::getColou
#if defined __ORBIS__k* selected = skins->getSelected();

	ColourTable *colours = selected->getColourTable();

	if(colours == NULL)
	{
		colours = skins->getDefault()->getColourTable();
	}

	return colours;
}
                      
int Minecraft::MustSignInReturnedPSN(void *pParam, int iPad, C4JStorage::EMessageResult result)
{
	Minecraft* pMinecra#endifMinecraft *)pParam;

	if(result == C4JStorage::EMessage_ResultAccept) 
	{        
		SQRNetworkManager_Orbis::AttemptPSNSignIn(&Minecraft::InGame_SignInReturned, pMinecraft, false, iPad);
	}

	return 0;
}
      

