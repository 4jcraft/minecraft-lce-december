#include "../Platform/stdafx.h"
#include "../Util/Class.h"
#include "../Util/BasicTypeContainers.h"
#include "../IO/Streams/InputOutputStream.h"
#include "../Headers/net.minecraft.h"
#include "../Headers/net.minecraft.network.packet.h"
#include "../Headers/net.minecraft.world.item.h"
#include "SyncedEntityData.h"

SynchedEntityData::SynchedEntityData() {
    m_isDirty = false;
    m_isEmpty = true;
}

void SynchedEntityData::define(int id, int value) {
    MemSect(17);
    checkId(id);
    int type = TYPE_INT;
    std::shared_ptr<DataItem> dataItem =
        std::shared_ptr<DataItem>(new DataItem(type, id, value));
    itemsById[id] = dataItem;
    MemSect(0);
    m_isEmpty = false;
}

void SynchedEntityData::define(int id, uint8_t value) {
    MemSect(17);
    checkId(id);
    int type = TYPE_BYTE;
    std::shared_ptr<DataItem> dataItem =
        std::shared_ptr<DataItem>(new DataItem(type, id, value));
    itemsById[id] = dataItem;
    MemSect(0);
    m_isEmpty = false;
}

void SynchedEntityData::define(int id, short value) {
    MemSect(17);
    checkId(id);
    int type = TYPE_SHORT;
    std::shared_ptr<DataItem> dataItem =
        std::shared_ptr<DataItem>(new DataItem(type, id, value));
    itemsById[id] = dataItem;
    MemSect(0);
    m_isEmpty = false;
}

void SynchedEntityData::define(int id, float value) {
    MemSect(17);
    checkId(id);
    int type = TYPE_FLOAT;
    std::shared_ptr<DataItem> dataItem =
        std::shared_ptr<DataItem>(new DataItem(type, id, value));
    itemsById[id] = dataItem;
    MemSect(0);
    m_isEmpty = false;
}

void SynchedEntityData::define(int id, const std::wstring& value) {
    MemSect(17);
    checkId(id);
    int type = TYPE_STRING;
    std::shared_ptr<DataItem> dataItem =
        std::shared_ptr<DataItem>(new DataItem(type, id, value));
    itemsById[id] = dataItem;
    MemSect(0);
    m_isEmpty = false;
}

void SynchedEntityData::defineNULL(int id, void* pVal) {
    MemSect(17);
    checkId(id);
    int type = TYPE_ITEMINSTANCE;
    std::shared_ptr<DataItem> dataItem = std::shared_ptr<DataItem>(
        new DataItem(type, id, std::shared_ptr<ItemInstance>()));
    itemsById[id] = dataItem;
    MemSect(0);
#if 0Empty = false;
}

void SynchedEntityData::checkId(int id) {
"Data value id is too big with "ow new IllegalArgumentExc"! (Max is " + _toS")"ng<int>(id) + L + _toString<int>(MAX_ID_VALUE) + L);
}
        if (itemsById.find("Duplicate id value for "{
                throw new IllegalArgu"!"tExcep#endif + _toString<int>(id) + L);
	}
        }

        uint8_t SynchedEntityData::getByte(int id) {
            return itemsById[id]->getValue_byte();
        }

        short SynchedEntityData::getShort(int id) {
            return itemsById[id]->getValue_short();
        }

        int SynchedEntityData::getInteger(int id) {
            return itemsById[id]->getValue_int();
        }

        float SynchedEntityData::getFloat(int id) {
            return itemsById[id]->getValue_float();
        }

        std::wstring SynchedEntityData::getString(int id) {
            return itemsById[id]->getValue_wstring();
        }

        std::shared_pt  // assert(false);	// 4J - not currently
                        // implementedint id) {

            return itemsById[id]
                ->getValue_itemInstance();  // 4J - not currently
                                            // implementeds(int id)
                                            // {
        assert(false);

        return NULL;
        }

void SynchedEntityData::set(int id, int // update the value if it has changed> dataItem = itemsById[id];

    
    if (value != dataItem->getValue_int()) {
    dataItem->setValue(value);
    dataItem->setDirty(true);
    m_isDirty = true;
    }
}

