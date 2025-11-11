#pragma once

#include "f4se_common/Relocation.h"
#include "f4se/GameFormComponents.h"
#include "f4se/GameCustomization.h"
#include "f4se/GameUtilities.h"

#include <functional>

struct ModInfo;
class TESForm;
class TESNPC;
class BGSColorForm;
class BGSHeadPart;
class BSFile;
class TESGlobal;
class ActorValueInfo;
class TESEffectShader;
class BGSProjectile;
class BGSPerk;
class BGSExplosion;
class BGSArtObject;
class TESImageSpaceModifier;
class TESObjectLIGH;
class BGSEncounterZone;
class BGSMusicType;
class IFormFactory;
class TESObjectMISC;
class TESImageSpace;
class SpellItem;
class TESObjectLAND;
class TESWorldSpace;
class bhkWorld;
class BGSComponent;

typedef TESForm* (* _LookupFormByID)(UInt32 id);
extern RelocAddr <_LookupFormByID> LookupFormByID;
extern RelocPtr <IFormFactory*> g_formFactoryList;

/**** form types ***************************************************************
*	
*
*	Code	id	name
	NONE	0
	TES4	1
	GRUP	2
	GMST	3
	KYWD	4	BGSKeyword
	LCRT	5	BGSLocationRefType
	AACT	6	BGSAction
	TRNS	7
	CMPO	8
	TXST	9	BGSTextureSet
	MICN	10	BGSMenuIcon
	GLOB	11	TESGlobal
	DMGT	12
	CLAS	13	TESClass
	FACT	14	TESFaction
	HDPT	15	BGSHeadPart
	EYES	16	TESEyes
	RACE	17	TESRace
	SOUN	18	TESSound
	ASPC	19	BGSAcousticSpace
	SKIL	20
	MGEF	21	EffectSetting
	SCPT	22	Script
	LTEX	23	TESLandTexture
	ENCH	24	EnchantmentItem
	SPEL	25	SpellItem
	SCRL	26	ScrollItem
	ACTI	27	TESObjectACTI
	TACT	28	BGSTalkingActivator
	ARMO	29	TESObjectARMO
	BOOK	30	TESObjectBOOK
	CONT	31	TESObjectCONT
	DOOR	32	TESObjectDOOR
	INGR	33	IngredientItem
	LIGH	34	TESObjectLIGH
	MISC	35	TESObjectMISC
	STAT	36	TESObjectSTAT
	SCOL	37	BGSStaticCollection
	MSTT	38	BGSMovableStatic
	GRAS	39	TESGrass
	TREE	40	TESObjectTREE
	FLOR	41	TESFlora
	FURN	42	TESFurniture
	WEAP	43	TESObjectWEAP
	AMMO	44	TESAmmo
	NPC_	45	TESNPC
	LVLN	46	TESLevCharacter
	KEYM	47	TESKey
	ALCH	48	AlchemyItem
	IDLM	49	BGSIdleMarker
	NOTE	50	BGSNote
	PROJ	51	BGSProjectile
	HAZD	52	BGSHazard
	BNDS	53
	SLGM	54	TESSoulGem
	TERM	55	BGSTerminal
	LVLI	56	TESLevItem
	WTHR	57	TESWeather
	CLMT	58	TESClimate
	SPGD	59	BGSShaderParticleGeometryData
	RFCT	60	BGSReferenceEffect
	REGN	61	TESRegion
	NAVI	62	NavMeshInfoMap
	CELL	63	TESObjectCELL
	REFR	64	TESObjectREFR / Actor
	ACHR	65	Character / PlayerCharacter
	PMIS	66	MissileProjectile
	PARW	67	ArrowProjectile
	PGRE	68	GrenadeProjectile
	PBEA	69	BeamProjectile
	PFLA	70	FlameProjectile
	PCON	71	ConeProjectile
	PBAR	72	BarrierProjectile
	PHZD	73	Hazard
	WRLD	74	TESWorldSpace
	LAND	75	TESObjectLAND
	NAVM	76	NavMesh
	TLOD	77
	DIAL	78	TESTopic
	INFO	79	TESTopicInfo
	QUST	80	TESQuest
	IDLE	81	TESIdleForm
	PACK	82	TESPackage
	CSTY	83	TESCombatStyle
	LSCR	84	TESLoadScreen
	LVSP	85	TESLevSpell
	ANIO	86	TESObjectANIO
	WATR	87	TESWaterForm
	EFSH	88	TESEffectShader
	TOFT	89
	EXPL	90	BGSExplosion
	DEBR	91	BGSDebris
	IMGS	92	TESImageSpace
	IMAD	93	TESImageSpaceModifier
	FLST	94	BGSListForm
	PERK	95	BGSPerk
	BPTD	96	BGSBodyPartData
	ADDN	97	BGSAddonNode
	AVIF	98	ActorValueInfo
	CAMS	99	BGSCameraShot
	CPTH	100	BGSCameraPath
	VTYP	101	BGSVoiceType
	MATT	102	BGSMaterialType
	IPCT	103	BGSImpactData
	IPDS	104	BGSImpactDataSet
	ARMA	105	TESObjectARMA
	ECZN	106	BGSEncounterZone
	LCTN	107	BGSLocation
	MESG	108	BGSMessage
	RGDL	109	BGSRagdoll
	DOBJ	110
	DFOB	111
	LGTM	112	BGSLightingTemplate
	MUSC	113	BGSMusicType
	FSTP	114	BGSFootstep
	FSTS	115	BGSFootstepSet
	SMBN	116	BGSStoryManagerBranchNode
	SMQN	117	BGSStoryManagerQuestNode
	SMEN	118	BGSStoryManagerEventNode
	DLBR	119	BGSDialogueBranch
	MUST	120	BGSMusicTrackFormWrapper
	DLVW	121
	WOOP	122	TESWordOfPower
	SHOU	123	TESShout
	EQUP	124	BGSEquipSlot
	RELA	125	BGSRelationship
	SCEN	126	BGSScene
	ASTP	127	BGSAssociationType
	OTFT	128	BGSOutfit
	ARTO	129	BGSArtObject
	MATO	130	BGSMaterialObject
	MOVT	131	BGSMovementType
	SNDR	132	BGSSoundDescriptorForm
	DUAL	133	BGSDualCastData
	SNCT	134	BGSSoundCategory
	SOPM	135	BGSSoundOutput
	COLL	136	BGSCollisionLayer
	CLFM	137	BGSColorForm
	REVB	138	BGSReverbParameters
	PKIN	139
	RFGP	140
	AMDL	141
	LAYR	142
	COBJ	143
	OMOD	144 BGSMod::Attachment::Mod
	MSWP	145 BGSMaterialSwap
	ZOOM	146
	INNR	147
	KSSM	148
	AECH	149
	SCCO	150
	AORU	151
	SCSN	152
	STAG	153
	NOCM	154
	LENS	155
	LSPR	156
	GDRY	157
	OVIS	158

	// Special types
	160
	161
	162
	163
	164
	165
*/

