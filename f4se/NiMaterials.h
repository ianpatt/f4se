#pragma once

#include "f4se/GameTypes.h"
#include "f4se/NiTypes.h"
#include "f4se/NiTextures.h"
#include "f4se/NiProperties.h"

class NiTexture;
class BSTextureSet;
class NiStream;
class BSShaderData;
class BSTriShape;

class BSLightingShaderPropertyFloatController;
class BSLightingShaderPropertyUShortController;
class BSLightingShaderPropertyColorController;
class BSEffectShaderPropertyFloatController;
class BSEffectShaderPropertyColorController;
struct BSNiAlphaPropertyTestRefController;

// 38
class BSShaderMaterial
{
public:
	virtual ~BSShaderMaterial();

	virtual BSShaderMaterial	* Create();
	virtual void	CopyMembers(const BSShaderMaterial * other);
	virtual void	ComputeCRC32(UInt32, bool);
	virtual BSShaderMaterial	* GetDefault();
	virtual UInt32	GetType();
	virtual UInt32	GetFeature(); // 2 for Lighting shaders, 1 for effect shaders
	virtual void	Compare(const BSShaderMaterial & other);
	virtual void	IsCopy(const BSShaderMaterial * other);

	volatile UInt32	m_refCount;		// 08
	NiPoint2	textCoordOffset[2];	// 0C
	NiPoint2	textCoordScale[2];	// 1C
	SInt32		unk2C;				// 2C
	UInt32		uiHashKey;			// 30
	UInt32		uiUniqueCode;		// 34

	void SetOffsetUV(float u, float v)
	{
		textCoordOffset[0].x = u;
		textCoordOffset[1].x = u;
		textCoordOffset[0].y = v;
		textCoordOffset[1].y = v;
	}

	void GetOffsetUV(float * u, float * v)
	{
		*u = textCoordOffset[0].x;
		*v = textCoordOffset[0].y;
	}

	void SetScaleUV(float u, float v)
	{
		textCoordScale[0].x = u;
		textCoordScale[1].x = u;
		textCoordScale[0].y = v;
		textCoordScale[1].y = v;
	}

	void GetScaleUV(float * u, float * v)
	{
		*u = textCoordScale[0].x;
		*v = textCoordScale[0].y;
	}
};

// B8
class BSEffectShaderMaterial : public BSShaderMaterial
{
public:
	virtual ~BSEffectShaderMaterial();

	float					fFalloffStartAngle;				// 38
	float					fFalloffStopAngle;				// 3C
	float					fFalloffStartOpacity;			// 40
	float					fFalloffStopOpacity;			// 44
	NiColorA				kBaseColor;						// 48 - Alpha is Material Alpha
	NiPointer<NiTexture>	spBaseTexture;					// 58
	NiPointer<NiTexture>	spGrayscaleTexture;				// 60
	NiPointer<NiTexture>	spEnvironmentMapTexture;		// 68
	NiPointer<NiTexture>	spEnvironmentMapMaskTexture;	// 70
	NiPointer<NiTexture>	spNormalTexture;				// 78
	float					fSoftDepth;						// 80
	float					fBaseColorScale;				// 84
	BSFixedString			strBaseTexture;					// 88
	BSFixedString			strGrayscaleTexture;			// 90
	BSFixedString			strEnvironmentMapTexture;		// 98
	BSFixedString			strEnvironmentMapMaskTexture;	// A0
	BSFixedString			strNormalTexture;				// A8
	union
	{
		float				fEnvironmentMaskScale;			// B0
		float				fRefractionPower;				// B0 - Needs kShaderFlags_Refraction
	};
	UInt8					uTextureClampMode;				// B4
	UInt8					uLightingInfluence;				// B5 - divided by 255
	UInt8					uEnvironmentMapMinLOD;			// B6
	UInt8					unkB7;							// B7
};
STATIC_ASSERT(sizeof(BSEffectShaderMaterial) == 0xB8);

