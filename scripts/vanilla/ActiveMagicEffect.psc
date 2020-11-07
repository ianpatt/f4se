Scriptname ActiveMagicEffect extends ScriptObject Native Hidden

; Dispel this effect
Function Dispel() native

; Get the base MagicEffect this active effect is using
MagicEffect Function GetBaseObject() native

; Get the actor that cast this spell
Actor Function GetCasterActor() native

; Get the actor this spell is targeting (is attached to)
Actor Function GetTargetActor() native

; Turns on profiling for this specific object and all scripts attached to it - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StartObjectProfiling() native debugOnly

; Turns off profiling for this specific object and all scripts attached to it - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StopObjectProfiling() native debugOnly

; Event received when this effect is first started (OnInit may not have been run yet!)
Event OnEffectStart(Actor akTarget, Actor akCaster)
EndEvent

; Event received when this effect is finished (effect may already be deleted, calling
; functions on this effect will fail)
Event OnEffectFinish(Actor akTarget, Actor akCaster)
EndEvent

; The following events are received from the actor this effect is attached to:

;------------------------------------------------------------------------------
; Events from object references below
;------------------------------------------------------------------------------

; Event received when this reference is activated
Event OnActivate(ObjectReference akActionRef)
EndEvent

; Event received when this object's parent cell is attached
Event OnCellAttach()
EndEvent

; Event received when this object's parent cell is detached
Event OnCellDetach()
EndEvent

; Event received when every object in this object's parent cell is loaded (TODO: Find restrictions)
Event OnCellLoad()
EndEvent

; Event received when this object is closed
Event OnClose(ObjectReference akActionRef)
EndEvent

; Event received when this object enters, exits, or changes containers
Event OnContainerChanged(ObjectReference akNewContainer, ObjectReference akOldContainer)
EndEvent

; Event received when this reference's destruction stage has changed
Event OnDestructionStageChanged(int aiOldStage, int aiCurrentStage)
EndEvent

; Event received when this object is equipped by an actor
Event OnEquipped(Actor akActor)
EndEvent

; Event received when this furniture is exited.
Event OnExitFurniture(ObjectReference akActionRef)
EndEvent

; Event received when this object is grabbed by the player
Event OnGrab()
EndEvent

; Event that occurs when a flash program on a holotape wants to communicate with script.
Event OnHolotapeChatter( string astrChatter, float afNumericData )
EndEvent

; Event that occurs when a holotape is played via Pipboy or terminal.
Event OnHolotapePlay( ObjectReference aTerminalRef )
EndEvent

