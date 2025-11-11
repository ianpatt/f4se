#pragma once

#include "f4se/GameTypes.h"

class BGSMaterialSwap;
class BGSMessage;
class TESQuest;
class TESWaterForm;
class TBO_InstanceData;

enum ExtraDataType
{
	kExtraData_Havok			= 0x1,
	kExtraData_Cell3D			= 0x2,
	kExtraData_CellWaterType	= 0x3,
	kExtraData_RegionList		= 0x4,
	kExtraData_SeenData			= 0x5,
	kExtraData_EditorID			= 0x6,
	kExtraData_CellMusicType	= 0x7,
	kExtraData_CellSkyRegion	= 0x8,
 	kExtraData_ProcessMiddleLow	= 0x9,
 	kExtraData_DetachTime		= 0xA,
 	kExtraData_PersistentCell	= 0xB,	// Confirmed
	kExtraData_Keywords			= 0xC,	// Confirmed
 	kExtraData_Action			= 0xD,	// Confirmed
 	kExtraData_StartingPosition	= 0xE,
	// ??						= 0xF,	// D.N.E
 	kExtraData_AnimGraphManager	= 0x10,	// Confirmed
	kExtraData_Biped			= 0x11,	// Confirmed
 	kExtraData_UsedMarkers		= 0x12,
 	kExtraData_DistantData		= 0x13,	// Confirmed
 	kExtraData_RagDollData		= 0x14,
 	kExtraData_PreVisRefs		= 0x15,	// Confirmed
 	kExtraData_InitActions		= 0x16,	// Confirmed
 	kExtraData_EssentialProtected	= 0x17,	// Confirmed
 	kExtraData_PackageStartLocation	= 0x18,	// Confirmed
 	kExtraData_Package			= 0x19,	// Confirmed
 	kExtraData_TresPassPackage	= 0x1A,
 	kExtraData_RunOncePacks		= 0x1B,
 	kExtraData_ReferenceHandle	= 0x1C,	// Confirmed
 	kExtraData_Follower			= 0x1D,	// Confirmed
 	kExtraData_LevCreaModifier	= 0x1E,	// Confirmed
 	kExtraData_Ghost			= 0x1F,
 	kExtraData_OriginalReference	= 0x20,	// Confirmed
 	kExtraData_Ownership		= 0x21,
 	kExtraData_Global			= 0x22,
 	kExtraData_Rank				= 0x23,
 	kExtraData_Count			= 0x24,
 	kExtraData_Health			= 0x25,
	kExtraData_RangedDistOverride	= 0x26,	// Confirmed
 	kExtraData_TimeLeft			= 0x27,
 	kExtraData_Charge			= 0x28,
 	kExtraData_Light			= 0x29,
 	kExtraData_Lock				= 0x2A,	// Confirmed
 	kExtraData_Teleport			= 0x2B,	// Confirmed
 	kExtraData_MapMarker		= 0x2C,	// Confirmed
 	kExtraData_LeveledCreature	= 0x2D,	// Confirmed
 	kExtraData_LeveledItem		= 0x2E,
 	kExtraData_Scale			= 0x2F,
 	kExtraData_Seed				= 0x30,	// Confirmed
 	kExtraData_MagicCaster		= 0x31,	// Confirmed
 	// ??						= 0x32,	// D.N.E
 	kExtraData_MasterFileCell	= 0x33, // Confirmed
 	kExtraData_PlayerCrimeList	= 0x34,
	kExtraData_ObjectInstance	= 0x35,	// Confirmed
 	kExtraData_EnableStateParent	= 0x36,
 	kExtraData_EnableStateChildren	= 0x37,
 	kExtraData_ItemDropper		= 0x38,	// Confirmed
 	kExtraData_DroppedItemList	= 0x39,	// Confirmed
 	kExtraData_RandomTeleportMarker = 0x3A,	// Confirmed
	//??						= 0x3B,	// D.N.E
 	kExtraData_SavedHavokData	= 0x3C,	// Confirmed
 	kExtraData_CannotWear		= 0x3D,	// Confirmed
 	kExtraData_Poison			= 0x3E,
	//??						= 0x3F,	// D.N.E
 	kExtraData_LastFinishedSequence	= 0x40,	// Confirmed
 	kExtraData_SavedAnimation	= 0x41,	// Confirmed
 	kExtraData_NorthRotation	= 0x42,
 	kExtraData_SpawnContainer	= 0x43,	// Confirmed
 	kExtraData_FriendHits		= 0x44,	// Confirmed
 	kExtraData_HeadingTarget	= 0x45,
	//??						= 0x46,	// D.N.E
 	kExtraData_RefractionProperty	= 0x47,
 	kExtraData_StartingWorldOrCell	= 0x48,
 	kExtraData_Hotkey			= 0x49,
	kExtraData_EditorRef3DData	= 0x4A,
 	kExtraData_EditiorRefMoveData	= 0x4B,	// Confirmed
 	kExtraData_InfoGeneralTopic	= 0x4C,
 	kExtraData_HasNoRumors		= 0x4D,
 	kExtraData_Sound			= 0x4E,	// Confirmed
 	kExtraData_TerminalState	= 0x4F,
 	kExtraData_LinkedRef		= 0x50,
 	kExtraData_LinkedRefChildren	= 0x51,
 	kExtraData_ActivateRef		= 0x52,	// Confirmed
 	kExtraData_ActivateRefChildren	=0x53,	// Confirmed
 	kExtraData_CanTalkToPlayer	= 0x54,	// Confirmed
 	kExtraData_ObjectHealth		= 0x55,
 	kExtraData_CellImageSpace	= 0x56,
 	kExtraData_NavMeshPortal	= 0x57,
 	kExtraData_ModelSwap		= 0x58,	// Confirmed
 	kExtraData_Radius			= 0x59,
	//??						= 0x5A,	// D.N.E
 	kExtraData_FactionChanges	= 0x5B,	// Confirmed
 	kExtraData_DismemberedLimbs	= 0x5C,	// Confirmed
 	kExtraData_ActorCause		= 0x5D,	// Confirmed
 	kExtraData_MultiBound		= 0x5E,
 	kExtraData_MultiBoundData	= 0x5F,
 	kExtraData_MultiBoundRef	= 0x60,
 	kExtraData_ReflectedRefs	= 0x61,
 	kExtraData_ReflectorRefs	= 0x62,
 	kExtraData_EmittanceSource	= 0x63,
 	kExtraData_RadioData		= 0x64,	// Confirmed
 	kExtraData_CombatStyle		= 0x65,
	//??						= 0x66,	// D.N.E
 	kExtraData_Primitive		= 0x67,
 	kExtraData_OpenCloseActivateRef	= 0x68,
 	kExtraData_AnimNoteReceiver	= 0x69,
 	kExtraData_Ammo				= 0x6A,
 	kExtraData_PatrolRefData	= 0x6B,
 	kExtraData_PackageData		= 0x6C,
 	kExtraData_OcclusionShape	= 0x6D,	// Confirmed
 	kExtraData_CollisionData	= 0x6E,	// Confirmed
 	kExtraData_SayTopicInfoOnceADay	= 0x6F,
 	kExtraData_EncounterZone	= 0x70,
 	kExtraData_SayTopicInfo		= 0x71,
 	kExtraData_OcclusionPlaneRefData	= 0x72,
 	kExtraData_PortalRefData	= 0x73,
 	kExtraData_Portal			= 0x74,
 	kExtraData_Room				= 0x75,
 	kExtraData_HealthPerc		= 0x76,
 	kExtraData_RoomRefData		= 0x77,
 	kExtraData_GuardedRefData	= 0x78,
 	kExtraData_CreatureAwakeSound	= 0x79,
	//??						= 0x7A,	// D.N.E
 	kExtraData_Horse			= 0x7B,
 	kExtraData_IgnoredBySandbox	= 0x7C,
 	kExtraData_CellAcousticSpace	= 0x7D,
 	kExtraData_ReservedMarkers	= 0x7E,	// Confirmed
 	kExtraData_TransitionCellCount	= 0x7F,	// Confirmed
 	kExtraData_WaterLightRefs	= 0x80,
 	kExtraData_LitWaterRefs		= 0x81,
 	kExtraData_RadioRepeater	= 0x82,	// Confirmed
 	kExtraData_ActivateLoopSound	= 0x83,
 	kExtraData_PatrolRefInUseData	= 0x84,
 	kExtraData_AshPileRef		= 0x85,
	kExtraData_CreatureMovementSound	= 0x86,	// Confirmed
 	kExtraData_FollowerSwimBreadcrumbs	= 0x87,
 	kExtraData_AliasInstanceArray	= 0x88,
 	kExtraData_Location			= 0x89,	// Confirmed
	kExtraData_MasterLocation	= 0x8A,	// Confirmed
 	kExtraData_LocationRefType	= 0x8B,
 	kExtraData_PromotedRef		= 0x8C,
	kExtraData_AnimationSequencer	= 0x8D,	// Confirmed
 	kExtraData_OutfitItem		= 0x8E,	// Confirmed
	//??						= 0x8F,	// D.N.E
 	kExtraData_LeveledItemBase	= 0x90,	// Confirmed
 	kExtraData_LightData		= 0x91,
 	kExtraData_SceneData		= 0x92,
 	kExtraData_BadPosition		= 0x93,	// Confirmed
 	kExtraData_HeadTrackingWeight	= 0x94,
 	kExtraData_FromAlias		= 0x95,
 	kExtraData_ShouldWear		= 0x96,	// Confirmed
 	kExtraData_FavorCost		= 0x97,
 	kExtraData_AttachedArrows3D	= 0x98,
 	kExtraData_TextDisplayData	= 0x99,
 	kExtraData_AlphaCutoff		= 0x9A,
 	kExtraData_Enchantment		= 0x9B,
 	kExtraData_Soul				= 0x9C,
 	kExtraData_ForcedTarget		= 0x9D,	// Confirmed
	kExtraData_SoundOutputOverride	= 0x9E,	// Confirmed
 	kExtraData_UniqueID			= 0x9F,	// Confirmed
 	kExtraData_Flags			= 0xA0,	// Confirmed
 	kExtraData_RefrPath			= 0xA1,
 	kExtraData_DecalGroup		= 0xA2,
 	kExtraData_LockList			= 0xA3,
 	kExtraData_ForcedLandingMarker	= 0xA4,
 	kExtraData_LargeRefOwnerCells	= 0xA5,
 	kExtraData_CellWaterEnvMap	= 0xA6,	// Confirmed
 	kExtraData_CellGrassData	= 0xA7,
 	kExtraData_TeleportName		= 0xA8,
 	kExtraData_Interaction		= 0xA9,	// Confirmed
 	kExtraData_WaterData		= 0xAA,
 	kExtraData_WaterCurrentZoneData	= 0xAB,	// Confirmed
 	kExtraData_AttachRef		= 0xAC,	// Confirmed
 	kExtraData_AttachRefChildren	= 0xAD,	// Confirmed
 	kExtraData_GroupConstraint	= 0xAE,	// Confirmed
 	kExtraData_ScriptedAnimDependence = 0xAF,	// Confirmed
 	kExtraData_CachedScale		= 0xB0,	// Confirmed
 	kExtraData_RaceData			= 0xB1,
 	kExtraData_GIDBuffer		= 0xB2,	// Confirmed
 	kExtraData_MissingRefIDs	= 0xB3,	// Confirmed
	kExtraData_BendableSplineParams	= 0xB4,	// Confirmed
	//??						= 0xB5,	// D.N.E
	//??						= 0xB6,	// D.N.E
	//??						= 0xB7,	// D.N.E
	//??						= 0xB8,	// D.N.E
	kExtraData_MaterialSwap		= 0xB9,	// Confirmed
	kExtraData_InstanceData		= 0xBA,	// Confirmed
	kExtraData_PowerArmor		= 0xBB,	// Confirmed
	//??						= 0xBC,	// D.N.E
	kExtraData_InputEnableLayer		= 0xBD,	// Confirmed
	kExtraData_ProjectedDecalData		= 0xBE,	// Confirmed
	kExtraData_WorkshopExtraData	= 0xBF,
	kExtraData_RadioReceiver	= 0xC0,	// Confirmed
	kExtraData_CulledBone		= 0xC1,	// Confirmed
	kExtraData_ActorValueStorage	= 0xC2,	// Confirmed
	kExtraData_DirectAtTarget	= 0xC3,	// Confirmed
	kExtraData_ActivateText		= 0xC4,	// Confirmed
	kExtraData_CombinedRefs		= 0xC5,	// Confirmed
	kExtraData_ObjectBreakable	= 0xC6,	// Confirmed
	kExtraData_SavedDynamicIdles	= 0xC7,	// Confirmed
	kExtraData_IgnoredAttractKeywords	= 0xC8,	// Confirmed
	kExtraData_ModRank			= 0xC9,	// Confirmed
	kExtraData_InteriorLODWorldspace	= 0xCA,	// Confirmed
	kExtraData_BoneScaleMap		= 0xCB,	// Confirmed
	kExtraData_FXPickNodes		= 0xCC,	// Confirmed
	kExtraData_PowerArmorPreload	= 0xCD,	// Confirmed
	kExtraData_AnimGraphPreload	= 0xCE,	// Confirmed
	kExtraData_AnimSounds		= 0xCF,	// Confirmed
	kExtraData_PowerLinks		= 0xD0,	// Confirmed
	kExtraData_ObjectSavedUnrecoverableSubgraph	= 0xD1,	// Confirmed
	kExtraData_RefWeaponSounds	= 0xD2,	// Confirmed
	kExtraData_InvestedGold		= 0xD3,	// Confirmed
	kExtraData_FurnitureEntryData	= 0xD4,	// Confirmed
	kExtraData_VoiceType		= 0xD5,	// Confirmed
};

