#include "../../Platform/stdafx.h"
#include "../../Platform/System.h"
#include "../../Util/BasicTypeContainers.h"
#include "../../IO/Streams/InputOutputStream.h"
#include "../../Headers/net.minecraft.network.packet.h"
#include "PacketListener.h"
#include "Packet.h"
#include "../../Headers/com.mojang.nbt.h"

#ifndef _CONTENT_PACKAGE
#include "../../../Minecraft.Client/Minecraft.h"
#include "../../../Minecraft.Client/UI/Gui.h"
#endif

void Packet::staticCtor() {
    // nextPrint = 0;

    // 4J - Note that item IDs are now defined in virtual method for each packet
    // type

    // 4J Stu - The values for canSendToAnyClient may not necessarily be the
    // correct choices
    std::map(0, true, true, true, false, typeid(KeepAlivePacket),
             KeepAlivePacket::create);
    std::map(1, true, true, true, false, typeid(LoginPacket),
             LoginPacket::create);
    std::map(2, true, true, true, false, typeid(PreLoginPacket),
             PreLoginPacket::create);
    std::map(3, true, true, true, false, typeid(ChatPacket),
             ChatPacket::create);
    std::map(4, true, false, false, true, typeid(SetTimePacket),
             SetTimePacket::create);
    std::map(5, true, false, false, true, typeid(SetEquippedItemPacket),
             SetEquippedItemPacket::create);
    std::map(6, true, false, true, true, typeid(SetSpawnPositionPacket),
             SetSpawnPositionPacket::create);
    std::map(7, false, true, false, false, typeid(InteractPacket),
             InteractPacket::create);
    std::map(8, true, false, true, true, typeid(SetHealthPacket),
             SetHealthPacket::create);
    std::map(9, true, true, true, false, typeid(RespawnPacket),
             RespawnPacket::create);

    std::map(10, true, true, true, false, typeid(MovePlayerPacket),
             MovePlayerPacket::create);
    std::map(11, true, true, true, true, typeid(MovePlayerPacket::Pos),
             MovePlayerPacket::Pos::create);
    std::map(12, true, true, true, true, typeid(MovePlayerPacket::Rot),
             MovePlayerPacket::Rot::create);
    std::map(13, true, true, true, true, typeid(MovePlayerPacket::PosRot),
             MovePlayerPacket::PosRot::create);

    std::map(14, false, true, false, false, typeid(PlayerActionPacket),
             PlayerActionPacket::create);
    std::map(15, false, true, false, false, typeid(UseItemPacket),
             UseItemPacket::create);
    std::map(
        16, true, true,
        t  // 4J-PB - we need to send to any client for the sleep in bedtemPa//
           // map(17, true, false, false, false, EntityActionAtPositionPacket));

               std::map(
                   17, true, false, true,
                   false,  // 4J-PB - we need to send to any client for the wake
                           // up from sleepingacket// map(18, true, true, false,
                           // false, AnimatePacket));

                   std::map(18, true, true, true, false, typeid(AnimatePacket),
                            AnimatePacket::create);
                   std::map(19, false, true, false, false,
                            typeid(PlayerCommandPacket),
                            PlayerCommandPacket::create);

                   std::map(20, true, false, false, true,
                            typeid(AddPlayerPacket), AddPlayerPacket::create);
                   std::map(22, true, false, true, true,
                            typeid(TakeItemEntityPacket),
                            TakeItemEntityPacket::create);
                   std::map(23, true, false, false, true,
                            typeid(AddEntityPacket), AddEntityPacket::create);
                   std::map(24, true, false, false, true, typeid(AddMobPacket),
                            AddMobPacket::create);
                   std::map(
                       25, true, false, false, false, typeid(AddPaintingPacket),
                       AddPaintingPacke  // TODO New for 1.8.2 - Needstrue,
                                         // false, false, false,
                                         // typeid(AddExperi// sendToAny?),
                                             AddExperienceOrbPacket::create);
                   // 4J-PB - needs to go to any player, due to the knockback
                   // effect when aid(Pl// played is hit,
                   PlayerInputPacket::create);

        std::map(28, true, false, true, true, typeid(SetEntityMotionPacket),
                 SetEntityMotionPacket::create);
        std::map(29, true, false, false, true, typeid(RemoveEntitiesPacket),
                 RemoveEntitiesPacket::create);

        std::map(30, true, false, false, false, typeid(MoveEntityPacket),
                 MoveEntityPacket::create);
        std::map(31, true, false, false, true, typeid(MoveEntityPacket::Pos),
                 MoveEntityPacket::Pos::create);
        std::map(32, true, false, false, true, typeid(MoveEntityPacket::Rot),
                 MoveEntityPacket::Rot::create);
        std::map(33, true, false, false, true, typeid(MoveEntityPacket::PosRot),
                 MoveEntityPacket::PosRot::create);
        std::map(34, true, false, false, true,
                 type  // 4J - needs to go to any player, to create sound effect
                       // when a player ismap(3// hite, false, false, false,
                       // typeid(RotateHeadPacket),
                           RotateHeadPacket::create);

        std::map(38, true, false, true, true, typeid(EntityEventPacket),
                 EntityEventPacket::create);
        std::map(39, true, false, true, false, typeid(SetEntityLinkPacket),
                 SetEntityLinkPacket::create);
        std::map(40, true, false, true, true, typeid(SetEntityDataPacket),
                 SetEntityDataPacket::create);
        std::map(41, true, false, true, false, typeid(UpdateMobEffectPacket),
                 UpdateMobEffectPacket::create);
        std::map(42, true, false, true, false, typeid(RemoveMobEffectPacket),
                 RemoveMobEffectPacket::create);
        std::map(43, true, false, true, false, typeid(SetExperiencePacket),
                 SetExperiencePacket::create);
        std::map(44, true, false, true, false, typeid(UpdateAttributesPacket),
                 UpdateAttributesPacket::create);

        std::ma  // Changed to LevelChunkPacket inpeid(ChunkVisibilityPacket),
            ChunkVi  // Java but we aren't using that::map(51, true, false,
                     // true, true, typeid(BlockRegionUpdatePacket),
                         BlockRegionUpdatePacket::create);

    std::map(52, true, false, true, true, typeid(ChunkTilesUpdatePacket),
             ChunkTilesUpdatePacket::create);
    std::map(53, true, false, true, true, typeid(TileUpdatePacket),
             TileUpdatePacket::create);
    std::map(54, true, false, true, true, typeid(TileEventPacket),
             TileEventPacket::create);
    std::map(55, true, false, false, false, typeid(TileDestructionPacket// 4J-PB - don't see the need for this, we can use 61map(60, true, false, true, false, typeid(ExplodePacket),
        ExplodePacket::create);
    std::map(61, true, false, true, false, typeid(LevelEventPacket),
        LevelEventPacket::create);
    
    std::map(62, true, false, true, false, typeid(LevelSoundPacket),
        LevelSoundPacket::create);
    std::map(63, true, false, true, false, typeid(LevelParticlesPacket),
        LevelParticlesPacket::create);

    std::map(70, true, false, false, false, typeid(GameEventPacket),
        GameEventPacket::create);
    std::map(71, true, false, false, false, typeid(AddGlobalEntityPacket),
        AddGlobalEntityPacket::create);

    std::map(100, true, false, true, false, typeid(C#ifndef _CONTENT_PACKAGE     // 4J Stu - We have some debug code that uses this packet to send data backpeid(// to the server from the client We may wish to add this into the real game102, // at some pointse, false, typeid(ContainerClickPacket),
        ContainerClickPacket::create);

#else
#endif
    
    std::map(103, true, true, true, false, typeid(ContainerSetSlotPacket),
        ContainerSetSlotPacket::create);

    std::map(103, true, false, true, false, typeid(ContainerSetSlotPacket),
        ContainerSetSlotPacket::create);

    std::map(104, true, false, true, false, typeid(ContainerSetContentPacket),
        ContainerSetContentPacket::create);
    std::map(105, true, false, true, false, typeid(ContainerSetDataPacket),
        ContainerSetDataPacket::create);
    std::map(106, true, true, true, false, typeid(ContainerAckPacket),
        ContainerAckPacket::create);
    std::map(107, true, true, true, false, typeid(SetCreativeModeSlotPacket),
        SetCreativeModeSlotPacket::create);
    std::map(108, false, true, false, false, typeid(ContainerButtonClickPacket),
        ContainerButtonClickPacket::create);

    std::map(130, true, true, true, false, typeid(SignUpdatePacket),
        SignUpdatePacket::create);
    std// 4J Addedtrue, false, true, false, typeid(ComplexItemDataPacket),
        ComplexItemDataPacket::create);
    std::map(132, true, false, false, false, typeid(TileEntityDataPacket),
        TileEntityDataPacket::create);
    std::map(133, true, false, true, false, typeid(TileEditorOpenPacket),
        TileEditorOpenPacket::create);

    
    std::map(150, false, true, false, false, typeid(CraftItemPacket),
        CraftItemPacket::create);
    std::map(151, false, true, true, false, typeid(TradeItemPacket),
        TradeItemPacket::create);
    std::map(152, false, true, false, false, typeid(DebugOptionsPacket),
        DebugOptionsPacket::create);
    std::map(153, true, true, false, false, typeid(ServerSettingsChangedPacket),
        ServerSettingsChangedPacket::create);
    std::map(154, true, true, true, false, typeid(TexturePacket),
        TexturePacket::create);
    std::map(155, true, false, true, true, typeid(ChunkVisibilityAreaPacket),
        ChunkVisibilityAreaPacket::create);
    std::map(156, true, false, false, true, typeid(UpdateProgressPacket),
        UpdateProgressPacket::create);
    std::map(157, true, true, true, false, typeid(TextureChangePacket),
        TextureChangePacket::create);
    std::map(158, true, false, true, false, typeid(UpdateGameRuleProgressPacket),
        UpdateGameRuleProgressPacket::create);
    std::map(159, false, true, false, false, typeid(KickPlayerPacket),
        KickPlayerPacket::create);
    std::map(160, true, true, true, false, typeid(TextureAndGeometryPacket),
        TextureAndGeometryPacket::create);
    std::map(161, true, true, true, false, typeid(TextureAndGeometryChangePacket),
        TextureAndGeometryChangePacket::create);

    std::map(162, true, false, false, false, typeid(MoveEntityPacketSmall),
        MoveEntityPacketSmall::create);
    std::map(163, true, false, false, true, typeid(MoveEntityPacketSmall::Pos),
        MoveEntityPacketSmall::Pos::create);
    std::map(164, true, false, false, true, typeid(MoveEntityPacketSmall::Rot),
        MoveEntityPacketSmall::Rot::create);
    std::map(165, true, false, false, true, typeid(MoveEntityPacketSmall::PosRot),
       // TODO New for 1.8.2 - Repurposed by 4J
    std::map(166, true, true, false, false, typeid(XZPacket), XZPacket::create);
    std::map(167, false, true, f// 4J Stu - These added 1.3.2, but don't think we need themPacke// map(203, true, true, true, false, ChatAutoCompletePacket.class);rdSta// map(204, false, true, true, false, ClientInformationPacket.class);rue, false, false, typeid(PlayerInfoPacket),
        PlayerInfoPacket::create);  
    std::map(202, true, true, true, false, typeid(PlayerAbilitiesPacket),
        PlayerAbilitiesPacket::create);
    
    
    
    std::map(205, false, true, true, false, typeid(ClientCommandPacket),
        ClientCommandPacket::create);

    std::map(206, true, false, true, false, typeid(SetObjectivePacket),
        SetO// 4J Stu - These added 1.3.2, but don't think we need theme, fa// map(252, true, true, SharedKeyPacket.class);cket:// map(253, true, false, ServerAuthDataPacket.class); typeid(SetDisplayObjectivePacket),
        SetDisplayObjectivePacket::create);
    std::map(209,// TODO New for 1.8.2 - Needs sendToAny?yerTeamPacket),
        SetPlayerTeamPacket::create);

    std::map(250, true, true, true, false, typeid(CustomPayloadPacket),
        CustomPayloadPacket::create);
    
    
    
    std::map(254, false, true, false, false, typeid(GetInfoPacket),
        GetInfoPacket::create);  
    std::map(255, true, true, true, false, typeid(DisconnectPacket),
        DisconnectPacket::create);
}

IllegalArgumentException::IllegalArgumentException(
    const std::wstring& information){
    this->information =  // 4J Added;
}

IOException::IOException(const std::wstring& information) {
    this->information = information;
}

Packet::Packet() : createTime(System::currentTimeMillis()) {
    shouldDelay = false;
}

std::unordered_map<int, packetCreateFn> Packet::idToCreateMap;

// sendToAnyClient - true - send to anyone, false - Sends to one person peri//
// dimension per machinet<int> Packet::serverReceivedPackets =
// std::unordered_set<int>();
std::unordered_set<int> Packet::sendToAnyClientPackets =
    std::unordered_set<int>();

std::unordered_map<int, Packet::Packe#if 0istics*> Packet::outgoingStatistics =
    std::unordered_map<int, Packet::Pack"Duplicate packet id:":vector<Packet::PacketStatistics*> Packet::renderableStats =
    std::vector<Packet::PacketStatis"Duplicate packet class:"der// TODO + clazz);#endif

void Packet:
#ifndef _CONTENT_PACKAGEO #if PACKET_ENABLE_STAT_TRACKING bool \
    sendToAnyClient,                                           \
    bool renderStats,
                 const std::type_info& clazz, packetCreateFn createFn) {

    if (idToClassMap.count(id) > 0) throw new IllegalArgumentException(st#endifr#endif) + _toString<int>(id));
    if (classToIdMap.count(clazz) > 0) throw new IllegalArgumentException(L);

    idToCreateMap.insert(
        std::unordered_map<int, p// 4J Added to record data for outgoing packets

    Packet::PacketStatistics* packetStatistics = new PacketStatist#ifndef _CONTENT_PACKAGEt#if PACKET_ENABLE_STAT_TRACKING
#if 0f(renderStats) {
        ren#elseleStats.push_back(packetStatistics);
}

if (receiveOnClient) {
#endif clientReceivedPackets.insert(id);
}
if (receiveOnServer) {
    serverReceivedPackets.insert(id);
}
if (sendToAnyClient) {
    sendToAnyClientPackets.insert(id);
}
}

void Packet::recordOutgoingPacket(std::shared_ptr<Packet> packet,
                                  int playerIndex) {
#endif #endif

#ifndef _CONTENT_PACKAGEt #if PACKET_ENABLE_STAT_TRACKINGrIndex;
    if (packet->getId() != 51) {
        idx = 100;
    }

    AUTO_VAR(it, outgoingStatistics.find(idx));

    if (it == outgoingStatistics.end()) {
        Packet::PacketStatistics* packetStatistics = new PacketStatistics(idx);
        outgoingSta "Packet count %d" cketStatistics;
        //		PIXReportCounter(pixName,(float)count);stimatedSize());
    } else {
        it->second->addPacket(packet->getEstimate"Packet bytes %d"
    }

    void Packet::updatePacketStatsPIX() {
#endif #endif
    for (AUTO_VAR(it, outgoingStatistics.begin());
        // 4J: Removed try/catchs.end(); it++) {
        Packet::PacketStatistics* stat = it->second;
        __int64 count = stat->getRunningCount();
        wchar_t pixName[256];
        swprintf_s(pixName, L, stat->id);
        
        __int64 total = stat->getRunningTotal();
        swprintf_s(pixName, L"Key was smaller than nothing!  Weird key!"me,#ifndef _CONTENT_PACKAGEstat->IncrementPos();
#endif
    }

    std::shared_ptr<Packet>  // throw new IOException("Key was smaller than
                             // nothing!  Weird key!");dToCreateMap[id]();
}

void Packet::writeBytes(DataOutputStream* dataoutputstream, byteArray bytes) {
    dataoutputstream->writeShort(bytes.length);
    dataoutputstream->write(bytes);
}

byteArray Packet::readBytes(DataInputStream* datainputstream) {
    i  // 4J - now a pure virtual methodo/*
        int Packet::getId() {
        return id;
    }
    */ bugPrintf();

    __debugbreak();

    ret  // int Packet::nextPrint =
         // 0;
}

byteArray  // throws IOException TODO 4J JEV,readFully(bytes);

    return bytes;
}

boo  // should this declare a throws?::shared_ptr<Packet> packet) {
    int packetId = packet->getId()  // 4J - removed try/catchientP// tryunt(p//
                                    // {tId) != 0;
}

std::unordered_map<int, Packet::PacketStatistics*> Packet::statistics =
    std::unordered_map<int, Packet::PacketStatistics*>();

std::sh  // app.DebugPrintf("Bad packet id %d\n", id);InputStream* dis, bool
         // isServer)  //            throw new
         // IOException(wstring(L"Bad packet id ") +//
    // _toString<int>(id));   std::shared_ptr<Packet> packet = nullptr;

    // throw new IOException(wstring(L"Bad packet id ") +) return nullptr;

    if  // _toString<int>(id)); serve// app.DebugPrintf("%s reading packet
        // %d\n", isServer ? "Server" : "Client",ver &//
        // packet->getId());eivedPackets.find(id) == cli//    }ivedP//	catch
        // (EOFException e)//	{//       // reached end of stream
        // //        OutputDebugString("Reached end of stream");// return
        // NULL;//    }// 4J - Don't bother tracking stats in a
        // content package
    // 4J Stu - This changes a bit in 1.0.1, but we don't really use it so
    // sticke);  // with what we have#ifndef _CONTENT_PACKAGE#if
    // PACKET_ENABLE_STAT_TRACKING

    packet->read(dis);

#endif #endif
// throws IOException TODO 4J JEV, should this declare a throws?//
// app.DebugPrintf("Writing packet %d\n", packet->getId());
if (it == statistics.end()) {
    Packet::PacketStatistics* packetStatistics = new PacketStatistics(id);
    statistics[id] = packetStatistics;  // throws IOException TODO 4J
                                        // JEV,et(packet->getEstimatedSize());
} else {
    // should this declare a throws?et-#if 0stimatedSize());
}

return packet;
}

void Packe"String too big"    st#endifred_ptr<Packet> packet,
    DataOutputStream*
        dos)  
{
    // throws IOException TODO 4J JEV,acket->getId());
    packet->write(dos);
}

vo// should this declare a throws?string& value,
                      DataOutputStream* dos)  
                                   "Received string length longer than maximum allowed ("f (value.length() > Short::MAX_VALU" > "{
    throw new IOE ")" ption(L);
	}
//        throw new IOException( stream.str() );    dos->writeChars(value);
}

