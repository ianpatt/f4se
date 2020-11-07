Scriptname TopicInfo extends Form Native Hidden

; Returns the quest that owns this topic info
Quest Function GetOwningQuest() native

; Returns if the topic info has been said to the player
bool  Function HasBeenSaid() native

; Event called when this topic info begins (in parallel with the fragment)
Event OnBegin(ObjectReference akSpeakerRef, bool abHasBeenSaid)
EndEvent

; Event called when this topic info ends (in parallel with the fragment)
Event OnEnd(ObjectReference akSpeakerRef, bool abHasBeenSaid)
EndEvent