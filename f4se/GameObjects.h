#pragma once

#include "f4se_common/Utilities.h"

#include "f4se/GameFormComponents.h"
#include "f4se/GameForms.h"
#include "f4se/GameEvents.h"
#include "f4se/GameCustomization.h"
#include "f4se/GameUtilities.h"
#include "f4se/NiTextures.h"

class TESClass;
class TESCombatStyle;
class TESFaction;
class BGSOutfit;
class BGSHeadPart;
class BGSColorForm;
class BGSFootstepSet;
class SpellItem;
class TESObjectMISC;
class BGSDamageType;
class VirtualMachine;
class BGSCollisionLayer;

typedef TESObjectREFR* (* _PlaceAtMe_Native)(VirtualMachine* vm, UInt32 stackId, TESObjectREFR** target, TESForm* form, SInt32 count, bool bForcePersist, bool bInitiallyDisabled, bool bDeleteWhenAble);
extern RelocAddr<_PlaceAtMe_Native> PlaceAtMe_Native;

// 20+
class TESObject : public TESForm
{
public:
	virtual void	Unk_48();
	virtual void	Unk_49();
	virtual void	Unk_4A();
	virtual void	Unk_4B();
	virtual void	Unk_4C();
	virtual void	Unk_4D();
	virtual void	Unk_4E();
	virtual void	Unk_4F();
	virtual void	Unk_50();
	virtual void	Unk_51();
	virtual void	Unk_52();
	virtual void	Unk_53();
};

// 68
class TESBoundObject : public TESObject
{
public:
	virtual void	Unk_54();
	virtual void	Unk_55();
	virtual TBO_InstanceData	* CloneInstanceData(TBO_InstanceData * other);
	virtual void	Unk_57();
	virtual void	Unk_58();
	virtual void	Unk_59();
	virtual void	Unk_5A();
	virtual void	Unk_5B();
	virtual void	Unk_5C();
	virtual void	Unk_5D();
	virtual void	Unk_5E();
	virtual void	Unk_5F();
	virtual void	Unk_60();
	virtual void	Unk_61();
	virtual void	Unk_62();
	virtual void	Unk_63();
	virtual void	Unk_64();

	struct Bound
	{
		UInt16	x;
		UInt16	y;
		UInt16	z;
	};
	Bound bounds1;								// 20
	Bound bounds2;								// 26
	BGSMod::Template::Items templateItems;		// 30
	BGSPreviewTransform		previewTransform;	// 50
	BGSSoundTagComponent	soundTagComponent;	// 60
};
STATIC_ASSERT(offsetof(TESBoundObject, templateItems) == 0x30);
STATIC_ASSERT(offsetof(TESBoundObject, previewTransform) == 0x50);
STATIC_ASSERT(offsetof(TESBoundObject, soundTagComponent) == 0x60);
STATIC_ASSERT(sizeof(TESBoundObject) == 0x68);

// 68
class TESBoundAnimObject : public TESBoundObject
{
public:

};

// 1B0
class TESActorBase : public TESBoundAnimObject
{
public:
	virtual void	Unk_65();
	virtual void	Unk_66();
	virtual void	Unk_67();
	virtual void	Unk_68();

	// parents
	TESActorBaseData			actorData;				// 68
	TESContainer				container;				// D0
	TESSpellList				spellList;				// E8
	TESAIForm					aiForm;					// F8
	TESFullName					fullName;				// 120 - CF 0x20
	ActorValueOwner				actorValueOwner;		// 130
	BGSDestructibleObjectForm	destructibleObjectForm;	// 138
	BGSSkinForm					skinForm;				// 148
	BGSKeywordForm				keywords;				// 158
	BGSAttackDataForm			attackData;				// 178
	BGSPerkRankArray			perkRankArray;			// 188
	BGSPropertySheet			propertySheet;			// 1A0
};
STATIC_ASSERT(sizeof(TESActorBase) == 0x1B0);

// 308
class TESNPC : public TESActorBase
{
public:
	enum { kTypeID = kFormType_NPC_ };

