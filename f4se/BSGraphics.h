#pragma once

#include "f4se_common/Relocation.h"
#include "f4se_common/Utilities.h"

#include "f4se/BSGeometry.h"
#include "f4se/NiTextures.h"

class BSLines;
class BSDynamicLines;
class NiDefaultAVObjectPalette;
class BSMeshLODTriShape;
class BSMultiStreamInstanceTriShape;
class NiPick;
class NiShadeProperty;
class NiStream;

class IShaderResourceManager
{
public:
	virtual ~IShaderResourceManager();

	struct FadeNodeSettings
	{
		const float* pFadeOutMultiplierAddr;
		const float* pMeshLODLevel1FadeAddr;
		const float* pMeshLODLevel2FadeAddr;
		float fDistanceMultiplier;
		float fMeshLODFadePercent;
		float fMeshLODFadeTime;
		float fFadeOutAlphaThreshold;
		float fFadeInAlphaThreshold;
		float fMaxFadeIncrement;
		float fActorAlphaFadeSeconds;
		float fFadeInTime;
		float fFadeOutTime;
		float fMeshLODFadeBound;
		int iFadeCounter;
		UInt32 uiFadeEnabled;
		UInt32 uiDrawFadeEnabled;
	};


	virtual BSTriShape* CreateTriShape(UInt32 auiNumTriangles, UInt16* apIndices, UInt32 auiNumVertices, NiPoint3* apPositions, NiPoint2* apTexCoords0, NiColorA* apTexCoords1, NiPoint3* apNormals, NiPoint3* apBinormals, NiPoint3* apTangents, NiColorA* apColors, NiColorA* apSkinBoneWeights, UInt8* apSkinBoneIndices, NiColorA* apLandscapeData1, NiColorA* apLandscapeData2, float* apEyeData, bool aGeneratePositionData);
	virtual BSTriShape* CreateTriShape(UInt32 auiNumTriangles, UInt32 auiNumVertices, NiPoint3* apPositions, NiPoint2* apTexCoords, NiColorA* apColors, UInt16* apIndices);
	virtual void* CreateTriShape(NiStream* kStream, UInt64 auiVertexDesc, UInt32 auiVertexCount, UInt32 auiIndexCount, char** apDynData);
	virtual void* CreateTriShapeRendererData(void* apVertexBuffer, UInt64 auiVertexDesc, UInt16* apIndices, UInt32 auiIndexCount);
	virtual BSSubIndexTriShape* CreateSubIndexTriShape(UInt32 auiNumTriangles, UInt32 auiNumSegments, UInt16* apIndices, UInt32 auiNumVertices, NiPoint3* apPositions, NiPoint2* apTexCoords0, NiColorA* apTexCoords1, NiPoint3* apNormals, NiPoint3* apBinormals, NiPoint3* apTangents, NiColorA* apColors, NiColorA* apSkinBoneWeights, UInt8* apSkinBoneIndices, NiColorA* apLandscapeData1, NiColorA* apLandscapeData2, float* apEyeData);
	virtual void IncRefTriShape(void* apRendererData);
	virtual void DecRefTriShape(void* apRendererData);
	virtual BSDynamicTriShape* CreateDynamicTriShape(UInt32 auiDynamicFlags, UInt32 auiNumTriangles, UInt16* apIndices, UInt32 auiNumVertices, NiPoint3* apPositions, NiPoint2* apTexCoords0, NiColorA* apTexCoords1, NiPoint3* apNormals, NiPoint3* apBinormals, NiPoint3* apTangents, NiColorA* apColors, NiColorA* apSkinBoneWeights, UInt8* apSkinBoneIndices, NiColorA* apLandscapeData1, NiColorA* apLandscapeData2, float* apEyeData);
	virtual BSDynamicTriShape* CreateDynamicTriShape(UInt32, UInt32, UInt32, NiPoint3*, NiPoint2*, NiColorA*, UInt16*);
	virtual void IncRefDynamicTriShape(void* apRendererData);
	virtual void DecRefDynamicTriShape(void* apRendererData);
	virtual void ConvertBSTriShapeToBSDynamicTriShape(NiNode* apRoot, NiDefaultAVObjectPalette* apObjectPal);
	virtual void ApplyMaterials(NiAVObject* apRoot);
	virtual void SetTriShapeStreamDynamicFlags(UInt32);
	virtual void*  CreateParticleShape();
	virtual void IncRefParticleShape(void* apRendererData);
	virtual void DecRefParticleShape(void* apRendererData);
	virtual BSLines*  CreateLineShape(UInt32, UInt16*, UInt32, NiPoint3*, NiPoint2*, NiColorA*, NiPoint3*, NiPoint3*, NiPoint3*, NiColorA*, NiColorA*, UInt8*, NiColorA*, NiColorA*, float*);
	virtual BSDynamicLines* CreateDynamicLineShape(UInt32, UInt32, UInt32, NiPoint3*, NiPoint2*, NiColorA*, UInt16*);
	virtual void* CreateDynamicLineShape(NiStream*, UInt64, UInt32, UInt32);
	virtual BSDynamicLines* CreateDynamicLineShape(UInt32, UInt32, UInt16*, UInt32, NiPoint3*, NiPoint2*, NiColorA*, NiPoint3*, NiPoint3*, NiPoint3*, NiColorA*, NiColorA*, UInt8*, NiColorA*, NiColorA*, float*);
	virtual void IncRefLines(void* apRendererData);
	virtual void DecRefLines(void* apRendererData);
	virtual void IncRefDynamicLines(void* apRendererData);
	virtual void DecRefDynamicLines(void* apRendererData);
	virtual void LoadTexture(NiTexture* apTexture);
	virtual void CreateStreamingTexture(NiTexture* apTexture, const void* apUserDataIn, void* apUserDataOut);
	virtual bool ReadStreamingTextureData(NiTexture* apTexture, void* apUserDataIn);
	virtual bool CreateStreamingTextureArraySlice(NiTexture* apTexture, const void* apUserDataIn, void* apUserDataOut, UInt32 auiSlice);
	virtual bool ReadStreamingTextureDataToArraySlice(NiTexture* apTexture, void* apUserDataIn, UInt32 auiSlice);
	virtual void FinishStreamingTexture(NiTexture* apTexture);
	virtual void IncRefTexture(BSGraphics::Texture* apRendererData);
	virtual void DecRefTexture(BSGraphics::Texture* apRendererData);
	virtual void GetTextureWidthHeight(NiTexture* apTexture, UInt32* auiWidth, UInt32* auiHeight);
	virtual UInt32 GetTextureFormat(NiTexture* apTexture);
	virtual bool UpdateTextureToDesiredMipLevel(NiTexture* apTexture, UInt32*);
	virtual void LoadUpgradeTextureData(NiTexture* apTexture);
	virtual bool UpdateStreamingTextureToDesiredMipLevel(NiTexture* apTexture, UInt32*, void*, void*);
	virtual void FinishStreamingTextureUpgade(NiTexture* apTexture);
	virtual void*  CreateVertexBuffer(UInt32* auiDataSize, void* apData, UInt32 auiStride, UInt64);
	virtual void DecRefVertexBuffer(void* apRendererData);
	virtual NiShadeProperty*  CreateDefaultEffectShaderProperty(bool abVertexColors, bool abSkinned);
	virtual BSMeshLODTriShape*  CreateMeshLODTriShape(UInt32*, UInt32, UInt64, UInt32, UInt32, void*);
	virtual BSMeshLODTriShape*  CreateMeshLODTriShape(BSTriShape*, UInt32*, UInt32);
	virtual UInt32 UpdateIndexBufferForInstancing(BSMultiStreamInstanceTriShape*, UInt32);
	virtual bool FindIntersectionsTriShapeFastPath(const NiPoint3*, const NiPoint3*, NiPick*, BSTriShape*);
	virtual void CreateTangentSpace(UInt32, UInt16*, UInt32, NiPoint3*, NiPoint2*, NiPoint3*, NiPoint3*, NiPoint3*);
	virtual float GetShaderFrameCount();
	virtual float GetShaderTimerDelta();
	virtual void GetFadeNodeSettings(FadeNodeSettings* abSkinned);
	virtual void GetCameraVectors(NiPoint3* arUp, NiPoint3* arRight, NiPoint3* arDir);
};

