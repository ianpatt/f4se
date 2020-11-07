Scriptname Form extends ScriptObject Native Hidden

; Returns the formID for this object
Int Function GetFormID() native

; Obtains this form's value in gold. Will return -1 if the form doesn't have any value (like a quest)
int Function GetGoldValue() native

; Returns if this form has the specified keyword attached
bool Function HasKeyword(Keyword akKeyword) native

; Returns if this form has any of the keywords in the list attached
bool Function HasKeywordInFormList(FormList akKeywordList) native

; Is the "Known" flag set for this form?
bool Function PlayerKnows() native

; Turns on profiling for this specific object and all scripts attached to it - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StartObjectProfiling() native debugOnly

; Turns off profiling for this specific object and all scripts attached to it - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StopObjectProfiling() native debugOnly