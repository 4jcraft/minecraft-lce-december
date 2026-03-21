#pragma once

#include "../../Entities/Entity.h"
#include "../../Util/JavaIntHash.h"

class Level;
class CompoundTag;
typedef Entity *(*entityCreateFn)(Level *);
class EntityIO
{
public:
	class SpawnableMobInfo
	{
	public:
		int id;
		eMinecraftColour eggColor1;
		eMinecraftColour eggColor2;
		int nameId; // 4J Added

		SpawnableMobInfo(int id, eMinecraftColour eggColor1, eMinecraftColour eggColor2, int nameId)
		{
			this->id = id;
			this->eggColor1 = eggColor1;
			this->eggColor2 = eggColor2;
			this->nameId = nameId;
		}
	};

private:
	static unordered_map<wstring, entityCreateFn> *idCreateMap;
	static unordered_map<eINSTANCEOF, wstring, eINSTANCEOFKeyHash, eINSTANCEOFKeyEq> *classIdMap;
	static unordered_map<int, entityCreateFn> *numCreateMap;
	static unordered_map<int, eINSTANCEOF> *numClassMap;
	static unordered_map<eINSTANCEOF, int, eINSTANCEOFKeyHash, eINSTANCEOFKeyEq> *classNumMap;
	static unordered_map<wstring, int> *idNumMap;

public:
	static unordered_map<int, SpawnableMobInfo *> idsSpawnableInCreative;

private:
	static void setId(entityCreateFn createFn, eINSTANCEOF clas, const std::wstring &id, int idNum);
	static void setId(entityCreateFn createFn, eINSTANCEOF clas, const std::wstring &id, int idNum, eMinecraftColour color1, eMinecraftColour color2, int nameId);

public:
	static void staticCtor();
	static shared_ptr<Entity> newEntity(const std::wstring& id, Level *level);
	static shared_ptr<Entity> loadStatic(CompoundTag *tag, Level *level);
	static shared_ptr<Entity> newById(int id, Level *level);
	static shared_ptr<Entity> newByEnumType(eINSTANCEOF eType, Level *level);
	static int getId(shared_ptr<Entity> entity);
	static std::wstring getEncodeId(shared_ptr<Entity> entity);
	static int getId(const std::wstring &encodeId);
	static std::wstring getEncodeId(int entityIoValue);
	static int getNameId(int entityIoValue);
	static eINSTANCEOF getType(const std::wstring &idString);
	static eINSTANCEOF getClass(int id);

	// 4J-JEV, added for enumerating mobs.
	static int eTypeToIoid(eINSTANCEOF eType);
};
