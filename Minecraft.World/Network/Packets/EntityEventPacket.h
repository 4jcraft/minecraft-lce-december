#pragma once

#include "Packet.h"

class EntityEventPacket : public Packet,
                          public std::enable_shared_from_this<EntityEventPacket> {
public:
    int entityId;
    byte eventId;

    EntityEventPacket();
    EntityEventPacket(int entityId, byte eventId);

    virtual void read(DataInputStream* dis);
    virtual void write(DataOutputStream* dos);
    virtual void handle(PacketListener* listener);
    virtual int getEstimatedSize();

public:
    static std::shared_ptr<Packet> create() {
        return std::shared_ptr<Packet>(new EntityEventPacket());
    }
    virtual int getId() { return 38; }
};