	// parents
	TESRaceForm					race;					// 1B0 - CF 0x2000000
	BGSOverridePackCollection	overridePackCollection;	// 1C0
	BGSForcedLocRefType			forcedLocRefType;		// 1F8
	BGSNativeTerminalForm		nativeTerminalForm;		// 208
	BSTEventSink<MenuOpenCloseEvent>	menuOpenCloseEvent;		// 210
	BGSAttachParentArray		attachParentArray;		// 220

	struct HeadData
	{
		HeadData() : hairColor(nullptr), unk08(nullptr), faceTextures(nullptr) { }

		BGSColorForm	* hairColor;		// 00
		void			* unk08;			// 08
		BGSTextureSet	* faceTextures;		// 10

		DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)
	};

	UInt32						unk238;					// 238
	UInt16						unk23C;					// 23C
	UInt8						unk23E;					// 23E
	UInt8						pad23F;					// 23F
	TESClass					* npcClass;				// 240 - CF 0x400
	HeadData					* headData;				// 248 - CF 0x800
	UInt64						unk250;					// 250
	TESCombatStyle				* combatStyle;			// 258
	UInt64						unk260;					// 260
	UInt64						unk268;					// 268 - CF 0x2000000
	TESNPC						* templateNPC;			// 270
	float						weightThin;				// 278 - CF 0x4000
	float						weightMuscular;			// 27C - CF 0x4000
	float						weightLarge;			// 280 - CF 0x4000
	float						unk284;					// 284
	float						unk288;					// 288
	UInt32						unk28C;					// 28C
	UInt64						unk290;					// 290
	void						* unk298;				// 298
	UInt64						unk2A0;					// 2A0
	UInt64						unk2A8;					// 2A8
	BGSOutfit					* outfit[2];			// 2B0 - CF 0x40000, 0x80000
	UInt64						unk2C0;					// 2C0
	TESFaction					* unk2C8;				// 2C8
	BGSHeadPart					** headParts;			// 2D0
	tArray<float>				* morphSetValue;		// 2D8 - 5 elements (MRSV) - CF 0x4000

	// 08
	struct MorphSetData // (MSDK/MSDV) pair
	{
		UInt32	key;		// 00
		float	value;		// 04

		bool operator==(const UInt32 a_key) const	{ return key == a_key; }
		operator UInt32() const						{ return key; }
		static inline UInt32 GetHash(const UInt32 * key)
		{
			UInt32 hash;
			CalculateCRC32_32(&hash, *key, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tkey: %08X ", key);
			_MESSAGE("\t\tdata: %f", value);
		}
	};

	// 28
	struct FaceMorphRegion	// (FMRI/FMRS) pair
	{
		UInt32	index;		// 00
		float	value[8];	// 04

		bool operator==(const UInt32 a_key) const	{ return index == a_key; }
		operator UInt32() const						{ return index; }
		static inline UInt32 GetHash(const UInt32 * key)
		{
			UInt32 hash;
			CalculateCRC32_32(&hash, *key, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tkey: %08X ", index);
			for(UInt32 i = 0; i < 8; i++)
				_MESSAGE("\t\tdata: %f", value[i]);
		}
	};

	tHashSet<FaceMorphRegion, UInt32>	* morphRegionData;	// 2E0 - (key links to CharacterCreation::FaceMorphRegion::index)
	UInt8								numHeadParts;		// 2E8 - CF 0x800
	UInt8								unk2E9;				// 2E9
	struct SkinColor
	{
		UInt8 red;
		UInt8 green;
		UInt8 blue;
		UInt8 alpha;
	} skinColor; // - CF 0x800
	UInt8								unk2EE;				// 2EE
	UInt8								unk2EF;				// 2EF
	UInt64								unk2F0;				// 2F0 - CF 0x800
	tHashSet<MorphSetData, UInt32>		* morphSetData;		// 2F8 - (key links to CharacterCreation::MorphGroup::Preset::index)
	tArray<BGSCharacterTint::Entry*>	* tints;			// 300 - CF 0x800

	MEMBER_FN_PREFIX(TESNPC);
	DEFINE_MEMBER_FN(ctor, TESNPC*, 0x0064B460);
	DEFINE_MEMBER_FN(HasOverlays, bool, 0x0065BB40);
	DEFINE_MEMBER_FN(GetOverlayHeadParts, BGSHeadPart**, 0x0065BC50);
	DEFINE_MEMBER_FN(GetNumOverlayHeadParts, int, 0x0065BD00);
	DEFINE_MEMBER_FN(GetSex, SInt64, 0x00641610); // npc->actorData.unk08 & 1
	DEFINE_MEMBER_FN(ChangeHeadPartRemovePart, void, 0x00652300, BGSHeadPart *, bool bRemoveExtraParts);
	DEFINE_MEMBER_FN(ChangeHeadPart, void, 0x00656710, BGSHeadPart *);
	DEFINE_MEMBER_FN(GetSkinColorFromTint, void, 0x00652670, NiColorA * outColor, BGSCharacterTint::PaletteEntry* paletteEntry, bool allowCustomization); // This function alters the npc's Skin Color values
	DEFINE_MEMBER_FN_0(GetFacialBoneMorphIntensity, float, 0x00651B80);
	DEFINE_MEMBER_FN_1(SetFacialBoneMorphIntensity, void, 0x00651B20, float intensity);

	void ChangeHeadPart(BGSHeadPart * headPart, bool bRemovePart, bool bRemoveExtraParts);
	BGSHeadPart * GetHeadPartByType(UInt32 type, bool bOverlays = false);

	BGSColorForm * GetHairColor();
};
STATIC_ASSERT(offsetof(TESNPC, npcClass) == 0x240);
STATIC_ASSERT(offsetof(TESNPC, combatStyle) == 0x258);
STATIC_ASSERT(offsetof(TESNPC, outfit) == 0x2B0);
STATIC_ASSERT(offsetof(TESNPC, tints) == 0x300);
STATIC_ASSERT(sizeof(TESNPC) == 0x308);

// 1B0
class TESAmmo : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_AMMO };

	TESFullName					fullName;			// 068
	BGSModelMaterialSwap		materialSwap;		// 078	BGSModelMaterialSwap
	TESIcon						icon;				// 0B8
	BGSMessageIcon				messageIcon;		// 0C8
	TESValueForm				value;				// 0E0
	BGSDestructibleObjectForm	destructible;		// 0F0
	BGSPickupPutdownSounds		pickupSounds;		// 100
	TESDescription				description;		// 118
	BGSKeywordForm				keywordForm;		// 130
	TESWeightForm				weight;				// 150

	struct AMMO_DATA
	{
	public:
		// members
		BGSProjectile* projectile;	// 00
		UInt32 health;				// 08
		UInt8 flags;				// 0C
		UInt8 unk0D;				// 0D
		UInt8 unk0E;				// 0E
		UInt8 unk0F;				// 0F
		float damage;				// 10
	};
	STATIC_ASSERT(sizeof(AMMO_DATA) == 0x18);

	AMMO_DATA data;                // 160
	UInt64 shortDesc;  // 178
	TESModel shellCasing;          // 180
};