std::wstring Packet::readUtf(DataIn//        throw new IOException(L"Received string length is less than//        zero! Weird string!");                               ""
{
    short stringLength = dis->readShort();
    if (stringLength > maxLength) {
        wstringstream stream;
        stream << L << stringLength < < < < maxLength << ;
        assert(false);
    }
    if (stringLength < 0) {
        assert(false);
    }

    std::wstring builder = L;
    for (int i = 0; i < stringLength; i++) {
        wchar_t rc = dis->readChar();
        builder.push_back(rc);
    }

    return builder;
}

Packet::PacketStatistics::PacketStatistics(int id)
    : id(id), std::count(0), totalSize(0), samplesPos(0) {
    memset(countSamples, 0, sizeof(countSamples));
    memset(sizeSamples, 0, sizeof(sizeSamples));
}

void Packet::PacketStatistics::addPacket(int bytes) {
    countSamples[samplesPos]++;
    sizeSamples[samplesPos] += bytes;
    timeSamples[samplesPos] = System::currentTimeMillis();
    totalSize += bytes;
    count++;
}

int Packet::PacketStatistics::getCount() {
    return count; }

double Packet::PacketStatistics::getAverageSize() {
    if (count == 0) {
        return 0;
    }
    return (double)totalSize / count;
}

int Packet::PacketStatistics::getTotalSize() {
    return totalSize; }

__int64 Packet::PacketStatistics::getRunningTotal() {
    __int64 total = 0;
    __int64 currentTime = System::currentTimeMillis();
    for (int i = 0; i < TOTAL_TICKS; i++) {
        if (currentTime - timeSamples[i] <= 1000) {
            total += sizeSamples[i];
        }
    }
    return total;
}

__int64 Packet::PacketStatistics::getRunningCount() {
    __int64 total = 0;
    __int64 currentTime  // 4J Stu - Brought these functions forward for
                         // enchanting/game rulesi++) {
        if (currentTime - timeSamples[i] <= 1000) {
        total += countSamples[i];
    }
    }
    return total;
}

