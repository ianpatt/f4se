#pragma once

#include "f4se_common/Relocation.h"
#include "f4se_common/Utilities.h"

#include "f4se/GameUtilities.h"
#include "f4se/GameTypes.h"

class Actor;
class TESNPC;
class BGSHeadPart;
class BGSColorForm;
class BGSTextureSet;
class ActorValueInfo;
class Condition;
class NiAVObject;
class BSTriShape;

class BGSCharacterTint
{
public:
	class Template
	{
	public:
		// 20
		class Entry
		{
		public:
			virtual ~Entry();

			virtual void Unk_01();
			virtual void Unk_02();
			virtual void Unk_03();
			virtual void Load(void * unk1) = 0; // Loads template from plugin stream

			enum
			{
				kSlotForeheadMask = 0,
				kSlotEyesMask,
				kSlotNoseMask,
				kSlotEarsMask,
				kSlotCheeksMask,
				kSlotMouthMask,
				kSlotNeckMask,
				kSlotLipColor,
				kSlotCheekColor,
				kSlotEyeliner,
				kSlotEyeSocketUpper,
				kSlotEyeSocketLower,
				kSlotSkinTone,
				kSlotPaint,
				kSlotLaughLines,
				kSlotCheekColorLower,
				kSlotNose,
				kSlotChin,
				kSlotNeck,
				kSlotForehead,
				kSlotDirt,
				kSlotScar,
				kSlotFaceDetail,
				kSlotBrows,
				kNumSlots
			};

			enum
			{
				kBlendOpDefault,
				kBlendOpMultiply,
				kBlendOpOverlay,
				kBlendOpSoftLight,
				kBlendOpHardLight,
			};

			enum
			{
				kFlagOnOff			= (1 << 0),
				kFlagChargenDetail	= (1 << 1),
				kFlagTakesSkinTone	= (1 << 2)
			};

			BSFixedString	name;			// 08
			Condition		* conditions;	// 10
			UInt32			slot;			// 18
			UInt16			templateIndex;	// 1C
			UInt8			flags;			// 1E
			UInt8			unk1F;			// 1F

			static Entry * Create(UInt32 size, UInt64 vtbl);
		};

		// 30
		class Mask : public Entry
		{
		public:
			virtual void Unk_01();
			virtual void Unk_02();
			virtual void Unk_03();
			virtual void Load(void * unk1);

			BSFixedString	texture;	// 20
			UInt32			blendOp;	// 28
			UInt32			unk2C;		// 2C

			static Mask * Create();
		};

		// 48
		class Palette : public Entry
		{
		public:
			virtual void Unk_01();
			virtual void Unk_02();
			virtual void Unk_03();
			virtual void Load(void * unk1);

			struct ColorData
			{
				BGSColorForm	* colorForm;	// 00
				float			alpha;			// 08
				UInt32			blendOp;		// 0C
				UInt16			colorID;		// 10
				UInt16			unk12;			// 12
				UInt32			unk14;			// 14
			};

			BSFixedString texture;		// 20
			UInt32	unk28;				// 28
			UInt32	unk2C;				// 2C
			tArray<ColorData>	colors;	// 30

			static Palette * Create();
			ColorData * GetColorDataByID(UInt16 colorID);
		};

		// 40
		class TextureSet : public Entry
		{
		public:
			virtual void Unk_01();
			virtual void Unk_02();
			virtual void Unk_03();
			virtual void Load(void * unk1);
			
			BSFixedString	diffuse;		// 20
			BSFixedString	normal;			// 28
			BSFixedString	specular;		// 30
			UInt32			blendOp;		// 38
			float			defaultValue;	// 3C

			static TextureSet * Create();
		};
	};

	// 18
	class Entry
	{
	public:
		virtual ~Entry();

		virtual bool IsEqual(Entry * rhs);
		virtual void Copy(Entry * rhs);
		virtual void Unk_03(void);
		virtual void Unk_04(void);
		virtual UInt32 GetType(void) = 0;
		virtual void Unk_06(void) { };

