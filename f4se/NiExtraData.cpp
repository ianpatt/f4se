#include "f4se/NiExtraData.h"
#include "f4se/BSGeometry.h"

// ??_7NiStringExtraData@@6B@
RelocAddr<uintptr_t> s_NiStringExtraDataVtbl(0x02E15AA8);
// ??_7BSFaceGenBaseMorphExtraData@@6B@
RelocAddr<uintptr_t> s_BSFaceGenBaseMorphExtraDataVtbl(0x02CEB188);
// ??_7BSDynPosData@@6B@
RelocAddr<uintptr_t> s_BSDynPosDataVtbl(0x02E16D08);
// ??_7NiBinaryExtraData@@6B@
RelocAddr<uintptr_t> s_NiBinaryExtraDataVtbl(0x02E19808);

NiStringExtraData * NiStringExtraData::Create(const BSFixedString & name, const BSFixedString & string)
{
	void* memory = Heap_Allocate(sizeof(NiStringExtraData));
	memset(memory, 0, sizeof(NiStringExtraData));
	((UInt64*)memory)[0] = s_NiStringExtraDataVtbl.GetUIntPtr();
	NiStringExtraData * data = (NiStringExtraData*)memory;
	data->m_name = name;
	data->m_string = string;
	return data;
}

BSDynPosData* BSDynPosData::Create(const BSFixedString & name, BSTriShape * shape)
{
	void* memory = Heap_Allocate(sizeof(BSDynPosData));
	memset(memory, 0, sizeof(BSDynPosData));
	((UInt64*)memory)[0] = s_BSDynPosDataVtbl.GetUIntPtr();
	BSDynPosData * data = (BSDynPosData*)memory;
	data->m_name = name;

	UInt16 vertexSize = shape->GetVertexSize();
	UInt32 dynamicBlock = sizeof(UInt16) * 4;

	data->vertexData = (UInt8*)Heap_Allocate(shape->numVertices * dynamicBlock);
	for(UInt32 i = 0; i < shape->numVertices; i++)
	{
		memcpy_s(&data->vertexData[i * dynamicBlock], dynamicBlock, &shape->geometryData->vertexData->vertexBlock[i * vertexSize], dynamicBlock);
	}
	return data;
}

BSFaceGenBaseMorphExtraData * BSFaceGenBaseMorphExtraData::Create(BSTriShape * shape)
{
	BSFaceGenBaseMorphExtraData * data = (BSFaceGenBaseMorphExtraData*)Heap_Allocate(sizeof(BSFaceGenBaseMorphExtraData));
	CALL_MEMBER_FN(data, ctor)(shape);
	return data;
}

BSFaceGenBaseMorphExtraData* BSFaceGenBaseMorphExtraData::Create(const BSFixedString & name, UInt32 vertexCount)
{
	void* memory = Heap_Allocate(sizeof(BSFaceGenBaseMorphExtraData));
	memset(memory, 0, sizeof(BSFaceGenBaseMorphExtraData));
	((UInt64*)memory)[0] = s_BSFaceGenBaseMorphExtraDataVtbl.GetUIntPtr();
	BSFaceGenBaseMorphExtraData * data = (BSFaceGenBaseMorphExtraData*)memory;
	data->m_name = name;
	data->modelVertexCount = vertexCount;
	data->vertexCount = vertexCount;
	data->vertexData = (NiPoint3*)Heap_Allocate(sizeof(NiPoint3) * vertexCount);
	memset(data->vertexData, 0, sizeof(NiPoint3) * vertexCount);
	return data;
}

NiBinaryExtraData* NiBinaryExtraData::Create(const BSFixedString & name, UInt32 size)
{
	void* memory = Heap_Allocate(sizeof(NiBinaryExtraData));
	memset(memory, 0, sizeof(NiBinaryExtraData));
	((UInt64*)memory)[0] = s_NiBinaryExtraDataVtbl.GetUIntPtr();
	NiBinaryExtraData * data = (NiBinaryExtraData*)memory;
	data->m_name = name;
	data->binaryData = (UInt8*)Heap_Allocate(size);
	data->size = size;
	return data;
}
