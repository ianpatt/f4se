Function RegisterForKey(int key) native

Function UnregisterForKey(int key) native

Event OnKeyDown(int keyCode)
EndEvent

Event OnKeyUp(int keyCode, float time)
EndEvent

Function RegisterForControl(string control) native

Function UnregisterForControl(string control) native

Event OnControlDown(string control)
EndEvent

Event OnControlUp(string control, float time)
EndEvent

; Callback is the function name that is called when the event is triggered
Function RegisterForExternalEvent(string eventName, string callback) native

Function UnregisterForExternalEvent(string eventName) native


Function RegisterForCameraState() native

Function UnregisterForCameraState() native

; States
; 0 - First Person
; 1 - Auto Vanity
; 2 - VATS
; 3 - Free
; 4 - Iron Sights
; 5 - Transition
; 6 - Tween Menu
; 7 - ThirdPerson1
; 8 - ThirdPerson2
; 9 - Furniture
; 10 - Horse
; 11 - Bleedout
; 12 - Dialogue
Event OnPlayerCameraState(int oldState, int newState)
EndEvent

; Filter can be one or none of the following:
; ObjectReference
; Var[] (containing ObjectReferece Vars)
; FormList (containing ObjectReference)
; Filtering by Furniture object will receive events from any Actor who sits in it
; Filtering by Actor will receive events from whenever the specified Actor sits on any furniture
; Registering again with a different filter will result in merging of the filters
Function RegisterForFurnitureEvent(Var filter = None) native

; Filter works similarly to Register
; None filter will remove all active filters and unregister the event
Function UnregisterForFurnitureEvent(Var filter = None) native

Event OnFurnitureEvent(Actor akActor, ObjectReference akFurniture, bool isGettingUp)
EndEvent

; gamepad buttons using xinput button codes
Function RegisterForGamepadButton(int key) native
Function UnregisterForGamepadButton(int key) native

Event OnGamepadButtonDown(int keyCode)
EndEvent

Event OnGamepadButtonUp(int keyCode, float time)
EndEvent
