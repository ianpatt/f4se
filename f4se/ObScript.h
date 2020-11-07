#pragma once

#include "f4se_common/Utilities.h"

class TESObjectREFR;

// 0C
struct ObScriptParam
{
	const char	* typeStr;	// 00
	UInt32		typeID;		// 04
	UInt32		isOptional;	// 08
};

typedef bool (* ObScript_Execute)(void * paramInfo, void * scriptData, TESObjectREFR * thisObj, void * containingObj, void * scriptObj, void * locals, double * result, void * opcodeOffsetPtr);

// 50
struct ObScriptCommand
{
	const char			* longName;		// 00
	const char			* shortName;	// 08
	UInt32				opcode;			// 10
	UInt32				pad14;			// 14
	const char			* helpText;		// 18
	UInt8				needsParent;	// 20
	UInt8				pad21;			// 21
	UInt16				numParams;		// 22
	UInt32				pad24;			// 24
	ObScriptParam		* params;		// 28

	// handlers
	ObScript_Execute	execute;		// 30
	void				* parse;		// 38
	void				* eval;			// 40

	UInt32				flags;			// 48
	UInt32				pad4C;			// 4C
};

enum
{
	kObScript_NumObScriptCommands =	0x0332,
	kObScript_NumConsoleCommands =	0x0209,

	kObScript_ScriptOpBase =		0x1000,
	kObScript_ConsoleOpBase =		0x0100,
};

extern RelocPtr <ObScriptCommand>	g_firstObScriptCommand;
extern RelocPtr <ObScriptCommand>	g_firstConsoleCommand;
