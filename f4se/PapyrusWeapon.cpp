#include "f4se/PapyrusWeapon.h"

#include "f4se/GameObjects.h"
#include "f4se/GameData.h"

#include "f4se/PapyrusStruct.h"

namespace papyrusWeapon
{
	BGSMod::Attachment::Mod * GetEmbeddedMod(TESObjectWEAP* thisWeapon)
	{
		return thisWeapon ? thisWeapon->weapData.embeddedMod : nullptr;
	}

	void SetEmbeddedMod(TESObjectWEAP* thisWeapon, BGSMod::Attachment::Mod * mod)
	{
		if(thisWeapon) {
			thisWeapon->weapData.embeddedMod = mod;
		}
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusWeapon::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <TESObjectWEAP, BGSMod::Attachment::Mod*>("GetEmbeddedMod", "Weapon", papyrusWeapon::GetEmbeddedMod, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESObjectWEAP, void, BGSMod::Attachment::Mod*>("SetEmbeddedMod", "Weapon", papyrusWeapon::SetEmbeddedMod, vm));
}
