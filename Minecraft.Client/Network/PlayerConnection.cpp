#include "../Platform/stdafx.h"
#include "PlayerConnection.h"
#include "../Player/ServerPlayer.h"
#include "../Level/ServerLevel.h"
#include "../Player/ServerPlayerGameMode.h"
#include "PlayerList.h"
#include "../MinecraftServer.h"
#include "../../Minecraft.World/Headers/net.minecraft.commands.h"
#include "../../Minecraft.World/Headers/net.minecraft.network.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.entity.item.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.dimension.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.item.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.item.trading.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.inventory.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.tile.entity.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.level.saveddata.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.entity.animal.h"
#include "../../Minecraft.World/Headers/net.minecraft.network.h"
#include "../../Minecraft.World/Headers/net.minecraft.world.food.h"
#include "../../Minecraft.World/Util/AABB.h"
#include "../../Minecraft.World/Util/Pos.h"
#include "../../Minecraft.World/Util/SharedConstants.h"
#include "../../Minecraft.World/Network/Socket.h"
#include "../../Minecraft.World/Stats/Achievements.h"
#include "../../Minecraft.World/Headers/net.minecraft.h"
#include "../Player/EntityTracker.h"
#include "ServerConnection.h"
#include "../../Minecraft.World/Stats/GenericStats.h"
#include "../../Minecraft.World/Util/JavaMath.h"

// 4J Added
#include "../../Minecraft.World/Headers/net.minecraft.world.item.crafting.h"
#include "../GameState/Options.h"

Random PlayerConnection::random;

PlayerConnection::PlayerConnection(MinecraftServer* server,
                                   Connection* connection,
                                   std::shared_ptr<ServerPlayer> player) {
    // 4J - added initialisers
    done = false;
    tickCount = 0;
    aboveGroundTickCount = 0;
    xLastOk = yLastOk = zLastOk = 0;
    synched = true;
    didTick = false;
    lastKeepAliveId = 0;
    lastKeepAliveTime = 0;
    lastKeepAliveTick = 0;
    chatSpamTickCount = 0;
    dropSpamTickCount = 0;

    this->server = server;
    this->connection = connection;
    connection->setListener(this);
    this->player = player;
    //	player->connection = this;		// 4J - moved out as we can't
    // assign in a ctor
    InitializeCriticalSection(&done_cs);

    m_bCloseOnTick = false;
    m_bWasKicked = false;

    m_friendsOnlyUGC = false;
    m_offlineXUID = INVALID_XUID;
    m_onlineXUID = INVALID_XUID;
    m_bHasClientTickedOnce = false;

    setShowOnMaps(
        app.GetGameHostOption(eGameHostOption_Gamertags) != 0 ? true : false);
}

PlayerConnection::~PlayerConnection() {
    delete connection;
    DeleteCriticalSection(&done_cs);
}

void PlayerConnection::tick() {
    if (done) return;

    if (m_bCloseOnTick) {
        disconnect(DisconnectPacket::eDisconnect_Closed);
        return;
    }

    didTick = false;
    tickCount++;
    connection->tick();
    if (done) return;

    if ((tickCount - lastKeepAliveTick) > 20 * 1) {
        lastKeepAliveTick = tickCount;
        lastKeepAliveTime = System::nanoTime() / 1000000;
        lastKeepAliveId = random.nextInt();
        send(std::shared_ptr<KeepAlivePacket>(
            new KeepAlivePacket(lastKeepAliveId)));
    }

    if (chatSpamTickCount > 0) {
        chatSpamTickCount--;
    }
    if (dropSpamTickCount > 0) {
        dropSpamTickCount--;
    }
}

void PlayerConnection::disconnect(DisconnectPacket::eDisconnectReason reason) {
    EnterCriticalSection(&done_cs);
    if (done) {
        LeaveCriticalSection(&done_cs);
        return;
    }

        app.DebugPr"PlayerConnection disconect reason: %d\n", reason );
        player->disconnect(// 4J Stu - Need to remove the player from the receiving list before their socket is NULLed so that we can find another player on their system
	server->getPlayers()->removePlayerFromReceiving( player );
	send( std::shared_ptr<DisconnectPacket>( new DisconnectPacket(reason) ));
	connection->sendAn// 4J-PB - removed, since it needs to be localised in the language the client is in//server->players->broadcastAll( shared_ptr<ChatPacket>( new ChatPacket(L"�e" + player->name + L" left the game.") ) );
	if(getWasKicked())
	{
        server->getPlayers()->broadcastAll(
            std::shared_ptr<ChatPacket>(new ChatPacket(
                player->name, ChatPacket::e_ChatPlayerKickedFromGame)));
	}
	else
	{
        server->getPlayers()->broadcastAll(std::shared_ptr<ChatPacket>(
            new ChatPacket(player->name, ChatPacket::e_ChatPlayerLeftGame)));
	}

	server->getPlayers()->remove(player);
	done = true;
	LeaveCriticalSection(&done_cs);
}

void PlayerConnection::handlePlayerInput(
    std::shared_ptr<PlayerInputPacket> packet) {
    player->setPlayerInput(packet->getXxa(), packet->getYya(),
                           packet->isJumping(), packet->isSneaking());
}

