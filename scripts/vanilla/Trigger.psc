Scriptname Trigger extends ObjectReference  
{
 base class for a trigger that watches for a target to enter and exit

 fireTriggerEvent function is called whenever the target enters or leaves the trigger
 
 Derive from this script, and override fireTriggerEvent function for whatever you need -- you can use IsTargetInTrigger to check if the target has just entered or left
 
}

bool bTargetInTrigger = false conditional

objectReference property targetRef auto
{ if None (default), watching for player
  otherwise, watching for this reference
}

Event onLoad()
	if targetRef == None
		targetRef = game.getPlayer()
	endif
endEvent

Event onTriggerEnter(objectReference triggerRef)
	if(triggerRef == targetRef)
		;target has entered the trigger
		SetTargetInTrigger(true)
; 		debug.trace(self + " target has entered the trigger")
	endif
endEvent

Event onTriggerLeave(objectReference triggerRef)
	if (triggerRef == targetRef)
		;target has left the trigger
		SetTargetInTrigger(false)
; 		debug.trace(self + " target has left the trigger")
	endif
endEvent

bool function IsTargetInTrigger()
	return bTargetInTrigger
endFunction

; PRIVATE function - do not call from outside this script
function SetTargetInTrigger(bool isInTrigger)
	bTargetInTrigger = isInTrigger
	fireTriggerEvent()
endFunction

function fireTriggerEvent()
	; do something when target enters/leaves trigger
endFunction
