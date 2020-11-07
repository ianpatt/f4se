Scriptname VisualEffect extends Form Native Hidden

; Plays this visual effect on the specified object, for a certain length of time (negative for infinite) and possibly facing another object
Function Play(ObjectReference akObject, float afTime = -1.0, ObjectReference akFacingObject = None) native

; Stops this visual effect from playing in the specified object
Function Stop(ObjectReference akObject) native