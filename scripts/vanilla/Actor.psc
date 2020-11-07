Scriptname Actor extends ObjectReference Native Hidden

; Relationship functions use the following values:
; 4 - Lover
; 3 - Ally
; 2 - Confidant
; 1 - Friend
; 0 - Acquaintance
; -1 - Rival
; -2 - Foe
; -3 - Enemy
; -4 - Archnemesis

; DEPRECATED - use MakePlayerFriend() instead
; replacement for ModFavorPoints
; if iFavorPoints > 0, will setRelationshipRank to 1 if 0
; otherwise, won't do anything
Function ModFavorPoints(int iFavorPoints = 1)
	if iFavorPoints > 0
		MakePlayerFriend()
	else
; 		debug.trace(self + " ModFavorPoints called with negative param. NO EFFECT.")
	endif
endFunction

; also DEPRECATED
Function ModFavorPointsWithGlobal(GlobalVariable FavorPointsGlobal)
	ModFavorPoints(FavorPointsGlobal.GetValueInt())
endFunction

;this function will make an actor a friend of the player if allowed
Function MakePlayerFriend()
	ActorBase myBase = GetActorBase()
	if myBase.IsUnique()
		if GetRelationshipRank(Game.GetPlayer())== 0
; 			debug.trace(self + " MakePlayerFriend called on neutral actor - changed to FRIEND.")
			SetRelationshipRank(Game.GetPlayer(), 1)
		else
; 			debug.trace(self + " MakePlayerFriend called on non-neutral actor - NO EFFECT.")
		endif
	else
; 		debug.trace(self + " MakePlayerFriend called on non-Unique actor. NO EFFECT.")
	endif
endFunction

; Adds the specified perk to this actor
Function AddPerk(Perk akPerk, bool abNotify=false) native

; Adds the specified spell to this actor - returns true on success
bool Function AddSpell(Spell akSpell, bool abVerbose=true) native

; Sets this a essential actors ability to talk when in a bleedout state
Function AllowBleedoutDialogue(bool abCanTalk ) native

; overrides the race flag on an actor and determines if he can talk to the player in dialogue menu
Function AllowPCDialogue(bool abTalk) native

; Attaches an "ash pile" to this actor, placing it at this actor's location and using the specified
; base object (or leveled item list) to represent the pile. If None is passed, it will use the
; default ash pile object
Function AttachAshPile(Form akAshPileBase = None) native

; Attempt to change the set of animations loaded for this actor.
Function AttemptAnimationSetSwitch() native

; Can this actor fly here?
bool Function CanFlyHere() native

; Change this actor's anim archetype
bool Function ChangeAnimArchetype(keyword apKeyword = none) native

; Change this actor's anim flavor
bool Function ChangeAnimFlavor(keyword apKeyword = none) native

; Change this actor's headpart
Function ChangeHeadPart(headpart apHeadPart, bool abRemovePart = false, bool abRemoveExtraParts = false) native

; Clears this actor's arrested state
Function ClearArrested() native

; Clears any expression override on the actor
Function ClearExpressionOverride() native

; Clears this actor's extra arrows 3D
Function ClearExtraArrows() native

; Remove the obligation to use a particular marker when this actor has to land.
Function ClearForcedLandingMarker()
  SetForcedLandingMarker( None )
endFunction

; Clears this actor's look at target
Function ClearLookAt() native

; Initiates a dismount.
bool Function Dismount() native

; Dispel all spells from this actor
Function DispelAllSpells() native

; Dispel a spell from this actor
bool Function DispelSpell( Spell akSpell ) native

; Apply a spell to a target in combat
Function DoCombatSpellApply( Spell akSpell, ObjectReference akTarget ) native

; Enables or disable's this actor's AI, pausing their voice mid-line if desired.
Function EnableAI(bool abEnable = true, bool abPauseVoice = false) native

; End the Deferred Kill state. This must only be called if StartDeferredKill was called first.
Function EndDeferredKill() native

