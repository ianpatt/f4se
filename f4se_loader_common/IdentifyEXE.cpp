#include "IdentifyEXE.h"
#include "LoaderError.h"
#include "f4se_common/f4se_version.h"
#include <string>

bool GetFileVersion(const char * path, VS_FIXEDFILEINFO * info, std::string * outProductName)
{
	bool result = false;

	UInt32	versionSize = GetFileVersionInfoSize(path, NULL);
	if(!versionSize)
	{
		_ERROR("GetFileVersionInfoSize failed (%08X)", GetLastError());
		return false;
	}

	UInt8	* versionBuf = new UInt8[versionSize];
	if(versionBuf)
	{
		if(GetFileVersionInfo(path, NULL, versionSize, versionBuf))
		{
			VS_FIXEDFILEINFO	* retrievedInfo = NULL;
			UInt32				realVersionSize = sizeof(VS_FIXEDFILEINFO);

			if(VerQueryValue(versionBuf, "\\", (void **)&retrievedInfo, (PUINT)&realVersionSize) && retrievedInfo)
			{
				*info = *retrievedInfo;
				result = true;
			}
			else
			{
				_ERROR("VerQueryValue failed (%08X)", GetLastError());
			}

			if(outProductName)
			{
				// try to get the product name, failure is ok
				char * productName = NULL;
				UInt32 productNameLen = 0;
				if(VerQueryValue(versionBuf, "\\StringFileInfo\\040904B0\\ProductName", (void **)&productName, (PUINT)&productNameLen) && productNameLen && productName)
				{
					*outProductName = productName;
				}
			}
		}
		else
		{
			_ERROR("GetFileVersionInfo failed (%08X)", GetLastError());
		}

		delete [] versionBuf;
	}

	return result;
}

static bool GetFileVersionData(const char * path, UInt64 * out, std::string * outProductName)
{
	VS_FIXEDFILEINFO	versionInfo;
	if(!GetFileVersion(path, &versionInfo, outProductName))
	{
		return false;
	}

	DumpVersionInfo(versionInfo);

	UInt64 version = (((UInt64)versionInfo.dwFileVersionMS) << 32) | versionInfo.dwFileVersionLS;

	*out = version;

	return true;
}

void DumpVersionInfo(const VS_FIXEDFILEINFO & info)
{
	_MESSAGE("dwSignature = %08X", info.dwSignature);
	_MESSAGE("dwStrucVersion = %08X", info.dwStrucVersion);
	_MESSAGE("dwFileVersionMS = %08X", info.dwFileVersionMS);
	_MESSAGE("dwFileVersionLS = %08X", info.dwFileVersionLS);
	_MESSAGE("dwProductVersionMS = %08X", info.dwProductVersionMS);
	_MESSAGE("dwProductVersionLS = %08X", info.dwProductVersionLS);
	_MESSAGE("dwFileFlagsMask = %08X", info.dwFileFlagsMask);
	_MESSAGE("dwFileFlags = %08X", info.dwFileFlags);
	_MESSAGE("dwFileOS = %08X", info.dwFileOS);
	_MESSAGE("dwFileType = %08X", info.dwFileType);
	_MESSAGE("dwFileSubtype = %08X", info.dwFileSubtype);
	_MESSAGE("dwFileDateMS = %08X", info.dwFileDateMS);
	_MESSAGE("dwFileDateLS = %08X", info.dwFileDateLS);
}