// 168
class BSWaterShaderMaterial : public BSShaderMaterial
{
public:
	NiPointer<NiTexture>	spStaticReflectionMap;			// 38
	NiPointer<NiTexture>	spNormalMap01;					// 40
	NiPointer<NiTexture>	spNormalMap02;					// 48
	NiPointer<NiTexture>	spNormalMap03;					// 50
	NiColorA				kShallowColor;					// 58
	NiColorA				kDeepColor;						// 68
	NiColorA				kReflectionColor;				// 78
	NiColorA				kUnderwaterFogColor;			// 88
	NiColorA				kLightSiltColor;				// 98
	NiColorA				kDarkSiltColor;					// A8
	NiColorA				kVarAmounts;					// B8
	NiColorA				kWaterParams;					// C8
	NiColorA				kWaterParams2;					// D8
	NiColorA				kWaterParams3;					// E8
	NiColorA				kWaterParams4;					// F8
	NiColorA				kNormalsScroll1;				// 108
	NiColorA				kNormalsScroll2;				// 118
	NiColorA				kNormalsScale;					// 128
	NiColorA				kNormalsAmplitude;				// 138
	NiPlane					kReflectPlane;					// 148
	SInt32					uwTexOffsetX;					// 158
	SInt32					uwTexOffsetY;					// 15C
	bool					bUpdateConstants;				// 160
	bool					bUseDefaultReflectionTexture;	// 161
	bool					bUseSSR;						// 162
};
STATIC_ASSERT(sizeof(BSWaterShaderMaterial) == 0x168);

// C0
class BSLightingShaderMaterialBase : public BSShaderMaterial
{
public:
	enum ShaderTypes
	{
		kType_Default = 0,
		kType_Envmap,
		kType_Glowmap,
		kType_Parallax,
		kType_Face,
		kType_SkinTint,
		kType_HairTint,
		kType_ParallaxOcc,
		kType_Landscape,
		kType_LODLandscape,
		kType_Snow,
		kType_MultiLayerParallax,
		kType_TreeAnim,
		kType_LODObjects,
		kType_MultiIndexSnow,
		kType_LODObjectsHD,
		kType_Eye,
		kType_Cloud,
		kType_LODLandscapeNoise,
		kType_LODLandscapeBlend,
		kType_Dismemberment
	};

	virtual bool	IsLightingShaderMaterialEnvmap();
	virtual void	ClearTextures();
	virtual void	GetTextures(NiTexture ** textures);	// Returns textures into this array
	virtual void	SaveBinary(NiStream * stream);
	virtual void	LoadBinary(NiStream * stream);
	virtual void	OnPrefetchTextures();
	virtual void	OnLoadTextureSet1();
	virtual void	OnLoadTextureSet(const BSTextureSet *);
	virtual void	DoReceiveValuesFromRootMaterial(const BSShaderData &);

	NiColor					kSpecularColor;					// 38
	NiPointer<NiTexture>	spDiffuseTexture;				// 48
	NiPointer<NiTexture>	spNormalTexture;				// 50
	NiPointer<NiTexture>	spRimSoftLightingTexture;		// 58
	NiPointer<NiTexture>	spSmoothnessSpecMaskTexture;	// 60
	NiPointer<NiTexture>	spLookupTexture;				// 68
	UInt32					eTextureClampMode;				// 70
	UInt32					unk74;							// 74
	NiPointer<BSTextureSet>	spTextureSet;					// 78
	float					fMaterialAlpha;					// 80
	float					fRefractionPower;				// 84
	float					fSmoothness;					// 88
	float					fSpecularColorScale;			// 8C
	float					fFresnelPower;					// 90
	float					fWetnessControl_SpecScale;		// 94
	float					fWetnessControl_SpecPowerScale;	// 98
	float					fWetnessControl_SpecMin;		// 9C
	float					fWetnessControl_EnvMapScale;	// A0
	float					fWetnessControl_FresnelPower;	// A4
	float					fWetnessControl_Metalness;		// A8
	float					fSubsurfaceLightRolloff;		// AC
	float					fRimLightPower;					// B0
	float					fBackLightPower;				// B4
	float					fLookupScale;					// B8
	BSNonReentrantSpinLock	LoadTextureSetLock;				// BC