; Forces this actor to equip the specified item, preventing removal if requested
Function EquipItem(Form akItem, bool abPreventRemoval = false, bool abSilent = false) native

; Forces this actor to equip the specified spell. The casting source can be:
; 0 - Left hand
; 1 - Right hand
Function EquipSpell(Spell akSpell, int aiSource) native

; Forces the AI to re-evaluate its package stack
Function EvaluatePackage(bool abResetAI = false) native


;FOLLOWER Functions -- jduvall
;NOTE: For these functions to work, the NPC must have a FollowPlayer package that he is currently running
function FollowerWait()
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.FollowerWait(self)
endFunction

function FollowerFollow()
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.FollowerFollow(self)
endFunction

function FollowerSetDistanceNear()
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.FollowerSetDistanceNear(self)
endFunction

function FollowerSetDistanceMedium()
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.FollowerSetDistanceMedium(self)
endFunction

function FollowerSetDistanceFar()
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.FollowerSetDistanceFar(self)
endFunction


;returns the ActorBase
ActorBase function GetActorBase()
	return GetBaseObject() as ActorBase
endFunction

; Obtains how much it would cost to bribe this actor
int Function GetBribeAmount() native

; Get all combat targets for this actor
Actor[] Function GetAllCombatTargets() native

; Get the faction this actor reports crimes to
Faction Function GetCrimeFaction() native

; Gets this actor's current combat state
int Function GetCombatState() native

; Gets this actor's current combat target
Actor Function GetCombatTarget() native

; Gets this actor's current AI package
Package Function GetCurrentPackage() native

; Gets this actor's current dialogue target
Actor Function GetDialogueTarget() native

; Obtains the item quipped for the specified equip index
; Return values are:
; -1 - Error
; 0 - Nothing
; 1 - One-handed sword
; 2 - One-handed dagger
; 3 - One-handed axe
; 4 - One-handed mace
; 5 - Two-handed sword
; 6 - Two-handed axe
; 7 - Bow
; 8 - Staff
; 9 - Gun
; 10 - Grenade
; 11 - Mine
; 24 - Magic spell
; 25 - Shield
; 26 - Torch
int Function GetEquippedItemType(int aiEquipIndex) native

; Gets this actor's currently equipped weapon for the specified equip index
; Defaults to the actor's first equip index
Weapon Function GetEquippedWeapon(int aiEquipIndex = 0) native

; Gets this actor's currently equipped shield
Armor Function GetEquippedShield() native

; Gets the spell currently equipped in the specified source
; 0 - Left Hand
; 1 - Right Hand
; 2 - Other
; 3 - Instant
Spell Function GetEquippedSpell(int aiSource) native

; Obtains this actor's rank with the specified faction - returns -1 if the actor is not a member
int Function GetFactionRank(Faction akFaction) native

; Obtains this actor's faction-based reaction to the other actor
; 0 - Neutral
; 1 - Enemy
; 2 - Ally
; 3 - Friend
int Function GetFactionReaction(Actor akOther) native

; Obtains this actor's current flight state
; 0 - Not flying
; 1 - Taking off
; 2 - Cruising
; 3 - Hovering
; 4 - Landing
int Function GetFlyingState() native

; Get the ref at which this actor is obliged to land, if one is set (or none, if not).
ObjectReference Function GetForcedLandingMarker() native

; Retrieves the amount of gold this actor has
int Function GetGoldAmount() native

; Gets this actor's highest relationship rank - returns 0 if they have no relationships
int Function GetHighestRelationshipRank() native

; Returns this actor's killer - or None if this actor is still alive
Actor Function GetKiller() native

; Returns this actor's current level.
int Function GetLevel() native

; Returns this actor's current light level.
float Function GetLightLevel() native

; Gets this actor's highest relationship rank - returns 0 if they have no relationships
int Function GetLowestRelationshipRank() native

; Obtains a leveled actor's "fake" base (the one generated by the game when the
; actor is leveled. This differs from GetActorBase which will return the editor base
; object)
ActorBase Function GetLeveledActorBase() native

