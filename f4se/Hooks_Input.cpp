#include "f4se_common/Utilities.h"
#include "f4se_common/SafeWrite.h"

#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"

#include "f4se/InputMap.h"
#include "f4se/GameInput.h"
#include "f4se/GameMenus.h"

#include "f4se/PapyrusEvents.h"
#include "f4se/PapyrusUtilities.h"

#define HOOK_RAW_INPUT 0
#define LOG_INPUT_HOOK 0

typedef void (* _CreateMenuControlHandlers)(MenuControls * mem);
RelocAddr <_CreateMenuControlHandlers> CreateMenuControlHandlers(0x012A80F0);
_CreateMenuControlHandlers CreateMenuControlHandlers_Original = nullptr;

typedef void (* _CreatePlayerControlHandlers)(PlayerControls * mem);
RelocAddr <_CreatePlayerControlHandlers> CreatePlayerControlHandlers(0x00F44CC0);
_CreatePlayerControlHandlers CreatePlayerControlHandlers_Original = nullptr;

#if HOOK_RAW_INPUT

typedef BOOL (WINAPI * _RegisterRawInputDevices)(RAWINPUTDEVICE * devices, UINT numDevices, UINT structSize);
_RegisterRawInputDevices Original_RegisterRawInputDevices = nullptr;

typedef UINT (WINAPI * _GetRawInputData)(HRAWINPUT rawinput, UINT cmd, void * data, UINT * dataSize, UINT headerSize);
_GetRawInputData Original_GetRawInputData = nullptr;

BOOL WINAPI Hook_RegisterRawInputDevices(RAWINPUTDEVICE * devices, UINT numDevices, UINT structSize)
{
#if LOG_INPUT_HOOK
	_MESSAGE("RegisterRawInputDevices %08X %08X", numDevices, structSize);
	for(UINT i = 0; i < numDevices; i++)
	{
		RAWINPUTDEVICE * dev = &devices[i];

		_MESSAGE("%04X %04X %08X %08X", dev->usUsagePage, dev->usUsage, dev->dwFlags, dev->hwndTarget);
	}
#endif

	BOOL result = Original_RegisterRawInputDevices(devices, numDevices, structSize);

	return result;
}

UINT WINAPI Hook_GetRawInputData(HRAWINPUT rawinput, UINT cmd, void * data, UINT * dataSize, UINT headerSize)
{
	UINT result = Original_GetRawInputData(rawinput, cmd, data, dataSize, headerSize);

#if LOG_INPUT_HOOK

	_MESSAGE("GetRawInputData %08X %08X %08X", cmd, *dataSize, headerSize);

	if(data)
	{
		RAWINPUT * input = (RAWINPUT *)data;

		_MESSAGE("hdr: %08X %08X %08X %08X", input->header.dwType, input->header.dwSize, input->header.hDevice, input->header.wParam);

		if(cmd == RID_INPUT)
		{
			switch(input->header.dwType)
			{
				case RIM_TYPEHID:
				{
					_MESSAGE("hid: %08X %08X", input->data.hid.dwSizeHid, input->data.hid.dwCount);
				}
				break;

				case RIM_TYPEKEYBOARD:
				{
					RAWKEYBOARD * kbd = &input->data.keyboard;

					_MESSAGE("kbd: %04X %04X %04X %04X %08X %08X",
						kbd->MakeCode, kbd->Flags, kbd->Reserved, kbd->VKey, kbd->Message, kbd->ExtraInformation);
				}
				break;

				case RIM_TYPEMOUSE:
				{
					RAWMOUSE * mse = &input->data.mouse;

					_MESSAGE("mse: %04X %08X %08X %d %d %08X",
						mse->usFlags, mse->ulButtons, mse->ulRawButtons, mse->lLastX, mse->lLastY, mse->ulExtraInformation);
				}
				break;
			}
		}
	}

#endif

	return result;
}

#endif

