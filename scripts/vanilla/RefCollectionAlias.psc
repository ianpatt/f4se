Scriptname RefCollectionAlias extends Alias Native Hidden
import ObjectReference

; kmk
; add all actors in list to specified faction
Function AddToFaction(faction akFaction)
	int i = 0
	while (i < GetCount())
		actor theActor = GetActorAt(i)
		if theActor
			theActor.AddToFaction(akFaction)
		endif
		i += 1
	endwhile
endFunction

; kmk
; block activation on all refs in collection
Function BlockActivation(bool abBlocked = True, bool abHideActivateText = false)
	int i = 0
	while i < GetCount()
		ObjectReference theRef = GetAt(i)
		if theRef
			theRef.BlockActivation(abBlocked, abHideActivateText)
		endif
		i += 1		
	endWhile
endFunction

; kmk
; convenience function - casts the reference at aiIndex as an actor before returning it
Actor Function GetActorAt( int aiIndex )
	return GetAt(aiIndex) as Actor
endFunction

; kmk
; return (first) owned object in collection
ObjectReference function GetFirstOwnedObject(Actor actorOwner)
	int i = 0
	int ownerIndex = -1
	while i < GetCount() && ownerIndex == -1
		if actorOwner.IsOwner(GetAt(i) as ObjectReference)
			ownerIndex = i
		endif
		i += 1
	endWhile
	if ownerIndex > -1
		return GetAt(ownerIndex)
	else
		return NONE
	endif
endFunction

; fab
; Enable everyone in the collection
function EnableAll(bool bFadeIn = false)
	int i = 0
	while i < GetCount()
		ObjectReference theRef = GetAt(i)
		if theRef
			theRef.EnableNoWait(bFadeIn)
		endif
		i += 1		
	endWhile
endFunction

; kmk
; Disable everyone in the collection
function DisableAll(bool bFadeOut = false)
	int i = 0
	while i < GetCount()
		ObjectReference theRef = GetAt(i)
		if theRef
			theRef.DisableNoWait(bFadeOut)
		endif
		i += 1		
	endWhile
endFunction


; fab
; Evaluate the packages of everyone in the collection
function EvaluateAll()
	int i = 0
	while i < GetCount()
		actor theActor = GetActorAt(i)
		if theActor
			theActor.EvaluatePackage()
		endif
		i += 1		
	endWhile
endFunction

; kmk
; Move everyone in collection
function MoveAllTo(ObjectReference akTarget)
	int i = 0
	while i < GetCount()
		ObjectReference theRef = GetAt(i)
		if theRef
			theRef.MoveTo(akTarget)
		endif
		i += 1		
	endWhile
endFunction


; kmk
; return true if actorOwner owns anything in the collection
bool function IsOwnedObjectInList(Actor actorOwner)
	int i = 0
	bool foundOwner = false
	while i < GetCount() && !foundOwner
		if actorOwner.IsOwner(GetAt(i) as ObjectReference)
			foundOwner = true
			debug.trace("IsOwnedObjectInList: item " + GetAt(i)+ " owned by " + actorOwner)
		endif
		i += 1
	endWhile
	return foundOwner
endFunction

; kmk
; convenience function - kills everything in the collection
Function KillAll(actor akKiller = NONE)
	int i = 0
	while i < GetCount()
		actor theActor = GetActorAt(i)
		if theActor
			theActor.Kill(akKiller)
		endif
		i += 1
	endWhile
endFunction

; kmk
; convenience function - everything in the collection starts combat with target
Function StartCombatAll(actor akCombatTarget)
	int i = 0
	while i < GetCount()
		actor theActor = GetActorAt(i)
		if theActor
			theActor.StartCombat(akCombatTarget)
		endif
		i += 1
	endWhile
endFunction

; kmk
; remove all actors in list to specified faction
Function RemoveFromFaction(faction akFaction)
	int i = 0
	while (i < GetCount())
		actor theActor = GetActorAt(i)
		if theActor
			theActor.RemoveFromFaction(akFaction)
		endif
		i += 1
	endwhile
endFunction

; kmk
; remove all actors in list from all factions
Function RemoveFromAllFactions()
	int i = 0
	while (i < GetCount())
		actor theActor = GetActorAt(i)
		if theActor
			theActor.RemoveFromAllFactions()
		endif
		i += 1
	endwhile
