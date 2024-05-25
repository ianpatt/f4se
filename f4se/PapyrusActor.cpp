#include "f4se/PapyrusActor.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusArgs.h"
#include "f4se/PapyrusStruct.h"

#include "f4se/GameForms.h"
#include "f4se/GameReferences.h"
#include "f4se/GameRTTI.h"
#include "f4se/GameObjects.h"
#include "f4se/GameExtraData.h"

#include "f4se/PapyrusDelayFunctors.h"

#include "f4se/NiRTTI.h"
#include "f4se/NiNodes.h"
#include "f4se/BSGeometry.h"
#include "f4se/NiProperties.h"
#include "f4se/NiMaterials.h"
	
namespace papyrusActor
{
	DECLARE_STRUCT(WornItem, "Actor")

	WornItem GetWornItem(Actor* actor, UInt32 slotIndex, bool bFirstPerson)
	{
		WornItem result;
		result.Set<TESForm*>("item", nullptr);
		result.Set<TESForm*>("model", nullptr);
		result.Set<BSFixedString>("modelName", "");
		result.Set<TESForm*>("materialSwap", nullptr);
		result.Set<BGSTextureSet*>("texture", nullptr);

		if(slotIndex >= BIPOBJECT::BIPED_OBJECT::kTotal) {
			result.SetNone(true);
			return result;
		}

		BipedAnim* equipData = actor->biped.get();
		PlayerCharacter * pPC = DYNAMIC_CAST(actor, Actor, PlayerCharacter);
		if(pPC && bFirstPerson)
			equipData = pPC->playerEquipData.get();

		if(equipData) {
			auto materialSwap = DYNAMIC_CAST(equipData->object[slotIndex].part, TESModel, BGSModelMaterialSwap);
			result.Set<TESForm*>("item", equipData->object[slotIndex].parent.object);
			result.Set<TESForm*>("model", equipData->object[slotIndex].armorAddon);
			if(materialSwap) {
				result.Set<BSFixedString>("modelName", materialSwap->GetModelName());
				result.Set<TESForm*>("materialSwap", materialSwap->materialSwap);
			}
			result.Set<BGSTextureSet*>("texture", equipData->object[slotIndex].skinTexture);
		}

		return result;
	}

	VMArray<BGSMod::Attachment::Mod*> GetWornItemMods(Actor* actor, UInt32 slotIndex)
	{
		VMArray<BGSMod::Attachment::Mod*> result;
		result.SetNone(true);

		// Invalid slot id
		if(slotIndex >= BIPOBJECT::BIPED_OBJECT::kTotal)
			return result;

		ExtraDataList * stackDataList = nullptr;
		if (actor->GetEquippedExtraData(slotIndex, &stackDataList) && stackDataList)
		{
			result.SetNone(false);

			BSExtraData * extraData = stackDataList->GetByType(ExtraDataType::kExtraData_ObjectInstance);
			if (extraData)
			{
				BGSObjectInstanceExtra * objectModData = DYNAMIC_CAST(extraData, BSExtraData, BGSObjectInstanceExtra);
				if (objectModData)
				{
					auto data = objectModData->data;
					if (!data || !data->forms)
						return result;

					for (UInt32 i = 0; i < data->blockSize / sizeof(BGSObjectInstanceExtra::Data::Form); i++)
					{
						BGSMod::Attachment::Mod * objectMod = (BGSMod::Attachment::Mod *)Runtime_DynamicCast(LookupFormByID(data->forms[i].formId), RTTI_TESForm, RTTI_BGSMod__Attachment__Mod);
						result.Push(&objectMod);
					}
				}
			}
		}

		return result;
	}

	void QueueUpdate(Actor * actor, bool bDoEquipment, UInt32 flags)
	{
		if(actor) {
			actor->Reset3D(bDoEquipment, 0, true, flags);
		}
	}

	TESObjectREFR * GetFurnitureReference(Actor * actor)
	{
		NiPointer<TESObjectREFR> refr;
		if(!actor)
			return nullptr;

		auto middleProcess = actor->middleProcess;
		if(!middleProcess)
			return nullptr;

		UInt32 furnitureHandle = 0;
		auto data08 = middleProcess->unk08;
		if(!data08)
			return nullptr;

		if(actor->actorState.flags & (ActorState::Flags::kUnk1 | ActorState::Flags::kUnk2))
			furnitureHandle = data08->furnitureHandle2;
		else
			furnitureHandle = data08->furnitureHandle1;

		LookupREFRByHandle(furnitureHandle, refr);
		return refr;
	}

	bool IsProtected(Actor * actor)
	{
		return actor && (actor->uiFlags & 0x80000) != 0;
	}
}

void papyrusActor::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction2<Actor, WornItem, UInt32, bool>("GetWornItem", "Actor", papyrusActor::GetWornItem, vm));

	vm->RegisterFunction(
		new NativeFunction1<Actor, VMArray<BGSMod::Attachment::Mod*>, UInt32>("GetWornItemMods", "Actor", papyrusActor::GetWornItemMods, vm));

	vm->RegisterFunction(
		new NativeFunction2<Actor, void, bool, UInt32>("QueueUpdate", "Actor", papyrusActor::QueueUpdate, vm));

	vm->RegisterFunction(
		new NativeFunction0<Actor, TESObjectREFR*>("GetFurnitureReference", "Actor", papyrusActor::GetFurnitureReference, vm));

	vm->RegisterFunction(
		new NativeFunction0<Actor, bool>("IsProtected", "Actor", papyrusActor::IsProtected, vm));
}
