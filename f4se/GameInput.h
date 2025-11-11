#pragma once

#include "f4se_common/Utilities.h"
#include "f4se_common/Relocation.h"
#include "f4se/GameTypes.h"
#include "f4se/GameEvents.h"

// 28
class InputEvent
{
public:
	virtual ~InputEvent();

	virtual bool			IsIDEvent();
	virtual BSFixedString	*	GetControlID();

	enum
	{
		kDeviceType_Keyboard = 0,
		kDeviceType_Mouse,
		kDeviceType_Gamepad,
		kDeviceType_Kinect
	};

	enum
	{
		kEventType_Button = 0,
		kEventType_MouseMove,
		kEventType_Char,
		kEventType_Thumbstick,
		kEventType_DeviceConnect,
		kEventType_Kinect,
		kEventType_Unk6
	};

	UInt64		deviceType;		// 08
	UInt64		eventType;		// 10
	InputEvent	* next;			// 18
	UInt32		unk20;			// 20
	UInt32		handled;		// 24 - When this != 2 it means stop processing
};
STATIC_ASSERT(sizeof(InputEvent) == 0x28);

// 08
class IDEvent
{
public:
	BSFixedString	controlID;	// 00
};

// 40
class ButtonEvent : public IDEvent, public InputEvent
{
public:
	UInt32			keyMask;	// 30
	UInt32			flags;		// 34 (00000038 when ALT is pressed, 0000001D when STRG is pressed)
	float			isDown;		// 38
	float			timer;		// 3C (hold duration)
};
STATIC_ASSERT(sizeof(ButtonEvent) == 0x040);

// 30
class CharacterEvent : public InputEvent
{
public:
	UInt32 keyCode;
};
STATIC_ASSERT(sizeof(CharacterEvent) == 0x030);

// 40
class MouseMoveEvent : public IDEvent
{
public:
	UInt32 unk28[4];
};

// 40
class CursorMoveEvent : public IDEvent, public InputEvent
{
public:
	UInt32 unk28[4];
};

// 48
class ThumbstickEvent : public IDEvent, public InputEvent
{
public:
	UInt32 unk20[6];
};
STATIC_ASSERT(sizeof(ThumbstickEvent) == 0x048);

// 30
class DeviceConnectEvent : public InputEvent
{
public:
	UInt32 unk20[2];
};
STATIC_ASSERT(sizeof(DeviceConnectEvent) == 0x030);

// 40
class KinectEvent : public IDEvent, public InputEvent
{
public:
	UInt32 unk28[4];
};
STATIC_ASSERT(sizeof(KinectEvent) == 0x040);

// EF0
class InputEventTable
{
public:
	UInt64				unk00;
	ButtonEvent			buttonEvents[30];
	CharacterEvent		characterEvents[15];
	MouseMoveEvent		mouseMoveEvents[3];
	CursorMoveEvent		cursorMoveEvents[3];
	ThumbstickEvent		thumbstickEvents[6];
	DeviceConnectEvent	deviceConnectEvents[3];
	KinectEvent			kinectEvents[3];
};
//STATIC_ASSERT(offsetof(InputEventTable, mouseMoveEvents) == 0xA6C);
//STATIC_ASSERT(sizeof(InputEventTable) == 0xEF0);
extern RelocPtr <InputEventTable*> g_inputEventTable;

class BSInputDevice
{
public:
	virtual ~BSInputDevice();

	virtual void Unk_01();
	virtual void Unk_02();
	virtual bool IsEnabled();
	virtual void Unk_04();
	virtual void Unk_05();

	UInt64		unk008; // init to 0
	UInt64		unk010;
	UInt32		unk018; // padding?
	UInt32		unk01C; // init to 0
	UInt32		unk020;
	UInt32		unk024;
	void*		unk028;	// init to unk_1438CDC80 in 1.3.47
	UInt64		unk030;
	UInt64		unk038; // init to 0
	void*		unk040;
	UInt32		unk048;
	UInt32		unk04C; // init to 0
	UInt32		unk050;
	UInt32		unk054;
	void*		unk058;	// init to &unk_1438CDC84 in 1.3.47
	UInt64		unk060;
	UInt64		unk068; // init to 0
};
STATIC_ASSERT(offsetof(BSInputDevice, unk058) == 0x058);
STATIC_ASSERT(sizeof(BSInputDevice) == 0x70);

