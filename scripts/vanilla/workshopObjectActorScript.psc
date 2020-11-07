Scriptname workshopObjectActorScript extends Actor Const
{for workshop objects that are actors (e.g. turrets)}

Event OnLoad()
	SetNoBleedoutRecovery(true)
EndEvent

Event OnEnterBleedout()
	debug.trace("OnEnterBleedout")
    HandleDeath()
EndEvent
Event OnDeath(Actor akKiller)
	debug.trace("OnDeath")
	HandleDeath()
EndEvent

Event OnWorkshopObjectPlaced(ObjectReference akReference)
	EvaluatePackage(true)
EndEvent

function HandleRepair()
	debug.trace(self + "HandleRepair")
	if IsDead()
		Resurrect()
	endif

	workshopObjectScript workshopObject = (self as ObjectReference) as workshopObjectScript
	if workshopObject
    	SetValue(workshopObject.WorkshopParent.WorkshopActorWounded, 0)
	endif
	Repair()
endFunction

function HandleDeath()
	debug.trace(self + "HandleDeath")
	workshopObjectScript workshopObject = (self as ObjectReference) as workshopObjectScript
    if workshopObject
    	SetValue(workshopObject.WorkshopParent.WorkshopActorWounded, 1)
    	workshopObject.HandleDestruction()
    endif
endFunction
