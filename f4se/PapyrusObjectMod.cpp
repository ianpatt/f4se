#include "f4se/PapyrusObjectMod.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/GameObjects.h"

#include "f4se/PapyrusStruct.h"

namespace papyrusObjectMod
{
	DECLARE_STRUCT(PropertyModifier, "ObjectMod")

	VMArray<PropertyModifier> GetPropertyModifiers(BGSMod::Attachment::Mod * thisMod)
	{
		VMArray<PropertyModifier> result;
		if(!thisMod)
			return result;

		UInt32 targetOffset = 0;
		switch(thisMod->targetType) {
		case BGSMod::Attachment::Mod::kTargetType_Weapon:
			targetOffset = BGSMod::Container::kWeaponTarget_Offset;
			break;
		case BGSMod::Attachment::Mod::kTargetType_Armor:
			targetOffset = BGSMod::Container::kArmorTarget_Offset;
			break;
		case BGSMod::Attachment::Mod::kTargetType_Actor:
			targetOffset = BGSMod::Container::kActorTarget_Offset;
			break;
		}

		for(UInt32 i = 0; i < thisMod->modContainer.dataSize / sizeof(BGSMod::Container::Data); i++)
		{
			PropertyModifier propMod;

			UInt32 targetType = targetOffset;
			UInt32 op = 0;
			TESForm * form = nullptr;
			float value1 = 0.0f;
			float value2 = 0.0f;

			BGSMod::Container::Data * data = &thisMod->modContainer.data[i];
			targetType += data->target;

			switch(data->op)
			{
			case BGSMod::Container::Data::kOpFlag_Set_Bool:
			case BGSMod::Container::Data::kOpFlag_Or_Bool:
			case BGSMod::Container::Data::kOpFlag_And_Bool:
			case BGSMod::Container::Data::kOpFlag_Set_Enum:
			case BGSMod::Container::Data::kOpFlag_Set_Int:
			case BGSMod::Container::Data::kOpFlag_Add_Int:
				{
					value1 = data->value.i.v1;
					value2 = data->value.i.v2;

					switch(data->op)
					{
						case BGSMod::Container::Data::kOpFlag_Set_Bool:	op = BGSMod::Container::kOperator_Set;	break;
						case BGSMod::Container::Data::kOpFlag_Or_Bool:	op = BGSMod::Container::kOperator_Or;	break;
						case BGSMod::Container::Data::kOpFlag_And_Bool:	op = BGSMod::Container::kOperator_And;	break;
						case BGSMod::Container::Data::kOpFlag_Set_Enum:	op = BGSMod::Container::kOperator_Set;	break;
						case BGSMod::Container::Data::kOpFlag_Set_Int:	op = BGSMod::Container::kOperator_Set;	break;
						case BGSMod::Container::Data::kOpFlag_Add_Int:	op = BGSMod::Container::kOperator_Add;	break;
					}
				}
				break;
			case BGSMod::Container::Data::kOpFlag_Mul_Add_Float:
			//case BGSMod::Container::Data::kOpFlag_Mul_Add_Int: // Yes, mult int is treated the same as float
				{
					value1 = data->value.f.v1;
					value2 = data->value.f.v2;

					switch(data->op)
					{
						case BGSMod::Container::Data::kOpFlag_Mul_Add_Float:	op = BGSMod::Container::kOperator_Mult_Add;	break;
						case BGSMod::Container::Data::kOpFlag_Set_Float:		op = BGSMod::Container::kOperator_Set;		break;
						case BGSMod::Container::Data::kOpFlag_Add_Float:		op = BGSMod::Container::kOperator_Add;		break;
					}
				}
				break;
			case BGSMod::Container::Data::kOpFlag_Set_Form:
			case BGSMod::Container::Data::kOpFlag_Add_Form:
			case BGSMod::Container::Data::kOpFlag_Rem_Form:
				{
					form = data->value.form;

					switch(data->op)
					{
						case BGSMod::Container::Data::kOpFlag_Set_Form:	op = BGSMod::Container::kOperator_Set;	break;
						case BGSMod::Container::Data::kOpFlag_Add_Form:	op = BGSMod::Container::kOperator_Add;	break;
						case BGSMod::Container::Data::kOpFlag_Rem_Form:	op = BGSMod::Container::kOperator_Rem;	break;
					}
				}
				break;
			case BGSMod::Container::Data::kOpFlag_Set_Form_Float:
			case BGSMod::Container::Data::kOpFlag_Add_Form_Float:
			case BGSMod::Container::Data::kOpFlag_Mul_Add_Form_Float:
				{
					form = LookupFormByID(data->value.ff.formId);
					value1 = data->value.ff.v2;

					switch(data->op)
					{
					case BGSMod::Container::Data::kOpFlag_Set_Form_Float:		op = BGSMod::Container::kOperator_Set;		break;
					case BGSMod::Container::Data::kOpFlag_Add_Form_Float:		op = BGSMod::Container::kOperator_Add;		break;
					case BGSMod::Container::Data::kOpFlag_Mul_Add_Form_Float:	op = BGSMod::Container::kOperator_Mult_Add;	break;
					}
				}
				break;
			}

			propMod.Set<UInt32>("target", targetType);
			propMod.Set<UInt32>("operator", op);
			propMod.Set<TESForm*>("object", form);
			propMod.Set<float>("value1", value1);
			propMod.Set<float>("value2", value2);

			result.Push(&propMod);
		}

		return result;
	}

	UInt32 GetMaxRank(BGSMod::Attachment::Mod * thisMod)
	{
		return thisMod ? thisMod->maxRank : 0;
	}

	void SetMaxRank(BGSMod::Attachment::Mod * thisMod, UInt32 nuDamage)
	{
		if(thisMod) {
			thisMod->maxRank = max(0, min(nuDamage, 0xFF));
		}
	}

	UInt32 GetPriority(BGSMod::Attachment::Mod * thisMod)
	{
		return thisMod ? thisMod->priority : 0;
	}

	void SetPriority(BGSMod::Attachment::Mod * thisMod, UInt32 nuDamage)
	{
		if(thisMod) {
			thisMod->priority = max(0, min(nuDamage, 0xFF));
		}
	}

	TESObjectMISC * GetLooseMod(BGSMod::Attachment::Mod * thisMod)
	{
		auto pair = g_modAttachmentMap->Find(&thisMod);
		if(pair) {
			return pair->miscObject;
		}
		
		return nullptr;
	}
}

void papyrusObjectMod::RegisterFuncs(VirtualMachine* vm)
{
	vm->RegisterFunction(
		new NativeFunction0 <BGSMod::Attachment::Mod, VMArray<PropertyModifier>>("GetPropertyModifiers", "ObjectMod", papyrusObjectMod::GetPropertyModifiers, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSMod::Attachment::Mod, UInt32>("GetMaxRank", "ObjectMod", papyrusObjectMod::GetMaxRank, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSMod::Attachment::Mod, void, UInt32>("SetMaxRank", "ObjectMod", papyrusObjectMod::SetMaxRank, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSMod::Attachment::Mod, UInt32>("GetPriority", "ObjectMod", papyrusObjectMod::GetPriority, vm));

	vm->RegisterFunction(
		new NativeFunction1 <BGSMod::Attachment::Mod, void, UInt32>("SetPriority", "ObjectMod", papyrusObjectMod::SetPriority, vm));

	vm->RegisterFunction(
		new NativeFunction0 <BGSMod::Attachment::Mod, TESObjectMISC *>("GetLooseMod", "ObjectMod", papyrusObjectMod::GetLooseMod, vm));
}
