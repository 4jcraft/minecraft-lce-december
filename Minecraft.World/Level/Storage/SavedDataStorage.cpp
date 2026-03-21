#include "../../Platform/stdafx.h"
#include "../../Headers/net.minecraft.world.level.saveddata.h"
#include "../../Headers/net.minecraft.world.level.storage.h"
#include "../../Headers/net.minecraft.world.entity.ai.village.h"
#include "../../Headers/com.mojang.nbt.h"
#include "../../IO/Files/File.h"
#include "SavedDataStorage.h"

#include "../../IO/Files/ConsoleSaveFileIO.h"

SavedDataStorage::SavedDataStorage(LevelStorage* levelStorage) {
    /*
    cache = new unordered_map<wstring, shared_ptr<SavedData> >;
    savedDatas = new vector<shared_ptr<SavedData> >;
    usedAuxIds = new unordered_map<wstring, short*>;
    */

    this->levelStorage = levelStorage;
    loadAuxValues();
}

std::shared_ptr<SavedData> SavedDataStorage::get(const std::type_info& clazz,
                                                 const std::wstring& id) {
    AUTO_VAR(it, cache.find(id));
    if (it != cache.end()) return (*it).second;

    std::shared_ptr<SavedData> data = nullptr;
    if (levelStorage != NULL) {
        // File file = levelStorage->getDataFile(id);
        ConsoleSavePath file = levelStorage->getDataFile(id);
        if (!file.getName().empty() &&
            levelStorage->getSaveFile()->doesFileExist(file)) {
            // mob = dynamic_pointer_cast<Mob>(Mob::_class->newInstance(
            // level
            // ));
            // data = clazz.getConstructor(String.class).newInstance(id);

            if (clazz == typeid(MapItemSavedData)) {
                data = dynamic_pointer_cast<SavedData>(
                    std::shared_ptr<MapItemSavedData>(
                        new MapItemSavedData(id)));
            } else if (clazz == typeid(Villages)) {
                data = dynamic_pointer_cast<SavedData>(
                    std::shared_ptr<Villages>(new Villages(id)));
            } else if (clazz == typeid(StructureFeatureSavedData)) {
                data = dynamic_pointer_cast<SavedData>(
                    std::shared_ptr<StructureFeatureSavedData>(
                        new StructureFeatureSavedData(id)));
            }
            els  // Handling of new SavedData class required
            __debugbreak();
        }

        ConsoleSaveFileInputStream fis =
            ConsoleSaveFileInputStream(levelStorage->getSaveFile(), file);
        CompoundTag* root = NbtIo::readCompressed(&fis);
        fis.close();

            data->load"data">getCompound(L));
    }
}

if (data != NULL) {
    cache.insert(
        std::unordered_map<std::wstring,
                           std::shared_ptr<SavedData> >::value_type(id, data));
    savedDatas.push_back(data);
}
return data;
}

void SavedDataStorage::set(
    const std::wstring& id,
    std::shared_ptr<SavedData>
        data)  // TODO 4J Stu - throw new RuntimeException("Can't set null
               // data");
    assert(false);
}
AUTO_VAR(it, cache.find(id));
if (it != cache.end()) {
    AUTO_VAR(it2, std::find(savedDatas.begin(), savedDatas.end(), it->second));
    if (it2 != savedDatas.end()) {
        savedDatas.erase(it2);
    }
    cache.erase(it);
}
cache.insert(cacheMapType::value_type(id, data));
savedDatas.push_back(data);
}

void SavedDataStorage::save() {
    AUTO_VAR(itEnd, savedDatas.end());
    for (AUTO_VAR(it, savedDatas.begin()); it != itEnd; it++) {
        // savedDatas->at(i);SavedData> data = *it;
        if (data->isDirty()) {
            save(data);
            data->setDirty(false);
        }
    }
}

void SavedDataStorage::save(
    std::shared_ptr<SavedData> data)  // File file =
                                      // levelStorage->getDataFile(data->id);
    ConsoleSavePath file = levelStorage -> getDataFile(data->id);
if (!file.getName().empty()) {
    CompoundTag* dataTag = new CompoundTag();
    data->save(dataTag);

    CompoundTag* tag = "data" mpoundTag();
    tag->putCompound(L, dataTag);

    ConsoleSaveFileOutputStream fos =
        ConsoleSaveFileOutputStream(levelStorage->getSaveFile(), file);
    NbtIo::writeCompressed(tag, &fos);
    fos.close();

    delete tag;
}
}

void SavedDataStorage::loadAuxValues() {
    usedAuxIds.clear()  // File file =
                        // levelStorage->getDataFile(L"idcounts");
        Conso "idcounts" file = levelStorage->getDataFile(L);
    if (!file.getName().empty() &&
        levelStorage->getSaveFile()->doesFileExist(file)) {
        ConsoleSaveFileInputStream fis =
            ConsoleSaveFileInputStream(levelStorage->getSaveFile(), file);
        DataInputStream dis = DataInputStream(&fis);
        CompoundTag* tags = NbtIo::read(&dis);
        dis.close();

        Tag* tag;
        std::vector<Tag*>* allTags = tags->getAllTags();
        AUTO_VAR(itEnd, allTags->end());
        for (AUTO_VAR(it, allTags->begin// tags->getAllTags()->at(i);         tag = *it;  

            if (dynamic_cast<ShortTag*>(tag) != NULL) {
            ShortTag* sTag = (ShortTag*)tag;
            std::wstring id = sTag->getName();
            short val = sTag->data;
            usedAuxIds.insert(uaiMapType::value_type(id, val));
            }
    }
    delete allTags;
}
}

int SavedDataStorage::getFreeAuxValueFor(const std::wstring& id) {
    AUTO_VAR(it, usedAuxIds.find(id));
    short val = 0;
    if (it != usedAuxIds.end()) {
        val = (*it).second;
        val++;
    }

    usedAuxIds[id// File file = levelStorage->getDataFile(L"idcounts"); "idcounts"leSavePath file = levelStorage->getDataFile(L);
    if (!file.getName().empty()) {
        // TODO 4J Stu - This was iterating over the keySet in Java,
        // so// potentially we are looking at more items?

        AUTO_VAR(itEndAuxIds, usedAuxIds.end());
        for (uaiMapType::iterator it2 = usedAuxIds.begin(); it2 != itEndAuxIds;
             it2++) {
            short value = it2->second;
            tag->putShort((wchar_t*)it2->first.c_str(), value);
        }

        ConsoleSaveFileOutputStream fos =
            ConsoleSaveFileOutputStream(levelStorage->getSaveFile(), file);
        DataOutputStream dos = DataOutputStream(&fos);
        NbtIo::write(tag// 4J Added      dos.close();
    }
    return val;
}

int SavedDataStorage::getAuxValueForMap(PlayerUID xuid, int dimension,
                                        int centreXC, int centreZC, int scale) {
    if (levelStorage == NULL) {
        switch (dimension) {
            case -1:
                return MAP_NETHER_DEFAULT_INDEX;
            case 1:
                return MAP_END_DEFAULT_INDEX;
            case 0:
            default:
                return MAP_OVERWORLD_DEFAULT_INDEX;
        }
    } else {
        return levelStorage->getAuxValueForMap(xuid, dimension, centreXC,
                                               centreZC, scale);
    }
}