void HandleButtonEvent(ButtonEvent * inputEvent)
{
	UInt32	keyCode;
	UInt32	deviceType = inputEvent->deviceType;
	UInt32	keyMask = inputEvent->keyMask;

	// Mouse
	if (deviceType == InputEvent::kDeviceType_Mouse)
		keyCode = InputMap::kMacro_MouseButtonOffset + keyMask; 
	// Gamepad
	else if (deviceType == InputEvent::kDeviceType_Gamepad)
		keyCode = InputMap::GamepadMaskToKeycode(keyMask);
	// Keyboard
	else
		keyCode = keyMask;

	// Valid scancode?
	if (keyCode >= InputMap::kMaxMacros)
		return;

	BSFixedString	control	= *inputEvent->GetControlID();
	float			timer	= inputEvent->timer;

	bool isDown	= inputEvent->isDown == 1.0f && timer == 0.0f;
	bool isUp	= inputEvent->isDown == 0.0f && timer != 0.0f;

	if (isDown)
	{
		g_inputKeyEventRegs.ForEach(
			keyCode,
			[&keyCode](const EventRegistration<NullParameters> & reg)
		{
			SendPapyrusEvent1<UInt32>(reg.handle, reg.scriptName, "OnKeyDown", keyCode);
		}
		);
		g_inputControlEventRegs.ForEach(
			control,
			[&control](const EventRegistration<NullParameters> & reg)
		{
			SendPapyrusEvent1<BSFixedString>(reg.handle, reg.scriptName, "OnControlDown", control);
		}
		);
	}
	else if (isUp)
	{
		g_inputKeyEventRegs.ForEach(
			keyCode,
			[&keyCode, &timer](const EventRegistration<NullParameters> & reg)
		{
			SendPapyrusEvent2<UInt32, float>(reg.handle, reg.scriptName, "OnKeyUp", keyCode, timer);
		}
		);
		g_inputControlEventRegs.ForEach(
			control,
			[&control, &timer](const EventRegistration<NullParameters> & reg)
		{
			SendPapyrusEvent2<BSFixedString, float>(reg.handle, reg.scriptName, "OnControlUp", control, timer);
		}
		);
	}
}

class F4SEInputHandler : public PlayerInputHandler
{
public:
	F4SEInputHandler() : PlayerInputHandler() { }

	virtual void OnButtonEvent(ButtonEvent * inputEvent)
	{
		if((*g_ui)->numPauseGame)
		{
			return;
		}
		
		HandleButtonEvent(inputEvent);
	}
};

class F4SEInputMenuHandler : public BSInputEventUser
{
public:
	F4SEInputMenuHandler() : BSInputEventUser(true) { }

	virtual void OnButtonEvent(ButtonEvent * inputEvent)
	{
		if(!(*g_ui)->numPauseGame)
		{
			return;
		}

		HandleButtonEvent(inputEvent);
	}
};


F4SEInputHandler g_inputHandler;
F4SEInputMenuHandler g_inputMenuHandler;

void CreatePlayerControlHandlers_Hook(PlayerControls * playerControls)
{
	// Process F4SE handlers first so no events will be blocked
	playerControls->inputEvents1.Push(&g_inputHandler);

	CreatePlayerControlHandlers_Original(playerControls);
}

void CreateMenuControlHandlers_Hook(MenuControls * menuControls)
{
	CreateMenuControlHandlers_Original(menuControls);
	menuControls->inputEvents.Push(&g_inputMenuHandler);
}

void Hooks_Input_Init()
{
	//
}

void Hooks_Input_Commit()
{
#if HOOK_RAW_INPUT
	void ** iat = (void **)GetIATAddr(GetModuleHandle(NULL), "user32.dll", "RegisterRawInputDevices");
	Original_RegisterRawInputDevices = (_RegisterRawInputDevices)*iat;
	SafeWrite64((uintptr_t)iat, (UInt64)Hook_RegisterRawInputDevices);

	iat = (void **)GetIATAddr(GetModuleHandle(NULL), "user32.dll", "GetRawInputData");
	Original_GetRawInputData = (_GetRawInputData)*iat;
	SafeWrite64((uintptr_t)iat, (UInt64)Hook_GetRawInputData);
#endif

	// hook adding control handlers to PlayerControls
	{
		struct CreatePlayerControlHandlers_Code : Xbyak::CodeGenerator {
			CreatePlayerControlHandlers_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp+0x08], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(CreatePlayerControlHandlers.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		CreatePlayerControlHandlers_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		CreatePlayerControlHandlers_Original = (_CreatePlayerControlHandlers)codeBuf;

		g_branchTrampoline.Write5Branch(CreatePlayerControlHandlers.GetUIntPtr(), (uintptr_t)CreatePlayerControlHandlers_Hook);
	}

	// hook adding control handlers to MenuControls
	{
		struct CreateMenuControlHandlers_Code : Xbyak::CodeGenerator {
			CreateMenuControlHandlers_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp+0x08], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(CreateMenuControlHandlers.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		CreateMenuControlHandlers_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		CreateMenuControlHandlers_Original = (_CreateMenuControlHandlers)codeBuf;

		g_branchTrampoline.Write5Branch(CreateMenuControlHandlers.GetUIntPtr(), (uintptr_t)CreateMenuControlHandlers_Hook);
	}
}
