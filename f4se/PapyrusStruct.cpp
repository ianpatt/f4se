#include "f4se/PapyrusStruct.h"

bool CreateStruct(VMValue * dst, BSFixedString * structName, VirtualMachine * vm, bool bNone)
{
	dst->SetNone();

	VMValue::StructData * structData = nullptr;
	VMStructTypeInfo * typeInfo = nullptr;
	if(vm->GetStructTypeInfo(structName, &typeInfo)) {
		dst->type.id = typeInfo; // Always set the type info if its valid
		if(!bNone) {
			vm->CreateStruct(structName, &structData);
			if(structData) {
				dst->data.strct = structData;
			}
		}

		typeInfo->Release();
		return structData ? true : false;
	}

	return false;
}
