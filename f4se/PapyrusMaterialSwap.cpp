#include "f4se/PapyrusMaterialSwap.h"

#include "f4se/GameObjects.h"
#include "f4se/GameData.h"

#include "f4se/PapyrusStruct.h"

DECLARE_STRUCT(RemapData, "MatSwap");

namespace papyrusMaterialSwap
{
	VMArray<RemapData> GetRemapData(BGSMaterialSwap * materialSwap)
	{
		VMArray<RemapData> result;
		if(!materialSwap)
			return result;

		materialSwap->materialSwaps.ForEach([&](BGSMaterialSwap::MaterialSwap * swap)
		{
			RemapData swapData;
			swapData.Set("source", swap->source);
			swapData.Set("target", swap->target);
			swapData.Set("colorIndex", swap->colorRemapIndex);
			result.Push(&swapData);
			return true;
		});
		return result;
	}

	void SetRemapData(BGSMaterialSwap * materialSwap, VMArray<RemapData> newData)
	{
		if(materialSwap && newData.Length())
		{
			materialSwap->materialSwaps.Clear();

			for(int i = 0; i < newData.Length(); ++i)
			{
				RemapData swapData;
				newData.Get(&swapData, i);

				BGSMaterialSwap::MaterialSwap matSwap;
				swapData.Get("source", &matSwap.source);
				swapData.Get("target", &matSwap.target);
				swapData.Get("colorIndex", &matSwap.colorRemapIndex);
				matSwap.unk14 = 0;
				materialSwap->materialSwaps.Add(&matSwap);
			}
		}
	}
}

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

void papyrusMaterialSwap::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterForm(BGSMaterialSwap::kTypeID, "MatSwap");

	vm->RegisterFunction(
		new NativeFunction0 <BGSMaterialSwap, VMArray<RemapData>>("GetRemapData", "MatSwap", papyrusMaterialSwap::GetRemapData, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSMaterialSwap, void, VMArray<RemapData>>("SetRemapData", "MatSwap", papyrusMaterialSwap::SetRemapData, vm));
}
