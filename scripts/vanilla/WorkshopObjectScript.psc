Scriptname WorkshopObjectScript extends ObjectReference
{script for workshop buildable objects
holds data about the object and sends event when build
(possibly all of this will turn out to be temp)
TODO - make const when it stops holding data
}

import WorkshopDataScript

WorkshopParentScript Property WorkshopParent Auto Const mandatory

Group VendorData
	int Property VendorType = -1 auto const
	{ based on index from WorkshopParent VendorTypes }

	int Property VendorLevel = 0 auto const
	{ level of vendor for this store: 0-2 }

	bool Property bVendorTopLevelValid = false auto hidden
	{ set to TRUE when this object counts as a valid top level vendor}

EndGroup

; the workshop that created me
; -1 means no workshop assigned (editor-placed object)
int Property workshopID = -1 auto hidden conditional

bool Property bAllowPlayerAssignment = true auto conditional
{ 	TRUE = player can assign NPCs to work on this object (default)
	FALSE = ignore assignment by player - scripted/editor set assignment is all that is allowed
}

bool Property bAllowAutoRepair = true auto conditional
{ TRUE = can be repaired automatically by daily update process
  FALSE = can ONLY be repaired manually by player
}

bool Property bDefaultPlayerOwnership = true auto conditional
{ 	TRUE = set to player ownership when built - this allows objects which aren't work objects to be targeted by enemies
	FALSE = no default player ownership
}

bool Property bWork24Hours = false auto const conditional
{ 	TRUE = NPCs assigned to this will be set to work 24 hours a day (no sleep, no relaxation)
	FALSE = normal work hours
}

Keyword Property AssignedActorLinkKeyword auto conditional
{ OPTIONAL - if specified, assigned actor will be linked to this object using this keyword
  allows assigned actor to hold position near the object in combat
}

; used by ResetWorkshop to indicate which objects it has finished with
bool Property bResetDone = false auto hidden

; INTERNAL VARIABLES:
	bool bHasMultiResource = false
	; true = this object has a "multi resource" - allows one NPC to work on multiple resource objects }

	ActorValue multiResourceValue
	; if bHasMultiResource = true, this stores the resource rating (if any) that allows multiple objects per NPC (e.g. food) }

	bool bMultiResourceInitialized = false
	; set to true after we've initialized multi-resource data - no need to do it again }



