#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusArmorAddon.h"

#include "f4se/GameObjects.h"

namespace papyrusArmorAddon
{
	VMArray<TESRace*> GetAdditionalRaces(TESObjectARMA* thisArmorAddon)
	{
		VMArray<TESRace*> results;
		if(!thisArmorAddon)
			return results;

		for(UInt32 i = 0; i < thisArmorAddon->additionalRaces.count; ++i)
		{
			TESRace * race = nullptr;
			if(thisArmorAddon->additionalRaces.GetNthItem(i, race))
				results.Push(&race);
		}
		
		return results;
	}
}

void papyrusArmorAddon::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterForm(TESObjectARMA::kTypeID, "ArmorAddon");

	// Races
	vm->RegisterFunction(
		new NativeFunction0 <TESObjectARMA, VMArray<TESRace*>>("GetAdditionalRaces", "ArmorAddon", papyrusArmorAddon::GetAdditionalRaces, vm));
}