enum FormType
{
	kFormType_NONE = 0,
	kFormType_TES4,
	kFormType_GRUP,
	kFormType_GMST,
	kFormType_KYWD,
	kFormType_LCRT,
	kFormType_AACT,
	kFormType_TRNS,
	kFormType_CMPO,
	kFormType_TXST,
	kFormType_MICN,
	kFormType_GLOB,
	kFormType_DMGT,
	kFormType_CLAS,
	kFormType_FACT,
	kFormType_HDPT,
	kFormType_EYES,
	kFormType_RACE,
	kFormType_SOUN,
	kFormType_ASPC,
	kFormType_SKIL,
	kFormType_MGEF,
	kFormType_SCPT,
	kFormType_LTEX,
	kFormType_ENCH,
	kFormType_SPEL,
	kFormType_SCRL,
	kFormType_ACTI,
	kFormType_TACT,
	kFormType_ARMO,
	kFormType_BOOK,
	kFormType_CONT,
	kFormType_DOOR,
	kFormType_INGR,
	kFormType_LIGH,
	kFormType_MISC,
	kFormType_STAT,
	kFormType_SCOL,
	kFormType_MSTT,
	kFormType_GRAS,
	kFormType_TREE,
	kFormType_FLOR,
	kFormType_FURN,
	kFormType_WEAP,
	kFormType_AMMO,
	kFormType_NPC_,
	kFormType_LVLN,
	kFormType_KEYM,
	kFormType_ALCH,
	kFormType_IDLM,
	kFormType_NOTE,
	kFormType_PROJ,
	kFormType_HAZD,
	kFormType_BNDS,
	kFormType_SLGM,
	kFormType_TERM,
	kFormType_LVLI,
	kFormType_WTHR,
	kFormType_CLMT,
	kFormType_SPGD,
	kFormType_RFCT,
	kFormType_REGN,
	kFormType_NAVI,
	kFormType_CELL,
	kFormType_REFR,
	kFormType_ACHR,
	kFormType_PMIS,
	kFormType_PARW,
	kFormType_PGRE,
	kFormType_PBEA,
	kFormType_PFLA,
	kFormType_PCON,
	kFormType_PBAR,
	kFormType_PHZD,
	kFormType_WRLD,
	kFormType_LAND,
	kFormType_NAVM,
	kFormType_TLOD,
	kFormType_DIAL,
	kFormType_INFO,
	kFormType_QUST,
	kFormType_IDLE,
	kFormType_PACK,
	kFormType_CSTY,
	kFormType_LSCR,
	kFormType_LVSP,
	kFormType_ANIO,
	kFormType_WATR,
	kFormType_EFSH,
	kFormType_TOFT,
	kFormType_EXPL,
	kFormType_DEBR,
	kFormType_IMGS,
	kFormType_IMAD,
	kFormType_FLST,
	kFormType_PERK,
	kFormType_BPTD,
	kFormType_ADDN,
	kFormType_AVIF,
	kFormType_CAMS,
	kFormType_CPTH,
	kFormType_VTYP,
	kFormType_MATT,
	kFormType_IPCT,
	kFormType_IPDS,
	kFormType_ARMA,
	kFormType_ECZN,
	kFormType_LCTN,
	kFormType_MESG,
	kFormType_RGDL,
	kFormType_DOBJ,
	kFormType_DFOB,
	kFormType_LGTM,
	kFormType_MUSC,
	kFormType_FSTP,
	kFormType_FSTS,
	kFormType_SMBN,
	kFormType_SMQN,
	kFormType_SMEN,
	kFormType_DLBR,
	kFormType_MUST,
	kFormType_DLVW,
	kFormType_WOOP,
	kFormType_SHOU,
	kFormType_EQUP,
	kFormType_RELA,
	kFormType_SCEN,
	kFormType_ASTP,
	kFormType_OTFT,
	kFormType_ARTO,
	kFormType_MATO,
	kFormType_MOVT,
	kFormType_SNDR,
	kFormType_DUAL,
	kFormType_SNCT,
	kFormType_SOPM,
	kFormType_COLL,
	kFormType_CLFM,
	kFormType_REVB,
	kFormType_PKIN,
	kFormType_RFGP,
	kFormType_AMDL,
	kFormType_LAYR,
	kFormType_COBJ,
	kFormType_OMOD,
	kFormType_MSWP,
	kFormType_ZOOM,
	kFormType_INNR,
	kFormType_KSSM,
	kFormType_AECH,
	kFormType_SCCO,
	kFormType_AORU,
	kFormType_SCSN,
	kFormType_STAG,
	kFormType_NOCM,
	kFormType_LENS,
	kFormType_LSPR,
	kFormType_GDRY,
	kFormType_OVIS,
	kFormType_Max,

	// These are special types for Papyrus
	kFormType_Alias,
	kFormType_ReferenceAlias,
	kFormType_LocationAlias,
	kFormType_ActiveMagicEffect,
	kFormType_InputEnableLayer,
	kFormType_RefCollectionAlias,
	kFormType_ScriptObject = -1,
};

// 20
class TESForm : public BaseFormComponent
{
public:
	enum { kTypeID = 0 };	// special-case

	virtual void	Unk_07();
	virtual void	Unk_08();
	virtual void	Unk_09();
	virtual void	Unk_0A();
	virtual void	Unk_0B();
	virtual void	Unk_0C();
	virtual bool	MarkChanged(UInt64 changed);
	virtual void	Unk_0E();
	virtual void	Unk_0F();
	virtual void	Unk_10();
	virtual void	Unk_11(); // Serialize
	virtual void	Unk_12();
	virtual void	Unk_13();
	virtual void	Unk_14();
	virtual void	Unk_15();
	virtual void	Unk_16();
	virtual ModInfo*  GetLastModifiedMod(); // 17 - Returns the ModInfo* of the mod that last modified the form.
	virtual ModInfo*  GetLastModifiedMod_2(); // 18 - Returns the ModInfo* of the mod that last modified the form. Calls a helper function to do so.
	virtual UInt8     GetFormType();  // 19
	virtual void      Unk_1A(); // 1A - GetDebugName(TESForm * form, char * destBuffer, unsigned int bufferSize);
	virtual bool      GetPlayerKnows(); // 1B - Gets flag bit 6.
	virtual void	Unk_1C();
	virtual void	Unk_1D();
	virtual void	Unk_1E();
	virtual void	Unk_1F();
	virtual void	Unk_20();
	virtual void	Unk_21();
	virtual void	Unk_22();
	virtual void	Unk_23();
	virtual void	Unk_24();
	virtual void	Unk_25();
	virtual void	Unk_26();
	virtual void	Unk_27();
	virtual void	Unk_28();
	virtual void	Unk_29();
	virtual void	Unk_2A();
	virtual void	Unk_2B();
	virtual void	Unk_2C();
	virtual void	Unk_2D();
	virtual void	Unk_2E();
	virtual void	Unk_2F();
	virtual void	Unk_30();
	virtual void	Unk_31();
	virtual void	Unk_32();
	virtual void	Unk_33();
	virtual void	Unk_34();
	virtual const char *  GetFullName();  // 35
	virtual void	Unk_36();
	virtual void	Unk_37();
	virtual void	Unk_38();
	virtual void	Unk_39();
	virtual const char *	GetEditorID(); // Only returns string for things that actually store the EDID ingame
	virtual void	Unk_3B();
	virtual void	Unk_3C();
	virtual void	Unk_3D();
	virtual void	Unk_3E();
	virtual void	Unk_3F();
	virtual void	Unk_40();
	virtual void	Unk_41();
	virtual void	Unk_42();
	virtual void	Unk_43();
	virtual void	Unk_44();
	virtual void	Unk_45();
	virtual void	Unk_46();
	virtual void	Unk_47();

	enum {
		kFlag_IsDeleted     = 1 << 5,
		kFlag_PlayerKnows   = 1 << 6,
		kFlag_Persistent    = 1 << 10,
		kFlag_IsDisabled    = 1 << 11,
		kFlag_NoHavok       = 1 << 29,
	};

	struct Data
	{
		ModInfo ** entries;     // array of ModInfo* - mods that change this form.
		UInt64  size;
	};

	Data	* unk08;	// 08
	UInt32	flags;		// 10
	UInt32	formID;		// 14
	UInt16	unk18;		// 18
	UInt8	formType;	// 1A
	UInt8	unk1B;		// 1B
	UInt32	pad1C;		// 1C
};

// 38
class BGSDefaultObject : public TESForm
{
public:
	enum { kTypeID = kFormType_DFOB };

	TESForm			* form;		// 20
	UInt64			unk28;		// 28
	BSFixedString	editorId;	// 30
};

