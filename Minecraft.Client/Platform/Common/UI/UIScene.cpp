#include "../../stdafx.h"
#include "UI.h"
#include "UIScene.h"

#include "../../Lighting.h"
#include "../../LocalPlayer.h"
#include "../../ItemRenderer.h"
#include "../../../../Minecraft.World/Headers/net.minecraft.world.item.h"

UIScene::UIScene(int iPad, UILayer* parentLayer) {
    m_parentLayer = parentLayer;
    m_iPad = iPad;
    swf = NULL;
    m_pItemRenderer = NULL;

    bHasFocus = false;
    m_hasTickedOnce = false;
    m_bFocussedOnce = false;
    m_bVisible = true;
    m_bCanHandleInput = false;
    m_bIsReloading = false;

    m_iFocusControl = -1;
    m_iFocusChild = 0;
    m_lastOpacity = 1.0f;
    m_bUpdateOpacity = false;

    m_backScene = NULL;

    m_cacheSlotRenders = false;
    m_needsCacheRendered = true;
    m_expectedCachedSlotCount = 0;
    m_callbackUniqueId = 0;
}

UIScene::~UIScene() {
    /* Destroy the Iggy player. */
    IggyPlayerDestroy(swf);

    for (AUTO_VAR(it, m_registeredTextures.begin());
         it != m_registeredTextures.end(); ++it) {
        ui.unregisterSubstitutionTexture(it->first, it->second);
    }

    if (m_callbackUniqueId != 0) {
        ui.UnregisterCallbackId(m_callbackUniqueId);
    }

    if (m_pItemRenderer != NULL) delete m_pItemRenderer;
}

void UIScene::destroyMovie() {
    /* Destroy the Iggy player. */
    IggyPlayerDestroy(swf);
    swf = NULL;

    // Clear out the controls collection (doesn't delete the controls, and they
    // get re-setup later)
    m_controls.clear();

    // Clear out all the fast names for the current movie
    m_fastNames.clear();
}

void UIScene::reloadMovie(bool force) {
    if (!force &&
        (stealsFocus() &&
         (getSceneType() != eUIScene_FullscreenProgress && !bHasFocus)))
        return;

    m_bIsReloading = true;
    if (swf) {
        /* Destroy the Iggy player. */
        IggyPlayerDestroy(swf);

        // Clear out the controls collection (doesn't delete the controls, and
        // they get re-setup later)
        m_controls.clear();

        // Clear out all the fast names for the current movie
        m_fastNames.clear();
    }

    // Reload everything
    initialiseMovie();

    handlePreReload();

    // Reload controls
    for (AUTO_VAR(it, m_controls.begin()); it != m_controls.end(); ++it) {
        (*it)->ReInit();
    }

    updateComponents();
    handleReload();

    IggyDataValue result;
    IggyDataValue value[1];

    value[0].type = IGGY_DATATYPE_number;
    value[0].number = m_iFocusControl;

    IggyResult out = IggyPlayerCallMethodRS(getMovie(), &result,
                                            IggyPlayerRootPath(getMovie()),
                                            m_funcSetFocus, 1, value);

    m_needsCacheRendered = true;
    m_bIsReloading = false;
}

bool UIScene::needsReloaded() { return !swf && (!stealsFocus() || bHasFocus); }

bool UIScene::hasMovie() { return swf != NULL; }

F64 UIScene::getSafeZoneHalfHeight() {
    float height = ui.getScreenHeight();

    float safeHeight = 0.0f;

#ifndef __PSVITA__
    if (!RenderManager.IsHiDef() && RenderManager.IsWidescreen()) {
        // 90% safezone
        safeHeight = height * (0.15f / 2);
    } else {
        // 90% safezone
        safeHeight = height * (0.1f / 2);
    }
#endif
    return safeHeight;
}

F64 UIScene::getSafeZoneHalfWidth() {
    float width = ui.getScreenWidth();

    float safeWidth = 0.0f;
#ifndef __PSVITA__
    if (!RenderManager.IsHiDef() && RenderManager.IsWidescreen()) {
        // 85% safezone
        safeWidth = width * (0.15f / 2);
    } else {
        // 90% safezone
        safeWidth = width * (0.1f / 2);
    }
#endif
    return safeWidth;
}

