#pragma once

#include "f4se/GameTypes.h"
#include "f4se/GameExtraData.h"
#include "f4se/GameHandle.h"
#include "f4se/NiTypes.h"

class TESObjectREFR;
class BSExtraData;
class NiPoint3;
class bhkWorld;
class Actor;
class TESObjectCELL;
class TESWorldSpace;
class TESBoundObject;

namespace PowerUtils
{
// 08
struct GridConnection
{
	UInt32 connection;
	UInt32 connector;
};

// 10
struct GridSaveLoadData
{
	UInt32 node;
	GridConnection connection;
};

class PowerGrid
{
public:
	BSTHashMap<UInt32,BSTSet<GridConnection>*>	adjacencyMap;
	BSTArray<PowerUtils::GridSaveLoadData>		loadGameData;
	UInt32										loadElement;
	BSTArray<UInt32>							currentlyPowered;
	float										capacity;
	float										load;
};

typedef void(*_UpdateMovingWirelessItem)(TESObjectREFR* akRef, BSExtraData* workshopExtraData);
extern RelocAddr <_UpdateMovingWirelessItem> UpdateMovingWirelessItem;

}

namespace Workshop
{
// 30
class ContextData
{
public:
	ContextData(Actor * actor)
	{
		ctor(actor);
	}
	~ContextData() { };

	NiPoint3		lookPos;
	NiPoint3		lookDir;
	float			zAngle;
	TESObjectCELL	* cell;			// 20
	TESWorldSpace	* worldspace;	// 28

	DEFINE_MEMBER_FN_1(ctor, ContextData*, 0x00386560, Actor * refr);
};

// 08
struct DeletedItemInfo
{
	UInt32 formID;
	UInt32 count;
};

// 58
class ExtraData : public BSExtraData
{
public:
	PowerUtils::PowerGrid*					currentPowerGrid;
	BSTArray<PowerUtils::PowerGrid*>		powerGrid;
	BSTArray<Workshop::DeletedItemInfo*>	deletedItem;
	SInt32									powerRating;
	bool									offGridItems;

	DEFINE_MEMBER_FN_1(AddItem, void, 0x00383E00, TESObjectREFR* akRef);
	DEFINE_MEMBER_FN_3(AddConnection, void, 0x00384440, TESObjectREFR* akRef1, TESObjectREFR* akRef2, TESObjectREFR* akWireRef);
};

extern RelocPtr<BSPointerHandle<TESObjectREFR>> hCurrentWorkshop;

typedef TESObjectREFR*(*_FindNearestValidWorkshop)(TESObjectREFR* akRef);
extern RelocAddr <_FindNearestValidWorkshop> FindNearestValidWorkshop;

typedef void(*_ScrapReference)(Workshop::ContextData* contextData, NiPointer<TESObjectREFR>* akRef, BSTArray<BSTTuple<TESBoundObject*, UInt32>>* materials);
extern RelocAddr <_ScrapReference> ScrapReference;
}

namespace SplineUtils
{
typedef void(*_ConnectSpline)(TESObjectREFR* akEndpoint1, SInt32 linkType1, TESObjectREFR* akEndpoint2, SInt32 linkType2, TESObjectREFR* akWireRef);	// unk2 and unk4 always 0 - Adds the ExtraData
extern RelocAddr <_ConnectSpline> ConnectSpline;

typedef void(*_UpdateSpline)(Workshop::ContextData* contextData, TESObjectREFR* akWireRef, TESObjectREFR* akEndpoint1, int linkType1, TESObjectREFR* akEndpoint2, int linkType2);
extern RelocAddr <_UpdateSpline> UpdateSpline;
}

namespace TerminalUtils
{
typedef void(*_EstablishTerminalLinks)(TESObjectREFR* akWireRef);
extern RelocAddr <_EstablishTerminalLinks> EstablishTerminalLinks;
}

TESObjectREFR * GetObjectAtConnectPoint(const TESObjectREFR & source, NiPoint3 & connectPos, const bhkWorld & world, float radius);
