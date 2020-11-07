#pragma once

#include "f4se_common/Utilities.h"
#include "f4se/GameTypes.h"
#include "f4se/GameForms.h"
#include "f4se/GameObjects.h"
#include "f4se/GameInput.h"

class TESObjectWEAP;
class TESNPC;
class AlchemyItem;
class SpellItem;
class ScrollItem;
class IngredientItem;
class EffectSetting;
class TESObjectARMO;
class TESObjectARMA;
class TESObjectCONT;
class TESObjectCELL;
class TESRegionList;
class TESWorldSpace;
class BGSAddonNode;
class TESWaterForm;
class BGSPerk;
class TESLevCharacter;
class BGSDefaultObject;
class BGSConstructibleObject;

struct ModInfo		// referred to by game as TESFile
{
	ModInfo();
	~ModInfo();

	UInt64								unk00[10];			// 000
	BSFile*								file;				// 050
	UInt64								unk58;				// 058
	UInt64								unk60;				// 060
	void								* unk68;			// 068
	char								name[MAX_PATH];		// 070
	char								directory[MAX_PATH];	// 174
	UInt64								unk278[0xB8/8];		// 278
	struct Dependency
	{
		void		* unk00;	// 00
		const char	* name;		// 08
		Dependency	* next;		// 10
	};

	enum RecordFlag
	{
		kRecordFlags_None = 0,
		kRecordFlags_ESM = 1 << 0,
		kRecordFlags_Active = 1 << 3,
		kRecordFlags_Localized = 1 << 7,
		kRecordFlags_ESL = 1 << 9
	};

	UInt32								flags;				// 330
	UInt32							recordFlags;		// 334
	UInt64								unk338;				// 338
	UInt64								unk340;				// 340
	UInt64								unk348;				// 348
	Dependency							* depends;			// 350
	UInt32								numRefMods;			// 358
	UInt32								unk35C;				// 35C
	ModInfo								** refModInfo;		// 360
	UInt64								unk368;				// 360
	UInt8								modIndex;			// 370
	UInt8								pad371;				// 371
	UInt16								lightIndex;			// 372
	UInt8								pad[4];				// 371
	BSString							author;				// 378
	BSString							description;		// 388
	// ...

	// Checks if a particular formID is part of the mod
	bool IsFormInMod(UInt32 formID) const
	{
		if (!IsLight() && (formID >> 24) == modIndex)
			return true;
		if (IsLight() && (formID >> 24) == 0xFE && ((formID & 0x00FFF000) >> 12) == lightIndex)
			return true;
		return false;
	}

	// Returns either a modIndex or a modIndex|lightIndex pair
	UInt32 GetPartialIndex() const
	{
		return !IsLight() ? modIndex : (0xFE000 | lightIndex);
	}

	// Converts the lower bits of a FormID to a full FormID depending on plugin type
	UInt32 GetFormID(UInt32 formLower) const
	{
		return !IsLight() ? UInt32(modIndex) << 24 | (formLower & 0xFFFFFF) : 0xFE000000 | (UInt32(lightIndex) << 12) | (formLower & 0xFFF);
	}

	bool IsActive() const { return modIndex != 0xFF; }
	bool IsLight() const { return (recordFlags & kRecordFlags_ESL) == kRecordFlags_ESL; }
};

STATIC_ASSERT(offsetof(ModInfo, recordFlags) == 0x334);
STATIC_ASSERT(offsetof(ModInfo, author) == 0x378);

struct ModList
{
	tList<ModInfo>		modInfoList;		// 00
	tArray<ModInfo*>	loadedMods;			// 10
	tArray<ModInfo*>	lightMods;			// 28
	tArray<UInt32>		formMapping;		// 40
};

// 17E8?
class DataHandler
{
public:
	UInt64								unk00;		// 00
	UnkArray							unk08;		// 08
	UnkArray							unk20;		// 20
	UnkArray							unk38;		// 38
	UnkArray							unk50;		// 50

