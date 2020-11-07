Scriptname Alias extends ScriptObject Native Hidden

; Returns the quest that owns this alias
Quest Function GetOwningQuest() native

; Turns on profiling for this specific object and all scripts attached to it - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StartObjectProfiling() native debugOnly

; Turns off profiling for this specific object and all scripts attached to it - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StopObjectProfiling() native debugOnly

; Event received when this alias is initialized and filled - before quest startup stage is run
; (other aliases on the same quest will also be filled)
Event OnAliasInit()
EndEvent

; Event received when the quest owning this alias is reset
Event OnAliasReset()
EndEvent

; Event received when the alias has been shut down (because the quest has been shut down)
; Note that the alias will be empty by the time this event is received
Event OnAliasShutdown()
EndEvent