; Queries whether this actor has no bleedout recovery flag set.
bool Function GetNoBleedoutRecovery() native

; Queries whether this actor receives player input
bool Function GetPlayerControls() native

; Returns this actor's race
Race Function GetRace() native

; Obtains the relationship rank between this actor and another
int Function GetRelationshipRank(Actor akOther) native

; Obtains this actor's sit state, which is one of the following:
; 0 - Not sitting
; 2 - Not sitting, wants to sit
; 3 - Sitting
; 4 - Sitting, wants to stand
int Function GetSitState() native

; Obtains this actor's sleep state, which is one of the following:
; 0 - Not sleeping
; 2 - Not sleeping, wants to sleep
; 3 - Sleeping
; 4 - Sleeping, wants to wake
int Function GetSleepState() native

; Checks to see if this actor has the specified association with the other actor - or anyone (if no actor is passed)
bool Function HasAssociation(AssociationType akAssociation, Actor akOther = None) native

; Checks to see if this actor has a family relationship with the other actor - or anyone (if no actor is passed)
bool Function HasFamilyRelationship(Actor akOther = None) native

; Sees if this actor has detection-based line-of-sight to another object. Only the player can check LOS to a non-actor
bool Function HasDetectionLOS(ObjectReference akOther) native

; Checks to see if this actor is currently being affected by the given Magic Effect
bool Function HasMagicEffect(MagicEffect akEffect) native

; Checks to see if this actor is currently being affected by a Magic Effect with the given Keyword
bool Function HasMagicEffectWithKeyword(Keyword akKeyword) native

; Checks to see if this actor has a parent relationship with the other actor
bool Function HasParentRelationship(Actor akOther) native

; Checks to see if this actor has the given Perk
bool Function HasPerk(Perk akPerk) native

; Checks to see if this actor has the given Spell
bool Function HasSpell(Form akForm) native

; Returns whether this actor's AI is enabled.
bool Function IsAIEnabled() native

; Returns if this actor is alarmed or not
bool Function IsAlarmed() native

; Returns if this actor is alerted or not
bool Function IsAlerted() native

; Is this actor allowed to fly?
bool Function IsAllowedToFly() native

; Is this actor currently arrested?
bool Function IsArrested() native

; Is this actor currently arresting his target? (Must be a guard and alarmed)
bool Function IsArrestingTarget() native

; Is the actor being ridden?
bool Function IsBeingRidden() native

; check if akActor is riding this actor
bool Function IsBeingRiddenBy(Actor akActor) native

; Is this actor currently bleeding out?
bool Function IsBleedingOut() native

; Queries whether this actor has player bribe flag set.
bool Function IsBribed() native

; Is this actor a child?
bool Function IsChild() native

; Is this actor a commanded by another?
bool Function IsCommandedActor() native

; Returns if this actor is dead or not
bool Function IsDead() native

; Returns if this actor is detected by the other one
bool Function IsDetectedBy(Actor akOther) native

; Check to see if a limb has been dismembered
bool Function IsDismembered( string asBodyPart = "" ) native

; Is this actor doing a favor for the player?
bool Function IsDoingFavor() native

; Returns if the specified object is equipped on this actor
bool Function IsEquipped(Form akItem) native

; Is this actor essential?
bool Function IsEssential() native

; Returns if this actor is flying or not
bool Function IsFlying() native

; Returns if this actor is a guard or not
bool Function IsGuard() native

; Is this actor flagged as a ghost?
bool Function IsGhost() native

; Is this actor hostile to another actor?
bool Function IsHostileToActor(Actor akActor) native

; Returns if this actor is currently in combat
bool Function IsInCombat() native

; Checks to see if this actor is a member of the specified faction
bool Function IsInFaction(Faction akFaction) native

; Returns whether this actor is currently in iron sights.
bool Function IsInIronSights() native

; Returns if this actor is in a kill move or not
bool Function IsInKillMove() native

; Return true if this actor is in power armor
bool Function IsInPowerArmor()
	;Return if the actor has the Power Armor Perk
	Return HasPerk(Game.GetFormFromFile(0x0001F8A9, "Fallout4.esm") as perk)
