Scriptname Package extends Form Native Hidden

; Returns the quest that owns this package
Quest Function GetOwningQuest() native

; Obtains the package that is the parent template of this one (if any)
Package Function GetTemplate() native

; Event called when the package first starts (in parallel with the fragment)
Event OnStart(Actor akActor)
EndEvent

; Event called when the package ends (in parallel with the fragment)
Event OnEnd(Actor akActor)
EndEvent

; Event called when the package changes (in parallel with the fragment)
Event OnChange(Actor akActor)
EndEvent