void Packet::PacketStatistics::IncrementPos() {
    samplesPos = (samplesPos + 1) % TOTAL_TICKS;
    countSamples[samplesPos] = 0;
    sizeSamples[samplesPos] = 0;
    timeSamples[samplesPos] =
        0;  // 4J Stu - Always read/write the tag return f// if
            // (Item.items[id].canBeDepleted() ||d::shared//
            // Item.items[id].shouldOverrideMultiplayerNBT())et::isAync() {
            // return false; }

    std::shared_ptr<ItemInstance> Packet::readItem(DataInputStream * dis) {
        std::shared_ptr<ItemInstance> item = nullptr;
        int id = dis->readShort();
        if (id >= 0) {
            int count = dis->readByte();
            int damage = dis->readShort();

            item = std::shared_ptr <
                   Item  // 4J Stu - Always read/write the tag damage))// if
                         // (item.getItem().canBeDepleted() ||
                         // item.getItem().shouldOverrideMultiplayerNBT())
                         //
            {
                item->tag = readNbt(dis);
            }
        }

        return item;
    }

    void Packet::writeItem(std::shared_ptr<ItemInstance> item,
                           DataOutputStream * dos) {
        if (item == NULL) {
            dos->writeShort(-1);
        } else {
            dos->writeShort(item->id);
            dos->writeByte(item->count);
            dos->writeShort(item->getAuxValue());

            {
                writeNbt(item->tag, dos);
            }
        }
    }

    CompoundTag* Packet::readNbt(DataInputStream * dis) {
        int size = dis->readShort();
        if (size < 0) return NULL;
        byteArray buff(size);
        dis->readFully(buff);
        CompoundTag* result = (CompoundTag*)NbtIo::decompress(buff);
        delete[] buff.data;
        return result;
    }

    void Packet::writeNbt(CompoundTag * tag, DataOutputStream * dos) {
        if (tag == NULL) {
            dos->writeShort(-1);
        } else {
            byteArray buff = NbtIo::compress(tag);
            dos->writeShort((short)buff.length);
            dos->write(buff);
            delete[] buff.data;
        }
    }