EndFunction

; Queries whether this actor is in a scene.
bool Function IsInScene() native

; Queries whether this actor has player intimidated flag set.
bool Function IsIntimidated() native

; Is the actor on a mount?
bool Function IsOnMount() native

; Is the actor over-encumbered?
bool Function IsOverEncumbered() native

; kmk
; does the actor own the specified reference (in any way)
bool Function IsOwner(ObjectReference akObject)
	if akObject.GetActorRefOwner()
		return akObject.GetActorRefOwner() == self
	elseif akObject.GetActorOwner()
		; check against both normal actor base and leveled actor base for completeness
		return ( akObject.GetActorOwner() == GetActorBase() || akObject.GetActorOwner() == GetLeveledActorBase() )
	elseif akObject.GetFactionOwner()
		return IsInFaction(akObject.GetFactionOwner())
	else
		return false
	endif
endFunction

; Checks to see if this actor the last ridden horse of the player
bool Function IsPlayersLastRiddenHorse() native

; Is this actor currently a teammate of the player?
bool Function IsPlayerTeammate() native

; Is this actor currently running?
bool Function IsRunning() native

; Is the seat that corresponds to the passed in keyword currently occupied?
; note: if this is not a mount with multiple seats or apKeyword is not a valid seat, this function will return false
bool Function IsSeatOccupied(keyword apKeyword) native

; Is this actor currently sneaking?
bool Function IsSneaking() native

; Is this actor currently sprinting?
bool Function IsSprinting() native

; Is this actor talking
bool Function IsTalking() native

; Is this actor trespassing?
bool Function IsTrespassing() native

; Is this actor unconscious?
bool Function IsUnconscious() native

; Does this actor have his weapon and/or magic drawn?
bool Function IsWeaponDrawn() native

; Kills this actor with the killer being the guilty party
Function Kill(Actor akKiller = None) native

; Kills this actor even if essential
Function KillEssential(Actor akKiller = None)
	SetEssential(false)
	SetProtected(false)
	Kill(akKiller)
endFunction

; Kills this actor without a kill event with the killer being the guilty party
Function KillSilent(Actor akKiller = None) native

; Dismember a limb from the actor
; The body part string should be the same string as listed in the editor for body part - part type
; Here is the list of current parts
;	"Torso",
;	"Head1",
;	"Eye",
;	"LookAt",
;	"Fly Grab",
;	"Head2",
;	"LeftArm1",
;	"LeftArm2",
;	"RightArm1",
;	"RightArm2",
;	"LeftLeg1",
;	"LeftLeg2",
;	"LeftLeg3",
;	"RightLeg1",
;	"RightLeg2",
;	"RightLeg3",
;	"Brain",
;	"Weapon",
;	"Root",
;	"COM",
;	"Pelvis",
;	"Camera",
;	"Offset Root",
;	"Left Foot",
;	"Right Foot",
; The force bools should only be used if you want to ignore the random odds set up in the body part data in the editor
Function Dismember( string asBodyPart, bool abForceExplode = false, bool abForceDismember = false, bool abForceBloodyMess = false ) native

; Mark the given item as a favorite.
Function MarkItemAsFavorite(Form akItem, int aiSlot=-1) native

; Modifies this actor's rank in the faction
Function ModFactionRank(Faction akFaction, int aiMod) native

; Pop this actor to the initial location for a package. Mainly for use on
; disabled actors, since they would normally start at their editor locations.
Function MoveToPackageLocation( ) native

; Opens this actor's inventory, as if you were pick-pocketing them. Only works on teammates, or anyone if forced.
Function OpenInventory(bool abForceOpen = false) native

; Make the actor path to a reference, latent version
; Note: this method doesn't return until the goal is reached or pathing
; failed or was interrupted (by another request for instance)
bool Function PathToReference(ObjectReference aTarget, float afWalkRunPercent) native

; Send an idle to the actor to load in and play.
bool Function PlayIdle(Idle akIdle) native