	MEMBER_FN_PREFIX(BSLightingShaderMaterialBase);
	DEFINE_MEMBER_FN(Copy, void, 0x02825C70, const BSLightingShaderMaterialBase * src);
};

// D0
class BSLightingShaderMaterial : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterial();
};

// C8
class BSLightingShaderMaterialDismemberment : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialDismemberment();

	NiPointer<NiTexture>	spDisplacementTexture;	// C0
};

// D8
class BSLightingShaderMaterialEnvmap : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialEnvmap();

	NiPointer<NiTexture>	spEnvTexture;								// C0
	NiPointer<NiTexture>	spEnvMaskTexture;							// C8
	float					fEnvmapScale;								// D0
	bool					bUseScreenSpaceReflections;					// D4
	bool					bWetnessControl_UseScreenSpaceReflections;	// D5
};

// 110
class BSLightingShaderMaterialEye : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialEye();

	NiPointer<NiTexture>	spEnvTexture;		// E0
	NiPointer<NiTexture>	spEnvMaskTexture;	// E8
	NiPoint3				kEyeCenter[2];		// F0
	float					fEnvmapScale;		// 108
};

// C8
class BSLightingShaderMaterialFace : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialFace();

	NiPointer<NiTexture>	baseDiffuse;	// C0
};

// C8
class BSLightingShaderMaterialGlowmap : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialGlowmap();

	NiPointer<NiTexture>	spGlowMapTexture;	// C0
};

// D0
class BSLightingShaderMaterialHairTint : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialHairTint();
	
	NiColorA	kTintColor;	// C0
};

// 110
class BSLightingShaderMaterialLODLandscape : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialLODLandscape();

	NiPointer<NiTexture>	spParentDiffuseTexture;			// E0
	NiPointer<NiTexture>	spParentNormalTexture;			// E8
	NiPointer<NiTexture>	spLandscapeNoiseTexture;		// F0
	NiPointer<NiTexture>	spLandscapeDetailNormalTexture;	// F8
	float					fTerrainTexOffsetX;				// 100
	float					fTerrainTexOffsetY;				// 104
	float					fTerrainTexFade;				// 108
};

// 1B0
class BSLightingShaderMaterialLandscape : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialLandscape();

	bool					bLandscape_WetnessControl_UseScreenSpaceReflections[3];	// E0
	float					fLandscape_WetnessControl_SpecScale[3];					// E4
	float					fLandscape_WetnessControl_SpecPowerScale[3];			// F0
	float					fLandscape_WetnessControl_SpecMin[3];					// FC
	float					fLandscape_WetnessControl_EnvMapScale[3];				// 108
	float					fLandscape_WetnessControl_FresnelPower[3];				// 114
	float					fLandscape_WetnessControl_Metalness[3];					// 120
	UInt32					uiNumLandscapeTextures;									// 12C
	NiPointer<NiTexture>	spLandscapeDiffuseTexture[3];							// 130
	NiPointer<NiTexture>	spLandscapeNormalTexture[3];							// 148
	NiPointer<NiTexture>	spLandscapeSmoothSpecTexture[3];						// 160
	NiPointer<NiTexture>	spLandscapeLODBlendTexture;								// 178
	NiPointer<NiTexture>	spLandscapeNoiseTexture;								// 180
	NiPointer<NiTexture>	spLandscapeDetailNormalTexture;							// 188
	NiColorA				kLandBlendParams;										// 190
	float					fTerrainTexOffsetX;										// 1A0
	float					fTerrainTexOffsetY;										// 1A4
	float					fTerrainTexFade;										// 1A8
};

