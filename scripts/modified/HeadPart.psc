int Property Type_Misc = 0 AutoReadOnly
int Property Type_Face = 1 AutoReadOnly
int Property Type_Eyes = 2 AutoReadOnly
int Property Type_Hair = 3 AutoReadOnly
int Property Type_FacialHair = 4 AutoReadOnly
int Property Type_Scar = 5 AutoReadOnly
int Property Type_Brows = 6 AutoReadOnly
int Property Type_HeadRear = 9 AutoReadOnly

; Returns the head part type
int Function GetType() native

; Returns an array of all the extra parts to this part
HeadPart[] Function GetExtraParts() native

; Returns whether this head part has this extra part
bool Function HasExtraPart(HeadPart p) native

; Returns whether the head part is an extra part
bool Function IsExtraPart() native

; Returns a formlist of the valid races for this head part
FormList Function GetValidRaces() native
Function SetValidRaces(FormList vRaces) native

