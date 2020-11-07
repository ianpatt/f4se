#pragma once

#include "f4se/NiObjects.h"
#include "f4se/BSLight.h"

class BSShaderMaterial;
class BSParticleShaderCubeEmitter;
class NiTexture;
class BSRenderPass;
class BSFadeNode;

class NiProperty : public NiObjectNET
{
public:
	virtual ~NiProperty();

	enum
	{
		kTypeAlpha = 0,
		kTypeShade = 1
	};

	virtual SInt32 GetType(void);
};

// 30
class NiAlphaProperty : public NiProperty
{
public:
	virtual ~NiAlphaProperty();

	enum AlphaFunction
	{
		kAlpha_One = 0,
		kAlpha_Zero,
		kAlpha_SrcColor,
		kAlpha_InvSrcColor,
		kAlpha_DestColor,
		kAlpha_InvDestColor,
		kAlpha_SrcAlpha,
		kAlpha_InvSrcAlpha,
		kAlpha_DestAlpha,
		kAlpha_InvDestAlpha,
		kAlpha_SrcAlphaSat,
		kAlpha_Max_Modes,
	};

	enum TestFunction
	{
		kTest_Always = 0,
		kTest_Less,
		kTest_Equal,
		kTest_LessEqual,
		kTest_Greater,
		kTest_NotEqual,
		kTest_GreaterEqual,
		kTest_Never,
		kTest_Max_Modes,
	};

	UInt16	alphaFlags;		// 28
	UInt8	alphaThreshold;	// 2A
};

// 28
class NiShadeProperty : public NiProperty
{
public:
	virtual ~NiShadeProperty();

	virtual void Unk_23(void);
};

// 90
class BSEffectShaderData : public BSIntrusiveRefCounted
{
public:
	bool (__cdecl *pNodeFilterFunction)(BSFixedString *);		// 08
	NiPointer<NiTexture>			spBaseTexture;				// 10
	NiPointer<NiTexture>			spPaletteTexture;			// 18
	NiPointer<NiTexture>			spBlockOutTexture;			// 20
	UInt32							eTextureClampMode;			// 28
	NiColorA						FillColor;					// 2C
	NiColorA						RimColor;					// 3C
	float							fBaseFillScale;				// 4C
	float							fBaseFillAlpha;				// 50
	float							fBaseRimAlpha;				// 54
	float							fUOffset;					// 58
	float							fVOffset;					// 5C
	float							fUScale;					// 60
	float							fVScale;					// 64
	float							fEdgeExponent;				// 68
	float							fBoundDiameter;				// 6C
	NiAlphaProperty::AlphaFunction	eSrcBlend;					// 70
	NiAlphaProperty::AlphaFunction	eDestBlend;					// 78
	UInt32							eZTestFunc;					// 80
	char							ucAlphaTestRef;				// 84
	bool							bGrayscaleToColor;			// 85
	bool							bGrayscaleToAlpha;			// 86
	bool							bIgnoreTextureAlpha;		// 87
	bool							bBaseTextureProjectedUVs;	// 88
	bool							bIgnoreBaseGeomTexAlpha;	// 89
	bool							bLighting;					// 8A
	bool							bAlpha;						// 8B
};

// 70
class BSShaderProperty : public NiShadeProperty
{
public:
	float					fAlpha;					// 28
	SInt32					iLastRenderPassState;	// 2C - Set to 0x7FFFFFFF somekind of lock?
	UInt64					flags;					// 30
	BSRenderPass			* kRenderPassList;		// 38
	BSRenderPass			* kDebugRenderPassList;	// 40
	BSFadeNode				* pFadeNode;			// 48
	BSEffectShaderData		* pEffectData;			// 50
	BSShaderMaterial		* shaderMaterial;		// 58
	UInt32					uiLastAccumTime;		// 60
	float					fLODFade;				// 64
	BSNonReentrantSpinLock	ClearRenderPassesLock;	// 68

	enum : UInt64
	{
		kShaderFlags_GrayscaleToPalette	= 0x0000000080000000ULL,
		kShaderFlags_Hair				= 0x0000400000000000ULL,
		kShaderFlags_Refraction			= 0x0000000000008000ULL,
		kShaderFlags_EmitColor			= 0x0000000000400000ULL,
		kShaderFlags_NoExternalEmit		= 0x4000000000000000ULL
	};

	MEMBER_FN_PREFIX(BSShaderProperty);
	DEFINE_MEMBER_FN(SetMaterial, bool, 0x027BC560, BSShaderMaterial * material, bool unk1);
	DEFINE_MEMBER_FN(SetFlag, void, 0x027BC3E0, UInt8 flags, bool enabled); // sets or unsets particular flags
};
STATIC_ASSERT(sizeof(BSShaderProperty) == 0x70);

// 88
class BSEffectShaderProperty : public BSShaderProperty
{
public:
	BSParticleShaderCubeEmitter		* pEnvCubeEmitter;		// 70
	NiColor							* pExternalEmitColor;	// 78
	UInt32							uBaseTextureIndex;		// 80
	float							unk84;					// 84

	MEMBER_FN_PREFIX(BSEffectShaderProperty);
	DEFINE_MEMBER_FN(ctor, void, 0x027CAAC0);

	static BSEffectShaderProperty * Create();
};
STATIC_ASSERT(sizeof(BSEffectShaderProperty) == 0x88);

// E8
class BSLightingShaderProperty : public BSShaderProperty
{
public:
	NiColorA		kProjectedUVColor;				// 70
	NiColorA		kProjectedUVParams;				// 80
	BSRenderPass	* kDepthMapRenderPassListA[3];	// 90
	BSRenderPass	* pDepthRenderPass;				// A8
	BSRenderPass	* kForwardPassList;				// B0
	NiColor			* pEmissiveColor;				// B8
	BSFixedString	rootName;						// C0
	float			fEmitColorScale;				// C8
	float			fForcedDarkness;				// CC
	float			fSpecularLODFade;				// D0
	float			fEnvmapLODFade;					// D4
	UInt32			uiBaseTechniqueID;				// D8
	UInt32			ClearCommandBufferCount;		// DC
	UInt16			usDebugTintIndex;				// E0
	UInt16			uiStencilRef;					// E2
	UInt32			unkE4;							// E4

	MEMBER_FN_PREFIX(BSLightingShaderProperty);
	DEFINE_MEMBER_FN(ctor, BSLightingShaderProperty *, 0x027CC880);
	DEFINE_MEMBER_FN(MakeValidForRendering, void, 0x027CCB70, BSGeometry * geometry); // previously InvalidateShader
	DEFINE_MEMBER_FN(ApplyMaterial, bool, 0x00054170); // Calls BSShaderProperty::SetMaterial
	DEFINE_MEMBER_FN(LoadTextureSet, void, 0x027CD200, UInt32 unk1); // unk1 usually 0, called after material Releases textures (previously InvalidateTextures)

	static BSLightingShaderProperty * Create();
};
STATIC_ASSERT(sizeof(BSLightingShaderProperty) == 0xE8);

// C0
class BSWaterShaderProperty : public BSShaderProperty
{
public:
	UInt32						uiWaterFlags;						// 70
	UInt32						iStencilMask;						// 74
	NiPlane						kWaterPlane;						// 78
	BSRenderPass				* kSimpleRenderPassArray;			// 88
	BSRenderPass				* kRainOcclusionMapRenderPassList;	// 90
	BSShaderPropertyLightData	kLightData;							// 98
};

STATIC_ASSERT(sizeof(BSWaterShaderProperty) == 0xC0);
