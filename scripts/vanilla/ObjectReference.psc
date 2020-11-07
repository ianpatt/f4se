Scriptname ObjectReference extends Form Native Hidden

; convenient way to call Lock(False) on a door
Function Unlock(bool abAsOwner = false)
	Lock(False, abAsOwner)
endFunction

bool FUNCTION rampRumble(float power = 0.5, float duration = 0.25, float falloff = 1600.0)
	; Function to shake cam/controller based on distance from player
	; should always be called on the source of the rumble,
	; as final intensity is relevant to player
	if power > 1.0 || power <= 0
; 		debug.traceStack(self + " called rampRumble() but parameter 'power' was invalid.  Must be a non-zero float less than 1.0",1)
		; throw the warning, but don't return false - value gets clamped anyway
	endif
	float playerDist = game.getplayer().getDistance(self)
	; ignore if the player is too far away
	if playerDist < falloff
		float intensity = (1 - (playerDist / falloff))
		; ramp actual intensity down based on parameter value
		intensity = intensity*power
		if intensity > 1.0
			; clamp to prevent invalid values
; 			debug.traceStack(self + " called for too much controller/camera shake.  Clamped to 1.0", 0)
			intensity = 1.0
		elseif intensity <= 0
			; clamp to prevent invalid values
; 			debug.traceStack(self + " called for too little controller/camera shake", 0)
			intensity = 0
			return false
		endif
		game.shakeCamera(game.getPlayer(), intensity)
		game.shakeController(intensity, intensity, duration)
		return true
	else
; 		debug.traceStack(self + "called for rampedRumble(), but player is too far away", 0)
		return False
	endif
endFUNCTION

; Function to know if I'm near the player (whether I can be safely enabled or disabled)
bool Function IsNearPlayer()
	Actor player = Game.GetPlayer()
	Cell targetCell = self.GetParentCell()
	Cell playerCell = player.GetParentCell()

	if (targetCell != playerCell)
		; player and target are in different cells
		if (targetCell && targetCell.IsInterior() || playerCell && playerCell.IsInterior())
			; in different cells and at least one is an interior
			;  -- we can safely enable or disable
			return false
		else
			; both in an exterior -- no means of testing
			;  worldspace at the moment, so this will do.
			if (player.GetDistance(self) > 3000)
				; pretty darned far away -- safe
				return false
			else
				; too close for comfort
				return true
			endif
		endif
	else
		; in the same cell -- err on the side of caution
		return true
	endif
endFunction

;jduvall
bool Function IsInInterior()
{Returns !IsInExterior()}
  Cell parentCell = GetParentCell()
  Return parentCell && parentCell.IsInterior()
EndFunction