void UIScene::updateSafeZone() {
    // Distance from edge
    F64 safeTop = 0.0;
    F64 safeBottom = 0.0;
    F64 safeLeft = 0.0;
    F64 safeRight = 0.0;

    switch (m_parentLayer->getViewport()) {
        case C4JRender::VIEWPORT_TYPE_SPLIT_TOP:
            safeTop = getSafeZoneHalfHeight();
            break;
        case C4JRender::VIEWPORT_TYPE_SPLIT_BOTTOM:
            safeBottom = getSafeZoneHalfHeight();
            break;
        case C4JRender::VIEWPORT_TYPE_SPLIT_LEFT:
            safeLeft = getSafeZoneHalfWidth();
            break;
        case C4JRender::VIEWPORT_TYPE_SPLIT_RIGHT:
            safeRight = getSafeZoneHalfWidth();
            break;
        case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_LEFT:
            safeTop = getSafeZoneHalfHeight();
            safeLeft = getSafeZoneHalfWidth();
            break;
        case C4JRender::VIEWPORT_TYPE_QUADRANT_TOP_RIGHT:
            safeTop = getSafeZoneHalfHeight();
            safeRight = getSafeZoneHalfWidth();
            break;
        case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_LEFT:
            safeBottom = getSafeZoneHalfHeight();
            safeLeft = getSafeZoneHalfWidth();
            break;
        case C4JRender::VIEWPORT_TYPE_QUADRANT_BOTTOM_RIGHT:
            safeBottom = getSafeZoneHalfHeight();
            safeRight = getSafeZoneHalfWidth();
            break;
        case C4JRender::VIEWPORT_TYPE_FULLSCREEN:
        default:
            safeTop = getSafeZoneHalfHeight();
            safeBottom = getSafeZoneHalfHeight();
            safeLeft = getSafeZoneHalfWidth();
            safeRight = getSafeZoneHalfWidth();
            break;
    }
    setSafeZone(safeTop, safeBottom, safeLeft, safeRight);
}

void UIScene::setSafeZone(S32 safeTop, S32 safeBottom, S32 safeLeft,
                          S32 safeRight) {
    IggyDataValue result;
    IggyDataValue value[4];

    value[0].type = IGGY_DATATYPE_number;
    value[0].number = safeTop;
    value[1].type = IGGY_DATATYPE_number;
    value[1].number = safeBottom;
    value[2].type = IGGY_DATATYPE_number;
    value[2].number = safeLeft;
    value[3].type = IGGY_DATATYPE_number;
    value[3].number = safeRight;
    IggyResult out = IggyPlayerCallMethodRS(getMovie(), &result,
                                            IggyPlayerRootPath(getMovie()),
                                            m_funcSetSafeZone, 4, value);
}

void UIScene::initialiseMovie() {
    loadMovie();
    mapElementsAndNames();

    updateSafeZone();

    m_bUpdateOpacity = true;
}

#ifdef __PSVITA__
void UIScene::SetFocusToElement(int iID) {
    IggyDataValue result;
    IggyDataValue value[1];

    value[0].type = IGGY_DATATYPE_number;
    value[0].number = iID;

    IggyResult out = IggyPlayerCallMethodRS(getMovie(), &result,
                                            IggyPlayerRootPath(getMovie()),
                                            m_funcSetFocus, 1, value);

    // also trigger handle focus change (just in case if anything else in
    // relation needs updating!)
    _handleFocusChange(iID, 0);
}
#endif

bool UIScene::mapElementsAndNames() {
    m_rootPath = IggyPlayerRootPath(swf);

    m_funcRemoveObject = registerFastName(L"RemoveObject");
    m_funcSlideLeft = registerFastName(L"SlideLeft");
    m_funcSlideRight = registerFastName(L"SlideRight");
    m_funcSetSafeZone = registerFastName(L"SetSafeZone");
    m_funcSetAlpha = registerFastName(L"SetAlpha");
    m_funcSetFocus = registerFastName(L"SetFocus");
    m_funcHorizontalResizeCheck = registerFastName(L"DoHorizontalResizeCheck");
    return true;
}