class BSKeyboardDevice : public BSInputDevice
{
public:
};
STATIC_ASSERT(sizeof(BSKeyboardDevice) == 0x70);

// 070
class BSVirtualKeyboardDevice : public BSKeyboardDevice
{

};
STATIC_ASSERT(sizeof(BSVirtualKeyboardDevice) == 0x70);


// 270
class BSPCKeyboardDevice : public BSKeyboardDevice
{
public:
	UInt64	unk070[(0x270-0x70)/sizeof(UInt64)];
};
STATIC_ASSERT(sizeof(BSPCKeyboardDevice) == 0x270);

class BSMouseDevice : public BSInputDevice
{

};
STATIC_ASSERT(sizeof(BSVirtualKeyboardDevice) == 0x70);

// 88
class BSPCMouseDevice : public BSMouseDevice
{
public:
	UInt64	unk070;
	UInt64	unk078;
	UInt64	unk080;
};
STATIC_ASSERT(sizeof(BSPCMouseDevice) == 0x88);

class BSGamepadDevice : public BSInputDevice
{
};


class BSGamepadDeviceDelegate : public BSGamepadDevice
{

};

class BSPCGamepadDevice : public BSGamepadDeviceDelegate
{

};

// B8
class BSPCGamepadDeviceHandler : public BSGamepadDevice
{
public:
	UInt64 unk070[(0xB8-0x70)/sizeof(UInt64)];
};
STATIC_ASSERT(sizeof(BSPCGamepadDeviceHandler) == 0xB8);

// 70
class BSPCVirtualKeyboardDevice : public BSVirtualKeyboardDevice
{


};
STATIC_ASSERT(sizeof(BSPCVirtualKeyboardDevice) == 0x70);

// 10
class BSInputEventUser
{
public:
	BSInputEventUser() : enabled(false) { }
	BSInputEventUser(bool bEnabled) : enabled(bEnabled) { }
	virtual ~BSInputEventUser() { };

	virtual bool ShouldHandleEvent(InputEvent * inputEvent = nullptr) { return enabled; };
	virtual void OnKinectEvent(KinectEvent * inputEvent) { };
	virtual void OnDeviceConnectEvent(DeviceConnectEvent * inputEvent) { };
	virtual void OnThumbstickEvent(ThumbstickEvent * inputEvent) { };
	virtual void OnCursorMoveEvent(CursorMoveEvent * inputEvent) { };
	virtual void OnMouseMoveEvent(MouseMoveEvent * inputEvent) { };
	virtual void OnCharacterEvent(CharacterEvent * inputEvent) { };
	virtual void OnButtonEvent(ButtonEvent * inputEvent) { };

	bool	enabled;

private:
	// GameMenuBase:BSInputEventUser override should be the only one calling this function
	friend class GameMenuBase;
	DEFINE_MEMBER_FN_1(Impl_OnGameMenuBaseButtonEvent, bool, 0x01B13E60, ButtonEvent * button);
};

// 30
class PlayerInputHandler : public BSInputEventUser
{
public:
	PlayerInputHandler() : BSInputEventUser(true), 
		unk10(0), unk18(0), unk19(0), unk1A(0), unk1C(0), unk20(0), unk21(0), unk22(0), unk24(0), unk28(0), unk2C(0) { }

	virtual void Unk_09() { };
	virtual void Unk_0A(void * unk1) { };

	UInt64	unk10;	// 10
	UInt8	unk18;	// 18
	UInt8	unk19;	// 19
	UInt16	unk1A;	// 1A
	UInt32	unk1C;	// 1C
	UInt8	unk20;  // 20 
	UInt8	unk21;	// 21 unk21 == 1 has something to do with an additional vfunc
	UInt16	unk22;	// 22
	UInt32	unk24;	// 24
	UInt8	unk28;	// 28
	UInt32	unk2C;	// 2C
};

// 10
class BSInputEventReceiver
{
public:
	virtual ~BSInputEventReceiver();

