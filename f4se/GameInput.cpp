#include "f4se/GameInput.h"

// 143DFBBBC9422B50436ED42E5F2E83564DD49923+280
RelocPtr <InputEventTable*> g_inputEventTable(0x05A5FA60);
// 78EBF165B2B1065FD2448FCFC00FEA5B8AA7B28F+9C
RelocPtr <InputDeviceManager*> g_inputDeviceMgr(0x05A5F668);
// C34730A09C170DD83B6E000FC6B0D2E1EAEC63A4+121
RelocPtr <InputManager*> g_inputMgr(0x059DA210);

// 596277207F98DF39B4A6684C55367AE307B03594+17
RelocPtr<MenuControls*> g_menuControls(0x058D0AD8); // BSInputEventReceiver

// F719767F2CF934BC5BEE3EF3CBDFC3AA7BF47C13+2A8
RelocPtr<PlayerControls*> g_playerControls(0x059DA218);

UInt8 InputManager::AllowTextInput(bool allow)
{
	if(allow)
	{
		if(allowTextInput == 0xFF)
			_WARNING("InputManager::AllowTextInput: counter overflow");
		else
			allowTextInput++;
	}
	else
	{
		if(allowTextInput == 0)
			_WARNING("InputManager::AllowTextInput: counter underflow");
		else
			allowTextInput--;
	}

	return allowTextInput;
}

UInt32 InputManager::GetMappedKey(BSFixedString name, UInt32 deviceType, UInt32 contextIdx)
{
	ASSERT(contextIdx < kContextCount);

	tArray<InputContext::Mapping> * mappings;
	if (deviceType == InputEvent::kDeviceType_Mouse)
		mappings = &context[contextIdx]->mouseMap;
	else if (deviceType == InputEvent::kDeviceType_Gamepad)
		mappings = &context[contextIdx]->gamepadMap;
	else
		mappings = &context[contextIdx]->keyboardMap;

	for (UInt32 i=0; i < mappings->count; i++)
	{
		InputContext::Mapping m;
		if (!mappings->GetNthItem(i, m))
			break;
		if (m.name == name)
			return m.buttonID;
	}

	// Unbound
	return 0xFF;
}

BSFixedString InputManager::GetMappedControl(UInt32 buttonID, UInt32 deviceType, UInt32 contextIdx)
{
	ASSERT(contextIdx < kContextCount);

	// 0xFF == unbound
	if (buttonID == 0xFF)
		return BSFixedString();

	tArray<InputContext::Mapping> * mappings;
	if (deviceType == InputEvent::kDeviceType_Mouse)
		mappings = &context[contextIdx]->mouseMap;
	else if (deviceType == InputEvent::kDeviceType_Gamepad)
		mappings = &context[contextIdx]->gamepadMap;
	else
		mappings = &context[contextIdx]->keyboardMap;

	for (UInt32 i=0; i < mappings->count; i++)
	{
		InputContext::Mapping m;
		if (!mappings->GetNthItem(i, m))
			break;
		if (m.buttonID == buttonID)
			return m.name;
	}

	return BSFixedString();
}
