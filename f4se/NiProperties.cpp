#include "f4se/NiProperties.h"

BSEffectShaderProperty * BSEffectShaderProperty::Create()
{
	BSEffectShaderProperty * prop = (BSEffectShaderProperty *)Heap_Allocate(sizeof(BSEffectShaderProperty));
	CALL_MEMBER_FN(prop, ctor)();
	return prop;
}

BSLightingShaderProperty * BSLightingShaderProperty::Create()
{
	BSLightingShaderProperty * prop = (BSLightingShaderProperty *)Heap_Allocate(sizeof(BSLightingShaderProperty));
	CALL_MEMBER_FN(prop, ctor)();
	return prop;
}
