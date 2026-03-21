#include "../Platform/stdafx.h"
#include "LocalPlayer.h"
#include "User.h"
#include "../Input/Input.h"
#include "../GameState/StatsCounter.h"
#include "../Rendering/Particles/ParticleEngine.h"
#include "../Rendering/Particles/TakeAnimationParticle.h"
#include "../GameState/Options.h"
#include "../UI/Screens/TextEditScreen.h"
#include "../UI/Screens/ContainerScreen.h"
#include "../UI/Screens/CraftingScreen.h"
#include "../UI/Screens/FurnaceScreen.h"
#include "../UI/Screens/TrapScreen.h"

#include "MultiPlayerLocalPlayer.h"
#include "../GameState/CreativeMode.h"
#include "../Rendering/GameRenderer.h"
#include "../Rendering/EntityRenderers/ItemInHandRenderer.h"
#include "../../Minecraft.World/AI/Attributes/AttributeInstance.h"
#include "../../Minecraft.World/Level/LevelData.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.damagesource.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.item.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.food.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.effect.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.entity.player.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.entity.monster.h"
#include "../../Minecraft.World/Entities/ItemEntity.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.tile.entity.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.phys.h"
#include "../../Minecraft.World/Headers/net.minecraft.stats.h"
#include "../../Minecraft.World/Headers/com.mojang.nbt.h"
#include "../../Minecraft.World/Util/Random.h"
#include "../../Minecraft.World/Blocks/TileEntities/TileEntity.h"
#include "../../Minecraft.World/Util/Mth.h"
#include "../UI/Screens/AchievementPopup.h"
#include "../Rendering/Particles/CritParticle.h"

// 4J : WESTY : Added for new achievements.
#include "../../Minecraft.World/Items/Item.h"
#include "../../Minecraft.World/Items/MapItem.h"
#include "../../Minecraft.World/Blocks/Tile.h"

// 4J Stu - Added for tutorial callbacks
#include "../Minecraft.h"

#include "../../Minecraft.World/Entities/Mobs/Minecart.h"
#include "../../Minecraft.World/Entities/Mobs/Boat.h"
#include "../../Minecraft.World/Entities/Mobs/Pig.h"

#include "../../Minecraft.World/Util/StringHelpers.h"

#include "../GameState/Options.h"
#include "../../Minecraft.World/Level/Dimensions/Dimension.h"

#ifndef _DURANGO
#include "../../Minecraft.World/Stats/CommonStats.h"
#endif

LocalPlayer::LocalPlayer(Minecraft* minecraft, Level* level, User* user,
                         int dimension)
    : Player(level, user->name) {
    flyX = flyY = flyZ = 0.0f;  // 4J added
    m_awardedThisSession = 0;

    sprintTriggerTime = 0;
    sprintTriggerRegisteredReturn = false;
    twoJumpsRegistered = false;
    sprintTime = 0;
    m_uiInactiveTicks = 0;
    portalTime = 0.0f;
    oPortalTime = 0.0f;
    jumpRidingTicks = 0;
    jumpRidingScale = 0.0f;

    yBob = xBob = yBobO = xBobO = 0.0f;

    this->minecraft = minecraft;
    this->dimension = dimension;

    if (user != NULL && user->name.length() > 0) {
        customTextureUrl =
            L"http://s3.amazonaws.com/MinecraftSkins/" + user->name + L".png";
    }
    if (user != NULL) {
        this->name = user->name;
        // wprintf(L"Created LocalPlayer with name %ls\n", name.c_str() );
        //  check to see if this player's xuid is in the list of special players
        MOJANG_DATA* pMojangData = app.GetMojangDataForXuid(getOnlineXuid());
        if (pMojangData) {
            customTextureUrl = pMojangData->wchSkin;
        }
    }
    input = NULL;
    m_iPad = -1;
    m_iScreenSection =
        C4JRender::VIEWPORT_TYPE_FULLSCREEN;  // assume singleplayer default
    m_bPlayerRespawned = false;
    ullButtonsPressed = 0LL;
    ullDpad_last = ullDpad_this = ullDpad_filtered = 0;

    // 4J-PB - moved in from the minecraft structure
    // ticks=0;
    missTime = 0;
    lastClickTick[0] = 0;
    lastClickTick[1] = 0;
    isRaining = false;

    m_bIsIdle = false;
    m_iThirdPersonView = 0;

    // 4J Stu - Added for telemetry
    SetSessionTimerStart();

    // 4J - added for auto repeat in creative mode
    lastClickState = lastClick_invalid;
    lastClickTolerance = 0.0f;

    m_bHasAwardedStayinFrosty = false;
}

LocalPlayer::~LocalPlayer() {
    if (this->input != NULL) delete input;
}

void LocalPlayer::calculateFlight(float xa, float ya, float za) {
    xa = xa * minecraft->options->flySpeed;
    ya = 0;
    za = za * minecraft->options->flySpeed;

    flyX =
        smoothFlyX.getNewDeltaValue(xa, .35f * minecraft->options->sensitivity);
    flyY =
        smoothFlyY.getNewDeltaValue(ya, .35f * minecraft->options->sensitivity);
    flyZ =
        smoothFlyZ.getNewDeltaValue(za, .35f * minecraft->options->sensitivity);
}

void LocalPlayer::serverAiStep() {
    Player::serverAiStep();

    if (abilities.flying && abilities.mayfly) {
        // snap y rotation for flying to nearest 90 degrees in world space
        float fMag = sqrtf(input->xa * input->xa + input->ya * input->ya);
        // Don't bother for tiny inputs
        if (fMag >= 0.1f) {
            // Get angle (in player rotated space) of input controls
            float yRotInput = atan2f(input->ya, input->xa) * (180.0f / PI);
            // Now get in world space
            float yRotFinal = yRotInput + yRot;
            // Snap this to nearest 90 degrees
            float yRotSnapped = floorf((yRotFinal / 45.0f) + 0.5f) * 45.0f;
            // Find out how much we had to move to do this snap
            float yRotDiff = yRotSnapped - yRotFinal;
            // Apply the same difference to the player rotated space angle
            float yRotInputAdjust = yRotInput + yRotDiff;

            // Calculate final x/y player-space movement required
            this->xxa = cos(yRotInputAdjust * (PI / 180.0f)) * fMag;
            this->yya = sin(yRotInputAdjust * (PI / 180.0f)) * fMag;
        } else {
            this->xxa = input->xa;
            this->yya = input->ya;
        }
    } else {
        this->xxa = input->xa;
        this->yya = input->ya;
    }
    this->jumping = input->jumping;

    yBobO = yBob;
    xBobO = xBob;
    xBob += (xRot - xBob) * 0.5;
    yBob += (yRot - yBob) * 0.5;

    // TODO 4J - Remove
    // if (input->jumping)
    //	mapPlayerChunk(8);
}

bool LocalPlayer::isEffectiveAi() { return true; }

