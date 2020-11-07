// Native

#define CLASS_NAME __MACRO_JOIN__(NativeFunction, NUM_PARAMS)

#define VOID_SPEC 0
#include "PapyrusNativeFunctionDef_Base.inl"

#define VOID_SPEC 1
#include "PapyrusNativeFunctionDef_Base.inl"

#undef CLASS_NAME

// Latent native

#define CLASS_NAME __MACRO_JOIN__(LatentNativeFunction, NUM_PARAMS)
#define LATENT_SPEC 1

#define VOID_SPEC 0
#include "PapyrusNativeFunctionDef_Base.inl"

#define VOID_SPEC 1
#include "PapyrusNativeFunctionDef_Base.inl"

#undef LATENT_SPEC
#undef CLASS_NAME

#undef NUM_PARAMS