// 118
class BGSInstanceNamingRules : public TESForm
{
public:
	// 30
	struct KeywordData
	{
		void			* unk00;		// 00
		BGSKeywordForm	keywordForm;	// 08
		UInt64			unk28;			// 28
	};
	UInt64				unk20;	// 20
	tArray<KeywordData>	unk28;	// 28
	tArray<KeywordData>	unk40;	// 40
	tArray<KeywordData> unk58;	// 58
	tArray<KeywordData>	unk70;	// 70
	tArray<KeywordData>	unk88;	// 88
	UInt64	unkA0[(0x118 - 0xA0) / 8];	// A0
};
STATIC_ASSERT(offsetof(BGSInstanceNamingRules, unk88) == 0x88);
STATIC_ASSERT(sizeof(BGSInstanceNamingRules) == 0x118);

class BGSKeyword : public TESForm
{
public:
	enum { kTypeID = kFormType_KYWD };

	BSFixedString	keyword;	// 020
};

// 30
class BGSSoundDescriptorForm : public TESForm
{
public:
	enum { kTypeID = kFormType_SNDR };

	BGSSoundDescriptor	soundDescriptor;	// 20
	UInt64	unk28;							// 28
};

class BGSAction : public BGSKeyword
{
public:
	enum { kTypeID = kFormType_AACT };

	UInt64			unk28; // maybe index or id?
//Actions dump 1.1.30:
// unk28					Keyword
// -----                    --------
//  0                      ActionTrick
//  1                  ActionFireEmpty
//  2               ActionWeaponHotkey
//  3                ActionFlinchStart
//  4                ActionPerkSandman
//  5               ActionPerkCannibal
//  6                   ActionGunAlert
//  7                       ActionHide
//  8           ActionCameraBToCameraA
//  9           ActionCameraAToCameraB
// 10                   ActionSwimStop
// 11                  ActionSwimStart
// 12    ActionAvailableCondition1Heal
// 13               ActionRightArmHeal
// 14                ActionLeftArmHeal
// 15         ActionInstantAttackReset
// 16                ActionJetpackStop
// 17               ActionJetpackStart
// 18            ActionUnconsciousExit
// 19           ActionUnconsciousEnter
// 20             ActionFireChargeHold
// 21             ActionCustomLaughing
// 22               ActionPropellersOn
// 23              ActionPropellersOff
// 24               ActionCustomBooing
// 25             ActionCustomCheering
// 26                   ActionGunReady
// 27               ActionDeferredKill
// 28          ActionNonSupportContact
// 29                       ActionBark
// 30   ActionEnterDialogueCameraState
// 31             ActionGunChargeStart
// 32                ActionIdlePlayful
// 33           ActionChangeAnimFlavor
// 34        ActionChangeAnimArchetype
// 35                 ActionBoltCharge
// 36               ActionAttackMissed
// 37                     ActionTunnel
// 38         ActionInteractionExitAlt
// 39             ActionLeadingArrival
// 40        ActionLeadingArrivalEmote
// 41           ActionLeadingDoneEmote
// 42           ActionLeadingDeparture
// 43                 ActionEscortWait
// 44                 ActionIdleFlavor
// 45 ActionInstantInitializeGraphToBaseState
// 46                   ActionLegsHeal
// 47                      ActionPanic
// 48                      ActionCower
// 49                  ActionAOEAttack
// 50              ActionPipboyInspect
// 51                    ActionShuffle
// 52                      ActionDodge
// 53                      ActionEvade
// 54               ActionStumbleStart
// 55                 ActionGunRelaxed
// 56                   ActionTurnStop
// 57                   ActionTurnLeft
// 58                  ActionTurnRight
// 59                   ActionMoveStop
// 60                  ActionMoveStart
// 61                       ActionLand
// 62                       ActionFall
// 63         ActionLargeMovementDelta
// 64                  ActionDeathWait
// 65                ActionDualRelease
// 66                 ActionDualAttack
// 67                 ActionStopEffect
// 68                     ActionListen
// 69                    ActionTalking
// 70                      ActionDeath
// 71              ActionSummonedStart
// 72                 ActionBumpedInto
// 73                  ActionHoverStop
// 74                 ActionHoverStart
// 75                    ActionFlyStop
// 76                   ActionFlyStart
// 77                ActionSprintStart
// 78                    ActionPathEnd
// 79                  ActionPathStart
// 80                 ActionForceEquip
// 81            ActionBlockAnticipate
// 82                   ActionIdleStop
// 83               ActionBleedoutStop
// 84              ActionBleedoutStart
// 85                ActionRecoilLarge
// 86                     ActionRecoil
// 87                   ActionBlockHit
// 88               ActionStaggerStart
// 89             ActionVoiceInterrupt
// 90                 ActionVoiceReady
// 91               ActionVoiceRelease
// 92             ActionRightInterrupt
// 93                 ActionRightReady
// 94               ActionRightRelease
// 95              ActionLeftInterrupt
// 96                  ActionLeftReady
// 97                ActionLeftRelease
// 98           ActionRightPowerAttack
// 99                       ActionDraw
//100                 ActionSprintStop
//101                       ActionLook
//102                   ActionActivate
//103                      ActionVoice
//104                      ActionSneak
//105                       ActionJump
//106                ActionRightAttack
//107                 ActionLeftAttack
//108            ActionSwimStateChange
//109                       ActionIdle
//110                    ActionWardHit
//111                  ActionPipboyTab
//112                  ActionPipboyMap
//113            ActionPipboyInventory
//114                 ActionPipboyData
//115                ActionPipboyStats
//116                 ActionPipboyZoom
//117                  ActionExitCover
//118                 ActionEnterCover
//119                 ActionPipboyOpen
//120                      ActionThrow
//121                   ActionFireAuto
//122                 ActionFireCharge
//123                 ActionFireSingle
//124                      ActionMelee
//125             ActionSightedRelease
//126                    ActionSighted
//127                     ActionReload
//128             ActionRagdollInstant
//129            ActionIdleStopInstant
//130                     ActionSheath
//131                      ActionGetUp
//132                  ActionKnockDown
//133               ActionShieldChange
//134                  ActionMoveRight
//135                   ActionMoveLeft
//136               ActionMoveBackward
//137                ActionMoveForward
//138            ActionDualPowerAttack
//139            ActionLeftPowerAttack
//140                   ActionIdleWarn
//141                    ActionGunDown
//142              ActionListenNeutral
//143             ActionListenQuestion
//144             ActionListenNegative
//145             ActionListenPositive
//146                 ActionIntimidate
//147           ActionInteractionEnter
//148    ActionInteractionEnterInstant
//149            ActionInteractionExit
//150       ActionInteractionExitQuick
//151                ActionPipboyClose
//152             ActionPipboyRadioOff
//153              ActionPipboyRadioOn
//154               ActionPipboySelect
//155          ActionPipboyTabPrevious
//156           ActionCoverSprintStart
//157         ActionActivateLoopingEnd
//158                     ActionMantle
//159             ActionLeftSyncAttack
//160            ActionRightSyncAttack
//161                  ActionFlipThrow
//162               ActionDialogueExit
//163              ActionDialogueEnter
//164         ActionPipboyLoadHolotape
//165               ActionLimbCritical
//166                 ActionCombatExit
//167                ActionCombatEnter
//168       ActionActivateLoopingBegin
//169              ActionFurnitureFull
//170 ActionInitializeGraphToBaseState
//171      ActionFurnitureNoLongerFull
//172               ActionLegsCritical
};

// 190
class ActorValueInfo : public TESForm
{
public:
	enum { kTypeID = kFormType_AVIF };

	TESFullName       fullName;             // 20
	TESDescription    description;          // 30

#if _MSC_VER == 1700
	std::function<void(Actor *, ActorValueInfo&, float, float, Actor *)> calcFunction;	// 48
#else
	void            * func_vtable;			// 48 - vtable of the lambda function
	void            * func;					// 50
	UInt64            unk58;				// 58
	void            * func_ptr;				// 60 - address of offset 48
#endif

