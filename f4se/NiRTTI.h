#pragma once

#include "f4se_common/Relocation.h"

class NiObject;

// 10
class NiRTTI
{
public:
	const char	* name;
	NiRTTI		* parent;
};

NiObject * DoNiRTTICast(NiObject * src, const NiRTTI * typeInfo);

#define ni_cast(obj, type)	(type *)DoNiRTTICast(obj, NiRTTI_##type)

extern const RelocPtr<NiRTTI>	NiRTTI_BSLightingShaderProperty;
extern const RelocPtr<NiRTTI>	NiRTTI_BSEffectShaderProperty;
extern const RelocPtr<NiRTTI>	NiRTTI_BSShaderProperty;
extern const RelocPtr<NiRTTI>	NIRTTI_NiExtraData;
