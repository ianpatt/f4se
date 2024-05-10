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
}

class BSShaderResourceManager
{
public:
	virtual ~BSShaderResourceManager();

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
class BSRenderTargetManager
{
public:
	struct SharedTargetInfo
	{
		UInt32	width;		// 00 - 400
		UInt32	height;		// 04 - 400
		UInt32	unk08;		// 08 - 1C
		UInt32	unk0C;		// 0C - 10000
		UInt32	unk10;		// 10 - FFFFFFFF
		UInt32	unk14;		// 14 - r13d
		UInt32	unk18;		// 18 - r12b
	};

	MEMBER_FN_PREFIX(BSRenderTargetManager);
	// 
	DEFINE_MEMBER_FN(LockTextureType, void, 0x0171F9B0, UInt32 type);
	// 
	DEFINE_MEMBER_FN(ReleaseTextureType, void, 0x0171FA40, UInt32 type);
	DEFINE_MEMBER_FN(GetRenderData, BSRenderData *, 0x0171F910, UInt32 type, UInt64 unk1, UInt64 unk2, UInt32 unk3); // type, 0, 1, 0
	// 
	DEFINE_MEMBER_FN(Unk_01, void, 0x0171F960, UInt32 type, BSRenderData *, UInt8 unk3); // type, rendererData, 1
};

// 1B8
class ImageSpaceManager
{
public:

};

struct ID3D11DeviceContext;
struct ID3D11Device;

extern RelocPtr <ImageSpaceManager>			g_imageSpaceManager;
extern RelocPtr <BSGraphics::Renderer>		g_renderManager;
extern RelocPtr <BSRenderTargetManager>		g_renderTargetManager;
extern RelocPtr <BSShaderResourceManager>	g_shaderResourceManager;
extern RelocPtr <ID3D11Device>				g_D3D11Device;
extern RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext;
