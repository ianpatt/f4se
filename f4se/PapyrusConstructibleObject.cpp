#include "f4se/PapyrusConstructibleObject.h"

#include "f4se/PapyrusArgs.h"
#include "f4se/PapyrusStruct.h"

#include "f4se/GameExtraData.h"
#include "f4se/GameForms.h"
#include "f4se/GameObjects.h"
#include "f4se/GameRTTI.h"

namespace papyrusConstructibleObject
{
	DECLARE_STRUCT(ConstructibleComponent, "ConstructibleObject")

	VMArray<ConstructibleComponent> GetConstructibleComponents(BGSConstructibleObject * thisObject)
	{
		VMArray<ConstructibleComponent> result;
		if(!thisObject)
			return result;

		if(!thisObject->components)
			return result;

		for(UInt32 i = 0; i < thisObject->components->count; i++)
		{
			BGSConstructibleObject::Component cp;
			thisObject->components->GetNthItem(i, cp);

			ConstructibleComponent comp;
			comp.Set("object", cp.component);
			comp.Set("count", cp.count);
			result.Push(&comp);
		}

		return result;
	}

	void SetConstructibleComponents(BGSConstructibleObject * thisObject, VMArray<ConstructibleComponent> components)
	{
		if(thisObject) {
			if(!thisObject->components)
				thisObject->components = new tArray<BGSConstructibleObject::Component>();

			thisObject->components->Clear();

			for(UInt32 i = 0; i < components.Length(); i++)
			{
				ConstructibleComponent comp;
				components.Get(&comp, i);

				BGSConstructibleObject::Component cp;
				comp.Get("object", &cp.component);
				comp.Get("count", &cp.count);
				thisObject->components->Push(cp);
			}
		}
	}

	TESForm * GetCreatedObject(BGSConstructibleObject * thisObject)
	{
		return thisObject ? thisObject->createdObject : nullptr;
	}

	void SetCreatedObject(BGSConstructibleObject * thisObject, TESForm * form)
	{
		if(thisObject && form) {
			thisObject->createdObject = form;
		}
	}

	UInt32 GetCreatedCount(BGSConstructibleObject * thisObject)
	{
		return thisObject ? thisObject->createdCount : 0;
	}

	void SetCreatedCount(BGSConstructibleObject * thisObject, UInt32 count)
	{
		if(thisObject && count >= 0) {
			thisObject->createdCount = count;
		}
	}

	UInt32 GetPriority(BGSConstructibleObject * thisObject)
	{
		return thisObject ? thisObject->priority : 0;
	}

	void SetPriority(BGSConstructibleObject * thisObject, UInt32 priority)
	{
		if(thisObject) {
			thisObject->priority = priority;
		}
	}

	BGSKeyword * GetWorkbenchKeyword(BGSConstructibleObject * thisObject)
	{
		return thisObject ? thisObject->workbenchKeyword : nullptr;
	}

	void SetWorkbenchKeyword(BGSConstructibleObject * thisObject, BGSKeyword * keyword)
	{
		if(thisObject && keyword) {
			thisObject->workbenchKeyword = keyword;
		}
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusConstructibleObject::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <BGSConstructibleObject, VMArray<ConstructibleComponent>>("GetConstructibleComponents", "ConstructibleObject", papyrusConstructibleObject::GetConstructibleComponents, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSConstructibleObject, void, VMArray<ConstructibleComponent>>("SetConstructibleComponents", "ConstructibleObject", papyrusConstructibleObject::SetConstructibleComponents, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSConstructibleObject, TESForm*>("GetCreatedObject", "ConstructibleObject", papyrusConstructibleObject::GetCreatedObject, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSConstructibleObject, void, TESForm*>("SetCreatedObject", "ConstructibleObject", papyrusConstructibleObject::SetCreatedObject, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSConstructibleObject, UInt32>("GetCreatedCount", "ConstructibleObject", papyrusConstructibleObject::GetCreatedCount, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSConstructibleObject, void, UInt32>("SetCreatedCount", "ConstructibleObject", papyrusConstructibleObject::SetCreatedCount, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSConstructibleObject, UInt32>("GetPriority", "ConstructibleObject", papyrusConstructibleObject::GetPriority, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSConstructibleObject, void, UInt32>("SetPriority", "ConstructibleObject", papyrusConstructibleObject::SetPriority, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSConstructibleObject, BGSKeyword*>("GetWorkbenchKeyword", "ConstructibleObject", papyrusConstructibleObject::GetWorkbenchKeyword, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSConstructibleObject, void, BGSKeyword*>("SetWorkbenchKeyword", "ConstructibleObject", papyrusConstructibleObject::SetWorkbenchKeyword, vm));
}