// 300
class TESObjectWEAP : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_WEAP };

	// 138
	struct InstanceData : public TBO_InstanceData
	{
	public:
		BGSSoundDescriptorForm		* unk10;					// 10 BGSSoundDescriptorForm *
		UInt64						unk18;						// 18
		UInt64						unk20;						// 20
		BGSSoundDescriptorForm		*unk28;						// 28 BGSSoundDescriptorForm *
		BGSSoundDescriptorForm		* unk30;					// 30 BGSSoundDescriptorForm *
		BGSSoundDescriptorForm		* unk38;					// 38 BGSSoundDescriptorForm *
		BGSSoundDescriptorForm		* unk40;					// 40 BGSSoundDescriptorForm * 
		BGSSoundDescriptorForm		* unk48;					// 48 BGSSoundDescriptorForm *
		UInt64						unk50;						// 50
		BGSImpactDataSet			* unk58;					// 58 BGSImpactDataSet*
		TESLevItem					* addAmmoList;				// 60 TESLevItem *
		TESAmmo						* ammo;						// 68 TESAmmo *
		BGSEquipSlot				* equipSlot;				// 70 BGSEquipSlot*
		SpellItem					* unk78;					// 78 SpellItem*
		BGSKeywordForm				* keywords;					// 80
		BGSAimModel					* aimModel;					// 88 BGSAimModel *
		BGSZoomData					* zoomData;					// 90 BGSZoomData*

		struct FiringData
		{
			BGSProjectile	* projectileOverride;	// 00
			float			unk00;					// 08
			float			leftMotorStrength;		// 0C
			float			rightMotorStrength;		// 10
			float			duration;				// 14
			float			unk18;					// 18
			float			unk1C;					// 1C
			float			sightedTransition;		// 20
			UInt32			period;					// 24
			UInt32			unk28;					// 28
			UInt32			numProjectiles;			// 2C
		};

		FiringData					* firingData;				// 98
		tArray<EnchantmentItem*>	* enchantments;				// A0
		tArray<BGSMaterialSwap*>	* materialSwaps;			// A8
		tArray<DamageTypes>			* damageTypes;				// B0
		tArray<ValueModifier>		* modifiers;				// B8
		float						unkC0;						// C0
		float						reloadSpeed;				// C4
		float						speed;						// C8
		float						reach;						// CC
		float						minRange;					// D0
		float						maxRange;					// D4
		float						attackDelay;				// D8
		float						unkD8;						// DC
		float						outOfRangeMultiplier;		// E0
		float						secondary;					// E4
		float						critChargeBonus;			// E8
		float						weight;						// EC
		float						unkEC;						// F0
		float						actionCost;					// F4
		float						fullPowerSeconds;			// F8
		float						minPowerShot;				// FC
		UInt32						unk100;						// 100
		float						critDamageMult;				// 104
		UInt32						stagger;					// 108
		UInt32						value;						// 10C

		enum WeaponFlags
		{
			kFlag_IgnoresNormalResist	= 0x0000002,
			kFlag_MinorCrime			= 0x0000004,
			kFlag_ChargingReload		= 0x0000008,
			kFlag_HideBackpack			= 0x0000010,
			kFlag_NonHostile			= 0x0000040,
			kFlag_NPCsUseAmmo			= 0x0000200,
			kFlag_RepeatableSingleFire	= 0x0000800,
			kFlag_HasScope				= 0x0001000,
			kFlag_HoldInputToPower		= 0x0002000,
			kFlag_Automatic				= 0x0004000,
			kFlag_CantDrop				= 0x0008000,
			kFlag_ChargingAttack		= 0x0010000,
			kFlag_NotUsedInNormalCombat	= 0x0020000,
			kFlag_BoundWeapon			= 0x0040000,
			kFlag_SecondaryWeapon		= 0x0200000,
			kFlag_BoltAction			= 0x0400000,
			kFlag_NoJamAfterReload		= 0x0800000,
			kFlag_DisableShells			= 0x1000000,
		};

		UInt32						flags;						// 110
		UInt32						unk114;						// 114
		UInt32						unk118;						// 118
		UInt32						unk11C;						// 11C
		ActorValueInfo				* skill;					// 120
		ActorValueInfo				* damageResist;				// 128
		UInt16						ammoCapacity;				// 130
		UInt16						baseDamage;					// 132
		UInt16						unk134;						// 134
		UInt8						accuracyBonus;				// 136
		UInt8						unk137;						// 137
	};

	// 150
	struct Data : public InstanceData
	{
	public:
		BGSModelMaterialSwap*	swap138;	// 138
		UInt64	unk140;	// 140
		BGSMod::Attachment::Mod*	embeddedMod;	// 148
	};

	TESFullName					fullName;			// 068
	BGSModelMaterialSwap		materialSwap;		// 078	BGSModelMaterialSwap
	TESIcon						icon;				// 0B8
	TESEnchantableForm			enchantable;		// 0C8 TESEnchantableForm
	BGSDestructibleObjectForm	destructible;		// 0E0 BGSDestructibleObjectForm
	BGSEquipType				equipType;			// 0F0 BGSEquipType
	BGSPreloadable				preloadable;		// 100 BGSPreloadable
	BGSMessageIcon				messageIcon;		// 108 BGSMessageIcon
	BGSPickupPutdownSounds		pickupSounds;		// 120
	BGSBlockBashData			blockBash;			// 128
	BGSKeywordForm				keyword;			// 150
	TESDescription				description;		// 170 TESDescription
	BGSInstanceNamingRulesForm	namingRules;		// 188 BGSInstanceNamingRulesForm
	Data						weapData;			// 198 TESObjectWeap::Data
	BGSAttachParentArray		attachParentArray;	// 2E8 BGSAttachParentArray
};
STATIC_ASSERT(offsetof(TESObjectWEAP, previewTransform) == 0x50);
STATIC_ASSERT(offsetof(TESObjectWEAP, destructible) == 0x0E0);
STATIC_ASSERT(offsetof(TESObjectWEAP::InstanceData, unk114) == 0x114);
STATIC_ASSERT(sizeof(TESObjectWEAP::InstanceData) == 0x138);
STATIC_ASSERT(sizeof(TESObjectWEAP) == 0x300);


