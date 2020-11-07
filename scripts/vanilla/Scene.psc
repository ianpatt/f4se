Scriptname Scene extends Form Native Hidden

; Forces a scene to start and kills the current scenes running on any ref in that scene
Function ForceStart() native

; Starts this scene
Function Start() native

; Stops the scene
Function Stop() native

; Is this scene currently playing?
bool Function IsPlaying() native

; Returns the quest that owns this scene
Quest Function GetOwningQuest() native

; Returns whether the specified action is complete or not
bool Function IsActionComplete(int aiActionID) native

; Pauses or unpauses the scene
Function Pause(bool abPause) native

; Event received when a scene action is executed on an alias (in parallel with the fragment)
Event OnAction(int auiActionID, ReferenceAlias akAlias)
EndEvent

; Event received when this scene starts up (in parallel with the fragment)
Event OnBegin()
EndEvent

; Event received when this scene ends (in parallel with the fragment)
Event OnEnd()
EndEvent

; Event received when a phase starts (in parallel with the fragment)
Event OnPhaseBegin(int auiPhaseIndex)
EndEvent

; Event received when a phase ends (in parallel with the fragment)
Event OnPhaseEnd(int auiPhaseIndex)
EndEvent