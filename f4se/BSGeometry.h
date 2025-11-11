#pragma once

#include "f4se/NiObjects.h"
#include "f4se/BSSkin.h"

class NiProperty;
struct ID3D11Buffer;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;
struct ID3D11Texture2D;

// 38
struct BSGeometrySegmentFlagData
{
	tHashSet<UInt32,BSFixedString>	SegmentDeltas;	// 00
	BSFixedString					BaseBoneName;	// 30
};

// 68
class BSGeometrySegmentSharedData : public BSIntrusiveRefCounted
{
public:
	BSFixedString	SSFFileName;		// 08
	UInt32			uiNumSegments;		// 10
	UInt32			uiTotalNumSegments;	// 14
	UInt32			* pSegmentStarts;	// 18

	// 28
	struct PerSegmentSharedData
	{
		UInt32	uiUserIndex;			// 00
		UInt32	uiBoneID;				// 04
		float	fValidCutOffsets[8];	// 08
	};

	PerSegmentSharedData *pPerSegmentSharedData;	// 20
	BSGeometrySegmentFlagData SegmentsEnabledData;	// 28
	bool bProcessedCutOffsets;						// 60
};

// 40
class BSGeometrySegmentData : public NiObject
{
public:
	// 18
	struct Segment
	{
		UInt32 uiStartIndex;		// 00
		UInt32 uiNumPrimitives;		// 04
		UInt32 uiParentArrayIndex;	// 08
		UInt32 uiChildCount;		// 0C
		UInt8 ucDisabledCount;		// 10
	};

	// 8
	struct DrawData
	{
		UInt32 uiStartIndex;	// 00
		UInt32 uiNumPrimitives;	// 04
	};

	BSGeometrySegmentSharedData	* spSharedData;			// 10
	Segment						* pSegments;			// 18
	DrawData					* pSegmentDrawData;		// 20
	UInt32						uiNumDraws;				// 28
	UInt32						uiNumSegments;			// 2C
	UInt32						uiTotalNumSegments;		// 30
	UInt32						uiTotalNumPrimitives;	// 34
	UInt32						uiSegToZeroMap;			// 38
	bool						bSegmentsChanged;		// 3C
	bool						bIgnoreSegments;		// 3D
};

namespace BSGraphics
{
struct Buffer
{
  ID3D11Buffer* pBuffer;
  void* pData;
  Buffer* pNext;
  ID3D11ShaderResourceView* pShaderResource;
  ID3D11UnorderedAccessView* pUnorderedAccess;
  void** pRequestEventToWait; // BSEventFlag
  UInt32 uiMaxDataSize;
  UInt32 uiDataSize;
  UInt32 uiRefCount;
  UInt32 SRAcquire; // Atomic
  UInt32 UAVAcquire; // Atomic
  UInt32 PendingRequests; // Atomic
  UInt32 DataOffset;
  bool InvalidCpuData;
  bool HeapAllocated;
  volatile bool PendingCopy;
};
struct VertexBuffer : public Buffer {};
struct IndexBuffer : public Buffer {};
struct ByteBuffer : public Buffer {};

struct TriShape
{
	UInt64 uiVertexDesc;
	BSGraphics::VertexBuffer* pVB;
	BSGraphics::IndexBuffer* pIB;
	UInt32 uiRefCount;
};
struct DynamicTriShape : public TriShape
{
	UInt32 uiDynamicBufferSize;
};

struct TextureStreamData
{
	UInt32 uiRefCount;
	UInt8 ucDataFileIndex;
	UInt8 ucChunkCount;
	UInt16 usMinLOD;
};
struct TextureHeader
{
	union
	{
		UInt64 ui64;
		struct
		{
			UInt16 uiHeight;
			UInt16 uiWidth;
			UInt8 uiMipCount;
			UInt8 uiFormat;
			UInt8 uiFlags;
			UInt8 uiTilemode;
		} __s1;
	} __u0;
};
struct Texture
{
	ID3D11ShaderResourceView* pSRView;
	ID3D11Texture2D* pTexture2D;
	ID3D11UnorderedAccessView* pUAView;
	BSGraphics::TextureStreamData* pStreamData;
	void** pRequestEventToWait; // BSEventFlag
	BSGraphics::TextureHeader Header;
	UInt32 PendingRequests; // Atomic
	UInt32 uiRefCount;
	UInt32 uiCreationFrame;
	UInt8 uiMinLOD;
	UInt8 uiDegradeLevel;
	UInt8 usDesiredDegradeLevel;
	UInt8 usFlags;
};
}

// 160
class BSGeometry : public NiAVObject
{
public:
	virtual void Unk_39();
	virtual void Unk_3A();
	virtual void Unk_3B();
	virtual void Unk_3C();
	virtual void Unk_3D();
	virtual void Unk_3E();
	virtual void Unk_3F();
	virtual void Unk_40();