; Attempts to run action on actor.
bool Function PlayIdleAction(Action aAction, ObjectReference aTarget = None) native

; Send an idle to the actor to play, overriding its target with the specified reference
bool Function PlayIdleWithTarget(Idle akIdle, ObjectReference akTarget) native

; Send an event to the subgraphs of an actor.
Function PlaySubGraphAnimation(string asEventName) native

; Removes this actor from the specified faction
Function RemoveFromFaction(Faction akFaction) native

; Removes this actor from all factions
Function RemoveFromAllFactions() native

; Removes the specified perk from this actor
Function RemovePerk(Perk akPerk) native

; Removes the specified spell from this actor - returns true on success
bool Function RemoveSpell(Spell akSpell) native

; Resets this actor's health and limb state
Function ResetHealthAndLimbs() native

; Resurrects this actor
Function Resurrect() native

; Has this actor behave as if assaulted
Function SendAssaultAlarm() native

; Has this actor behave as if they caught the target trespassing
Function SendTrespassAlarm(Actor akCriminal) native

; Sets the actor in an alerted state
Function SetAlert(bool abAlerted = true) native

; Sets whether this actor is allowed to fly or not - if not, will land the actor
Function SetAllowFlying(bool abAllowed = true, bool abAllowCrash = true, bool abAllowSearch = false) native

; Sets this actor's alpha - with an optional fade to that alpha
; The alpha will be clamped between 0 and 1
Function SetAlpha(float afTargetAlpha, bool abFade = false) native

; Sets this actor to be attacked by all other actors on sight
Function SetAttackActorOnSight(bool abAttackOnSight = true) native


;ANIM ARCHETYPE SWITCHING
;jduvall
Function SetAnimArchetypeConfident()
	ChangeAnimArchetype(Game.GetCommonProperties().AnimArchetypeConfident)
EndFunction

Function SetAnimArchetypeDepressed()
	ChangeAnimArchetype(Game.GetCommonProperties().AnimArchetypeDepressed)
EndFunction

Function SetAnimArchetypeElderly()
	ChangeAnimArchetype(Game.GetCommonProperties().AnimArchetypeElderly)
EndFunction

Function SetAnimArchetypeFriendly()
	ChangeAnimArchetype(Game.GetCommonProperties().AnimArchetypeFriendly)
EndFunction

Function SetAnimArchetypeIrritated()
	ChangeAnimArchetype(Game.GetCommonProperties().AnimArchetypeIrritated)
EndFunction

Function SetAnimArchetypeNeutral()
	ChangeAnimArchetype(None)
EndFunction

Function SetAnimArchetypeNervous()
	ChangeAnimArchetype(Game.GetCommonProperties().AnimArchetypeNervous)
EndFunction

Function SetDogAnimArchetypeAgitated()
	ChangeAnimArchetype(Game.GetCommonProperties().DogAnimArchetypeAgitated)
EndFunction

Function SetDogAnimArchetypeAlert()
	ChangeAnimArchetype(Game.GetCommonProperties().DogAnimArchetypeAlert)
EndFunction

Function SetDogAnimArchetypeNeutral()
	ChangeAnimArchetype(None)
EndFunction

Function SetDogAnimArchetypePlayful()
	ChangeAnimArchetype(Game.GetCommonProperties().DogAnimArchetypePlayful)
EndFunction

; Tells pathing to avoid the player (which it already does by default) or not (for use in scenes where the player is meant to be an incorporeal observer).
Function SetAvoidPlayer(bool abAvoid = true) native

; Sets/Unsets the actor in command mode so that we can order them around.  SetCanDoCommand() needs to have already been called on the actor.
Function SetCommandState(bool abStartCommandMode ) native

; Flags/unflags this actor as bribed by the player
Function SetBribed(bool abBribe = true) native

; Flags/unflags this actor as can be commanded by the player
Function SetCanDoCommand(bool abCanCommand= true) native


; Sets the combatstyle for this actor
Function SetCombatStyle(CombatStyle akCombatStyle) native