void LocalPlayer::aiStep() {
    if (sprintTime > 0) {
        sprintTime--;
        if (sprintTime == 0) {
            setSprinting(false);
        }
    }
    if (sprintTriggerTime > 0) sprintTriggerTime--;
    if (minecraft->gameMode->isCutScene()) {
        x = z = 0.5;
        x = 0;
        z = 0;
        yRot = tickCount / 12.0f;
        xRot = 10;
        y = 68.5;
        return;
    }
    oPortalTime = portalTime;
    if (isInsidePortal) {
        if (!level->isClientSide) {
            if (riding != NULL) this->ride(nullptr);
        }
        if (minecraft->screen != NULL) minecraft->setScreen(NULL);

        if (portalTime == 0) {
            minecraft->soundEngine->playUI(eSoundType_PORTAL_TRIGGER, 1,
                                           random->nextFloat() * 0.4f + 0.8f);
        }
        portalTime += 1 / 80.0f;
        if (portalTime >= 1) {
            portalTime = 1;
        }
        isInsidePortal = false;
    } else if (hasEffect(MobEffect::confusion) &&
               getEffect(MobEffect::confusion)->getDuration() >
                   (SharedConstants::TICKS_PER_SECOND * 3)) {
        portalTime += 1 / 150.0f;
        if (portalTime > 1) {
            portalTime = 1;
        }
    } else {
        if (portalTime > 0) portalTime -= 1 / 20.0f;
        if (portalTime < 0) portalTime = 0;
    }

    if (changingDimensionDelay > 0) changingDimensionDelay--;
    bool wasJumping = input->jumping;
    float runTreshold = 0.8f;

    bool wasRunning = input->ya >= runTreshold;
    // input->tick( dynamic_pointer_cast<Player>( shared_from_this() ) );
    //  4J-PB - make it a localplayer
    input->tick(this);
    if (isUsingItem() && !isRiding()) {
        input->xa *= 0.2f;
        input->ya *= 0.2f;
        sprintTriggerTime = 0;
    }
    // this.heightOffset = input.sneaking?1.30f:1.62f;	// 4J - this was already
    // commented out
    if (input->sneaking)  // 4J - removed - TODO replace
    {
        if (ySlideOffset < 0.2f) ySlideOffset = 0.2f;
    }

    checkInTile(x - bbWidth * 0.35, bb->y0 + 0.5, z + bbWidth * 0.35);
    checkInTile(x - bbWidth * 0.35, bb->y0 + 0.5, z - bbWidth * 0.35);
    checkInTile(x + bbWidth * 0.35, bb->y0 + 0.5, z - bbWidth * 0.35);
    checkInTile(x + bbWidth * 0.35, bb->y0 + 0.5, z + bbWidth * 0.35);

    bool enoughFoodToSprint =
        getFoodData()->getFoodLevel() >
        FoodConstants::MAX_FOOD * FoodConstants::FOOD_SATURATION_LOW;

    // 4J Stu - If we can fly, then we should be able to sprint without
    // requiring food. This is particularly a problem for people who save a
    // survival world with low food, then reload it in creative.
    if (abilities.mayfly || isAllowedToFly()) enoughFoodToSprint = true;

    // 4J - altered this slightly to make sure that the joypad returns to below
    // returnTreshold in between registering two movements up to runThreshold
    if (onGround && !isSprinting() && enoughFoodToSprint && !isUsingItem() &&
        !hasEffect(MobEffect::blindness)) {
        if (!wasRunning && input->ya >= runTreshold) {
            if (sprintTriggerTime == 0) {
                sprintTriggerTime = 7;
                sprintTriggerRegisteredReturn = false;
            } else {
                if (sprintTriggerRegisteredReturn) {
                    setSprinting(true);
                    sprintTriggerTime = 0;
                    sprintTriggerRegisteredReturn = false;
                }
            }
        } else if ((sprintTriggerTime > 0) &&
                   (input->ya == 0.0f))  // ya of 0.0f here signifies that we
                                         // have returned to the deadzone
        {
            sprintTriggerRegisteredReturn = true;
        }
    }
    if (isSneaking()) sprintTriggerTime = 0;
    // 4J-PB - try not stopping sprint on collision
    // if (isSprinting() && (input->ya < runTreshold || horizontalCollision ||
    // !enoughFoodToSprint))
    if (isSprinting() && (input->ya < runTreshold || !enoughFoodToSprint)) {
        setSprinting(false);
    }

    // 4J Stu - Fix for #52705 - Customer Encountered: Player can fly in bed
    // while being in Creative mode.
    if (!isSleeping() && (abilities.mayfly || isAllowedToFly())) {
        // 4J altered to require jump button to released after being tapped
        // twice to trigger move between flying / not flying
        if (!wasJumping && input->jumping) {
            if (jumpTriggerTime == 0) {
                jumpTriggerTime = 10;  // was 7
                twoJumpsRegistered = false;
            } else {
                twoJumpsRegistered = true;
            }
        } else if ((!input->jumping) && (jumpTriggerTime > 0) &&
                   twoJumpsRegistered) {
#ifndef _CONTENT_PACKAGE
            printf("flying was %s\n", abilities.flying ? "on" : "off");
#endif
            abilities.flying = !abilities.flying;
#ifndef _CONTENT_PACKAGE
            printf("flying is %s\n", abilities.flying ? "on" : "off");
#endif
            jumpTriggerTime = 0;
            twoJumpsRegistered = false;
            if (abilities.flying)
                input->sneaking =
                    false;  // 4J added - would we ever intentially want to go
                            // into flying mode whilst sneaking?
        }
    } else if (abilities.flying) {
#ifdef _DEBUG_MENUS_ENABLED
        if (!abilities.debugflying)
#endif
        {
            abilities.flying = false;
        }
    }

    if (abilities.flying) {
        //            yd = 0;
        // 4J - note that the 0.42 added for going down is to make it match with
        // what happens when you jump - jumping itself adds 0.42 to yd in
        // Mob::jumpFromGround
        if (ullButtonsPressed & (1LL << MINECRAFT_ACTION_SNEAK_TOGGLE))
            yd -=
                (0.15 + 0.42);  // 4J - for flying mode,
                                // MINECRAFT_ACTION_SNEAK_TOGGLE isn't a toggle
                                // but just indicates that this button is down
        if (input->jumping) {
            noJumpDelay = 0;
            yd += 0.15;
        }

        // snap y rotation to nearest 90 degree axis aligned value
        float yRotSnapped = floorf((yRot / 90.0f) + 0.5f) * 90.0f;

        if (InputManager.GetJoypadMapVal(m_iPad) == 0) {
            if (ullDpad_filtered & (1LL << MINECRAFT_ACTION_DPAD_RIGHT)) {
                xd = -0.15 * cos(yRotSnapped * PI / 180);
                zd = -0.15 * sin(yRotSnapped * PI / 180);
            } else if (ullDpad_filtered & (1LL << MINECRAFT_ACTION_DPAD_LEFT)) {
                xd = 0.15 * cos(yRotSnapped * PI / 180);
                zd = 0.15 * sin(yRotSnapped * PI / 180);
            }
        }
    }

    if (isRidingJumpable()) {
        if (jumpRidingTicks < 0) {
            jumpRidingTicks++;
            if (jumpRidingTicks == 0) {
                // reset scale (for gui)
                jumpRidingScale = 0;
            }
        }
        if (wasJumping && !input->jumping) {
            // jump release
            jumpRidingTicks = -10;
            sendRidingJump();
        } else if (!wasJumping && input->jumping) {
            // jump press
            jumpRidingTicks = 0;
            jumpRidingScale = 0;
        } else if (wasJumping) {
            // calc jump scale
            jumpRidingTicks++;
            if (jumpRidingTicks < 10) {
                jumpRidingScale = (float)jumpRidingTicks * .1f;
            } else {
                jumpRidingScale =
                    .8f + (2.f / ((float)(jumpRidingTicks - 9))) * .1f;
            }
        }
    } else {
        jumpRidingScale = 0;
    }

    Player::aiStep();

    // 4J-PB - If we're in Creative Mode, allow flying on ground
    if (!abilities.mayfly && !isAllowedToFly()) {
        if (onGround && abilities.flying) {
#ifdef _DEBUG_MENUS_ENABLED
            if (!abilities.debugflying)
#endif
            {
                abilities.flying = false;
            }
        }
    }

    if (abilities.flying)  // minecraft->options->isFlying )
    {
        Vec3* viewVector = getViewVector(1.0f);

        // 4J-PB - To let the player build easily while flying, we need to
        // change this

#ifdef _DEBUG_MENUS_ENABLED
        if (abilities.debugflying) {
            flyX = (float)viewVector->x * input->ya;
            flyY = (float)viewVector->y * input->ya;
            flyZ = (float)viewVector->z * input->ya;
        } else
#endif
        {
            if (isSprinting()) {
                // Accelrate up to full speed if we are sprinting, moving in the
                // direction of the view vector
                flyX = (float)viewVector->x * input->ya;
                flyY = (float)viewVector->y * input->ya;
                flyZ = (float)viewVector->z * input->ya;

                float scale = ((float)(SPRINT_DURATION - sprintTime)) / 10.0f;
                scale = scale * scale;
                if (scale > 1.0f) scale = 1.0f;
                flyX *= scale;
                flyY *= scale;
                flyZ *= scale;
            } else {
                flyX = 0.0f;
                flyY = 0.0f;
                flyZ = 0.0f;
                if (ullDpad_filtered & (1LL << MINECRAFT_ACTION_DPAD_UP)) {
                    flyY = 0.1f;
                }
                if (ullDpad_filtered & (1LL << MINECRAFT_ACTION_DPAD_DOWN)) {
                    flyY = -0.1f;
                }
            }
        }

        Player::move(flyX, flyY, flyZ);

        fallDistance = 0.0f;
        yd = 0.0f;
        onGround = true;
    }

    // Check if the player is idle and the rich presence needs updated
    if (!m_bIsIdle && InputManager.GetIdleSeconds(m_iPad) > PLAYER_IDLE_TIME) {
        ProfileManager.SetCurrentGameActivity(m_iPad, CONTEXT_PRESENCE_IDLE,
                                              false);
        m_bIsIdle = true;
    } else if (m_bIsIdle &&
               InputManager.GetIdleSeconds(m_iPad) < PLAYER_IDLE_TIME) {
        // Are we offline or online, and how many players are there
        if (g_NetworkManager.GetPlayerCount() > 1) {
            // only do it for this player here - each player will run this code
            if (g_NetworkManager.IsLocalGame()) {
                ProfileManager.SetCurrentGameActivity(
                    m_iPad, CONTEXT_PRESENCE_MULTIPLAYEROFFLINE, false);
            } else {
                ProfileManager.SetCurrentGameActivity(
                    m_iPad, CONTEXT_PRESENCE_MULTIPLAYER, false);
            }
        } else {
            if (g_NetworkManager.IsLocalGame()) {
                ProfileManager.SetCurrentGameActivity(
                    m_iPad, CONTEXT_PRESENCE_MULTIPLAYER_1POFFLINE, false);
            } else {
                ProfileManager.SetCurrentGameActivity(
                    m_iPad, CONTEXT_PRESENCE_MULTIPLAYER_1P, false);
            }
        }
        updateRichPresence();
        m_bIsIdle = false;
    }
}