	UnkFormArray						arrNONE;	// Form Type 0
	UnkFormArray						arrTES4;	// Form Type 1
	UnkFormArray						arrGRUP;	// Form Type 2
	UnkFormArray						arrGMST;	// Form Type 3
	tArray<BGSKeyword*>					arrKYWD;	// Form Type 4
	UnkFormArray						arrLCRT;	// Form Type 5
	tArray<BGSAction*>					arrAACT;	// Form Type 6
	tArray<BGSTransform*>				arrTRNS;	// Form Type 7
	tArray<BGSComponent*>				arrCMPO;	// Form Type 8
	UnkFormArray						arrTXST;	// Form Type 9
	UnkFormArray						arrMICN;	// Form Type 10
	UnkFormArray						arrGLOB;	// Form Type 11
	UnkFormArray						arrDMGT;	// Form Type 12
	UnkFormArray						arrCLAS;	// Form Type 13
	UnkFormArray						arrFACT;	// Form Type 14
	tArray<BGSHeadPart*>				arrHDPT;	// Form Type 15
	UnkFormArray						arrEYES;	// Form Type 16
	tArray<TESRace*>					arrRACE;	// Form Type 17
	UnkFormArray						arrSOUN;	// Form Type 18
	UnkFormArray						arrASPC;	// Form Type 19
	UnkFormArray						arrSKIL;	// Form Type 20
	tArray<EffectSetting*>				arrMGEF;	// Form Type 21
	UnkFormArray						arrSCPT;	// Form Type 22
	UnkFormArray						arrLTEX;	// Form Type 23
	tArray<EnchantmentItem*>			arrENCH;	// Form Type 24
	tArray<SpellItem*>					arrSPEL;	// Form Type 25
	tArray<ScrollItem*>					arrSCRL;	// Form Type 26
	tArray<TESObjectACTI*>				arrACTI;	// Form Type 27
	UnkFormArray						arrTACT;	// Form Type 28
	tArray<TESObjectARMO*>				arrARMO;	// Form Type 29
	UnkFormArray						arrBOOK;	// Form Type 30
	tArray<TESObjectCONT*>				arrCONT;	// Form Type 31
	UnkFormArray						arrDOOR;	// Form Type 32
	tArray<IngredientItem*>				arrINGR;	// Form Type 33
	UnkFormArray						arrLIGH;	// Form Type 34
	tArray<TESObjectMISC*>				arrMISC;	// Form Type 35
	UnkFormArray						arrSTAT;	// Form Type 36
	UnkFormArray						arrSCOL;	// Form Type 37
	UnkFormArray						arrMSTT;	// Form Type 38
	UnkFormArray						arrGRAS;	// Form Type 39
	UnkFormArray						arrTREE;	// Form Type 40
	UnkFormArray						arrFLOR;	// Form Type 41
	tArray<TESFurniture*>				arrFURN;	// Form Type 42
	tArray<TESObjectWEAP*>				arrWEAP;	// Form Type 43
	UnkFormArray						arrAMMO;	// Form Type 44
	tArray<TESNPC*>						arrNPC_;	// Form Type 45
	tArray<TESLevCharacter*>			arrLVLN;	// Form Type 46
	UnkFormArray						arrKEYM;	// Form Type 47
	tArray<AlchemyItem*>				arrALCH;	// Form Type 48
	UnkFormArray						arrIDLM;	// Form Type 49
	UnkFormArray						arrNOTE;	// Form Type 50
	UnkFormArray						arrPROJ;	// Form Type 51
	UnkFormArray						arrHAZD;	// Form Type 52
	UnkFormArray						arrBNDS;	// Form Type 53
	UnkFormArray						arrSLGM;	// Form Type 54
	tArray<BGSTerminal*>				arrTERM;	// Form Type 55
	UnkFormArray						arrLVLI;	// Form Type 56
	UnkFormArray						arrWTHR;	// Form Type 57
	UnkFormArray						arrCLMT;	// Form Type 58
	UnkFormArray						arrSPGD;	// Form Type 59
	UnkFormArray						arrRFCT;	// Form Type 60
	UnkFormArray						arrREGN;	// Form Type 61
	UnkFormArray						arrNAVI;	// Form Type 62
	UnkFormArray						arrCELL;	// Form Type 63
	UnkFormArray						arrREFR;	// Form Type 64
	UnkFormArray						arrACHR;	// Form Type 65
	UnkFormArray						arrPMIS;	// Form Type 66
	UnkFormArray						arrPARW;	// Form Type 67
	UnkFormArray						arrPGRE;	// Form Type 68
	UnkFormArray						arrPBEA;	// Form Type 69
	UnkFormArray						arrPFLA;	// Form Type 70
	UnkFormArray						arrPCOM;	// Form Type 71
	UnkFormArray						arrPBAR;	// Form Type 72
	UnkFormArray						arrPHZD;	// Form Type 73
	UnkFormArray						arrWRLD;	// Form Type 74
	UnkFormArray						arrLAND;	// Form Type 75
	UnkFormArray						arrNAVM;	// Form Type 76
	UnkFormArray						arrTLOD;	// Form Type 77
	UnkFormArray						arrDIAL;	// Form Type 78
	UnkFormArray						arrINFO;	// Form Type 79
	UnkFormArray						arrQUST;	// Form Type 80
	UnkFormArray						arrIDLE;	// Form Type 81
	UnkFormArray						arrPACK;	// Form Type 82
	UnkFormArray						arrCSTY;	// Form Type 83
	UnkFormArray						arrLSCR;	// Form Type 84
	UnkFormArray						arrLVSP;	// Form Type 85
	UnkFormArray						arrANIO;	// Form Type 86
	tArray<TESWaterForm*>				arrWATR;	// Form Type 87
	UnkFormArray						arrEFSH;	// Form Type 88
	UnkFormArray						arrTOFT;	// Form Type 89
	UnkFormArray						arrEXPL;	// Form Type 90
	UnkFormArray						arrDEBR;	// Form Type 91
	UnkFormArray						arrIMGS;	// Form Type 92
	UnkFormArray						arrIMAD;	// Form Type 93
	tArray<BGSListForm*>				arrFLST;	// Form Type 94
	tArray<BGSPerk*>					arrPERK;	// Form Type 95
	UnkFormArray						arrBPTD;	// Form Type 96
	UnkFormArray						arrADDN;	// Form Type 97
	UnkFormArray						arrAVIF;	// Form Type 98
	UnkFormArray						arrCAMS;	// Form Type 99
	UnkFormArray						arrCPTH;	// Form Type 100
	UnkFormArray						arrVTYP;	// Form Type 101
	UnkFormArray						arrMATT;	// Form Type 102
	UnkFormArray						arrIPCT;	// Form Type 103
	UnkFormArray						arrIPDS;	// Form Type 104
	tArray<TESObjectARMA*>				arrARMA;	// Form Type 105
	tArray<BGSEncounterZone*>			arrECZN;	// Form Type 106
	tArray<BGSLocation*>				arrLCTN;	// Form Type 107
	UnkFormArray						arrMESG;	// Form Type 108
	UnkFormArray						arrRGDL;	// Form Type 109
	UnkFormArray						arrDOBJ;	// Form Type 110
	tArray<BGSDefaultObject*>			arrDFOB;	// Form Type 111
	UnkFormArray						arrLGTM;	// Form Type 112
	UnkFormArray						arrMUSC;	// Form Type 113
	UnkFormArray						arrFSTP;	// Form Type 114
	UnkFormArray						arrFSTS;	// Form Type 115
	UnkFormArray						arrSMBN;	// Form Type 116
	UnkFormArray						arrSMQN;	// Form Type 117
	UnkFormArray						arrSMEN;	// Form Type 118
	UnkFormArray						arrDLBR;	// Form Type 119
	UnkFormArray						arrMUST;	// Form Type 120
	UnkFormArray						arrDLVW;	// Form Type 121
	UnkFormArray						arrWOOP;	// Form Type 122
	UnkFormArray						arrSHOU;	// Form Type 123
	UnkFormArray						arrEQUP;	// Form Type 124
	UnkFormArray						arrRELA;	// Form Type 125
	UnkFormArray						arrSCEN;	// Form Type 126
	UnkFormArray						arrASTP;	// Form Type 127
	UnkFormArray						arrOTFT;	// Form Type 128
	UnkFormArray						arrARTO;	// Form Type 129
	UnkFormArray						arrMATO;	// Form Type 130
	UnkFormArray						arrMOVT;	// Form Type 131
	UnkFormArray						arrSNDR;	// Form Type 132
	UnkFormArray						arrDUAL;	// Form Type 133
	UnkFormArray						arrSNCT;	// Form Type 134
	UnkFormArray						arrSOPM;	// Form Type 135
	UnkFormArray						arrCOLL;	// Form Type 136
	tArray<BGSColorForm*>				arrCLFM;	// Form Type 137
	UnkFormArray						arrREVB;	// Form Type 138
	UnkFormArray						arrPKIN;	// Form Type 139
	UnkFormArray						arrRFGP;	// Form Type 140
	UnkFormArray						arrAMDL;	// Form Type 141
	UnkFormArray						arrLAYR;	// Form Type 142
	tArray<BGSConstructibleObject*>		arrCOBJ;	// Form Type 143
	tArray<BGSMod::Attachment::Mod*>	arrOMOD;	// Form Type 144
	tArray<BGSMaterialSwap*>			arrMSWP;	// Form Type 145
	UnkFormArray						arrZOOM;	// Form Type 146
	UnkFormArray						arrINNR;	// Form Type 147
	UnkFormArray						arrKSSM;	// Form Type 148
	UnkFormArray						arrAECH;	// Form Type 149
	UnkFormArray						arrSCCO;	// Form Type 150
	UnkFormArray						arrAORU;	// Form Type 151
	UnkFormArray						arrSCSN;	// Form Type 152
	UnkFormArray						arrSTAG;	// Form Type 153
	UnkFormArray						arrNOCM;	// Form Type 154
	UnkFormArray						arrLENS;	// Form Type 155
	UnkFormArray						arrLSPR;	// Form Type 156
	UnkFormArray						arrGDRY;	// Form Type 157
	UnkFormArray						arrOVIS;	// Form Type 158