const IMAGE_SECTION_HEADER * GetImageSection(const UInt8 * base, const char * name)
{
	const IMAGE_DOS_HEADER		* dosHeader = (IMAGE_DOS_HEADER *)base;
	const IMAGE_NT_HEADERS		* ntHeader = (IMAGE_NT_HEADERS *)(base + dosHeader->e_lfanew);
	const IMAGE_SECTION_HEADER	* sectionHeader = IMAGE_FIRST_SECTION(ntHeader);

	for(UInt32 i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
	{
		const IMAGE_SECTION_HEADER	* section = &sectionHeader[i];

		if(!strcmp((const char *)section->Name, name))
		{
			return section;
		}
	}

	return NULL;
}

// non-relocated image
bool HasImportedLibrary(const UInt8 * base, const char * name)
{
	auto * dosHeader = (const IMAGE_DOS_HEADER *)base;
	auto * ntHeader = (const IMAGE_NT_HEADERS *)(base + dosHeader->e_lfanew);
	auto * importDir = (const IMAGE_DATA_DIRECTORY *)&ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if(!importDir->Size || !importDir->VirtualAddress) return false;

	// resolve RVA -> file offset
	const auto * sectionHeader = IMAGE_FIRST_SECTION(ntHeader);

	auto LookupRVA = [ntHeader, sectionHeader, base](UInt32 rva) -> const UInt8 *
	{
		for(UInt32 i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
		{
			const auto * section = &sectionHeader[i];

			if(	(rva >= section->VirtualAddress) &&
				(rva < section->VirtualAddress + section->SizeOfRawData))
			{
				return base + rva - section->VirtualAddress + section->PointerToRawData;
			}
		}

		return nullptr;
	};

	if(const auto * importTable = (const IMAGE_IMPORT_DESCRIPTOR *)LookupRVA(importDir->VirtualAddress))
	{
		for(; importTable->Characteristics; ++importTable)
		{
			auto * dllName = (const char *)LookupRVA(importTable->Name);

			if(dllName && !_stricmp(dllName, name))
			{
				return true;
			}
		}
	}

	return false;
}

// steam EXE will have the .bind section
bool IsSteamImage(const UInt8 * base)
{
	return GetImageSection(base, ".bind") != NULL;
}

bool IsUPXImage(const UInt8 * base)
{
	return GetImageSection(base, "UPX0") != NULL;
}

static bool IsWinStoreImage(const UInt8 * base)
{
	// haven't seen this either, but if it imports this then it's a unique build
	return HasImportedLibrary(base, "api-ms-win-core-psm-appnotify-l1-1-0.dll");
}

static bool IsGOGImage(const UInt8 * base)
{
	// haven't seen it but they may have a unique build for achievements
	return HasImportedLibrary(base, "Galaxy64.dll");
}

static bool IsEpicImage(const UInt8 * base)
{
	// haven't seen it but let's assume they have a unique build for achievements
	return HasImportedLibrary(base, "eossdk-win64-shipping.dll");
}

bool ScanEXE(const char * path, ProcHookInfo * hookInfo)
{
	// open and map the file in to memory
	HANDLE	file = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(file == INVALID_HANDLE_VALUE)
	{
		_ERROR("ScanEXE: couldn't open file (%d)", GetLastError());
		return false;
	}

	bool result = false;

	HANDLE	mapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
	if(mapping)
	{
		const UInt8	* fileBase = (const UInt8 *)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
		if(fileBase)
		{
			// scan for packing type
			if(IsUPXImage(fileBase))
			{
				hookInfo->procType = kProcType_Packed;
			}
			else if(IsSteamImage(fileBase))
			{
				hookInfo->procType = kProcType_Steam;
			}
			else if(IsWinStoreImage(fileBase))
			{
				hookInfo->procType = kProcType_WinStore;
			}
			else if(IsGOGImage(fileBase))
			{
				hookInfo->procType = kProcType_GOG;
			}
			else if(IsEpicImage(fileBase))
			{
				hookInfo->procType = kProcType_Epic;
			}
			else
			{
				hookInfo->procType = kProcType_Normal;
			}

			result = true;

			UnmapViewOfFile(fileBase);
		}
		else
		{
			_ERROR("ScanEXE: couldn't map file (%d)", GetLastError());
		}

		CloseHandle(mapping);
	}
	else
	{
		_ERROR("ScanEXE: couldn't create file mapping (%d)", GetLastError());
	}

	CloseHandle(file);

	return result;
}

bool IdentifyEXE(const char * procName, bool isEditor, std::string * dllSuffix, ProcHookInfo * hookInfo)
{
	UInt64		version;
	std::string	productName;

	// check file version
	if(!GetFileVersionData(procName, &version, &productName))
	{
		PrintLoaderError("Couldn't retrieve EXE version information.");
		return false;
	}

	_MESSAGE("version = %016I64X", version);
	_MESSAGE("product name = %s", productName.c_str());

	hookInfo->version = version;
	hookInfo->packedVersion = MAKE_EXE_VERSION(version >> 48, version >> 32, version >> 16);

	if(productName == "F4SE")
	{
		_MESSAGE("found an F4SE component");
		return false;
	}

	if(productName == "Fallout 4 Launcher")
	{
		PrintLoaderError("You have instructed f4se_loader to run the vanilla Fallout 4 launcher, which cannot work. Most likely you have renamed files incorrectly.");
		return false;
	}

	// check protection type
	if(!ScanEXE(procName, hookInfo))
	{
		PrintLoaderError("Failed to identify EXE type.");
		return false;
	}

	switch(hookInfo->procType)
	{
	case kProcType_Steam:		_MESSAGE("steam exe"); break;
	case kProcType_Normal:		_MESSAGE("normal exe"); break;
	case kProcType_Packed:		_MESSAGE("packed exe"); break;
	case kProcType_WinStore:	_MESSAGE("winstore exe"); break;
	case kProcType_GOG:			_MESSAGE("gog exe"); break;
	case kProcType_Epic:		_MESSAGE("epic exe"); break;
	case kProcType_Unknown:
	default:					_MESSAGE("unknown exe type"); break;
	}

	if(hookInfo->procType == kProcType_WinStore)
	{
		PrintLoaderError("The Windows Store (gamepass) version of Fallout 4 is not supported.");
		return false;
	}

	if(hookInfo->procType == kProcType_Epic)
	{
		PrintLoaderError("The Epic Store version of Fallout 4 is not supported.");
		return false;
	}

	bool result = false;

	const UInt64 kCurVersion = 0x0001000B00890000;	// 1.11.137.0

	// convert version resource to internal version format
	UInt32 versionInternal = MAKE_EXE_VERSION(version >> 48, version >> 32, version >> 16);

	if(version < kCurVersion)
	{
#if F4SE_TARGETING_BETA_VERSION
		if(versionInternal == CURRENT_RELEASE_RUNTIME)
			PrintLoaderError(
				"You are using the version of F4SE intended for the Steam beta branch (%d.%d.%d).\n"
				"Download and install the non-beta branch version (%s) from http://f4se.silverlock.org/.",
				F4SE_VERSION_INTEGER, F4SE_VERSION_INTEGER_MINOR, F4SE_VERSION_INTEGER_BETA, CURRENT_RELEASE_F4SE_STR);
		else
			PrintLoaderError(
				"You are using Fallout version %d.%d.%d, which is out of date and incompatible with this version of F4SE. Update to the latest beta version.",
				GET_EXE_VERSION_MAJOR(versionInternal), GET_EXE_VERSION_MINOR(versionInternal), GET_EXE_VERSION_BUILD(versionInternal));
#else
		PrintLoaderError(
			"You are using Fallout version %d.%d.%d, which is out of date and incompatible with this version of F4SE. Update to the latest version.",
			GET_EXE_VERSION_MAJOR(versionInternal), GET_EXE_VERSION_MINOR(versionInternal), GET_EXE_VERSION_BUILD(versionInternal));
#endif
	}
	else if(version > kCurVersion)
	{
		PrintLoaderError(
			"You are using a newer version of Fallout than this version of F4SE supports.\n"
			"If this version just came out, please be patient while we update our code.\n"
			"In the meantime, please check http://f4se.silverlock.org for updates.\n"
			"Runtime: %d.%d.%d\n"
			"F4SE: %d.%d.%d",
			GET_EXE_VERSION_MAJOR(versionInternal), GET_EXE_VERSION_MINOR(versionInternal), GET_EXE_VERSION_BUILD(versionInternal),
			F4SE_VERSION_INTEGER, F4SE_VERSION_INTEGER_MINOR, F4SE_VERSION_INTEGER_BETA);
	}
	else if(isEditor)
	{
		switch(hookInfo->procType)
		{
		case kProcType_Steam:
		case kProcType_Normal:
			*dllSuffix = "";

			result = true;

			break;
		case kProcType_Unknown:
		default:
			PrintLoaderError("Unsupported editor executable type.");
			break;
		}
	}
	else
	{
		char name[128];
		sprintf_s(name, _countof(name), "%d_%d_%d",
			int((kCurVersion >> 48) & 0xFFFF),
			int((kCurVersion >> 32) & 0xFFFF),
			int((kCurVersion >> 16) & 0xFFFF));
		*dllSuffix = name;

		switch(hookInfo->procType)
		{
		case kProcType_Steam:
		case kProcType_Normal:
			result = true;
			break;

		case kProcType_GOG:
			*dllSuffix += "_gog";

			result = true;
			break;

		case kProcType_Packed:
			PrintLoaderError("Packed versions of Fallout are not supported.");
			break;

		case kProcType_Unknown:
		default:
			PrintLoaderError("Unknown executable type.");
			break;
		}
	}

	return result;
}