		enum
		{
			kTypeMask = 0,
			kTypePalette,
			kTypeTexture
		};

		Template::Entry * templateEntry;	// 08
		UInt16		tintIndex;		// 10
		UInt8		percent;		// 12	divided by 100
		UInt8		pad13;			// 13
		UInt32		pad14;			// 14
	};

	// 18
	class MaskEntry : public Entry
	{
	public:
		virtual ~MaskEntry();

		virtual void Unk_03(void);
		virtual void Unk_04(void);
		virtual UInt32 GetType(void) { return kTypeMask; };	// 0
		virtual void Unk_06(void) { };
	};

	// 20
	class PaletteEntry : public MaskEntry
	{
	public:
		virtual ~PaletteEntry();

		virtual bool IsEqual(Entry * rhs);
		virtual void Copy(Entry * rhs);
		virtual void Unk_03(void);
		virtual void Unk_04(void);
		virtual UInt32 GetType(void) { return kTypePalette; };	// 1
		virtual void Unk_06(void);

		union Color
		{
			struct Channel
			{
				UInt8 red;
				UInt8 green;
				UInt8 blue;
				UInt8 unused;
			} channel; // 18
			UInt32 bgra;
		} color;
		
		SInt16	colorID;	// 1C - ID of the color from the template's list
		UInt16	pad1E;		// 1E
	};

	// 18
	class TextureSetEntry : public Entry
	{
	public:
		virtual ~TextureSetEntry();

		virtual void Unk_03(void);
		virtual void Unk_04(void);
		virtual UInt32 GetType(void) { return kTypeTexture; };	// 2
		virtual void Unk_06(void) { };
	};

};

// 548
class CharacterCreation
{
public:
	UInt64	unk00;		// 00
	void	* unk08;	// 08
	UInt64	unk10;		// 10		4
	UInt64	unk18;		// 18		1
	UInt64	unk20;		// 20
	UInt64	unk28;		// 28
	UInt64	unk30;		// 30
	UInt64	unk38;		// 38
	UInt64	unk40;		// 40
	UInt64	unk48;		// 48
	UInt64	unk50;		// 50
	Actor	* actor;	// 58
	TESNPC	* npc;		// 60
	TESNPC	* npc2;		// 68

	struct TintData
	{
		BSFixedString								category;	// 00
		SInt32										unk08;		// 08
		UInt32										type;		// 0C
		tArray<BGSCharacterTint::Template::Entry*>	entry;		// 10 BGSCharacterTint::Template::Entry

		BGSCharacterTint::Template::Entry * GetTemplateByIndex(UInt16 index);
		BGSCharacterTint::Template::Entry * GetTemplateBySlot(UInt32 slotType);
	};

	// 40
	struct MorphGroup
	{
		BSFixedString name;				// 00 - MPGN
		struct Preset
		{
			BSFixedString	name;		// 00 - MPPN
			BSFixedString	morph;		// 08 - MPPM
			BGSTextureSet	* texture;	// 10 - MPPT
			UInt32			unk18;		// 18 - MPPF
			UInt32			index;		// 1C - MPPI (morph index?)
		};
		tArray<Preset>		presets;	// 08 - MPPC (count)
		tArray<float>		unk20;		// 20
		UInt32				key;		// 38 - MPPK (group key?)
	};

	// 90
	struct FaceMorphRegion
	{
		UInt64			unk00;			// 00
		UInt64			unk08;			// 08
		UInt64			unk10;			// 10
		UInt64			unk18;			// 18
		UInt64			unk20;			// 20
		BSFixedString	name;			// 28 - FMRN
		UInt32			* unk30;		// 30
		UInt32			unk30_capacity;	// 38
		UInt32			unk30_size;		// 3C
		struct Data
		{
			void			* key;		// 00 - ??
			UInt64			unk08;		// 08
			UInt64			unk10;		// 10
			BSFixedString	bone;		// 18
			float			* values;	// 20

