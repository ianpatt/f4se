#pragma once

#include "f4se/PluginAPI.h"

void Hooks_Scaleform_Init();
void Hooks_Scaleform_Commit();

bool RegisterScaleformPlugin(const char * name, F4SEScaleformInterface::RegisterCallback callback);
void Hooks_OpenCloseHandler();