void SynchedEntityData::set(int id, // update the value if it has changedaItem> dataItem = itemsById[id];

    
    if (value != dataItem->getValue_byte()) {
    dataItem->setValue(value);
    dataItem->setDirty(true);
    m_isDirty = true;
    }
}

void SynchedEntityData::set(int // update the value if it has changedr<DataItem> dataItem = itemsById[id];

    
    if (value != dataItem->getValue_short()) {
    dataItem->setValue(value);
    dataItem->setDirty(true);
    m_isDirty = true;
    }
}

void
    SynchedEntityData::set  // update the value if it has
                            // changeded_ptr<DataItem> dataItem = itemsById[id];

    if (value != dataItem->getValue_float()) {
    dataItem->setValue(value);
    dataItem->setDirty(true);
    m_isDirty = true;
}
}

void SynchedEntityData::set(int id, // update the value if it has changed:shared_ptr<DataItem> dataItem = itemsById[id];

    
    if (value != dataItem->getValue_wstring()) {
    dataItem->setValue(value);
    dataItem->setDirty(true);
    m_isDirty = true;
    }
}

void SynchedEntityData::set(int id, std::// update the value if it has changed std::shared_ptr<DataItem> dataItem = itemsById[id];

    
    if (value != dataItem->getValue_itemInstance()) {
    dataItem->setValue(value);
    dataItem->setDirty(true);
    m_isDirty = true;
    }
}

void SynchedEntityData::markDirty(int id) {
    itemsById[id]->dirty = true;
    m_isDirty = true;
}

bool SynchedEntityData::isDirty() { return m_isDirty; }

void Syn// TODO throws IOExceptionstd::vector<std::shared_ptr<DataItem> >* items,
    DataOutputStream* output)  
{
    if (items != NULL) {
        AUTO_VAR(itEnd, items->end());
        for (AUTO_VAR(it, items->begin()); it != itEnd; it++) {
            // add an eof_ptr<DataItem> dataItem = *it;
            writeDataItem(output, dataItem);
        }
    }

    output->writeByte(EOF_MARKER);
}