// F0
class BSLightingShaderMaterialMultiLayerParallax : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialMultiLayerParallax();

	NiPointer<NiTexture>	spLayerTexture;				// C0
	NiPointer<NiTexture>	spEnvTexture;				// C8
	NiPointer<NiTexture>	spEnvMaskTexture;			// D0
	float					fParallaxLayerThickness;	// D8
	float					fParallaxRefractionScale;	// DC
	float					fParallaxInnerLayerUScale;	// E0
	float					fParallaxInnerLayerVScale;	// E4
	float					fEnvmapScale;				// E8
};

// C8
class BSLightingShaderMaterialParallax : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialParallax();

	NiPointer<NiTexture> spHeightTexture; // C0
};

// D0
class BSLightingShaderMaterialParallaxOcc : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialParallaxOcc();

	NiPointer<NiTexture>	spHeightTexture;		// C0
	float					fParallaxOccMaxPasses;	// C8
	float					fParallaxOccScale;		// CC
};

// D0
class BSLightingShaderMaterialSkinTint : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialSkinTint();

	NiColorA	kTintColor;	// C0
};

// D0
class BSLightingShaderMaterialSnow : public BSLightingShaderMaterialBase
{
public:
	virtual ~BSLightingShaderMaterialSnow();

	float	unkC0;	// C0
	float	unkC4;	// C4
	float	unkC8;	// C8
	float	unkCC;	// CC
};

typedef BSShaderMaterial * (* _CreateShaderMaterialByType)(UInt32 type);
extern RelocAddr <_CreateShaderMaterialByType> CreateShaderMaterialByType;

// 1E0
class BSShaderData : public BSIntrusiveRefCounted
{
public:
	BSShaderData()
	{
		CALL_MEMBER_FN(this, ctor)();
	}
	~BSShaderData()
	{
		CALL_MEMBER_FN(this, dtor)();
	}

	UInt64	unk08[(0x1E0 - 0x08) >> 3];

