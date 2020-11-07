Scriptname SASTriggerScript extends ObjectReference

{Sets variables for Follower Idle dialogue being aware of the Location Set Style the player is in while in this trigger.

For example, if an office building breaks into a cave area put this trigger box over the whole cave area and set the LocSetKeyword to "LocSetCave"

See also FollowersScript}

Group KeywordsReferenceProperties

Keyword property LocSetKeyword auto const
{keyword associated with the Location Set Style type of this area of the dungeon
Keyword should start with: LocSetXXX prefix
Example: LocSetCave
}


EndGroup

;handles cases of teetering in and out the edge of the trigger:
int playerInCount	
int previousInCount

bool scriptLocked = false  ;lock is required in case FollowersScript is busy, we don't want to send enter/leave events at the same time and have that script process the leave before the enter

Event OnTriggerEnter(ObjectReference akActionRef)

	while scriptLocked
		utility.wait(1)
	endWhile
	scriptLocked = true

	if akActionRef == Game.GetPlayer()

		previousInCount = playerInCount
		playerInCount += 1

		debug.trace(self + "OnTriggerEnter() previousInCount: " + previousInCount + ", playerInCount: " + playerInCount)

		;handles cases of teetering in and out the edge of the trigger:
		if previousInCount == 0 && playerInCount == 1
			FollowersScript.GetScript().HandleMessageFromSASTrigger(self, addMe = true)
		EndIf

	EndIf


	scriptLocked = false
EndEvent

Event OnTriggerLeave(ObjectReference akActionRef)

	while scriptLocked
		utility.wait(1)
	endWhile
	scriptLocked = true


	if akActionRef == Game.GetPlayer()
		previousInCount = playerInCount
		playerInCount -= 1

		debug.trace(self + "OnTriggerLeave() previousInCount: " + previousInCount + ", playerInCount: " + playerInCount)

		;handles cases of teetering in and out the edge of the trigger:
		if previousInCount == 1 && playerInCount == 0
			FollowersScript.GetScript().HandleMessageFromSASTrigger(self, addMe = false)
		EndIf

	endif

	scriptLocked = false

EndEvent

