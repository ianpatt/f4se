#include "f4se/PapyrusInstanceData.h"

#include "f4se/GameObjects.h"
#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "f4se/GameExtraData.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

namespace papyrusInstanceData
{
	DECLARE_STRUCT(DamageTypeInfo, "InstanceData")
	DECLARE_STRUCT(Owner, "InstanceData")

	TBO_InstanceData * GetInstanceData(Owner* thisInstance)
	{
		if(!thisInstance || thisInstance->IsNone())
			return nullptr;

		TESForm * form = nullptr;
		ExtraDataList * extraDataList = nullptr;

		if(!thisInstance->Get("owner", &form))
			return nullptr;

		// Must be an inventory object, or doesn't exist
		if(!form) {
			VMRefOrInventoryObj ref;
			if(!thisInstance->Get("owner", &ref))
				return nullptr;

			// Try to get ExtraDataList instead
			ref.GetExtraData(&form, &extraDataList);
		}

		// Passed a reference directly, just get the instance data directly
		TESObjectREFR * refr = DYNAMIC_CAST(form, TESForm, TESObjectREFR);
		if(refr)
			extraDataList = refr->extraDataList;

		if(extraDataList) {
			BSExtraData * extraData = extraDataList->GetByType(ExtraDataType::kExtraData_InstanceData);
			if(extraData) {
				ExtraInstanceData * objectModData = DYNAMIC_CAST(extraData, BSExtraData, ExtraInstanceData);
				if(objectModData)
					return objectModData->instanceData;
			}
			else {
				TESBoundObject * boundObject = DYNAMIC_CAST(form, TESForm, TESBoundObject);
				TBO_InstanceData * instanceData = nullptr;
				if(boundObject) {
					instanceData = boundObject->CloneInstanceData(nullptr);
					if(instanceData) {
						ExtraInstanceData * objectModData = ExtraInstanceData::Create(form, instanceData);
						if(objectModData) {
							extraDataList->Add(ExtraDataType::kExtraData_InstanceData, objectModData);
							return instanceData;
						}
					}
				}
			}
		}

		TESObjectWEAP * weapon = DYNAMIC_CAST(form, TESForm, TESObjectWEAP);
		if(weapon) {
			return &weapon->weapData;
		}

		TESObjectARMO * armor = DYNAMIC_CAST(form, TESForm, TESObjectARMO);
		if(armor) {
			return &armor->instanceData;
		}

		Actor * actor = DYNAMIC_CAST(form, TESForm, Actor);
		if(actor) {
			UInt32 iSlotIndex = 0;
			if(!thisInstance->Get("slotIndex", &iSlotIndex))
				return nullptr;

			// Invalid slot id
			if(iSlotIndex >= ActorEquipData::kMaxSlots)
				return nullptr;

			ActorEquipData * equipData = actor->equipData;
			if(!equipData)
				return nullptr;

			// Make sure there is an item in this slot
			auto item = equipData->slots[iSlotIndex].item;
			if(!item)
				return nullptr;

			return equipData->slots[iSlotIndex].instanceData;
		}

		return nullptr;
	}

