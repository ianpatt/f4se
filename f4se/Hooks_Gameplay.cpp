#include "Hooks_Gameplay.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "f4se_common/SafeWrite.h"
#include "f4se_common/f4se_version.h"
#include "f4se_common/Utilities.h"
#include "common/IFileStream.h"
#include "xbyak/xbyak.h"

// DDB041F109C6211C69F6E5F20EEC28E4448B796B+53
RelocAddr <char *> g_gameVersion(0x02C5D6F8);

RelocAddr <uintptr_t> kHook_ShowVersion_Offset(0x00BBC4B0 + 0x53 + 3);
RelocAddr <uintptr_t> kHook_ShowVersion_OffsetBase(0x00BBC4B0 + 0x5A);

static char s_patchedGameVersion[256];

static void InitVersionHook()
{
	char * originalGameVersion = g_gameVersion;

	sprintf_s(s_patchedGameVersion, sizeof(s_patchedGameVersion), "(F4SE %d.%d.%d rel %d) %s",
		F4SE_VERSION_INTEGER, F4SE_VERSION_INTEGER_MINOR, F4SE_VERSION_INTEGER_BETA, F4SE_VERSION_RELEASEIDX, originalGameVersion);
}

static void CommitVersionHook()
{
	size_t bufLen = strlen(s_patchedGameVersion) + 1;

	char * versionBuf = (char *)g_branchTrampoline.Allocate(bufLen);
	memcpy(versionBuf, s_patchedGameVersion, bufLen);

	SafeWrite32(kHook_ShowVersion_Offset, uintptr_t(versionBuf) - kHook_ShowVersion_OffsetBase);
}

static char * s_customControlMap = nullptr;

RelocAddr <uintptr_t> kHook_CustomControlMap_Offset(0x01B2A1D0);
RelocAddr <void *> kHook_CustomControlMap_Return(0x01B2A1D0 + 5);

static void InitControlMap()
{
	std::string controlMapPath = GetRuntimeDirectory() + "Data\\F4SE\\CustomControlMap.txt";

	IFileStream src;
	if(src.Open(controlMapPath.c_str()))
	{
		_MESSAGE("using custom control map: %s", controlMapPath.c_str());

		s_customControlMap = new char[src.GetLength() + 1];
		src.ReadBuf(s_customControlMap, src.GetLength());
		s_customControlMap[src.GetLength()] = 0;
	}
}

static void CommitControlMap()
{
	if(s_customControlMap)
	{
		struct CustomControlMap_Code : Xbyak::CodeGenerator {
			CustomControlMap_Code(void * buf, char * controlMapPtr) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label addrLabel;

				mov(ptr [rsp + 0x10], rbx);
				mov(rdx, ptr [rip + addrLabel]);
				jmp(kHook_CustomControlMap_Return);

				L(addrLabel);
				dq(uintptr_t(controlMapPtr));
			}
		};

		void * codeBuf = g_branchTrampoline.StartAlloc();
		CustomControlMap_Code code(codeBuf, s_customControlMap);
		g_branchTrampoline.EndAlloc(code.getCurr());

		ASSERT(SafeWriteJump(kHook_CustomControlMap_Offset, uintptr_t(codeBuf)));
	}
}

void Hooks_Gameplay_Init()
{
	InitVersionHook();
	InitControlMap();
}

void Hooks_Gameplay_Commit()
{
	CommitVersionHook();
	CommitControlMap();
}
