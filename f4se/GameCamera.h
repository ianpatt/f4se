#pragma once

#include "f4se_common/Relocation.h"
#include "f4se/GameEvents.h"
#include "f4se/GameInput.h"
#include "f4se/NiTypes.h"

class NiNode;
class TESCameraState;

// 24
class TESCameraState : public BSIntrusiveRefCounted, public BSInputEventUser
{
public:
	virtual ~TESCameraState();

	virtual void Unk_09();
	virtual void Unk_0A();
	virtual void Unk_0B(void * arg);
	virtual void GetRotation(NiQuaternion * out);
	virtual void GetPosition(NiPoint3 * out);
	virtual void Unk_0E();
	virtual void Unk_0F();
	virtual void Unk_10();

	void	* unk18;	// 18
	UInt32	stateID;	// 20
	UInt32	pad24;		// 24
};

STATIC_ASSERT(sizeof(TESCameraState) == 0x28);

// 138
class ThirdPersonState : public TESCameraState
{
public:
	virtual ~ThirdPersonState();

	virtual void UpdateMode(bool weaponDrawn);
	virtual bool Unk_12();
	virtual void Unk_13();
	virtual void Unk_14();
	virtual void Unk_15();
};

// 38
class TESCamera
{
public:
	virtual ~TESCamera();

	virtual void SetCameraNode(NiNode * node);
	virtual void Unk_02(UInt8 unk1); // Sets 0x30
	virtual void Unk_03();

	float	unk08;	// 08
	float	unk0C;	// 0C
	float	unk10;	// 10
	float	unk14;	// 14
	float	unk18;	// 18
	UInt32	unk1C;	// 1C
	NiNode			* cameraNode;	// 20
	TESCameraState	* cameraState;	// 28
	UInt8	unk30;		// 30
	UInt8	unk31;		// 31
	UInt16	unk32;		// 32
	UInt32	unk34;		// 34

	MEMBER_FN_PREFIX(TESCamera);
	DEFINE_MEMBER_FN(SetCameraState, void, 0x0082E930, TESCameraState * cameraState);
};

class PlayerCamera : public TESCamera
{
public:
	virtual ~PlayerCamera();

	enum
	{
		kCameraState_FirstPerson = 0,
		kCameraState_AutoVanity,
		kCameraState_VATS,
		kCameraState_Free,
		kCameraState_IronSights,
		kCameraState_Transition,
		kCameraState_TweenMenu,
		kCameraState_ThirdPerson1,
		kCameraState_ThirdPerson2,
		kCameraState_Furniture,
		kCameraState_Horse,
		kCameraState_Bleedout,
		kCameraState_Dialogue,
		kNumCameraStates
	};

	BSInputEventReceiver				inputEventReceiver;				// 38
	BSTEventSink<void>					idleInputSink;					// 48
	BSTEventSink<UserEventEnabledEvent>	userEventEnabledSink;			// 50
	BSTEventSink<void>					otherEventEnabledSink;			// 58

	UInt32	unk60;	// 60
	UInt32	unk64;	// 64 - Handle
	UInt32	unk68;	// 68
	UInt32	unk6C;	// 6C

	UInt64	unk70[(0xE0 - 0x70) >> 3];

	TESCameraState * cameraStates[kNumCameraStates];	// E0
	UInt64			unk148;								// 148
	UInt64			unk150;								// 150 - hknpSphereShape
	UInt64			unk158;								// 158 - hknpBSWorld
	UInt32			unk160;								// 160
	UInt32			unk164;								// 164 - Handle
	float			fDefaultWorldFov;					// 168 - fDefaultWorldFOV:Display
	float			fDefault1stPersonFOV;				// 16C - fDefault1stPersonFOV:Display
	UInt64			unk170;								// 170
	UInt64			unk178;								// 178
	float			unk180;								// 180
	float			unk184;								// 184
	float			unk188;								// 188
	float			unk18C;								// 18C
	float			unk190;								// 190
	float			unk194;								// 194
	float			unk198;								// 198
	float			unk19C;								// 19C
	SInt32			unk1A0;								// 1A0
	UInt8			unk1A4;								// 1A4
	UInt8			unk1A5;								// 1A5
	UInt8			unk1A6;								// 1A6
	UInt8			unk1A7;								// 1A7
	UInt8			unk1A8;								// 1A8
	UInt8			unk1A9;								// 1A9

	SInt32 GetCameraStateId(TESCameraState * state);
};
STATIC_ASSERT(offsetof(PlayerCamera, cameraStates) == 0xE0);
STATIC_ASSERT(offsetof(PlayerCamera, unk148) == 0x148);

extern RelocPtr <PlayerCamera *> g_playerCamera;