// 2E0
class TESObjectARMO : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_ARMO };

	TESFullName					fullName;		// 068
	TESRaceForm					raceForm;		// 078
	TESEnchantableForm			enchantable;	// 088
	BGSDestructibleObjectForm	destructible;	// 0A0
	BGSPickupPutdownSounds		pickupPutdown;	// 0B0
	TESBipedModelForm			bipedModel;		// 0C8
	BGSEquipType				equipType;		// 1D0
	BGSBipedObjectForm			bipedObject;	// 1E0
	BGSBlockBashData			blockBash;		// 1F0
	BGSKeywordForm				keywordForm;	// 208
	TESDescription				description;	// 228
	BGSInstanceNamingRulesForm	namingRules;	// 240
	// 58
	struct InstanceData : public TBO_InstanceData
	{
	public:
		UInt64 unk10;								// 10
		UInt64 unk18;								// 18
		UInt64 unk20;								// 20
		BGSKeywordForm * keywords;					// 28
		tArray<DamageTypes>			* damageTypes;	// 30
		UInt64 unk38;								// 38
		float weight;								// 40
		SInt32 pad44;								// 44
		UInt32 value;								// 48
		UInt32 health;								// 4C
		UInt32 unk50;								// 50
		UInt16 armorRating;							// 54
		UInt16 unk56;								// 56
	};
	InstanceData				instanceData;	// 250 - 2A8 ( 592 - 680)

	struct ArmorAddons
	{
		void			* unk00;		// 00
		TESObjectARMA	* armorAddon;	// 08
	};

	tArray<ArmorAddons>			addons;			// 2A8
	UInt64						unk2C0;			// 2C0
	BGSAttachParentArray		parentArray;	// 2C8

};
STATIC_ASSERT(sizeof(TESObjectARMO::InstanceData) == 0x58);
STATIC_ASSERT(offsetof(TESObjectARMO, parentArray) == 0x2C8);
STATIC_ASSERT(sizeof(TESObjectARMO) == 0x2E0);