class BSShaderResourceManager
{
public:
	virtual ~BSShaderResourceManager();
};

namespace BSGraphics
{
class Renderer
{
public:
	UInt64				unk2588[0x2590 >> 3];	// 2588
	CRITICAL_SECTION	m_renderLock;			// 2590

	DEFINE_MEMBER_FN_4(CreateTriShape, TriShape*, 0x01812EB0, UInt32* arDataSize, void* apData, UInt64 auiVertexDesc, IndexBuffer* apIndexBuffer);
	DEFINE_MEMBER_FN_3(CreateVertexBuffer, VertexBuffer*, 0x01814720, UInt32* auiDataSize, SInt8* apData, UInt32 auiStride);
	DEFINE_MEMBER_FN_2(CreateIndexBuffer, IndexBuffer*, 0x01814A40, UInt32 auiIndexCount, const UInt16* apIndices);
};
STATIC_ASSERT(offsetof(Renderer, m_renderLock) == 0x2590);

// ??
class RenderTargetManager
{
public:
	// 20
	struct RenderTargetProperties
	{
		UInt32	uiWidth;					// 00
		UInt32	uiHeight;					// 04
		UInt32	eFormat;					// 08
		UInt32	uiMultiSample;				// 0C
		bool	bCopyable;					// 10
		bool	bSupportUnorderedAccess;	// 11
		bool	bAllowMipGeneration;		// 12
		bool	bForceLinear;				// 13
		SInt32	iMipLevel;					// 14
		UInt32	uiTextureTarget;			// 18
		bool	bEnableFastClear;			// 1C
		UInt8	pad[3];						// 1D
	};

