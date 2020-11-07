#include "f4se/PapyrusHeadPart.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/GameObjects.h"
#include "f4se/GameData.h"
 
namespace papyrusHeadPart
{	
	UInt32 GetType(BGSHeadPart* thisPart)
	{
		if(!thisPart)
			return 0;
		return thisPart->type;
	}
	
	VMArray<BGSHeadPart*> GetExtraParts(BGSHeadPart* thisPart)
	{
		VMArray<BGSHeadPart*> result;
		if(!thisPart)
			return result;

		BGSHeadPart* headPart;
		for(UInt32 i = 0; i < thisPart->extraParts.count; i++)
		{
			thisPart->extraParts.GetNthItem(i, headPart);
			result.Push(&headPart);
		}

		return result;
	}

	bool IsExtraPart(BGSHeadPart* thisPart)
	{
		return (thisPart && thisPart->IsExtraPart()) ? true : false;
	}

	bool HasExtraPart(BGSHeadPart* thisPart, BGSHeadPart* extraPart)
	{
		return (thisPart && thisPart->extraParts.GetItemIndex(extraPart) != -1) ? true : false;
	}
		
	BGSListForm* GetValidRaces(BGSHeadPart* thisPart)
	{
		return (thisPart) ? thisPart->validRaces : NULL;
	}
	
	void SetValidRaces(BGSHeadPart* thisPart, BGSListForm* raceList)
	{
		if(thisPart && raceList) {
			thisPart->validRaces = raceList;
		}
	}
}

void papyrusHeadPart::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0<BGSHeadPart, UInt32>("GetType", "HeadPart", papyrusHeadPart::GetType, vm));

	vm->RegisterFunction(
		new NativeFunction0<BGSHeadPart, VMArray<BGSHeadPart*>>("GetExtraParts", "HeadPart", papyrusHeadPart::GetExtraParts, vm));

	vm->RegisterFunction(
		new NativeFunction1<BGSHeadPart, bool, BGSHeadPart*>("HasExtraPart", "HeadPart", papyrusHeadPart::HasExtraPart, vm));

	vm->RegisterFunction(
		new NativeFunction0<BGSHeadPart, bool>("IsExtraPart", "HeadPart", papyrusHeadPart::IsExtraPart, vm));

	vm->RegisterFunction(
		new NativeFunction0<BGSHeadPart, BGSListForm*>("GetValidRaces", "HeadPart", papyrusHeadPart::GetValidRaces, vm));

	vm->RegisterFunction(
		new NativeFunction1<BGSHeadPart, void, BGSListForm*>("SetValidRaces", "HeadPart", papyrusHeadPart::SetValidRaces, vm));
}