// 228
class TESObjectARMA : public TESObject
{
public:
	enum { kTypeID = kFormType_ARMA };

	TESRaceForm					raceForm;			// 20
	BGSBipedObjectForm			bipedObject;		// 30
	UInt64						unk040;				// 40
	UInt64						unk048;				// 48
	BGSModelMaterialSwap		swap50[2];			// 50
	BGSModelMaterialSwap		swapD0[2];			// D0
	BGSModelMaterialSwap		swap150[2];			// 150
	BGSTextureSet*				unk1D0[2];			// 1D0
	BGSListForm*				unk1E0;				// 1E0
	UInt64						unk1E8;				// 1E8
	tArray<TESRace*>			additionalRaces;	// 1F0
	BGSFootstepSet*				footstepSet;		// 208
	BGSArtObject*				art;				// 210
	void*						unk218;				// 218
	void*						unk220;				// 220

	// Constructs a node name from the specified armor and actor
	bool GetNodeName(char * dest, TESNPC * refr, TESObjectARMO * armor);
};
STATIC_ASSERT(offsetof(TESObjectARMA, unk220) == 0x220);
STATIC_ASSERT(sizeof(TESObjectARMA) == 0x228);


// 350
class BGSTextureSet : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_TXST };

	BSTextureSet textureSet; // 68

	void		* unk78;				// 78
	TESTexture	texture[8];				// 80
	UInt64		unk100[(0x350-0x100)/8]; // 100
};
STATIC_ASSERT(sizeof(BGSTextureSet) == 0x350);


