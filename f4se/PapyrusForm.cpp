#include "f4se/PapyrusForm.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/GameObjects.h"
#include "f4se/GameRTTI.h"

namespace papyrusForm
{
	BSFixedString GetName(TESForm* thisForm)
	{
		if (!thisForm)
			return BSFixedString();

		TESFullName* pFullName = DYNAMIC_CAST(thisForm, TESForm, TESFullName);
		if (pFullName)
			return pFullName->name;

		return BSFixedString();
	}

	void SetName(TESForm* thisForm, BSFixedString nuName)
	{
		TESFullName* pFullName = DYNAMIC_CAST(thisForm, TESForm, TESFullName);
		if (pFullName) {
			pFullName->name = nuName;
		}
	}

	float GetWeight(TESForm* thisForm)
	{
		if (!thisForm)
			return 0.0;

		TESWeightForm* pWeight = DYNAMIC_CAST(thisForm, TESForm, TESWeightForm);
		if (pWeight)
			return pWeight->weight;
		return 0.0;
	}

	void SetWeight(TESForm* thisForm, float nuWeight)
	{
		if (!thisForm)
			return;

		TESWeightForm* pWeight = DYNAMIC_CAST(thisForm, TESForm, TESWeightForm);
		if (pWeight)
			pWeight->weight = nuWeight;
	}

	void SetGoldValue(TESForm* thisForm, UInt32 value)
	{
		if (!thisForm)
			return;
		TESValueForm* pValue = DYNAMIC_CAST(thisForm, TESForm, TESValueForm);
		if (pValue)
			pValue->value = value;
	}

	VMArray<BGSKeyword*> GetKeywords(TESForm* thisForm)
	{
		VMArray<BGSKeyword*> result;
		if (!thisForm)
			return result;

		BGSKeywordForm* pKeywords = DYNAMIC_CAST(thisForm, TESForm, BGSKeywordForm);
		if (pKeywords) {
			for(UInt32 i = 0; i < pKeywords->numKeywords; i++)
			{
				if(pKeywords->keywords[i])
					result.Push(&pKeywords->keywords[i]);
			}
		}
		return result;
	}

	bool HasWorldModel(TESForm * thisForm)
	{
		if(thisForm) {
			TESModel* pWorldModel = DYNAMIC_CAST(thisForm, TESForm, TESModel);
			if (pWorldModel)
				return true;
		}

		return false;
	}

	BSFixedString GetWorldModelPath(TESForm * thisForm)
	{
		if(!thisForm)
			return BSFixedString();

		TESModel* pWorldModel = DYNAMIC_CAST(thisForm, TESForm, TESModel);
		if (!pWorldModel)
			return BSFixedString();

		return pWorldModel->GetModelName();
	}

	void SetWorldModelPath(TESForm * thisForm, BSFixedString nuPath)
	{
		if(thisForm) {
			TESModel* pWorldModel = DYNAMIC_CAST(thisForm, TESForm, TESModel);
			if(pWorldModel)
				pWorldModel->SetModelName(nuPath.c_str());
		}
	}


	BSFixedString GetIconPath(TESForm * thisForm)
	{
		if(!thisForm)
			return BSFixedString();

		TESIcon* pIcon = DYNAMIC_CAST(thisForm, TESForm, TESIcon);
		return (pIcon) ? pIcon->str : BSFixedString();
	}

	void SetIconPath(TESForm * thisForm, BSFixedString nuPath)
	{
		if(thisForm) {
			TESIcon* pIcon = DYNAMIC_CAST(thisForm, TESForm, TESIcon);
			if(pIcon)
				pIcon->str = nuPath;
		}
	}

	BSFixedString GetMessageIconPath(TESForm * thisForm)
	{
		if(!thisForm)
			return BSFixedString();

		BGSMessageIcon* pMessageIcon = DYNAMIC_CAST(thisForm, TESForm, BGSMessageIcon);
		return (pMessageIcon) ? pMessageIcon->unk08.str : BSFixedString();
	}