function SetCompanion(bool SetCompanion = true, bool FillCompanionAlias = true)
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.SetCompanion(Self, SetCompanion, FillCompanionAlias)

endFunction

Function SetAvailableToBeCompanion()
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.SetAvailableToBeCompanion(Self)
EndFunction

function DisallowCompanion(bool SuppressDismissMessage = false)
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.DisallowCompanion(Self, SuppressDismissMessage)

endFunction

function AllowCompanion(bool MakeCompanionIfNoneCurrently = true, bool ForceCompanion = false)
	FollowersScript Followers = Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as FollowersScript

	Followers.AllowCompanion(Self, MakeCompanionIfNoneCurrently, ForceCompanion)

endFunction


; Sets the faction this actor reports crimes to
Function SetCrimeFaction(Faction akFaction) native

; Sets this actor's critical stage, which is one of the following (properties below also match this)
; 0 - None
; 1 - Goo start
; 2 - Goo end
; 3 - Disintegrate start
; 4 - Disintegrate end
; 5 - Freeze start
; 6 - Freeze end
Function SetCriticalStage(int aiStage) native

; Flag this actor as currently doing a favor for the player
; Indicate if the actor is in workshop or normal command mode
Function SetDoingFavor(bool abDoingFavor = true, bool abWorkShopMode=false) native

; Change this actor's anim face archetype
Function ChangeAnimFaceArchetype(keyword apKeyword = none) native

;forces the eye texture for this actor to the give texture set
Function SetEyeTexture(TextureSet akNewTexture) native

;overrides the Essential status of the actor
Function SetEssential(bool abEssential) native

; Sets this actor's rank with the specified faction
Function SetFactionRank(Faction akFaction, int aiRank) native

; Set a specific marker as the place at which this actor must land from flight.
; params:
; - aMarker:  The ObjectReference to set as this actor's landing marker
Function SetForcedLandingMarker( ObjectReference aMarker ) native

; Flags/unflags this actor as a ghost
Function SetGhost(bool abIsGhost = true) native

; Flags/unflags this actor as requiring a char gen skeleton
Function SetHasCharGenSkeleton(bool abCharGen = true) native

; Adds this actor to a faction at rank 0 if they aren't already in it
Function AddToFaction(Faction akFaction)
	if (!IsInFaction(akFaction))
		SetFactionRank(akFaction, 0)
	endif
EndFunction

; Turns on/off headtracking on this actor
Function SetHeadTracking(bool abEnable = true) native

; Flags/unflags this actor as intimidated by the player
Function SetIntimidated(bool abIntimidate = true) native

; Sets this actor's head tracking target, optionally forcing it as their pathing look-at target
Function SetLookAt(ObjectReference akTarget, bool abPathingLookAt = false) native

; Set the no bleedout recovery flag on this actor
Function SetNoBleedoutRecovery(bool abAllowed) native

; Sets this actor to not effect the detection level on the stealth meter if he is not hostile to the player
Function SetNotShowOnStealthMeter(bool abNotShow) native

; Sets the actors outfit and makes him wear it
Function SetOutfit( Outfit akOutfit, bool abSleepOutfit = false ) native

; Sets the actor's override voice type
Function SetOverrideVoiceType( VoiceType akVoiceType ) native

; Set/reset whether player input being sent to the actor
Function SetPlayerControls(bool abControls) native

; Sets the player as resisting arrest from this actor's faction
Function SetPlayerResistingArrest() native

; Sets or clears this actor as a teammate of the player
; abCanDoFavor - OPTIONAL default is true the teammate can do favors
; abGivePlayerXP - OPTIONAL default is false the teammate will give the player XP for kills and unlocks
Function SetPlayerTeammate(bool abTeammate = true, bool abCanDoFavor=true, bool abGivePlayerXP=false) native

;overrides the Protected status of the actor
Function SetProtected(bool abProtected) native

; Sets the actors race
; akRace - OPTIONAL (Def=None) New race for this actor. Default, no race, to switch back to the original race.
Function SetRace( Race akRace = None ) native

