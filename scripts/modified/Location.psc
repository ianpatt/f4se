
; Returns the parent location
Location Function GetParent() native

Function SetParent(Location akLocation) native

; Returns the locations EncounterZone, if recursive it goes up a parent until it finds an EncounterZone
EncounterZone Function GetEncounterZone(bool recursive = false) native

; Sets this locations encounter zone
Function SetEncounterZone(EncounterZone ez) native