	TESObjectWEAP::InstanceData * GetWeaponInstanceData(Owner* thisInstance)
	{
		TBO_InstanceData * instanceData = GetInstanceData(thisInstance);
		if(!instanceData)
			return nullptr;

		return (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
	}

	TESObjectARMO::InstanceData * GetArmorInstanceData(Owner* thisInstance)
	{
		TBO_InstanceData * instanceData = GetInstanceData(thisInstance);
		if(!instanceData)
			return nullptr;

		return (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
	}


	UInt32 GetAttackDamage(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->baseDamage : 0;
	}

	void SetAttackDamage(StaticFunctionTag*, Owner thisInstance, UInt32 nuDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->baseDamage = max(0, min(nuDamage, 0xFFFF));
		}
	}

	TESLevItem* GetAddAmmoList(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->addAmmoList : nullptr;
	}

	void SetAddAmmoList(StaticFunctionTag*, Owner thisInstance, TESLevItem* item)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->addAmmoList = item;
		}
	}

	UInt32 GetAmmoCapacity(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->ammoCapacity : 0;
	}

	void SetAmmoCapacity(StaticFunctionTag*, Owner thisInstance, UInt32 nuDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->ammoCapacity = max(0, min(nuDamage, 0xFFFF));
		}
	}

	TESAmmo * GetAmmo(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->ammo : nullptr;
	}

	void SetAmmo(StaticFunctionTag*, Owner thisInstance, TESAmmo * ammo)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->ammo = ammo;
		}
	}

	VMArray<DamageTypeInfo> GetDamageTypes(StaticFunctionTag*, Owner thisInstance)
	{
		VMArray<DamageTypeInfo> result;
		tArray<TBO_InstanceData::DamageTypes> * damageTypes = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(!instanceData)
			return result;

		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				damageTypes = weaponInstance->damageTypes;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				damageTypes = armorInstance->damageTypes;
		}

		if(!damageTypes)
			return result;

		for(UInt32 i = 0; i < damageTypes->count; i++)
		{
			TESObjectWEAP::InstanceData::DamageTypes dt;
			damageTypes->GetNthItem(i, dt);

			DamageTypeInfo dts;
			dts.Set<TESForm*>("type", dt.damageType);
			dts.Set<UInt32>("damage", dt.value);
			result.Push(&dts);
		}

		return result;
	}

	void SetDamageTypes(StaticFunctionTag*, Owner thisInstance, VMArray<DamageTypeInfo> dts)
	{
		tArray<TBO_InstanceData::DamageTypes> ** damageTypes = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				damageTypes = &weaponInstance->damageTypes;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				damageTypes = &armorInstance->damageTypes;
		}

		if(damageTypes)
		{
			if(!(*damageTypes))
				(*damageTypes) = new tArray<TBO_InstanceData::DamageTypes>();

			(*damageTypes)->Clear();

			for(UInt32 i = 0; i < dts.Length(); i++)
			{
				DamageTypeInfo dti;
				dts.Get(&dti, i);

				UInt32 damage = 0;
				TESForm * damageType = nullptr;
				dti.Get("type", &damageType);
				dti.Get("damage", &damage);

				if(damageType && damageType->formType == BGSDamageType::kTypeID)
				{
					TESObjectWEAP::InstanceData::DamageTypes dt;
					dt.damageType = (BGSDamageType*)damageType;
					dt.value = damage;
					(*damageTypes)->Push(dt);
				}
			}
		}
	}

	UInt32 GetAccuracyBonus(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->accuracyBonus : 0;
	}

	void SetAccuracyBonus(StaticFunctionTag*, Owner thisInstance, UInt32 nuDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->accuracyBonus = max(0, min(nuDamage, 0xFF));
		}
	}

	float GetCritMultiplier(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->critDamageMult : 0.0f;
	}

	void SetCritMultiplier(StaticFunctionTag*, Owner thisInstance, float critDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->critDamageMult = critDamage;
		}
	}

	float GetCritChargeBonus(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->critChargeBonus : 0.0f;
	}

	void SetCritChargeBonus(StaticFunctionTag*, Owner thisInstance, float critDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->critChargeBonus = critDamage;
		}
	}

	float GetAttackDelay(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->attackDelay : 0.0f;
	}

	void SetAttackDelay(StaticFunctionTag*, Owner thisInstance, float attackDelay)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->attackDelay = attackDelay;
		}
	}

	float GetOutOfRangeMultiplier(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->outOfRangeMultiplier : 0.0f;
	}

	void SetOutOfRangeMultiplier(StaticFunctionTag*, Owner thisInstance, float multiplier)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->outOfRangeMultiplier = multiplier;
		}
	}

	float GetActionPointCost(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->actionCost : 0.0f;
	}

	void SetActionPointCost(StaticFunctionTag*, Owner thisInstance, float cost)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->actionCost = cost;
		}
	}

	float GetReloadSpeed(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->reloadSpeed : 0.0f;
	}

	void SetReloadSpeed(StaticFunctionTag*, Owner thisInstance, float reloadSpeed)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->reloadSpeed = reloadSpeed;
		}
	}

	float GetReach(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->reach : 0;
	}

	void SetReach(StaticFunctionTag*, Owner thisInstance, float nuReach)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if(instanceData) {
			instanceData->reach = nuReach;
		}
	}

	float GetSpeed(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->speed : 0.0;
	}

	void SetSpeed(StaticFunctionTag*, Owner thisInstance, float speed)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->speed = speed;
		}
	}

	UInt32 GetStagger(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->stagger : 0.0;
	}

	void SetStagger(StaticFunctionTag*, Owner thisInstance, UInt32 stagger)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->stagger = max(0, min(stagger, 4));
		}
	}

	float GetMinRange(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->minRange : 0.0;
	}

	void SetMinRange(StaticFunctionTag*, Owner thisInstance, float minRange)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->minRange = minRange;
		}
	}

	float GetMaxRange(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? instanceData->maxRange : 0.0;
	}

	void SetMaxRange(StaticFunctionTag*, Owner thisInstance, float maxRange)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			instanceData->maxRange = maxRange;
		}
	}

	ActorValueInfo * GetSkill(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return instanceData ? instanceData->skill : nullptr;
	}

	void SetSkill(StaticFunctionTag*, Owner thisInstance, ActorValueInfo * skill)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if(instanceData) {
			instanceData->skill = skill;
		}
	}

	ActorValueInfo * GetResist(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return instanceData ? instanceData->damageResist : nullptr;
	}

	void SetResist(StaticFunctionTag*, Owner thisInstance, ActorValueInfo * resist)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if(instanceData) {
			instanceData->damageResist = resist;
		}
	}

	BGSProjectile * GetProjectileOverride(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return instanceData && instanceData->firingData ? instanceData->firingData->projectileOverride : nullptr;
	}

	void SetProjectileOverride(StaticFunctionTag*, Owner thisInstance, BGSProjectile * proj)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if(instanceData && instanceData->firingData) {
			instanceData->firingData->projectileOverride = proj;
		}
	}

	UInt32 GetNumProjectiles(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return instanceData && instanceData->firingData ? instanceData->firingData->numProjectiles : 0;
	}

	void SetNumProjectiles(StaticFunctionTag*, Owner thisInstance, UInt32 nuDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if(instanceData && instanceData->firingData) {
			instanceData->firingData->numProjectiles = max(0, min(nuDamage, 0xFF));
		}
	}

	float GetSightedTransition(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return instanceData && instanceData->firingData ? instanceData->firingData->sightedTransition : 0;
	}

	void SetSightedTransition(StaticFunctionTag*, Owner thisInstance, float nuDamage)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if(instanceData && instanceData->firingData) {
			instanceData->firingData->sightedTransition = nuDamage;
		}
	}

	bool GetFlag(StaticFunctionTag*, Owner thisInstance, UInt32 flagNumber)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		return (instanceData) ? ((instanceData->flags & flagNumber) == flagNumber) : false;
	}

	void SetFlag(StaticFunctionTag*, Owner thisInstance, UInt32 flagNumber, bool set)
	{
		auto instanceData = GetWeaponInstanceData(&thisInstance);
		if (instanceData) {
			if(set) {
				instanceData->flags |= flagNumber;
			} else {
				instanceData->flags &= ~flagNumber;
			}
		}
	}


	UInt32 GetArmorHealth(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetArmorInstanceData(&thisInstance);
		return instanceData ? instanceData->health : 0;
	}

	void SetArmorHealth(StaticFunctionTag*, Owner thisInstance, UInt32 health)
	{
		auto instanceData = GetArmorInstanceData(&thisInstance);
		if(instanceData) {
			instanceData->health = health;
		}
	}

	UInt32 GetArmorRating(StaticFunctionTag*, Owner thisInstance)
	{
		auto instanceData = GetArmorInstanceData(&thisInstance);
		return instanceData ? instanceData->armorRating : 0;
	}

	void SetArmorRating(StaticFunctionTag*, Owner thisInstance, UInt32 health)
	{
		auto instanceData = GetArmorInstanceData(&thisInstance);
		if(instanceData) {
			instanceData->armorRating = health;
		}
	}

	float GetWeight(StaticFunctionTag*, Owner thisInstance)
	{
		float * weight = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				weight = &weaponInstance->weight;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				weight = &armorInstance->weight;
		}

		return weight ? *weight : 0.0f;
	}

	void SetWeight(StaticFunctionTag*, Owner thisInstance, float newWeight)
	{
		float * weight = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				weight = &weaponInstance->weight;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				weight = &armorInstance->weight;
		}

		if(weight)
			*weight = newWeight;
	}

	UInt32 GetGoldValue(StaticFunctionTag*, Owner thisInstance)
	{
		UInt32 * value = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				value = &weaponInstance->value;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				value = &armorInstance->value;
		}

		return value ? *value : 0;
	}

	void SetGoldValue(StaticFunctionTag*, Owner thisInstance, UInt32 newValue)
	{
		UInt32 * value = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				value = &weaponInstance->value;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				value = &armorInstance->value;
		}

		if(value)
			*value = newValue;
	}
	

	VMArray<BGSKeyword*> GetKeywords(StaticFunctionTag*, Owner thisInstance)
	{
		VMArray<BGSKeyword*> result;
		BGSKeywordForm * keywordForm = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(!instanceData)
			return result;

		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				keywordForm = weaponInstance->keywords;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				keywordForm = armorInstance->keywords;
		}

		if(!keywordForm)
			return result;

		for(UInt32 i = 0; i < keywordForm->numKeywords; i++)
		{
			result.Push(&keywordForm->keywords[i]);
		}

		return result;
	}

	void SetKeywords(StaticFunctionTag*, Owner thisInstance, VMArray<BGSKeyword*> kwds)
	{
		BGSKeywordForm * keywordForm = nullptr;
		TBO_InstanceData * instanceData = GetInstanceData(&thisInstance);
		if(instanceData) {
			auto weaponInstance = (TESObjectWEAP::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectWEAP__InstanceData);
			if(weaponInstance)
				keywordForm = weaponInstance->keywords;

			auto armorInstance = (TESObjectARMO::InstanceData*)Runtime_DynamicCast(instanceData, RTTI_TBO_InstanceData, RTTI_TESObjectARMO__InstanceData);
			if(armorInstance)
				keywordForm = armorInstance->keywords;
		}

		if(keywordForm)
		{
			if(keywordForm->keywords) {
				Heap_Free(keywordForm->keywords);
				keywordForm->keywords = nullptr;
				keywordForm->numKeywords = 0;
			}
			if(kwds.Length() > 0)
			{
				keywordForm->keywords = (BGSKeyword**)Heap_Allocate(sizeof(BGSKeyword*) * kwds.Length());
				keywordForm->numKeywords = kwds.Length();

				for(UInt32 i = 0; i < kwds.Length(); i++)
				{
					BGSKeyword * kwd = nullptr;
					kwds.Get(&kwd, i);
					keywordForm->keywords[i] = kwd;
				}
			}
		}
	}
}