	const char      * avName;               // 68
	UInt64            unk70;                // 70
	ActorValueInfo  * dependentAVs[0xF];    // 78

#if _MSC_VER == 1700
	std::function<float(ActorValueOwner*, ActorValueInfo&)> derivedFunction;	// F0
#else
	void            * derived_func_vtable;	// F0 - vtable of the lambda function
	void            * derived_func;			// F8
	UInt64            unk100;				// 100
	void            * derived_func_ptr;		// 108 - address of offset F0
#endif

	UInt32    unk110[(0x16C - 0x110) / 4];  // 110

	UInt32    avFlags;                      // 16C
	enum AVFlags
	{
		kFlag_DefaultBase_0             = (1 << 10),    // 10 | Default Base: 0
		kFlag_DefaultBase_1             = (1 << 11),    // 11 | Default Base: 1
		kFlag_DefaultBase_100           = (1 << 12),    // 12 | Default Base: 100
		kFlag_DefaultBase_UserDefined   = (1 << 13),    // 13 | Default Base: User Defined
		kFlag_DefaultBase_Derived       = (1 << 15),    // 15 | Default Base: Derived (bits 10-13 must not be set)
		kFlag_DamageIsPositive          = (1 << 26),    // 26 | Damage is Positive
		kFlag_GodModeImmune             = (1 << 27),    // 27 | God Mode Immune
		kFlag_Hardcoded                 = (1 << 31)     // 31 | Hardcoded
	};

	UInt32    unk170;                       // 170
	UInt32    numDependentAVs;              // 174
	UInt32    unk178;                       // 178
	UInt32    unk17C;                       // 17C
	UInt32    unk180;                       // 180
	float     defaultBase;                  // 184
	UInt32    unk188;                       // 188
	UInt32    unk18C;                       // 18C
};
STATIC_ASSERT(offsetof(ActorValueInfo, avName) == 0x68);

// 80
class BGSMaterialType : public TESForm
{
public:
	enum { kTypeID = kFormType_MATT };

	UInt64	unk20[(0x80-0x20)/8];	// 20
};

// 38
class BGSColorForm : public TESForm
{
public:
	enum { kTypeID = kFormType_CLFM };

	TESFullName			fullName;		// 20
	union
	{
		struct
		{
			UInt8 r;
			UInt8 g;
			UInt8 b;
			UInt8 unknown;
		} channels;
		UInt32	rgb;
		float	remappingIndex;
	} color;							// 30
	UInt32		unk34;					// 34

	struct Data
	{
		Data		* next;				// 00
		float		unk08;				// 08
		float		unk0C;				// 0C
		float		unk10;				// 10
		SInt32		unk14;				// 14 - 0xFFFFFFFF
		UInt32		unk18;				// 18 
		UInt32		unk1C;				// 1C
	};

	Data				* unk38;		// 38
	enum Flags
	{
		kFlags_Playable = 1,
		kFlags_RemappableIndex = 2,
		kFlags_ExtendedLUT = 4
	};
	UInt32				flags;			// 40
	UInt32				unk44;			// 44
};

// 6C8
class TESRace : public TESForm
{
public:
	enum { kTypeID = kFormType_RACE };

	TESFullName			fullName;		// 20
	TESDescription		description;	// 30
	TESSpellList		spellList;		// 48
	BGSSkinForm			skinForm;		// 58
	BGSBipedObjectForm	bipedObjects;	// 68
	BGSKeywordForm		keywordForm;	// 78
	BGSAttackDataForm	attackDataForm;	// 98
	BGSPropertySheet	propertySheet;	// A8
	BGSPreloadable		preloadable;	// B8

	TESModel			models[2];		 // C0
	TESModel			unk120[2];		 // 120
	UInt64				unk170[(0x288-0x180)/8]; // 180
	BGSTextureModel		textureModel[2]; // 288
	BGSBehaviorGraphModel behaviorGraph[2]; // 2E8
	BSFixedString		unk348[2]; // 348
	BSFixedString		unk358[2]; // 358
	BGSVoiceType		* voiceType[2];	// 368
	BGSBodyPartData		* bodyPartData;	// 378
	BGSSoundTagComponent	unk380;	// 380
	BGSAttachParentArray	unk388;	// 388
	BSFixedString		editorId; // 3A0
	BGSMaterialType		* materialType;	// 3A8
	void				* unk3B0;		// 3B0
	BGSTextureSet		* textureSets[2];	// 3B8
	BGSSoundDescriptorForm * soundDescriptors[2]; // 3C8
	BSFixedString		bipedObjectNames[0x20]; // 3D8

	// 10
	template<int T>
	struct BoneScale // (BSMB/BSMS) pair
	{
		BSFixedString	bone;		// 00
		float			* value;	// 08

		bool operator==(const BSFixedString a_key) const	{ return bone.data == a_key.data; }
		static inline UInt32 GetHash(const BSFixedString * key)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)key->data->Get<char>(), 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tkey: %s", bone.data ? bone.data->Get<char>() : "");
			for(UInt32 i = 0; i < T; i++)
				_MESSAGE("\t\tdata: %f", value[i]);
		}
	};

	struct MorphSlider
	{
		UInt32	key;		// 00
		UInt32	pad04;		// 04

		struct Morphs
		{
			BSFixedString lower;	// 00
			BSFixedString upper;	// 08
		};
		Morphs * morphs;	// 08

		bool operator==(const UInt32 a_key) const	{ return key == a_key; }
		static inline UInt32 GetHash(const UInt32 * key)
		{
			UInt32 hash;
			CalculateCRC32_32(&hash, *key, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tkey: %08X", key);
			if(morphs)
			{
				_MESSAGE("\t\tLower: %s", morphs->lower.data->Get<char>());
				_MESSAGE("\t\tUpper: %s", morphs->upper.data->Get<char>());
			}
		}
	};

	struct BoneScaleMap
	{
		tHashSet<BoneScale<8>, BSFixedString>	weightMap1;	// value array length 8
		tHashSet<BoneScale<4>, BSFixedString>	weightMap2;	// value array length 4
	};

	UInt64								unk4C0[(0x648-0x4D8)/8];	// 4D8 - 654, 670, 660 table?
	tHashSet<MorphSlider, UInt32>		morphSliders;				// 648
	UInt64								unk678[(0x698-0x678)/8];	// 678
	CharacterCreation::CharGenData		* chargenData[2];			// 698
	BoneScaleMap						* boneScaleMap[2];			// 6A8
	TESTexture							hairColorLUT;				// 6B8
};

STATIC_ASSERT(offsetof(TESRace, chargenData) == 0x698);
STATIC_ASSERT(sizeof(TESRace) == 0x6C8);

// 48
class BGSListForm : public TESForm
{
public:
	enum { kTypeID = kFormType_FLST };

	tArray<TESForm*>	forms;			// 20
	tArray<UInt32>*       tempForms;	// 38
	UInt32				scriptAdded;	// 40 - Amount on the end of the tArray that is script-added
	UInt32				unk44;			// 44
};
STATIC_ASSERT(sizeof(BGSListForm) == 0x48);

// 178
class BGSHeadPart : public TESForm
{
public:
	enum { kTypeID = kFormType_HDPT };

	TESFullName				fullName;		// 20
	BGSModelMaterialSwap	materialSwap;	// 30

	enum 
	{
		kFlagPlayable	= 1 << 0,
		kFlagMale		= 1 << 1,
		kFlagFemale		= 1 << 2,
		kFlagExtraPart	= 1 << 3,
		kFlagSolidTint	= 1 << 4,
		kFlagUnk1		= 1 << 5 // only appears on Head Rear types
	};
	UInt32					partFlags;		// 70