endFunction

; kmk
; Reset everyone in the collection
function ResetAll()
	int i = 0
	while i < GetCount()
		ObjectReference theRef = GetAt(i)
		if theRef
			theRef.Reset()
		endif
		i += 1		
	endWhile
endFunction

; kmk
; convenience function - sets/clears Protected on everyone
Function SetProtected(bool bSetProtected = true)
	int i = 0
	while i < GetCount()
		actor theActor = GetActorAt(i)
		if theActor
			theActor.SetProtected(bSetProtected)
		endif
		i += 1
	endWhile
endFunction

; kmk
; convenience function - sets/clears Essential on everyone
Function SetEssential(bool bSetEssential = true)
	int i = 0
	while i < GetCount()
		actor theActor = GetActorAt(i)
		if theActor
			theActor.SetEssential(bSetEssential)
		endif
		i += 1
	endWhile
endFunction


Function AddRefCollection(RefCollectionAlias refCollectionAliasToAdd)
	int CollectionCount = refCollectionAliasToAdd.GetCount() const
	int index = 0
	while index < CollectionCount
		addRef(refCollectionAliasToAdd.getAt(index))
		index += 1
	endWhile
endFunction

; kmk
; add an array of references to a collection
Function AddArray(ObjectReference[] refArrayToAdd)
	int index = 0
	while index < refArrayToAdd.Length
		addRef(refArrayToAdd[index])
		index += 1
	endWhile
endFunction

; kmk
; set an actor value on all refs in the collection
function SetValue(ActorValue akActorValue, float fValue)
	int i = 0
	while i < GetCount()
		actor theActor = GetActorAt(i)
		if theActor
			theActor.SetValue(akActorValue, fValue)
		endif
		i += 1
	endWhile
endFunction

; Link this RefCollectionAlias to another refCollectionAlias with this keyword.
; Limited by the number of objects in the refCollectionAlias it is called on
; If wrapLinks == true : this will not be limited by the link target, but will wrap around to the top of that list
bool function LinkCollectionTo(RefCollectionAlias LinkedRefCollectionAlias, keyword LinkKeyword = none, bool WrapLinks = false)
	int index = 0
	int LinkTargetCount = LinkedRefCollectionAlias.GetCount() const
	int CollectionCount = GetCount() const
	while index < CollectionCount
		objectReference currentRef = GetAt(index)
		if currentRef
			currentRef.SetLinkedRef(LinkedRefCollectionAlias.GetAt(index % LinkTargetCount), LinkKeyword)
		endif
		index += 1
		if !wrapLinks && index >= LinkTargetCount
			return false
		endif
	endWhile
	return true
endFunction

; Adds the given ref to this ref collection
Function AddRef(ObjectReference akNewRef) native

; Finds the specified reference in the collection. Returns a negative value if not found.
int Function Find(ObjectReference akFindRef) native

; Gets the ref at the given index in the collection 
ObjectReference Function GetAt( int aiIndex ) native

; Gets the count of refs in the collection
int Function GetCount() native

; Empties all refs from the collection
Function RemoveAll( ) native

; Takes the specific ref out of the collection
Function RemoveRef(ObjectReference akRemoveRef ) native

; The following events are received if the object the alias points at receives the events. The
; sender of each event is the specific ref or actor in the collection that fired the event:

;------------------------------------------------------------------------------
; Events from object references below
;------------------------------------------------------------------------------

; Event received when this reference is activated
Event OnActivate(ObjectReference akSenderRef, ObjectReference akActionRef)
EndEvent

; Event received when this object's parent cell is attached
Event OnCellAttach(ObjectReference akSenderRef)
EndEvent

; Event received when this object's parent cell is detached
Event OnCellDetach(ObjectReference akSenderRef)
EndEvent

; Event received when every object in this object's parent cell is loaded (TODO: Find restrictions)
Event OnCellLoad(ObjectReference akSenderRef)
EndEvent

; Event received when this object is closed
Event OnClose(ObjectReference akSenderRef, ObjectReference akActionRef)
EndEvent

; Event received when this object enters, exits, or changes containers
Event OnContainerChanged(ObjectReference akSenderRef, ObjectReference akNewContainer, ObjectReference akOldContainer)
EndEvent