	TESRegionList						* regionList;	// F50
	NiTArray<TESObjectCELL*>			cellList;		// F58
	NiTArray<BGSAddonNode*>				addonNodes;		// F70

	UInt64								unkF88;			// F88
	UInt64								unkF90;			// F90
	UInt64								unkF98;			// F98
	UInt32								unkFA0;			// FA0 - FormID?
	UInt32								unkFA4;			// FA4
	UInt64								unkFA8;			// FA8

	ModList								modList;		// FB0

	UInt64								unk17C0[(0x17E8-0x17C0)/8];	// 17C0

	const ModInfo* LookupModByName(const char* modName);
	UInt8 GetModIndex(const char* modName);

	const ModInfo* LookupLoadedModByName(const char* modName);
	UInt8 GetLoadedModIndex(const char* modName);

	const ModInfo* LookupLoadedLightModByName(const char* modName);
	UInt16 GetLoadedLightModIndex(const char* modName);
};

extern RelocPtr <DataHandler*> g_dataHandler;
extern RelocPtr <bool> g_isGameDataReady;

struct DefaultObjectEntry
{
	BSFixedString		editorId;
	BGSDefaultObject	* defaultObject;

	operator StringCache::Entry*() const					{ return editorId.data; }

	static inline UInt32 GetHash(StringCache::Entry** key)
	{
		UInt32 hash;
		CalculateCRC32_64(&hash, (UInt64)*key, 0);
		return hash;
	}