void PlayerConnection::handleMovePlayer(
    std::shared_ptr<MovePlayerPacket> packet) {
    ServerLevel* level = server->getLevel(player->dimension);

    didTick = true;
    if (synched) m_bHasClientTickedOnce = true;

    if (player->wonGame) return;

    if (!synched) {
        double yDiff = packet->y - yLastOk;
        if (packet->x == xLastOk && yDiff * yDiff < 0.01 &&
            packet->z == zLastOk) {
            synched = true;
        }
    }

    if (synched) {
        if (player->riding != NULL) {
            float yRotT = player->yRot;
            float xRotT = player->xRot;
            player->riding->positionRider();
            double xt = player->x;
            double yt = player->y;
            double zt = player->z;

            if (packet->hasRot) {
                yRotT = packet->yRot;
                xRotT = packet->xRot;
            }

            player->onGround = packet->onGround;

            player->doTick(false);
            player->ySlideOffset = 0;
            player->absMoveTo(xt, yt, zt, yRotT, xRotT);
            if (player->riding != NULL) player->riding->positionRider();
            server
                ->ge  // player may have been kicked off the mount during the
                      // tick, so// only copy valid coordinates if the
                      // player still is "synched"
                if (synched) {
                xLastOk = player->x;
                yLastOk = player->y;
                zLastOk = player->z;
            }
            ((Level*)level)->tick(player);

            return;
        }

        if (player->isSleeping()) {
            player->doTick(false);
            player->absMoveTo(xLastOk, yLastOk, zLastOk, player->yRot,
                              player->xRot);
            ((Level*)level)->tick(player);
            return;
        }

        double startY = player->y;
        xLastOk = player->x;
        yLastOk = player->y;
        zLastOk = player->z;

        double xt = player->x;
        double yt = player->y;
        double zt = player->z;

        float yRotT = player->yRot;
        float xRotT = player->xRot;

        if (packet->hasPos && packet->y == -999 && packet->yView == -999) {
            packet->hasPos = false;
        }

        if (packet->hasPos) {
            xt = packet->x;
            yt = packet->y;
            zt = packet->z;
            double yd = packet->yView - packet->y;
            if (!player->isSleeping() && (yd > 1.65 || yd < 0.1)) {
                                disconnect(DisconnectPacket::eD//                logger.warning(player->name + " had an illegal stance: " + yd);
				return;
            }
            if (abs(packet->x) > 32000000 || abs(packet->z) > 32000000) {
                disconnect(DisconnectPacket::eDisconnect_IllegalPosition);
                return;
            }
        }
        if (packet->hasRot) {
            yRotT = packet->yRot;
            // 4J Stu Added to stop server player y pos being different than
            // client when flying
            if (player->abilities.mayfly || player->isAllowedToFly()) {
                player->abilities.flying = packet->isFlying;
            } else
                player->abilities.flying = false;

            player->doTick(false);
            player->ySlideOffset = 0;
            player->absMoveTo(xLastOk, yLastOk, zLastOk, yRotT, xRotT);

            if (!synched) return;

            double xDist = xt - player->x;
            double yDist = yt - player->y;
            double zDist = zt - player->z;

            double dist = xDist * xDist + yDis  // 4J-PB - removing this one for
                                                // now/*if (dist > 100.0f)
            {
                //            logger.warning(player->name + " moved too
                //            quickly!");
                disconnect(DisconnectPacket::eDisconnect_MovedTooQuickly);
                //                System.out.println("Moved too quickly at " +
                //                xt + ", " + yt + ", " + zt);
                //                teleport(player->x, player->y, player->z,
                //                player->yRot, player->xRot);
                return;
            }
            */

		float r = 1 / 16.0f;
            bool oldOk =
                level->getCubes(player, player->bb->copy()->shrink(r, r, r))
                    ->empty();

                if (player->onGround && !packet->// assume the player made a jump
			player->causeFoodExhaustion(FoodConstants::EXHAUSTION_JUMP);
        }

        player->  // 4J Stu - It is possible that we are no longer synched (eg
                  // By moving into an End Portal), so we should stop any
                  // further movement based on this packet// Fix for #87764 -
                  // Code: Gameplay: Host cannot move and experiences End World
                  // Chunks flickering, while in Splitscreen Mode// and Fix
                  // for #87788 - Code: Gameplay: Client cannot move and
                  // experiences End World Chunks flickering, while in
                  // Splitscreen Mode
            if (!synched) return;

        player
            ->o  // Since server players don't call travel we check food
                 // exhaustion// here
		
		player->checkMovementStatistiscs(xDist, yDist, zDist);

        double oyDist = yDist;

        xDist = xt - player->x;  // 4J-PB - line below will always be
                                 // true!
        if (yDist > -0.5 || yDist < 0.5) {
            yDist = 0;
        }
        zDist = zt - player->z;
        dist = xDist * xDist + yDist * yDist + zDist * zDist;
        bool fail = false;
                if (dist > 0.25 * 0.25 && !player->isSleeping() && !player->gameMode->isCreative() && !player->isAllowedTo//            logger.warning(player->name + " moved wrongly!");//            System.out.println("Got position " + xt + ", " + yt + ", " + zt);//            System.out.println("Expected " + player->x + ", " + player->y + ", " + player->z);#ifndef _CONTENT_PACKAGE
"%ls moved wrongly!\n"printf(L,player->name"Got position %f, %f, %f\n"tf(, "Expected %f, %f, %f\n"Printf(, play#endif player->y, player->z);

		
    
    }
    player->absMoveTo(xt, yt, zt, yRotT, xRotT);

    bool newOk =
        level->getCubes(player, player->bb->copy()->shrink(r, r, r))->empty();
    if (oldOk && (fail || !newOk) && !player->isSleeping()) {
        teleport(xLastOk, yLastOk, zLastOk, yRotT, xRotT);
        return;
    }
                AABB *testBox = player->bb->copy()->grow(r, // && server.level.getCubes(player, testBox).size() == 0
		if (!server->isFlightAllowed() && !player->gameMode->isCreative() && !level->containsAnyBlocks(testBox) && !player->isAllowedToFly() )
		{
        if (oyDist >= (-0.5f / 16.0f)) {
            aboveGroundTickCount++;
            if (aboveGro  //                    logger.warning(player->name + "
                          //                    was kicked for floating too
                          //                    long!");#ifndef
                          //                    _CONTENT_PACKAGE
"%ls was kicked for floating too "
          "long!\n"#endif player->name.c_str())
                ;
            
					disconnect(
                DisconnectPacket::eDisconnect_NoFlying);
            return;
        }
			}
}
else {
    aboveGroundTickCount = 0;
}

player->onGround = packet->onGround;
server->getPlayers()->move(player);
player->doCheckFallDamage(player->y - startY, packet->onGround);
}
else if ((tickCount % SharedConstants::TICKS_PER_SECOND) == 0) {
    teleport(xLastOk, yLastOk, zLastOk, player->yRot, player->xRot);
}
}

void PlayerConnection::teleport(double x, double y, double z,
                                float yRot /*= true*/ ot,
                                bool sendPacket ) {
    synched = false;
    xLastOk = x;
    yLastOk = y;
    zLastOk = z;
    player
        ->abs  // 4J - note that 1.62 is added to the height here as the client
               // connection that receives this will presume it represents y +
               // heightOffset at that end// This is different to the way that
               // height is sent back to the server, where it represents the
               // bottom of the player bounding
               // volume
        if (sendPacket) player->connection->send(
            std::shared_ptr<MovePlayerPacket>(new MovePlayerPacket::PosRot(
                x, y + 1.62f, y, z, yRot, xRot, false, false)));
}

void PlayerConnection::handlePlayerAction(
    std::shared_ptr<PlayerActionPacket> packet) {
    ServerLevel* level = server->getLevel(player->dimension);
    player->resetLastActionTime();

    if (packet->action == PlayerActionPacket::DROP_ITEM) {
        player->drop(false);
        return;
    } else if (packet->action == PlayerActionPacket::DROP_ALL_ITEMS) {
        player->drop(true);
        return;
    } else if (packet->action == PlayerActionPacket::RELEASE_USE_ITEM) {
        player->releaseUsingItem();
        return;
    }

    bool shouldVerifyLocation = false;
    if (packet->action == PlayerActionPacket::START_DESTROY_BLOCK)
        shouldVerifyLocation = true;
    if (packet->action == PlayerActionPacket::ABORT_DESTROY_BLOCK)
        shouldVerifyLocation = true;
    if (packet->action == PlayerActionPacket::STOP_DESTROY_BLOCK)
        shouldVerifyLocation = true;

    int x = packet->x;
    int y = packet->y;
    int z = packet->z;
    if (shouldVerifyLocation) {
        // there is a mismatch between the player's camera and the player's//
        // position, so add 1.5 blocks
        
		double yDist = player->y - (y + 0.5) + 1.5;
        double zDist = player->z - (z + 0.5);
        double dist = xDist * xDist + yDist * yDist + zDist * zDist;
        if (dist > 6 * 6) {
            return;
        }
        if (y >= server->getMaxBuildHeight()) {
            return;
        }
    }

    if (packet->action == PlayerActionPacket::START_DESTROY_BLOCK) {
        if (true)
            player->gameMode
                ->startDestr  // 4J - condition was
                              // !server->isUnderSpawnProtection(level, x, y, z,
                              // player) (from Java 1.6.4) but putting back to
                              // old
                              // behaviour
                else player->connection->send(std::shared_ptr<TileUpdatePacket>(
                    new TileUpdatePacket(x, y, z, level)));

    } else if (packet->action == PlayerActionPacket::STOP_DESTROY_BLOCK) {
        player->gameMode->stopDestroyBlock(x, y, z);
        server->getPlayers()
            ->prioritis  // 4J added - make sure that the update packets for
                         // this get prioritised over other general world
                         // updates
            if (level->getTile(x, y, z) != 0)
                player->connection->send(std::shared_ptr<TileUpdatePacket>(
                    new TileUpdatePacket(x, y, z, level)));
    } else if (packet->action == PlayerActionPacket::ABORT_DESTROY_BLOCK) {
        player->gameMode->abortDestroyBlock(x, y, z);
        if (level->getTile(x, y, z) != 0)
            player->connection->send(std::shared_ptr<TileUpdatePacket>(
                new TileUpdatePacket(x, y, z, level)));
    }
}

void PlayerConnection::handleUseItem(std::shared_ptr<UseItemPacket> packet) {
    ServerLevel* level = server->getLevel(player->dimension);
    std::shared_ptr<ItemInstance> item = player->inventory->getSelected();
    bool informClient = false;
    int x = packet->getX();
    int y = packet->getY();
    int z = packet->getZ();
    // 4J Stu - We don't have ops, so just use the levels setting;

        // = level->dimension->id != 0 || server->players->isOp(player->name);
        if (packet->getFace() == 255) {
        if (item == NULL) return;
        player->gameMode->useItem(player, level, item);
    }
    else if ((packet->getY() < server->getMaxBuildHeight() - 1) ||
             (packet->getFace() != Facing::UP &&
              packet->getY() < server->getMaxBuildHeight())) {
                if (synched && player->dis// 4J - condition was !server->isUnderSpawnProtection(level, x, y, z, player) (from java 1.6.4) but putting back to old behaviour
			{
                                player->gameMode->useItemOn(player, level, item, x, y, z, face, packet->getClickX(), packet->getClickY(), pack//player->connection->send(shared_ptr<ChatPacket>(new ChatPacket("\u00A77Height limit for building is " + server->maxBuildHeight)));
		informClient = true;
	}

	if (informClient)
	{
            player->connection->send(std::shared_ptr<TileUpdatePacket>(
                new TileUpdatePacket(x, y, z, level)));

            if (face == 0) y--;
            if (face == 1) y++;
            if (face == 2)
                z--;  // 4J - Fixes an issue where pistons briefly disappear
                      // when retracting. The pistons themselves shouldn't have
                      // their change from being pistonBase_Id to
                      // pistonMovingPiece_Id// directly sent to the client,
                      // as this will happen on the client as a result of it
                      // actioning (via a tile event) the retraction of the
                      // piston locally. However, by putting a switch//
                      // beside a piston and then performing an action on the
                      // side of it facing a piston, the following line of code
                      // will send a TileUpdatePacket containing the change to
                      // pistonMovingPiece_Id// to the client, and this
                      // packet is received before the piston retract action
                      // happens - when the piston retract then occurs, it
                      // doesn't work properly because the piston tile//
                      // isn't what it is
                      // expecting.
            
		if (level->getTile(x, y, z) != Tile::pistonMovingPiece_Id) {
                player->connection->send(std::shared_ptr<TileUpdatePacket>(
                    new TileUpdatePacket(x, y, z, level)));
            }

	}

	item = player->inventory->getSelected();

	bool forceClientUpdate = false;
	if(item != NULL && packet->getItem() == NULL)
	{
            forceClientUpdate = true;
	}
	if (item != NULL && item->count == 0)
	{
            player->inventory->items[player->inventory->selected] = nullptr;
            item = nullptr;
	}

	if (item == NULL || item->getUseDuration() == 0)
	{
            player->ignoreSlotUpdateHack = true;
            player->inventory->items[player->inventory->selected] =
                ItemInstance::clone(
                    player->inventory->items[player->inventory->selected]);
            Slot* s = player->containerMenu->getSlotFor(
                player->inventory, player->inventory->selected);
            player->containerMenu->broadcastChanges();
            player->ignoreSlotUpdateHack = false;

            if (forceClientUpdate ||
                !ItemInstance::matches(player->inventory->getSelected(),
                                       packet->getItem())) {
                send(std::shared_ptr<ContainerSetSlotPacket>(
                    new ContainerSetSlotPacket(
                        player->containerMenu->containerId, s->index,
                        player->inventory->getSelected())));
            }
	}
    }

void PlayerConnection::onDisconnect(DisconnectPacket::eDisconnectReason reaso//    logger.info(player.name + " lost connection: " + reason); d// 4J-PB - removed, since it needs to be localised in the language the client is in//server->players->broadcastAll( shared_ptr<ChatPacket>( new ChatPacket(L"�e" + player->name + L" left the game.") ) );
	if(getWasKicked())
	{
        server->getPlayers()->broadcastAll(
            std::shared_ptr<ChatPacket>(new ChatPacket(
                player->name, ChatPacket::e_ChatPlayerKickedFromGame)));
	}
	else
	{
        server->getPlayers()->broadcastAll(std::shared_ptr<ChatPacket>(
            new ChatPacket(player->name, ChatPacket::e_ChatPlayerLeftGame)));
	}
	server->getPlayers()->remove(player);
	done = true;
	LeaveCriticalSection(//    logger.warning(getClass() + " wasn't prepared to deal with a " + packet.getClass());)
{
        
	disconnect(DisconnectPacket::eDisconnect_UnexpectedPacket);
}

void PlayerConnection::sstd::end(std::shared_ptr<Packet> packet)
{
        if( connec// Check if we are allowed to send this packet typeayers()->canReceiveAllPackets( player ) )
		{
                //wprintf(L"Not the systems primary player, so not sending them a packet : %ls / %d\n", player->name.c_str(), packet->getId() );// 4J Added
                return;
			}
		}
		connection->send(packet);
}
}


void PlayerConnection::queueSstd::end(std::shared_ptr<Packet> packet)
{
        if( c// Check if we are allowed to send this packet typegetPlayers()->canReceiveAllPackets( player ) )
		{
            //wprintf(L"Not the systems primary player, so not queueing them a packet : %ls\n", connection->getSocket()->getPlayer()->GetGamertag() );
            return;
			}
}
connection->queueSend(packet);
}
}

void PlayerConnection::handleSetCarriedItem(std::shared_ptr//        logger.warning(player.name + " tried to set an invalid carried item");ory::getSelectionSize())
	{
    
		return;
	}
	player->inventory->selected = packet-// 4J - TODOe#if 0setLastActionTime();
}

void PlayerConnection::handleChat(std::shared_ptr<ChatPacket> packet) {
    
"Chat message too long" sage = packet->message;
    if (message.length() > SharedConstants::maxChatLength) {
        disconnect(L);
        return;
    }
    message = message.trim();
    for (int i = 0; i < message.length(); i++) {
        i "Illegal characters in chat" eLetters.indexOf(message.charAt(i)) <
                0 &&
            (in "/" message.charAt(i) < 32) {
            disconnect(L"<");
            "> " eturn;
        }
    }

    if (message.startsWith()) {
        handleCommand(message);
    } else {
        message =  + player.name +  + message;
        logger.info(message);
        server.players.broadcastAll(new ChatPacket(message));
    }
    chatSpamTickCount "disconnect.spam" ts::TI #endifR_SECOND;
        if (chatSpamTickCount > SharedConstants::TICKS_PER_SECOND * 10// 4J - TODOn#if 0);
}



}

void PlayerConnection::handleCom#endifostd::nst std::wstring& message)
{
    

	server.getCommandDispatcher()
        .performCommand(player, message);
    


}

void PlayerConnection::handleAnimate(std::shared_ptr<AnimatePacket> packet) {
    player->resetLastActionTime();
    if (packet->action == AnimatePacket::SWING) {
        player->swing();
    }
}

void PlayerConnection::handlePlayerCommand(
    std::shared_ptr<PlayerCommandPacket> packet) {
    player->resetLastActionTime();
    if (packet->action == PlayerCommandPacket::START_SNEAKING) {
        player->setSneaking(true);
    } else if (packet->action == PlayerCommandPacket::STOP_SNEAKING) {
        player->setSneaking(false);
    } else if (packet->action == PlayerCommandPacket::START_SPRINTING) {
        player->setSprinting(true);
    } else if (packet->action == PlayerCommandPacket::STOP_SPRINTING) {
        player->setSprinting(false);
    } else if (packet->action ==
               PlayerCommandPacket::STOP_SLEEPING) {  // currently only
                                                      // supported by
                                                      // horses...rue);
        synched = false;
    } else if (packet->action == PlayerCommandPacket::RIDING_JUMP) {
        
		if ((player->riding != NULL) &&
                    player->riding->GetType() == eTYPE_HORSE) {
            dynamic_pointer_cast <  // also only supported by
                                    // horses...yerJump(packet->data);
        }
    } else if (packet->action == PlayerCommandPacket::OPEN_INVENTORY) {
        
		if ((player->riding != NULL) &&
                    player->riding->instanceof(eTYPE_HORSE)) {
            dynamic_pointer_cast<EntityHorse>(player->riding)
                ->openInventory(player);
        }
    } else if (packet->action == PlayerCommandPacket::START_IDLEANIM) {
        player->setIsIdle(true);
    } else if (packet->action == PlayerCommandPacket::STOP_IDLEANIM) {
        player->setIsIdle(false);
    }
}

void PlayerConnection::
    setSh  // 4J Stu - Need to remove the player from the receiving list before
           // their socket is NULLed so that we can find another player on their
           // system
        server->getPlayers()
            ->removePlayerFromReceiving(player);
connection->close(DisconnectPacket::eDisconnect_Quitting);
}

int PlayerC  // 4J-PB - removed, since it needs to be localised in the language
             // the client is inid//send( shared_ptr<ChatPacket>( new
             // ChatPacket(L"�7" + string) )
             // );//
             // 4J-PB - removed, since it needs to be localised in the language
             // the client is in//send( shared_ptr<ChatPacket>( new
             // ChatPacket(L"�9" + string) std::) );{
	
	


}

std::wstring PlayerConnection::getConsoleName() { return player->getName(); }

void PlayerConnection::handleInteract(std::shared_ptr<InteractPack// Fix for #8218 - Gameplay: Attacking zombies from a different level often results in no hits being registeredl-// 4J Stu - If the client says that we hit something, then agree with it. The canSee can fail here as it checks// a ray from head->head, but we may actually be looking at a different part of the entity that can be seen// even though the ray is blocked.// && player->canSee(target) && player->distanceToSqr(target) < 6 * 6)//boole canSee = player->canSee(target);
	//double maxDist = 6 * 6;//if (!canSee)rge//{= N//	maxDist = 3 * 3;//}//if (player->distanceToSqr(target) < maxDist)
	{//{
    
		
		
		
		

		
		
		if (packet->action == InteractPacket::INTERACT) {
        player->interact(target);
    }
                else if (packet->action == InteractPacket//disconnect("Attempting to attack an invalid entity");TITY)//server.warn("Player " + player.getName() + " tried to attack an invalid entity");) || target == player)
			{
            //}
				
				return;
			}
			player// Both PlayerConnection and ClientConnection should handle this mostly the same way	return true;
}

void PlayerCon// Request for texture(#ifndef _CONTENT_PACKAGEacket> packe"Server received request for custom texture %ls\n"#endif

	if(packet->dwBytes==0)
	{
    

		wprintf(L,
                        packet->textureName.c_str());
    
		PBYTE pbData = NULL;
    DWORD dwBytes = 0;
    app.GetMemFileDetails(packet->textureName, &pbData, &dwBytes);

    if (dwBytes != 0) {
        se  // Response with texture datac#ifndef
            // _CONTENT_PACKAGEpacket->text"Server received custom texture
            // %ls\n"e
        {
            m_texturesRequested.pu #endifk(packet->textureName);
        }
    } else {
        

		wprintf(L,
                        packet->textureName.c_str());

		app.AddMemoryTextureFile(packet->textureName,packet->pbData,p// Both PlayerConnection and ClientConnection should handle this mostly the same way}
    }

    void PlayerConnection::handleText  // Request for texture and
                                       // geometryu#ifndef _CONTENT_PACKAGEket)
    {
        "Server received request for custom texture %ls\n"
#endifacket->dwTextureBytes==0)
	{
            

		wprintf(L,
                        packet->textureName.c_str());
            
		PBYTE pbData = NULL;
            DWORD dwTextureBytes = 0;
            app.GetMemFileDetails(packet->textureName, &pbData,
                                  &dwTextureBytes);
            DLCSkinFile* pDLCSkinFile =
                app.m_dlcManager.getSkinFile(packet->textureName);

            if (dwTextureBytes != 0) {
                if (pDLCSkinFile) {
                    if (pDLCSkinFile->getAdditionalBoxesCount() != 0) {
                                        send( std::shared_ptr<TextureAndGeometryPacket>( new TextureAndGeometryPacket(packet->textureName,pbData,dwTextureBytes// we don't have the dlc skin, so retrieve the data from the app storextureAndGeometryPacket>( new TextureAndGeometryPacket(packet->textureName,pbData,dwTextureBytes) ) );
                    }
                } else {
                    
				std::vector<SKIN_BOX*>* pvSkinBoxes =
                        app.GetAdditionalSkinBoxes(packet->dwSkinID);
                    unsigned int uiAnimOverrideBitmask =
                        app.GetAnimOverrideBitmask(packet->dwSkinID);

                                send( std::shared_ptr<TextureAndGeometryPacke// Response with texture and geometry datae#ifndef _CONTENT_PACKAGEreBytes,pvSk"Server received custom texture %ls and geometry\n"
		{
                        m_texturesRequested.pus #endif(packet->textureName);
		}
                }
                else {
                    
    // add the geometry to the app list#ifndef _CONTENT_PACKAGEt->textureNam"Adding skin boxes for skin id %X, box count %d\n"et->textureName,packet->pbData,pack#endifTextureBytes);

		
		if (packet->dwBoxC != 0) {
                            // Add the anim overriderintf(L,packet->dwSkinID,packet->dwBoxC);

			app.SetAdditionalSkinBoxes(
                            packet->dwSkinID, packet->BoxDataA, packet->dwBoxC);
                    }
                    
		app.SetAnimOverrideBitmask(
                        packet->dwSkinID,
                        packet
                            ->uiAnimOverrideBitmask);  // This sends the server
                                                       // received texture out
                                                       // to any other players
                                                       // waiting for the
                                                       // datametryReceived(packet->textureName);
                }
            }

            void PlayerConnection::handleTextureReceivestd::d(
                const std::wstring& textureName) {
                
	AUTO_VAR(it, std::find(m_texturesRequested.begin(),
                               m_texturesRequested.end(), textureName));
                if (it != m_texturesRequested.end()) {
                    PBYTE pbData = NULL;
                    DWORD dwBytes = 0;
                    app.GetMemFileDetails(textureName, &pbData, &dwBytes);

                    if (dwBytes != 0) {
                        // This sends the server received texture out to any
                        // other players waiting for the data;
                        m_texturesRequested.erase(it);
                    }
                }
            }

            void PlayerConnection::handleTextureAndGeometryReceivestd::d(
                const std::wstring& textureName) {
                
	AUTO_VAR(it, std::find(m_texturesRequested.begin(),
                               m_texturesRequested.end(), textureName));
                if (it != m_texturesRequested.end()) {
                    PBYTE pbData = NULL;
                    DWORD dwTextureBytes = 0;
                    app.GetMemFileDetails(textureName, &pbData,
                                          &dwTextureBytes);
                    DLCSkinFile* pDLCSkinFile =
                        app.m_dlcManager.getSkinFile(textureName);

                    if (
                        dwTextureByte  // get the data from the app &&
                                       // (pDLCSkinFile->getAdditionalBoxesCount()!=0))
                        {
                            send(std::shared_ptr<TextureAndGeometryPacket>(
                                new TextureAndGeometryPacket(
                                    textureName, pbData, dwTextureBytes,
                                    pDLCSkinFile)));
                        } else {
                            
				DWORD dwSkinID =
                                app.getSkinIdFromPath(textureName);
                            std::vector<SKIN_BOX*>* pvSkinBoxes =
                                app.GetAdditionalSkinBoxes(dwSkinID);
                            unsigned int uiAnimOverrideBitmask =
                                app.GetAnimOverrideBitmask(dwSkinID);

                            send(std::shared_ptr<TextureAndGeometryPacket>(
                                new TextureAndGeometryPacket(
                                    textureName, pbData, dwTextureBytes,
                                    pvSkinBoxes, uiAnimOverrideBitmask)));
                        } m_texturesRequested
                            .#ifndef _CONTENT_PACKAGEoid PlayerCo
                        "Skin for server player %ls has changed to %ls "
                        "(%d)\n" hangePacket > packet) {
                        switch (packet->action) {
        case TextureChangePacket::e_TextureChange#endif
		player->setCustomSkin( app.getSkinIdFromPath( packet->path ) );

		wprintf(L//player->customTextureUrl2 = packet->path;s#ifndef _CONTENT_PACKAGEureUrl.c_str"Cape for server player %ls has changed to %ls\n"eak;
	case TextureChangePacket::e_TextureChange_Cape:
            playe #endifCustomCape(Player::getCapeIdFromPath(packet->path));
                "def"

		wprintf(L, player->name.c_st#ifndef _CONTENT_PACKAGEreUrl2.c_str("Sending texture packet to get custom skin %ls from player %ls\n".substr(0,3).compare(L) != 0 && !app.IsFi#endifmoryTextures(packet->path))
	{
                                    if (server->connection
                                            ->addPendingTextureRequest(
                                                packet->path)) {

			wprintf(L// Update the ref count on the memory texture datar(), player->name.c_str());

			send(std::shared_ptr<TexturePacket>( new TexturePacket(packet->path,NULL,0) ) );
                                    }
	}
	else if(!packet->path.empty() && app.IsFileInMemoryTextures(packet->path))
	{
                                    
		app.AddMemoryTextureFile(packet->path, NULL, 0);
	}
	server->getPlayers()->broadcastAll( std::shared_ptr<TextureChangePack#ifndef _CONTENT_PACKAGEcket(player"PlayerConnection::handleTextureAndGeometryChange - Skin for server player %ls has changed to %ls (%d)\n"hange(std::shared_ptr<TextureAndGeometryChangePacket> packet)
{
                                    player->setCustomSkin(
                                        app #endifinIdFromPath(packet->path));
                                    
	wprintf("def",
                                        player->name.c_str(),
                                        p #ifndef _CONTENT_PACKAGEc_str(),
                                        play "Sending texture packet to get "
                                             "custom skin %ls from player "
                                             "%ls\n" &&
                                            packet->path.substr(0, 3).compare(
                                                L) !=
#endif !app
                                                    .IsFileInMemoryTextures(
                                                        packet->path)) {
                                        if (server->connection
                                                ->addPendingTextureRequest(
                                                    packet->path)) {

			wprintf(L// Update the ref count on the memory texture dataayer->name.c_str());

			send(std::shared_ptr<TextureAndGeometryPacket>( new TextureAndGeom// If we already have the texture, then we already have the model parts too&& //app.SetAdditionalSkinBoxes(packet->dwSkinID,)
		//DebugBreak();
		app.AddMemoryTextureFile(packet->path,NULL,0);

		player->setCustomSkin(packet->dwSkinID);

		
		
		
	
                                        
                                        }
        server->getPlayers()->broadcastAll( std::shared_ptr<TextureAndGeometryC// Need to check that this player has permission to change each individual setting?mension );
                                    }

                                    void PlayerConnection::
                                        handleServerSettingsstd::Changed(
                                            std::shared_ptr<
                                                ServerSettingsChangedPacket>
                                                packet) {
                                        if (packet->action ==
                                            ServerSettingsChangedPacket::
                                                HOST_IN_GAME_SETTINGS) {
                                            

		INetworkPlayer* networkPlayer = getNetworkPlayer();
                                            if ((networkPlayer != NULL &&
                                                 networkPlayer->IsHost()) ||
                                                player->isModerator()) {
                                                app.SetGameHostOption(
                                                    eGameHostOption_FireSpreads,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_FireSpreads));
                                                app.SetGameHostOption(
                                                    eGameHostOption_TNT,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_TNT));
                                                app.SetGameHostOption(
                                                    eGameHostOption_MobGriefing,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_MobGriefing));
                                                app.SetGameHostOption(
                                                    eGameHostOption_KeepInventory,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_KeepInventory));
                                                app.SetGameHostOption(
                                                    eGameHostOption_DoMobSpawning,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_DoMobSpawning));
                                                app.SetGameHostOption(
                                                    eGameHostOption_DoMobLoot,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_DoMobLoot));
                                                app.SetGameHostOption(
                                                    eGameHostOption_DoTileDrops,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_DoTileDrops));
                                                app.SetGameHostOption(
                                                    eGameHostOption_DoDaylightCycle,
                                                    app.GetGameHostOption(
                                                        packet->data,
                                                        eGameHostOption_DoDaylightCycle));
                        app.SetGameHostOption(eGameHostOption_NaturalRegeneration, app.GetGa// Update the QoS dataata, eGameHostOption_NaturalRegeneration));

			server->getPlayers()->broadcastAll( std::shared_ptr<ServerSettingsChangedPacket>( new ServerSettingsChangedPacket( ServerSettingsChangedPacket::HOST_IN_GAME_SETTINGS,app.GetGameHostOption(eGameHostOption_All) ) ) );

			
			g_NetworkManager.UpdateAndSetGameSessionData();
                                            }
                                        }
                                    }

                                    void PlayerConnection::handleKickPlayer(
                                        std::shared_ptr<KickPlayerPacket>
                                            packet) {
                                        INetworkPlayer* networkPlayer =
                                            getNetworkPlayer();
                                        if ((networkPlayer != NULL &&
                                             networkPlayer->IsHost()) ||
                                            player->isModerator()) {
                                            server->getPlayers()
                                                ->kickPlayerByShortId(
                                                    packet->m_networkSmallId);
                                        }
                                    }

                                    void PlayerConnection::handleGameCommand(
                                        std::shared_ptr<GameCommandPacket>
                                            packet) {
                                        MinecraftServer::getInstance()
                                            ->getCommandDispatcher()
                                            ->performCommand(player,
                                                             packet->command,
                                                             packet->data);
                                    }

                                    void
                                        PlayerConnection::  // else if
                                                            // (player.getLevel().getLevelData().isHardcore())pac//{)
                                    {  //	if (server.isSingleplayer() &&
                                       // player.name.equals(server.getSingleplayerName()))M_R//
                                       //{WN)//
                                       // player.connection.disconnect("You have
                                       // died. Game over, man, it's game
                                       // over!");aye//
                                       // server.selfDestruct();0:p//	}r->//
                                       // elseon,//	{e);//		BanEntry
                                       // ban = new BanEntry(player.name);//
                                       // ban.setReason("Death in
                                       // Hardcore");//
                                       // server.getPlayers().getBans().add(ban);//
                                       // player.connection.disconnect("You have
                                       // died. Game over, man, it's game
                                       // over!");//	}//}
                                        
		
		
		
		
		

		
		
		
		
		else {
#ifndef _CONTENT_PACKAGE> 0) return;
                                            player =
                                                server->getPlayers()->respawn(
                                                    player, 0, false);
                                        }
                                    }
}

void PlayerConnection::handleRespawn(std::shared_ptr<RespawnPacket> packet)
{
}

void PlayerConnection::handleContainerClose(std::shared_ptr<ContainerClosePacket> packet)
{
                                    player->doCloseContainer();
}

void PlayerConnection::handleContainerSetSlot(std::shared_ptr<ContainerSetSlotPacket> packet)
{
                                    if (packet->containerId ==
                                        AbstractContainerMenu::
                                            CONTAINER_ID_CARRIED) {
                                        player->inventory->setCarried(
                                            packet->item);
                                    } else {
                                        if (packet->containerId ==
                                                AbstractContainerMenu::
                                                    CONTAINER_ID_INVENTORY &&
                                            packet->slot >= 36 &&
                                            packet->slot < 36 + 9) {
                                            std::shared_ptr<ItemInstance>
                                                lastItem =
                                                    player->inventoryMenu
                                                        ->getSlot(packet->slot)
                                                        ->getItem();
                                            if (packet->item != NULL) {
                                                if (lastItem == NULL ||
                                                    lastItem->count <
                                                        packet->item->count) {
                                                    packet->item->popTime =
                                                        Inventory::
                                                            POP_TIME_DURATION;
                                                }
                                            }
                                            player->inventoryMenu->setItem(
                                                packet->slot, packet->item);
                                            player->ignoreSlotUpdateHack = true;
                                            player->containerMenu
                                                ->broadcastChanges();
                                            player->broadcastCarriedItem();
                                            player
                                                ->ignoreSlotUpdateH #endiffalse;
                                        } else if (packet->containerId ==
                                                   player->containerMenu
                                                       ->containerId) {
                                            player->containerMenu->setItem(
                                                packet->slot, packet->item);
                                            player->ignoreSlotUpdateHack = true;
                                            player->containerMenu
                                                ->broadcastChanges();
                                            player->broadcastCarriedItem();
                                            player->ignoreSlotUpdateHack =
                                                false;
                                        }
                                    }
}


void PlayerConnection::handleContainerClick(std::shared_ptr<ContainerClickPacket> packet)
{
                                    play  // Yep, you sure did click what you
                                          // claimed to click!->containerId ==
                                          // packet->containerId &&
                                          // player->containerMenu->isSynched(player))
                                    {
                                        std::shared_ptr<ItemInstance> clicked =
                                            player->containerMenu->clicked(
                                                packet->slotNum,
                                                packet->buttonNum,
                                                packet->clickType, player);

                                        if (ItemInstance::matches(packet->item,
                                                                  clicked)) {
                                                // No, you clicked the wrong thing!->connection->send( std::shared_ptr<ContainerAckPacket>( new ContainerAckPacket(packet->containerId, packet->uid, true) ) );
                                                player->ignoreSlotUpdateHack =
                                                true;
                                            player->containerMenu
                                                ->broadcastChanges();
                                            player->broadcastCarriedItem();
                                            player->ignoreSlotUpdateHack =
                                                false;
                                        } else {
                                            
			expectedAcks[player->containerMenu->containerId] =
                                                packet->uid;
                        player->connection->send( std::shared_ptr<ContainerAckPacket>( new ContainerAckPacket(packet->contain//                player.containerMenu.broadcastChanges();etSynched(player, false);

			std::vector<std::shared_ptr<ItemInstance> > items;
			for (unsigned int i = 0; i < player->containerMenu->slots.size(); i++)
			{
                                                items.push_back(
                                                    player->containerMenu->slots
                                                        .at(i)
                                                        ->getItem());
			}
			player->refreshContainer(player->containerMenu, &items);

			
		
                                        
                                        }
                                    }

}

void PlayerConnection::handleContainerButtonClick(std::shared_ptr<ContainerButtonClickPacket> packet)
{
                                    player->resetLastActionTime();
                                    if (player->containerMenu->containerId ==
                                            packet->containerId &&
                                        player->containerMenu->isSynched(
                                            player)) {
                player->containerMenu->clickMenuButton(player, pac#ifdef _LARGE_WORLDSayer->containerMenu->broadcastChanges();
                                    }
}

void PlayerConnection::handleSetCreativeModeSlot(std::shared_ptr<SetCreativeModeSlotPacket> packet)
{
                                    if (player->gameMode->isCreative()) {
                                        bool dr #elsepack  // 4J-PB - for Xbox
                                                           // maps, we'll centre
                                                           // them on the origin
                                                           // of the world,
                                                           // since we can fit
                                                           // the whole world in
                                                           // our mapd)
                                        {
                                            int mapScale = 3;
                                            #endif int scale =
                                                MapItemSavedData::MAP_SIZE * 2 *
                                                (1 << mapScale);
                                            int centreXC =
                                                (int)(Math::round(player->x /
                                                                  scale) *
                                                      scale);
                                            int centreZC =
                                                (int)(Math::round(player->z /
                                                                  scale) *
                                                      scale);
                                            
			    // 4J Stu - We only have one map per player per dimension, so don't reset the one that they have cen// when a new one is created 0;

			item->setAuxValue(player->"map_%d" etAuxValueForMap(
                                                player->getXuid(),
                                                player->dimension, centreXC,
                                                centreZC, mapScale));

                                            std::shared_ptr<MapItemSavedData>
                                                data = MapItem::getSavedData(
                                                    item->getAuxValue(),
                                                    player->level);
                                                // 4J-PB - for Xbox maps, we'll centre them on the origin of the world, since we can fit the whole world in our map, item->getAuxValue());
                                                std::wstring id =
                                                    std::wstring(buf);
                                            if (data == NULL) {
                                                data = std::shared_ptr<
                                                    MapItemSavedData>(
                                                    new MapItemSavedData(id));
                                            }
                                            player->level->setSavedData(
                                                id, (std::shared_ptr<SavedData>)
                                                        data);

                                            data->scale = mapScale;
                                            
			data->x = centreXC;
                                            data->z = centreZC;
                                            data->dimension =
                                                (uint8_t)player->level->dimension
                                                    ->id;
                                            data->setDirty();
                                        }

                                        bool validSlot =
                                            (packet->slotNum >=
                                                 InventoryMenu::
                                                     CRAFT_SLOT_START &&
                                             packet->slotNum <
                                                 (InventoryMenu::
                                                      USE_ROW_SLOT_START +
                                                  Inventory::
                                                      getSelectionSize()));
                bool validItem = item == NULL || (item->id < Item::items.length && item->id >= 0 && Item::items[item->i//                player.slotChanged(player.inventoryMenu, packet.slotNum, player.inventoryMenu.getSlot(packet.slotNum).getItem());t && validItem && validData)
		{
                                            if (item == NULL) {
                                                player->inventoryMenu->setItem(
                                                    packet->slotNum, nullptr);
                                            } else {
                                player->inventoryMenu->setItem(packet->slotNu// drop item		}
			player->inventoryMenu->setSynched(player, true);
			
		
                                            
                                            }
                                            // 4J Stu - Maps need to have their
                                            // aux value update, so the client
                                            // should always be assumed to be
                                            // wrongOND // This is how the Java
                                            // works, as the client also
                                            // incorrectly predicts the auxvalue
                                            // of the
                                            // mapItemhared_ptr<ItemEntity>
                                            // dropped = player->drop(item);
                                            if (dropped != NULL) {
                                                dropped->setShortLifeTime();
                                            }
			}
                                    }

                                    if (item != NULL &&
                                        item->id == Item::map_Id) {
                                        
			
			std::vector<std::shared_ptr<ItemInstance> >
                                            items;
                                        for (unsigned int i = 0;
                                             i < player->inventoryMenu->slots
                                                     .size();
                                             i++) {
                                            items.push_back(
                                                player->inventoryMenu->slots
                                                    .at(i)
                                                    ->getItem());
                                        }
                                        player->refreshContainer(
                                            player->inventoryMenu, &items);
                                    }
	}
                        }

                        void PlayerConnection::handleContainerAck(
                            std::shared_ptr<ContainerAckPacket> packet) {
        AUTO_VAR(it, e"PlayerConnection::handleSignUpdate\n"->containerId));

        if (it != expectedAcks.end() && packet->uid == it->second &&
            player->containerMenu->containerId == packet->containerId &&
            !player->containerMenu->isSynched(player)) {
            player->containerMenu->setSynched(player, true);
        }
                        }

                        void PlayerConnection::handleSignUpdate(
                            std::shared_ptr<SignUpdatePacket> packet) {
                            player->resetLastActionTime();
                            app.DebugPrintf();

        ServerLevel *leve"Player "r->getLevel(player->dime" just tried to change non-editable sign" packet->y, packet->z))
	{
            // 4J-JEV: Changed to allow characters to display as a [].cket->x,
            // packet->y, packet->z);

            if (dynamic_pointer_cast<SignTileEntity>(te) != NULL) {
                std::shared_ptr<SignTileEntity> ste =
                    dynamic_pointer_cast<SignTileEntity>(te);
                if (!ste->isEditable() ||
                    ste->getPlayerWhoMayEdit() != player) {
                    server->warn(L + player->getName() +
                                 L);
                    return;
                }
            }

            
		if (dynamic_pointer_cast<SignTileEntity>(te) != NULL) {
                int x = packet->x;
                int y = packet->y;
                int z = packet->z;
                std::shared_ptr<SignTileEntity> ste =
                    dynamic_pointer_cast<SignTileEntity>(te);
                for (int i = 0; i < 4; i++) {
                    std::wstring lineText = packet->lines[i].substr(0, 15);
                                ste->SetMessage( i, lineTex// Need to check that this player has permission to change each individual setting?ed(x, y, z);
                }
            }
        }

        void PlayerConnection::handleKeepAlive(
            std::shared_ptr<KeepAlivePacket> packet) {
            if (packet->id == lastKeepAliveId) {
                int time = (int) (System::nanoTime() // Find the player being edited
		player->latency = (player->latency * 3 + time) / 4;
            }
        }

        void PlayerConnection::handlePlayerInfo(
            std::shared_ptr<PlayerInfoPacket> packet) {
            

	INetworkPlayer* networkPlayer = getNetworkPlayer();
            if ((networkPlayer != NULL && networkPlayer->IsHost()) ||
                player->isModerator()) {
                std::shared_ptr<ServerPlayer> serverPlayer;
                
		for (AUTO_VAR(it, server->getPlayers()->players.begin());
                     it != server->getPlayers()->players.end(); ++it) {
                    std::shared_ptr<ServerPlayer> checkingPlayer = *it;
                    if (checkingPlayer->connection->getNetworkPlayer() !=
                            NULL &&
                        checkingPlayer->connection->getNetworkPlayer()
                                ->GetSmallId() == packet->m_networkSmallId) {
                        serverPlayer = checkingPlayer;
                        break;
                    }
                }

                if (serverPlayer != NULL) {
                    unsigned int origPrivs =
                        serverPlayer->getAllPlayerGamePrivileges();

                    bool trustPlayers = app.GetGameHostOption(
                                            eG #ifndef _CONTENT_PACKAGErs) != 0;
                    bo "Setting %ls to game mode %d\n" on(
                        eGameHostOption_CheatsEnabled) != 0;
#endiferverPlayer == player)
                    {
                        GameType* gameType =
                            Player::getPlayerGamePrivilege(
                                packet->m_playerPrivileges,
                                Player::ePlayerGamePrivilege_CreativeMode)
                                ? GameType::CREATIVE
                                : GameType::SURVIVAL;
                        gameType =
                            LevelSettings::validateGameType(gameType->getId());
                        if (serverPlayer->gameMode->getGameModeForPlayer() !=
                            gameType) {
                            
					wprintf(
                                L,
                                serve #ifndef _CONTENT_PACKAGEameType);
"%ls already has game mode %d\n"mePrivilege(Player::ePlayerGamePrivilege_#endifveMode,Player::getPlayerGamePrivi// Editing selfplayerPrivileges,Player::ePlayerGamePrivilege_CreativeMode) );
					serverPlayer->gameMode->setGameModeForPlayer(gameType);
					serverPlayer->connection->send( std::shared_ptr<GameEventPacket>( new GameEventPacket(GameEventPacket::CHANGE_GAME_MODE, gameType->getId()) ));
                        } else {
                            
					wprintf(
                                L,
                                serverPlayer->name.c_str(), gameType);
                            
				
                        
                        }
                        if (cheats) {
                            
					bool canBeInvisible =
                                Player::getPlayerGamePrivilege(
                                    origPrivs,
                                    Player::
                                        ePlayerGamePrivilege_CanToggleInvisible) !=
                                0;
                            if (canBeInvisible)
                                serverPlayer->setPlayerGamePrivilege(
                                    Player::ePlayerGamePrivilege_Invisible,
                                    Player::getPlayerGamePrivilege(
                                        packet->m_playerPrivileges,
                                        Player::
                                            ePlayerGamePrivilege_Invisible));
                            if (canBeInvisible)
                                serverPlayer->setPlayerGamePrivilege(
                                    Player::ePlayerGamePrivilege_Invulnerable,
                                    Player::getPlayerGamePrivilege(
                                        packet->m_playerPrivileges,
                                        Player::
                                            ePlayerGamePrivilege_Invulnerable));

                            bool inCreativeMode =
                                Player::getPlayerGamePrivilege(
                                    origPrivs,
                                    Player::
                                        ePlayerGamePrivilege_CreativeMode) != 0;
                            if (!inCreativeMode) {
                                bool canFly = Player::getPlayerGamePrivilege(
                                    origPrivs,
                                    Player::ePlayerGamePrivilege_CanToggleFly);
                                                bool canChangeHunger = Player::getPlayerGamePrivilege(origPrivs,Player::ePlayerGamePrivileg// Editing someone else				
						if(canFly)serverPlayer->setPlayerGamePrivilege(Player::ePlayerGamePrivilege_CanFly,Player::getPlayerGamePrivilege(packet->m_playerPrivileges,Player::ePlayerGamePrivilege_CanFly) );
						if(canChangeHunger)serverPlayer->setPlayerGamePrivilege(Player::ePlayerGamePrivilege_ClassicHunger,Player::getPlayerGamePrivilege(packet->m_playerPrivileges,Player::ePlayerGamePrivilege_ClassicHunger) );
                            }
                        }
                    }
                    else {
                        
				if (!trustPlayers && !serverPlayer->connection
                                                          ->getNetworkPlayer()
                                                          ->IsHost()) {
                            serverPlayer->setPlayerGamePrivilege(
                                Player::ePlayerGamePrivilege_CannotMine,
                                Player::getPlayerGamePrivilege(
                                    packet->m_playerPrivileges,
                                    Player::ePlayerGamePrivilege_CannotMine));
                            serverPlayer->setPlayerGamePrivilege(
                                Player::ePlayerGamePrivilege_CannotBuild,
                                Player::getPlayerGamePrivilege(
                                    packet->m_playerPrivileges,
                                    Player::ePlayerGamePrivilege_CannotBuild));
                            serverPlayer->setPlayerGamePrivilege(
                                Player::
                                    ePlayerGamePrivilege_CannotAttackPlayers,
                                Player::getPlayerGamePrivilege(
                                    packet->m_playerPrivileges,
                                    Player::
                                        ePlayerGamePrivilege_CannotAttackPlayers));
                            serverPlayer->setPlayerGamePrivilege(
                                Player::
                                    ePlayerGamePrivilege_CannotAttackAnimals,
                                Player::getPlayerGamePrivilege(
                                    packet->m_playerPrivileges,
                                    Player::
                                        ePlayerGamePrivilege_CannotAttackAnimals));
                            serverPlayer->setPlayerGamePrivilege(
                                Player::
                                    ePlayerGamePrivilege_CanUseDoorsAndSwitches,
                                Player::getPlayerGamePrivilege(
                                    packet->m_playerPrivileges,
                                    Player::
                                        ePlayerGamePrivilege_CanUseDoorsAndSwitches));
                            serverPlayer->setPlayerGamePrivilege(
                                Player::ePlayerGamePrivilege_CanUseContainers,
                                Player::getPlayerGamePrivilege(
                                    packet->m_playerPrivileges,
                                    Player::
                                        ePlayerGamePrivilege_CanUseContainers));
                        }

                        if (networkPlayer->IsHost()) {
                            if (cheats) {
                                serverPlayer->setPlayerGamePrivilege(
                                    Player::
                                        ePlayerGamePrivilege_CanToggleInvisible,
                                    Player::getPlayerGamePrivilege(
                                        packet->m_playerPrivileges,
                                        Player::
                                            ePlayerGamePrivilege_CanToggleInvisible));
                                serverPlayer->setPlayerGamePrivilege(
                                    Player::ePlayerGamePrivilege_CanToggleFly,
                                    Player::getPlayerGamePrivilege(
                                        packet->m_playerPrivileges,
                                        Player::
                                            ePlayerGamePrivilege_CanToggleFly));
                                serverPlayer->setPlayerGamePrivilege(
                                    Player::
                                        ePlayerGamePrivilege_CanToggleClassicHunger,
                                    Player::getPlayerGamePrivilege(
                                        packet->m_playerPrivileges,
                                        Player::
                                            ePlayerGamePrivilege_CanToggleClassicHunger));
                                serverPlayer->setPlayerGamePrivilege(
                                    Player::ePlayerGamePrivilege_CanTeleport,
                                    Player::getPlayerGamePrivilege(
                                        packet->m_playerPrivileges,
                                        Player::
                                            ePlayerGamePrivilege_CanTeleport));
                            }
                                        serverPlayer->setPlayerGamePrivilege(Player::ePlayerGamePrivilege_Op,Player::getPlayerGamePrivilege(p//void handleChatAutoComplete(ChatAutoCompletePacket packet) {	//	StringBuilder result = new StringBuilder();ll//	for (String candidate : server.getAutoCompletions(player, packet.getMessage())) {}//		if (result.length() > 0) result.append("\0");()//		result.append(candidate);e//	}ne//	player.connection.send(new ChatAutoCompletePacket(result.toString()));r//}li//void handleClientInformation(shared_ptr<ClientInformationPacket> packet))//{ //	player->updateOptions(packet);//}
#if 0










"Invalid book tag!"// make sure the sent item is the currently carried itemhared_ptr<CustomPayloadPacket> customPayloadPacket)
{
                                                    
	if (CustomPayloadPacket.CUSTOM_BOOK_PACKET.equals(
                customPayloadPacket.identifier)) {
                                                        ByteArrayInputStream bais(
                                                            customPayloadPacket
                                                                ->data);
                                                        DataInputStream input(
                                                            &bais);
                                                        std::shared_ptr<
                                                            ItemInstance>
                                                            sentItem = Packet::
                                                                readItem(input);

                                                        if (!WritingBookItem
                                                                 .makeSureTagIsValid(
                                                                     sentItem
                                                                         .getTag())) {
                                                            throw new IOException();
                                                        }

                                                        
		ItemInstance carried = player.inventory.getSelected();
                                                        if (sentItem != null &&
                                                            sentItem.id ==
                                                                Item.writingBook
                                                                    .id &&
                                                            sen
                                                            "Invalid book tag!"
                                                                .id) {
                                                            // make sure the
                                                            // sent item is the
                                                            // currently carried
                                                            // itemtem.getTag().getList(WrittenBookItem.TAG_PAGES));
                                                        }
                                                    }
                                                    else if (
                                                        CustomPayloadPacket
                                                            .CUSTOM_BOOK_SIGN_PACKET
                                                            .equals(
                                                                customPayloadPacket
                                                                    .identifier)) {
                                                        DataInputStream input =
                                                            new DataInputStream(
                                                                new ByteArrayInputStream(
                                                                    customPayloadPacket
                                                                        .data));
                                                        ItemInstance sentItem =
                                                            Packet.readItem(
                                                                input);

                                                        if (!WrittenBookItem
                                                                 .makeSureTagIsValid(
                                                                     sentItem
                                                                         .getTag())) {
                                                            throw new IOException();
                                                        }

                                                        
		ItemInstance carried = player.inventory.getSelected();
                                                        if (sentItem != null &&
                                                            sentItem.id ==
                                                                Item.#endifnBook
                                                                    .id &&
                                                            carried.id ==
                                                                Item.writingBook
                                                                    .id) {
                                                            carried.addTagElement(
                                                                WrittenBookItem
                                                                    .TAG_AUTHOR,
                                                                new StringTag(
                                                                    WrittenBookItem
                                                                        .TAG_AUTHOR,
                                                                    player
                                                                        .getName()));
                                                            carried.addTagElement(
                                                                WrittenBookItem
                                                                    .TAG_TITLE,
                                                                new StringTag(
                                                                    WrittenBookItem
                                                                        .TAG_TITLE,
                                                                    sentItem
                                                                        .getTag()
                                                                        .getString(
                                                                            WrittenBookItem
                                                                                .TAG_TITLE)));
                                                            carried.addTagElement(
                                                                WrittenBookItem
                                                                    .TAG_PAGES,
                                                                sentItem
                                                                    .getTag()
                                                                    .getList(
                                                                        WrittenBookItem
                                                                            .TAG_PAGES));
                                                            carried.id =
                                                                Item.writtenBook
                                                                    .id;
                                                        }
                                                    }
        else

		if (CustomPayloadPacket::TRADER_SELECTION_PACKET.compare(customPayloadPacket->identifier) == 0)
"Command blocks not enabled"bais(cu//player->sendMessage(ChatMessageComponent.forTranslation("advMode.notEnabled"));ut.readInt();

			AbstractContainerMenu *menu = player->containerMenu;
			if (dynamic_cast<MerchantMenu *>(menu))
			{
                                                        ((MerchantMenu*)menu)
                                                            ->setSelectionHint(
                                                                selection);
			}
		}
		else if (CustomPayloadPacket::SET_ADVENTURE_COMMAND_PACKET.compare(customPayloadPacket->identifier) == 0)
		{
                                                    if (!server
                                                             ->isCommandBlockEnabled()) {
                                                        app.DebugPrintf();
                                                        
			
                                                    
                                                    } else if (
                                                        player->hasPermission(
                                                            eGameCommand_Effect) &&
                                                        player->abilities
                                                            .instabuild) {
                                                        ByteArrayInputStream bais(
                                                            customPayloadPacket
                                                                ->data);  // player->sendMessage(ChatMessageComponent.forTranslation("advMode.setCommand.success",
                                                                          // command));nt
                                                                          // z =
                                                                          // input.readInt();
                                                        s  // player.sendMessage(ChatMessageComponent.forTranslation("advMode.notAllowed"));ity>
                                                           // tileEntity =
                                                           // player->level->getTileEntity(x,
                                                           // y, z);
                                                            std::shared_ptr<
                                                                CommandBlockEntity>
                                                                cbe = dynamic_pointer_cast<
                                                                    CommandBlockEntity>(
                                                                    tileEntity);
                                                        if (tileEntity !=
                                                                NULL &&
                                                            cbe != NULL) {
                                                            cbe->setCommand(
                                                                command);
                                                            player->level
                                                                ->sendTileUpdated(
                                                                    x, y, z);
                                                            
				
                                                        
                                                        }
                                                    } else {
                                                        
			
                                                    
                                                    }
		}
		else if (CustomPayloadPacket::SET_BEACON_PACKET.compare(customPayloadPacket->identifier) == 0)
		{
                                                    if (dynamic_cast<
                                                            BeaconMenu*>(
                                                            player
                                                                ->containerMenu) !=
                                                        NULL) {
                                                        ByteArrayInputStream bais(
                                                            customPayloadPacket
                                                                ->data);
                                                        DataInputStream input(
                                                            &bais);
                                                        int primary =
                                                            input.readInt();
                                                        int secondary =
                                                            input.readInt();

                                                        BeaconMenu* beaconMenu =
                                                            (BeaconMenu*)player
                                                                ->containerMenu;
                                                        Slot* slot =
                                                            beaconMenu->getSlot(
                                                                0);
                                                        if (slot->hasItem()) {
                                                            slot->remove(1);
                                                        std:
                                                            "" hared_ptr<
                                                                BeaconTileEntity>
                                                                beacon =
                                                                    beaconMenu
                                                                        ->getBeacon();
                                                            beacon
                                                                ->setPrimaryPower(
                                                                    primary);
                                                            beacon
                                                                ->setSecondaryPower(
                                                                    secondary);
                                                            beacon
                                                                ->setChanged();
                                                        }
                                                    }
		}
		else if (CustomPayloadPacket::SET_ITEM_NAME_PACKET.compare(customPayloadPacket->identifier) == 0)
		{
                                                    AnvilMenu  // 4J
                                                               // Addedamic_cast<AnvilMenu
                                                               // *>(
                                                               // player->containerMenu);
                                                        if (menu) {
                                if (customPayloa//Player player = dynamic_pointer_cast<Player>( player->shared_from_this() );		menu->setItemName(L);
                                                    }
                                                    else {
                                                        ByteArrayInputStream bais(
                                                            customPayloadPacket
                                                                ->data);
                                                        DataInputStream dis(
                                                            &bais);
                                                        std::wstring name =
                                                            dis.readUTF();
                                                        if (name.length() <=
                                                            30) {
                                                            menu->setItemName(
                                                                name);
                                                        }
                                                    }
			}
                        }
                    }

                    bool PlayerConnection::isDisconnected() { return done; }

                    

void PlayerConnection::handleDebugOptions(
                        std::shared_ptr<DebugOptionsPacket> packet) {
                        
	player->SetDebugOptions(packet->m_uiVal);
                    }

                    void PlayerConnection::handleCraftItem(
                        std::shared_ptr<CraftItemPacket> packet) {
                        int iRecipe = packet->recipe;

        if(iRecipe =// no room in inventory, so throw it downUIRED *pRecipeIngredientsRequired=Recipes::getInstance()->getRecipeIngredientsArray();
	std::shared_ptr<ItemInstance> pTempItemInst=pRecipeIngredientsRequired[iRecipe].pRecipy->assemble(nullptr);

	if(app.DebugSettingsOn() && (player->GetDebugOptions()&(1L<<eDebugSetting_CraftAnything)))
// TODO 4J Stu - Assume at the moment that the client can work this out for us...er-//if(pRecipeIngredientsRequired[iRecipe].bCanMake) (pl//{r->inventory->add(pTempItemInst)==false )
		{
                                                
			player->drop(pTempItemInst);
		}
                    }
                    e  // and remove those resources from your inventorye_Id ||
                       // pTempItemInst->id == Item::fireworks_Id)
                    {
                        CraftingMenu* menu =
                            (CraftingMenu*)player->containerMenu;
                        player->openFireworks(menu->getX(), menu->getY(),
                                              menu->getZ());
                    }
                    else {
                            // do we need to remove a specific aux value?
		
		
		pTempItemInst->onCraftedBy(player->level,
                                           dynamic_pointer_cast<Player>(
                                               player->shared_from_this()),
                                           pTempItemInst->count);

                        
		for (int i = 0; i < pRecipeIngredientsRequired[iRecipe].iIngC;
                     i++) {
                            for (int j = 0;
                                 j < pRecipeIngredientsRequired[iRecipe]
                                         .iIngValA[i];
                                 j++) {
                                std::shared_ptr<ItemInstance> ingItemInst =
                                    nullptr;
                                
				if (pRecipeIngredientsRequired[iRecipe]
                                        .iIngAuxValA[i] !=
                                    Recipes::ANY_AUX_VALUE)
                                    // 4J Stu - Fix for #13097 - Bug: Milk
                                    // Buckets are removed when crafting
                                    // CakesRequired[iRecipe].iIngIDA[i],pRecipeIngredientsRequired[iRecipe].iIngAuxValA[i]
                                    // );
                                    player->inventory
                                        ->re  // replace item with remaining
                                              // resulted[iRecipe].iIngIDA[i],pRecipeIngredientsRequired[iRecipe].iIngAuxValA[i]);
                            }
                            else {
                                        ingItemInst = player->inventory->getResourceItem( pReci// 4J Stu - Fix for #13119 - We should add the item after we remove the ingredients(pRecipeIngredientsRequired[iRecipe].iIngIDA[i]);
                            }

                            // no room in inventory, so throw it
                            // down
                            if (ingItemInst != NULL) {
                                        if (ingIt// 4J Stu - Maps need to have their aux value update, so the client should always be assumed to be wrongplay// This is how the Java works, as the client also incorrectly predicts the auxvalue of the mapItemftingRemainingItem()) ) );
                            }
                        }
                    }
                }

                
		if (player->inventory->add(pTempItemInst) == false) {
                    
			player->drop(pTempItemInst);
                }

                // Do same hack as PlayerConnection::handleContainerClick does -
                // do our broadcast of changes just now, but with a hack so it
                // just thinks it has sent// things but hasn't really. This
                // will stop the client getting a message back confirming the
                // current inventory items, which might then arrivestan// after
                // another local change has been made on the client and be
                // stale.ts.size(); i++)
                {
                    items.push_back(
                        player->containerMenu->slots.at(i)->getItem());
                }
                player->refreshContainer(player->containerMenu, &items);
            } else {
                // handle
                // achievements
                
			
			player->ignoreSlotUpdateHack = true;
                player->containerMenu->broadcastChanges();
                player->broadcastCarriedItem();
                player->ignoreSlotUpdateHack = false;
            }
        }

        
	switch (pTempItemInst->id) {
            case Tile::workBench_Id:
                player->awardStat(GenericStats::buildWorkbench(),
                                  GenericStats::param_buildWorkbench());
                break;
            case Item::pickAxe_wood_Id:
                player->awardStat(GenericStats::buildPickaxe(),
                                  GenericStats::param_buildPickaxe());
                break;
            case Tile::furnace_Id:
                player->awardStat(GenericStats::buildFurnace(),
                                  GenericStats::param_buildFurnace());
                break;
            case Item::hoe_wood_Id:
                player->awardStat(GenericStats::buildHoe(),
                                  GenericStats::param_buildHoe());
                break;
            case Item::bread_Id:
                player->awardStat(GenericStats::makeBread(),
                                  GenericStats::param_makeBread());
                break;
            case Item::cake_Id:
                player->awardStat(GenericStats::bakeCake(),
                                  GenericStats::param_bakeCake());
                break;
            case Item::pickAxe_stone_Id:	player->awardStat(GenericStats::buildBetterPickaxe(),	Gen//}cS// ELSE The server thinks the client was wrong...tem::sword_wood_Id:		player->awardStat(GenericStats::buildSword(),			GenericStats::param_buildSword());			break;
	case Tile::dispenser_Id:		player->awardStat(GenericStats::dispenseWithThis(),		GenericStats::param_dispenseWithThis());	break;
	case Tile::enchantTable_Id:		player->awardStat(GenericStats::enchantments(),			GenericStats::param_enchantments());		break;
	case Tile::bookshelf_Id:		player->awardStat(GenericStats::bookcase(),				GenericStats::param_bookcase());			break;
        }
        
	    // Do we have the ingredients?
                        }

                        void PlayerConnection::handleTradeItem(
                            std::shared_ptr<TradeItemPacket> packet) {
                            if (player->containerMenu->containerId ==
                                packet->containerId) {
                                MerchantMenu* menu =
                                    (MerchantMenu*)player->containerMenu;

                                MerchantRecipeList* offers =
                                    menu->getMerchant()->getOffers(player);

                                if (offers) {
                                    int selectedShopItem = packet->offer;
                                    if (selectedShopItem < offers->size()) {
                                MerchantRecipe *activeRecipe = offers->at(selectedShopIte// Remove the items we are purchasing with				{
					
					std::shared_ptr<ItemInstance> buyAItem = activeRecipe->getBuyAIte// Add the item we have purchasedtance> buyBItem = activeRecipe->getBuyBItem();

					int buyAMatches = player->invent// 4J JEV - Award itemsBought stat.nt buyBMatches = player->inventory->countMatches(buyBItem);
					if( (buyAItem != NULL && buyAMatches >= buyAItem->count) && (buyBItem == NULL || buyBMatches >= buyBItem->count) )
					{
                                            menu->getMerchant()->notifyTrade(
                                                activeRecipe);

                                            
						player->inventory
                                                ->removeResources(buyAItem);
                                            player->inventory->removeResources(
                                                buyBItem);

                                            
						std::shared_ptr<ItemInstance>
                                                result =
                                                    activeRecipe->getSellItem()
                                                        ->copy();

                                            
						player->awardStat(
                                                GenericStats::itemsBought(
                                                    result->getItem()->id),
                                                GenericStats::param_itemsBought(
                                                    result->getItem()->id,
                                                    result->getAuxValue(),
                                                    result->GetCount()));

                                            if (!player->inventory->add(
                                                    result)) {
                                                player->drop(result);
                                            }
					}
                                    }
                                }
                            }
                        }
                    }

                    INetworkPlayer* PlayerConnection::getNetworkPlayer() {
                        if (connection != NULL &&
                            connection->getSocket() != NULL)
                            return connection->getSocket()->getPlayer();
                        else
                            return NULL;
                    }

                    bool PlayerConnection::isLocal() {
                        if (connection->getSocket() == NULL) {
                            return false;
                        } else {
                            bool isLocal = connection->getSocket()->isLocal();
                            return connection->getSocket()->isLocal();
                        }
                    }

                    bool PlayerConnection::isGuest() {
                        if (connection->getSocket() == NULL) {
                            return false;
                        } else {
                            INetworkPlayer* networkPlayer =
                                connection->getSocket()->getPlayer();
                            bool isGuest = false;
                            if (networkPlayer != NULL) {
                                isGuest = networkPlayer->IsGuest() == TRUE;
                            }
                            return isGuest;
                        }
                    }
