#include "IdentifyEXE.h"
#include "LoaderError.h"
#include "f4se_common/f4se_version.h"
#include <string>

static bool GetFileVersion(const char * path, VS_FIXEDFILEINFO * info, std::string * outProductName)
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

	_MESSAGE("dwSignature = %08X", versionInfo.dwSignature);
	_MESSAGE("dwStrucVersion = %08X", versionInfo.dwStrucVersion);
	_MESSAGE("dwFileVersionMS = %08X", versionInfo.dwFileVersionMS);
	_MESSAGE("dwFileVersionLS = %08X", versionInfo.dwFileVersionLS);
	_MESSAGE("dwProductVersionMS = %08X", versionInfo.dwProductVersionMS);
	_MESSAGE("dwProductVersionLS = %08X", versionInfo.dwProductVersionLS);
	_MESSAGE("dwFileFlagsMask = %08X", versionInfo.dwFileFlagsMask);
	_MESSAGE("dwFileFlags = %08X", versionInfo.dwFileFlags);
	_MESSAGE("dwFileOS = %08X", versionInfo.dwFileOS);
	_MESSAGE("dwFileType = %08X", versionInfo.dwFileType);
	_MESSAGE("dwFileSubtype = %08X", versionInfo.dwFileSubtype);
	_MESSAGE("dwFileDateMS = %08X", versionInfo.dwFileDateMS);
	_MESSAGE("dwFileDateLS = %08X", versionInfo.dwFileDateLS);

	UInt64 version = (((UInt64)versionInfo.dwFileVersionMS) << 32) | versionInfo.dwFileVersionLS;

	*out = version;

	return true;
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

// steam EXE will have the .bind section
bool IsSteamImage(const UInt8 * base)
{
	return GetImageSection(base, ".bind") != NULL;
}

bool IsUPXImage(const UInt8 * base)
{
	return GetImageSection(base, "UPX0") != NULL;
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
			bool	isSteam = IsSteamImage(fileBase);
			bool	isUPX = IsUPXImage(fileBase);

			if(isUPX)
			{
				hookInfo->procType = kProcType_Packed;
			}
			else if(isSteam)
			{
				hookInfo->procType = kProcType_Steam;
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
	case kProcType_Steam:	_MESSAGE("steam exe"); break;
	case kProcType_Normal:	_MESSAGE("normal exe"); break;
	case kProcType_Packed:	_MESSAGE("packed exe"); break;
	case kProcType_Unknown:
	default:				_MESSAGE("unknown exe type"); break;
	}

	bool result = false;

	const UInt64 kCurVersion = 0x0001000A00A30000;	// 1.10.163.0

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
		switch(hookInfo->procType)
		{
		case kProcType_Steam:
		case kProcType_Normal:
			*dllSuffix = "1_10_163";

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