	void SetMessageIconPath(TESForm * thisForm, BSFixedString nuPath)
	{
		if(thisForm) {
			BGSMessageIcon* pMessageIcon = DYNAMIC_CAST(thisForm, TESForm, BGSMessageIcon);
			if(pMessageIcon)
				pMessageIcon->unk08.str = nuPath;
		}
	}

	EnchantmentItem* GetEnchantment(TESForm * thisForm)
	{
		if(!thisForm)
			return nullptr;

		TESEnchantableForm* pEnchantment = DYNAMIC_CAST(thisForm, TESForm, TESEnchantableForm);
		return (pEnchantment) ? pEnchantment->enchantment : nullptr;
	}

	void SetEnchantment(TESForm * thisForm, EnchantmentItem* enchantment)
	{
		if (thisForm) {
			TESEnchantableForm* pEnchantment = DYNAMIC_CAST(thisForm, TESForm, TESEnchantableForm);
			if(pEnchantment)
				pEnchantment->enchantment = enchantment;
		}
	}

	UInt32 GetEnchantmentValue(TESForm * thisForm)
	{
		if(!thisForm)
			return 0;

		TESEnchantableForm* pEnchantment = DYNAMIC_CAST(thisForm, TESForm, TESEnchantableForm);
		return (pEnchantment && pEnchantment->enchantment) ? pEnchantment->maxCharge : 0;
	}

	void SetEnchantmentValue(TESForm * thisForm, UInt32 value)
	{
		if (thisForm) {
			TESEnchantableForm* pEnchantment = DYNAMIC_CAST(thisForm, TESForm, TESEnchantableForm);
			if(pEnchantment->enchantment)
				pEnchantment->maxCharge = value;
		}
	}

	BGSEquipSlot * GetEquipType(TESForm * thisForm)
	{
		if(!thisForm)
			return nullptr;

		BGSEquipType* pEquipType = DYNAMIC_CAST(thisForm, TESForm, BGSEquipType);
		if (pEquipType) {
			return pEquipType->GetEquipSlot();
		}

		// Invalid EquipSlot
		return nullptr;
	}

	void SetEquipType(TESForm * thisForm, BGSEquipSlot * slot)
	{
		if (thisForm && slot) {
			BGSEquipType* pEquipType = DYNAMIC_CAST(thisForm, TESForm, BGSEquipType);
			if(pEquipType)
				pEquipType->SetEquipSlot(slot);
		}
	}

	BSFixedString GetDescription(TESForm * thisForm)
	{
		if(!thisForm)
			return BSFixedString();

		TESDescription * pDescription = DYNAMIC_CAST(thisForm, TESForm, TESDescription);
		if(pDescription) {
			BSString str;
			CALL_MEMBER_FN(pDescription, Get)(&str, nullptr);
			return str.Get();
		}

		return BSFixedString();
	}

	TESRace * GetRaceForm(TESForm * thisForm)
	{
		if(!thisForm)
			return nullptr;

		TESRaceForm* pRaceForm = DYNAMIC_CAST(thisForm, TESForm, TESRaceForm);
		if (pRaceForm) {
			return pRaceForm->race;
		}

		return nullptr;
	}

	void SetRaceForm(TESForm * thisForm, TESRace * race)
	{
		if (thisForm && race) {
			TESRaceForm* pRaceForm = DYNAMIC_CAST(thisForm, TESForm, TESRaceForm);
			if(pRaceForm)
				pRaceForm->race = race;
		}
	}

	UInt32 GetSlotMask(TESForm* thisForm)
	{
		BGSBipedObjectForm* pBipedObject = DYNAMIC_CAST(thisForm, TESForm, BGSBipedObjectForm);
		return (pBipedObject) ? pBipedObject->GetSlotMask() : 0;
	}

