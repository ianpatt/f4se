#pragma once

#include "f4se/PluginAPI.h"

void Hooks_Papyrus_Init();
void Hooks_Papyrus_Commit();

bool RegisterPapyrusPlugin(F4SEPapyrusInterface::RegisterFunctions);
void GetExternalEventRegistrations(const char * eventName, void * data, F4SEPapyrusInterface::RegistrantFunctor functor);