// 18
class BSExtraData
{
public:
	virtual ~BSExtraData();

	virtual void Unk_01();
	virtual void Unk_02();

	BSExtraData	* next;	// 08
	UInt16	unk10;		// 10
	UInt8	type;		// 12
	UInt8	unk13;		// 13
	UInt32	unk14;		// 14

	static BSExtraData* Create(UInt32 size, UInt64 vtbl);
};

// 20?
class ExtraDataList : public BSIntrusiveRefCounted
{
public:
	struct PresenceBitfield
	{
		bool HasType(UInt32 type) const;

		UInt8	bits[0x1B];
	};

	bool HasType(UInt32 type);
	void MarkType(UInt32 type, bool bCleared);
	bool Remove(UInt8 type, BSExtraData* toRemove);
	bool Add(UInt8 type, BSExtraData* toAdd);
	BSExtraData* GetByType(UInt32 type);

#ifdef _DEBUG
	void Dump();
#endif

	BSExtraData			* m_data;		// 08
	void				* unk10;		// 10
	PresenceBitfield	* m_presence;	// 18
	BSReadWriteLock		m_lock;			// 20
};
STATIC_ASSERT(sizeof(ExtraDataList) == 0x28);

// 28
class BGSObjectInstanceExtra : public BSExtraData
{
public:
	struct Data
	{
		struct Form
		{
			UInt32	formId;	// 00
			UInt32	unk04;	// 04
		};

