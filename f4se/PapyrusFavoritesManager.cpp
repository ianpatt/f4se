#include "f4se/PapyrusFavoritesManager.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusDelayFunctors.h"

#include "f4se/GameData.h"

namespace papyrusFavoritesManager {

	VMArray<TESForm*> GetTaggedFormsLatent(UInt32 stackId, StaticFunctionTag *)
	{
		VMArray<TESForm*> result;
		(*g_favoritesManager)->taggedForms.ForEach([&](FavoritesManager::TaggedEntry * entry)
		{
			if(entry->form)
			{
				result.Push(&entry->form);
			}
			
			return true;	
		});
		return result;
	}

	DECLARE_DELAY_FUNCTOR(F4SEGetTaggedFormsFunctor, 0, GetTaggedFormsLatent, StaticFunctionTag, VMArray<TESForm*>);

	bool GetTaggedForms(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag* s)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEGetTaggedFormsFunctor(GetTaggedFormsLatent, vm, stackId, s));
		return true;
	}

	void AddTaggedFormsLatent(UInt32 stackId, StaticFunctionTag *, VMArray<TESForm*> forms)
	{
		for(int i = 0; i < forms.Length(); ++i)
		{
			TESForm * form = nullptr;
			forms.Get(&form, i);

			if(form) {
				FavoritesManager::TaggedEntry entry;
				entry.form = form;
				entry.unk08 = 1;
				(*g_favoritesManager)->taggedForms.Add(&entry);
			}
		}
	}

	DECLARE_DELAY_FUNCTOR(F4SEAddTaggedFormsFunctor, 1, AddTaggedFormsLatent, StaticFunctionTag, void, VMArray<TESForm*>);

	bool AddTaggedForms(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag* s, VMArray<TESForm*> forms)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEAddTaggedFormsFunctor(AddTaggedFormsLatent, vm, stackId, s, forms));
		return true;
	}

	void RemoveTaggedFormsLatent(UInt32 stackId, StaticFunctionTag *, VMArray<TESForm*> forms)
	{
		for(int i = 0; i < forms.Length(); ++i)
		{
			TESForm * form = nullptr;
			forms.Get(&form, i);

			if(form) {
				(*g_favoritesManager)->taggedForms.Remove(&form);
			}
		}
	}

	DECLARE_DELAY_FUNCTOR(F4SERemoveTaggedFormsFunctor, 1, RemoveTaggedFormsLatent, StaticFunctionTag, void, VMArray<TESForm*>);

	bool RemoveTaggedForms(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag* s, VMArray<TESForm*> forms)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SERemoveTaggedFormsFunctor(RemoveTaggedFormsLatent, vm, stackId, s, forms));
		return true;
	}

	bool IsTaggedFormLatent(UInt32 stackId, StaticFunctionTag *, TESForm* form)
	{
		if(form) {
			return (*g_favoritesManager)->taggedForms.Find(&form) != nullptr;
		}

		return false;
	}

	DECLARE_DELAY_FUNCTOR(F4SEIsTaggedFormFunctor, 1, IsTaggedFormLatent, StaticFunctionTag, bool, TESForm*);

	bool IsTaggedForm(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag* s, TESForm* form)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEIsTaggedFormFunctor(IsTaggedFormLatent, vm, stackId, s, form));
		return true;
	}

	VMArray<TESForm*> GetFavoritesLatent(UInt32 stackId, StaticFunctionTag *)
	{
		VMArray<TESForm*> result;
		for(int i = 0; i < FavoritesManager::kNumFavorites; ++i)
		{
			TESForm * form = (*g_favoritesManager)->favorites[i];
			result.Push(&form);
		}
		
		return result;
	}

	DECLARE_DELAY_FUNCTOR(F4SEGetFavoritesFunctor, 0, GetFavoritesLatent, StaticFunctionTag, VMArray<TESForm*>);

	bool GetFavorites(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag* s)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEGetFavoritesFunctor(GetFavoritesLatent, vm, stackId, s));
		return true;
	}

	void SetFavoritesLatent(UInt32 stackId, StaticFunctionTag *, VMArray<TESForm*> forms)
	{
		for(int i = 0; i < FavoritesManager::kNumFavorites; ++i)
		{
			TESForm * form = nullptr;
			if(i < forms.Length()) {
				forms.Get(&form, i);
			}
			(*g_favoritesManager)->favorites[i] = form;
		}
	}

	DECLARE_DELAY_FUNCTOR(F4SESetFavoritesFunctor, 1, SetFavoritesLatent, StaticFunctionTag, void, VMArray<TESForm*>);

	bool SetFavorites(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag* s, VMArray<TESForm*> forms)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SESetFavoritesFunctor(SetFavoritesLatent, vm, stackId, s, forms));
		return true;
	}
}

void papyrusFavoritesManager::RegisterFuncs(VirtualMachine* vm)
{
	F4SEObjectRegistry& f4seObjRegistry = F4SEObjectRegistryInstance();
	f4seObjRegistry.RegisterClass<F4SEGetTaggedFormsFunctor>();
	f4seObjRegistry.RegisterClass<F4SEAddTaggedFormsFunctor>();
	f4seObjRegistry.RegisterClass<F4SERemoveTaggedFormsFunctor>();
	f4seObjRegistry.RegisterClass<F4SEIsTaggedFormFunctor>();
	f4seObjRegistry.RegisterClass<F4SESetFavoritesFunctor>();
	f4seObjRegistry.RegisterClass<F4SEGetFavoritesFunctor>();

	vm->RegisterFunction(
		new LatentNativeFunction0<StaticFunctionTag, VMArray<TESForm*>>("GetTaggedForms", "FavoritesManager", papyrusFavoritesManager::GetTaggedForms, vm));

	vm->RegisterFunction(
		new LatentNativeFunction1<StaticFunctionTag, void, VMArray<TESForm*>>("AddTaggedForms", "FavoritesManager", papyrusFavoritesManager::AddTaggedForms, vm));

	vm->RegisterFunction(
		new LatentNativeFunction1<StaticFunctionTag, void, VMArray<TESForm*>>("RemoveTaggedForms", "FavoritesManager", papyrusFavoritesManager::RemoveTaggedForms, vm));

	vm->RegisterFunction(
		new LatentNativeFunction1<StaticFunctionTag, bool, TESForm*>("IsTaggedForm", "FavoritesManager", papyrusFavoritesManager::IsTaggedForm, vm));

	vm->RegisterFunction(
		new LatentNativeFunction0<StaticFunctionTag, VMArray<TESForm*>>("GetFavorites", "FavoritesManager", papyrusFavoritesManager::GetFavorites, vm));

	vm->RegisterFunction(
		new LatentNativeFunction1<StaticFunctionTag, void, VMArray<TESForm*>>("SetFavorites", "FavoritesManager", papyrusFavoritesManager::SetFavorites, vm));

	vm->SetFunctionFlags("FavoritesManager", "GetTaggedForms", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("FavoritesManager", "AddTaggedForms", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("FavoritesManager", "RemoveTaggedForms", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("FavoritesManager", "IsTaggedForm", IFunction::kFunctionFlag_NoWait);

	vm->SetFunctionFlags("FavoritesManager", "GetFavorites", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("FavoritesManager", "SetFavorites", IFunction::kFunctionFlag_NoWait);
}
