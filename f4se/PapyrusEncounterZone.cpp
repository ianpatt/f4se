#include "f4se/PapyrusEncounterZone.h"
 
#include "f4se/GameForms.h"
#include "f4se/PapyrusArgs.h"

namespace papyrusEncounterZone
{
	BGSLocation * GetLocation(BGSEncounterZone * thisEZ)
	{
		return thisEZ ? thisEZ->location : nullptr;
	}

	void SetLocation(BGSEncounterZone * thisEZ, BGSLocation * newLoc)
	{
		if(thisEZ) {
			thisEZ->location = newLoc;
		}
	}

	SInt32 GetRank(BGSEncounterZone * thisEZ)
	{
		return thisEZ ? thisEZ->rank : 0;
	}

	void SetRank(BGSEncounterZone * thisEZ, SInt32 newRank)
	{
		if(thisEZ) {
			thisEZ->rank = newRank;
		}
	}

	SInt32 GetMinLevel(BGSEncounterZone * thisEZ)
	{
		return thisEZ ? thisEZ->minLevel : 0;
	}

	void SetMinLevel(BGSEncounterZone * thisEZ, SInt32 newLevel)
	{
		if(thisEZ) {
			thisEZ->minLevel = newLevel;
		}
	}

	SInt32 GetMaxLevel(BGSEncounterZone * thisEZ)
	{
		return thisEZ ? thisEZ->maxLevel : 0;
	}

	void SetMaxLevel(BGSEncounterZone * thisEZ, SInt32 newLevel)
	{
		if(thisEZ) {
			thisEZ->maxLevel = newLevel;
		}
	}

	bool IsNeverResetable(BGSEncounterZone * thisEZ)
	{
		return thisEZ ? ((thisEZ->flags & BGSEncounterZone::kFlag_NeverResets) == BGSEncounterZone::kFlag_NeverResets) : false;
	}

	void SetNeverResetable(BGSEncounterZone * thisEZ, bool neverResets)
	{
		if(thisEZ) {
			if(neverResets)
				thisEZ->flags |= BGSEncounterZone::kFlag_NeverResets;
			else
				thisEZ->flags &= ~BGSEncounterZone::kFlag_NeverResets;
		}
	}

	bool IsWorkshop(BGSEncounterZone * thisEZ)
	{
		return thisEZ ? ((thisEZ->flags & BGSEncounterZone::kFlag_Workshop) == BGSEncounterZone::kFlag_Workshop) : false;
	}

	void SetWorkshop(BGSEncounterZone * thisEZ, bool workshop)
	{
		if(thisEZ) {
			if(workshop)
				thisEZ->flags |= BGSEncounterZone::kFlag_Workshop;
			else
				thisEZ->flags &= ~BGSEncounterZone::kFlag_Workshop;
		}
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusEncounterZone::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <BGSEncounterZone, BGSLocation*>("GetLocation", "EncounterZone", papyrusEncounterZone::GetLocation, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSEncounterZone, void, BGSLocation*>("SetLocation", "EncounterZone", papyrusEncounterZone::SetLocation, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSEncounterZone, SInt32>("GetRank", "EncounterZone", papyrusEncounterZone::GetRank, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSEncounterZone, void, SInt32>("SetRank", "EncounterZone", papyrusEncounterZone::SetRank, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSEncounterZone, SInt32>("GetMinLevel", "EncounterZone", papyrusEncounterZone::GetMinLevel, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSEncounterZone, void, SInt32>("SetMinLevel", "EncounterZone", papyrusEncounterZone::SetMinLevel, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSEncounterZone, SInt32>("GetMaxLevel", "EncounterZone", papyrusEncounterZone::GetMaxLevel, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSEncounterZone, void, SInt32>("SetMaxLevel", "EncounterZone", papyrusEncounterZone::SetMaxLevel, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSEncounterZone, bool>("IsNeverResetable", "EncounterZone", papyrusEncounterZone::IsNeverResetable, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSEncounterZone, void, bool>("SetNeverResetable", "EncounterZone", papyrusEncounterZone::SetNeverResetable, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSEncounterZone, bool>("IsWorkshop", "EncounterZone", papyrusEncounterZone::IsWorkshop, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSEncounterZone, void, bool>("SetWorkshop", "EncounterZone", papyrusEncounterZone::SetWorkshop, vm));
}