	NiBound kModelBound;					// 120
	NiPointer<NiProperty> effectState;		// 130
	NiPointer<NiProperty> shaderProperty;	// 138
	NiPointer<BSSkin::Instance>	skinInstance;		// 140

	union VertexDesc
	{
		struct
		{
			UInt8 szVertexData : 4;
			UInt8 szVertex : 4;  // 0 when not dynamic
			UInt8 oTexCoord0 : 4;
			UInt8 oTexCoord1 : 4;
			UInt8 oNormal : 4;
			UInt8 oTangent : 4;
			UInt8 oColor : 4;
			UInt8 oSkinningData : 4;
			UInt8 oLandscapeData : 4;
			UInt8 oEyeData: 4;
			UInt16 vertexFlags : 16;
			UInt8 unused : 8;
		};
		UInt64 vertexDesc;
	};

	enum : UInt64
	{
		kFlag_Unk1			= (1ULL << 40),
		kFlag_Unk2			= (1ULL << 41),
		kFlag_Unk3			= (1ULL << 42),
		kFlag_Unk4			= (1ULL << 43),
		kFlag_Vertex		= (1ULL << 44),
		kFlag_UVs			= (1ULL << 45),
		kFlag_Unk5			= (1ULL << 46),
		kFlag_Normals		= (1ULL << 47),
		kFlag_Tangents		= (1ULL << 48),
		kFlag_VertexColors	= (1ULL << 49),
		kFlag_Skinned		= (1ULL << 50),
		kFlag_Unk6			= (1ULL << 51),
		kFlag_MaleEyes		= (1ULL << 52),
		kFlag_Unk7			= (1ULL << 53),
		kFlag_FullPrecision	= (1ULL << 54),
		kFlag_Unk8			= (1ULL << 55),
	};

	void*			pRendererData;			// 148
	UInt64			vertexDesc;				// 150

	UInt16 GetVertexSize() const { return (vertexDesc << 2) & 0x3C; } // 0x3C might be a compiler optimization, (vertexDesc & 0xF) << 2 makes more sense

	SInt8	ucType;							// 158
	bool	Registered;						// 159
	UInt16	pad15A;							// 15A
	UInt32	unk15C;							// 15C

	MEMBER_FN_PREFIX(BSGeometry);
	// 
	DEFINE_MEMBER_FN(UpdateShaderProperty, UInt32, 0x021BB340);
};
STATIC_ASSERT(sizeof(BSGeometry) == 0x160);

// 170
class BSTriShape : public BSGeometry
{
public:
	UInt32	numTriangles;	// 160
	UInt16	numVertices;	// 164
	UInt16	unk166;	// 166
	float	unk168;	// 168
	float	unk16C;	// 16C

	MEMBER_FN_PREFIX(BSTriShape);
	DEFINE_MEMBER_FN(CreateDynamicTriShape, BSDynamicTriShape*, 0x0182BFE0, NiAVObject * unk1);
};
STATIC_ASSERT(sizeof(BSTriShape) == 0x170);

// 1A0
class BSDynamicTriShape : public BSTriShape
{
public:
	UInt32								uiDynamicDataSize;	// 170
	UInt32								uiFrameCount;		// 174
	SimpleLock							lock;				// 178
	UInt8								* dynamicVertices;	// 180 - geometry pointer, must lock/unlock when altering
	NiPointer<BSGeometrySegmentData>	spSegments;		// 188
	void								* unk190;			// 190
	void								* unk198;			// 198

	UInt16 GetDynamicVertexSize() const { return (vertexDesc >> 2) & 0x3C; }
};
STATIC_ASSERT(sizeof(BSDynamicTriShape) == 0x1A0);

// 190
class BSSubIndexTriShape : public BSTriShape
{
public:
	NiPointer<BSGeometrySegmentData> spSegments;	// 170

	struct SegmentData
	{
		ID3D11Buffer	* d3d11Buffer;	// 00 - const CLayeredObjectWithCLS<class CBuffer>::CContainedObject::vftable'{forCPrivateDataImpl<struct ID3D11Buffer>'}
		UInt8			* segmentBlock;	// 08
		UInt64			unk10;			// 10
		UInt64			unk18;			// 18
		UInt64			unk20;			// 20
		void			* unk28;		// 28
		UInt32			unk30;			// 30
		UInt32			unk34;			// 34
		volatile SInt32	refCount;		// 38
	};

	SegmentData		* segmentData;	// 178
	UInt32			numIndices;		// 180
	UInt32			unk184;			// 184
	void			* unk188;		// 188
};
STATIC_ASSERT(sizeof(BSSubIndexTriShape) == 0x190);

// Offset is the amount of bytes until the next iteration
// e.g.
// ConvertHalfToFloat(&in, 0x04, &out, 0x02, 1);
// Converts a single float to a single half-float
typedef void (* _ConvertHalfToFloat)(float * src, UInt64 offsetFloat, UInt16 * dest, UInt64 offsetHalf, UInt64 count);
extern RelocAddr <_ConvertHalfToFloat> ConvertHalfToFloat;
