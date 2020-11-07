#pragma once

#include "f4se/NiObjects.h"

// 180
class bhkWorld : public NiObject
{
public:
	UInt64	unk10[(0x180 - 0x10) >> 3];
};

// 1B0
class bhkWorldM : public bhkWorld
{
public:
	UInt64	unk180[(0x1B0 - 0x180) >> 3];
};

class hkSerializableCinfo
{

};

// 18
class bhkSerializable : public NiObject
{
	hkSerializableCinfo *pInfo;	// 10
};

// 08
class hkBaseObject
{
public:
	virtual ~hkBaseObject();
};

// 10
class hkReferencedObject : public hkBaseObject
{
public:
	UInt32 m_memSizeAndRefCount;	// 8
};

// 20
class bhkRefObject : public bhkSerializable
{
	hkReferencedObject *phkObject;	// 18
};

// 28
class bhkWorldObject : public bhkRefObject
{
public:
	enum Flags
	{
		kFlags_Wind							= (1 << 0),
		kFlags_Dynamic						= (1 << 1),
		kFlags_FixedConstraints				= (1 << 2),
		kFlags_Unused1						= (1 << 3),
		kFlags_Batch_Add					= (1 << 4),
		kFlags_Batch_Remove					= (1 << 5),
		kFlags_Disabled						= (1 << 6),
		kFlags_Unused2						= (1 << 7),
		kFlags_Sticking_Target				= (1 << 8),
		kFlags_Keyframed_By_LoadedAreaBound = (1 << 9),
	};
	UInt16 iFlags;						// 20
	UInt16 uiUnderwaterCount;			// 22
	UInt32 uiWaterProcessingFrameCount;	// 24
};

// 28
class bhkEntity : public bhkWorldObject
{
public:

};

// 40
class bhkRigidBody : public bhkEntity
{
public:
	tArray<NiPointer<bhkRefObject>> ActionConstraint;	// 28
};

// 20
class bhkConstraint : public bhkRefObject
{
public:

};

// 20
class bhkLimitedHingeConstraint : public bhkConstraint
{
public:
};

class hkpMotion : public hkReferencedObject
{
public:
	enum MotionType : UInt8
	{
		kMotionType_Invalid = 0,
		kMotionType_Dynamic,
		kMotionType_Sphere_Inertia,
		kMotionType_Box_Inertia,
		kMotionType_Keyframed,
		kMotionType_Fixed,
		kMotionType_Thin_Box_Inertia,
		kMotionType_Character,
		kMotionType_Max_ID,
	};

	MotionType m_type;
	// ... more
};
