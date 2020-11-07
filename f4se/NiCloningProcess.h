#pragma once

#include "f4se/NiObjects.h"
#include "f4se/NiTypes.h"

// 68
class NiCloningProcess
{
public:
	tHashSet<void*,void*>	unk00;	// 00
	void		* unk30;	// 30
	UInt64		unk38;		// 38
	UInt64		unk40;		// 40
	void		* unk48;	// 48 - DEADBEEF
	void		* unk50;	// 50 - bucket
	UInt64		unk58;		// 58
	UInt32		unk60;		// 60 - copytype? 0, 1, 2
	UInt32		unk64;		// 64
};