class MagicItem : public TESBoundObject
{
public:
	TESFullName		name;			// 68
	BGSKeywordForm	keywordForm;	// 78
	UnkArray		effectItemsProbably; // 98
	UInt64			unk0B0[4];		// B0
};
STATIC_ASSERT(offsetof(MagicItem, unk0B0) == 0x0B0);
STATIC_ASSERT(sizeof(MagicItem) == 0x0D0);

// 1E0
class AlchemyItem : public MagicItem
{
public:
	enum { kTypeID = kFormType_ALCH };

	BGSModelMaterialSwap	materialSwap; // 0D0
	TESIcon					icon;			// 110
	BGSMessageIcon			msgIcon;		// 120
	TESWeightForm			weightForm;		// 138
	BGSEquipType			equipType;		// 148
	BGSDestructibleObjectForm destructible;	// 158
	BGSPickupPutdownSounds	pickupPutdown;	// 168
	BGSCraftingUseSound		craftingSounds;	// 180
	TESDescription			description;	// 190
	UInt32					unk1A8;			// 1A8
	UInt32					unk1AC;			// 1AC
	UInt64					unk1B0[4];		// 1B0
	TESIcon					icon1D0;		// 1D0
};
STATIC_ASSERT(offsetof(AlchemyItem, icon1D0) == 0x1D0);
STATIC_ASSERT(sizeof(AlchemyItem) == 0x1E0);

// 100
class EnchantmentItem : public MagicItem
{
public:
	enum { kTypeID = kFormType_ENCH };

	UInt64					unk0D0[6];
};
STATIC_ASSERT(offsetof(EnchantmentItem, unk0D0) == 0x0D0);
STATIC_ASSERT(sizeof(EnchantmentItem) == 0x100);


class SpellItem : public MagicItem
{
public:
	enum { kTypeID = kFormType_SPEL };

	BGSEquipType				equipType;		// 0D0
	BGSMenuDisplayObject		displayObject;	// 0E0
	TESDescription				description;	// 0F0
	UInt64						unk108[5];		// 108
};
STATIC_ASSERT(offsetof(SpellItem, description) == 0x0F0);
STATIC_ASSERT(offsetof(SpellItem, unk108) == 0x108);
STATIC_ASSERT(sizeof(SpellItem) == 0x130);

// Seemingly unused in the game. No entries in the DataHandler
// 1B8
class ScrollItem : public SpellItem
{
public:
	enum { kTypeID = kFormType_SCRL };

	BGSModelMaterialSwap		materialSwap;	// 130
	BGSDestructibleObjectForm	destructible;
	BGSPickupPutdownSounds		pickupPutdown;
	TESWeightForm				weight;
	TESValueForm				value;
};

// 188 - only one it seems (Copper Pipe)
class IngredientItem : public MagicItem
{
public:
	enum { kTypeID = kFormType_INGR };

	BGSModelMaterialSwap		materialSwap;	// 0D0
	TESIcon						icon;			// 110
	TESWeightForm				weight;			// 120
	BGSEquipType				equipType;		// 130
	BGSDestructibleObjectForm	destructible;	// 140
	BGSPickupPutdownSounds		pickupPutdown;	// 150
	TESValueForm				value;			// 168
	UInt64						unk178;			// 178
	UInt64						unk180;			// 180
};
STATIC_ASSERT(offsetof(IngredientItem, unk180) == 0x180);
STATIC_ASSERT(sizeof(IngredientItem) == 0x188);