	enum {
		kTypeMisc = 0,
		kTypeFace,
		kTypeEyes,
		kTypeHair,
		kTypeFacialHair,
		kTypeScar,
		kTypeBrows,
		kType7,
		kType8,
		kTypeHeadRear,
		kNumTypes
	};
	UInt32					type;			// 74

	tArray<BGSHeadPart*>	extraParts;		// 78
	BGSTextureSet			* textureSet;	// 90
	TESModel				model;			// 98
	TESModelTri				morphs[3];		// C8
	UInt64					unk158;			// 158
	BGSListForm				* validRaces;	// 160
	Condition				* conditions;	// 168 - Condition most likely
	BSFixedString			partName;		// 170

	bool IsExtraPart() { return (partFlags & kFlagExtraPart) == kFlagExtraPart; }
};

// 1B0
class EffectSetting : public TESForm
{
public:
	enum { kTypeID = kFormType_MGEF };

	TESFullName				fullName;		// 20
	BGSMenuDisplayObject	menuObject;		// 30
	BGSKeywordForm			keywordForm;	// 40
	UInt64					unk060[2]; // 60
	UInt32					unk070; // 70
	float					unk074;
	TESForm*				unk078;	// primary object? (SpellItem, TESObjectLIGH, BGSDamageType, BGSHazard)
	UInt64					unk080;
	ActorValueInfo*			actorValInfo88; // 088
	UInt8					unk090;
	UInt8					pad092[3];
	TESObjectLIGH*			light98;
	float					unkA0;
	UInt32					padA4;
	TESEffectShader*		effectShaderA8;
	UInt64					unkB0;
	UInt64					unkB8;
	float					unkC0;
	float					unkC4;
	float					unkC8;
	float					unkCC;
	UInt32					unk0D0; 
	UInt32					pad0D4;
	ActorValueInfo*			actorValInfoD8;
	BGSProjectile*			projectileE0;
	BGSExplosion*			explosionE8;
	UInt32					unkF0; // init to 3 cast type?
	UInt32					unkF4; // init to 5 delivery type?
	ActorValueInfo*			actorValInfoF8;
	UInt64					unk100;
	BGSArtObject*			art108;
	BGSImpactDataSet*		impact110;
	UInt32					unk118;
	UInt32					pad11C;
	UInt64					unk120;
	float					unk128; // initialized to  3F800000 dual casting scale?
	UInt32					pad12C;
	UInt64					unk130[4];
	TESImageSpaceModifier*	spaceModifier150; // 150
	BGSPerk*				perk158;
	UInt64					unk160;	// initialized to 1		
	UInt64					unk168[3];
	UInt64					unk180;
	UInt32					unk188;
	UInt32					unk18C;
	UInt64					unk190;
	void*					unk198;	// pointer to something
	UInt64					unk1A0;
	void*					unk1A8;
};
STATIC_ASSERT(offsetof(EffectSetting, unk160) == 0x160);
STATIC_ASSERT(sizeof(EffectSetting) == 0x1B0);

// 50
// MSWP
class BGSMaterialSwap : public TESForm
{
public:
	enum { kTypeID = kFormType_MSWP };

	struct MaterialSwap
	{
		BSFixedString		source;				// 00
		BSFixedString		target;				// 08
		float				colorRemapIndex;	// 10
		UInt32				unk14;				// 14

		operator BSFixedString() const						{ return source; }
		bool operator==(const BSFixedString a_name) const	{ return source == a_name; }
		static inline UInt32 GetHash(BSFixedString * key)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)key->data, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tname: %s", source.c_str());
			_MESSAGE("\t\ttarget: %s", target.c_str());
			_MESSAGE("\t\tremapIndex: %f", colorRemapIndex);
		}
	};

	tHashSet<MaterialSwap, BSFixedString> materialSwaps; // 20
};
STATIC_ASSERT(sizeof(BGSMaterialSwap) == 0x50);

class BGSMod
{
public:
	class Container
	{
	public:

		// Used by Papyrus code to verify internal type
		// i.e. can't assign an Armor property to a Weapon OMOD
		enum
		{
			kWeaponTarget_Offset	= 0x000,
			kArmorTarget_Offset		= 0x100,
			kActorTarget_Offset		= 0x200
		};

		// Used by Papyrus to specify the particular operator
		enum
		{
			kOperator_Set = 0,
			kOperator_Add = 1,
			kOperator_Mult_Add = 2,
			kOperator_And = 3,
			kOperator_Or = 4,
			kOperator_Rem = 5
		};

		enum WeaponFormProperty
		{
			kWeaponTarget_bAlternateRumble = 22,
			kWeaponTarget_bAutomatic = 25,
			kWeaponTarget_bBoltAction = 81,
			kWeaponTarget_bCantDrop = 26,
			kWeaponTarget_bChargingAttack = 93,
			kWeaponTarget_bChargingReload = 18,
			kWeaponTarget_bDisableShells = 92,
			kWeaponTarget_bEffectOnDeath = 21,
			kWeaponTarget_bFixedRange = 20,
			kWeaponTarget_bHasScope = 48,
			kWeaponTarget_bHoldInputToPower = 85,
			kWeaponTarget_bIgnoreResist = 24,
			kWeaponTarget_bMinorCrime = 19,
			kWeaponTarget_bNonHostile = 23,
			kWeaponTarget_bNonPlayable = 27,
			kWeaponTarget_bNPCsUseAmmo = 17,
			kWeaponTarget_bPlayerOnly = 16,
			kWeaponTarget_bRepeatableSingleFire = 86,
			kWeaponTarget_ehHitBehavior = 9,
			kWeaponTarget_elSoundLevel = 59,
			kWeaponTarget_eoZoomDataOverlay = 68,
			kWeaponTarget_esStaggerValue = 82,
			kWeaponTarget_ewType = 15,
			kWeaponTarget_cAccuracyBonus = 78,
			kWeaponTarget_fAimModelBaseStability = 66,
			kWeaponTarget_fAimModelConeDecreasePerSec = 36,
			kWeaponTarget_fAimModelConeIncreasePerShot = 35,
			kWeaponTarget_fAimModelConeIronSightsMultiplier = 47,
			kWeaponTarget_fAimModelConeSneakMultiplier = 38,
			kWeaponTarget_fAimModelMaxConeDegrees = 34,
			kWeaponTarget_fAimModelMinConeDegrees = 33,
			kWeaponTarget_fAimModelRecoilArgDeg = 45,
			kWeaponTarget_fAimModelRecoilArgRotateDeg = 46,
			kWeaponTarget_fAimModelRecoilDiminishSightsMult = 40,
			kWeaponTarget_fAimModelRecoilDiminishSpringForce = 39,
			kWeaponTarget_fAimModelRecoilHipMult = 43,
			kWeaponTarget_fAimModelRecoilMaxDegPerShot = 41,
			kWeaponTarget_fAimModelRecoilMinDegPerShot = 42,
			kWeaponTarget_fAttackActionPointCost = 79,
			kWeaponTarget_fAttackDelaySec = 4,
			kWeaponTarget_fColorRemappingIndex = 88,
			kWeaponTarget_fCriticalChargeBonus = 8,
			kWeaponTarget_fCriticalDamageMult = 90,
			kWeaponTarget_fFireSeconds = 50,
			kWeaponTarget_fFullPowerSeconds = 84,
			kWeaponTarget_fMaxRange = 3,
			kWeaponTarget_fMinPowerPerShot = 87,
			kWeaponTarget_fMinRange = 2,
			kWeaponTarget_fOutOfRangeDamageMult = 6,
			kWeaponTarget_fReach = 1,
			kWeaponTarget_fReloadSpeed = 76,
			kWeaponTarget_fSecondaryDamage = 7,
			kWeaponTarget_fSightedTransitionSeconds = 83,
			kWeaponTarget_fSoundLevelMult = 74,
			kWeaponTarget_fSpeed = 0,
			kWeaponTarget_fUnused = 5,
			kWeaponTarget_fWeight = 30,
			kWeaponTarget_fZoomDataCameraOffsetX = 70,
			kWeaponTarget_fZoomDataCameraOffsetY = 71,
			kWeaponTarget_fZoomDataCameraOffsetZ = 72,
			kWeaponTarget_iAmmoCapacity = 12,
			kWeaponTarget_iAttackDamage = 28,
			kWeaponTarget_iRank = 10,
			kWeaponTarget_iValue = 29,
			kWeaponTarget_paAimModel = 32,
			kWeaponTarget_peEnchantments = 65,
			kWeaponTarget_pgZoomDataImageSpace = 69,
			kWeaponTarget_piBashImpactDataSet = 63,
			kWeaponTarget_piImpactDataSet = 60,
			kWeaponTarget_pkKeywords = 31,
			kWeaponTarget_plCritEffect = 62,
			kWeaponTarget_pmBlockMaterial = 64,
			kWeaponTarget_pnNPCAmmoList = 75,
			kWeaponTarget_poAmmo = 61,
			kWeaponTarget_ppOverrideProjectile = 80,
			kWeaponTarget_psAttackFailSound = 55,
			kWeaponTarget_psAttackLoop = 54,
			kWeaponTarget_psAttackSound = 52,
			kWeaponTarget_psAttackSound2D = 53,
			kWeaponTarget_psEquipSound = 57,
			kWeaponTarget_psFastEquipSound = 91,
			kWeaponTarget_psIdleSound = 56,
			kWeaponTarget_psUnEquipSound = 58,
			kWeaponTarget_ptEquipSlot = 73,
			kWeaponTarget_pwMaterialSwaps = 89,
			kWeaponTarget_pzZoomData = 67,
			kWeaponTarget_uAimModelConeDecreaseDelayMs = 37,
			kWeaponTarget_uAimModelRecoilShotsForRunaway = 44,
			kWeaponTarget_uNumProjectiles = 51,
			kWeaponTarget_vaActorValues = 94,
			kWeaponTarget_vdDamageTypeValues = 77
		};