void LocalPlayer::changeDimension(int i) {
    if (!level->isClientSide) {
        if (dimension == 1 && i == 1) {
            awardStat(GenericStats::winGame(), GenericStats::param_noArgs());
            // minecraft.setScreen(new WinScreen());
#ifndef _CONTENT_PACKAGE
            app.DebugPrintf(
                "LocalPlayer::changeDimension from 1 to 1 but WinScreen has "
                "not been implemented.\n");
            __debugbreak();
#endif
        } else {
            awardStat(GenericStats::theEnd(), GenericStats::param_theEnd());

            minecraft->soundEngine->playUI(eSoundType_PORTAL_TRAVEL, 1,
                                           random->nextFloat() * 0.4f + 0.8f);
        }
    }
}

float LocalPlayer::getFieldOfViewModifier() {
    float targetFov = 1.0f;

    // modify for movement
    if (abilities.flying) targetFov *= 1.1f;

    AttributeInstance* speed =
        getAttribute(SharedMonsterAttributes::MOVEMENT_SPEED);
    targetFov *= (speed->getValue() / abilities.getWalkingSpeed() + 1) / 2;

    // modify for bow =)
    if (isUsingItem() && getUseItem()->id == Item::bow->id) {
        int ticksHeld = getTicksUsingItem();
        float scale = (float)ticksHeld / BowItem::MAX_DRAW_DURATION;
        if (scale > 1) {
            scale = 1;
        } else {
            scale *= scale;
        }
        targetFov *= 1.0f - scale * .15f;
    }

    return targetFov;
}

void LocalPlayer::addAdditonalSaveData(CompoundTag* entityTag) {
    Player::addAdditonalSaveData(entityTag);
    // entityTag->putInt(L"Score", score);
}

void LocalPlayer::readAdditionalSaveData(CompoundTag* entityTag) {
    Player::readAdditionalSaveData(entityTag);
    // score = entityTag->getInt(L"Score");
}

void LocalPlayer::closeContainer() {
    Player::closeContainer();
    minecraft->setScreen(NULL);

    // 4J - Close any xui here
    // Fix for #9164 - CRASH: MP: Title crashes upon opening a chest and having
    // another user destroy it.
    ui.PlayUISFX(eSFX_Back);
    ui.CloseUIScenes(m_iPad);
}

