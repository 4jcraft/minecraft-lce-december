#include "stdafx.h"
#include "net.minecraft.network.packet.h"
#include "PacketListener.h"

void PacketListener::handleBlockRegionUpdate(std::shared_ptr<BlockRegionUpdatePacket> packet) 
{
}

void PacketListener::onUnhandledPacket(std::shared_ptr<Packet> packet)
{
}

void PacketListener::onDisconnect(DisconnectPacket::eDisconnectReason reason, void *reasonObjects) 
{
}

void PacketListener::handleDisconnect(std::shared_ptr<DisconnectPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleLogin(std::shared_ptr<LoginPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleMovePlayer(std::shared_ptr<MovePlayerPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleChunkTilesUpdate(std::shared_ptr<ChunkTilesUpdatePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handlePlayerAction(std::shared_ptr<PlayerActionPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTileUpdate(std::shared_ptr<TileUpdatePacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleChunkVisibility(std::shared_ptr<ChunkVisibilityPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAddPlayer(std::shared_ptr<AddPlayerPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleMoveEntity(std::shared_ptr<MoveEntityPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleMoveEntitySmall(std::shared_ptr<MoveEntityPacketSmall> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTeleportEntity(std::shared_ptr<TeleportEntityPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleUseItem(std::shared_ptr<UseItemPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetCarriedItem(std::shared_ptr<SetCarriedItemPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleRemoveEntity(std::shared_ptr<RemoveEntitiesPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTakeItemEntity(std::shared_ptr<TakeItemEntityPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleChat(std::shared_ptr<ChatPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAddEntity(std::shared_ptr<AddEntityPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAnimate(std::shared_ptr<AnimatePacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handlePlayerCommand(std::shared_ptr<PlayerCommandPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handlePreLogin(std::shared_ptr<PreLoginPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAddMob(std::shared_ptr<AddMobPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetTime(std::shared_ptr<SetTimePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetSpawn(std::shared_ptr<SetSpawnPositionPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetEntityMotion(std::shared_ptr<SetEntityMotionPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetEntityData(std::shared_ptr<SetEntityDataPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleEntityLinkPacket(std::shared_ptr<SetEntityLinkPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleInteract(std::shared_ptr<InteractPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleEntityEvent(std::shared_ptr<EntityEventPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetHealth(std::shared_ptr<SetHealthPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleRespawn(std::shared_ptr<RespawnPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTexture(std::shared_ptr<TexturePacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTextureAndGeometry(std::shared_ptr<TextureAndGeometryPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleExplosion(std::shared_ptr<ExplodePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerOpen(std::shared_ptr<ContainerOpenPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerClose(std::shared_ptr<ContainerClosePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerClick(std::shared_ptr<ContainerClickPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerSetSlot(std::shared_ptr<ContainerSetSlotPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerContent(std::shared_ptr<ContainerSetContentPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSignUpdate(std::shared_ptr<SignUpdatePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerSetData(std::shared_ptr<ContainerSetDataPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleSetEquippedItem(std::shared_ptr<SetEquippedItemPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleContainerAck(std::shared_ptr<ContainerAckPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAddPainting(std::shared_ptr<AddPaintingPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTileEvent(std::shared_ptr<TileEventPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAwardStat(std::shared_ptr<AwardStatPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleEntityActionAtPosition(std::shared_ptr<EntityActionAtPositionPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handlePlayerInput(std::shared_ptr<PlayerInputPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleGameEvent(std::shared_ptr<GameEventPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleAddGlobalEntity(std::shared_ptr<AddGlobalEntityPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleComplexItemData(std::shared_ptr<ComplexItemDataPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleLevelEvent(std::shared_ptr<LevelEventPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

// 1.8.2
void PacketListener::handleGetInfo(std::shared_ptr<GetInfoPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleUpdateMobEffect(std::shared_ptr<UpdateMobEffectPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleRemoveMobEffect(std::shared_ptr<RemoveMobEffectPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handlePlayerInfo(std::shared_ptr<PlayerInfoPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleKeepAlive(std::shared_ptr<KeepAlivePacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleSetExperience(std::shared_ptr<SetExperiencePacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleSetCreativeModeSlot(std::shared_ptr<SetCreativeModeSlotPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleAddExperienceOrb(std::shared_ptr<AddExperienceOrbPacket> packet)
{
	onUnhandledPacket(packet);
}

// 1.0.1
void PacketListener::handleContainerButtonClick(std::shared_ptr<ContainerButtonClickPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleTileEntityData(std::shared_ptr<TileEntityDataPacket> tileEntityDataPacket)
{
	onUnhandledPacket(tileEntityDataPacket);
}

// 1.1
void PacketListener::handleCustomPayload(std::shared_ptr<CustomPayloadPacket> customPayloadPacket)
{
	onUnhandledPacket(customPayloadPacket);
}

// 1.2.3
void PacketListener::handleRotateMob(std::shared_ptr<RotateHeadPacket> rotateMobPacket)
{
	onUnhandledPacket(rotateMobPacket);
}

// 1.3.2
void PacketListener::handleClientProtocolPacket(std::shared_ptr<ClientProtocolPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleServerAuthData(std::shared_ptr<ServerAuthDataPacket> packet)
{
	onUnhandledPacket(packet);
}

//void PacketListener::handleSharedKey(shared_ptr<SharedKeyPacket> packet)
//{
//	onUnhandledPacket(packet);
//}

void PacketListener::handlePlayerAbilities(std::shared_ptr<PlayerAbilitiesPacket> playerAbilitiesPacket)
{
	onUnhandledPacket(playerAbilitiesPacket);
}

void PacketListener::handleChatAutoComplete(std::shared_ptr<ChatAutoCompletePacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleClientInformation(std::shared_ptr<ClientInformationPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleSoundEvent(std::shared_ptr<LevelSoundPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleTileDestruction(std::shared_ptr<TileDestructionPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleClientCommand(std::shared_ptr<ClientCommandPacket> packet)
{
}

//void PacketListener::handleLevelChunks(shared_ptr<LevelChunksPacket> packet)
//{
//	onUnhandledPacket(packet);
//}

bool PacketListener::canHandleAsyncPackets()
{
	return false;
}



// 1.6.4
void PacketListener::handleAddObjective(std::shared_ptr<SetObjectivePacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleSetScore(std::shared_ptr<SetScorePacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleSetDisplayObjective(std::shared_ptr<SetDisplayObjectivePacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleSetPlayerTeamPacket(std::shared_ptr<SetPlayerTeamPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleParticleEvent(std::shared_ptr<LevelParticlesPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleUpdateAttributes(std::shared_ptr<UpdateAttributesPacket> packet)
{
	onUnhandledPacket(packet);
}

void PacketListener::handleTileEditorOpen(std::shared_ptr<TileEditorOpenPacket> tileEditorOpenPacket)
{
}

bool PacketListener::isDisconnected()
{
	return false;
}

// 4J Added

void PacketListener::handleCraftItem(std::shared_ptr<CraftItemPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTradeItem(std::shared_ptr<TradeItemPacket> packet) 
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleDebugOptions(std::shared_ptr<DebugOptionsPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleServerSettingsChanged(std::shared_ptr<ServerSettingsChangedPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleChunkVisibilityArea(std::shared_ptr<ChunkVisibilityAreaPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleUpdateProgress(std::shared_ptr<UpdateProgressPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTextureChange(std::shared_ptr<TextureChangePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleTextureAndGeometryChange(std::shared_ptr<TextureAndGeometryChangePacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleUpdateGameRuleProgressPacket(std::shared_ptr<UpdateGameRuleProgressPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleKickPlayer(std::shared_ptr<KickPlayerPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleXZ(std::shared_ptr<XZPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}

void PacketListener::handleGameCommand(std::shared_ptr<GameCommandPacket> packet)
{
	onUnhandledPacket( (shared_ptr<Packet> ) packet);
}