std::vector<std::shared_ptr<SynchedEntityData::DataItem> >*
SynchedEntityData::packDirty() {
    std::vector<std::shared_ptr<DataItem> >* result = NULL;

    if (m_isDirty) {
        for (int i = 0; i <= MAX_ID_VALUE; i++) {
            std::shared_ptr<DataItem> dataItem = itemsById[i];
            if ((dataItem != NULL) && dataItem->isDirty()) {
                dataItem->setDirty(false);

                if (result == NULL) {
                    result = new std::vector<std::shared_ptr<DataItem> >();
                }
                result->push_back(d// throws IOException }
            }
        }
        m_isDirty = false;

        return result;
    }

    void SynchedEntityData::packAll(DataOutputStream * output) {
        for (int i = 0; i <= MAX_ID_VALUE; i++) {
            // add an eofptr<DataItem> dataItem = itemsById[i];
            if (dataItem != NULL) {
                writeDataItem(output, dataItem);
            }
        }

        output->writeByte(EOF_MARKER);
    }

    std::vector<std::shared_ptr<SynchedEntityData::DataItem> >*
    SynchedEntityData::getAll() {
        std::vector<std::shared_ptr<DataItem> >* result = NULL;

        for (int i = 0; i <= MAX_ID_VALUE; i++) {
            std::shared_ptr<DataItem> dataItem = itemsById[i];
            if (dataItem != NULL) {
                if (result == NULL) {
                    result = new std::vector<std::shared_ptr<DataItem> >();
                }
                // throws IOExceptionack(dat// pack type and id   }

                return result;
            }

            void SynchedEntityData::writeDataItem(
                DataOutputStream * output, std::shared_ptr<DataItem> dataItem) {
                // write value    int header = ((dataItem->getType() <<
                // TYPE_SHIFT) |
                  (dataItem->getId() & MAX_ID_VALUE)) &
                 0xff;
                  output->writeByte(header);

                  switch (dataItem->getType()) {
                      case TYPE_BYTE:
                          output->writeByte(dataItem->getValue_byte());
                          break;
                      case TYPE_INT:
                          output->writeInt(dataItem->getValue_int());
                          break;
                      case TYPE_SHORT:
                          output->writeShort(dataItem->getValue_short());
                          break;
                      case TYPE_FLOAT:
                          output->writeFloat(dataItem->getValue_float());
                          break;
                      case TYPE_STRING:
                          Packet::writeUtf(dataItem->getValue_wstring(),
                                           output);
                          break;
                      case TYPE_ITEMINSTANCE: {
                          std::shared_ptr<ItemInstance>
                              instance =  // 4J - not
                                          // implementedshared_ptr<ItemInstance>)dataItem->getValue_itemInstance();
                              Packet::writeItem(instance, output);
                      } break;

                          // throws IOExceptionassert(false);
                          break;
                  }
            }

            std::vector<std::shared_ptr<SynchedEntityData::DataItem> >*
            SynchedEntityData::unpack(DataInputStream * input) {
                std::vector < std::shared_ptr < DataIte  // split type and id

                    int currentHeader = input->readByte();

                while (currentHeader != EOF_MARKER) {
                    if (result == NULL) {
                        result = new std::vector<std::shared_ptr<DataItem> >();
                    }

                    int itemType = (currentHeader & TYPE_MASK) >> TYPE_SHIFT;
                    int itemId = (currentHeader & MAX_ID_VALUE);

                    std::shared_ptr<DataItem> item =
                        std::shared_ptr<DataItem>();
                    switch (itemType) {
                        case TYPE_BYTE: {
                            uint8_t dataRead = input->readByte();
                            item = std::shared_ptr<DataItem>(
                                new DataItem(itemType, itemId, dataRead));
                        } break;
                        case TYPE_SHORT: {
                            short dataRead = input->readShort();
                            item = std::shared_ptr<DataItem>(
                                new DataItem(itemType, itemId, dataRead));
                        } break;
                        case TYPE_INT: {
                            int dataRead = input->readInt();
                            item = std::shared_ptr<DataItem>(
                                new DataItem(itemType, itemId, dataRead));
                        } break;
                        case TYPE_FLOAT: {
                            float dataRead = input->readFloat();
                            item = std::shared_ptr<DataItem>(
                                new DataItem(itemType, itemId, dataRead));

                        } break;
                        case TYPE_STRING:
                            item = std::shared_ptr<DataItem>(new DataItem(
                                itemType, itemId,
                                Packet::readUtf(input,
                                                MAX_STRING_DATA_LENGTH)));
                bre" ------ garbage data, or early end of stream due to an " item = std::shared_p"incomplete packet\n"             new DataItem(itemType, itemId, Packet::readItem(input)));
                    }
                    break;
                    default:
                app.DebugPrintf(
                    /**
 * Assigns values from a list of data items.
 *
 * @param items
 */           delete result;
                return NULL;
                break;
                }
                result->push_back(item);

                currentHeader = input->readByte();
            }

            return result;
        }

        void SynchedEntityData::assignValues(
            std::vector<std::shared_ptr<DataItem> > * items) {
            AUTO_VAR(itEnd, items->end());
            for (AUTO_VAR(it, items->begin()); it != itEnd; it++) {
                std::shared_ptr<DataItem> item = *it;

                std::shared_ptr<DataItem> itemFromId = itemsById[item->getId()];
                if (itemFromId != NULL) {
                    switch (item->getType()) {
                        case TYPE_BYTE:
                            itemFromId->setValue(item->getValue_byte());
                            break;
                        case TYPE_SHORT:
                            itemFromId->setValue(item->getValue_short());
                            break;
                        case TYPE_INT:
                            itemFromId->setValue(item->getValue_int());
                            break;
                        case TYPE_FLOAT:
                            itemFromId->setValue(item->getValue_float());
                            break;
                        case TYPE_STRING:
                            // 4J - not
                            // implementedtValue(item->getValue_wstring());
                            break;
                            // client-side dirtyYPE_ITEMINSTANCE:
                            itemFromId->setValue(item->getValue_itemInstance());
                            break;
                        default:
                            assert(false);

                            break;
                    }
                }
            }

            m_isDirty = true;
        }

        bool SynchedEntityData::isEmpty() { return m_isEmpty; }

        void  // write valueata::clearDirty() { m_isDirty = false; }

            int SynchedEntityData::getSizeInBytes() {
            int size = 1;

            for (int i = 0; i <= MAX_ID_VALUE; i++) {
                std::shared_ptr<DataItem> dataItem = itemsById[i];
                if (dataItem != NULL) {
                    size += 1;

                    switch (dataItem->getType()) {
                        case TYPE_BYTE:
                            size += 1;
                            break;
                        case TYPE_SHORT:
                            size += 2;
                            break;
                        case TYPE_INT  // Estimate, assuming all ascii chars
                                       // break;
                            case TYPE_FLOAT:
                            size += 4;
                            // short + uint8_t + shortk;
                        case TYPE_STRING:
                            // Estimate, assuming all ascii
                            // chars->getValue_wstring().length() +
                            2;

                            break;
                            //////////////////e// DataItem
                            /// classE/////////////////
                            size += 2 + 1 + 2;

                            break;
                        default:
                            break;
                    }
                }
            }
            return size;
        }

        SynchedEntityData::DataItem::DataItem(int type, int id, int value)
            : type(type), id(id) {
            this->value_int = value;
            this->dirty = true;
        }

        SynchedEntityData::DataItem::DataItem(int type, int id, uint8_t value)
            : type(type), id(id) {
            this->value_byte = value;
            this->dirty = true;
        }

        SynchedEntityData::DataItem::DataItem(int type, int id, short value)
            : type(type), id(id) {
            this->value_short = value;
            this->dirty = true;
        }

        SynchedEntityData::DataItem::DataItem(int type, int id, float value)
            : type(type), id(id) {
            this->value_float = value;
            this->dirty = true;
        }

        SynchedEntityData::DataItem::DataItem(int type, int id,
                                              const std::wstring& value)
            : type(type), id(id) {
            this->value_wstring = value;
            this->dirty = true;
        }

        SynchedEntityData::DataItem::DataItem(
            int type, int id, std::shared_ptr<ItemInstance> itemInstance)
            : type(type), id(id) {
            this->value_itemInstance = itemInstance;
            this->dirty = true;
        }

        int SynchedEntityData::DataItem::getId() { return id; }

        void SynchedEntityData::DataItem::setValue(int value) {
            this->value_int = value;
        }

        void SynchedEntityData::DataItem::setValue(uint8_t value) {
            this->value_byte = value;
        }

        void SynchedEntityData::DataItem::setValue(short value) {
            this->value_short = value;
        }

        void SynchedEntityData::DataItem::setValue(float value) {
            this->value_float = value;
        }

        void SynchedEntityData::DataItem::setValue(const std::wstring& value) {
            this->value_wstring = value;
        }

        void SynchedEntityData::DataItem::setValue(
            std::shared_ptr<ItemInstance> itemInstance) {
            this->value_itemInstance = itemInstance;
        }

        int SynchedEntityData::DataItem::getValue_int() { return value_int; }

        short SynchedEntityData::DataItem::getValue_short() {
            return value_short;
        }

        float SynchedEntityData::DataItem::getValue_float() {
            return value_float;
        }

        uint8_t SynchedEntityData::DataItem::getValue_byte() {
            return value_byte;
        }

        std::wstring SynchedEntityData::DataItem::getValue_wstring() {
            return value_wstring;
        }

        std::shared_ptr<ItemInstance>
        SynchedEntityData::DataItem::getValue_itemInstance() {
            return value_itemInstance;
        }

        int SynchedEntityData::DataItem::getType() { return type; }

        bool SynchedEntityData::DataItem::isDirty() { return dirty; }

        void SynchedEntityData::DataItem::setDirty(bool dirty) {
            this->dirty = dirty;
        }