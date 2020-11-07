#include "f4se/PapyrusInput.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/GameInput.h"
#include "f4se/InputMap.h"

namespace papyrusInput
{
	SInt32 GetMappedKey(StaticFunctionTag* thisInput, BSFixedString name, UInt32 deviceType)
	{
		InputManager * inputManager = (*g_inputMgr);
		if (!inputManager)
			return -1;

		UInt32 key = 0xFF;

		// Manual device selection
		if (deviceType != 0xFF)
		{
			key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
		}
		// Auto-selected device
		else
		{
			// Gamepad
			if ((*g_inputDeviceMgr)->IsGamepadEnabled())
			{
				deviceType = InputEvent::kDeviceType_Gamepad;
				key = inputManager->GetMappedKey(name, InputEvent::kDeviceType_Gamepad, InputManager::kContext_Gameplay);
			}
			// Mouse + Keyboard
			else
			{
				deviceType = InputEvent::kDeviceType_Keyboard;
				key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
				if (key == 0xFF)
				{
					deviceType = InputEvent::kDeviceType_Mouse;
					key = inputManager->GetMappedKey(name, deviceType, InputManager::kContext_Gameplay);
				}
			}
		}

		if (key == 0xFF)
			return -1;

		// Map to common value space
		if (deviceType == InputEvent::kDeviceType_Mouse)
		{
			return key + InputMap::kMacro_MouseButtonOffset;
		}
		else if (deviceType == InputEvent::kDeviceType_Gamepad)
		{
			UInt32 mapped = InputMap::GamepadMaskToKeycode(key);
			return (mapped != InputMap::kMaxMacros ? mapped : -1);
		}
		else
		{
			return key;
		}
	}

	BSFixedString GetMappedControl(StaticFunctionTag* thisInput, SInt32 keyCode)
	{
		if (keyCode < 0 || keyCode >= InputMap::kMaxMacros)
			return BSFixedString();

		InputManager * inputManager = (*g_inputMgr);
		if (!inputManager)
			return BSFixedString();

		UInt32 buttonID;
		UInt32 deviceType;

		if (keyCode >= InputMap::kMacro_GamepadOffset)
		{
			buttonID = InputMap::GamepadKeycodeToMask(keyCode);
			deviceType = InputEvent::kDeviceType_Gamepad;
		}
		else if (keyCode >= InputMap::kMacro_MouseButtonOffset)
		{
			buttonID = keyCode - InputMap::kMacro_MouseButtonOffset;
			deviceType = InputEvent::kDeviceType_Mouse;
		}
		else
		{
			buttonID = keyCode;
			deviceType = InputEvent::kDeviceType_Keyboard;
		}

		return inputManager->GetMappedControl(buttonID, deviceType, InputManager::kContext_Gameplay);
	}
}

void papyrusInput::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, SInt32, BSFixedString, UInt32>("GetMappedKey", "Input", papyrusInput::GetMappedKey, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, BSFixedString, SInt32>("GetMappedControl", "Input", papyrusInput::GetMappedControl, vm));

	vm->SetFunctionFlags("Input", "GetMappedKey", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("Input", "GetMappedControl", IFunction::kFunctionFlag_NoWait);
}