; Sets the relationship rank between this actor and another (See GetRelationshipRank for the ranks)
Function SetRelationshipRank(Actor akOther, int aiRank) native

; Sets this actor as restrained or not - returns false if the state wasn't changed
bool Function SetRestrained(bool abRestrained = true) native

; Set a variable on all of an actor's subgraphs
Function SetSubGraphFloatVariable(string asVariableName, float afValue) native

; Sets this actor as unconscious or not - returns false if the state wasn't changed
bool Function SetUnconscious(bool abUnconscious = true) native

; Attach the actor to a horse or other vehicle.
; akVehicle is the vehicle ref.  To detach the actor from its current vehicle, set akVehicle to None (or to the Actor itself).
Function SetVehicle( Actor akVehicle ) native

; Opens the Barter menu
Function ShowBarterMenu() native

; Starts Cannibal with the target
Function StartCannibal(Actor akTarget) native

; Starts combat with the target
; abPreferredTarget - OPTIONAL (Def=false) True to set the target as the actor's preferred target
Function StartCombat(Actor akTarget, bool abPreferredTarget = false) native

; Start the Deferred Kill state. Be sure to call EndDeferredKill or the actor will be invulnerable.
Function StartDeferredKill() native

; Starts vampire feed with the target
Function StartVampireFeed(Actor akTarget) native

; Starts a frenzey attack target selector.
Function StartFrenzyAttack(float aChance = 0.1, float aInterval = 0.5) native

; Removes this actor from combat
Function StopCombat() native

; Stops all combat and alarms against this actor
Function StopCombatAlarm() native

; Instantly pop an actor into aArmorFurniture without any animation or repositioning
Function SwitchToPowerArmor(ObjectReference aArmorFurniture) native

; (near) Instantly snaps an actor into a furniture or onto a mount
bool Function SnapIntoInteraction(ObjectReference akTarget) native

; Returns whether the actor can trap the soul of the given actor.
bool Function TrapSoul(Actor akTarget) native

; Unequips the all items from this actor
Function UnequipAll() native

; Unequips the specified item from this actor
Function UnequipItem(Form akItem, bool abPreventEquip = false, bool abSilent = false) native

; Unequips the all items in this slot for the actor
Function UnequipItemSlot(int aiSlot) native

; Forces this actor to unequip the specified spell. The casting source can be:
; 0 - Left hand
; 1 - Right hand
Function UnequipSpell(Spell akSpell, int aiSource) native

; This actor will unlock all the doors that he qualifies for ownership in his current parentcell
Function UnLockOwnedDoorsInCell() native

; Returns whether intimidate will succeed against this actor or not
bool Function WillIntimidateSucceed() native

; Returns whether anything the actor is wearing has the specified keyword
bool Function WornHasKeyword(Keyword akKeyword) native

; Returns if actor is stealing taking a ref
bool Function WouldBeStealing(ObjectReference akObject) native

; Obtains the ability to do a command on ref for player
; 0 - will do the command
; 1 - refuse for reasons other then moral/impossible
; 2 - Impossible
; 3 - Morale refusal
; Returns if actor can do command on ref for player
int Function WouldRefuseCommand(ObjectReference akObject) native


; Makes this actor start sneaking
Function StartSneaking() native

; Makes this actor draw his weapon
Function DrawWeapon() native

; Add this object to the dog's mouth
Function DogPlaceInMouth( Form akItem ) native

; Drop the dog's items in the world
Function DogDropItems( ) native

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
Event OnConsciousnessStateChanged( bool abUnconscious )
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

; Event that is triggered when this actor begins to die
Event OnDying(Actor akKiller)
EndEvent

; Event received when an actor enters bleedout.
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

; Recieved immeadiately after the limb of a robot (or other actor whose race allows sub-segment damage) has become (or ceased to be) partially crippled.
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

