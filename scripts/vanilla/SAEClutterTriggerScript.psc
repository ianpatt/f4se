Scriptname SAEClutterTriggerScript extends ObjectReference

{Sets variables for Follower Idle dialogue being aware of the enemy type able to be seen while the player is in this trigger.

For example, if there's obviously Raider clutter in a room or area of a dungeon, put this trigger box there and set the LocEncKeyword to "LocEncRaiders"

See also FollowersScript}

Group KeywordsReferenceProperties

Keyword property LocEncKeyword auto const
{keyword associated with the Encounter type of this area of the dungeon
Keyword should start with: LocEncXXX prefix
Example: LocEncRaiders
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
			FollowersScript.GetScript().HandleMessageFromSAEClutterTrigger(self, addMe = true)
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
			FollowersScript.GetScript().HandleMessageFromSAEClutterTrigger(self, addMe = false)
		EndIf

	endif

	scriptLocked = false

EndEvent