			void Dump(void)
			{
				_MESSAGE("\t\tkey: %16lX bone: %s unk08: %16lX unk10: %16lX", key, bone.data ? bone.data->Get<char>() : "", unk08, unk10);
				if(values)
				{
					for(UInt32 i = 0; i < 4; i++)
						_MESSAGE("\t\tdata: %f", values[i]);
				}
			}
		};
		tHashSet<Data>			dataSet;		// 40
		UInt64					unk70;			// 70
		UInt64					unk78;			// 78
		UInt64					unk80;			// 80
		UInt32					index;			// 88 - FMRI
		UInt32					unk8C;			// 8C
	};

	// 10
	struct MorphIntensity
	{
		TESNPC *	npc;			// 00
		float		morphIntensity;	// 08

		operator TESNPC*() const					{ return npc; }

		static inline UInt32 GetHash(TESNPC ** key)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)*key, 0);
			return hash;
		}

		void Dump(void);
	};

	// 48+
	struct CharGenData
	{
		tArray<TintData*>			* tintData;			// 00
		tArray<BGSTextureSet*>		* textureSets;		// 08
		BGSTextureSet				* defaultTexture;	// 10
		tArray<TESNPC*>				* presets;			// 18
		tArray<BGSColorForm*>		* colors;			// 20
		BGSColorForm				* defaultColor;		// 28
		tArray<BGSHeadPart*>		* headParts;		// 30
		tArray<MorphGroup*>			* morphGroups;		// 38
		tArray<FaceMorphRegion*>	* faceMorphs;		// 40

		BGSCharacterTint::Template::Entry * GetTemplateByIndex(UInt16 index);
		BGSCharacterTint::Template::Entry * GetTemplateBySlot(UInt32 slotType);
	};

	struct Data1
	{
		tArray<TESNPC*>			presets;	// 00
		tArray<BGSHeadPart*>	headParts;	// 18
		tArray<void*>			unk30;		// 30

		struct Details
		{
			BSFixedString	location;	// 00
			struct Data
			{
				BSFixedString	name;	// 00
				BSFixedString	type;	// 08
				BGSTextureSet	* textureSet;	// 10
				UInt32			unk18;
				float			unk1C;
			};
			tArray<Data>	selection;	// 08
		};
		tArray<Details*>		details;	// 48
		tArray<TintData*>		tintData;	// 60
		tArray<BGSTextureSet*>	textures;	// 78
		// ...
	};
	Data1					* unk70;		// 70

	// Used by Scaleform callback to SetHairColor (17)
	tArray<BGSColorForm*>	hairColors;		// 78

	struct Data2
	{
		tArray<BGSTextureSet*>	unk00;	// 00
		tArray<BGSColorForm*>	unk18;	// 18
	};
	Data2					* unk90;		// 90
	UInt64					unk98;			// 98
	UInt64					unkA0;			// A0
	UInt64					unkA8;			// A8
	tArray<BGSHeadPart*>	faces;			// B0
	tArray<BGSHeadPart*>	eyes;			// C8
	tArray<BGSHeadPart*>	hairStyles;		// E0
	tArray<UInt64>			unkF8;			// F8
	UInt64					unk110[(0x350-0x110)/8];	// 110

	BGSCharacterTint::Template::Palette	* skinTint;		// 350 BGSCharacterTint::Template::Entry - Skin Tone?

	// Used by Scaleform callback to SetDetailColor (31)
	tArray<TintData*> tints;										// 358
	tArray<tArray<BGSCharacterTint::Template::Entry*>*>	details;	// 370

	UInt64					unk388[(0x510-0x388)/8];	// 388
	UInt32					presetIndex;				// 510
	UInt16					unk514;						// 514
	UInt8					unk516;						// 516 - weight change?
	UInt8					unk517;						// 517 - face dirty?
	UInt8					dirty;						// 518
	UInt8					unk519[3];					// 519
	UInt64					unk520[(0x548-0x520)/8];	// 520

	MEMBER_FN_PREFIX(CharacterCreation);
	DEFINE_MEMBER_FN(LoadPreset, void, 0x00CACD40, UInt32 presetIndex); // Loads preset by index onto the actor
};

