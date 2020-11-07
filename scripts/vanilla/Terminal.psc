Scriptname Terminal extends Form Native Hidden

; Event called when a terminal menu item is run, with the specified target and terminal owner (in parallel with the fragment)
Event OnMenuItemRun(int auiMenuItemID, ObjectReference akTerminalRef)
EndEvent

Function ShowOnPipboy() native