	void SetSlotMask(TESForm* thisForm, UInt32 slotMask)
	{
		if (thisForm) {
			BGSBipedObjectForm* pBipedObject = DYNAMIC_CAST(thisForm, TESForm, BGSBipedObjectForm);
			if(pBipedObject)
				pBipedObject->SetSlotMask(slotMask);
		}
	}

	UInt32 AddSlotToMask(TESForm* thisForm, UInt32 slot)
	{
		BGSBipedObjectForm* pBipedObject = DYNAMIC_CAST(thisForm, TESForm, BGSBipedObjectForm);
		return (pBipedObject) ? pBipedObject->AddSlotToMask(slot) : 0;

	}

	UInt32 RemoveSlotFromMask(TESForm* thisForm, UInt32 slot)
	{
		BGSBipedObjectForm* pBipedObject = DYNAMIC_CAST(thisForm, TESForm, BGSBipedObjectForm);
		return (pBipedObject) ? pBipedObject->RemoveSlotFromMask(slot) : 0;
	}

	UInt32 GetMaskForSlot(StaticFunctionTag*, UInt32 slot) 
	{
		if (slot < 29 || slot > 61)
			return 0;

		return (1 << (slot - 30));
	}
}

void papyrusForm::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <TESForm, BSFixedString> ("GetName", "Form", papyrusForm::GetName, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, BSFixedString> ("SetName", "Form", papyrusForm::SetName, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, BSFixedString> ("GetDescription", "Form", papyrusForm::GetDescription, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, float> ("GetWeight", "Form", papyrusForm::GetWeight, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, float> ("SetWeight", "Form", papyrusForm::SetWeight, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, UInt32> ("SetGoldValue", "Form", papyrusForm::SetGoldValue, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, VMArray<BGSKeyword*>> ("GetKeywords", "Form", papyrusForm::GetKeywords, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, bool> ("HasWorldModel", "Form", papyrusForm::HasWorldModel, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, BSFixedString> ("GetWorldModelPath", "Form", papyrusForm::GetWorldModelPath, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, BSFixedString> ("SetWorldModelPath", "Form", papyrusForm::SetWorldModelPath, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, BSFixedString>("GetIconPath", "Form", papyrusForm::GetIconPath, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, BSFixedString>("SetIconPath", "Form", papyrusForm::SetIconPath, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, BSFixedString>("GetMessageIconPath", "Form", papyrusForm::GetMessageIconPath, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, BSFixedString>("SetMessageIconPath", "Form", papyrusForm::SetMessageIconPath, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, EnchantmentItem*>("GetEnchantment", "Form", papyrusForm::GetEnchantment, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, EnchantmentItem*>("SetEnchantment", "Form", papyrusForm::SetEnchantment, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, UInt32>("GetEnchantmentValue", "Form", papyrusForm::GetEnchantmentValue, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, UInt32>("SetEnchantmentValue", "Form", papyrusForm::SetEnchantmentValue, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, BGSEquipSlot*>("GetEquipType", "Form", papyrusForm::GetEquipType, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, BGSEquipSlot*>("SetEquipType", "Form", papyrusForm::SetEquipType, vm));

	vm->RegisterFunction(
		new NativeFunction0 <TESForm, TESRace*> ("GetRaceForm", "Form", papyrusForm::GetRaceForm, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, TESRace*> ("SetRaceForm", "Form", papyrusForm::SetRaceForm, vm));

	// Slot Mask
	vm->RegisterFunction(
		new NativeFunction0 <TESForm, UInt32>("GetSlotMask", "Form", papyrusForm::GetSlotMask, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, void, UInt32>("SetSlotMask", "Form", papyrusForm::SetSlotMask, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, UInt32, UInt32>("AddSlotToMask", "Form", papyrusForm::AddSlotToMask, vm));

	vm->RegisterFunction(
		new NativeFunction1 <TESForm, UInt32, UInt32>("RemoveSlotFromMask", "Form", papyrusForm::RemoveSlotFromMask, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, UInt32> ("GetMaskForSlot", "Form", papyrusForm::GetMaskForSlot, vm));
}
