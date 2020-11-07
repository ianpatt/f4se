Scriptname MatSwap extends Form native Hidden

struct RemapData
	string source
	string target
	float colorIndex
endStruct

RemapData[] Function GetRemapData() native

Function SetRemapData(RemapData[] data) native