#include "f4se/PapyrusDefaultObject.h"

#include "f4se/GameData.h"
#include "f4se/GameForms.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

namespace papyrusDefaultObject
{
	TESForm * GetDefaultObject(StaticFunctionTag*, BSFixedString name)
	{
		return (*g_defaultObjectMap)->GetDefaultObject(name);
	}

	TESForm * Get(BGSDefaultObject * defaultObject)
	{
		return defaultObject ? defaultObject->form : nullptr;
	}

	void Set(BGSDefaultObject * defaultObject, TESForm * form)
	{
		if(defaultObject)
			defaultObject->form = form;
	}
}

void papyrusDefaultObject::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterForm(BGSDefaultObject::kTypeID, "DefaultObject");

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, TESForm*, BSFixedString>("GetDefaultObject", "DefaultObject", papyrusDefaultObject::GetDefaultObject, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSDefaultObject, TESForm*>("Get", "DefaultObject", papyrusDefaultObject::Get, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSDefaultObject, void, TESForm*>("Set", "DefaultObject", papyrusDefaultObject::Set, vm));
}