void papyrusInstanceData::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetAttackDamage", "InstanceData", papyrusInstanceData::GetAttackDamage, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetAttackDamage", "InstanceData", papyrusInstanceData::SetAttackDamage, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, VMArray<DamageTypeInfo>, Owner>("GetDamageTypes", "InstanceData", papyrusInstanceData::GetDamageTypes, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, VMArray<DamageTypeInfo>>("SetDamageTypes", "InstanceData", papyrusInstanceData::SetDamageTypes, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetAmmoCapacity", "InstanceData", papyrusInstanceData::GetAmmoCapacity, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetAmmoCapacity", "InstanceData", papyrusInstanceData::SetAmmoCapacity, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, TESAmmo*, Owner>("GetAmmo", "InstanceData", papyrusInstanceData::GetAmmo, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, TESAmmo*>("SetAmmo", "InstanceData", papyrusInstanceData::SetAmmo, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, TESLevItem*, Owner>("GetAddAmmoList", "InstanceData", papyrusInstanceData::GetAddAmmoList, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, TESLevItem*>("SetAddAmmoList", "InstanceData", papyrusInstanceData::SetAddAmmoList, vm));


	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetAccuracyBonus", "InstanceData", papyrusInstanceData::GetAccuracyBonus, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetAccuracyBonus", "InstanceData", papyrusInstanceData::SetAccuracyBonus, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetActionPointCost", "InstanceData", papyrusInstanceData::GetActionPointCost, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetActionPointCost", "InstanceData", papyrusInstanceData::SetActionPointCost, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetAttackDelay", "InstanceData", papyrusInstanceData::GetAttackDelay, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetAttackDelay", "InstanceData", papyrusInstanceData::SetAttackDelay, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetOutOfRangeMultiplier", "InstanceData", papyrusInstanceData::GetOutOfRangeMultiplier, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetOutOfRangeMultiplier", "InstanceData", papyrusInstanceData::SetOutOfRangeMultiplier, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetReloadSpeed", "InstanceData", papyrusInstanceData::GetReloadSpeed, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetReloadSpeed", "InstanceData", papyrusInstanceData::SetReloadSpeed, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetReach", "InstanceData", papyrusInstanceData::GetReach, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetReach", "InstanceData", papyrusInstanceData::SetReach, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetMinRange", "InstanceData", papyrusInstanceData::GetMinRange, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetMinRange", "InstanceData", papyrusInstanceData::SetMinRange, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetMaxRange", "InstanceData", papyrusInstanceData::GetMaxRange, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetMaxRange", "InstanceData", papyrusInstanceData::SetMaxRange, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetSpeed", "InstanceData", papyrusInstanceData::GetSpeed, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetSpeed", "InstanceData", papyrusInstanceData::SetSpeed, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetStagger", "InstanceData", papyrusInstanceData::GetStagger, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetStagger", "InstanceData", papyrusInstanceData::SetStagger, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, ActorValueInfo*, Owner>("GetSkill", "InstanceData", papyrusInstanceData::GetSkill, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, ActorValueInfo*>("SetSkill", "InstanceData", papyrusInstanceData::SetSkill, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, ActorValueInfo*, Owner>("GetResist", "InstanceData", papyrusInstanceData::GetResist, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, ActorValueInfo*>("SetResist", "InstanceData", papyrusInstanceData::SetResist, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetCritMultiplier", "InstanceData", papyrusInstanceData::GetCritMultiplier, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetCritMultiplier", "InstanceData", papyrusInstanceData::SetCritMultiplier, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetCritChargeBonus", "InstanceData", papyrusInstanceData::GetCritChargeBonus, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetCritChargeBonus", "InstanceData", papyrusInstanceData::SetCritChargeBonus, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, BGSProjectile*, Owner>("GetProjectileOverride", "InstanceData", papyrusInstanceData::GetProjectileOverride, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, BGSProjectile*>("SetProjectileOverride", "InstanceData", papyrusInstanceData::SetProjectileOverride, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetNumProjectiles", "InstanceData", papyrusInstanceData::GetNumProjectiles, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetNumProjectiles", "InstanceData", papyrusInstanceData::SetNumProjectiles, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetSightedTransition", "InstanceData", papyrusInstanceData::GetSightedTransition, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetSightedTransition", "InstanceData", papyrusInstanceData::SetSightedTransition, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, bool, Owner, UInt32>("GetFlag", "InstanceData", papyrusInstanceData::GetFlag, vm));

	vm->RegisterFunction(
		new NativeFunction3 <StaticFunctionTag, void, Owner, UInt32, bool>("SetFlag", "InstanceData", papyrusInstanceData::SetFlag, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetArmorHealth", "InstanceData", papyrusInstanceData::GetArmorHealth, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetArmorHealth", "InstanceData", papyrusInstanceData::SetArmorHealth, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetArmorRating", "InstanceData", papyrusInstanceData::GetArmorRating, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetArmorRating", "InstanceData", papyrusInstanceData::SetArmorRating, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, float, Owner>("GetWeight", "InstanceData", papyrusInstanceData::GetWeight, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, float>("SetWeight", "InstanceData", papyrusInstanceData::SetWeight, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, UInt32, Owner>("GetGoldValue", "InstanceData", papyrusInstanceData::GetGoldValue, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, UInt32>("SetGoldValue", "InstanceData", papyrusInstanceData::SetGoldValue, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, VMArray<BGSKeyword*>, Owner>("GetKeywords", "InstanceData", papyrusInstanceData::GetKeywords, vm));

	vm->RegisterFunction(
		new NativeFunction2 <StaticFunctionTag, void, Owner, VMArray<BGSKeyword*>>("SetKeywords", "InstanceData", papyrusInstanceData::SetKeywords, vm));
}