	// Pulled from Export PDB
	/*enum LightingShaderEnum
	{
		kStandard = 0,
		kWater,
		kEffect,
		kSky,
		kTallGrass,
		kCount,
	};

	LightingShaderEnum	eLightingShader;
	bool				bSpecularEnabled;
	NiColor				SpecularColor;
	float				fSpecularMult;
	NiColor				EmittanceColor;
	float				fEmittanceColorScale;
	float				fLightingInfluence;
	char				ucEnvmapMinLOD;
	float				fSmoothness;
	float				fFresnelPower_ShaderDataEntry;
	float				fWetnessControl_SpecScale_ShaderDataEntry;
	float				fWetnessControl_SpecPowerScale_ShaderDataEntry;
	float				fWetnessControl_SpecMin_ShaderDataEntry;
	float				fWetnessControl_EnvMapScale_ShaderDataEntry;
	float				fWetnessControl_FresnelPower_ShaderDataEntry;
	float				fWetnessControl_Metalness_ShaderDataEntry;
	bool				bWetnessControl_ScreenSpaceReflections;
	BSFixedString		rootMaterialPath;
	float				fAlpha;
	float				fEmittanceMult;
	float				fLookupScale;
	bool				bEmitEnabled;
	BSFixedString		materialPath;
	bool				bAlphaBlend;
	NiAlphaProperty::AlphaFunction				eAlphaSrcBlendMode;
	NiAlphaProperty::AlphaFunction				eAlphaDestBlendMode;
	bool				bAlphaTest;
	NiAlphaProperty::TestFunction				eAlphaTestFunction;
	char				ucAlphaTestRef;
	bool				bEnableEditorAlphaRef;
	bool				bVertexColors;
	bool				bVertexAlpha;
	bool				bModelSpaceNormals;
	bool				bDecal;
	bool				bExternalEmittance;
	bool				bHideSecret;
	bool				bNoFade;
	bool				bDismemberment;
	bool				bFacegen;
	bool				SkinTint;
	bool				bHair;
	bool				bTwoSided;
	bool				bTree;
	bool				bBloodEnabled;
	bool				bUniqueAlphaProperty;
	bool				bEffectLightingEnabled;
	bool				bCastShadows;
	bool				bReceiveShadows;
	bool				bDissolveFade;
	bool				bGlowmap;
	bool				bAssumeShadowmask;
	bool				bScreenSpaceReflections;
	bool				bNonOccluder;
	bool				bSkewSpecularAlpha;
	bool				Tessellate;
	bool				bBackLighting;
	bool				bSubSurfaceLighting;
	bool				bRimLighting;
	float				fSubSurfaceRolloff;
	float				fRimPower;
	float				fBackLightPower;
	bool				bAnisoLighting;
	bool				bZBufferWrite;
	bool				bZBufferTest;
	bool				bRefraction;
	float				fRefractionPower;
	bool				bRefractionFalloff;
	tArray<BSLightingShaderPropertyFloatController*> pLightingFloatControllerA;
	tArray<BSLightingShaderPropertyUShortController*> pLightingUShortControllerA;
	tArray<BSLightingShaderPropertyColorController*> pLightingColorControllerA;
	tArray<BSEffectShaderPropertyFloatController*> pEffectFloatControllerA;
	tArray<BSEffectShaderPropertyColorController*> pEffectColorControllerA;
	BSNiAlphaPropertyTestRefController* pAlphaRefTestController;
	bool				bParallax;
	bool				bParallaxOcclusion;
	bool				bMultiLayerParallax;
	float				fParallaxOcclusionHeightScale;
	float				fParallaxOcclusionMaxPasses;
	float				fParallaxLayerThickness;
	float				fParallaxRefractionScale;
	float				fParallaxInnerLayerUScale;
	float				fParallaxInnerLayerVScale;
	float				fDisplacementTextureBias;
	float				fDisplacementTextureScale;
	float				fTessellationPNScale;
	float				fTessellationFadeDistance;
	float				fTessellationBaseFactor;
	bool				bTessellationNeedsDominantUVs;
	bool				bTessellationNeedsCrackFreeNormals;
	bool				bEnvironmentMapping;
	bool				bEnvironmentMappingEye;
	bool				bEnvironmentMappingWindow;
	float				fEnvironmentMappingMaskScale;
	int					iSkyType;
	bool				bReflectionsEnabled;
	bool				bSilhouetteReflectionsEnabled;
	bool				bRefractionsEnabled;
	bool				bDepthEnabled;
	bool				bVertexUVEnabled;
	bool				bVertexAlphaDepthEnabled;
	bool				bFoggingEnabled;
	bool				bFalloffEnabled;
	bool				bRGBFalloffEnabled;
	NiColorA			FalloffData;
	bool				bSoftEnabled;
	float				fSoftDepth;
	NiPoint2			UVOffset;
	NiPoint2			UVScale;
	bool				bGrayscaleToPaletteColor;
	bool				bGrayscaleToPaletteAlpha;
	NiColor				HairTintColor;
	NiPointer<BSShaderTextureSet> spTextureSet;
	UInt32				eClampMode;*/

	MEMBER_FN_PREFIX(BSShaderData);
	DEFINE_MEMBER_FN(ctor, void, 0x027BD630);
	DEFINE_MEMBER_FN(dtor, void, 0x003901F0);
	DEFINE_MEMBER_FN(ApplyMaterialData, void, 0x027C4550, BSGeometry * trishape, bool unk1);
};
STATIC_ASSERT(sizeof(BSShaderData) == 0x1E0);

typedef UInt32 (* _LoadMaterialFile)(const char * filePath, BSShaderData * shaderData, UInt32 unk1);
extern RelocAddr <_LoadMaterialFile> LoadMaterialFile;