		Form	* forms;	// 00
		UInt32	blockSize;	// 04 - blockSize/sizeof(Form) yields length
	};

	Data	* data;	// 18
	SInt16	unk20;		// 20
	UInt16	unk22;		// 22
	UInt32	unk24;		// 24
};

// 30
class ExtraPowerLinks : public BSExtraData
{
public:
	tArray<UInt64> connections; // 18 - The Reference formids that this is power-linked to (item A wired to item B, the wire receives formids to A and B, and A and B receive formids to the wire)

	static ExtraPowerLinks* Create();
};

// 30
class ExtraBendableSplineParams : public BSExtraData
{
public:
	float	unk18;		// 18
	float	thickness;	// 1C
	float	xOffset;	// 20
	float	yOffset;	// 24
	float	zOffset;	// 28
	float	unk2C;		// 2C
};

// 20
class ExtraMaterialSwap : public BSExtraData
{
public:
	BGSMaterialSwap * materialSwap;

	static ExtraMaterialSwap* Create(BGSMaterialSwap * matSwap);
};


// 48
class ExtraTextDisplayData : public BSExtraData {
public:
	ExtraTextDisplayData();
	virtual ~ExtraTextDisplayData();

	// 10
	struct TextReplaceEntries
	{
		BSFixedString	token;
		TESForm*		value;
	};

