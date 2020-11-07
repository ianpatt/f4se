#include "Hooks_Papyrus.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusEvents.h"

#include "f4se/PapyrusF4SE.h"
#include "f4se/PapyrusForm.h"
#include "f4se/PapyrusMath.h"
#include "f4se/PapyrusActor.h"
#include "f4se/PapyrusActorBase.h"
#include "f4se/PapyrusHeadPart.h"
#include "f4se/PapyrusObjectReference.h"
#include "f4se/PapyrusGame.h"
#include "f4se/PapyrusScriptObject.h"
#include "f4se/PapyrusDelayFunctors.h"
#include "f4se/PapyrusUtility.h"
#include "f4se/PapyrusUI.h"
#include "f4se/PapyrusEquipSlot.h"
#include "f4se/PapyrusWeapon.h"
#include "f4se/PapyrusObjectMod.h"
#include "f4se/PapyrusInstanceData.h"
#include "f4se/PapyrusWaterType.h"
#include "f4se/PapyrusCell.h"
#include "f4se/PapyrusPerk.h"
#include "f4se/PapyrusLocation.h"
#include "f4se/PapyrusEncounterZone.h"
#include "f4se/PapyrusInput.h"
#include "f4se/PapyrusDefaultObject.h"
#include "f4se/PapyrusConstructibleObject.h"
#include "f4se/PapyrusComponent.h"
#include "f4se/PapyrusMiscObject.h"
#include "f4se/PapyrusMaterialSwap.h"
#include "f4se/PapyrusFavoritesManager.h"
#include "f4se/PapyrusArmorAddon.h"
#include "f4se/PapyrusArmor.h"

#include "f4se/Serialization.h"

#include "xbyak/xbyak.h"

RelocAddr <uintptr_t> RegisterPapyrusFunctions_Start(0x013E6BC0 + 0x461);
RelocAddr <uintptr_t> DelayFunctorQueue_Start(0x01377CD0 + 0x6A);

typedef bool (* _SaveRegistrationHandles)(void * unk1, void * vm, void * handleReaderWriter, void * saveStorageWrapper);
RelocAddr <_SaveRegistrationHandles> SaveRegistrationHandles(0x01474AD0);
_SaveRegistrationHandles SaveRegistrationHandles_Original = nullptr;

typedef bool (* _LoadRegistrationHandles)(void * unk1, void * vm, void * handleReaderWriter, UInt16 version, void * loadStorageWrapper, void * unk2);
RelocAddr <_LoadRegistrationHandles> LoadRegistrationHandles(0x01474B60);
_LoadRegistrationHandles LoadRegistrationHandles_Original = nullptr;

typedef void (* _RevertGlobalData)(void * vm);
RelocAddr <_RevertGlobalData> RevertGlobalData(0x013762F0);
_RevertGlobalData RevertGlobalData_Original = nullptr;

typedef std::list <F4SEPapyrusInterface::RegisterFunctions> PapyrusPluginList;
static PapyrusPluginList s_pap_plugins;

bool RegisterPapyrusPlugin(F4SEPapyrusInterface::RegisterFunctions callback)
{
	s_pap_plugins.push_back(callback);
	return true;
}

void GetExternalEventRegistrations(const char * eventName, void * data, F4SEPapyrusInterface::RegistrantFunctor functor)
{
	g_externalEventRegs.ForEach(BSFixedString(eventName), [&functor, &data](const EventRegistration<ExternalEventParameters> & reg)
	{
		functor(reg.handle, reg.scriptName.c_str(), reg.params.callbackName.c_str(), data);
	});
}

void Hooks_Papyrus_Init()
{

}

void RegisterPapyrusFunctions_Hook(VirtualMachine ** vmPtr)
{
	_MESSAGE("RegisterPapyrusFunctions_Hook");

	VirtualMachine * vm = (*vmPtr);

	// ScriptObject
	papyrusScriptObject::RegisterFuncs(vm);

	// EquipSlot
	papyrusEquipSlot::RegisterFuncs(vm);

	// WaterType
	papyrusWaterType::RegisterFuncs(vm);

	// MaterialSwap
	papyrusMaterialSwap::RegisterFuncs(vm);

	// F4SE
	papyrusF4SE::RegisterFuncs(vm);

	// Math
	papyrusMath::RegisterFuncs(vm);

	// Form
	papyrusForm::RegisterFuncs(vm);

	// ObjectReference
	papyrusObjectReference::RegisterFuncs(vm);

	// ActorBase
	papyrusActorBase::RegisterFuncs(vm);

	// Actor
	papyrusActor::RegisterFuncs(vm);

	// HeadPart
	papyrusHeadPart::RegisterFuncs(vm);

	// Game
	papyrusGame::RegisterFuncs(vm);

	// Utility
	papyrusUtility::RegisterFuncs(vm);

	// UI
	papyrusUI::RegisterFuncs(vm);

	// Weapon
	papyrusWeapon::RegisterFuncs(vm);

	// ObjectMod
	papyrusObjectMod::RegisterFuncs(vm);

	// InstanceData
	papyrusInstanceData::RegisterFuncs(vm);

	// Cell
	papyrusCell::RegisterFuncs(vm);

	// Perk
	papyrusPerk::RegisterFuncs(vm);

	// EncounterZone
	papyrusEncounterZone::RegisterFuncs(vm);

	// Location
	papyrusLocation::RegisterFuncs(vm);

	// Input
	papyrusInput::RegisterFuncs(vm);

	// DefaultObject
	papyrusDefaultObject::RegisterFuncs(vm);

	// ConstructibleObject
	papyrusConstructibleObject::RegisterFuncs(vm);

	// Component
	papyrusComponent::RegisterFuncs(vm);

	// MiscObject
	papyrusMiscObject::RegisterFuncs(vm);

	// FavoritesManager
	papyrusFavoritesManager::RegisterFuncs(vm);

	// ArmorAddon
	papyrusArmorAddon::RegisterFuncs(vm);

	// Armor
	papyrusArmor::RegisterFuncs(vm);

	GetEventDispatcher<TESFurnitureEvent>()->AddEventSink(&g_furnitureEventSink);

	// Plugins
	for(PapyrusPluginList::iterator iter = s_pap_plugins.begin(); iter != s_pap_plugins.end(); ++iter)
	{
		(*iter)(vm);
	}
}

