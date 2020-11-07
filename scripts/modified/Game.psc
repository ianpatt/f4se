; Returns the current console ref
ObjectReference Function GetCurrentConsoleRef() native global

; Best not to store this struct anywhere
struct PluginInfo
	int index
	string name
	string author
	string description
EndStruct

PluginInfo[] Function GetInstalledPlugins() native global

PluginInfo[] Function GetInstalledLightPlugins() native global

; Returns the mod dependencies of the specified plugin
string[] Function GetPluginDependencies(string plugin) native global

Function SetGameSettingFloat(string setting, float value) global native
Function SetGameSettingInt(string setting, int value) global native
Function SetGameSettingBool(string setting, bool value) global native
Function SetGameSettingString(string setting, string value) global native

; refreshes the third person camera state
Function UpdateThirdPerson() native global

; returns the current camera state
; -1 - unknown/there is no camera yet
; 0 - first person
; 1 - auto vanity
; 2 - VATS
; 3 - free
; 4 - iron sights
; 5 - transition
; 6 - tween menu
; 7 - third person 1
; 8 - third person 2
; 9 - furniture
; 10 - horse
; 11 - bleedout
; 12 - dialogue
int Function GetCameraState() native global