extern CRITICAL_SECTION s_loadSkinCS;
void UIScene::loadMovie() {
    EnterCriticalSection(
        &UIController::ms_reloadSkinCS);  // MGH - added to prevent crash
                                          // loading Iggy movies while the skins
                                          // were being reloaded
    std::wstring moviePath = getMoviePat
#ifdef __PS3__
        if (RenderManager.IsWidescreen()) {
                moviePath.app"720.swf");
                m_loadedResolution = eSceneResolution_720;
    }
    else {
                moviePath.app"480.swf");
                m_loadedResolution = eSceneResolution_480#elif defined __PSVITA__
	moviePath.app"Vita.swf");
                m_loadedResolution = eSceneResolution_V #elif defined
                    _WINDOWS64 if (ui.getScreenHeight() == 720) {
                moviePath.app"720.swf");
                m_loadedResolution = eSceneResolution_720;
                }
                else if (ui.getScreenHeight() == 480) {
                moviePath.app"480.swf");
                m_loadedResolution = eSceneResolution_480;
                }
                else if (ui.getScreenHeight() < 720) {
                moviePath.app"Vita.swf");
                m_loadedResolution = eSceneResolution_Vita;
                }
                else {
                moviePath.app"1080.swf");
                m_loadedResolution = eSceneResolution_1080#else
	moviePath.app"1080.swf");
                m_loadedResolution = eSceneResolution_1 #endif

                    if (!app.hasArchiveFile(moviePath)) {
                app.DebugPr"WARNING: Could not find iggy movie %ls, falling back on 720\n", moviePath.c_str());

                moviePath = getMoviePath();
                moviePath.app"720.swf");
                m_loadedResolution = eSceneResolution_720;

                if (!app.hasArchiveFile(moviePath)) {
                    app.DebugPr
                        "ERROR: Could not find any iggy movie for %ls!\n",
                        moviePath
                            .c_str #ifndef _CONTENT_PACKAGE
                                __debugbrea #endif app.FatalLoadError();
                }
                }

                byteArray baFile = ui.getMovieData(moviePath.c_str());
                __int64 beforeLoad = ui.iggyAllocCount;
                swf = IggyPlayerCreateFromMemory(baFile.data, baFile.length,
                                                 NULL);
                __int64 afterLoad = ui.iggyAllocCount;
                IggyPlayerInitializeAndTickRS(swf);
                __int64 afterTick = ui.iggyAllocCount;

                if (!swf) {
                    app.DebugPr
                        "ERROR: Failed to load iggy "
                        "scene!"
                        "\n" #ifndef _CONTENT_PACKAGE __debugbrea #endif app
                            .FatalLoadError();
                }
                app.DebugPrintf(app.USER "Loaded iggy movie %ls\n",
                                moviePath.c_str());
                IggyProperties* properties = IggyPlayerProperties(swf);
                m_movieHeight = properties->movie_height_in_pixels;
                m_movieWidth = properties->movie_width_in_pixels;

                m_renderWidth = m_movieWidth;
                m_renderHeight = m_movieHeight;

                S32 width, height;
                m_parentLayer->getRenderDimensions(width, height);
                IggyPlayerSetDisplaySize(swf, width, height);

        IggyPlayerSetUserdata(swf,thi//#ifdef _DEBUG#if 0
	IggyMemoryUseInfo memoryInfo;
	rrbool res;
	int iteration = 0;
	__int64 totalStatic = 0;
	__int64 totalDynamic = 0;
	while(res = IggyDebugGetMemoryUseInfo ( swf ,
		NULL ,
		0 ,
		0 ,
		iteration ,
		&memoryInfo ))
	{
                totalStatic += memoryInfo.static_allocation_bytes;
                totalDynamic += memoryInfo.dynamic_allocation_bytes;
                app.DebugPrintf(
                    app.USER
                    "%ls - %.*s static: %d ( %d ) dynamic: %d ( %d )\n",
                    moviePath.c_str(), memoryInfo.subcategory_stringlen,
                    memoryInfo.subcategory, memoryInfo.static_allocation_bytes,
                    memoryInfo.static_allocation_count,
                    memoryInfo.dynamic_allocation_bytes,
                    memoryInfo.dynamic_allocation_count);
                ++iteratio  // if(memoryInfo.static_allocation_bytes > 0)
                            // getDebugMemoryUseRecursive(moviePath,
                            // memoryInfo);

	}

	app.DebugPrintf(app.USER"%ls - Total: %d, Expected: %d, Diff: %d\n", moviePath.c_str(), totalStatic + totalDynamic, afterTick - beforeLoad, (afterTick - beforeLoad) - (totalStatic + totalDynami
#endif 
	LeaveCriticalSection(&UIController::ms_reloadSkinCS);
                }

                void UIScene::getDebugMemoryUseRecursive(
                    const std::wstring& moviePath,
                    IggyMemoryUseInfo& memoryInfo) {
                    rrbool res;
                    IggyMemoryUseInfo internalMemoryInfo;
                    int internalIteration = 0;
                    while (res = IggyDebugGetMemoryUseInfo(
                               swf, NULL, memoryInfo.subcategory,
                               memoryInfo.subcategory_stringlen,
                               internalIteration, &internalMemoryInfo)) {
                        app.DebugPrintf(
                            app.USER
                            "%ls - %.*s static: %d ( %d ) dynamic: %d ( %d "
                            ")\n",
                            moviePath.c_str(),
                            internalMemoryInfo.subcategory_stringlen,
                            internalMemoryInfo.subcategory,
                            internalMemoryInfo.static_allocation_bytes,
                            internalMemoryInfo.static_allocation_count,
                            internalMemoryInfo.dynamic_allocation_bytes,
                            internalMemoryInfo.dynamic_allocation_count);
                        ++internalIteration;
                        if (internalMemoryInfo.subcategory_stringlen >
                            memoryInfo.subcategory_stringlen)
                            getDebugMemoryUseRecursive(moviePath,
                                                       internalMemoryInfo);
                    }
                }

                void UIScene::PrintTotalMemoryUsage(__int64& totalStatic,
                                                    __int64& totalDynamic) {
                    if (!swf) return;

                    IggyMemoryUseInfo memoryInfo;
                    rrbool res;
                    int iteration = 0;
                    __int64 sceneStatic = 0;
                    __int64 sceneDynamic = 0;
                    while (res = IggyDebugGetMemoryUseInfo(
                               swf, NULL "", 0, iteration, &memoryInfo)) {
                        sceneStatic += memoryInfo.static_allocation_bytes;
                        sceneDynamic += memoryInfo.dynamic_allocation_bytes;
                        totalStatic += memoryInfo.static_allocation_bytes;
                        totalDynamic += memoryInfo.dynamic_allocation_bytes;
                        ++iteration;
                    }

                    app.DebugPrintf(app.USER
                                    "    \\- Scene static: %d , Scene dynamic: "
                                    "%d , Total: %d - %ls\n",
                                    sceneStatic, sceneDynamic,
                                    sceneStatic + sceneDynamic,
                                    getMoviePath().c_str());
                }

                void UIScene::tick() {
                    if (m_bIsReloading) return;
                    if (m_hasTickedOnce) m_bCanHandleInput = true;
                    while (IggyPlayerReadyToTick(swf)) {
                        tickTimers();
                        for (AUTO_VAR(it, m_controls.begin());
                             it != m_controls.end(); ++it) {
                            (*it)->tick();
                        }
                        IggyPlayerTickRS(swf);
                        m_hasTickedOnce = true;
                    }
                }

                UIControl* UIScene::GetMainPanel() { return NULL; }

                void UIScene::addTimer(int id, int ms) {
                    int currentTime = System::currentTimeMillis();

                    TimerInfo info;
                    info.running = true;
                    info.duration = ms;
                    info.targetTime = currentTime + ms;
                    m_timers[id] = info;
                }

                void UIScene::killTimer(int id) {
                    AUTO_VAR(it, m_timers.find(id));
                    if (it != m_timers.end()) {
                        it->second.running = false;
                    }
                }

                void UIScene::tickTimers() {
                    int currentTime = System::currentTimeMillis();
                    for (AUTO_VAR(it, m_timers.begin());
                         it != m_timers.end();) {
                        if (!it->second.running) {
                            it = m_timers.erase(it);
                        } else {
                            if (currentTime > it->second.targetTime) {
                                handleTimerComplete(it->first);
                                // Auto-restart
                                it->second.targetTime =
                                    it->second.duration + currentTime;
                            }
                            ++it;
                        }
                    }
                }

                IggyName UIScene::registerFastName(const std::wstring& name) {
                    IggyName var;
                    AUTO_VAR(it, m_fastNames.find(name));
                    if (it != m_fastNames.end()) {
                        var = it->second;
                    } else {
                        var = IggyPlayerCreateFastName(
                            getMovie(), (IggyUTF16*)name.c_str(), -1);
                        m_fastNames[name] = var;
                    }
                    return var;
                }

                void UIScene::removeControl(UIControl_Base * control,
                                            bool centreScene) {
                    IggyDataValue result;
                    IggyDataValue value[2];

                    std::string name = control->getControlName();
                    IggyStringUTF8 stringVal;
                    stringVal.string = (char*)name.c_str();
                    stringVal.length = name.length();
                    value[0].type = IGGY_DATATYPE_string_UTF8;
                    value[0].string8 = stringVal;

                    value[1].type = IGGY_DATATYPE_boolean;
                    value[1].boolval = centreScene;
                    IggyResult out = IggyPlayerCallMethodRS(
                        getMovie(), &result, IggyPlayerRootPath(getMovie()),
                        m_funcRemoveObject, 2,
#ifdef __PSVITA__  // update the button positions since they may have changed
                        UpdateSceneCont  // mark the button as removed
                            control
                                ->setHidde  // remove it from the
                                            // touchboxes
                                    ui.TouchBoxRebuild(
                                        control->getParent #endif));
                }

                void UIScene::slideLeft() {
                    IggyDataValue result;
                    IggyResult out = IggyPlayerCallMethodRS(
                        getMovie(), &result, IggyPlayerRootPath(getMovie()),
                        m_funcSlideLeft, 0, NULL);
                }

                void UIScene::slideRight() {
                    IggyDataValue result;
                    IggyResult out = IggyPlayerCallMethodRS(
                        getMovie(), &result, IggyPlayerRootPath(getMovie()),
                        m_funcSlideRight, 0, NULL);
                }

                void UIScene::doHorizontalResizeCheck() {
                    IggyDataValue result;
                    IggyResult out = IggyPlayerCallMethodRS(
                        getMovie(), &result, IggyPlayerRootPath(getMovie()),
                        m_funcHorizontalResizeCheck, 0, NULL);
                }

                void UIScene::render(S32 width, S32 height,
                                     C4JRender::eViewportType viewport) {
                    if (m_bIsReloading) return;
                    if (!m_hasTickedOnce || !swf) return;
                    ui.setupRenderPosition(viewport);
                    IggyPlayerSetDisplaySize(swf, width, height);
                    IggyPlayerDraw(swf);
                }

                void UIScene::setOpacity(float percent) {
                    if (percent != m_lastOpacity ||
                        (m_bUpdateOpacity && getMovie())) {
                        m_lastOpacity =
                            pe  // 4J-TomK once a scene has been freshly loaded
                                // or re-loaded we force update opacity via
                                // initialiseMovie
                            if (m_bUpdateOpacity) m_bUpdateOpacity = false;

                        IggyDataValue result;
                        IggyDataValue value[1];
                        value[0].type = IGGY_DATATYPE_number;
                        value[0].number = percent;

                        IggyResult out = IggyPlayerCallMethodRS(
                            getMovie(), &result, IggyPlayerRootPath(getMovie()),
                            m_funcSetAlpha, 1, value);
                    }
                }

                void UIScene::setVisible(bool visible) { m_bVisible = visible; }

                void UIScene::customDraw(IggyCustomDrawCallbackRegion *
                                         region) {
        app.De"Handling custom draw for scene with no override!\n");
                }

                void UIScene::customDrawSlotControl(
                    IggyCustomDrawCallbackRegion * region, int iPad,
                    std::shared_ptr<ItemInstance> item, float fAlpha,
                    bool isFoil, bool bDecorations) {
                    if (item != NULL) {
                        if (m_cacheSlotRenders) {
                            if ((m_cachedSlotDraw.size() + 1) ==
                                    m_expectedCachedSlotC  // Make sure that
                                                           // pMinecraft->player
                                                           // is the correct
                                                           // player so that
                                                           // player specific
                                                           // rendering//
                                                           // eg clock and
                                                           // compass, are
                                                           // rendered
                                                           // correctly
                                    Minecraft* pMinecraft =
                                    Minecraft::GetInstance();
                                std::shared_ptr<MultiplayerLocalPlayer>
                                    oldPlayer = pMinecraft->player;
                                if (iPad >= 0 && iPad < XUSER_MAX_COUNT)
                                    pMinecraft->player =
                                    pMinecraft
                                        ->local  // Setup GDraw, normal game
                                                 // render states and
                                                 // matrices//CustomDrawData
                                                 // *customDrawRegion =
                                                 // ui.setupCustomDraw(this,region);
                                    P
                                    "Starting Iggy custom "
                                    "draw\n")
                                ;
                            CustomDrawData* customDrawRegion =
                                ui.calculateCustomDraw(region);
                            ui.beginIggyCustomDraw4J(region, customDrawRegion);
                            ui.setupCustomDrawGameState();

                            int list = m_parentLayer->m_parentGroup
                                           ->getCommandBufferList();

                            bool useComm #ifdef _XBOX_ONEse;

                            useCommandB  // 4J Stu - Temporary until we fix
                                         // the glint animation which needs
                                         // updated if we are just replaying
                                         // a command
                                         // buffer
                                m_needsCa #endifdered = true;

                                if(!useCommandBuffers || m_needsC#if (!defined __PS3__) && (!defined __PSVITA__)
					if(useCommandBuffers) RenderManager.CBuf#endif(std::list, true);

	"Draw uncached"edEvent(0,);
					ui.setupCustomDrawMatrices(this, customDrawRegion);	
					_customDrawSlotControl(customDrawRegion, iPad, item, fAlpha, isFoil, bDecorations, useCommandBuffers);
					delete customDrawRegion;
					PIXEndNamedEvent();

	"Draw all cache"dEvent(0// Draw all the cached slots
					for(AUTO_VAR(it, m_cachedSlotDraw.begin()); it != m_cachedSlotDraw.end(); ++it)
					{
                        CachedSlotDrawData* drawData = *it;
                        ui.setupCustomDrawMatrices(this,
                                                   drawData->customDrawRegion);
                        _customDrawSlotControl(
                            drawData->customDrawRegion, iPad, drawData->item,
                            drawData->fAlpha, drawData->isFoil,
                            drawData->bDecorations, useCommandBuffers);
                        delete drawData->customDrawRegion;
                        delete drawData;
					}
#ifndef __PS3__dEvent();

					if(useCommandBuffers) R#endifanager.CBuffEnd();
                        }
                        m
#ifndef __PS3__clear();

                            if (useCommandBuffers) Render #endifr
                                .CBuf  // Finish GDraw and anything else that
                                       // needs to be finalised
                                           ui.endCustomDraw(region);

                        pMinecraft->player = oldPlayer;
                    } else
                        "Caching region" nNamedEvent(0, );
                    CachedSlotDrawData* drawData = new CachedSlotDrawData();
                    drawData->item = item;
                    drawData->fAlpha = fAlpha;
                    drawData->isFoil = isFoil;
                    drawData->bDecorations = bDecorations;
                    drawData->customDrawRegion = ui.calculateCustomDraw(region);

                    m_cachedSlotDraw.push_back(drawData);
                    PIXEndNamedEv  // Setup GDraw, normal game render states and
                                   // matrices
                        CustomDrawData* customDrawRegion =
                            ui.setupCustomDraw(this, region);

                    Minecraft* pMinecraft =  // Make sure that
                                             // pMinecraft->player is the
                                             // correct player so that player
                                             // specific rendering// eg clock
                                             // and compass, are rendered
                                             // correctly
                        std::shared_ptr<MultiplayerLocalPlayer> oldPlayer =
                            pMinecraft->player;
                    if (iPad >= 0 && iPad < XUSER_MAX_COUNT)
                        pMinecraft->player = pMinecraft->localplayers[iPad];

                    _customDrawSlotControl(customDrawRegion, iPad, item, fAlpha,
                                           isFoil, bDecorations, false);
                    delete customDrawRegion;
                    p  // Finish GDraw and anything else that needs to be
                       // finalised
                        ui.endCustomDraw(region);
                }
    }
}

void UIScene::_customDrawSlotControl(CustomDrawData* region, int iPad,
                                     std::shared_ptr<ItemInstance> item,
                                     float fAlpha, bool isFoil,
                                     bool bDecorations,
                                     bool usingCommandBuffer) {
    Minecraft* pMinecraft = Minecraft::GetInstance();

    float bwidth, bheight;
    bwidth = region->x1 - region->x0;
    bheight = region->y1 - region->y0;

    float x  // Base scale on height of this control, compared to height of what
             // the item renderer normally renders (16 pixels high).
             // Potentially// we might want separate x & y scales
             // here

        float scaleX = bwidth / 16.0f;
    float scaleY = bheight / 16.0f;

    glEnable(GL_RESCALE_NORMAL);
    glPushMatrix();
    glRotatef(120, 1, 0, 0);
    Lighting::turnOn();
    glPopMatrix();

    float pop = item->popTime;
    if (pop > 0) {
        glPushMatrix();
        float squeeze = 1 + pop / (float)Inventory::POP_TIME_DURATION;
        float sx = x;
        float sy = y;
        float sxoffs = 8 * scaleX;
        float syoffs = 12 * scaleY;
        glTranslatef((float)(sx + sxoffs), (float)(sy + syoffs), 0);
        glScalef(1 / squeeze, (squeeze + 1) / 2, 1);
                glTranslatef((float)-(sx + sxoffs), (float)-(sy +"Render and decorate"XBeginNamedEvent(0,);
	if(m_pItemRenderer == NULL) m_pItemRenderer = new ItemRenderer();
	RenderManager.StateSetBlendEnable(true);
	RenderManager.StateSetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	RenderManager.StateSetBlendFactor(0xffffffff);
	m_pItemRenderer->renderAndDecorateItem(pMinecraft->font, pMinecraft->textures, item, x, y,scaleX,scaleY,fAlpha,isFoil,false, !usingCommandBuffer);
	PIXEndNamedEvent();

	if (pop > 0)
	{
            glPopMatrix();
	}

	if(bDecorations)
	{
            if ((scaleX != 1.0f) || (scaleY != 1.0f)) {
                glPushMatrix();
                glScalef(scaleX, scaleY, 1.0f);
                int iX = (int)(0.5f + ((float)x) / scaleX);
                int iY = (int)(0.5f + ((float)y) / scaleY);

                m_pItemRenderer->renderGuiItemDecorations(pMinecraft->font,
                                                          pMinecraft->textures,
                                                          item, iX, iY, fAlpha);
                glPopMatrix();
            } else {
                m_pItemRenderer->renderGuiItemDecorations(
                    pMinecraft->font, pMinecraft->textures, item, (int)x,
                    (int)y, fAlpha);
            }
	}

	Lighting::turnO// 4J Stu - Not threadsafeE//void UIScene::navigateForward(int iPad, EUIScene scene, void *initData)//{//	if(m_parentLayer == NULL)//	{//		app.DebugPrintf("A scene is trying to navigate forwards, but it's parent layer is NULL!\n");//#ifndef _CONTENT_PACKAGE//		__debugbreak();//#endif//	}//	else//	{//		m_parentLayer->NavigateToScene(iPad,scene,initData);//	}//}
//CD - Added for audionavigateBack()
{
            ui.PlayUISFX(eSFX_Back);

        ui.NavigateBack(//		app.DebugPrintf("A scene is trying to navigate back, but it's parent layer is NULL!\n");#ifndef _CONTENT_PACKAGE//		__debugbreak();#endif//		m_parentLayer->removeScene(this);
#ifdef _DURANGO

		if (ui.GetTopScene(0))
			InputManager.SetEnabledGtcButtons( ui.Ge#endifene(0)->getDefaultGtcButtons() );

	}
    }

    void UIScene::gainFocus() {
        // 4J Stu - Don't do thisFoc/*
        IggyEvent event;
        IggyMakeEventFocusGained(&event, 0);

        IggyEventResult result;
        IggyPlayerDispatchEventRS(getMovie(), &event, &result);

        app.DebugPrintf("Sent gain focus event to scene\n");
        */ bHasFocus = true;
        if (needsReloaded()) {
            reloadMovie();
        }

        updateTooltips();
        updateComponents();

        if (!m_bFocussedOnce) {
            IggyDataValue result;
            IggyDataValue value[1];

            value[0].type = IGGY_DATATYPE_number;
            value[0].number = -1;

            IggyResult out = IggyPlayerCallMethodRS(
                getMovie(), &result, IggyPlayerRootPath(getMovie()),
                m_funcSetFocus, 1, value);
        }

        handleGainFocus(m_bFocussedOnce);
        if (bHasFocus) m_bFocussedOnce = true;
    }
    else if (bHasFocus && stealsFocus()) {
        updateTooltips();
    }
}

void UIS  // 4J Stu - Don't do thisHas/*
    IggyEvent event;
IggyMakeEventFocusLost(&event);
IggyEventResult result;
IggyPlayerDispatchEventRS(getMovie(), &event, &result);
*/ "Sent lose focus event to scene\n" rintf();
bHasFocus = false;
handleLoseFocus();
}
}

void U#ifdef _DURANGOainFocus(bool navBack) {
    InputManager.SetEnabledGtcB #endif(this->getDefaultGtcButtons());
}

void UIScene::updateTooltips() {
    if (!ui.IsReloadingSkin()) ui.SetTooltips(m_iPad, -1);
}

void UIScene::sendInputToMovie(int key, bool repeat, bool pressed,
                               bool released) {
    if (!swf) return;

    int iggyKeyCode = convertGameActionToIggyKeycode(key);

    i "UI WARNING: Ignoring input as game action does not translate to an Iggy "
      "keycode\n"  // 4J Stu - Keyloc is always standard as we don't care about
                   // shift/alt
        IggyMakeEventKey(&keyEvent,
                         pressed ? IGGY_KEYEVENT_Down : IGGY_KEYEVENT_Up,
                         (IggyKeycode)iggyKeyCode, IGGY_KEYLOC_Standard);

    IggyEventResult result;
    IggyPlayerDispatchEventRS(swf, &keyEvent, &result);
}

int UIScene::conver  // TODO: This action to key mapping should probably use the
                     // control
                     // mapping#ifdef
                     // __ORBIS__ -1;
    switch (action) {
#endif case ACTION_MENU_TOUCHPAD_PRESS:

    case ACTION_MENU_A:
        keycode = IGGY_KEYCODE_ENTER;
        break;
    case ACTION_MENU_B:
        keycode = IGGY_KEYCODE_ESCAPE;
        break;
    case ACTION_MENU_X:
        keycode = IGGY_KEYCODE_F1;
        break;
    case ACTION_MENU_Y:
        keycode = IGGY_KEYCODE_F2;
        break;
    case ACTION_MENU_OK:
        keycode = IGGY_KEYCODE_ENTER;
        break;
    case ACTION_MENU_CANCEL:
        keycode = IGGY_KEYCODE_ESCAPE;
        break;
    case ACTION_MENU_UP:
        keycode = IGGY_KEYCODE_UP;
        break;
    case ACTION_MENU_DOWN:
        keycode = IGGY_KEYCODE_DOWN;
        break;
    case ACTION_MENU_RIGHT:
        keycode = IGGY_KEYCODE_RIGHT;
        break;
    case ACTION_MENU_LEFT:
        keycode = IGGY_KEYCODE_LEFT;
        break;
    case ACTION_MENU_PAGEUP:
        keycode = IGGY_KEYCODE_PAGE_U #ifdef __PSVITA__ ACTION_MENU_PAGEDOWN :

            if (!InputManager.IsVitaTV()) {
#endifode = IGGY_KEYCODE_F6;
        }
        else {
            keycode = IGGY_KEYCODE_PAGE_DOWN;
        }
        break;
    case ACTION_MENU_RIGHT_SCROLL:
        keycode = IGGY_KEYCODE_F3;
        break;
    case ACTION_MENU_LEFT_SCROLL:
        keycode = IGGY_KEYCODE_F4;
        break;
    case ACTION_MENU_STICK_PRESS:
        break;
    case ACTION_MENU_OTHER_STICK_PRESS:
        keycode = IGGY_KEYCODE_F5;
        break;
    case ACTION_MENU_OTHER_STICK_UP:
        keycode = IGGY_KEYCODE_F11;
        break;
    case ACTION_MENU_OTHER_STICK_DOWN:
        keycode = IGGY_KEYCODE_F12;
        break;
    case ACTION_MENU_OTHER_STICK_LEFT:
        break;
    case ACTION_MENU_OTHER_STICK_RIGHT:
        break;
};

return keycode;
}  // 4J-PB - ignore repeats of action ABXY buttons // fix for PS3 213 - [MAIN
   // MENU] Holding down buttons will continue to activate every prompt.
   //
switch (key) {
    case ACTION_MENU_OK:
    case ACTION_MENU_CANCEL:
    case ACTION_MENU_A:
    case ACTION_MENU_B:
    case ACTION_MENU_X:
    case ACTION_MENU_Y:
        return false;
}
return true;
}

void UIScene::externalCallback(IggyExternalFunctionCallUTF16* call) {
    if (wcscmp "handlePress" all->function_name.string, L)==0)
	{
            if (call->num
                "Callback for handlePress did not have the correct number of "
                "arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
    if (call->arguments[0].type != IGGY_DATATYPE_number ||
        call->arguments[1].type != IGGY_
            "Arguments for handlePress were not of the correct "
            "type\n" #ifndef _CONTENT_PACKAGE)
        ;
#endif
    __debugbreak();

    return;
}
handlePress(call->arguments[0].number, call->arguments[1].number);
}
        else if(wcscmp"handleFocusChange"unction_name.string,L)==0)
	{
            if (call->num
                "Callback for handleFocusChange did not have the correct "
                "number of "
                "arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        if (call->arguments[0].type != IGGY_DATATYPE_number ||
            call->arguments[1].type != IGGY_
                "Arguments for handleFocusChange were not of the correct "
                "type\n" #ifndef _CONTENT_PACKAGE)
            ;
#endif
        __debugbreak();

        return;
        }
        _handleFocusChange(call->arguments[0].number,
                           call->arguments[1].number);
        }
        else if(wcscmp"handleInitFocus">function_name.string,L)==0)
	{
            if (call->num
                "Callback for handleInitFocus did not have the correct number "
                "of arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        if (call->arguments[0].type != IGGY_DATATYPE_number ||
            call->arguments[1].type != IGGY_
                "Arguments for handleInitFocus were not of the correct "
                "type\n" #ifndef _CONTENT_PACKAGE)
            ;
#endif
        __debugbreak();

        return;
        }
        _handleInitFocus(call->arguments[0].number, call->arguments[1].number);
        }
        else if(wcscmp"handleCheckboxToggled"ion_name.string,L)==0)
	{
            if (call->num
                "Callback for handleCheckboxToggled did not have the correct "
                "number of "
                "arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        if (call->arguments[0].type != IGGY_DATATYPE_number ||
            call->arguments[1].type != IGGY_D
                "Arguments for handleCheckboxToggled were not of the correct "
                "type\n" #ifndef _CONTENT_PACKAGE)
            ;
#endif
        __debugbreak();

        return;
        }
        handleCheckboxToggled(call->arguments[0].number,
                              call->arguments[1].boolval);
        }
        else if(wcscmp"handleSliderMove"function_name.string,L)==0)
	{
            if (call->num
                "Callback for handleSliderMove did not have the correct number "
                "of arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        if (call->arguments[0].type != IGGY_DATATYPE_number ||
            call->arguments[1].type != IGGY_
                "Arguments for handleSliderMove were not of the correct "
                "type\n" #ifndef _CONTENT_PACKAGE)
            ;
#endif
        __debugbreak();

        return;
        }
        handleSliderMove(call->arguments[0].number, call->arguments[1].number);
        }
        else if(wcscmp"handleAnimationEnd"nction_name.string,L)==0)
	{
            if (call->num
                "Callback for handleAnimationEnd did not have the correct "
                "number of "
                "arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        handleAnimationEnd();
        }
        else if(wcscmp"handleSelectionChanged"on_name.string,L)==0)
	{
            if (call->num
                "Callback for handleSelectionChanged did not have the correct "
                "number of "
                "arguments\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        if (call->arguments[0].type != IGGY_
            "Arguments for handleSelectionChanged were not of the correct "
            "type\n" #ifndef _CONTENT_PACKAGE)
            ;
#endif
        __debugbreak();

        return;
        }
        handleSelectionChanged(call->arguments[0].number);
        }
        else if(wcscmp"handleRequestMoreData"ion_name.string,L)==0)
	{
            if (call->num_arguments == 0) {
                handleRequestMoreData(0, false);
            } else {
                if (call->num_a
                    "Callback for handleRequestMoreData did not have the "
                    "correct number of "
                    "arguments\n" #ifndef _CONTENT_PACKAGE)
                    ;
#endif
                __debugbreak();

                return;
            }
            if (call->arguments[0].type != IGGY_DATATYPE_number ||
                call->arguments[1].type != IGGY_DAT
                    "Arguments for handleRequestMoreData were not of the "
                    "correct type\n" #ifndef _CONTENT_PACKAGE)
                ;
#endif
            __debugbreak();

            return;
        }
        handleRequestMoreData(call->arguments[0].number,
                              call->arguments[1].boolval);
        }
        }
        else if(wcscmp"handleTouchBoxRebuild"ion_name.string,L)==0)
	{
            handleTouchBoxR "Unhandled callback: %s\n" p.DebugPrintf(
                , call->function_name.string);
        }
        }

        void UIScene::registerSubstitutionTexture(
            const std::wstring& textureName, PBYTE pbData, DWORD dwLength,
            bool deleteData) {
            m_registeredTextures[textureName] = deleteData;
            ;
            ui.registerSubstitutionTexture(textureName, pbData, dwLength);
        }

        bool UIScene::hasRegisteredSubstitutionTexture(
            const std::wstring& textureName) {
            AUTO_VAR(it, m_registeredTextures.find(textureName));

            return it != m_registeredTextures.end();
        }

        void UIScene::_handleFocusChange(F64 controlId, F64 childId) {
            m_iFocusControl = (int)controlId;
            m_iFocusChild = (int)childId;

            handleFocusChange(controlId, childId);
            ui.PlayUISFX(eSFX_Focus);
        }

        void UIScene::_handleInitFocus(F64 controlId, F64 childId) {
            m_iFocusControl = (int)cont  // handleInitFocus(controlId, childId);

                handleFocusChange(controlId, childId);
        }

        bool UIScene::controlHasFocus(int iControlId) {
            return m_iFocusControl == iControlId;
        }

        bool UIScene::controlHasFocus(UIControl_Base* control) {
            return controlHasFocus(control->getId());
        }

        int UIScene::getControlChildFocus() { return m_iFocusChild; }

        int UIScene::getControlFocus() { return m_iFocusControl; }

        void UIScene::setBackScene(UIScene* scene) { m_backScene = scene; }

        UIScene* UIScene::#ifdef __PSVITA__ return m_backScene;
        }

        void UIScene::UpdateSceneControls() {
            AUTO_VAR(itEnd, GetControls()->end());
            for (AUTO_VAR(it, GetControls()->begin()); it != itEnd; it++) {
                UIControl *control=(UIControl #endif
		control->UpdateControl();
            }
        }

        void UIScene::HandleMessage(EUIMessage message, void* data) {}

        std::size_t UIScene::GetCallbackUniqueId() {
            if (m_callbackUniqueId == 0) {
                m_callbackUniqueId = ui.RegisterForCallbackId(this);
            }
            return m_callbackUniqueId;
        }

        bool UIScene::isReadyToDelete() { return true; }