#pragma once

#include "f4se/NiObjects.h"
#include "f4se/bhkWorld.h"

class bhkPhysicsSystem;
class bhkMigrationWorld;

// 18
class NiCollisionObject : public NiObject
{
public:
	NiAVObject *m_pkSceneObject;	// 10
};

// 20
class bhkNPCollisionObjectBase : public NiCollisionObject
{
public:
	UInt16 m_uFlags;	// 18
};

// 28
class bhkNiCollisionObject : public NiCollisionObject
{
public:
	enum Flags
	{
		kFlags_Active_Pos			= (1 << 0),
		kFlags_ResetTrans_Pos		= (1 << 1),
		kFlags_Notify_Pos			= (1 << 2),
		kFlags_SetLocal_Pos			= (1 << 3),
		kFlags_DebugDisplay_Pos		= (1 << 4),
		kFlags_Usevel_Pos			= (1 << 5),
		kFlags_Reset_Pos			= (1 << 6),
		kFlags_Sync_On_Update		= (1 << 7),
		kFlags_Unused1				= (1 << 8),
		kFlags_Unused2				= (1 << 9),
		kFlags_Anim_Targeted		= (1 << 10),
		kFlags_Dismbebered_Limb		= (1 << 11),
	};

	UInt16 m_uFlags;							// 18
	NiPointer<bhkWorldObject> spWorldObject;	// 20
};

// 28
class bhkCollisionObject : public bhkNiCollisionObject
{
public:

};

// 48
class bhkBlendCollisionObject : public bhkCollisionObject
{
public:
	enum TransType
	{
		kTransType_Blend = 0,
		kTransType_Havok,
		kTransType_Keyframe,
		kTransType_Undefined,
	};

	float fHeirGain;					// 28
	float fVelGain;						// 2C
	hkpMotion::MotionType ePrevType;	// 30
	bhkMigrationWorld *pStoredWorld;	// 38
	int iForceAddCount;					// 40
};

// 28
class bhkPCollisionObject : public bhkNiCollisionObject
{
public:
};

// 30
class bhkNPCollisionObject : public bhkNPCollisionObjectBase
{
public:
	enum UserBodyFlags
	{
		kFlag_StartAsKeyframed						 = 0x10000,
		kFlag_KeyframedCollideWithStaticAndKeyframed = 0x4000000,
	};

	NiPointer<bhkPhysicsSystem> spSystem;	// 20
	UInt32 uiSystemBodyIdx;					// 28
};

// 30
class bhkNPCollisionObjectUnlinked : public bhkNPCollisionObject
{
public:
};

// 70
class bhkNPCollisionProxyObject : public bhkNPCollisionObjectBase
{
	NiPointer<bhkNPCollisionObject> spCollisionObject;	// 20
	UInt64							unk28;				// 28
	NiTransform						OffsetTransf;		// 30
};