; furniture marker (for things that aren't furniture)
group FurnitureMarker
	Form Property FurnitureBase const auto
	{ if set, will be used to create reference (myFurnitureMarkerRef) when object is created }


	bool property bPlaceMarkerOnCreation = false const auto
	{ true = place marker when object is created -- this is a very special case, currently used only for MQ206 teleporter objects
	  false = place marker only when object is assigned (and delete when unassigned) }

	bool property bMarkersGetOwnership = true const auto
	{ true = markers get same ownership as object (e.g. flora work markers)
	  false = markers don't pick up object ownership (e.g. relaxation markers) }

	String[] property FurnitureMarkerNodes const auto
	{ Nodes to use to place furniture markers }

	; Patch 1.4 - add a special furniture base to look for that gets ownership based on its own flag
	Form Property SpecialFurnitureBase const auto
	{ use this as a flag - whereever this appears place at SpecialFurnitureBaseNode }

	String property SpecialFurnitureBaseNode const auto
	{ see above - node in FurnitureMarkerNodes array to place SpecialFurnitureBase }

	bool property bSpecialMarkerGetsOwnership = true const auto
	{ true = special marker gets same ownership as object
	  false = marker doesn't pick up object ownership }

	Keyword property SpecialFurniturePlacementKeyword const auto
	{ OPTIONAL - if it exists, only place SpecialFurnitureBase if owning actor has this keyword }
endGroup

ObjectReference[] myFurnitureMarkerRefs

MovableStatic Property DamageHelper const auto
{ OPTIONAL - if set, create a damage helper when built and link it to me }

; damage helper reference
ObjectReference myDamageHelperRef

bool Property bRadioOn = true auto hidden		; for now only used on temp radio object

float floraResetHarvestDays = 1.0 const				; how many game days to reset harvestable flora?

Event OnInit()
;	;WorkshopParent.wsTrace(self + " OnInit")
	; if an actor that requires power, make unconscious
	Actor meActor = (self as ObjectReference) as Actor
	if meActor && IsPowered() == false && RequiresPower() 
		meActor.SetUnconscious(true)
	endif
	if bPlaceMarkerOnCreation
		CreateFurnitureMarkers()
	endif
	HandleCreation(false)
EndEvent

event OnLoad()
	; create link on load if I have an owner
	if AssignedActorLinkKeyword
		Actor myOwner = GetActorRefOwner()
		if myOwner
			myOwner.SetLinkedRef(self, AssignedActorLinkKeyword)
		endif
	endif
EndEvent

event OnUnload()
	; remove link on unload if it exists, to avoid creating persistence
	if AssignedActorLinkKeyword
		Actor myOwner = GetActorRefOwner()
		if myOwner
			myOwner.SetLinkedRef(NONE, AssignedActorLinkKeyword)
		endif
	endif
EndEvent

; returns true if this object has a "multi resource" actor value
bool function HasMultiResource()
	if bMultiResourceInitialized
		return bHasMultiResource
	else
		GetMultiResourceValue()
		return bHasMultiResource
	endif
endFunction


; returns "multi resource" actor value, if any; otherwise returns NONE
ActorValue function GetMultiResourceValue()
	if bMultiResourceInitialized
		if bHasMultiResource
			return multiResourceValue
		else
			return NONE
		endif
	endif

	; otherwise, initialize multiresource values
	; get this once for speed
	WorkshopDataScript:WorkshopActorValue[] WorkshopResourceAVs = WorkshopParent.WorkshopResourceAVs
	int arrayLength = WorkshopResourceAVs.Length
	int i = 0
	while i < arrayLength
;		;WorkshopParent.wstrace(self + " i=" + i + ": WorkshopResourceAVs[i].resourceValue=" + WorkshopResourceAVs[i].resourceValue)
		; check for multi-resource
		if GetBaseValue(WorkshopResourceAVs[i].resourceValue) > 0 && WorkshopParent.WorkshopRatings[WorkshopResourceAVs[i].workshopRatingIndex].maxProductionPerNPC > 0
			;WorkshopParent.wstrace(self + " 	'Multi' resource found: " + WorkshopResourceAVs[i].resourceValue, bNormalTraceAlso=true)
			bHasMultiResource = true
			multiResourceValue = WorkshopResourceAVs[i].resourceValue
			i = arrayLength
		endif
		i += 1
	endWhile
	bMultiResourceInitialized = true
endFunction

; returns true if this requires an actor assigned in order to produce/consume resources
bool function RequiresActor()
	return HasKeyword(WorkshopParent.WorkshopWorkObject)
endFunction

; returns true if this is considered a bed by the workshop system
bool function IsBed()
	float val = GetBaseValue(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingBeds].resourceValue)
	;WorkshopParent.wsTrace(self + " IsBed=" + val )
	return ( val > 0 )
endFunction

; is an actor assigned to this?
bool function IsActorAssigned()
	bool val = GetAssignedActor() != NONE
	if !val && IsBed() && GetFactionOwner() != NONE
		; if I'm a bed, faction ownership counts as valid "assignment"
		;WorkshopParent.wsTrace(self + " IsActorAssigned - bed with faction ownership = true")
		val = true
	endif
	;WorkshopParent.wsTrace(self + " IsActorAssigned=" + val )	
	return val
endFunction

WorkshopNPCScript function GetAssignedActor()
	WorkshopNPCScript assignedActor = GetActorRefOwner() as WorkshopNPCScript
	if !assignedActor
		; check for base actor ownership
		ActorBase baseActor = GetActorOwner()
;		;WorkshopParent.wsTrace(" baseActor=" + baseActor)
		if baseActor && baseActor.IsUnique()
			; if this has Actor ownership, use GetUniqueActor when available to get the actor ref
			assignedActor = baseActor.GetUniqueActor() as WorkshopNPCScript
		endif
	endif
;	;WorkshopParent.wsTrace(" GetAssignedActor: " + assignedActor)
	return assignedActor
endFunction

bool function RequiresPower()
	return ( GetValue(WorkshopParent.PowerRequired) > 0 )
endFunction

bool function GeneratesPower()
	return ( GetValue(WorkshopParent.PowerGenerated) > 0 )
endFunction

; timer ID - waiting to respond to OnPowerOn event
int OnPowerOnTimer = 0
float OnPowerOnTimerLength = 0.5 	; in seconds
int OnPowerOnTimerMaxCount = 10		; max times to iterate waiting for OnPowerOn event (failsafe to end waiting for timer)
int OnPowerOnTimerCount = 0			; how many times have we iterated?

; timer event
Event OnTimer(int aiTimerID)
	; waiting to respond to OnPowerOn event
	if aiTimerID == OnPowerOnTimer
		OnPowerOnTimerCount += 1
		if OnPowerOnTimerCount <= OnPowerOnTimerMaxCount
			HandlePowerStateChange(true)
		endif
	endif
EndEvent

bool function IsFactionOwner(WorkshopNPCScript theActor)
	Faction theFaction = GetFactionOwner()
	return ( theFaction && theActor.IsInFaction(theFaction) )
endFunction

; special helper function for MQ206 objects
; just assign/remove actor ref ownership for specified actor
function AssignActorOwnership(Actor newActor = None)
	if newActor
		SetActorRefOwner(newActor, true)
		; create furniture marker if necessary
		if FurnitureBase && myFurnitureMarkerRefs.Length == 0
			CreateFurnitureMarkers()
			UpdatePosition()
		endif
		SetFurnitureMarkerOwnership(newActor)
	else
		SetActorRefOwner(none)
		; if marker placed on creation, just remove ownership
		if bPlaceMarkerOnCreation
			SetFurnitureMarkerOwnership(none)
		else
		; otherwise, delete marker when unassigned
			DeleteFurnitureMarkers()
		endif
	endif
endFunction

; helper function to set/clear ownership of furniture markers
function SetFurnitureMarkerOwnership(Actor newActor)
	if myFurnitureMarkerRefs.Length > 0
		int i = 0
		while i < myFurnitureMarkerRefs.Length
			if newActor
				; Patch 1.4 - special marker check
				bool bSetOwnership = bMarkersGetOwnership
				if SpecialFurnitureBase
					if myFurnitureMarkerRefs[i].GetBaseObject() == SpecialFurnitureBase && bSpecialMarkerGetsOwnership
						bSetOwnership = true
					endif
				endif
				if bSetOwnership
					myFurnitureMarkerRefs[i].SetActorRefOwner(newActor, true)
				endif
			else
				myFurnitureMarkerRefs[i].SetActorRefOwner(None)
			endif
			i += 1
		endWhile
	endif
endFunction

; helper function to delete furniture markers and clear array
function DeleteFurnitureMarkers()
	if myFurnitureMarkerRefs.Length > 0
		int i = 0
		while i < myFurnitureMarkerRefs.Length
			myFurnitureMarkerRefs[i].Delete()
			i += 1
		endWhile
		myFurnitureMarkerRefs.Clear()
	endif
endFunction


function AssignActor(WorkshopNPCScript newActor = None)
	;WorkshopParent.wsTrace(self + "	AssignActor: newActor=" + newActor)
	if newActor
		; if this is a bed, and has faction ownership OR actor base ownership, just keep it
		if IsBed() && ( IsOwnedBy(newActor) )
			; do nothing - editor-set faction ownership for double beds etc.
			;WorkshopParent.wsTrace(self + "	AssignActor: DO NOTHING - actor already owns this bed.")
		else
			SetActorRefOwner(newActor, true)
		endif
		;WorkshopParent.wsTrace(self + " AssignActor - " + GetActorRefOwner())
		; create furniture marker if necessary
		if FurnitureBase && myFurnitureMarkerRefs.Length == 0
			CreateFurnitureMarkers()
			UpdatePosition()
		endif
		if myFurnitureMarkerRefs.Length > 0
			SetFurnitureMarkerOwnership(newActor)
		endif

		; link actor to me if keyword
		if AssignedActorLinkKeyword
			newActor.SetLinkedRef(self, AssignedActorLinkKeyword)
		endif
	else
		SetActorRefOwner(none)
		; default ownership = player (so enemies will attack them if appropriate)
		SetActorOwner(Game.GetPlayer().GetActorBase())
		;WorkshopParent.wsTrace(self + " AssignActor: no owner, so assigning player ownership: " + GetActorOwner())
		if myFurnitureMarkerRefs.Length > 0
			; if marker placed on creation, just remove ownership
			if bPlaceMarkerOnCreation
				SetFurnitureMarkerOwnership(none)
			else
			; otherwise, delete markers when unassigned
				DeleteFurnitureMarkers()
			endif
		endif
	endif

	AssignActorCustom(newActor)
endFunction

function AssignActorCustom(WorkshopNPCScript newActor)
	; blank function for extended scripts to use
endFunction

; create new "furniture" marker - including randomized pick from form lists
; OBSOLETE
;/
function CreateFurnitureMarker()
	Form myFurnitureBase
	FormList myFormList = FurnitureBase as FormList
	if myFormList
		; if FurnitureBase is a form list, pick one at random
		int pickIndex = Utility.RandomInt(0, myFormList.GetSize()-1)
		myFurnitureBase = myFormList.GetAt(pickIndex)
	else
		; otherwise, just use FurnitureBase
		myFurnitureBase = FurnitureBase
	endif

	if myFurnitureBase
		myFurnitureMarkerRef = PlaceAtMe(myFurnitureBase)
		if HasNode(FurnitureMarkerNode)
			myFurnitureMarkerRef.MoveToNode(self, FurnitureMarkerNode)
		endif
		; move to closest spot on navmesh
		myFurnitureMarkerRef.MoveToNearestNavmeshLocation()
	endif
endFunction
/;

function CreateFurnitureMarkers()
	if FurnitureBase == None
		return
	else
		; clear marker ref array for safety
		if myFurnitureMarkerRefs.Length > 0
			DeleteFurnitureMarkers()
		endif
		; recreate
		myFurnitureMarkerRefs = new ObjectReference[0]
	endif

	Form myFurnitureBase
	FormList myFormList = FurnitureBase as FormList

	int i = 0
	while i < FurnitureMarkerNodes.Length
		; place marker at each node
		if myFormList
			; if FurnitureBase is a form list, pick one at random
			int pickIndex = Utility.RandomInt(0, myFormList.GetSize()-1)
			myFurnitureBase = myFormList.GetAt(pickIndex)
		else
			; otherwise, just use FurnitureBase
			myFurnitureBase = FurnitureBase
		endif

		; Patch 1.4 - special marker check
		if SpecialFurnitureBase
			; if this node matches the special node, use this instead of normal marker
			String currentNode = FurnitureMarkerNodes[i]
			if currentNode == SpecialFurnitureBaseNode
				if SpecialFurniturePlacementKeyword
					; if keyword is present, only place this marker if owner has this keyword
					Actor myOwner = GetActorRefOwner()
					if myOwner && myOwner.HasKeyword(SpecialFurniturePlacementKeyword)
						myFurnitureBase = SpecialFurnitureBase
					else
						myFurnitureBase = NONE
					endif
				else
					myFurnitureBase = SpecialFurnitureBase
				endif
			endif
		endif

		if myFurnitureBase
			ObjectReference newMarker = PlaceAtMe(myFurnitureBase)
			; add to array
			myFurnitureMarkerRefs.Add(newMarker)
			if HasNode(FurnitureMarkerNodes[i])
				newMarker.MoveToNode(self, FurnitureMarkerNodes[i])
			endif
			; move to closest spot on navmesh
			newMarker.MoveToNearestNavmeshLocation()
		endif

		i += 1
	endWhile

endFunction


function HandleDestruction()
	;debug.tracestack(self + " HandleDestruction")

	;WorkshopParent.wsTrace(self + "HandleDestruction")
	WorkshopScript workshopRef = WorkshopParent.GetWorkshop(workshopID)

	RecalculateResourceDamage(workshopRef)

	;WorkshopParent.wsTrace(self + "HandleDestruction: done with resources")
	WorkshopParent.UpdateWorkshopRatingsForResourceObject(self, WorkshopParent.GetWorkshop(workshopID), bRecalculateResources = false)

	; destroy furniture marker if any
	int i = 0
	while i < myFurnitureMarkerRefs.Length
		myFurnitureMarkerRefs[i].SetDestroyed()
		i += 1
	endwhile

	; if Flora, mark as harvested
	if GetBaseObject() as Flora
		SetHarvested(true)
	endif
	; send custom event for this object
	WorkshopParent.SendDestructionStateChangedEvent(self, workshopRef)
	;WorkshopParent.wsTrace(self + "HandleDestruction: sent event")
endFunction

function RecalculateResourceDamage(WorkshopScript workshopRef = NONE, bool clearAllDamage = false)
	if workshopRef == NONE
		; get it
		workshopRef = WorkshopParent.GetWorkshop(workshopID)
	endif

	; get this once for speed
	WorkshopDataScript:WorkshopActorValue[] WorkshopResourceAVs = WorkshopParent.WorkshopResourceAVs

	; recalc total damage ratings for each relevant resource
	int i = 0
	int arrayLength = WorkshopResourceAVs.Length
	while i < arrayLength
		ActorValue resourceValue = WorkshopResourceAVs[i].resourceValue
		float baseValue = GetBaseValue(resourceValue)
		if baseValue > 0
			if clearAllDamage 
				; restore this value
				RestoreValue(resourceValue, baseValue)
			endif
			;WorkshopParent.wsTrace(self + "RecalculateResourceDamage: resource  " + resourceValue)
			; recalculate the total damage for this resource
			WorkshopParent.RecalculateResourceDamageForResource(workshopRef, resourceValue)
		endif
		i += 1
	endWhile

	; update overall damage
	WorkshopParent.UpdateCurrentDamage(workshopRef)

endFunction

Event OnDestructionStageChanged(int aiOldStage, int aiCurrentStage)
	;WorkshopParent.wsTrace(self + " OnDestructionStageChanged: oldStage=" + aiOldStage + ", currentStage=" + aiCurrentStage)
	if IsDestroyed()
		HandleDestruction()
	elseif aiCurrentStage == 0
		WorkshopScript workshopRef = WorkshopParent.GetWorkshop(workshopID)
		; I've been repaired  - clear all damage
		RecalculateResourceDamage(workshopRef, true)
		; send custom event for this object
		WorkshopParent.SendDestructionStateChangedEvent(self, workshopRef)
	endif
EndEvent

Event OnPowerOn(ObjectReference akPowerGenerator)
	;WorkshopParent.wsTrace(self + " OnPowerOn akPowerGenerator=" + akPowerGenerator)
	HandlePowerStateChange(true)
EndEvent

Event OnPowerOff()
	;WorkshopParent.wsTrace(self + " OnPowerOff")
	HandlePowerStateChange(false)
EndEvent

function HandlePowerStateChange(bool bPowerOn = true)
	if bPowerOn
		; if we don't have a workshopID yet, wait to handle this later
		if workshopID < 0
			;WorkshopParent.wsTrace(self + " HandlePowerStateChange: waiting for valid workshop ID")
			StartTimer(OnPowerOnTimerLength, OnPowerOnTimer)
			return
		endif

		; we got an ID, clear the counter
		OnPowerOnTimerCount = 0
	endif
	WorkshopParent.UpdateWorkshopRatingsForResourceObject(self, WorkshopParent.GetWorkshop(workshopID), bRecalculateResources = false) ; code handles recalculating resources from power state changes

	; send custom event for this object
	WorkshopScript workshopRef = WorkshopParent.GetWorkshop(workshopID)
	WorkshopParent.SendPowerStateChangedEvent(self, workshopRef)

	; update missing food/water/safety if applicable
	; REMOVE - code now updates missing food/water/safety
	;/
	bool missingMatchingResource = workshopRef.IsMissingMatchingResource(self)
	; if power going on AND missing a matching resource; OR; power going off AND not missing matching resource, update
	if (bPowerOn && missingMatchingResource) || (bPowerOn == false && missingMatchingResource == false)
		;WorkshopParent.wsTrace(self + " ActivatedByWorkshopActor: missing matching resource - update")
		workshopRef.DailyUpdate(bRealUpdate = false)
	endif
	/;
endFunction

Event OnActivate(ObjectReference akActionRef)
	;WorkshopParent.wsTrace(self + " activated by " + akActionRef + " isradio?" + HasKeyword(WorkshopParent.WorkshopRadioObject))
	if akActionRef == Game.Getplayer() 
		;WorkshopParent.wstrace(self + " activated by player")
		if CanProduceForWorkshop()
			if HasKeyword(WorkshopParent.WorkshopRadioObject)
				; radio on/off
				; toggle state
				bRadioOn = !bRadioOn
				WorkshopParent.UpdateRadioObject(self)
			endif
		else
		endif
		; player comment
		if IsBed() == false
			WorkshopParent.PlayerComment(self)
		endif
	endif

	if GetBaseObject() as Flora
		SetValue(WorkshopParent.WorkshopFloraHarvestTime, Utility.GetCurrentGameTime())
	endif
EndEvent

function ActivatedByWorkshopActor(WorkshopNPCScript workshopNPC)
	if workshopNPC && workshopNPC.IsDoingFavor() && workshopNPC.IsInFaction(WorkshopParent.Followers.CurrentCompanionFaction) == false
		;debug.Trace(self + " activated by " + workshopNPC + " in favor state - assigning")
		if bAllowPlayerAssignment
			; turn off favor state
			workshopNPC.setDoingFavor(false, false) ; going back to using normal command mode for now
			; unregister for distance event
			workshopNPC.UnregisterForDistanceEvents(workshopNPC, WorkshopParent.GetWorkshop(workshopID))
			; assign this NPC to me if this is a work object
			if RequiresActor() || IsBed()
				workshopNPC.SayCustom(WorkshopParent.WorkshopParentAssignConfirmTopicType)
				WorkshopParent.AssignActorToObjectPUBLIC(workshopNPC, self)
				WorkshopParent.WorkshopResourceAssignedMessage.Show()
				workshopNPC.StartAssignmentTimer()
				; if food/water/safety are missing, run check if this is that kind of object
				if IsBed() == false
					WorkshopScript workshopRef = WorkshopParent.GetWorkshop(workshopID)
					workshopRef.RecalculateResources()
				endif

			endif
			workshopNPC.EvaluatePackage()
		else
			WorkshopParent.WorkshopResourceNoAssignmentMessage.Show()
		endif
	endif

endFunction

; returns TRUE if object was actually damaged/repaired
; otherwise FALSE
bool function ModifyResourceDamage(ActorValue akActorValue, float aiDamageMod)
	WorkshopParent.wsTrace(self + "	ModifyResourceDamage: " + akActorValue + " " + aiDamageMod)
	float totalDamage = 0
	float baseValue = GetBaseValue(akActorValue)
	bool returnVal = false

	if baseValue > 0
		if aiDamageMod < 0
			; negative damage = repair
			if bAllowAutoRepair
				WorkshopParent.wsTrace(self + "		restoring " + (aiDamageMod * -1) + " to " + akActorValue)
				RestoreValue(akActorValue, aiDamageMod * -1)
				returnVal = true ; actually repaired
			endif
		else
			; positive damage = damage...
			float currentDamage = (baseValue - GetValue(akActorValue))
			; total damage can't exceed base value, so make sure it doesn't
			WorkshopParent.wsTrace(self + "		currentDamage=" + currentDamage + ", baseValue=" + baseValue)

			if (currentDamage + aiDamageMod) > baseValue
				; reduce incoming damage
				aiDamageMod = aiDamageMod - ((currentDamage + aiDamageMod) - baseValue)
				WorkshopParent.wsTrace(self + "		final aiDamageMod=" + aiDamageMod)
			endif
			DamageValue(akActorValue, aiDamageMod)
			returnVal = true ; actually damaged
		endif

		; check total damage
		totalDamage = totalDamage + (baseValue - GetValue(akActorValue))
		WorkshopParent.wsTrace(self + "		totalDamage=" + totalDamage)
	endif

	; if there is any damage, destroy me
	bool bDestroyed = (totalDamage > 0)

	if bDestroyed
		if IsDestroyed() == false
			; state change
			SetDestroyed(true)
			DamageObject(9999.0)
			HandleDestruction()
		endif
		WorkshopParent.wsTrace(self + "		DESTROYED")
	else
		WorkshopParent.wsTrace(self + "		UNDESTROYED")
		Repair()
		;/
		if IsDestroyed() == true
			; state change
			ClearDestruction()
			SetDestroyed(false)
		endif
		if (self as ObjectReference) as Actor
			(((self as ObjectReference) as Actor) as WorkshopObjectActorScript).HandleRepair()
		endif
		/;
		int i = 0
		while i < myFurnitureMarkerRefs.Length
			myFurnitureMarkerRefs[i].SetDestroyed(false)
			i += 1
		endWhile

		if myDamageHelperRef
			myDamageHelperRef.ClearDestruction()
		endif
	endif

	return returnVal
endFunction

Event OnWorkshopObjectGrabbed(ObjectReference akReference)
	;debug.trace(self + " OnWorkshopObjectGrabbed")
	; disable markers while moving
	HideMarkers()
EndEvent

Event OnWorkshopObjectMoved(ObjectReference akReference)
	;debug.trace(self + " OnWorkshopObjectMoved")
	UpdatePosition()
EndEvent

function UpdatePosition()
	int i = 0
	int size = myFurnitureMarkerRefs.Length
	while i < size
		ObjectReference theMarker = myFurnitureMarkerRefs[i]
		;debug.trace(self + " UpdatePosition: marker " + i + ": " + theMarker)
		; make sure enabled
		theMarker.Enable()
		theMarker.MoveTo(self)
		;debug.trace(self + " 	UpdatePosition: " + theMarker + "GetDistance=" + theMarker.GetDistance(self))
		if HasNode(FurnitureMarkerNodes[i])
			;debug.trace(self + " 	UpdatePosition: moving to node " + FurnitureMarkerNodes[i])
			theMarker.MoveToNode(self, FurnitureMarkerNodes[i])
			;debug.trace(self + " 	UpdatePosition: " + theMarker + "GetDistance=" + theMarker.GetDistance(self))
		endif
		; move to closest spot on navmesh
		;debug.trace(self + " 	UpdatePosition: now move to nearest navmesh location")
		;debug.trace(self + " 	UpdatePosition: " + theMarker + "GetDistance=" + theMarker.GetDistance(self))
		theMarker.MoveToNearestNavmeshLocation()
		;debug.trace(self + " 	UpdatePosition: " + theMarker + "GetDistance=" + theMarker.GetDistance(self))
		i += 1
	endWhile

	if myDamageHelperRef
		myDamageHelperRef.Moveto(self)
		; make sure enabled
		myDamageHelperRef.Enable()
	endif
endFunction

function HideMarkers()
	int i = 0
	int size = myFurnitureMarkerRefs.Length
	while i < size
		myFurnitureMarkerRefs[i].Disable()
		i += 1
	endWhile
	if myDamageHelperRef
		myDamageHelperRef.Disable()
	endif
endFunction

; called when object is created
function HandleCreation(bool bNewlyBuilt = true)
	; create damage helper if appropriate
	if DamageHelper && !myDamageHelperRef
		myDamageHelperRef = PlaceAtMe(DamageHelper)
		; link to me
		myDamageHelperRef.SetLinkedRef(self)
	endif

	if bNewlyBuilt
		; if flora, mark initially as harvested
		if GetBaseObject() as Flora
			SetHarvested(true)
		endif
	endif

	; if unowned, give player ownership
	if HasOwner() == false && IsBed() == false && bDefaultPlayerOwnership
		; default ownership = player (so enemies will attack them if appropriate)
		SetFactionOwner(WorkshopParent.PlayerFaction)
		;WorkshopParent.wsTrace(self + " HandleCreation: unowned, assigning PlayerFaction ownership: " + GetFactionOwner())
	endif		
endFunction

; clean up created refs when deleted
function HandleDeletion()
	if myFurnitureMarkerRefs.Length > 0
		DeleteFurnitureMarkers()
	endif
	if myDamageHelperRef
		myDamageHelperRef.Delete()
	endif
endFunction

; called on workshop objects during reset
function HandleWorkshopReset()
	; since workshop locations don't reset, do it manually
	float harvestTime = GetValue(WorkshopParent.WorkshopFloraHarvestTime)
	if GetBaseObject() as Flora
		;WorkshopParent.wsTrace(self + " HandleWorkshopReset: last harvest time=" + harvestTime +", IsActorAssigned=" + IsActorAssigned() + ", current time=" + utility.GetCurrentGameTime())
		if IsActorAssigned() && utility.GetCurrentGameTime() > (harvestTime + floraResetHarvestDays)
			SetHarvested(false)
		endif
	endif
endFunction

bool function HasResourceValue(ActorValue akValue)
	if akValue && GetBaseValue(akValue) > 0
		return true
	else
		return false
	endif
endFunction

float function GetResourceRating(ActorValue akValue)
;	;WorkshopParent.wsTrace(self + "GetResourceRating for " + akValue)
	return GetValue(akValue)
endFunction

bool function HasResourceDamage()
	; loop through resource actor value on this object
	int i = 0
	float totalDamage = 0

	; get this once for speed
	WorkshopDataScript:WorkshopActorValue[] WorkshopResourceAVs = WorkshopParent.WorkshopResourceAVs
	int arrayLength = WorkshopResourceAVs.Length
	while i < WorkshopResourceAVs.Length && totalDamage == 0
		ActorValue testValue = WorkshopResourceAVs[i].resourceValue
		float damage = GetBaseValue(testValue) - GetValue(testValue)
		totalDamage = totalDamage + damage
		i += 1
	endWhile
	return (totalDamage > 0)
endFunction

function testGetResourceDamage(ActorValue akAV = None)
	float damage = GetResourceDamage(akAV)
	;debug.trace(self + " GetResourceDamage(" +akAV + ")=" + damage)
endFunction