; Event received when an item is added to this object's inventory. If the item is a persistant reference, akItemReference will
; point at it - otherwise the parameter will be None
Event OnItemAdded(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
EndEvent

; Event received when an item is removed from this object's inventory. If the item is a persistant reference, akItemReference
; will point at it - otherwise the parameter will be None
Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
EndEvent

; Event recieved when this object is completely loaded - will be fired every time this object is loaded
Event OnLoad()
EndEvent

; Event received when the lock on this object changes
Event OnLockStateChanged()
EndEvent

; Event received when this object is opened
Event OnOpen(ObjectReference akActionRef)
EndEvent

; Event that occurs when the Pipboy enters/leaves the outer-radius of this radio ref
Event OnPipboyRadioDetection( bool abDetected )
EndEvent

; Event received when ref goes into dialogue with player
Event OnPlayerDialogueTarget()
EndEvent

; Event received when a workshop item receives power
Event OnPowerOn(ObjectReference akPowerGenerator)
EndEvent

; Event received when a workshop loses all power
Event OnPowerOff()
EndEvent

; Event received when this object, if a book, is read
Event OnRead()
EndEvent

; Event received when this object is released by the player
Event OnRelease()
EndEvent

; Event received when this reference is reset
Event OnReset()
EndEvent

; Event received when this reference is sold by an actor
Event OnSell(Actor akSeller)
EndEvent

; Event received when a spell is cast by this object
Event OnSpellCast(Form akSpell)
EndEvent

; Event received when translation is almost complete (from a call to TranslateTo, "almost" is determined by a gamesetting, default is 90% of the way)
Event OnTranslationAlmostComplete()
EndEvent

; Event received when translation is complete (from a call to TranslateTo)
Event OnTranslationComplete()
EndEvent

; Event received when translation is aborted (from a call to StopTranslateTo)
Event OnTranslationFailed()
EndEvent

; Event recieved when this starts hitting a target
Event OnTrapHitStart(ObjectReference akTarget, float afXVel, float afYVel, float afZVel, float afXPos, float afYPos, float afZPos, \
	int aeMaterial, bool abInitialHit, int aeMotionType)
EndEvent

; Event recieved when this stops hitting a target
Event OnTrapHitStop(ObjectReference akTarget)
EndEvent

; Event received when this trigger volume is entered
Event OnTriggerEnter(ObjectReference akActionRef)
EndEvent

; Event received when this trigger volume is left
Event OnTriggerLeave(ObjectReference akActionRef)
EndEvent

; Event received when this object is unequipped by an actor
Event OnUnequipped(Actor akActor)
EndEvent

; Event recieved when this object is being unloaded - will be fired every time this object is unloaded
Event OnUnload()
EndEvent

; Event received when the player enters or leaves Workshop Mode
Event OnWorkshopMode(bool aStart)
EndEvent

; Event received when a workshop item is destroyed ( removed from world and added back to workshop )
Event OnWorkshopObjectDestroyed(ObjectReference akReference)
EndEvent

; Event received when the player grabs an existing object reference for edit while in Workshop mode.
Event OnWorkshopObjectGrabbed(ObjectReference akReference)
EndEvent

; Event received when an existing workshop item is moved in the world
Event OnWorkshopObjectMoved(ObjectReference akReference)
EndEvent

; Event received when a workshop item is placed in the world
Event OnWorkshopObjectPlaced(ObjectReference akReference)
EndEvent

; Event that occurs when a workshop NPC is directed to move to another settlement
Event OnWorkshopNPCTransfer(Location akNewWorkshop, Keyword akActionKW)
EndEvent

; Event received when a workshop item has been repaired
Event OnWorkshopObjectRepaired(ObjectReference akReference)
EndEvent

;------------------------------------------------------------------------------
; Events from actors only below
;------------------------------------------------------------------------------

; Event that is triggered when this actor's combat state against the target changes
; State is as follows:
; 0 - not in combat
; 1 - in combat
; 2 - searching
Event OnCombatStateChanged(Actor akTarget, int aeCombatState)
EndEvent

; Event received when the actor completes a command in command mode.
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
Event OnCommandModeCompleteCommand(int aeCommandType, ObjectReference akTarget)
EndEvent

; Event received when the player begins commanding this actor.
Event OnCommandModeEnter()
EndEvent

; Event received when the player ends commanding this actor.
Event OnCommandModeExit()
EndEvent

; Event received when the player gives actor a command in command mode.
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
Event OnCommandModeGiveCommand(int aeCommandType, ObjectReference akTarget)
EndEvent

; Received when player dismisses teammate companion
Event OnCompanionDismiss()
EndEvent

; Event that is triggered when this actor's consciousness state changes
Event OnConsciousnessStateChanged(bool abUnconscious)
EndEvent

;Recieved immeadiately after a limb has become crippled or uncrippled.
Event OnCripple(ActorValue akActorValue, bool abCrippled)
EndEvent

; Event that is triggered when this actor gets a deferred kill added
; (they were in deferred kill mode and got "killed")
Event OnDeferredKill(Actor akKiller)
EndEvent

; Event that is triggered when this actor finishes dying
Event OnDeath(Actor akKiller)
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
Event OnDifficultyChanged(int aOldDifficulty, int aNewDifficulty)
EndEvent

; Event that is triggered when this actor begins dying
Event OnDying(Actor akKiller)
EndEvent

; Event received when an actor enters bleedout. (only if this alias points at an actor)
Event OnEnterBleedout()
EndEvent

; Event received when an actor enters sneaking.
Event OnEnterSneaking()
EndEvent

; Event received when this actor is in an Escort procedure and begins waiting for the escorted actor to catch up.
Event OnEscortWaitStart()
EndEvent

; Event received when this actor is in an Escort procedure and stops waiting because the escorted actor has caught up.
Event OnEscortWaitStop()
EndEvent

; Event that is triggered when this actor leaves the furniture
Event OnGetUp(ObjectReference akFurniture)
EndEvent

; Event received when this actor equips something - akReference may be None if object is not persistent
Event OnItemEquipped(Form akBaseObject, ObjectReference akReference)
EndEvent

; Event received when this actor unequips something - akReference may be None if object is not persistent
Event OnItemUnequipped(Form akBaseObject, ObjectReference akReference)
EndEvent

; Event that is triggered when this actor kills another
Event OnKill(Actor akVictim)
EndEvent

; Event that is triggered when this actor changes from one location to another
Event OnLocationChange(Location akOldLoc, Location akNewLoc)
EndEvent

; Event received when this actor's package changes
Event OnPackageChange(Package akOldPackage)
EndEvent

; Event received when this actor's package ends
Event OnPackageEnd(Package akOldPackage)
EndEvent

; Event received when this actor starts a new package
Event OnPackageStart(Package akNewPackage)
EndEvent

;Recieved immeadiately after the limb of a robot (or other actor whose race allows sub-segment damage) has become (or ceased to be) partially crippled.
Event OnPartialCripple(ActorValue akActorValue, bool abCrippled)
EndEvent

;Received when the player fails to pickpocket this actor
Event OnPickpocketFailed()
EndEvent

; Received when player creates a new robot
Event OnPlayerCreateRobot(Actor akNewRobot)
EndEvent

; Received when player enters vertibird
Event OnPlayerEnterVertibird(ObjectReference akVertibird)
EndEvent

; Received when player takes fall damage
Event OnPlayerFallLongDistance(float afDamage)
EndEvent

; Received when player fires a weapon out of combat and based on timer
Event OnPlayerFireWeapon(Form akBaseObject)
EndEvent

; Received when player heals his teammate
Event OnPlayerHealTeammate(Actor akTeammate)
EndEvent

; Received immediately after the player has loaded a save game. A good time to check for additional content.
Event OnPlayerLoadGame()
EndEvent

; Received when player mods a weapon or armor in the menu.
Event OnPlayerModArmorWeapon(Form akBaseObject, ObjectMod akModBaseObject)
EndEvent

; Received when the player mods a robot
Event OnPlayerModRobot(Actor akRobot, ObjectMod akModBaseObject)
EndEvent

; Received when player starts swimming
Event OnPlayerSwimming()
EndEvent

; Received when player uses a workbench
Event OnPlayerUseWorkBench(ObjectReference akWorkBench)
EndEvent

; Event received when this actor finishes changing its race
Event OnRaceSwitchComplete()
EndEvent

; Event that is triggered when this actor sits in the furniture
Event OnSit(ObjectReference akFurniture)
EndEvent

; Received when player gets a speech challenge in dialogue
Event OnSpeechChallengeAvailable(ObjectReference akSpeaker)
EndEvent