; Event received when this reference's destruction stage has changed
Event OnDestructionStageChanged(ObjectReference akSenderRef, int aiOldStage, int aiCurrentStage)
EndEvent

; Event received when this object is equipped by an actor
Event OnEquipped(ObjectReference akSenderRef, Actor akActor)
EndEvent

; Event received when this furniture is exited.
Event OnExitFurniture(ObjectReference akSenderRef, ObjectReference akActionRef)
EndEvent

; Event received when this object is grabbed by the player
Event OnGrab(ObjectReference akSenderRef)
EndEvent

; Event received when an item is added to this object's inventory. If the item is a persistant reference, akItemReference will
; point at it - otherwise the parameter will be None
Event OnItemAdded(ObjectReference akSenderRef, Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
EndEvent

; Event received when an item is removed from this object's inventory. If the item is a persistant reference, akItemReference
; will point at it - otherwise the parameter will be None
Event OnItemRemoved(ObjectReference akSenderRef, Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
EndEvent

; Event recieved when this object is completely loaded - will be fired every time this object is loaded
Event OnLoad(ObjectReference akSenderRef)
EndEvent

; Event received when the lock on this object changes
Event OnLockStateChanged(ObjectReference akSenderRef)
EndEvent

; Event received when this object is opened
Event OnOpen(ObjectReference akSenderRef, ObjectReference akActionRef)
EndEvent

; Event that occurs when the Pipboy enters/leaves the outer-radius of this radio ref
Event OnPipboyRadioDetection(ObjectReference akSenderRef, bool abDetected)
EndEvent

; Event received when a workshop item receives power
Event OnPowerOn(ObjectReference akSenderRef, ObjectReference akPowerGenerator)
EndEvent

; Event received when a workshop loses all power
Event OnPowerOff(ObjectReference akSenderRef)
EndEvent

; Event received when this object, if a book, is read
Event OnRead(ObjectReference akSenderRef)
EndEvent

; Event received when this object is released by the player
Event OnRelease(ObjectReference akSenderRef)
EndEvent

; Event received when this reference is reset
Event OnReset(ObjectReference akSenderRef)
EndEvent

; Event received when this reference is sold by an actor
Event OnSell(ObjectReference akSenderRef, Actor akSeller)
EndEvent

; Event received when a spell is cast by this object
Event OnSpellCast(ObjectReference akSenderRef, Form akSpell)
EndEvent

; Event received when translation is almost complete (from a call to TranslateTo, "almost" is determined by a gamesetting, default is 90% of the way)
Event OnTranslationAlmostComplete(ObjectReference akSenderRef)
EndEvent

; Event received when translation is complete (from a call to TranslateTo)
Event OnTranslationComplete(ObjectReference akSenderRef)
EndEvent

; Event received when translation is aborted (from a call to StopTranslateTo)
Event OnTranslationFailed(ObjectReference akSenderRef)
EndEvent

; Event recieved when this starts hitting a target
Event OnTrapHitStart(ObjectReference akSenderRef, ObjectReference akTarget, float afXVel, float afYVel, float afZVel, float afXPos, float afYPos, \
	float afZPos, int aeMaterial, bool abInitialHit, int aeMotionType)
EndEvent

; Event recieved when this stops hitting a target
Event OnTrapHitStop(ObjectReference akSenderRef, ObjectReference akTarget)
EndEvent

; Event received when this trigger volume is entered
Event OnTriggerEnter(ObjectReference akSenderRef, ObjectReference akActionRef)
EndEvent

; Event received when this trigger volume is left
Event OnTriggerLeave(ObjectReference akSenderRef, ObjectReference akActionRef)
EndEvent

; Event received when this object is unequipped by an actor
Event OnUnequipped(ObjectReference akSenderRef, Actor akActor)
EndEvent

; Event recieved when this object is being unloaded - will be fired every time this object is unloaded
Event OnUnload(ObjectReference akSenderRef)
EndEvent

; Event received when the player enters or leaves Workshop Mode
Event OnWorkshopMode(ObjectReference akSenderRef, bool aStart)
EndEvent

; Event received when a workshop item is destroyed ( removed from world and added back to workshop )
Event OnWorkshopObjectDestroyed(ObjectReference akSenderRef, ObjectReference akReference)
EndEvent

; Event received when the player grabs an existing object reference for edit while in Workshop mode.
Event OnWorkshopObjectGrabbed(ObjectReference akSenderRef, ObjectReference akReference)
EndEvent

; Event received when an existing workshop item is moved in the world
Event OnWorkshopObjectMoved(ObjectReference akSenderRef, ObjectReference akReference)
EndEvent

; Event received when a workshop item is placed in the world
Event OnWorkshopObjectPlaced(ObjectReference akSenderRef, ObjectReference akReference)
EndEvent

; Event that occurs when a workshop NPC is directed to move to another settlement
Event OnWorkshopNPCTransfer(ObjectReference akSenderRef, Location akNewWorkshop, Keyword akActionKW)
EndEvent

; Event received when a workshop item has been repaired
Event OnWorkshopObjectRepaired(ObjectReference akSenderRef, ObjectReference akReference)
EndEvent

;------------------------------------------------------------------------------
; Events from actors only below
;------------------------------------------------------------------------------

; Event that is triggered when this actor's combat state against the target changes
; State is as follows:
; 0 - not in combat
; 1 - in combat
; 2 - searching
Event OnCombatStateChanged(ObjectReference akSenderRef, Actor akTarget, int aeCombatState)
EndEvent

; Event received when the object completes a command in command mode.
; Type is as follows:
; 0 - None
; 1 - Call
; 2 - Follow
; 3 - Move
; 4 - Attack
; 5 - Inspect
; 6 - Retrieve
; 7 - Stay
; 8 - Release
; 9 - Heal
Event OnCommandModeCompleteCommand(ObjectReference akSenderRef,  int aeCommand, ObjectReference akTarget)
EndEvent

; Event received when the player begins commanding this object 
Event OnCommandModeEnter(ObjectReference akSenderRef)
EndEvent

; Event received when the player ends commanding this object 
Event OnCommandModeExit(ObjectReference akSenderRef)
EndEvent

; Event received when the player gives object a command in command mode.
; Type is as follows:
; 0 - None
; 1 - Call
; 2 - Follow
; 3 - Move
; 4 - Attack
; 5 - Inspect
; 6 - Retrieve
; 7 - Stay
; 8 - Release
; 9 - Heal
Event OnCommandModeGiveCommand(ObjectReference akSenderRef, int aeCommand, ObjectReference akTarget)
EndEvent

; Received when player dismisses teammate companion
Event OnCompanionDismiss(ObjectReference akSenderRef)
EndEvent

; Event that is triggered when this actor's consciousness state changes
Event OnConsciousnessStateChanged(ObjectReference akSenderRef, bool abUnconscious)
EndEvent

;Recieved immeadiately after a limb has become crippled or uncrippled.
Event OnCripple(ObjectReference akSenderRef, ActorValue akActorValue, bool abCrippled)
EndEvent

; Event that is triggered when this actor gets a deferred kill added
; (they were in deferred kill mode and got "killed")
Event OnDeferredKill(ObjectReference akSenderRef, Actor akKiller)
EndEvent

; Event that is triggered when this actor finishes dying (only if this alias points at an actor)
Event OnDeath(ObjectReference akSenderRef, Actor akKiller)
EndEvent

; Event received when the player changes their difficulty
; Type is as follows:
; 0 - Very Easy
; 1 - Easy
; 2 - Normal
; 3 - Hard
; 4 - Very Hard
; 5 - Survival (no Hardcore) DEFUNCT
; 6 - Survival w/ Hardcore
Event OnDifficultyChanged(ObjectReference akSenderRef, int aOldDifficulty, int aNewDifficulty)
EndEvent

; Event that is triggered when this actor begins dying (only if this alias points at an actor)
Event OnDying(ObjectReference akSenderRef, Actor akKiller)
EndEvent

; Event received when an actor enters bleedout. (only if this alias points at an actor)
Event OnEnterBleedout(ObjectReference akSenderRef)
EndEvent

; Event received when an actor enters sneaking.
Event OnEnterSneaking(ObjectReference akSenderRef)
EndEvent

; Event received when this actor is in an Escort procedure and begins waiting for the escorted actor to catch up.
Event OnEscortWaitStart(ObjectReference akSenderRef)
EndEvent

; Event received when this actor is in an Escort procedure and stops waiting because the escorted actor has caught up.
Event OnEscortWaitStop(ObjectReference akSenderRef)
EndEvent

; Event that is triggered when this actor leaves the furniture
Event OnGetUp(ObjectReference akSenderRef, ObjectReference akFurniture)
EndEvent

; Event that occurs when a flash program on a holotape wants to communicate with script.
Event OnHolotapeChatter( ObjectReference akSenderRef, string astrChatter, float afNumericData )
EndEvent

; Event that occurs when a holotape is played via Pipboy or terminal. 
Event OnHolotapePlay( ObjectReference akSenderRef, ObjectReference aTerminalRef )
EndEvent

; Event received when this actor equips something - akReference may be None if object is not persistent (only if this alias points at an actor)
Event OnItemEquipped(ObjectReference akSenderRef, Form akBaseObject, ObjectReference akReference)
EndEvent

; Event received when this actor unequips something - akReference may be None if object is not persistent (only if this alias points at an actor)
Event OnItemUnequipped(ObjectReference akSenderRef, Form akBaseObject, ObjectReference akReference)
EndEvent

; Event that is triggered when this actor kills another
Event OnKill(ObjectReference akSenderRef, Actor akVictim)
EndEvent

; Event that is triggered when this actor changes from one location to another (only if this alias points at an actor)
Event OnLocationChange(ObjectReference akSenderRef, Location akOldLoc, Location akNewLoc)
EndEvent

; Event received when this actor's package changes (only if this alias points at an actor)
Event OnPackageChange(ObjectReference akSenderRef, Package akOldPackage)
EndEvent

; Event received when this actor's package ends (only if this alias points at an actor)
Event OnPackageEnd(ObjectReference akSenderRef, Package akOldPackage)
EndEvent

; Event received when this actor starts a new package (only if this alias points at an actor)
Event OnPackageStart(ObjectReference akSenderRef, Package akNewPackage)
EndEvent

;Recieved immeadiately after the limb of a robot (or other actor whose race allows sub-segment damage) has become (or ceased to be) partially crippled.
Event OnPartialCripple(ObjectReference akSenderRef, ActorValue akActorValue, bool abCrippled)
EndEvent

;Received when the player fails to pickpocket this actor
Event OnPickpocketFailed(ObjectReference akSenderRef)
EndEvent

; Received when player creates a new robot
Event OnPlayerCreateRobot(ObjectReference akSenderRef, Actor akNewRobot)
EndEvent

; Event received when ref goes into dialogue with player
Event OnPlayerDialogueTarget(ObjectReference akSenderRef)
EndEvent

; Received when player enters vertibird
Event OnPlayerEnterVertibird(ObjectReference akSenderRef, ObjectReference akVertibird)
EndEvent

; Received when player takes fall damage
Event OnPlayerFallLongDistance(ObjectReference akSenderRef, float afDamage)
EndEvent

; Received when player fires a weapon out of combat and based on timer
Event OnPlayerFireWeapon(ObjectReference akSenderRef, Form akBaseObject)
EndEvent

; Received when player heals his teammate
Event OnPlayerHealTeammate(ObjectReference akSenderRef, Actor akTeammate)
EndEvent

; Received immediately after the player has loaded a save game. A good time to check for additional content.
Event OnPlayerLoadGame(ObjectReference akSenderRef)
EndEvent

; Received when player mods a weapon or armor in the menu.
Event OnPlayerModArmorWeapon(ObjectReference akSenderRef, Form akBaseObject, ObjectMod akModBaseObject)
EndEvent

; Received when the player mods a robot
Event OnPlayerModRobot(ObjectReference akSenderRef, Actor akRobot, ObjectMod akModBaseObject)
EndEvent

; Received when player starts swimming
Event OnPlayerSwimming(ObjectReference akSenderRef)
EndEvent

; Received when player uses a workbench
Event OnPlayerUseWorkBench(ObjectReference akSenderRef, ObjectReference akWorkBench)
EndEvent

; Event received when this actor finishes changing its race
Event OnRaceSwitchComplete(ObjectReference akSenderRef)
EndEvent

; Event that is triggered when this actor sits in the furniture
Event OnSit(ObjectReference akSenderRef, ObjectReference akFurniture)
EndEvent

; Received when player gets a speech challenge in dialogue
Event OnSpeechChallengeAvailable(ObjectReference akSenderRef, ObjectReference akSpeaker)
EndEvent