#pragma once

#include "f4se/NiRTTI.h"
#include "f4se/NiObjects.h"

class BSGeometrySegmentData;

// 18
class NiExtraData : public NiObject
{
public:
	virtual ~NiExtraData() { };

	virtual NiRTTI				* GetRTTI(void) override { return NIRTTI_NiExtraData; };

	virtual void				LoadBinary(void * stream) override { CALL_MEMBER_FN(this, Internal_LoadBinary)(stream); };
	virtual void				SaveBinary(void * stream) override { CALL_MEMBER_FN(this, Internal_SaveBinary)(stream); };

	virtual bool				IsEqual(NiObject * object) override
	{
		bool equal = __super::IsEqual(object);
		if(equal)
			return m_name == static_cast<NiExtraData*>(object)->m_name;
	}

	virtual bool				Unk_28() { return false; };
	virtual bool				Unk_29() { return false; };
	virtual bool				Unk_2A() { return true; };
	virtual bool				Unk_2B() { return true; };

	BSFixedString	m_name;	// 10

	MEMBER_FN_PREFIX(NiObject);
	// 834C1A014705796BD4E62721AE5EFA5F72AE14DB+15
	DEFINE_MEMBER_FN(Internal_LoadBinary, void, 0x01B96400, void * stream);
	// 99E296799A58456B4597D53A5954CD9AFFA441FB+1A
	DEFINE_MEMBER_FN(Internal_SaveBinary, void, 0x01B964A0, void * stream);
};

// 20
class NiStringExtraData : public NiExtraData
{
public:
	virtual ~NiStringExtraData();

	BSFixedString	m_string;	// 18

	static NiStringExtraData * Create(const BSFixedString & name, const BSFixedString & string);
};
extern RelocAddr<uintptr_t> s_NiStringExtraDataVtbl;

// 28
class NiBinaryExtraData : public NiExtraData
{
public:
	UInt8 * binaryData;	// 18
	UInt32	size;		// 20

	static NiBinaryExtraData* Create(const BSFixedString & name, UInt32 size);
};

// 20 (DynPosData)
class BSDynPosData : public NiExtraData
{
public:
	UInt8 * vertexData;	// 18

	static BSDynPosData* Create(const BSFixedString & name, BSTriShape * shape);
};

// 28 (FOD)
class BSFaceGenBaseMorphExtraData : public NiExtraData
{
public:
	NiPoint3	* vertexData;		// 18
	UInt32		modelVertexCount;	// 20
	UInt32		vertexCount;		// 24 - 0 if owning type isnt BSDynamicTriShape

	static BSFaceGenBaseMorphExtraData* Create(BSTriShape * shape);
	static BSFaceGenBaseMorphExtraData* Create(const BSFixedString & name, UInt32 vertexCount);

protected:
	// Fills the vertex data from the original object's vertex data in NiPoint3 layout
	MEMBER_FN_PREFIX(BSFaceGenBaseMorphExtraData);
	DEFINE_MEMBER_FN(ctor, BSFaceGenBaseMorphExtraData*, 0x00684440, BSTriShape * shape);
};

extern RelocAddr<uintptr_t> s_BSFaceGenBaseMorphExtraDataVtbl;


class BSConnectPoint
{
public:
	// 30
	class Parents : public NiExtraData
	{
	public:
		struct ConnectPoint
		{
			UInt64			unk00;	// 00 - refcount?
			BSFixedString	parent;	// 08
			BSFixedString	name;	// 10
			NiQuaternion	rot;	// 18
			NiPoint3		pos;	// 28
			float			scale;	// 34
		};
		tArray<ConnectPoint*>	points;	// 18
	};
};

// 198
class BSDismembermentExtraData : public NiExtraData
{
public:
	tArray<BSTriShape*> segments[16];	// 18
};

STATIC_ASSERT(sizeof(BSDismembermentExtraData) == 0x198);

// 20
class NiIntegerExtraData : public NiExtraData
{
public:
	UInt32	value;	// 18
	UInt32	pad1C;	// 1C
};

// 20
class BSDismembermentLimbExtraData : public NiExtraData
{
public:
	UInt32	dismemberIndex;	// 18 - Links to which index of the 0-15 array in BSDismemberExtraData this shape is on
	UInt32	unk1C;	// 1C
};

// 20
class BSSegmentDataStorage : public NiExtraData
{
public:
	BSGeometrySegmentData * segmentData;	// 18
};