bool SaveRegistrationHandles_Hook(void * unk1, void * vm, void * handleReaderWriter, void * loadStorageWrapper)
{
	bool ret = SaveRegistrationHandles_Original(unk1, vm, handleReaderWriter, loadStorageWrapper);
	Serialization::HandleSaveGlobalData();
	return ret;
}

bool LoadRegistrationHandles_Hook(void * unk1, void * vm, void * handleReaderWriter, UInt16 version, void * loadStorageWrapper, void * unk2)
{
	bool ret = LoadRegistrationHandles_Original(unk1, vm, handleReaderWriter, version, loadStorageWrapper, unk2);
	Serialization::HandleLoadGlobalData();
	return ret;
}

void RevertGlobalData_Hook(void * vm)
{
	RevertGlobalData_Original(vm);
	Serialization::HandleRevertGlobalData();
}

LONGLONG DelayFunctorQueue_Hook(float budget)
{
	F4SEDelayFunctorManagerInstance().OnPreTick();

	LARGE_INTEGER startTime;
	QueryPerformanceCounter(&startTime);

	// Sharing budget with papyrus queue
	F4SEDelayFunctorManagerInstance().OnTick(startTime.QuadPart, budget);

	return startTime.QuadPart;
}

void Hooks_Papyrus_Commit()
{
	struct RegisterPapyrusFunctions_Code : Xbyak::CodeGenerator {
		RegisterPapyrusFunctions_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
		{
			call((void *)&RegisterPapyrusFunctions_Hook);
			mov(rcx, rbx);
			add(rsp, 0x20);
			pop(rbx);
			ret();
		}
	};

	void * codeBuf = g_localTrampoline.StartAlloc();
	RegisterPapyrusFunctions_Code code(codeBuf);
	g_localTrampoline.EndAlloc(code.getCurr());

	g_branchTrampoline.Write6Branch(RegisterPapyrusFunctions_Start, uintptr_t(code.getCode()));

	// save registration handles hook
	{
		struct SaveRegistrationHandles_Code : Xbyak::CodeGenerator {
			SaveRegistrationHandles_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr [rsp+0x10], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(SaveRegistrationHandles.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		SaveRegistrationHandles_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		SaveRegistrationHandles_Original = (_SaveRegistrationHandles)codeBuf;

		g_branchTrampoline.Write5Branch(SaveRegistrationHandles.GetUIntPtr(), (uintptr_t)SaveRegistrationHandles_Hook);
	}


	// load registration handles hook
	{
		struct LoadRegistrationHandles_Code : Xbyak::CodeGenerator {
			LoadRegistrationHandles_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr [rsp+0x10], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(LoadRegistrationHandles.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		LoadRegistrationHandles_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		LoadRegistrationHandles_Original = (_LoadRegistrationHandles)codeBuf;

		g_branchTrampoline.Write5Branch(LoadRegistrationHandles.GetUIntPtr(), (uintptr_t)LoadRegistrationHandles_Hook);
	}
	
	// revert global data hook
	{
		struct RevertGlobalData_Code : Xbyak::CodeGenerator {
			RevertGlobalData_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				push(rdi);
				push(r15);
				sub(rsp, 0x38);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(RevertGlobalData.GetUIntPtr() + 8);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		RevertGlobalData_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		RevertGlobalData_Original = (_RevertGlobalData)codeBuf;

		g_branchTrampoline.Write6Branch(RevertGlobalData.GetUIntPtr(), (uintptr_t)RevertGlobalData_Hook);
	}

	// hook ProcessVMTick
	{
		struct DelayFunctorQueue_Code : Xbyak::CodeGenerator {
			DelayFunctorQueue_Code(void * buf, uintptr_t funcAddr) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label funcLabel;
				Xbyak::Label retnLabel;

				movss(xmm0, ptr[rsp + 0x98 + 0x28]);
				call(ptr [rip + funcLabel]);
				jmp(ptr [rip + retnLabel]);

				L(funcLabel);
				dq(funcAddr);

				L(retnLabel);
				dq(DelayFunctorQueue_Start.GetUIntPtr() + 0x5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		DelayFunctorQueue_Code code(codeBuf, (uintptr_t)DelayFunctorQueue_Hook);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Branch(DelayFunctorQueue_Start.GetUIntPtr(), uintptr_t(code.getCode()));
	}
}
