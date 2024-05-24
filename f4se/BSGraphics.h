#pragma once

#include "f4se_common/Relocation.h"
#include "f4se_common/Utilities.h"

#include "f4se/BSGeometry.h"
#include "f4se/NiTextures.h"

class BSGeometryData;

namespace BSGraphics
{
class Renderer
{
public:
	UInt64				unk2588[0x2590 >> 3];	// 2588
	CRITICAL_SECTION	m_renderLock;			// 2590

	MEMBER_FN_PREFIX(Renderer);
	DEFINE_MEMBER_FN(CreateBSGeometryData, BSGeometryData*, 0x016FD950, UInt32 * blockSize, UInt8 * vertexData, UInt64 vertexDesc, BSGeometryData::TriangleData * triData); // Creates a block with a vertex copy in the resource pool with a reference to the supplied triblock (partial deep copy)
};
STATIC_ASSERT(offsetof(Renderer, m_renderLock) == 0x2590);

class ShaderResourceManager
{
public:
	virtual ~ShaderResourceManager();

	virtual void Unk_01();
	virtual void Unk_02();
	virtual void Unk_03();
	virtual void Unk_04();
	virtual void Unk_05();
	virtual void Unk_06();
	virtual void Unk_07();
	virtual void IncGeometryRef(BSGeometryData * geomData);
	virtual void DefGeometryRef(BSGeometryData * geomData); // Will auto-destroy the block when it reaches zero
	//... 

	// Unk_21(BSRenderData * rendererData); // Release texture?
};

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

	DEFINE_MEMBER_FN_1(LockTextureType, void, 0x0171F9B0, UInt32 type);
	DEFINE_MEMBER_FN_1(ReleaseTextureType, void, 0x0171FA40, UInt32 type);
	DEFINE_MEMBER_FN_4(GetRenderData, BSRenderData *, 0x0171F910, UInt32 type, UInt64 unk1, UInt64 unk2, UInt32 unk3); // type, 0, 1, 0
	DEFINE_MEMBER_FN_3(Unk_01, void, 0x0171F960, UInt32 type, BSRenderData *, UInt8 unk3); // type, rendererData, 1
	DEFINE_MEMBER_FN_4(CreateRenderTarget, void, 0x0171DE10, UInt32 aIndex, const RenderTargetProperties& properties, UInt8 aPersistent);
};
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
extern RelocPtr <BSGraphics::ShaderResourceManager>		g_shaderResourceManager;
extern RelocPtr <ID3D11Device>							g_D3D11Device;
extern RelocPtr <ID3D11DeviceContext>					g_D3D11DeviceContext;
