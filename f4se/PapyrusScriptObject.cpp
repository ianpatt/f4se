#include "f4se/PapyrusScriptObject.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/PapyrusEvents.h"

#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"

namespace papyrusScriptObject
{
	void RegisterForKey(VMObject * thisObject, UInt32 key)
	{
		if(!thisObject)
			return;

		g_inputKeyEventRegs.Register(key, thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void UnregisterForKey(VMObject * thisObject, UInt32 key)
	{
		if(!thisObject)
			return;
		g_inputKeyEventRegs.Unregister(key, thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void RegisterForControl(VMObject * thisObject, BSFixedString control)
	{
		if(!thisObject)
			return;
		g_inputControlEventRegs.Register(control, thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void UnregisterForControl(VMObject * thisObject, BSFixedString control)
	{
		if(!thisObject)
			return;
		g_inputControlEventRegs.Unregister(control, thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void RegisterForExternalEvent(VMObject * thisObject, BSFixedString eventName, BSFixedString callback)
	{
		if(!thisObject)
			return;

		ExternalEventParameters params;
		params.callbackName = callback;
		g_externalEventRegs.Register(eventName, thisObject->GetHandle(), thisObject->GetObjectType(), &params);
	}

	void UnregisterForExternalEvent(VMObject * thisObject, BSFixedString eventName)
	{
		if(!thisObject)
			return;
		g_externalEventRegs.Unregister(eventName, thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void RegisterForCameraState(VMObject * thisObject)
	{
		if(!thisObject)
			return;

		g_cameraEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void UnregisterForCameraState(VMObject * thisObject)
	{
		if(!thisObject)
			return;

		g_cameraEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());
	}

	void RegisterForFurnitureEvent(VMObject * thisObject, VMVariable var)
	{
		if(!thisObject)
			return;

		EventRegistration<FormParameters> p;
		p.handle = thisObject->GetHandle();
		p.scriptName = thisObject->GetObjectType();
		FormParameters * params = &p.params;

		g_furnitureEventRegs.Lock();

		// Lookup the existing params from the registrations if it exists
		// Set contents are normally const, we are casting because the order of the set is unchanged by this
		auto it = g_furnitureEventRegs.m_data.find(p);
		if(it != g_furnitureEventRegs.m_data.end()) {
			params = const_cast<FormParameters*>(&it->params);
		}
		
		if(var.IsNone())
		{
			// ObjectReference
			TESObjectREFR* refr = nullptr;
			if(var.Get(&refr))
			{
				if(refr)
					params->AddFilter(refr);
			}

			// This one might not be possible with ObjectReference[] as Var
			VMArray<TESObjectREFR*> arr;
			if(var.Get(&arr))
			{
				for(UInt32 i = 0; i < arr.Length(); i++)
				{
					TESObjectREFR * refr = nullptr;
					arr.Get(&refr, i);
					if(refr)
						params->AddFilter(refr);
				}
			}

			// Var[]
			VMArray<VMVariable> varr;
			if(var.Get(&varr))
			{
				for(UInt32 i = 0; i < varr.Length(); i++)
				{
					VMVariable var;
					varr.Get(&var, i);

					TESObjectREFR * refr = nullptr;
					var.Get(&refr);
					if(refr)
						params->AddFilter(refr);
				}
			}

			// FormList
			BGSListForm * formList = nullptr;
			if(var.Get(&formList) && formList)
			{
				for(UInt32 i = 0; i < formList->forms.count; i++)
				{
					TESForm * form = nullptr;
					formList->forms.GetNthItem(i, form);
					TESObjectREFR * refr = DYNAMIC_CAST(form, TESForm, TESObjectREFR);
					if(refr)
						params->AddFilter(refr);
				}
			}
		}
		else // We want to add a new entry
			g_furnitureEventRegs.Register(thisObject->GetHandle(), thisObject->GetObjectType(), params);

		g_furnitureEventRegs.Release();
	}

	void UnregisterForFurnitureEvent(VMObject * thisObject, VMVariable var)
	{
		if(!thisObject)
			return;

		EventRegistration<FormParameters> p;
		p.handle = thisObject->GetHandle();
		p.scriptName = thisObject->GetObjectType();
		FormParameters * params = &p.params;

		g_furnitureEventRegs.Lock();

		// Lookup the existing params from the registrations if it exists
		// Set contents are normally const, we are casting because the order of the set is unchanged by this
		auto it = g_furnitureEventRegs.m_data.find(p);
		if(it != g_furnitureEventRegs.m_data.end()) {
			params = const_cast<FormParameters*>(&it->params);
		}

		if(!var.IsNone())
		{
			// ObjectReference
			TESObjectREFR* refr = nullptr;
			if(var.Get(&refr))
			{
				if(refr)
					params->RemoveFilter(refr);
			}

			// This one might not be possible with ObjectReference[] as Var
			VMArray<TESObjectREFR*> arr;
			if(var.Get(&arr))
			{
				for(UInt32 i = 0; i < arr.Length(); i++)
				{
					TESObjectREFR * refr = nullptr;
					arr.Get(&refr, i);
					if(refr)
						params->RemoveFilter(refr);
				}
			}

			// Var[]
			VMArray<VMVariable> varr;
			if(var.Get(&varr))
			{
				for(UInt32 i = 0; i < varr.Length(); i++)
				{
					VMVariable var;
					varr.Get(&var, i);

					TESObjectREFR * refr = nullptr;
					var.Get(&refr);
					if(refr)
						params->RemoveFilter(refr);
				}
			}

			// FormList
			BGSListForm * formList = nullptr;
			if(var.Get(&formList) && formList)
			{
				for(UInt32 i = 0; i < formList->forms.count; i++)
				{
					TESForm * form = nullptr;
					formList->forms.GetNthItem(i, form);
					TESObjectREFR * refr = DYNAMIC_CAST(form, TESForm, TESObjectREFR);
					if(refr)
						params->RemoveFilter(refr);
				}
			}
		}
		else // No parameter, remove all registrations
			g_furnitureEventRegs.Unregister(thisObject->GetHandle(), thisObject->GetObjectType());

		g_furnitureEventRegs.Release();
	}
}

void papyrusScriptObject::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, UInt32>("RegisterForKey", "ScriptObject", papyrusScriptObject::RegisterForKey, vm));

	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, UInt32>("UnregisterForKey", "ScriptObject", papyrusScriptObject::UnregisterForKey, vm));

	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, BSFixedString>("RegisterForControl", "ScriptObject", papyrusScriptObject::RegisterForControl, vm));

	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, BSFixedString>("UnregisterForControl", "ScriptObject", papyrusScriptObject::UnregisterForControl, vm));

	vm->RegisterFunction(
		new NativeFunction2 <VMObject, void, BSFixedString, BSFixedString>("RegisterForExternalEvent", "ScriptObject", papyrusScriptObject::RegisterForExternalEvent, vm));

	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, BSFixedString>("UnregisterForExternalEvent", "ScriptObject", papyrusScriptObject::UnregisterForExternalEvent, vm));

	vm->RegisterFunction(
		new NativeFunction0 <VMObject, void>("RegisterForCameraState", "ScriptObject", papyrusScriptObject::RegisterForCameraState, vm));

	vm->RegisterFunction(
		new NativeFunction0 <VMObject, void>("UnregisterForCameraState", "ScriptObject", papyrusScriptObject::UnregisterForCameraState, vm));

	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, VMVariable>("RegisterForFurnitureEvent", "ScriptObject", papyrusScriptObject::RegisterForFurnitureEvent, vm));

	vm->RegisterFunction(
		new NativeFunction1 <VMObject, void, VMVariable>("UnregisterForFurnitureEvent", "ScriptObject", papyrusScriptObject::UnregisterForFurnitureEvent, vm));
}
