
; Returns whether this perk is playable
bool Function IsPlayable() native

; Returns whether this perk is hidden
bool Function IsHidden() native

; Returns the required level for this perk
int Function GetLevel() native

; Returns the number of ranks
int Function GetNumRanks() native

; Returns the next perk, this function will loop
; should only do it NumRanks times
Perk Function GetNextPerk() native

; Returns the SWF path
string Function GetSWFPath() native

; Returns true if all the Perk conditions are met
; Returns true if there are no Perk conditions
; This does not evaluate the Level check or Entry Points
bool Function IsEligible(Actor akActor) native