		enum ArmorFormProperty
		{
			kArmorTarget_ebBodyPart = 8,
			kArmorTarget_fColorRemappingIndex = 12,
			kArmorTarget_fWeight = 4,
			kArmorTarget_iAddonIndex = 7,
			kArmorTarget_iHealth = 11,
			kArmorTarget_iRating = 6,
			kArmorTarget_iValue = 5,
			kArmorTarget_peEnchantments = 0,
			kArmorTarget_piBashImpactDataSet = 1,
			kArmorTarget_pkKeywords = 3,
			kArmorTarget_pmBlockMaterial = 2,
			kArmorTarget_pwMaterialSwaps = 13,
			kArmorTarget_vaActorValues = 10,
			kArmorTarget_vdDamageTypeValues = 9
		};

		enum NPCFormProperty
		{
			kActorTarget_fColorRemappingIndex = 4,
			kActorTarget_iXPOffset = 2,
			kActorTarget_peEnchantments = 3,
			kActorTarget_piForcedInventory = 1,
			kActorTarget_pkKeywords = 0,
			kActorTarget_pwMaterialSwaps = 5
		};

		// 10
		struct Data
		{
			union
			{
				struct 
				{
					float	v1;
					float	v2;
				} f;

				struct 
				{
					UInt32	v1;
					UInt32	v2;
				} i;

				struct 
				{
					UInt32	formId;
					float	v2;
				} ff;

				struct 
				{
					UInt32	formId;
					UInt32	v2;
				} fi;

				TESForm * form;
			} value;		// 00

			UInt8	target;	// 08 - ArmorFormProperty/NPCFormProperty/WeaponFormProperty

			enum
			{
				kOpFlag_Mod = 0x02,
				kOpFlag_Mul = 0x08,
				kOpFlag_Add = 0x10,
				kOpFlag_Set = 0x20,
				kOpFlag_Dual = 0x40,
				kOpFlag_Form = 0x80,

				kOpFlag_Set_Bool = 0,
				kOpFlag_Or_Bool = (kOpFlag_Add | kOpFlag_Dual),
				kOpFlag_And_Bool = (kOpFlag_Mul | kOpFlag_Dual),

				kOpFlag_Set_Enum = (kOpFlag_Set | kOpFlag_Form),

				kOpFlag_Set_Int = kOpFlag_Dual,
				kOpFlag_Add_Int = kOpFlag_Add,
				kOpFlag_Mul_Add_Int = (kOpFlag_Mul | kOpFlag_Set), // Same as Float version

				kOpFlag_Set_Form = kOpFlag_Form,
				kOpFlag_Add_Form = (kOpFlag_Add | kOpFlag_Form),
				kOpFlag_Rem_Form = (kOpFlag_Mul | kOpFlag_Form),

				kOpFlag_Set_Float = kOpFlag_Set,
				kOpFlag_Add_Float = (kOpFlag_Add | kOpFlag_Set),
				kOpFlag_Mul_Add_Float = (kOpFlag_Mul | kOpFlag_Set),
				
				kOpFlag_Set_Form_Float = (kOpFlag_Dual | kOpFlag_Form),
				kOpFlag_Add_Form_Float = (kOpFlag_Add | kOpFlag_Dual | kOpFlag_Form),
				kOpFlag_Mul_Add_Form_Float = (kOpFlag_Mul | kOpFlag_Dual | kOpFlag_Form)
			};

			UInt8	op;		// 09
			UInt16	unk0A;	// 0A
			UInt32	unk0C;	// 0C
		};
		Data	* data;		// 00
		UInt32	dataSize;	// 08 - dataSize/sizeof(Data) = count
	};

	class Template
	{
	public:
		// 20
		class Items : public BaseFormComponent
		{
		public:
			virtual ~Items();

			virtual void	Unk_07(void);
			virtual void	Unk_08(void);

			void	* unk08;	// 08
			void	* unk10;	// 10
			void	* unk18;	// 18
		};

		// 38
		class Item : public TESFullName
		{
		public:
			virtual ~Item();

			virtual void	Unk_07(void);
			virtual void	Unk_08(void);

			void	* unk08;	// 08
			void	* unk10;	// 10
			UInt32	unk18;		// 18
			UInt32	unk1C;		// 1C
			void	* unk20;		// 20
			UInt64	unk28;		// 28
			UInt32	unk30;		// 30
			UInt8	unk34[4];	// 34
		};
	};

	class Attachment {
	public:
		// C8
		class Mod : public TESForm
		{
		public:
			enum { kTypeID = kFormType_OMOD };

			TESFullName									fullName;	// 20
			TESDescription								description; // 30
			BGSModelMaterialSwap						materialSwap; // 48
			BGSMod::Container							modContainer; // 88
			BGSAttachParentArray						unk98;			// 98
			UInt64										unkB0;			// B0
			UInt64										unkB8;			// B8
			UInt16										unkC0;			// C0

			enum
			{
				kTargetType_None = 0,
				kTargetType_Armor = 0x1D,
				kTargetType_Furniture = 0x2A,
				kTargetType_Weapon = 0x2B,
				kTargetType_Actor = 0x2D
			};

			UInt8										targetType;		// C2
			UInt8										maxRank;		// C3
			UInt8										scaledOffset;	// C4
			UInt8										priority;		// C5
			UInt8										unkC6;			// C6
			UInt8										unkC7;			// C7
		};
	};
};
STATIC_ASSERT(offsetof(BGSMod::Attachment::Mod, modContainer) == 0x88);
STATIC_ASSERT(sizeof(BGSMod::Attachment::Mod) == 0xC8);

// 48
class BGSTransform : public TESForm
{
public:
	enum { kTypeID = kFormType_TRNS };

	float				matrix[9];
	UInt32				pad44;
};
STATIC_ASSERT(offsetof(BGSTransform, pad44) == 0x44);
STATIC_ASSERT(sizeof(BGSTransform) == 0x48);