	void Dump(void)
	{
		_MESSAGE("\t\tname: %s", editorId.data->Get<char>());
		_MESSAGE("\t\tinstance: %08X", defaultObject->formID);
	}
};

class DefaultObjectMap : public tHashSet<DefaultObjectEntry, StringCache::Entry*>
{
public:
	BGSDefaultObject * GetDefaultObject(BSFixedString name);
};

extern RelocPtr <DefaultObjectMap*> g_defaultObjectMap;
extern RelocPtr <BSReadWriteLock> g_defaultObjectMapLock;

class FavoritesManager : public BSIntrusiveRefCounted
{
public:
	virtual ~FavoritesManager();

	BSInputEventUser	inputEventUser;				// 10
	BSTEventSink<void>	inventoryEventSink;			// 20
	BSTEventSink<void>	favoriteChangedEventSink;	// 28

	UInt64	unk30;	// 30
	UInt64	unk38;	// 38
	tArray<BSTEventSink<void>> favoritesSinks;	// 40
	UInt64	unk58;	// 58
	UInt64	unk60;	// 60
	UInt64	unk68;	// 68
	UInt64	unk70;	// 70
	UInt64	unk78;	// 78
	UInt64	unk80;	// 80
	UInt64	unk88;	// 88

	enum Favorites
	{
		kNumFavorites = 12
	};

	TESForm	* favorites[kNumFavorites];	// 90
	void * queuedFile[kNumFavorites];	// F0
	tHashSet<uintptr_t> unk150;			// 150

	struct TaggedEntry
	{
		TESForm * form;	// 00
		UInt32	unk08;	// 08
		UInt32	unk0C;	// 0C

		operator TESForm *() const					{ return form; }

		static inline UInt32 GetHash(TESForm ** key)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)*key, 0);
			return hash;
		}

		void Dump(void)
		{
			_MESSAGE("\t\tForm: %08X", form ? form->formID : 0);
			_MESSAGE("\t\tunk08: %08X", unk08);
		}
	};

	tHashSet<TaggedEntry, TESForm*> taggedForms;	// 180
	// ...
};
STATIC_ASSERT(offsetof(FavoritesManager, taggedForms) == 0x180);

extern RelocPtr <FavoritesManager*> g_favoritesManager;
