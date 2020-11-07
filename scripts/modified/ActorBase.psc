; Gets the template of the ActorBase, topmost will return the highest parent
ActorBase Function GetTemplate(bool bTopMost = true) native

; Returns whether this actorbase has overlay head parts
bool Function HasHeadPartOverlays() native

; Returns the head part array from either the overlay list or original list
HeadPart[] Function GetHeadParts(bool bOverlays = false) native

; Get the Outfit of the actor
Outfit Function GetOutfit(bool bSleepOutfit = false) native

struct BodyWeight
	float thin
	float muscular
	float large
EndStruct

; The sum of the three ratios should add to 1.0
; Call QueueUpdate on the Actor instance to update visually
Function SetBodyWeight(BodyWeight weight) native

; Returns the 3 weight ratios
BodyWeight Function GetBodyWeight() native