// 170
class TESObjectCONT : public TESBoundAnimObject
{
public:
	enum { kTypeID = kFormType_CONT };

	TESContainer				container;			// 68
	TESFullName					fullName;			// 80
	BGSModelMaterialSwap		materialSwap;		// 90
	TESWeightForm				weightForm;			// D0
//	TESMagicCasterForm			magicCaster;
//	TESMagicTargetForm			magicTarget;
	BGSDestructibleObjectForm	destructible;		// E0
	BGSOpenCloseForm			openClose;			// F0
	BGSKeywordForm				keywordForm;		// F8
	BGSForcedLocRefType			forcedLockRefType;	// 118
	BGSPropertySheet			propertySheet;		// 128
	BGSNativeTerminalForm		nativeTerminal;		// 138
	UInt8						unk148;
	UInt8						unk149;
	UInt8						pad14A[2];
	BGSSoundDescriptorForm*		sound150;
	BGSSoundDescriptorForm*		sound158;
	BGSSoundDescriptorForm*		sound160;
	UInt64						unk168;
};
STATIC_ASSERT(offsetof(TESObjectCONT, pad14A) == 0x14A);
STATIC_ASSERT(sizeof(TESObjectCONT) == 0x170);


// 140
class TESObjectACTI : public TESBoundAnimObject
{
public:
	enum { kTypeID = kFormType_ACTI };

	TESFullName					fullName;			// 68
	BGSModelMaterialSwap		materialSwap;		// 78
	// TESMagicTargetForm		magicTarget;
	BGSDestructibleObjectForm	destructible;		// B8
	BGSOpenCloseForm			openClose;			// C8
	BGSKeywordForm				keywordForm;		// D0
	BGSPropertySheet			propertySheet;		// F0
	BGSForcedLocRefType			forcedLocRefType;	// 100
	BGSNativeTerminalForm		nativeTerminalForm;	// 110
	UInt64						unk120;				// 120
	UInt64						unk128;				// 128
	UInt64						unk130;
	UInt8						unk138;
	UInt8						pad13C[3];		
};
STATIC_ASSERT(sizeof(TESObjectACTI) == 0x140);

// 1A0
class TESFurniture : public TESObjectACTI
{
public:
	enum { kTypeID = kFormType_FURN };

//	BGSNavmeshableObject		navMeshableObject;
	UInt64						unk140;				// 140

	struct Data {
		UInt32 unk00;
		UInt32 unk04;
	};

	tArray<Data>				arr148;
	//UInt64						unk148;
	//UInt32						unk150;
	//UInt32						unk154;
	//UInt32						unk158;
	//UInt32						unk15C;
	BGSAttachParentArray		unk160;
	UInt8						unk178;
	float						unk17C;
	UInt64						unk180;

	struct Data2
	{
		UInt64 unk00[4];
	};

	tArray<Data2>				arr188; // struct { UInt64 data[4]; }
	//void*						unk188;
	//UInt32						unk190;
	//UInt32						unk194;
	//UInt32						unk198;
	//UInt32						unk19C;
};
STATIC_ASSERT(offsetof(TESFurniture, arr188) == 0x188);
STATIC_ASSERT(sizeof(TESFurniture) == 0x1A0);

// 1E0
class BGSTerminal : public TESFurniture
{
public:
	enum { kTypeID = kFormType_TERM };

	UnkArray					arr1A0;
	//void*						unk1A0;	/// 1A0
	//UInt32						unk1A8;
	//UInt32						unk1AC;
	//UInt32						unk1B0;
	//UInt32						unk1B4;
	UnkArray					arr1B8;
	//void*						unk1B8;
	//UInt32						unk1C0;
	//UInt32						unk1C4;
	//UInt32						unk1C8;
	//UInt32						unk1CC;
	void*						unk1D0;
	void*						unk1D8;
};
STATIC_ASSERT(offsetof(BGSTerminal, arr1B8) == 0x1B8);
STATIC_ASSERT(sizeof(BGSTerminal) == 0x1E0);


