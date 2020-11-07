struct MiscComponent
	Component object
	int count
endStruct

; Do not use these functions on ConstructibleObject they will not work.
; Papyrus defines Constructible as extends MiscObject when this is not
; remotely true internally and F4SE does not support this kind of conversion
; in code so these are separate functions

MiscComponent[] Function GetMiscComponents() native

Function SetMiscComponents(MiscComponent[] components) native