class BGSOpenCloseForm
{
public:
	//	void	** _vtbl;	// 00
};

class TESMagicTargetForm
{
public:
};

class TESMagicCasterForm
{
public:
};


class BGSLocationRefType : public BGSKeyword
{
public:
	enum { kTypeID = kFormType_LCRT };
};


// 140
class BGSLocation : public TESForm
{
public:
	enum { kTypeID = kFormType_LCTN };

	TESFullName				fullName;		// 020
	BGSKeywordForm			keywordForm;	// 030

	BGSLocation*			parent;			// 050
	UInt64					unk58;			// 058
	BGSMusicType*			musicType;		// 060 
	BGSEncounterZone*		encounterZone;	// 068 
	UInt32					unk70;			// 070
	float					worldRadius;	// 074
	float					unk78;	// init to 1.0
	UInt32					unk7C;

	struct CellStaticReference {
		BGSLocationRefType*	refType;	// 00
		UInt32	markerForm;				// 08
		UInt32	locationForm;			// 0C
		SInt16	gridY;					// 10
		SInt16	gridX;					// 12
		UInt32	pad10;					// 14
	};
	tArray<CellStaticReference>		staticReferences;
	UnkArray				arr98;
	UInt64					unkB0;
	UInt64					unkB8;
	UInt32					unkC0;
	UInt32					unkC4;
	UInt32					unkC8;
	UInt32					unkCC;
	UInt64					unkD0;
	UInt32					unkD8;
	float					unkDC;
	UInt32					unkE0;
	UInt32					unkE4;
	UInt64					unkE8;
	UInt64					unkF0;
	UInt32					unkF8;
	float					unkFC;
	UInt32					unk100;
	UInt32					unk104;
	UInt32					unk108;
	float					unk10C;
	UInt64					unk110;
	UInt32					unk118;
	float					unk11C;
	UInt32					unk120;
	UInt32					unk124;
	UInt32					unk128;
	UInt32					unk12C;
	UInt32					unk130;
	UInt8					cleared;
	UInt8					everCleared;
	UInt8					unk136;
	UInt8					unk137;
	UInt32					unk138;
	UInt32					unk13C;
};
STATIC_ASSERT(offsetof(BGSLocation, unkFC) == 0xFC);
STATIC_ASSERT(sizeof(BGSLocation) == 0x140);

// F0
class TESObjectCELL : public TESForm
{
public:
	enum { kTypeID = kFormType_CELL };

	// parents
	TESFullName				fullName;		// 20

	UInt64					unk30;				// 30
	UInt64					unk38;				// 38

	enum
	{
		kFlag_IsInterior = 1,
		kFlag_HasWater = 2		// Water is implied for Exteriors, this bit is set when it has an override
	};

	UInt16					flags;				// 40
	UInt16					unk42;				// 42
	UInt32					unk44;				// 44
	ExtraDataList			* extraDataList;	// 48

	struct MaxHeightData
	{
		UInt32	x;				// 00
		UInt32	y;				// 04
		UInt8	* maxHeightBlock;	// 08
	};

	MaxHeightData			* heightData;			// 50
	TESObjectLAND			* land;				// 58
	SInt32					unk60;				// 60
	float					unk64;				// 64
	tArray<void*>			* navMeshes;		// 68
	tArray<TESObjectREFR*>	objectList;			// 70
	UInt64					unk88;				// 88
	tHashSet<void*, void*>	unk90;				// 90
	void					* unkC0;			// C0
	TESWorldSpace			* worldSpace;		// C8
	UInt64					unkD0;				// D0
	UInt64					unkD8;				// D8
	UInt64					unkE0;				// E0
	UInt32					preVisCell;			// E8 - FormID
	UInt32					unkEC;				// EC

	MEMBER_FN_PREFIX(TESObjectCELL);
	DEFINE_MEMBER_FN(GetHavokWorld, bhkWorld*, 0x004C77D0);
};
STATIC_ASSERT(offsetof(TESObjectCELL, objectList) == 0x70);
STATIC_ASSERT(offsetof(TESObjectCELL, worldSpace) == 0xC8);
STATIC_ASSERT(sizeof(TESObjectCELL) == 0xF0);

// 48
class BGSEncounterZone : public TESForm
{
public:
	enum { kTypeID = kFormType_ECZN };

	enum
	{
		kFlag_NeverResets = 1,
		kFlag_Workshop = 8
	};

	UInt64					unk20;		// 20
	BGSLocation*			location;	// 28
	SInt8					rank;		// 30
	UInt8					minLevel;	// 30
	UInt8					flags;		// 32
	UInt8					maxLevel;	// 33
	UInt32					unk34;		// 34
	UInt64					unk38;		// 38
	UInt64					unk40;		// 40
};

// 38
class BGSOutfit : public TESForm
{
public:
	enum { kTypeID = kFormType_OTFT };

	tArray<TESForm*>	forms;	// 20
};

// 30
class BGSDamageType : public TESForm
{
public:
	enum { kTypeID = kFormType_DMGT };

	UInt64	unk20;
	UInt64	unk28;
};

// 48
class BGSEquipSlot : public TESForm
{
public:
	enum { kTypeID = kFormType_EQUP };

	tArray<BGSEquipSlot*>	parentSlots;	// 20
	UInt64		unk38;
	UInt64		unk40;
};

// 228
class TESWaterForm : public TESForm
{
public:
	enum { kTypeID = kFormType_WATR };

	// parents
	TESFullName				fullName;		// 20

	UInt64	unk30[(0x68 - 0x30) >> 3];		// 30

	TESTexture	layer1Noise;				// 68
	TESTexture	layer2Noise;				// 78
	TESTexture	layer3Noise;				// 88

	UInt32		opacity;					// 98
	UInt32		unk9C;						// 9C
	BGSMaterialType	* material;				// A0
	BGSSoundDescriptorForm	* openSound;	// A8

	// C8
	struct VisualData
	{
		float	depthAmount;			// 00
		union Color
		{
			UInt32 bgr;
			struct
			{
				UInt8	red;
				UInt8	green;
				UInt8	blue;
				UInt8	unused;
			} color;
		};
		Color	shallowColor;			// 04
		Color	deepColor;				// 08
		float	shallowColorRange;		// 0C
		float	deepColorRange;			// 10
		float	shallowAlpha;			// 14
		float	deepAlpha;				// 18
		float	alphaShallowRange;		// 1C
		float	alphaDeepRange;			// 20
		Color	underwaterColor;		// 24
		float	underwaterFogAmount;	// 28
		float	underwaterNearFog;		// 2C
		float	underwaterFarFog;		// 30
		float	normalMagnitude;		// 34
		float	shadowNormalFalloff;	// 38
		float	deepNormalFalloff;		// 3C
		float	reflectivityAmount;		// 40
		float	fresnelAmount;			// 44
		float	surfaceEffectFalloff;	// 48
		float	displacementForce;		// 4C
		float	displacementVelocity;	// 50
		float	displacementFalloff;	// 54
		float	displacementDampener;	// 58
		float	displacementStartingSize;	// 5C
		Color	reflectionColor;		// 60
		float	sunSpecularPower;		// 64
		float	sunSpecularMagnitude;	// 68
		float	sunSparklePower;		// 6C
		float	sunSparkleMagnitude;	// 70
		float	interiorSpecularRadius;	// 74
		float	interiorSpecularBrightness;	// 78
		float	interiorSpecularPower;	// 7C
		float	layer1WindDirection;	// 80
		float	layer2WindDirection;	// 84
		float	layer3WindDirection;	// 88
		float	layer1WindSpeed;	// 8C
		float	layer2WindSpeed;	// 90
		float	layer3WindSpeed;	// 94
		float	layer1AmplitudeScale;	// 98
		float	layer2AmplitudeScale;	// 9C
		float	layer3AmplitudeScale;	// A0
		float	layer1UVScale;	// A4
		float	layer2UVScale;	// A8
		float	layer3UVScale;	// AC
		float	layer1NoiseFalloff;	// B0
		float	layer2NoiseFalloff;	// B4
		float	layer3NoiseFalloff;	// B8
		float	siltAmount;			// BC
		Color	siltLightColor;		// C0
		Color	siltDarkColor;		// C4
	};
	VisualData	visualData;					// B0
	UInt64		unk178;		// 178
	UInt64		unk180;		// 180
	UInt64		unk188;		// 188
	UInt64		unk190;		// 190
	UInt64		unk198;		// 198
	UInt32		unk1A0;		// 1A0 - 9
	UInt32		unk1A4;		// 1A4 - 9
	UInt32		unk1A8;		// 1A8 - 4
	float		unk1AC;		// 1AC
	float		unk1B0;		// 1B0
	float		unk1B4;		// 1B4
	float		unk1B8;		// 1B8
	float		unk1BC;		// 1BC
	SpellItem	* consumeSpell;	// 1C0
	SpellItem	* contactSpell;	// 1C8
	UInt64	unk1D0[(0x208 - 0x1D0) >> 3];		// 30
	TESImageSpace	* imageSpace;	// 208
	float	linearVelocityX;	// 210
	float	linearVelocityY;	// 214
	float	linearVelocityZ;	// 218
	float	angularVelocityX;	// 21C
	float	angularVelocityY;	// 220
	float	angularVelocityZ;	// 224
};