// 90
class BGSBendableSpline : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_BNDS };

	UInt64 unk68[(0x90 - 0x68) >> 3];
};
STATIC_ASSERT(sizeof(BGSBendableSpline) == 0x90);

// A8
class BGSComponent : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_CMPO };

	// parents
	TESFullName					fullName;			// 68
	TESValueForm				value;				// 78
	BGSCraftingUseSound			craftingSounds;		// 88

	TESObjectMISC	* scrapItem;	// 98
	TESGlobal		* scrapScalar;	// A0
};
STATIC_ASSERT(sizeof(BGSComponent) == 0xA8);

// 168
class TESObjectMISC : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_MISC };

	// parents
	TESFullName					fullName;			// 68
	BGSModelMaterialSwap		materialSwap;		// 78
	TESIcon						icon;				// B8
	TESValueForm				value;				// C8
	TESWeightForm				weight;				// D8
	BGSDestructibleObjectForm	destructible;		// E8
	BGSMessageIcon				messageIcon;		// F8
	BGSPickupPutdownSounds		pickupPutdown;		// 110
	BGSKeywordForm				keywordForm;		// 128
	BGSFeaturedItemMessage		featuredMessage;	// 148

	struct Component
	{
		BGSComponent	* component;
		UInt64			count;
	};

	tArray<Component>	* components;	// 158
	UInt64	unk160;	// 160
};
STATIC_ASSERT(sizeof(TESObjectMISC) == 0x168);

// 188
class BGSProjectile : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_PROJ };

	TESFullName					fullName;			// 68
	TESModel					model;				// 78
	BGSPreloadable				preloadable;		// A8
	BGSDestructibleObjectForm	destructible;		// B0

	struct BGSProjectileData
	{
		UInt32					flags;					// 00
		float					gravity;				// 04
		float					speed;					// 08
		float					range;					// 0C
		TESObjectLIGH*			light;					// 10
		TESObjectLIGH*			muzzleFlashLight;		// 18
		float					explosionProximity;		// 20
		float					explosionTimer;			// 24
		BGSExplosion*			explosionType;			// 28
		BGSSoundDescriptorForm*	activeSoundLoop;		// 30
		float					muzzleFlashDuration;	// 38
		float					fadeOutTime;			// 3C
		float					force;					// 40
		BGSSoundDescriptorForm*	countdownSound;			// 48
		BGSSoundDescriptorForm*	deactivateSound;		// 50
		TESObjectWEAP*			defaultWeaponSource;	// 58
		float					coneSpread;				// 60
		float					collisionRadius;		// 64
		float					lifetime;				// 68
		float					relaunchInterval;		// 6C
		BGSTextureSet*			decalData;				// 70
		BGSCollisionLayer*		collisionLayer;			// 78
		BGSProjectile*			VATSProjectile;			// 80
		SInt8					tracerFrequency;		// 88
	};

	BGSProjectileData			projectileData;			// C0
	TESModel					muzzleFlashModel;		// 150
	UInt32						soundLevel;				// 180
};
STATIC_ASSERT(sizeof(BGSProjectile) == 0x188);
STATIC_ASSERT(sizeof(BGSProjectile::BGSProjectileData) == 0x90);

// D8
class TESLevCharacter : public TESBoundAnimObject
{
public:
	enum { kTypeID = kFormType_LVLN };

	TESLeveledList			leveledList;	// 68
	BGSModelMaterialSwap	matSwap;		// 98
};
STATIC_ASSERT(sizeof(TESLevCharacter) == 0xD8);

// 98
class TESLevItem : public TESBoundObject
{
public:
	enum { kTypeID = kFormType_LVLI };

	TESLeveledList          leveledList;    // 68
};
STATIC_ASSERT(sizeof(TESLevItem) == 0x98);