// ??
class BSFaceGenManager
{
public:
	UInt8	unk00;								// 00
	UInt8	bDisableCustomFaceGeneration;		// 01
	UInt16	unk02;								// 02
	UInt32	numActorsAllowedToMorph;			// 04 numActorsAllowedToMorph

	UInt64		unk08[(0x31E8 - 0x08) >> 3];		// 08
	void		* unk31E8;							// 31E8
	UInt64		unk31F0[(0x3230 - 0x31F0) >> 3];	// 31F0
	void		* unk3230;							// 3230
	UInt64		unk3238[(0x3478 - 0x3238) >> 3];	// 3238
	void		* unk3478;							// 3478
	UInt64		unk3480[(0x34A8 - 0x3480) >> 3];	// 3238
	NiAVObject	* camera;							// 34A8 - NiCamera (WorldRoot Camera)

	MEMBER_FN_PREFIX(BSFaceGenManager);
	DEFINE_MEMBER_FN(ApplyDynamicData, void, 0x00678E40, BSTriShape * trishape);
};
STATIC_ASSERT(offsetof(BSFaceGenManager, unk3230) == 0x3230);
STATIC_ASSERT(offsetof(BSFaceGenManager, unk3478) == 0x3478);
STATIC_ASSERT(offsetof(BSFaceGenManager, camera) == 0x34A8);

extern RelocPtr <BSFaceGenManager*> g_faceGenManager;
extern RelocPtr <CharacterCreation*> g_characterCreation;
extern RelocPtr <UInt32> g_characterIndex;

extern RelocPtr <TESNPC*> g_customizationDummy1;
extern RelocPtr <TESNPC*> g_customizationDummy2;

typedef BGSCharacterTint::Entry* (* _CreateCharacterTintEntry)(UInt32 id);
extern RelocAddr <_CreateCharacterTintEntry> CreateCharacterTintEntry; // ID is (templateIndex << 16 | type)

typedef UInt64 (* _ClearCharacterTints)(tArray<BGSCharacterTint::Entry*> * src);
extern RelocAddr <_ClearCharacterTints> ClearCharacterTints;

typedef UInt64 (* _CopyCharacterTints)(tArray<BGSCharacterTint::Entry*> * dst, tArray<BGSCharacterTint::Entry*> * src);
extern RelocAddr <_CopyCharacterTints> CopyCharacterTints;

// Signature might not be correct
typedef void (* _FillTintTemplates)(tArray<BGSCharacterTint::Entry*> dst, CharacterCreation::CharGenData * src);
extern RelocAddr <_FillTintTemplates> FillTintTemplates;

// Signature might not be correct
typedef bool (* _MergeTintTextures)(TESNPC * npc, tArray<BGSCharacterTint::Entry*> * dst, void * unk3478, void * unk3230, UInt64 unk1, UInt64 unk2);
extern RelocAddr <_MergeTintTextures> MergeTintTextures;

// Signature might not be correct
typedef void (* _CreateMergeTintTextures)(TESNPC * npc, tArray<BGSCharacterTint::Entry*> * dst, void * unk1, UInt64 unk2);
extern RelocAddr <_CreateMergeTintTextures> CreateMergeTintTextures;

extern RelocAddr<uintptr_t> s_BGSCharacterTint_Template_MaskVtbl;
extern RelocAddr<uintptr_t> s_BGSCharacterTint_Template_PaletteVtbl;
extern RelocAddr<uintptr_t> s_BGSCharacterTint_Template_TextureSetVtbl;

extern RelocPtr <tHashSet<CharacterCreation::MorphIntensity, TESNPC*>> g_morphIntensityMap;