; Set of read-only properties to essentually make a fake enum for critical stages
int Property CritStage_None = 0 AutoReadOnly
int Property CritStage_GooStart = 1 AutoReadOnly
int Property CritStage_GooEnd = 2 AutoReadOnly
int Property CritStage_DisintegrateStart = 3 AutoReadOnly
int Property CritStage_DisintegrateEnd = 4 AutoReadOnly
int Property CritStage_FreezeStart = 5 AutoReadOnly
int Property CritStage_FreezeEnd = 6 AutoReadOnly

; **** For Debugging Movement Animations (not in release builds) ****
; Forces the movement direction on the actor
; afXAngle, afYAngle and afZAngle are in degrees
Function ForceMovementDirection(float afXAngle = 0.0, float afYAngle = 0.0, float afZAngle = 0.0) native debugOnly

; Forces the movement speed on the actor
; afSpeedMult is a speed multiplier based on the current max speeds
; - 0 -> 1 Scales between 0 and the Walk speed
; - 1 -> 2 Scales between Walk speed and Run Speed
; - 2 and above is a multiplier of the run speed (less 1.0 since Run is 2.0)
Function ForceMovementSpeed(float afSpeedMult) native debugOnly

; Forces the movement rotation speed on the actor
; Each component of the rotation speed is a multiplier following these rules:
; - 0 -> 1 Scales between 0 and the Walk speed
; - 1 -> 2 Scales between Walk speed and Run Speed
; - 2 and above is a multiplier of the run speed (less 1.0 since Run is 2.0)
Function ForceMovementRotationSpeed(float afXMult = 0.0, float afYMult = 0.0, float afZMult = 0.0) native debugOnly

; Ramps the movement direction on the actor to the passed in value over the passed in time
; afXAngle, afYAngle and afZAngle are in degrees
; afRampTime is in seconds
Function ForceMovementDirectionRamp(float afXAngle = 0.0, float afYAngle = 0.0, float afZAngle = 0.0, float afRampTime = 0.1) native debugOnly

; Ramps the movement speed on the actor to the passed in value over the passed in time
; afSpeedMult is a speed multiplier based on the current max speeds
; - 0 -> 1 Scales between 0 and the Walk speed
; - 1 -> 2 Scales between Walk speed and Run Speed
; - 2 and above is a multiplier of the run speed (less 1.0 since Run is 2.0)
; afRampTime is in seconds
Function ForceMovementSpeedRamp(float afSpeedMult, float afRampTime = 0.1) native debugOnly

; Ramps the movement rotation speed on the actor to the passed in value over the passed in time
; Each component of the rotation speed is a multiplier following these rules:
; - 0 -> 1 Scales between 0 and the Walk speed
; - 1 -> 2 Scales between Walk speed and Run Speed
; - 2 and above is a multiplier of the run speed (less 1.0 since Run is 2.0)
; afRampTime is in seconds
Function ForceMovementRotationSpeedRamp(float afXMult = 0.0, float afYMult = 0.0, float afZMult = 0.0, float afRampTime = 0.1) native debugOnly

; Sets the target movement direction on the actor
; afXAngle, afYAngle and afZAngle are in degrees
Function ForceTargetDirection(float afXAngle = 0.0, float afYAngle = 0.0, float afZAngle = 0.0) native debugOnly

; Sets the target movement speed on the actor
; afSpeedMult is a speed multiplier based on the current max speeds
; - 0 -> 1 Scales between 0 and the Walk speed
; - 1 -> 2 Scales between Walk speed and Run Speed
; - 2 and above is a multiplier of the run speed (less 1.0 since Run is 2.0)
Function ForceTargetSpeed(float afSpeed) native debugOnly

; Sets the target facing angle on the actor
; afXAngle, afYAngle and afZAngle are in degrees
Function ForceTargetAngle(float afXAngle = 0.0, float afYAngle = 0.0, float afZAngle = 0.0) native debugOnly

; Clears any forced movement on the actor and return it to its standard state
Function ClearForcedMovement() native debugOnly

; Check to see if the actor can move vertically.
bool Function CanMoveVertical() native debugOnly

; Check to see if the actor can strafe side to side.
bool Function CanStrafe() native debugOnly