// 98
class BGSPerk : public TESForm
{
public:
	enum { kTypeID = kFormType_PERK };

	// parents
	TESFullName				fullName;		// 20
	TESDescription			description;	// 30
	TESIcon					icon;			// 48

	UInt8					trait;			// 58
	UInt8					perkLevel;		// 59
	UInt8					numRanks;		// 5A
	UInt8					playable;		// 5B
	UInt8					hidden;			// 5C
	UInt8					unk5D;			// 5D
	UInt8					unk5E;			// 5E
	UInt8					unk5F;			// 5F
	Condition				* condition;	// 60
	tArray<BGSPerkEntry*>	perkEntries;	// 68
	BGSPerk					* nextPerk;		// 80
	BGSSoundDescriptorForm	* sound;		// 88
	BSFixedString			swfPath;		// 90
};

// 88
class BGSConstructibleObject : public TESForm
{
public:
	enum { kTypeID = kFormType_COBJ };

	BGSPickupPutdownSounds	pickupPutdownSounds;	// 20
	TESDescription			description;			// 38

	struct Component
	{
		TESForm	* component;	// 00
		UInt32	count;			// 08
	};

	tArray<Component>	* components;		// 50
	Condition			* conditions;		// 58
	TESForm				* createdObject;	// 60
	BGSKeyword			* workbenchKeyword;	// 68
	UInt16				createdCount;		// 70
	UInt16				priority;			// 72
	UInt32				unk74;				// 74
	void				* unk78;			// 78
	UInt64				unk80;				// 80
};
STATIC_ASSERT(offsetof(BGSConstructibleObject, createdObject) == 0x60);
STATIC_ASSERT(offsetof(BGSConstructibleObject, workbenchKeyword) == 0x68);
STATIC_ASSERT(sizeof(BGSConstructibleObject) == 0x88);

// 38
class TESGlobal : public TESForm
{
public:
	enum { kTypeID = kFormType_GLOB };

	const char* editorID;   // 20
	UInt64	unk28;			// 28
	float value;			// 30
	UInt32    unk34;		// 34
};

// 28
class BGSStoryManagerTreeForm : public TESForm
{
public:
	UInt64		unk20;		// 20
};

// 2F0
class TESQuest : public BGSStoryManagerTreeForm
{
public:

	enum { kTypeID = kFormType_QUST };	
	
	TESFullName	fullName;	// 28
	UInt64	unk38[(0xF0 - 0x38) >> 3];	// 038
	UInt32	unkF0;		// 0F0
	UInt16	flags;		// 0F4
	UInt8	priority;	// 0F6
	UInt8	type;		// 0F7

	UInt64	unk0F8[(0x2F0 - 0xF8) >> 3];	// 0F8

	bool IsCompleted()
	{
		return ((flags >> 1) & 1);
	}

	void SetActive(bool active)
	{
		if (active)
		{
			flags |= 0x800;
		}
		else
		{
			flags &= ~0x800;
		}
	}

	MEMBER_FN_PREFIX(TESQuest);
	// broken address
//	DEFINE_MEMBER_FN(ForceRefTo, UInt32, 0x00375050, UInt32 aliasId, TESObjectREFR * reference);
	DEFINE_MEMBER_FN(NewGame_Internal, UInt8, 0x006681A0, UInt8 * unk1, UInt8 unk2);

	UInt8 NewGame_Hook(UInt8 * unk1, UInt8 unk2);
};
STATIC_ASSERT(offsetof(TESQuest, type) == 0xF7);

// 28
class BGSBaseAlias // Not actually a form, but its used like one in Papyrus
{
public:
	virtual ~BGSBaseAlias();
	virtual void		Unk01(void) = 0;
	virtual void		Unk02(void) = 0;
	virtual void		Unk03(void) = 0;
	virtual void		Unk04(void) = 0;
	virtual void		Unk05(void) = 0;
	virtual void		Unk06(void) = 0;
	virtual void		Unk07(void) = 0;

	enum { kTypeID = kFormType_Alias };

	StringCache::Ref	name;		// 08
	TESQuest			* owner;	// 10
	UInt64				aliasId;	// 18
	UInt16				flags;		// 20
	UInt16				pad22;		// 22
	UInt32				pad24;		// 24
};

// 48
class BGSRefAlias : public BGSBaseAlias
{
public:
	enum { kTypeID = kFormType_ReferenceAlias };

	UInt64	unk28;	// 28
	UInt64	unk30;	// 30
	UInt64	unk38;	// 38
	UInt64	unk40;	// 40
};

// 50
class BGSRefCollectionAlias : public BGSBaseAlias
{
public:
	enum { kTypeID = kFormType_RefCollectionAlias };

	UInt64	unk48; // 48
};

// 60
class BGSLocAlias : public BGSBaseAlias
{
public:
	enum { kTypeID = kFormType_LocationAlias };

	UInt64	unk28;	// 28
	UInt64	unk30;	// 30
	UInt64	unk38;	// 38
	UInt64	unk40;	// 40
	SInt64	unk48;	// 48
	SInt64	unk50;	// 50
	UInt32	unk58;	// 58
	UInt32	unk5C;	// 5C
};

// 08
class IFormFactory
{
public:
	virtual ~IFormFactory();

	virtual TESForm *		Create(void) = 0;
	virtual const char *	GetName(void) = 0;
	virtual UInt32			GetTypeID(void) = 0;

	virtual UInt32			Unk_04(void);	// return 0
	virtual UInt32			Unk_05(void);	// return 0x7E
	virtual UInt32			Unk_06(void);	// return 9
	virtual UInt32			Unk_07(void);	// return 0

	static IFormFactory *	GetFactoryForType(UInt32 type)
	{
		if(type > kFormType_Max)
			return nullptr;

		IFormFactory ** factoryList = g_formFactoryList;
		return factoryList[type];
	}
};

// 10
class ConcreteFormFactory : public IFormFactory
{
public:
	const char	* name;		// 08
};

struct ObjectModMiscPair
{
	BGSMod::Attachment::Mod	* key;
	TESObjectMISC			* miscObject;

	operator BGSMod::Attachment::Mod*() const					{ return key; }

	static inline UInt32 GetHash(BGSMod::Attachment::Mod ** key)
	{
		UInt32 hash;
		CalculateCRC32_64(&hash, (UInt64)*key, 0);
		return hash;
	}
};

extern RelocPtr <tHashSet<ObjectModMiscPair, BGSMod::Attachment::Mod*>> g_modAttachmentMap;