	DEFINE_MEMBER_FN_1(LockTextureType, void, 0x01834F60, UInt32 type);
	DEFINE_MEMBER_FN_1(ReleaseTextureType, void, 0x01834FF0, UInt32 type);
	DEFINE_MEMBER_FN_4(GetRenderData, BSRenderData *, 0x01834EC0, UInt32 type, UInt64 unk1, UInt64 unk2, UInt32 unk3); // type, 0, 1, 0
	DEFINE_MEMBER_FN_3(Unk_01, void, 0x01834F10, UInt32 type, BSRenderData *, UInt8 unk3); // type, rendererData, 1
	DEFINE_MEMBER_FN_4(CreateRenderTarget, void, 0x018333C0, UInt32 aIndex, const RenderTargetProperties& properties, UInt8 aPersistent);
};
}

namespace BSShaderutil
{
	typedef void (*_ClearRenderPasses)(NiAVObject* object);
	extern RelocAddr< _ClearRenderPasses> ClearRenderPasses;
}

// 1B8
class ImageSpaceManager
{
public:

};

struct ID3D11DeviceContext;
struct ID3D11Device;

extern RelocPtr <ImageSpaceManager>						g_imageSpaceManager;
extern RelocPtr <BSGraphics::Renderer>					g_renderManager;
extern RelocPtr <BSGraphics::RenderTargetManager>		g_renderTargetManager;
extern RelocPtr <BSShaderResourceManager>				g_shaderResourceManager;
extern RelocPtr <ID3D11Device>							g_D3D11Device;
extern RelocPtr <ID3D11DeviceContext>					g_D3D11DeviceContext;
