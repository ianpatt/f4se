#include "f4se/PapyrusPerk.h"

#include "f4se/GameForms.h"
#include "f4se/GameFormComponents.h"
#include "f4se/GameReferences.h"

namespace papyrusPerk
{
	bool IsPlayable(BGSPerk* thisPerk)
	{
		return thisPerk ? (thisPerk->playable >= 0) : false;
	}

	bool IsHidden(BGSPerk* thisPerk)
	{
		return thisPerk ? (thisPerk->hidden >= 0) : false;
	}

	UInt32 GetLevel(BGSPerk* thisPerk)
	{
		return thisPerk ? thisPerk->perkLevel : 0;
	}

	UInt32 GetNumRanks(BGSPerk* thisPerk)
	{
		return thisPerk ? thisPerk->numRanks : 0;
	}

	BGSPerk* GetNextPerk(BGSPerk* thisPerk)
	{
		return thisPerk ? thisPerk->nextPerk : nullptr;
	}

	BSFixedString GetSWFPath(BGSPerk* thisPerk)
	{
		return thisPerk ? thisPerk->swfPath : BSFixedString();
	}

	bool IsEligible(BGSPerk* thisPerk, Actor * actor)
	{
		if(!thisPerk || !actor)
			return false;

		Condition ** condition = &thisPerk->condition;
		if(condition)
			return EvaluationConditions(condition, actor, actor);

		// No conditions to fulfill
		return true;
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusPerk::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <BGSPerk, bool>("IsPlayable", "Perk", papyrusPerk::IsPlayable, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSPerk, bool>("IsHidden", "Perk", papyrusPerk::IsHidden, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSPerk, UInt32>("GetLevel", "Perk", papyrusPerk::GetLevel, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSPerk, UInt32>("GetNumRanks", "Perk", papyrusPerk::GetNumRanks, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSPerk, BGSPerk*>("GetNextPerk", "Perk", papyrusPerk::GetNextPerk, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSPerk, BSFixedString>("GetSWFPath", "Perk", papyrusPerk::GetSWFPath, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSPerk, bool, Actor*>("IsEligible", "Perk", papyrusPerk::IsEligible, vm));
}
