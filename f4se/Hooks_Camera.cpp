#include "Hooks_Camera.h"

#include "xbyak/xbyak.h"
#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/Relocation.h"

#include "f4se/PapyrusEvents.h"
#include "f4se/GameCamera.h"

void Hooks_Camera_Init()
{

}

typedef void (* _SetCameraState)(TESCamera * camera, TESCameraState * newState);
RelocAddr <_SetCameraState> SetCameraState(0x0082E930);
_SetCameraState SetCameraState_Original = nullptr;

void SetCameraState_Hook(TESCamera * camera, TESCameraState * newCameraState)
{
	if(camera == (*g_playerCamera))
	{
		SInt32 oldState = (*g_playerCamera)->GetCameraStateId(camera->cameraState);
		SInt32 newState = (*g_playerCamera)->GetCameraStateId(newCameraState);

		g_cameraEventRegs.ForEach(
			[&oldState, &newState](const EventRegistration<NullParameters> & reg)
		{
			SendPapyrusEvent2<SInt32, SInt32>(reg.handle, reg.scriptName, "OnPlayerCameraState", oldState, newState);
		}
		);
	}

	SetCameraState_Original(camera, newCameraState);
}

void Hooks_Camera_Commit()
{
	// hook global tinting of objects
	{
		struct SetCameraState_Code : Xbyak::CodeGenerator {
			SetCameraState_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp+0x08], rbx);
				push(rdi);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(SetCameraState.GetUIntPtr() + 6);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		SetCameraState_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		SetCameraState_Original = (_SetCameraState)codeBuf;

		g_branchTrampoline.Write6Branch(SetCameraState.GetUIntPtr(), (uintptr_t)SetCameraState_Hook);
	}
}