	BSFixedString    name;		// 18
	BGSMessage*     message;	// 20
	TESQuest*     quest;		// 28
	UInt32      type;			// 30 - 1 for refs with ref alias set name, -2 for names set by user
	UInt32      unk34;			// 34 - some terminals 1, actors 0. terminal sometimes also 0.. maybe HasTextReplaceData?
	tArray<TextReplaceEntries> * textReplaceData; // 38
	UInt16      nameLength;		// 40
	UInt16      unk42;			// 42
	UInt32      unk44;			// 44

	MEMBER_FN_PREFIX(ExtraTextDisplayData);
	DEFINE_MEMBER_FN(GetReferenceName, BSFixedString *, 0x002A9580, TESForm * baseForm);
};

// 20
class ExtraUniqueID : public BSExtraData
{
public:
	UInt16	uniqueId;	// 18
	UInt16	unk1A;		// 1A
	UInt16	formOwner;	// 1C
};

// 20
class ExtraCellWaterType : public BSExtraData
{
public:
	TESWaterForm * waterForm;	// 18
};

// 28
class ExtraInstanceData : public BSExtraData
{
public:
	TESForm				* baseForm;		// 18
	TBO_InstanceData	* instanceData;	// 20

	static ExtraInstanceData * Create(TESForm * baseForm, TBO_InstanceData * instanceData);
};

// 30
class ExtraPromotedRef : public BSExtraData
{
public:
	tArray<TESForm*> promoters; // 18 - a list of forms that are keeping this reference persistent
};
STATIC_ASSERT(sizeof(ExtraPromotedRef) == 0x30);

// 20
class ExtraFlags : public BSExtraData
{
public:
	enum {
		kFlag_PlayerHasTaken = 1 << 5,
	};
	UInt32 flags;   // 18
	UInt32 pad1C;   // 1C
};
STATIC_ASSERT(sizeof(ExtraFlags) == 0x20);

// 20
class ExtraHealth : public BSExtraData
{
public:
	float health;    // 18 - value is a percentage from 0-1.
	UInt32 pad1C;    // 1C

	static ExtraHealth* Create(float value);
};
STATIC_ASSERT(sizeof(ExtraHealth) == 0x20);