void LocalPlayer::openTextEdit(std::shared_ptr<TileEntity> tileEntity) {
    bool success;

    if (tileEntity->GetType() == eTYPE_SIGNTILEENTITY) {
        success = app.LoadSignEntryMenu(
            GetXboxPad(), dynamic_pointer_cast<SignTileEntity>(tileEntity));
    } else if (tileEntity->GetType() == eTYPE_COMMANDBLOCKTILEENTITY) {
        success = app.LoadCommandBlockMenu(
            GetXboxPad(), dynamic_pointer_cast<CommandBlockEntity>(tileEntity));
    }

    if (success) ui.PlayUISFX(eSFX_Pres//minecraft->setScreen(new TextEditScreen(sign));
}

bool LocalPlayer::openContainer(std::shared_ptr<Container> container) {
    bool success = app.LoadContainerMenu(GetXboxPad(), inventory, container);
    if (success) ui.PlayUISFX(eSFX_Pres//minecraft->setScreen(new ContainerScreen(inventory, container));
	return success;
}

bool LocalPlayer::openHopper(std::shared_ptr<HopperTileEntity> cont//minecraft->setScreen(new HopperScreen(inventory, container));
	bool success = app.LoadHopperMenu(GetXboxPad(), inventory, container );
	if( success ) ui.PlayUISFX(eSFX_Press);
	return success;
}

bool LocalPlayer::openHopper(std::shared_ptr<MinecartHopper>//minecraft->setScreen(new HopperScreen(inventory, container));
	bool success = app.LoadHopperMenu(GetXboxPad(), inventory, container );
	if( success ) ui.PlayUISFX(eSFX_Press);
	return success;
}

bool LocalPlayer::openHorseInventory(std::shared_ptr<EntityHorse> horse, std::shared_ptr<Conta//minecraft->setScreen(new HorseInventoryScreen(inventory, container, horse));
	bool success = app.LoadHorseMenu(GetXboxPad(), inventory, container, horse);
	if( success ) ui.PlayUISFX(eSFX_Press);
	return success;
}

bool LocalPlayer::startCrafting(int x, int y, int z) {
    bool success = app.LoadCrafting3x3Menu(
        GetXboxPad(), dynamic_pointer_cast<LocalPlayer>(shared_from_this()), x,
        y, z);
    if (success)
        ui.Play  // app.LoadXuiCraftMenu(0,inventory, level, x, y,
                 // z);//minecraft->setScreen(new CraftingScreen(inventory,
                 // level, x, y, z));
            return success;
}

bool LocalPlayer::openFireworks(int x, int y, int z) {
    bool success = app.LoadFireworksMenu(
        GetXboxPad(), dynamic_pointer_cast<LocalPlayer>(shared_from_this()), x,
        y, z);
    if (success) ui.PlayUISFX(eSFX_Press);
    return success;
}

bool LocalPlayer::startEnchanting(int x, int y, int z,
                                  const std::wstring& name) {
    bool success =
        app.LoadEnchantingMenu(GetXboxPad(), inventory, x, y, z, level, name);
    if (success)
        ui.Play  // minecraft.setScreen(new EnchantmentScreen(inventory, level,
                 // x, y, z));
            return success;
}

bool LocalPlayer::startRepairing(int x, int y, int z) {
    bool success =
        app.LoadRepairingMenu(GetXboxPad(), inventory, level, x, y, z);
    if (success)
        ui.Play  // minecraft.setScreen(new RepairScreen(inventory, level, x, y,
                 // z));
            return success;
}

bool LocalPlayer::openFurnace(std::shared_ptr<FurnaceTileEntity> furnace) {
    bool success = app.LoadFurnaceMenu(GetXboxPad(), inventory, furnace);
    if (success)
        ui.Play  // minecraft->setScreen(new FurnaceScreen(inventory,
                 // furnace));
            return success;
}

bool LocalPlayer::openBrewingStand(
    std::shared_ptr<BrewingStandTileEntity> brewingStand) {
    bool success =
        app.LoadBrewingStandMenu(GetXboxPad(), inventory, brewingStand);
    if (success)
        ui  // minecraft.setScreen(new BrewingStandScreen(inventory,
            // brewingStand));
            return success;
}

bool LocalPlayer::openBeacon(std::shared_ptr//minecraft->setScreen(new BeaconScreen(inventory, beacon));
	bool success = app.LoadBeaconMenu(GetXboxPad(), inventory, beacon);
	if( success ) ui.PlayUISFX(eSFX_Press);
	return success;
}

bool LocalPlayer::openTrap(std::shared_ptr<DispenserTileEntity> trap) {
    bool success = app.LoadTrapMenu(GetXboxPad(), inventory, trap);
        if( success//minecraft->setScreen(new TrapScreen(inventory, trap));
	return success;
}

bool LocalPlayer::openTrading(std::shared_ptr<Merchant> traderTarget,
                              const std::wstring& name) {
    bool success =
        app.LoadTradingMenu(GetXboxPad(), inventory, traderTarget, level, name);
        if( su//minecraft.setScreen(new MerchantScreen(inventory, traderTarget, level));
	return success;
}

void LocalPlayer::crit(std::shared_ptr<Entity> e) {
    std::shared_ptr<CritParticle> critParticle = std::shared_ptr<CritParticle>(
        new CritParticle((Level*)minecraft->level, e));
    critParticle->CritParticlePostConstructor();
    minecraft->particleEngine->add(critParticle);
}

void LocalPlayer::magicCrit(std::shared_ptr<Entity> e) {
    std::shared_ptr<CritParticle> critParticle = std::shared_ptr<CritParticle>(
        new CritParticle((Level*)minecraft->level, e, eParticleType_magicCrit));
    critParticle->CritParticlePostConstructor();
    minecraft->particleEngine->add(critParticle);
}

void LocalPlayer::take(std::shared_ptr<Entity> e, int orgCount) {
    minecraft->particleEngine->add(
        std::shared_ptr<TakeAnimationParticle>(new TakeAnimationParticle(
            (Level*)minecraft->level, e, shared_from_this(), -0.5f)));
}

void LocalPlayer::chat(const std::wstring& message) {}

bool LocalPlayer::isSneaking() { return input->sneaking && !m_isSleeping; }

void LocalPlayer::hurtTo(float newHealth, ETelemetryChallenges damageSource) {
    float dmg = getHealth() - newHealth;
    if (dmg <= 0) {
        setHealth(newHealth);
        if (dmg < 0) {
            invulnerableTime = invulnerableDuration / 2;
        }
    } else {
        lastHurt = dmg;
        setHealth(getHealth());
        invulnerableTime = invulnerableDuration;
        actuallyHurt(DamageSource::genericSource, dmg);
        hurtTime = hurtDuration = 10;
    }

    if (this->getHealth() <= 0) {
        int deathTime =
            (int)(level->getGameTime() % Level::TICKS_PER_DAY) / 1000;
        int carriedId =
            inventory->getSelected() == NULL ? 0 : inventory->getSelected()->id;
        TelemetryManager->RecordPlayerDiedOrFaile  // if there are any xuiscenes
                                                   // up for this player, close
                                                   // them
            if (ui.GetMenuDisplayed(GetXboxPad())) {
            ui.CloseUIScen  // Select the right payer to
                            // respawncalPlayer::respawn()
            {
        minecraft->respawnPlayer(G//        Player.animateRespawn(this, level);ateRespawn()
{
                    
}

void LocalPlayer::displayClientMessage(int messageId)
{
                    minecraft->gui->displayClientMessage(messageId,
                                                         GetXboxPad());
}
#ifdef _DURANGOer  // 4J-JEV: Maybe we want to fine tune this later? #TODO
	
	if (	   !ProfileManager.IsGuest(GetXboxPad()) 
			&& app.CanRecordStatsAndAchievements()
			&& ProfileManager.IsFullVersion()
		)
	{
                stat->handleParamBlob(dynamic_pointer_cast<LocalPla#elseshared_from_this()), param);
	}
	delete [] param.data;

	int count = CommonStats::readParam(param);
	delete [] param.data;

	if (!app.CanRecordStatsAndAchievements())	return;
	if (stat == NULL)							return;

	if (stat->isAch// 4J-PB - changed to attempt to award everytime - the award may need a storage device, so needs a primary player, and the player may not have been a primary player when they first 'got' the award// so let the award manager figure it out//if (!minecraft->stats[m_iPad]->hasTaken(ach))// 4J-PB - Don't display the java popup//minecraft->achievementPopup->popup(ach);// 4J Stu - Added this function in the libraries as some achievements don't get awarded to all players// e.g. Splitscreen players cannot get theme/avatar/gamerpic and Trial players cannot get any// This causes some extreme flooding of some awards
			
			if(ProfileManage// 4J Stu - We don't (currently) care about the gamerscore, so setting to a default of 0 points
				TelemetryManager->Record// 4J Stu - Some awards cause a menu to popup. This can be bad, especially if you are surrounded by mobs!// We cannot pause the game unless in offline single player, but lets at least do it then
				if( g_NetworkManager.IsLocalGame() && g_NetworkManager.GetPlayerCount() == 1 && ProfileManager.GetAwardType(ach->getAchievementID() ) != eAwardType_Achievement )
				{
                    ui.CloseUIScenes(m_iPad);
                    ui  // 4J-JEV: To stop spamming trophies.eMenu);
				}
            }

            unsigned long long achBit = ((unsigned long long)1)
                                        << ach->getAchievementID();
            if (!(achBit & m_awardedThisSession)) {
                ProfileManager.Award(m_iPad, ach->getAchievementID());
                if (ProfileManager.IsFullVersion())
                    m_awardedThisSession |= achBit;
            }
        }
                minecraft->stats[m_// 4J : WESTY : Added for new achievements.);
    } else {
                StatsCounter* pStats = minecraft->stats[m// 4J-JEV: Check achievements for unlocks.cult// LEADER OF THE PACK

		
		// Check to see if we have befriended 5 wolves! Is this really the best place to do this??!!
			if ( pStats->getTotalValue(GenericStats::tamedEntity(eTYPE_WOLF)) >= 5 )
			{
                                awardStat(GenericStats::le// MOAR TOOLS(), GenericStats::param_noArgs());
			}
    }

    {
        Stat* toolStats[4][5];
        toolStats[0][0] = GenericStats::itemsCrafted(Item::shovel_wood->id);
        toolStats[0][1] = GenericStats::itemsCrafted(Item::shovel_stone->id);
        toolStats[0][2] = GenericStats::itemsCrafted(Item::shovel_iron->id);
        toolStats[0][3] = GenericStats::itemsCrafted(Item::shovel_diamond->id);
        toolStats[0][4] = GenericStats::itemsCrafted(Item::shovel_gold->id);
        toolStats[1][0] = GenericStats::itemsCrafted(Item::pickAxe_wood->id);
        toolStats[1][1] = GenericStats::itemsCrafted(Item::pickAxe_stone->id);
        toolStats[1][2] = GenericStats::itemsCrafted(Item::pickAxe_iron->id);
        toolStats[1][3] = GenericStats::itemsCrafted(Item::pickAxe_diamond->id);
        toolStats[1][4] = GenericStats::itemsCrafted(Item::pickAxe_gold->id);
        toolStats[2][0] = GenericStats::itemsCrafted(Item::hatchet_wood->id);
        toolStats[2][1] = GenericStats::itemsCrafted(Item::hatchet_stone->id);
        toolStats[2][2] = GenericStats::itemsCrafted(Item::hatchet_iron->id);
        toolStats[2][3] = GenericStats::itemsCrafted(Item::hatchet_diamond->id);
        toolStats[2][4] = GenericStats::itemsCrafted(Item::hatchet_gold->id);
        toolStats[3][0] = GenericStats::itemsCrafted(Item::hoe_wood->id);
        toolStats[3][1] = GenericStats::itemsCrafted(Item::hoe_stone->id);
        toolStats[3][2] = GenericStats::itemsCrafted(Item::hoe_iron->id);
        toolStats[3][3] = GenericStats::itemsCrafted(Item::hoe_diamond->id);
        toolStats[3][4] = GenericStats::itemsCrafted(Item::hoe_gold->id);

        bool justCraftedTool = false;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 5; j++) {
                if (stat == toolStats[i][j]) {
                    justCraftedTool = true;
                    break;
                }
            }
        }

        if (justCraftedTool) {
            bool awardNow = true;
            for (int i = 0; i < 4; i++) {
                bool craftedThisTool = false;
                for (int j = 0; j < 5; j++) {
                    if (pStats->getTotalValue(toolStats[i][j]) > 0)
                        craftedThisTool = true;
                }

                if (!craftedThisTool) {
                    awardNow = false;
                    break;
                }
            }

            if (awardNow) {
                                        awardStat(GenericStats::MOA
#ifdef _XBOXeri  // AWARD: Have we killed 10 creepers?
            }

            if (pStats->getTotalValue(GenericStats::killsCreeper()) >= 10) {
                        awardStat( GenericSta// AWARD : Have we been playing for 100 game days?);
            }

            if (pStats->getTotalValue(GenericStats::timePlayed()) >=
                (Level::TICKS_PER_DAY * 100)) {
                        awardStat( Generi// AWARD : Have we mined 100 blocks?:param_noArgs());
            }

            if (pStats->getTotalValue(GenericStats::totalBlocksMined()) >=
                100) {
                        awardStat( Generi#endif:
#ifdef _EXTENDED_ACHIEVEMENTS::pa  // AWARD : Porkchop, cook and eat a
                                   // porkchop.

		
		{
                    Stat *cookPorkchop, *eatPorkchop;
                    cookPorkchop =
                        GenericStats::itemsCrafted(Item::porkChop_cooked_Id);
                    eatPorkchop =
                        GenericStats::itemsUsed(Item::porkChop_cooked_Id);

                    if (stat == cookPorkchop || stat == eatPorkchop) {
                        int numCookPorkchop, numEatPorkchop;
                        numCookPorkchop = pStats->getTotalValue(cookPorkchop);
                                numEatPorkchop = pSt"[AwardStat] Check unlock 'Porkchop': "pp.Deb"pork_cooked=%i, pork_eaten=%i.\n"
					,
					numCookPorkchop, numEatPorkchop
					);

                                if ((0 < numCookPorkchop) &&
                                    (0 < numEatPorkchop)) {
                                        awardStat( GenericStats::pork// AWARD : Passing the Time, play for 100 minecraft days.
		
		{
                                Stat* timePlayed = GenericStats::timePlayed();

                                if (stat == timePlayed) {
                                    int iPlayedTicks, iRequiredTicks;
                                iPlayedTicks = pStats->getTotalValue(timePla/* app.DebugPrintf(
					"[AwardStat] Check unlock 'Passing the Time': "
					"total_ticks=%i, req=%i.\n",
					iPlayedTicks, iRequiredTicks
					); */

				if (iPlayedTicks >= iRequiredTicks)
				{
                                        awardStat( GenericStats::passingTheTime()// AWARD : The Haggler, Acquire 30 emeralds.		}
			}
                                }

                                {
                                    Stat *emeraldMined, *emeraldBought;
                                    emeraldMined = GenericStats::blocksMined(
                                        Tile::emeraldOre_Id);
                                    emeraldBought = GenericStats::itemsBought(
                                        Item::emerald_Id);

                                    if (stat == emeraldMined ||
                                        stat == emeraldBought) {
                                        int numEmeraldMined, numEmeraldBought,
                                            totalSum;
                                        numEmeraldMined =
                                            pStats->getTotalValue(emeraldMined);
                                        numEmeraldBought =
                                            pStats->getTotalValue(
                                                emeraldBought);
                                totalSum = numEmer"[AwardStat] Check unlock 'The Haggler': "DebugP"emerald_mined=%i, emerald_bought=%i, sum=%i.\n"
					,
					numEmeraldMined, numEmeraldBought, totalSum
					);

                                if (totalSum >= 30)	awardStat( GenericStats::th// AWARD : Pot Planter, craft and place a flowerpot.
                                    }

                                    {
                                        Stat *craftFlowerpot, *placeFlowerpot;
                                        craftFlowerpot =
                                            GenericStats::itemsCrafted(
                                                Item::flowerPot_Id);
                                        placeFlowerpot =
                                            GenericStats::blocksPlaced(
                                                Tile::flowerPot_Id);

                                        if (stat == craftFlowerpot ||
                                            stat == placeFlowerpot) {
                                            if ((pStats->getTotalValue(
                                                     craftFlowerpot) > 0) &&
                                                (pStats->getTotalValue(
                                                     placeFlowerpot) > 0)) {
                                        awardStat( GenericStats::potPlant// AWARD : It's a Sign, craft and place a sign.
                                            }
                                        }

                                        {
                                            Stat *craftSign, *placeWallsign,
                                                *placeSignpost;
                                            craftSign =
                                                GenericStats::itemsCrafted(
                                                    Item::sign_Id);
                                            placeWallsign =
                                                GenericStats::blocksPlaced(
                                                    Tile::wallSign_Id);
                                            placeSignpost =
                                                GenericStats::blocksPlaced(
                                                    Tile::sign_Id);

                                            if (stat == craftSign ||
                                                stat == placeWallsign ||
                                                stat == placeSignpost) {
                                                int numCraftedSigns,
                                                    numPlacedWallSign,
                                                    numPlacedSignpost;
                                                numCraftedSigns =
                                                    pStats->getTotalValue(
                                                        craftSign);
                                                numPlacedWallSign =
                                                    pStats->getTotalValue(
                                                        placeWallsign);
                                numPlacedSignpost	= pStat"[AwardStat] Check unlock 'It's a Sign': "DebugP"crafted=%i, placedWallSigns=%i, placedSignposts=%i.\n"					,
					numCraftedSigns, numPlacedWallSign, numPlacedSignpost
					);

                                if ((numCraftedSigns > 0) &&
                                    ((numPlacedWallSign + numPlacedSignpost) >
                                     0)) {
                                        awardStat( GenericStats::its// AWARD : Rainbow Collection, collect all different colours of wool.
		{
                                                        bool justPickedupWool =
                                                            false;

                                                        for (int i = 0; i < 16;
                                                             i++)
                                                            if (stat ==
                                                                GenericStats::
                                                                    itemsCollected(
                                                                        Tile::
                                                                            wool_Id,
                                                                        i))
                                                                justPickedupWool =
                                                                    true;

                                                        if (justPickedupWool) {
                                                            unsigned int
                                                                woolCount = 0;

                                                            for (unsigned int
                                                                     i = 0;
                                                                 i < 16; i++) {
                                                                if (pStats->getTotalValue(
                                                                        GenericStats::itemsCollected(
                                                                            Tile::
                                                                                wool_Id,
                                                                            i)) >
                                                                    0)
                                                                    woolCount++;
                                                            }

                                                            if (woolCount >= 16) awardStat( GenericStats::rainbowCollectio// AWARD : Adventuring Time, visit at least 17 biomes		}

		
		{
                                                                    bool justEnteredBiome =
                                                                        false;

                                                                    for (int i =
                                                                             0;
                                                                         i < 23;
                                                                         i++)
                                                                        if (stat ==
                                                                            GenericStats::
                                                                                enteredBiome(
                                                                                    i))
                                                                            justEnteredBiome =
                                                                                true;

                                                                    if (justEnteredBiome) {
                                                                        unsigned int
                                                                            biomeCount =
                                                                                0;

                                                                        for (
                                                                            unsigned int
                                                                                i = 0;
                                                                            i <
                                                                            23;
                                                                            i++) {
                                                                            if (pStats
                                                                                    ->getTotalValue(
                                                                                        GenericStats::
                                                                                            enteredBiome(
                                                                                                i)) >
                                                                                0)
                                                                                biomeCount++;
                                                                        }

                                                                        if (biomeCount >=
                                                                            17)
                                                                            awardStat(
                                                                                GenericStats::
                                                                                    adventuri #endif(),
#endifcStats::
                                                                                param_adventuringTime());
                                                                    }
		}
                                                        }
                                                        
}

bool LocalPlayer::isSolidBlock(int x, int y, int z)
{
                                                        return level
                                                            ->isSolidBlockingTile(
                                                                x, y, z);
}

bool LocalPlayer::checkInTile(double x, double y, double z)
{
                                                        int xTile =
                                                            Mth::floor(x);
                                                        int yTile =
                                                            Mth::floor(y);
                                                        int zTile =
                                                            Mth::floor(z);

                                                        double xd = x - xTile;
                                                        double zd = z - zTile;

                                                        if (isSolidBlock(
                                                                xTile, yTile,
                                                                zTile) ||
                                                            isSolidBlock(
                                                                xTile,
                                                                yTile + 1,
                                                                zTile)) {
                                                            bool west =
                                                                !isSolidBlock(
                                                                    xTile - 1,
                                                                    yTile,
                                                                    zTile) &&
                                                                !isSolidBlock(
                                                                    xTile - 1,
                                                                    yTile + 1,
                                                                    zTile);
                                                            bool east =
                                                                !isSolidBlock(
                                                                    xTile + 1,
                                                                    yTile,
                                                                    zTile) &&
                                                                !isSolidBlock(
                                                                    xTile + 1,
                                                                    yTile + 1,
                                                                    zTile);
                                                            bool north =
                                                                !isSolidBlock(
                                                                    xTile,
                                                                    yTile,
                                                                    zTile -
                                                                        1) &&
                                                                !isSolidBlock(
                                                                    xTile,
                                                                    yTile + 1,
                                                                    zTile - 1);
                                                            bool south =
                                                                !isSolidBlock(
                                                                    xTile,
                                                                    yTile,
                                                                    zTile +
                                                                        1) &&
                                                                !isSolidBlock(
                                                                    xTile,
                                                                    yTile + 1,
                                                                    zTile + 1);

                                                            int dir = -1;
                                                            double closest =
                                                                9999;
                                                            if (west &&
                                                                xd < closest) {
                                                                closest = xd;
                                                                dir = 0;
                                                            }
                                                            if (east &&
                                                                1 - xd <
                                                                    closest) {
                                                                closest =
                                                                    1 - xd;
                                                                dir = 1;
                                                            }
                                                            if (north &&
                                                                zd < closest) {
                                                                closest = zd;
                                                                dir = 4;
                                                            }
                                                            if (south &&
                                                                1 - zd <
                                                                    closest) {
                                                                closest =
                                                                    1 - zd;
                                                                dir = 5;
                                                            }

                                                            float speed = 0.1f;
                                                            if (dir == 0)
                                                                this->xd =
                                                                    -speed;
                                                            if (dir == 1)
                                                                this->xd =
                                                                    +speed;
                                                            if (dir == 4)
                                                                this->zd =
                                                                    -speed;
                                                            if (dir == 5)
                                                                this->zd =
                                                                    +speed;
                                                        }

                                                        return false;

}

void LocalPlayer::setSprinting(bool value)
{
                                                        Player::setSprinting(
                                                            value);
                                                        if (value == false)
                                                            sprintTime = 0;
                                                        else
                                                            sprintTime =
                                                                SPRINT_DURATION;
}

void LocalPlayer::setExperienceValues(float experienceProgress, int totalExp, int experienceLevel)
{
                                                        this->experienceProgress =
                                                            experienceProgress;
                                                        this->totalExperien  // 4J: removed
                                                                             // void
                                                                             // LocalPlayer::sendMessage(ChatMessageComponent
                                                                             // *message)//{//
                                                                             // minecraft->gui->getChat()->addMessage(message.toString(true));//}

                                                            Pos LocalPlayer::
                                                                getCommandSenderWorldPosition() {
                                                            return new Pos(
                                                                floor(x + .5),
                                                                floor(y + .5),
                                                                floor(z + .5));
                                                        }

                                                        std::shared_ptr<
                                                            ItemInstance>
                                                        LocalPlayer::
                                                            getCarriedItem() {
                                                            return inventory
                                                                ->getSelected();
                                                        }

                                                        void
                                                        LocalPlayer::playSound(
                                                            int soundId,
                                                            float volume,
                                                            float pitch) {
                                                            level->playLocalSound(
                                                                x,
                                                                y - heightOffset,
                                                                z, soundId,
                                                                volume, pitch,
                                                                false);
                                                        }

                                                        bool LocalPlayer::
                                                            isRidingJumpable() {
                                                            return riding !=
                                                                       NULL &&
                                                                   riding->GetType() ==
                                                                       eTYPE_HORSE;
                                                        }

                                                        float LocalPlayer::
                                                            getJumpRidingScale() {
                                                            return jumpRidingScale;
                                                        }

                                                        void LocalPlayer::
                                                            sendRidingJump() {}

                                                        bool LocalPlayer::
                                                            hasPermission(
                                                                EGameCommand
                                                                    command) {
                                                            return level
                                                                ->getLevelData()
                                                                ->getAllowCommands();
                                                        }

                                                        void
                                                        LocalPlayer::onCrafted(
                                                            std::shared_ptr<
                                                                ItemInstance>
                                                                item) {
                                                            if (minecraft
                                                                    ->localgameModes
                                                                        [m_iPad] !=
                                                                NULL) {
                                                                TutorialMode* gameMode =
                                                                    (TutorialMode*)minecraft
                                                                        ->localgameModes
                                                                            [m_iPad];
                                                                gameMode
                                                                    ->getTutorial()
                                                                    ->onCrafted(
                                                                        item);
                                                            }
                                                        }

                                                        void LocalPlayer::
                                                            setAndBroadcastCustomSkin(
                                                                DWORD skinId) {
                                                            setCustomSkin(
                                                                skinId);
                                                        }

                                                        void LocalPlayer::
                                                            setAndBro  // 4J
                                                                       // TODO -
                                                                       // RemoveO#include
                                                                       // "..\Minecraft.World\LevelChunk.h"

                                                            void
                                                            LocalPlayer::mapPlayerChunk(
                                                                const unsigned int
                                                                    flagTileType) {
                                                            int cx =
                                                                this->xChunk;
                                                            int cz =
                                                                this->zChunk;

                                                            int pZ =
                                                                ((int)floor(
                                                                    t
                                                                    "player in "
                                                                    "chunk "
                                                                    "(" pX =
                                                                        ("," t)
                                                                            flo
                                                                        ") at "
                                                                        "(" >
                                                                        x)) %
                                                                16;

                                                            st "," cout
                                                                    << ","
                                                                       ")"
                                                                       "\n" < < <
                                                                cz < < < <
                                                                this->x < < < <
                                                                this->y < < < <
                                                                this->z << ;

                                                            for (int v = -1;
                                                                 v < 2; v++)
                                                                for (
                                                                    unsigned int
                                                                        z = 0;
                                                                    z < 16;
                                                                    z++) {
                                                                    for (int u =
                                                                             -1;
                                                                         u < 2;
                                                                         u++)
                                                                        for (
                                                                            unsigned int
                                                                                x = 0;
                                                                            x <
                                                                            16;
                                                                            x++) {
                                                                            LevelChunk* cc =
                                                                                level
                                                                                    ->getChunk(
                                                                                        cx +
                                                                                        u"O" z +
                                                                                        v);
                                                                            if (x ==
                                                                                    pX &&
                                                                                z ==
                                                                                    pZ &&
                                                                                u ==
                                                                                    0 &&
                                                                                v ==
                                                                                    0)
                                                                                std::cout
                                                                                    << ;
                                                                            else
                                                                                for (
                                                                                    unsigned int
                                                                                        y = 127;
                                                                                    y >
                                                                                    0;
                                                                                    y--) {
                                                                                    int t = cc->getTile(
                                                                                        "@",
                                                                                        z);
                                                                                    if (flagTileType !=
                                                                                            0 &&
                                                                                        t ==
                                                                                            flagTileType) {
                                                                                        std::cout
                                                                                            << ;
                                                                                        break;
                                                                                    } else if (
                                                                                        "#" =
                                                                                            0 &&
                                                                                            t < 10) {
                                                                                        std::cout
                                                                                            << t;
                                                                                        b "\n";
                                                                                    } else i"\n" > 0)			{
                                                                                            std::cout
                                                                                                << ;
                                                                                            break;
                                                                                        }
                                                                                }
                                                                        }
                                                                    std::cout
                                                                        <<  // 4J Stu - We should not accept any input while asleep, except the above to wake upl down)
                                                                    {
                                                                        if (isSleeping() &&
                                                                            level !=
                                                                                NULL &&
                                                                            level
                                                                                ->isClientSide) {
                                                                            return;
                                                                        }
                                                                        if (!down)
                                                                            missTime =
                                                                                0;
                                                                        if (button ==
                                                                                0 &&
                                                                            missTime >
                                                                                0)
                                                                            return;

                                                                        if (down &&
                                                                            minecraft
                                                                                    ->hitResult !=
                                                                                NULL &&
                                                                            minecraft
                                                                                    ->hitResult
                                                                                    ->type ==
                                                                                HitResult::
                                                                                    TILE &&
                                                                            button ==
                                                                                0) {
                                                                            int x =
                                                                                min  // 4J - addition to stop layer mining out of the top or bottom of the worldt->// 4J Stu - Allow this for The End

                                                                                if (((y ==
                                                                                      0) ||
                                                                                     ((y ==
                                                                                       127) &&
                                                                                      level
                                                                                          ->dimension
                                                                                          ->hasCeiling)) &&
                                                                                    level->dimension
                                                                                            ->id !=
                                                                                        1) return;

                                                                            minecraft
                                                                                ->gameMode
                                                                                ->continueDestroyBlock(
                                                                                    x,
                                                                                    y,
                                                                                    z,
                                                                                    minecraft
                                                                                        ->hitResult
                                                                                        ->f);

                                                                            if (mayDestroyBlockAt(
                                                                                    x,
                                                                                    y,
                                                                                    z)) {
                                                                                minecraft
                                                                                    ->particleEngine
                                                                                    ->crack(
                                                                                        x,
                                                                                        y,
                                                                                        z,
                                                                                        minecraft
                                                                                            ->hitResult
                                                                                            ->f);
                                                                                swing();
                                                                            }
                                                                        } else {
                                                                            minecraft
                                                                                ->gameMode
                                                                                ->stopDestroyBlock();
                                                                        }
                                                                    }

                                                                    bool
                                                                    LocalPlayer::creativeModeHandleMouseClick(
                                                                        int button,
                                                                        bool
                                                                            buttonPressed) {
                                                                        if  // Are we in an auto-repeat situation? - If so only tell the game that we've clicked if we move more than a unit away from our last// click position in any axis// If we're in disabled mode already (set when sprinting) then don't do anything - if we're sprinting, we don't auto-repeat at all.// With auto repeat on, we can quickly place fires causing photosensitivity issues due to rapid flashing// If we've started sprinting, go into this mode & also don't do anythinged )// Ignore repeate when sleeping

                                                                            // Get distance from last click point in each axisstClick_disabled;
                                                                            return false;
                                                                    }

                                                                    float dX =
                                                                        (float)
                                                                            x -
                                                                        lastClickX;
                                                                    float dY =
                                                                        (float)
                                                                            y -
                                                                        lastClickY;
                                                                    float dZ =
                                                                        (float)
                                                                            z -
                                                                        lastClickZ;
                                                                    bool newClick =
                                                                        false;

                                                                    float ddx =
                                                                        dX -
                                                                        lastClickdX;
                                                                    float ddy =
                                                                        dY -
                                                                        lastClickdY;
                                                                    float ddz =
                                                                        dZ -
                                                                        lastClickdZ;

                                                                    if (lastClickState ==
                                                                        lastClick_moving) {
                                                                        float deltaChange = sqrtf(
                                                                            ddx *
                                                                                ddx +
                                                                            ddy *
                                                                                ddy +
                                                                            ddz *
                                                                                ddz);
                                                                        if (deltaChange <
                                                                            0.01f) {
                                                                            lastClickState =
                                                                                lastClick_stopped;
                                                                            lastClickTolerance =
                                                                                0.0f;
                                                                        }
                                                                    } else if (
                                                                        lastClickState ==
                                                                        lastClick_stopped) {
                                                                        float deltaChange = sqrtf(
                                                                            ddx *
                                                                                ddx +
                                                                            ddy *
                                                                                ddy +
                                                                            ddz *
                                                                                ddz);
                                                                        if (deltaChange >=
                                                                            0.01f) {
                                                                            lastClickState =
                                                                                lastClick_moving;
                                                                            lastClickTolerance =
                                                                                0.0f;
                                                                        } else {
                                                                            lastClickTolerance +=
                                                                                0.1f;
                                                                            if (lastClickTolerance >
                                                                                0.7f) {
                                                                                lastClickTolerance =
                                                                                    0.0f;
                                                                                lastClickState =
                                                                                    lastCli  // If we have moved more than one unit in any one axis, then register a new clickkdZ // The new click position is normalised at one unit in the direction of movement, so that we don't gradually drift away if we detect the movement a fraction over// the unit distance each time

                                                                                    if (fabsf(
                                                                                            dX) >=
                                                                                        1.0f) {
                                                                                    dX =
                                                                                        (dX <
                                                                                         0.0f)
                                                                                            ? ceilf(
                                                                                                  dX)
                                                                                            : floorf(
                                                                                                  dX);
                                                                                    newClick =
                                                                                        true;
                                                                                }
                                                                                else if (
                                                                                    fabsf(
                                                                                        dY) >=
                                                                                    1.0f) {
                                                                                    dY =
                                                                                        (dY <
                                                                                         0.0f)
                                                                                            ? ceilf(
                                                                                                  dY)
                                                                                            : floorf(
                                                                                                  dY);
                                                                                    newClick =
                                                                                        true;
                                                                                }
                                                                                else if (
                                                                                    fabsf(
                                                                                        dZ) >=
                                                                                    1.0f) {
                                                                                    dZ =
                                                                                        (dZ <
                                                                                         0.0f)
                                                                                            ? ceilf(
                                                                                                  dZ)
                                                                                            : floorf(
                                                                                                  dZ);
                                                                                    newClick =
                                                                                        true;
                                                                                }

                                                                                if ((!newClick) &&
                                                                                    (lastClickTolerance >
                                                                                     0.0f)) {
                                                                                    float fTarget =
                                                                                        1.0f -
                                                                                        lastClickTolerance;

                                                                                    if (fabsf(
                                                                                            dX) >=
                                                                                        fTarget)
                                                                                        newClick =
                                                                                            true;
                                                                                    if (fabsf(
                                                                                            dY) >=
                                                                                        fTarget)
                                                                                        newClick =
                                                                                            true;
                                                                                    if (fabsf(
                                                                                            dZ) >=
                                                                                        fTarget)
                                                                                        newClick =
                                                                                            true;
                                                                                }
                                                                                // Get a more accurate pick from the position where the new click should ideally have come from, rather than// where we happen to be now (ie a rounded number of units from the last Click position)

                                                                                double oldX =
                                                                                    x;
                                                                                double oldY =
                                                                                    y;
                                                                                double oldZ =
                                                                                    z;
                                                                                x = lastClickX;
                                                                                y = lastClickY;
                                                                                z = lastClickZ;

                                                                                minecraft
                                                                                    ->gameRenderer
                                                                                    ->pick(
                                                                                        1);

                                                                                x = oldX;
                                                                                y = oldY;
                                                                                z = oldZ;

                                                                                handleMouseClick(
                                                                                    button);

                                                                                if (lastClickState ==
                                                                                    lastClick_stopped) {
                                                                                    lastClickState =
                                                                                        lastClick_init;
                                                                                    lastClickTolerance =
                                                                                        0.0f;
                                                                                } else {
                                                                                    l  // First click - just record position & handleckTolerance = 0.0f;
                                                                                }
                                                                            }
                                                                        }
                                                                        else {
                                                                            // If we actually placed an item, then move into the init state as we are going to be doing the special creative mode auto repeat// If we're sprinting or riding, don't auto-repeat at all. With auto repeat on, we can quickly place fires causing photosensitivity issues due to rapid flashing// Also ignore repeats when the player is sleeping

                                                                            if (isSprinting() ||
                                                                                isRiding() ||
                                                                                isSleeping()) {
                                                                                lastClickState =
                                                                                    lastClick_disabled;
                                                                            } else {
                                                                                if (itemPlaced) {
                                                                                    // Didn't place an item - might actually be activating a switch or door or something - just do a standard auto repeat in this case
                                                                                    lastClickState =
                                                                                        lastClick_oldRepeat;
                                                                                }
                                                                            }
                                                                            return true;
                                                                        }
                                                                    } else {
                                                                        lastClickState =
                                                                            lastClick_invalid;
                                                                    }
                                                                    return false;
                                                                }

                                                            bool LocalPlayer::
                                                                handleMouseClick(
                                                                    int button) {
                                                                bool
                                                                    retu  // app.DebugPrintf("handleMouseClick
                                                                          // -
                                                                          // Player
                                                                          // %d
                                                                          // is
                                                                          // swinging\n",GetXboxPad());utton
                                                                          // ==
                                                                          // 0)
                                                                {
                                                                    // 4J-PB -
                                                                    // Adding a
                                                                    // special
                                                                    // case in
                                                                    // here for
                                                                    // sleeping
                                                                    // in a bed
                                                                    // in a
                                                                    // multiplayer
                                                                    // game - we
                                                                    // need to
                                                                    // wake up,
                                                                    // and we
                                                                    // don't
                                                                    // have the
                                                                    // inbedchatscreen
                                                                    // with a
                                                                    // button

                                                                    if (button ==
                                                                            1 &&
                                                                        (isSleeping() &&
                                                                         level !=
                                                                             NULL &&
                                                                         level
                                                                             ->isClientSide)) {
                                                                        if (lastClickState ==
                                                                            lastClick_oldRepeat)
                                                                            return false;

                                                                        std::shared_ptr<
                                                                            MultiplayerLocalPlayer>
                                                                            mplp =
                                                                                dynamic_pointer_cast <
                                                                                Multipl  // 4J Stu - We should not accept any input while asleep, except the above to wake upeeping();
                                                                    }

                                                                    if (isSleeping() &&
                                                                        level !=
                                                                            NULL &&
                                                                        level
                                                                            ->isClientSide) {
                                                                        return false;
                                                                    }

                                                                    std::shared_ptr<
                                                                        ItemInstance>
                                                                        oldItem =
                                                                            inventory
                                                                                ->getSelected();

                                                                    if (minecraft
                                                                            ->hitResult ==
                                                                        NULL) {
                                                                        if (button ==
                                                                                0 &&
                                                                            minecraft
                                                                                ->localgameModes
                                                                                    [GetXboxPad()]
                                                                                ->hasMissTime())
                                                                            missTime =
                                                                                10;
                                                                    } else if (
                                                                        minecraft
                                                                            ->hitResult
                                                                            ->type ==
                                                                        HitResult::
                                                                            ENTITY) {
                                                                        if (button ==
                                                                            0) {
                                                                            minecraft
                                                                                ->gameMode
                                                                                ->at  // 4J-PB - if we milk a cow here, and end up with a bucket of milk, the if (mayUse && button == 1) further down will // then empty our bucket if we're pointing at a tile// It looks like interact really should be returning a result so we can check this, but it's possibly just the // milk bucket that causes a problem
                                                                            // If I have an empty bucket in my hand, it's going to be filled with milk, so turn off mayUse)
                                                                            {
                                                                                std::shared_ptr<
                                                                                    ItemInstance>
                                                                                    item =
                                                                                        inventory
                                                                                            ->getSelected();
                                                                                if (item &&
                                                                                    (item->id ==
                                                                                     Item::
                                                                                         bucket_empty_Id)) {
                                                                                    mayUse =
                                                                                        false;
                                                                                }
                                                                            }
                                                                            if (minecraft
                                                                                    ->gameMode
                                                                                    ->interact(
                                                                                        minecraft
                                                                                            ->localplayers
                                                                                                [GetXboxPad()],
                                                                                        minecraft
                                                                                            ->hitResult
                                                                                            ->entity)) {
                                                                                mayUse =
                                                                                    false;
                                                                            }
                                                                        }
                                                                    } else if (
                                                                        minecraft
                                                                            ->hitResult
                                                                            ->type ==
                                                                        HitResult::
                                                                            TILE) {
                                                                        int x =
                                                                            minecraft
                                                                                ->hitResult
                                                                                ->x;
                                                                        int y =
                                                                            minecraft
                                                                                ->hitR  // 4J - addition to stop layer mining out of the top or bottom of the worldult-// 4J Stu - Allow this for The End

                                                                            if (!((y ==
                                                                                   0) ||
                                                                                  ((y ==
                                                                                    127) &&
                                                                                   level
                                                                                       ->dimension
                                                                                       ->hasCeiling)) ||
                                                                                level->dimension
                                                                                        ->id ==
                                                                                    1) {
                                                                            minecraft
                                                                                ->gameMode
                                                                                ->startDestroyBlock(
                                                                                    x,
                                                                                    y,
                                                                                    z,
                                                                                    minecraft
                                                                                        ->hitResult
                                                                                        ->f);
                                                                        }
                                                                    } else {
                                                                        std::shared_ptr<
                                                                            ItemInstance>
                                                                            item =
                                                                                oldItem;
                                                                        int oldCount =
                                                                            item != NULL
                                                                                ? item->count
                                                                                : 0;
                                                                        bool usedItem =
                                                                            false;
                        if (minecraft->gameMode->useItemOn(minecra// Presume that if we actually used the held item, then we've placed ithitResult->pos, false, &usedItem))
			{
                                                                            // app.DebugPrintf("Player %d is swinging\n",GetXboxPad());		returnItemPlaced = true;
				}
				mayUse = false;
				
				swing();
                                                                    }
                                                                    if (item ==
                                                                        NULL) {
                                                                        return false;
                                                                    }

                                                                    if (item->count ==
                                                                        0) {
                                                                        inventory
                                                                            ->items
                                                                                [inventory
                                                                                     ->selected] =
                                                                            nullptr;
                                                                    } else if (
                                                                        item->count !=
                                                                            oldCount ||
                                                                        minecraft
                                                                            ->localgameModes
                                                                                [GetXboxPad()]
                                                                            ->hasInfiniteItems()) {
                                                                        minecraft
                                                                            ->gameRenderer
                                                                            ->itemInHandRenderer
                                                                            ->itemPlaced();
                                                                    }
                                                                }
                                                            }

                                                            if (mayUse &&
                                                                button == 1) {
                                                                std::shared_ptr<
                                                                    ItemInstance>
                                                                    item =
                                                                        inventory
                                                                            ->getSelected();
                                                                if (item !=
                                                                    NULL) {
                                                                    if (minecraft
                                                                            ->gameMode
                                                                            ->useItem(
                                                                                minecraft
                                                                                    ->localplayers
                                                                                        [GetXboxPad()],
                                                                                level,
                                                                                item)) {
                                                                        minecraft
                                                                            ->gameRenderer
                                                                            ->itemInHand /* && !ui.GetMenuDisplayed(m_iPad)*/
                                                                                eturn
                                                                                    returnItemPlaced;
                                                                    }

                                                                    void
                                                                    LocalPlayer::
                                                                        updateRichPresence() {
                                                                        if ((m_iPad !=
                                                                             -1)) {
                                                                            std::shared_ptr<
                                                                                ItemInstance>
                                                                                selectedItem =
                                                                                    inventory
                                                                                        ->getSelected();
                                                                            if (selectedItem !=
                                                                                    NULL &&
                                                                                selectedItem
                                                                                        ->id ==
                                                                                    Item::
                                                                                        fishingRod_Id) {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_FISHING);
                                                                            } else if (
                                                                                selectedItem !=
                                                                                    NULL &&
                                                                                selectedItem
                                                                                        ->id ==
                                                                                    Item::
                                                                                        map_Id) {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_MAP);
                                                                            } else if (
                                                                                (riding !=
                                                                                 NULL) &&
                                                                                riding
                                                                                    ->instanceof(
                                                                                        eTYPE_MINECART)) {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_RIDING_MINECART);
                                                                            } else if (
                                                                                (riding !=
                                                                                 NULL) &&
                                                                                riding
                                                                                    ->instanceof(
                                                                                        eTYPE_BOAT)) {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_BOATING);
                                                                            } else if (
                                                                                (riding !=
                                                                                 NULL) &&
                                                                                riding
                                                                                    ->instanceof(
                                                                                        eTYPE_PIG)) {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_RIDING_PIG);
                                                                            } else if (
                                                                                this->dimension ==
                                                                                -1) {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_NETHER);
                                                                            } else if (
                                                                                minecraft
                                                                                    ->soundEngine
                                                                                    ->GetIsPlayingStreamingCDMusic()) {
                                                                                app.SetRichPresenceCon  // 4J Stu - Added for telemetryCD);
                                                                            } else {
                                                                                app.SetRichPresenceContext(
                                                                                    m_iPad,
                                                                                    CONTEXT_GAME_STATE_BLANK);
                                                                            }
                                                                        }
                                                                    }

                                                                    void
                                                                    LocalPlayer::
                                                                        SetSessionTimerStart(
                                                                            void) {
                                                                        m_sessionTimeStart =
                                                                            app.getAppTime();
                                                                        m_dimensionTimeStart =
                                                                            m_sessionTimeStart;
                                                                    }

                                                                    float
                                                                    LocalPlayer::
                                                                        getSessionTimer(
                                                                            void) {
                                                                        return app.getAppTime() -
                                                                               m_sessionTimeStart;
                                                                    }

                                                                    float
                                                                    LocalPlayer::
                                                                        getAndResetChangeDimensionTimer() {
                                                                        float appTime =
                                                                            app.getAppTime();
                                                                        float returnVal =
                                                                            appTime -
                                                                            m_dimensionTimeStart;
                                                                        m_dimensionTimeStart =
                                                                            appTime;
                                                                        return returnVal;
                                                                    }

                                                                    void
                                                                    LocalPlayer::handleCollectItem(
                                                                        std::shared_ptr<
                                                                            ItemInstance>
                                                                            item) {
                                                                        if (item !=
                                                                            NULL) {
                                                                            unsigned int
                                                                                itemCountAnyAux =
                                                                                    0;
                                                                            unsigned int
                                                                                itemCou  // do they have the itemsigned int k = 0; k < inventory->items.length; ++k)
                                                                            {
                                                                                if (inventory
                                                                                        ->items
                                                                                            [k] !=
                                                                                    NULL) {
                                                                                    if (inventory
                                                                                            ->items
                                                                                                [k]
                                                                                            ->id ==
                                                                                        item->id) {
                                                                                        unsigned int quantity =
                                                                                            inventory
                                                                                                ->items
                                                                                                    [k]
                                                                                                ->GetCount();

                                                                                        itemCountAnyAux +=
                                                                                            quantity;

                                                                                        if (inventory
                                                                                                ->items
                                                                                                    [k]
                                                                                                ->getAuxValue() ==
                                                                                            item->getAuxValue()) {
                                                                                            itemCountThisAux +=
                                                                                                quantity;
                                                                                        }
                                                                                    }
                                                                                }
                                                                            }
                                                                            TutorialMode* gameMode =
                                                                                (TutorialMode*)minecraft
                                                                                    ->localgameModes
                                                                                        [m_iPad];
                                                                            gameMode
                                                                                ->getTutorial()
                                                                                ->onTake(
                                                                                    item,
                                                                                    itemCountAnyAux,
                                                                                    itemCountThisAux);
                                                                        }

                                                                        if (ui.IsContainerMenuDisplayed(
                                                                                m_iPad)) {
                                                                            ui.HandleInventoryUpdated(
                                                                                m_iPad);
                                                                        }
                                                                    }

                                                                    void
                                                                    LocalPlayer::SetPlayerAdditionalModelParts(
                                                                        std::vector<
                                                                            ModelPart*>
                                                                            pAdditionalModelParts) {
                                                                        m_pAdditionalModelParts =
                                                                            pAdditionalModelParts;
                                                                    }