	UInt32	unk08;	// 08
	UInt32	unk0C;	// 0C
};

class MenuControls : public BSInputEventReceiver
{
public:
	virtual ~MenuControls(); // Executes receiving function

	UInt64	unk10;	// 10
	tArray<BSInputEventUser*>	inputEvents;	// 18
	BSInputEventUser			* events[8];	// 30
};

extern RelocPtr<MenuControls*> g_menuControls;

class PlayerControls : public BSInputEventReceiver
{
public:
	virtual ~PlayerControls();

	BSTEventSink<MenuOpenCloseEvent>		openCloseEvent;				// 10
	BSTEventSink<MenuModeChangeEvent>		menuModeChangeEvent;		// 18
	BSTEventSink<TESFurnitureEvent>			furnitureEvent;				// 20
	BSTEventSink<UserEventEnabledEvent>		userEventEnabledEvent;		// 28
	void*									movementInterface;			// 30 - IMovementPlayerControls
	BSTEventSink<QuickContainerStateEvent>	quickContainerStateEvent;	// 38

	UInt64	unk40;	// 40
	UInt64	unk48;	// 48
	UInt64	unk50;	// 50
	UInt64	unk58;	// 58
	UInt64	unk60;	// 60
	UInt32	unk68;	// 68
	UInt32	unk6C;	// 6C
	UInt32	unk70;	// 70
	UInt32	unk74;	// 74
	UInt32	unk78;	// 78
	UInt32	unk7C;	// 7C
	UInt32	unk80;	// 80
	float	unk84;	// 84
	UInt32	unk88;	// 88
	UInt32	unk8C;	// 8C
	tArray<PlayerInputHandler*>	inputEvents1;	// 90
	tArray<PlayerInputHandler*>	inputEvents2;	// A8 - This subset has to do with unk20 and unk21 on the handler
	// ...
};
extern RelocPtr<PlayerControls*> g_playerControls;

class InputDeviceManager
{
public:
	UInt64						unk00;					// 000
	BSPCKeyboardDevice*			keyboardDevice;			// 008
	BSPCMouseDevice*			mouseDevice;			// 010
	BSPCGamepadDeviceHandler*	gamepadHandler;			// 018
	BSPCGamepadDeviceHandler*	gamepadHandler020;		// 020
	BSPCVirtualKeyboardDevice*	virtualKeyboardDevice;	// 028

	bool IsGamepadEnabled()
	{
		return gamepadHandler ? gamepadHandler->IsEnabled() : false;
	}
};
extern RelocPtr <InputDeviceManager*> g_inputDeviceMgr;

// 148
class InputManager
{
public:
	enum
	{
		kContext_Gameplay = 0,
		kContextCount = (0x108/8) // 33
	};

	struct InputContext
	{
		// 18
		struct Mapping
		{
			BSFixedString	name;		// 00
			UInt32			buttonID;	// 08
			UInt32			sortIndex;	// 0C
			UInt32			unk10;		// 10
			UInt32			unk14;		// 14
		};

		tArray<Mapping>	keyboardMap;
		tArray<Mapping>	mouseMap;
		tArray<Mapping>	gamepadMap;
	};

	void		* unk00;					// 000
	InputContext * context[kContextCount];	// 008
	tArray<UInt32>	unk110;					// 110
	tArray<InputContext::Mapping>	unk128;	// 128
	UInt8			allowTextInput;			// 140
	UInt8			unk141;					// 141
	UInt8			unk142;					// 142
	UInt8			unk143;					// 143
	UInt32			unk144;					// 144

	UInt8			AllowTextInput(bool allow);
	UInt32			GetMappedKey(BSFixedString name, UInt32 deviceType, UInt32 contextIdx);
	BSFixedString	GetMappedControl(UInt32 buttonID, UInt32 deviceType, UInt32 contextIdx);
};
STATIC_ASSERT(offsetof(InputManager, unk110) == 0x110);
STATIC_ASSERT(offsetof(InputManager, unk128) == 0x128);
STATIC_ASSERT(offsetof(InputManager, allowTextInput) == 0x140);

extern RelocPtr <InputManager*> g_inputMgr;
