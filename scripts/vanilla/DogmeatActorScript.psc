Scriptname DogmeatActorScript extends Actor 
{Script for handling things that Dogmeat should do because he's a dog / dogmeat. Companion/Follower related stuff is in scripts attached to the Followers Quest and Aliases}

faction property currentCompanionFaction auto const

Location Property HomeLocation auto mandatory
{initial location for companion's home, this will be changed to whereever the player assigns them when dismissing the companion}
Idle Property Dogmeat_Neutral_ToyShake Auto Const Mandatory
MiscObject Property TeddyBear Auto Const Mandatory

Event OnCombatStateChanged(Actor akTarget, int aeCombatState)
	if (aeCombatState == 0)
    	;Dogmeat leaves combat, give him Neutral Face
    	DogmeatIdles.FaceNeutral()
        DogmeatIdles.setDogmeatNeutral()
        if game.getPlayer().isSneaking()
            ; if the player is sneaking when combat ends, so should Dogmeat
            dogmeatIdles.setDogmeatAgitated()
        endif
    elseif (aeCombatState == 1)
    	;Dogmeat enters combat, give him Neutral Face
    	DogmeatIdles.FaceNeutral()
        DogmeatIdles.setDogmeatNeutral()
	elseif (aeCombatState == 2)
		;Dogmeat searching, give him Neutral Face
    	DogmeatIdles.FaceNeutral()
        DogmeatIdles.setDogmeatNeutral()
	endIf
endEvent

Event OnInit()
    addInventoryEventFilter(TeddyBear)
    ignoreFriendlyHits()
EndEvent

Event OnItemAdded(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
    debug.trace("Dogmeat received "+akBaseItem+" from "+akSourceContainer)
    if akBaseItem == TeddyBear
        debug.trace("I got me a teddy bear!  LET'S PLAY!!!!")
        playIdle(Dogmeat_Neutral_ToyShake)
    endif
EndEvent

;event OnCommandModeCompleteCommand(int aeCommandType, objectreference akTarget)
    ; note - this event is handled in followersScript.psc.
    ; as of this writing it's just to make sure he mirrors player sneak.
;endEvent

Event OnEnterBleedout()
    ;Dogmeat enters essential down, give him Sad Face
    debug.trace(self + "DOGMEAT ENTERS BLEEDOUT")
    DogmeatIdles.FaceSad()
EndEvent

bool FUNCTION makeDogmeatSniffy(bool bControl)
    {This function will make dogmeat use high sniff settings when true, normal when false.}
    if self.is3dloaded()
        if bControl == TRUE
            debug.trace("Dogmeat: High Sniff Settings ON")
            self.setAnimationVariableInt("iMinTicksWalk",1)
            self.setAnimationVariableInt("iMaxTicksWalk",2)
            self.setAnimationVariableInt("iMinTicksWalkFlavor",10)
            self.setAnimationVariableInt("iMaxTicksWalkFlavor",99)
        else
            debug.trace("Dogmeat: High Sniff Settings OFF")
            self.setAnimationVariableInt("iMinTicksWalk",3)
            self.setAnimationVariableInt("iMaxTicksWalk",10)
            self.setAnimationVariableInt("iMinTicksWalkFlavor",1)
            self.setAnimationVariableInt("iMaxTicksWalkFlavor",5)
        endif

        return TRUE
    else
        debug.trace("DogmeatActorScript: Tried to set sniff variables, but 3D wasn't loaded.")
        return FALSE
    endif
endFUNCTION