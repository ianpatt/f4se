#include "f4se/NiRTTI.h"
#include "f4se/NiObjects.h"

NiObject * DoNiRTTICast(NiObject * src, const NiRTTI * typeInfo)
{
	if(src)
		for(NiRTTI * iter = src->GetRTTI(); iter; iter = iter->parent)
			if(iter == typeInfo)
				return src;

	return nullptr;
}

const RelocPtr<NiRTTI>	NiRTTI_BSLightingShaderProperty(0x03E47390);
const RelocPtr<NiRTTI>	NiRTTI_BSEffectShaderProperty(0x03E47380);
const RelocPtr<NiRTTI>	NiRTTI_BSShaderProperty(0x03E47318);
const RelocPtr<NiRTTI>	NiRTTI_NiExtraData(0x034380C0);