;kkuhlmann:
bool function MoveToIfUnloaded(ObjectReference akTarget, float afXOffset = 0.0, float afYOffset = 0.0, float afZOffset = 0.0)
{Calls MoveTo if the calling ObjectReference is currently unloaded. Doesn't do anything if it IS loaded. No waiting or while loops. Returns true if it does the moveto}
	if !Is3DLoaded()
		MoveTo(akTarget, afXOffset, afYOffset, afZOffset)
		return true
	else
		return false
	endif
endFunction

; Returns if this reference has the specified location ref type (for backwards compatibility with function rename
bool Function HasRefType(LocationRefType akRefType)
	return HasLocRefType( akRefType )
EndFunction

;jburgess
ObjectReference[] Function getLinkedRefArray(keyword keywordLink)
	; Function which finds any linked to-or-from references with a specific keyword, and returns an array of them.
	;debug.trace("|----------------------------------------")
	;debug.trace("| GetLinkedRefArray() debug output")

	; create the array and populate with linked ref children first.
	ObjectReference[] myLinkedRefArray = new objectReference[0]
	myLinkedRefArray = self.getLinkedRefChildren(keywordLink)
	;debug.trace("| Added linkedRef Children to array")

	; Check to see if I have a linked ref
	if self.getLinkedRef(keywordLink)
		if myLinkedRefArray.length == 0
			; create an empty data set for the array if it's NONE (ie there were no linked ref kids above)
			myLinkedRefArray = new ObjectReference[0]
		endif
		myLinkedRefArray.add(self.getLinkedref(keywordLink))

		; now look for daisy-chains from the linked ref.
		if self.getLinkedRef(keywordLink).countLinkedRefChain(keywordLink) > 0
			; my keyword-linked ref has links of its own.  Let's add them all to the array.
			int iChain = 1 ; set to 1, not 0 - we want to skip the first link.  It's already in the array.
			while iChain <= getLinkedRef(keywordLink).countLinkedRefChain(keywordLink)
				;debug.trace("|"+ichain+"/"+self.getLinkedRef(keywordLink).countLinkedRefChain(keywordLink))
				;debug.trace("| array attempting to add "+self.getLinkedRef(keywordLink).GetNthLinkedRef(iChain,keywordLink))
				myLinkedRefArray.add(self.getLinkedRef(keywordLink).GetNthLinkedRef(iChain,keywordLink))
				iChain += 1
			endWhile
		endif
	endif

	;debug.trace("| Preparing to return array")
	;debug.trace("| myLinkedRefArray.Length = "+myLinkedRefArray.length)
	;debug.trace("| myLinkedRefArray: "+myLinkedRefArray)

	return myLinkedRefArray

endFunction


;jduvall:
function MoveToWhenUnloaded(ObjectReference akTarget, float afXOffset = 0.0, float afYOffset = 0.0, float afZOffset = 0.0)
{DEPRECATED: DO NOT USE. Calls MoveTo if both the calling ObjectReference and the akTarget ObjectReference have current locations that are not loaded.}
	while self.GetCurrentLocation().IsLoaded() || akTarget.GetCurrentLocation().IsLoaded()
		;do nothing
; 		debug.trace(self + "MoveToWhenUnloaded() waiting for current location and target location to be unloaded before moving. If called by a quest stage fragment, this may cause that quest stage to not complete until this function finishes (and if it's a startup stage, the quest will not report itself as running until the stage finishes.).", 1)
		Utility.Wait(5)	;when this function is threaded we can increase this wait time... I set it lower for testing purposes so it reevaluates faster when I need to purge cell buffers in the Civil War when calling moveto on the player between Civil War campaigns
	EndWhile
	self.MoveTo(akTarget, afXOffset, afYOffset, afZOffset)
EndFunction

;jduvall
Function DeleteWhenAble()
{This will become a native function... it will wait until the object is not persisting, then delete itself.}
	While GetParentCell() && GetParentCell().IsAttached()
		;do nothing
; 		debug.trace(self + "DeleteWhenAble() waiting for current location to be unloaded before deleting. If called by a quest stage fragment, this may cause that quest stage to not complete until this function finishes (and if it's a startup stage, the quest will not report itself as running until the stage finishes.).", 1)
		Utility.Wait(5) ;when this function is threaded we can increase this wait time... I set it lower for testing purposes so it reevaluates faster when I need to purge cell buffers in the Civil War when calling moveto on the player between Civil War campaigns
	EndWhile
	Delete()
EndFunction



;jduvall
Function AddKeyIfNeeded(ObjectReference ObjectWithNeededKey)
{Should only be called by ObjectReferences that have/are containers (ie Containers and Actors). Checks to see if self has the key to ObjectWithNeededKey, and if not, creates a copy of the key and puts it in self.}
	key NeededKey = ObjectWithNeededKey.GetKey()
	if NeededKey != None
		if GetItemCount(NeededKey) == 0
			AddItem(NeededKey)
		EndIf
	EndIf
EndFunction


; Property to obtain the current X position of the object
float Property X
  float Function get()
    return GetPositionX()
  EndFunction
EndProperty

; Property to obtain the current Y position of the object
float Property Y
  float Function get()
    return GetPositionY()
  EndFunction
EndProperty

; Property to obtain the current Z position of the object
float Property Z
  float Function get()
    return GetPositionZ()
  EndFunction
EndProperty

; Have akActivator activate this reference. If abDefaultProcessingOnly is true then any block will be bypassed
; and no OnActivate event will be sent. The function returns true if default processing ran, and succeeded. If
; default processing has been blocked, will always return false.
bool Function Activate(ObjectReference akActivator, bool abDefaultProcessingOnly = false) native

; Sets up a dependent animated object
; This function should be used only with a coder supervision.  It is left undocumented because it can cause dangling pointers as well as very broken functionality
; for the dependent object if used improperly.
bool Function AddDependentAnimatedObjectReference( ObjectReference akDependent ) native

; Adds the specified base object or object reference to this object reference's container/inventory
; Note that you cannot add more then one copy of a reference to a container (a warning will be printed if you try)
Function AddItem(Form akItemToAdd, int aiCount = 1, bool abSilent = false) native

; Adds a keyword to this reference
Function AddKeyword(Keyword apKeyword) native

; Adds the given text replacement pair to this ref
Function AddTextReplacementData(string asTokenLabel, Form akForm) native

; Adds this reference (which is a map marker) to the map, optionally making it available for fast travel
Function AddToMap(bool abAllowFastTravel = false) native

; Add conveyor belt to this reference
Function ApplyConveyorBelt( string aTarget, float aLinVelX, float aLinVelY, float aLinVelZ, bool abOn = true, bool abReverse = false ) native

; Apply an impulse to this reference
Function ApplyHavokImpulse(float afX, float afY, float afZ, float afMagnitude) native

; Add fan motor to this reference
Function ApplyFanMotor( string aTarget, float aAxisX, float aAxisY, float aAxisZ, float aForce, bool abOn = true) native

; Set fan motor on.
Function FanMotorOn( bool abOn = true) native

; Check if fan motor on.
bool Function IsFanMotorOn(  ) native

; Attach a mod to this reference
bool Function AttachMod(ObjectMod akMod, int aiAttachIndex = 0) native

; Attach a mod to an inventory item in this reference's inventory
bool Function AttachModToInventoryItem(Form akItem, ObjectMod akMod) native

; Attach this reference to the specified parent (only works on created refs)
Function AttachTo(ObjectReference akParent) native

; Turns on and off blocking of normal activation - OnActivate events will still be sent
Function BlockActivation(bool abBlocked = true, bool abHideActivateText = false) native

; Calculate's this references encounter level based on the requested difficulty level
; 0 - Easy
; 1 - Medium
; 2 - Hard
; 3 - Very Hard
; 4 - None
int Function CalculateEncounterLevel(int aiDifficulty = 4) native

; Can the map marker be fast traveled to?
bool Function CanFastTravelToMarker() native

; Can the reference produce resources for a Workshop?
bool Function CanProduceForWorkshop() native

; Clears all effects of destruction from this object
Function ClearDestruction() native

; Clears this object from data on all invalid locations
Function ClearFromOldLocations() native

; Set conveyor belt on.
Function ConveyorBeltOn( bool abOn = true ) native

; Get the count of all instantiated Actors (across all process levels) that link to this reference. 
; The actor must link to this ref with a keyworded linkedref using the keyword (apLinkKeyword).
; Can optionally specify another keyword (apExcludeKeyword); actors with a linkedref using that keyword will be skipped when building the array.
; (NOTE: if you're also necessarily going to call GetActorsLinkedToMe, just do that and check its size, instead of calling this.)
int Function CountActorsLinkedToMe( Keyword apLinkKeyword = None, Keyword apExcludeKeyword = None ) native

;This function  counts the number of linked refs that are in a linked Ref chain (ie object is linked to A, A is linked to B, etc. this then counts all the linked refs.)
;Aborts early if it detects a loop
int Function CountLinkedRefChain(keyword apKeyword = None, int maxExpectedLinkedRefs = 100)	native

; Get the count of all instantiated ObjectReferences that link to this reference. 
; They must link to this ref with a keyworded linkedref, using the keyword (apLinkKeyword).
; Can optionally specify another keyword (apExcludeKeyword); refs with a linkedref using that keyword will be skipped when building the array.
; (NOTE: if you're also necessarily going to call GetRefsLinkedToMe, just do that and check its size, instead of calling this.)
int Function CountRefsLinkedToMe( Keyword apLinkKeyword = None, Keyword apExcludeKeyword = None ) native

; Create a detection event at this reference, with the specified owner. Sound level is between 0 and 100
Function CreateDetectionEvent(Actor akOwner, int aiSoundLevel = 0 ) native

; Damages this object and advances the destruction stage - does not return until the object is damaged
Function DamageObject(float afDamage) native

; Damages the given actor value
Function DamageValue(ActorValue akAV, float afDamage) native

; Delets this object
Function Delete() native

; Disables this object - fading out if requested
Function Disable(bool abFadeOut = false) native

; Disables all of the references that are linked, in a chain, to this one. (But NOT this one)
; Does not wait for everything to disable before returning
Function DisableLinkChain(Keyword apKeyword = None, bool abFadeOut = false) native

; Disables this object - fading out if requested. Does NOT wait for the fade or disable to finish
Function DisableNoWait(bool abFadeOut = false) native

; Drops this reference from the container it is in. If silent, will not display a message to the
; player if it was in the player's inventory
Function Drop(bool abSilent = false) native

; Drops the first object from this object's inventory
ObjectReference Function DropFirstObject(bool abInitiallyDisabled = false) native

; Drops the specified object from this object's inventory (will not drop references)
ObjectReference Function DropObject(Form akObject, int aiCount = 1) native

; Enables this object - fading in if requested
Function Enable(bool abFadeIn = false) native

; Enables the ability to fast travel to this marker - or disables it. Note that if you disable
; fast travel the player will see "You haven't discovered this location" as an error message
Function EnableFastTravel(bool abEnable = true) native

; Enables all of the references that are linked, in a chain, to this one. (But NOT this one)
; Does not wait for everything to enable before returning
Function EnableLinkChain(Keyword apKeyword = None, bool abFadeIn = false) native

; Enables this object - fading in if requested. Does NOT wait for the fade or enable to finish
Function EnableNoWait(bool abFadeIn = false) native

; Finds all references to objects in the given list in the loaded area within the given radius of the calling
; ref and returns an array of them
ObjectReference[] Function FindAllReferencesOfType(Form akObjectOrList, float afRadius) native

; Finds all references with the given keyword(s) in the loaded area within the given radius of the calling
; ref and returns an array of them
ObjectReference[] Function FindAllReferencesWithKeyword(Form akKeywordOrList, float afRadius) native

; Forcibly adds / removes the ragdoll for a reference to the world
Function ForceAddRagdollToWorld() native
Function ForceRemoveRagdollFromWorld() native

; Gets the ActorBase that owns this object (or None if not owned by an ActorBase)
; CAUTION: if owned by an Actor Ref, this will return the ActorBase of that Actor ref.
ActorBase Function GetActorOwner() native

; Gets the Actor (reference) that owns this object (or None if not owned by an Actor)
Actor Function GetActorRefOwner() native

; Get an array of all instantiated Actors (across all process levels) that link to this reference. 
; The actor must link to this ref with a keyworded linkedref using the keyword (apLinkKeyword).
; Can optionally specify another keyword (apExcludeKeyword); actors with a linkedref using that keyword will be skipped when building the array.
Actor[] Function GetActorsLinkedToMe( Keyword apLinkKeyword = None, Keyword apExcludeKeyword = None ) native

; Get the current X angle of this object
float Function GetAngleX() native

; Get the current Y angle of this object
float Function GetAngleY() native

; Get the current Z angle of this object
float Function GetAngleZ() native

; Get a variable from the reference's animation graph (if applicable). Bool version.
bool Function GetAnimationVariableBool(string arVariableName) native

; Get a variable from the reference's animation graph (if applicable). Int version.
int Function GetAnimationVariableInt(string arVariableName) native

; Get a variable from the reference's animation graph (if applicable). Float version.
float Function GetAnimationVariableFloat(string arVariableName) native

; Returns the base object this reference represents
Form Function GetBaseObject() native

; Gets the base value of the specified actor value - returns 0 and logs an error if the value is unknown
float Function GetBaseValue(ActorValue akAV) native

; Returns the object's current destruction stage
int Function GetCurrentDestructionStage() native

; Returns this reference's current location
Location Function GetCurrentLocation() native

; Returns the scene this reference is currently in - if any
Scene Function GetCurrentScene() native

; Calculates the distance between this reference and another - both must either be in the same interior, or same worldspace
float Function GetDistance(ObjectReference akOther) native

; Returns this reference's editor location
Location Function GetEditorLocation() native

; Gets the EncounterZone to which this object belongs (or None if none).
EncounterZone Function GetEncounterZone() native

; Gets the faction that owns this object (or None if not owned by a Faction)
Faction Function GetFactionOwner() native

; Gets the angle between this object's heading and the other object in degrees - in the range from -180 to 180
float Function GetHeadingAngle(ObjectReference akOther) native

; Get the current height of the object
float Function GetHeight() native

; Obtains the value of all objects in this reference's inventory
int Function GetInventoryValue() native

; Returns how many of the specified item is in this object reference's inventory
; If None is passed, it counts every item in the inventory
int Function GetItemCount(Form akItem = None) native

; Returns how many of the specified components are in this object reference's inventory
; If None is passed, it counts every component in the inventory
int Function GetComponentCount(Form akItem = None) native

; Returns the container this reference is in - if any
ObjectReference Function GetContainer() native

; Returns the smithed health of this object reference (1.0 == 100%)
float Function GetItemHealthPercent() native

; Returns the key base object that will unlock this object
Key Function GetKey() native

; Get the current length of the object
float Function GetLength() native

; Get our linked reference
ObjectReference Function GetLinkedRef(Keyword apKeyword = NONE) native


; Returns a chain of linked refs as an array,
; Aborts early if it detects a loop
ObjectReference[] Function GetLinkedRefChain(keyword apKeyword = None, int iMaxExpectedLinkedRefs = 100) native

; Returns an array of linked reference children which were linked using the given keyword
ObjectReference[] Function GetLinkedRefChildren(keyword apKeyword) native

; Get the level of the lock on this object
int Function GetLockLevel() native

; Get the location ref types for this object
LocationRefType[] Function GetLocRefTypes() native

; Returns the Nth linked ref from this reference (0 = self, 1 = GetLinkedRef, 2 = GetLinkedRef.GetLinkedRef, etc)
ObjectReference Function GetNthLinkedRef(int aiLinkedRef, Keyword apKeyword = None) native

; Get this object's mass
float Function GetMass() native

; Gets the open state of this object. Which can be one of the following:
; 0 - None
; 1 - Open
; 2 - Opening
; 3 - Closed
; 4 - Closing
int Function GetOpenState() native

; Gets the cell this object is in
Cell Function GetParentCell() native

; Get the current X position of the object
float Function GetPositionX() native

; Get the current Y position of the object
float Function GetPositionY() native

; Get the current Z position of the object
float Function GetPositionZ() native

; Get the frequency of this radio receiver (if it is a receiver)
float Function GetRadioFrequency() native

; Get the volume of this radio receiver (if it is a receiver)
float Function GetRadioVolume() native

; Get the total damage, if any, of the given resource or of all nonzero resource actor values.
float Function GetResourceDamage(ActorValue akValue=None) native

; Gets an array of the x,y,z of a safeRadius radius location within searchRadius of the reference, returns empty array if it fails.
float[] Function GetSafePosition(float aSearchRadius = -1.0, float aSafeRadius = -1.0 ) native

; Get an array of all instantiated refs that link to this reference. 
; They must link to this ref with a keyworded linkedref using the keyword (apLinkKeyword).
; Can optionally specify another keyword (apExcludeKeyword); refs with a linkedref using that keyword will be skipped when building the array.
ObjectReference[] Function GetRefsLinkedToMe( Keyword apLinkKeyword = None, Keyword apExcludeKeyword = None ) native

; Get all of the workshop objects on this Workbench owned by the given Actor.
ObjectReference[] Function GetWorkshopOwnedObjects(Actor akActor) native

; Get the total damage, if any, of the given resource on the Workshop.
float Function GetWorkshopResourceDamage(ActorValue akValue) native

; Get all of the resource-producing objects owned by this Workbench. If akAV is provided,
; only collect objects producing the corresponding resource. If aiOption is provided, use 0 for all nonzero resources, 1 for only damaged resources, 2 for only undamaged resources.
ObjectReference[] Function GetWorkshopResourceObjects(ActorValue akAV = None, int aiOption = 0) native

; Get the current scale of the object
float Function GetScale() native

; Get the target cell for this teleport ref
Cell Function GetTeleportCell() native

; Get the transition cell for this teleport ref
Cell Function GetTransitionCell() native

; Get the distance between this radio transmitter and the player
float Function GetTransmitterDistance() native

; Get the number of objects inside this trigger (throws warning if not a trigger)
int Function GetTriggerObjectCount() native

; Gets the specified actor value - returns 0 and logs an error if the value is unknown
float Function GetValue(ActorValue akAV) native

; Gets the specified actor value as a percentage of its max value - from 0 to 1
float Function GetValuePercentage(ActorValue akAV) native

; Gets the voice type for this reference. Will return None if not an actor or a talking activator
VoiceType Function GetVoiceType() native

; Get the current width of the object
float Function GetWidth() native

; Get this objects worldspace
WorldSpace Function GetWorldSpace() native

; Returns self cast as an actor
actor Function GetSelfAsActor()
	return self as Actor
endFunction

; Returns true if there was an Actor (reference) set to own this object (even if that actor was a temp and is now gone).
bool Function HasActorRefOwner() native

; Checks to see if there is a direct line-of-sight between us and the target, using the specified nodes
; If the node strings are empty, the check will be done from the roots of the objects
bool Function HasDirectLOS(ObjectReference akTarget, string asSourceNode = "", string asTargetNode = "") native

; Returns if this reference has an active effect coming from a magic effect with the specified keyword attached
bool Function HasEffectKeyword(Keyword akKeyword) native

; Returns true if this reference has the specified keyword
bool Function HasKeyword(Keyword apKeyword) native

; Returns if this reference has any of the keywords in the list attached
bool Function HasKeywordInFormList(FormList akKeywordList) native

; Returns if this reference has the specified location ref type
bool Function HasLocRefType(LocationRefType akRefType) native

; Returns whether the reference has the given node
bool Function HasNode(string asNodeName) native

; kmk - utility function - is this ref owned in any way?
bool Function HasOwner()
	bool returnVal = false
	if HasActorRefOwner() || GetActorOwner() || GetFactionOwner()
		returnVal = true
	endif
;	debug.trace(self + " HasOwner=" + returnVal + ": HasActorRefOwner()=" + HasActorRefOwner() + ", GetActorOwner()=" + GetActorOwner() + ", GetFactionOwner()=" + GetFactionOwner())
	return returnVal
endFunction

; Returns true if this reference is on the same power grid as the compare reference.
bool Function HasSharedPowerGrid(ObjectReference akCompare) native

; Flags this reference as ignoring (or not ignoring) friendly hits
Function IgnoreFriendlyHits(bool abIgnore = true) native

; Interrupts any spell-casting this object may be doing
Function InterruptCast() native

; Checks to see if the passed in reference is the activate child of this one
bool Function IsActivateChild(ObjectReference akChild) native

; Checks to see if activation is currently blocked on this object
bool Function IsActivationBlocked() native

; Returns if the 3d for this object is loaded or not
bool Function Is3DLoaded() native

; Check if conveyor belt on.
bool Function IsConveyorBeltOn( ) native

; Was this object created in-game?
bool Function IsCreated() native

; Is this object currently flagged for delete?
bool Function IsDeleted() native

; Is this object destroyed?
bool Function IsDestroyed() native

; Is this object currently disabled?
bool Function IsDisabled() native

; Because Shane got tired of remembering which way to call this
bool Function IsEnabled()
	return !IsDisabled()
EndFunction

; Is any marker on this furniture in use?
bool Function IsFurnitureInUse(bool abIgnoreReserved = false) native

; Is a particular marker on this furniture in use?
bool Function IsFurnitureMarkerInUse(int aiMarker, bool abIgnoreReserved = false) native

; Is this object ignoring friendly hits?
bool Function IsIgnoringFriendlyHits() native

; Is this actor or talking activator currently talking to the player?
bool Function IsInDialogueWithPlayer() native

; Is the lock on this object broken?
bool Function IsLockBroken() native

; Is the lock on this object locked?
bool Function IsLocked() native

; Is the map marker visible?
bool Function IsMapMarkerVisible() native

; Is this ref owned by the given actor?
bool Function IsOwnedBy(Actor akOwner) native

; Returns if the workshop object is considered powered
bool Function IsPowered() native

; Returns if this is a quest item or not
bool Function IsQuestItem() native

; Returns if this radio receiver is on or not
bool Function IsRadioOn() native

; Returns if the given ref is inside this teleport ref's transition cell
bool Function IsRefInTransitionCell(ObjectReference akRef) native

; Returns if the given ref's teleport area is fully loaded or not
bool Function IsTeleportAreaLoaded() native

; Returns if this reference is within the given Workshop activator's buildable area
bool Function IsWithinBuildableArea(ObjectReference akRef) native

; Executes a knock effect to an area
Function KnockAreaEffect(float afMagnitude, float afRadius) native

; Lock/unlock this object. If told to lock it, it will add a lock if it doesn't have one. If locked/unlocked as the owner on a door,
; the adjoining cell will be made public/private as appropriate
Function Lock(bool abLock = true, bool abAsOwner = false) native

; Make this object a radio receiver, tuned to the given frequency.  Optionally specify a volume [0.0-1.0], an override OutputModel,
; or override the default on/off state (on).
Function MakeRadioReceiver(float afFrequency, float afVolume = 1.0, OutputModel aOverrideModel = None, bool abActive = true, bool abNoStatic = false) native

; Make this object a radio transmitter which will repeat all transmissions sent by akTransmitterToRepeat.
Function MakeTransmitterRepeater(ObjectReference akTransmitterToRepeat, float afInnerRadius, float afOuterRadius, bool abUnlimitedRange = false) native

; Modifies the specified actor value
Function ModValue(ActorValue akAV, float afAmount) native

; Moves this object to the position of the specified object, with an offset, and optionally matching its rotation
Function MoveTo(ObjectReference akTarget, float afXOffset = 0.0, float afYOffset = 0.0, float afZOffset = 0.0, bool abMatchRotation = true) native

; Moves this object to its editor location
Function MoveToMyEditorLocation() native

; Moves this object to the nearest location on a Navmesh
Function MoveToNearestNavmeshLocation() native

; Moves this object (or its specified node if asMatchNodeName is valid) to the position (and rotation) of the specified node on the specified object's 3D
Function MoveToNode(ObjectReference akTarget, string asNodeName, string asMatchNodeName = "") native

; If this is a workshop NPC, opens up the Workshop Caravan submenu, which allows you to send this NPC to another workshop settlement.
; Returns selected location after user has chosen one, or NONE if user exited out without choosing
Location Function OpenWorkshopSettlementMenu(Keyword akActionKW, Message astrConfirm = None, Location aLocToHighlight = None) native

; If this is a workshop NPC, opens up the Workshop Caravan submenu, which allows you to send this NPC to another workshop settlement.
; Returns selected location after user has chosen one, or NONE if user exited out without choosing
Location Function OpenWorkshopSettlementMenuEx(Keyword akActionKW, Message astrConfirm = None, Location aLocToHighlight = None, FormList akIncludeKeywordList = None, FormList akExcludeKeywordList = None, bool abExcludeZeroPopulation = false, bool abOnlyOwnedWorkshops = true, bool abTurnOffHeader = false, bool abOnlyPotentialVassalSettlements = false, bool abDisableReservedByQuests = false) native

; Check to see if the request to open the workshop settlement menu will result in a valid list.  Parameters mirror OpenWorkshopSettlementMenuEx
; Returns true if at least one workshop qualifies.
int Function RaidTargetsAvailable(Keyword akActionKW, Message astrConfirm = None, Location aLocToHighlight = None, FormList akIncludeKeywordList = None, FormList akExcludeKeywordList = None, bool abExcludeZeroPopulation = false, bool abOnlyOwnedWorkshops = true, bool abTurnOffHeader = false, bool abOnlyPotentialVassalSettlements = false, bool abDisableReservedByQuests = false) native

; Create x copies of the passed in form (forcing them to persist if desired) and place them at our location, returning the last object created
ObjectReference Function PlaceAtMe(Form akFormToPlace, int aiCount = 1, bool abForcePersist = false, bool abInitiallyDisabled = false, bool abDeleteWhenAble = true) native

; Create an actor at this object's location. Level mod is one of the following:
; 0 - Easy
; 1 - Medium
; 2 - Hard
; 3 - Boss
; 4 - None
Actor Function PlaceActorAtMe(ActorBase akActorToPlace, int aiLevelMod = 4, EncounterZone akZone = None) native

; Create x copies of the passed in form (forcing them to persist if desired) and place them at the location of the given node, returning the last object created
ObjectReference Function PlaceAtNode(string asNodeName, Form akFormToPlace, int aiCount = 1, bool abForcePersist = false, bool abInitiallyDisabled = false, bool abDeleteWhenAble = true, bool abAttach = false) native

; Start the specified animation playing - returns true if it succeeds
bool Function PlayAnimation(string asAnimation) native

; Start the specified animation playing and wait for the specified event - returns true if succeeds
bool Function PlayAnimationAndWait(string asAnimation, string asEventName) native

; Start the specified Gamebryo animation playing - returns true if it succeeds
bool Function PlayGamebryoAnimation(string asAnimation, bool abStartOver = false, float afEaseInTime = 0.0) native

; Play the specified impact effect - returns true if it succeeds
bool Function PlayImpactEffect(ImpactDataSet akImpactEffect, string asNodeName = "", float afPickDirX = 0.0, float afPickDirY = 0.0, float afPickDirZ = -1.0, float afPickLength = 512.0, bool abApplyNodeRotation = false, bool abUseNodeLocalRotation = false) native

; Play two animations at once - one on this object, one on another object
bool Function PlaySyncedAnimationSS(string asAnimation1, ObjectReference akObj2, string asAnimation2) native

; Play two animations at once - one on this object, one on another object - and wait for both
bool Function PlaySyncedAnimationAndWaitSS(string asAnimation1, string asEvent1, ObjectReference akObj2, string asAnimation2, string asEvent2) native

; Play a terrain effect that is attached to the specified bone of this object.
Function PlayTerrainEffect(string asEffectModelName, string asAttachBoneName) native

; Pause the audio file for a ref in a scene
Function PauseAudio( ) native

; Preload only the exterior cell containing this reference
Function PreloadExteriorCell() native

; Preload the target interior or exterior area for this reference
Function PreloadTargetArea() native

; Tells this object to process a trap hitting it
Function ProcessTrapHit(ObjectReference akTrap, float afDamage, float afPushback, float afXVel, float afYVel, float afZVel, float afXPos, float afYPos, float afZPos, int aeMaterial, float afStagger) native

; Pushes the passed-in actor away from this object, using the passed in knockback force to determine the speed
Function PushActorAway(Actor akActorToPush, float aiKnockbackForce) native

; Recalculate Resources if this reference is a Workshop
Function RecalculateResources() native

; Removes all items from this container, transferring it to the other object if passed
Function RemoveAllItems(ObjectReference akTransferTo = None, bool abKeepOwnership = false) native

; Removes the specified count of component from the container, scrapping items, and returning the remainder to said container
Function RemoveComponents(Component akComponent, int aiCount, bool abSilent = false) native

; Removes the specified item from this object reference's inventory
Function RemoveItem(Form akItemToRemove, int aiCount = 1, bool abSilent = false, ObjectReference akOtherContainer = None) native

; Removes items from this object reference's inventory containing up to aiCount components
Function RemoveItemByComponent(Form akComponentToRemove, int aiCount = 1, bool abSilent = false, ObjectReference akOtherContainer = None) native

; Removes a keyword from this reference
Function RemoveKeyword(Keyword apKeyword) native

; Removes every single mod attached to this reference (warning, for some actors/items this will leave no rendering geometry at all!)
Function RemoveAllMods() native

; Removes every single mod attach to an item in this reference's inventory. Note that some items won't have any geometry if you do this (handmade guns)
Function RemoveAllModsFromInventoryItem(Form akItem) native

; Remove the requested mod from the reference
Function RemoveMod(ObjectMod akMod) native

; Remove the requested mod from an item in this reference's inventory
Function RemoveModFromInventoryItem(Form akItem, ObjectMod akMod) native

; Removes a previously added dependent object
; This function should be used only with a coder supervision.  It is left undocumented because it can cause dangling pointers as well as very broken functionality
; for the dependent object if used improperly.
bool Function RemoveDependentAnimatedObjectReference( ObjectReference akDependent ) native

; Repair this object.
Function Repair() native

; Resets this object, optional place the object at the new target
Function Reset(ObjectReference akTarget = None) native

; Resets a keyword's add/remove status so the data comes from the base object and aliases
Function ResetKeyword(Keyword apKeyword) native

; Restores damage done to the actor value(up to 0 damage)
Function RestoreValue(ActorValue akAV, float afAmount) native

; Resume the audio file for a ref in a scene
Function ResumeAudio( ) native

; Set conveyor belt reverse state.
Function ReverseConveyorBelt( bool abReverse = true ) native

; Has this object "say" the specified topic, as if spoken by the specified actor (if one is
; provided), and potentially "speaking" in the player's head.
Function Say(Topic akTopicToSay, Actor akActorToSpeakAs = None, bool abSpeakInPlayersHead = false, ObjectReference akTarget = None) native

; Has this object "say" a keyworded topic, as if spoken by the specified actor (if one is
; provided), and potentially "speaking" in the player's head.
Function SayCustom(Keyword akKeywordToSay, Actor akActorToSpeakAs = None, bool abSpeakInPlayersHead = false, ObjectReference akTarget = None) native

;jduvall
;Sell item removes items from the calling container, and adds a payment to that container
;it returns the number of items sold
;optionally you can set:
;whether the add/remove messages show up (silent) - used when the player is the container
;what the payment item should be if not caps001 (paymentItem)
;what container to put the payment in if not the calling container (paymentcontainer)
;typical use: game.GetPlayer().SellItem(itemToSell, 5)
int Function SellItem(Form Item, int Value, int amountToSell = -1, bool silent = false, form paymentItem = none, objectReference PaymentContainer = None)
	;Return value:
	;number of items sold

	;DEFAULTS:
	;PaymentContainer  = calling object
	;amountToSell = all items
	;paymentItem  = caps001


	int AllItemsInContainer = GetItemCount(Item)
	int payment
	int amountToRemove

	If AllItemsInContainer <= 0
		debug.traceStack("WARNING: ObjectReference.SellItem() can't sell because " + self + " has 0 " + Item, 1)
		RETURN 0
	Endif

	if paymentItem == NONE
		paymentItem = Game.GetFormFromFile(0x0000000F, "Fallout4.esm") ;caps001
	endif

	if PaymentContainer == NONE
		PaymentContainer = self
	endif

	If amountToSell < 0
		amountToRemove = AllItemsInContainer
	else
		if amountToSell > AllItemsInContainer 	;just in case someone didn't check before calling the function
			amountToSell = AllItemsInContainer
		endif
		amountToRemove = amountToSell
	Endif

	payment = amountToRemove * value
	RemoveItem(Item, amountToRemove, abSilent = silent)
	PaymentContainer.additem(paymentItem, payment, abSilent = silent)

	RETURN amountToRemove

EndFunction


; Has this object behave as if the specified actor attempted to steal it
Function SendStealAlarm(Actor akThief) native

; Sets this object's activate text to be the full name of the given message
Function SetActivateTextOverride(Message akText) native

; Sets this object's actor cause to the specified actor
Function SetActorCause(Actor akActor) native

; Sets this object's owner to the specified actor base - None means to remove ownership
Function SetActorOwner(ActorBase akActorBase, bool abNoCrime = false) native

; Sets this object's owner to the specified actor ref - None means to remove ownership
Function SetActorRefOwner(Actor akActor, bool abNoCrime = false) native

; Set the orientation of the object (angles are in degrees)
Function SetAngle(float afXAngle, float afYAngle, float afZAngle) native

; Set a variable on the reference's animation graph (if applicable). Bool version.
Function SetAnimationVariableBool(string arVariableName, bool abNewValue) native

; Set a variable on the reference's animation graph (if applicable). Int version.
Function SetAnimationVariableInt(string arVariableName, int aiNewValue) native

; Set a variable on the reference's animation graph (if applicable). Float version.
Function SetAnimationVariableFloat(string arVariableName, float afNewValue) native

; Sets whether this ref should send attraction-object story manager events for the specified keyword.
Function SetAttractionActive(Keyword apKeyword, bool abActive = true) native

; Set linear velocity for conveyor belt.
Function SetConveyorBeltVelocity( float afLinVelX, float afLinVelY, float afLinVelZ ) native

; Sets this object as destroyed or not
Function SetDestroyed(bool abDestroyed = true) native

; Sets the direct-at target for this reference (or clears it if None is passed)
; Note that the object must have an animation graph with a direct-at modifier on it for this to do anything
Function SetDirectAtTarget(ObjectReference akTarget) native

; Sets this object's owner to the specified faction
Function SetFactionOwner(Faction akFaction, bool abNoCrime = false) native

; Marks the object as Harvested or not Harvested (if it's Flora)
Function SetHarvested(bool abHarvested) native

; Set our linked reference
Function SetLinkedRef(ObjectReference akLinkedRef, Keyword apKeyword = NONE) native

; Sets the lock level on this object. Will add an unlocked lock to it if it doesn't have one
Function SetLockLevel(int aiLockLevel) native

; Sets the an in-game created reference to have the given loc ref type for the given location
Function SetLocRefType(Location akLoc, LocationRefType akRefType ) native

; Sets the motion type of the reference
; aeMotionType: The type of motion (see properties at end of file)
; abAllowActivate: When setting to a dynamic type, allows the simulation to be activated
Function SetMotionType(int aeMotionType, bool abAllowActivate = true) native

; Sets this object reference as one that teammates will refuse to do favors on
Function SetNoFavorAllowed(bool abNoFavor = true) native

; Opens/closes this object
Function SetOpen(bool abOpen = true) native

; Sets the persist location on an in-game created reference
Function SetPersistLoc(Location akLoc) native

; Flags/unflags this reference as having been taken by the player
Function SetPlayerHasTaken(bool abTaken = true) native

; Set the position of the object
Function SetPosition(float afX, float afY, float afZ) native

; Turns on/off this radio receiver
Function SetRadioOn(bool abOn = true) native

; Sets the frequency of this radio receiver
Function SetRadioFrequency(float afFrequency) native

; Sets the volume of this radio receiver
Function SetRadioVolume(float afVolume) native

; Set the current scale of the object
Function SetScale(float afScale) native

; Sets the specified actor value
Function SetValue(ActorValue akAV, float afValue) native

; Makes the reference translate to the given position/orientation
; Note: Rotation speed is entirely dependent on the length of the path and the movement speed
; that is, the rotation will happen such that the reference reaches the goal orientation at the end
; of the translation.
Function TranslateTo(float afX, float afY, float afZ, float afXAngle, float afYAngle, float afZAngle, float afSpeed, float afMaxRotationSpeed = 0.0) native

; Makes the reference translate to the given position/orientation on a spline
Function SplineTranslateTo(float afX, float afY, float afZ, float afXAngle, float afYAngle, float afZAngle, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed = 0.0) native

; Makes the reference translate to the target node's ref/orient on a spline at the given speed
Function SplineTranslateToRefNode(ObjectReference arTarget, string arNodeName, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed = 0.0) native

; Starts/Stops Workshop mode with the given workshop ref
Function StartWorkshop(bool abStart = true) native

; Stops the reference from moving
Function StopTranslation() native

; If the reference is a Workshop, stores the given object with an optional count.
Function StoreInWorkshop(Form akBaseItem, int aiCount=1) native

; Makes the reference translate to the target ref position/orient at the given speed
Function TranslateToRef(ObjectReference arTarget, float afSpeed, float afMaxRotationSpeed = 0.0)
	TranslateTo(arTarget.X, arTarget.Y, arTarget.Z, arTarget.GetAngleX(), arTarget.GetAngleY(), arTarget.GetAngleZ(), afSpeed, afMaxRotationSpeed)
endFunction

; Makes the reference translate to the target ref position/orient on a spline at the given speed
Function SplineTranslateToRef(ObjectReference arTarget, float afTangentMagnitude, float afSpeed, float afMaxRotationSpeed = 0.0)
	SplineTranslateTo(arTarget.X, arTarget.Y, arTarget.Z, arTarget.GetAngleX(), arTarget.GetAngleY(), arTarget.GetAngleZ(), afTangentMagnitude, afSpeed, afMaxRotationSpeed)
endFunction

; Tether a prisoner cart to the given horse.
Function TetherToHorse(ObjectReference akHorse) native

; Waits for the animation graph to send the specified event
bool Function WaitForAnimationEvent(string asEventName) native

; Waits for this object's 3d to load. Returns true if the 3D loaded, or false if it was disabled, the parent cell was unloaded
; or something else is preventing it from loading its 3D
bool Function WaitFor3DLoad() native

; Waits for workshop resource recalculation on this workshop to finish - returns immediately if this is not a workshop or
; if no recalc is pending or in progress
Function WaitForWorkshopResourceRecalc() native

; Convenience function to check if I'm in a location or any of its children
bool Function IsInLocation(Location akLocation)
	; cache current location to avoid changing location while this function is running (surprisingly that seems to be happening occasionally)
	Location currLoc = GetCurrentLocation()
	if currLoc == None
		return false
	else
		return akLocation.IsChild(currLoc) || currLoc == akLocation
	endif
endFunction

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
Event OnPlayerDialogueTarget( )
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

; Event received when a workshop item has been repaired
Event OnWorkshopObjectRepaired(ObjectReference akReference)
EndEvent

; Event that occurs when a workshop NPC is directed to move to another settlement
Event OnWorkshopNPCTransfer(Location akNewWorkshop, Keyword akActionKW)
EndEvent

; Set of read-only properties to essentually make a fake enum for motion types passed in to the trap hit
int Property Motion_Fixed = 0 AutoReadOnly
int Property Motion_Dynamic = 1 AutoReadOnly
int Property Motion_Keyframed = 2 AutoReadOnly

