Scriptname WorkshopNPCScript extends Actor Conditional
{script for all NPCs that can be assigned to a workshop}

WorkshopParentScript Property WorkshopParent Auto Const

Group WorkerData

	bool Property bCommandable = false auto Conditional
	{ TRUE = commandable by player - can be ordered to different work objects (default)
	  FALSE = player can't command although will still count as worker if given default work
	}

	bool Property bAllowCaravan = false auto Conditional
	{ TRUE = can be assigned to caravan duty
	}

	bool Property bAllowMove = false auto Conditional
	{ TRUE = can be moved to different settlements
	}

	; worker flag - used by package conditions etc.
	bool Property bIsWorker = false auto Conditional
	{ worker flag - used by package conditions
		set to TRUE if this NPC is a worker of any kind
	}

	bool Property bWork24Hours = false auto Conditional
	{ set to TRUE to have someone work 24 hours }

	; guard flag - used by package conditions
	bool Property bIsGuard = false auto Conditional Hidden
	{ set to TRUE if this NPC is a "guard" - assigned to Safety work objects like guard posts etc. }

	; scavenger flag - used by package conditions
	bool Property bIsScavenger = false auto Conditional Hidden
	{ set to TRUE if this NPC is a scavenger - assigned to Scavenge work objects }

	ActorValue Property assignedMultiResource auto
	{ if NONE this worker is assigned a single object to work on
	  otherwise, this is the rating keyword (food, safety, etc.) of the type of resource
	  this NPC can work on }

	float Property multiResourceProduction = 0.0 auto Hidden Conditional
	{ if assignedMultiResource is set, this tracks how much production this NPC is assigned to }

	bool Property bIsSynth = false auto conditional hidden
	{ set to TRUE if this NPC has been tagged as a synth - gives appropriate death item }
endGroup

bool Property bWorkshopStatusOn = true auto conditional hidden
{ set to false when temporarily turning off - but saving - workshop NPC status, e.g. for companions }
; if workshop status turned off, save these flags here so they can be restored later
bool bSavedAllowMove
bool bSavedAllowCaravan
bool bSavedCommandable

; used by ResetWorkshop to indicate which objects it has finished with
bool Property bResetDone = false auto hidden

; the brahmin assigned to me if I'm a caravan actor
Actor Property myBrahmin auto

bool Property bNewSettler = false auto Conditional
{ set to true when new settlers are created - set back to false after player "meets" them }

bool Property bCountsForPopulation = true auto Conditional
{ set to false for things like brahmin which don't count for total population }

bool Property bApplyWorkshopOwnerFaction = true auto conditional
{ set to false for NPCs that should not pick up the owner faction of their assigned workshop - e.g. companions }

LocationRefType Property CustomBossLocRefType Auto Const
{ Patch 1.4: custom loc ref type to use for this actor when assigning to workshop }

group VendorData
	int Property specialVendorType = -1 auto const
	{ based on index from WorkshopParent VendorTypes - set for NPCs who have special vendor abilities }

	int Property specialVendorMinLevel = 2 auto const
	{ if a special vendor, what level does the vendor object have to be to allow special ability? }

	Container Property specialVendorContainerBase auto const
	{ base object of special vendor container to link to when special ability is allowed }

	ObjectReference Property specialVendorContainerRef auto hidden
	{ reference (created by script) to link to when special ability is allowed }

	ObjectReference Property specialVendorContainerRefUnique auto
	{ reference to link to when special ability is allowed }
endGroup

; workshop that this NPC is assigned to (also mirrored by the actor value - but need a way to say "no workshop" since the default actor value is 0)
int workshopID = -1 conditional

; for dropping out of command state
int iSelfActivationCount = 0

; return the workshopID of this actor
int function GetWorkshopID()
	return workshopID
endFunction

function SetWorkshopID(int newWorkshopID)
	workshopID = newWorkshopID
	SetValue(WorkshopParent.workshopIDActorValue, newWorkshopID)
	; put script in correct state (we only care about events if assigned to a workshop)
	if newWorkshopID > -1
		gotoState("assigned")
	else
		gotoState("unassigned")
	endif
endFunction

function UpdatePlayerOwnership(WorkshopScript workshopRef = NONE)
	; get workshop if not passed in
	if workshopRef == NONE
		workshopRef = WorkshopParent.GetWorkshop(workshopID)
	endif
	;WorkshopParent.wsTrace(self + " UpdatePlayerOwnership for workshop " + workshopRef + ": OwnedByPlayer=" + workshopRef.OwnedByPlayer)
	if workshopRef
		; set player ownership actor value
		SetValue(WorkshopParent.WorkshopPlayerOwnership, workshopRef.OwnedByPlayer as int)
	endif
endFunction

; return workshopID of caravan destination (if any)
int function GetCaravanDestinationID()
	return GetValue(WorkshopParent.WorkshopCaravanDestination) as int   
endFunction

bool function IsWounded()
;	;;debug.trace(GetValue(WorkshopParent.WorkshopActorWounded) as bool)
	return GetValue(WorkshopParent.WorkshopActorWounded) as bool
endFunction

function SetWounded(bool bIsWounded)
	SetValue(WorkshopParent.WorkshopActorWounded, bIsWounded as int)
	; am I a caravan actor?
	int foundIndex = WorkshopParent.CaravanActorAliases.Find(self)
	if foundIndex > -1
		WorkshopParent.TurnOnCaravanActor(self, bIsWounded == false)
	endif
endFunction

function SetWorker(bool isWorker)
	bIsWorker = isWorker
	if !isWorker
		bIsGuard = false
		bIsScavenger = false
	endif
endFunction

function SetScavenger(bool isScavenger)
	bIsScavenger = isScavenger
endFunction

function SetSynth(bool isSynth)
	;WorkshopParent.wsTrace(self + " SetSynth " + isSynth)
	bIsSynth = isSynth
	SetValue(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingPopulationSynths].resourceValue, (isSynth == true) as float)
	; if created and assigned to a workshop, set ref type
	if IsCreated() && workshopID > -1
		WorkshopScript workshopRef = WorkshopParent.GetWorkshop(GetWorkshopID())
		if workshopRef.myLocation
			if isSynth
				SetLocRefType(workshopRef.myLocation, WorkshopParent.WorkshopSynthRefType)
			else
				; change back to Boss
				SetLocRefType(workshopRef.myLocation, WorkshopParent.Boss)
			endif
			ClearFromOldLocations() ; 101931: make sure location data is correct
		endif
	endif
endFunction

function SetMultiResource(ActorValue resourceValue)
	;WorkshopParent.wstrace(self + " SetMultiResource: resourceValue=" + resourceValue)

	assignedMultiResource = resourceValue
	if assignedMultiResource == WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingSafety].resourceValue
		bIsGuard = true
	else
		bIsGuard = false
	endif

	if !assignedMultiResource
		; clear production if no longer assigned to a multiresource
		multiResourceProduction = 0.0
	endif
endFunction

function AddMultiResourceProduction(float newProduction)
	multiResourceProduction += newProduction
endFunction

; Patch 1.4 - custom boss loc ref type
function SetAsBoss(Location newLocation)
	if CustomBossLocRefType
		SetLocRefType(newLocation, CustomBossLocRefType)
	else
		SetLocRefType(newLocation, WorkshopParent.Boss)
	endif
	ClearFromOldLocations() ; 101931: make sure location data is correct
endFunction

auto state unassigned
; default state

	Event OnInit()
		; if I'm a follower, register for companion change events
		if (self as Actor) is CompanionActorScript
			;;debug.trace(self + " OnInit - Companion - registering for CompanionChange events")
			RegisterForCustomEvent(FollowersScript.GetScript(), "CompanionChange")		
		endif
		SetCommandable(bCommandable)
		SetAllowCaravan(bAllowCaravan)
		SetAllowMove(bAllowMove)

		; if I have a linked work object, set my ownership to it
		if GetLinkedRef(WorkshopParent.WorkshopLinkWork)
			WorkshopObjectScript workobject = (GetLinkedRef(WorkshopParent.WorkshopLinkWork) as WorkshopObjectScript)
			;WorkshopParent.wstrace(self + " assigning to " + workobject)
			workobject.AssignActor(self)
		endif
	EndEvent

endState

; when assigned to a workshop, script is put into this state
state assigned

	Event OnActivate(ObjectReference akActionRef)
		;;debug.trace(self + "OnActivate " + akActionRef)
		if WorkshopParent.GetWorkshop(GetWorkshopID()).OwnedByPlayer
			;;debug.trace(self + " Owned by player")
			if IsDoingFavor() && akActionRef == self && bCommandable ; must be commandable so this doesn't trigger for companions
				debug.trace(self + " OnActivate - workshop commandable")
				iSelfActivationCount += 1
				if iSelfActivationCount > 1
					; toggle favor state
					setDoingFavor(false, true)
				endif
			endif
		endif
	EndEvent

	Event OnCommandModeGiveCommand(int aeCommandType, ObjectReference akTarget)
		debug.trace(self + " OnCommandModeGiveCommand aeCommandType=" + aeCommandType + " akTarget=" + akTarget)
		WorkshopObjectScript workObject = akTarget as WorkshopObjectScript
		if workObject && aeCommandType == 10 ; workshop assign command
			workObject.ActivatedByWorkshopActor(self)
		endif
	endEvent

	Event OnEnterBleedout()
		; set this guy as "wounded"
		;WorkshopParent.wstrace(self + " OnEnterBleedout")
		if IsWounded()
			;WorkshopParent.wstrace(self + " already wounded - do nothing")
		else
			WorkshopParent.WoundActor(self)
		endif
	EndEvent

	; WOUNDED STATE: removing visible wounded state for now
	Event OnCombatStateChanged(Actor akTarget, int aeCombatState)
	    if aeCombatState == 0 && IsWounded()
	    	WorkshopParent.WoundActor(self, false)
	    endif
	EndEvent

	Event OnDeath(Actor akKiller)
		WorkshopParent.wstrace(self + " OnDeath")
		; death item if synth
		if bIsSynth
			AddItem(WorkshopParent.SynthDeathItem)
		endif
		; remove me from the workshop
		WorkshopParent.HandleActorDeath(self, akKiller)
	EndEvent

	Event OnLoad()
		; do this on load to make sure reset doesn't clear it
		if bWorkshopStatusOn
			SetCommandable(bCommandable)
			SetAllowCaravan(bAllowCaravan)
			SetAllowMove(bAllowMove)
		endif

		; WOUNDED STATE: removing visible wounded state for now
		if IsDead() == false && IsWounded()
	    	WorkshopParent.WoundActor(self, false)
		endif			

		; check if I should create caravan brahmin
		WorkshopParent.CaravanActorBrahminCheck(self)
	EndEvent


endState

Event FollowersScript.CompanionChange(FollowersScript akSender, Var[] akArgs)
	Actor EventActor = akArgs[0] as Actor
	Bool IsNowCompanion = akArgs[1] as bool
	;;debug.trace(self + " CompanionChange event received for " + EventActor + " IsNowCompanion=" + IsNowCompanion)
	if EventActor == self
		if IsNowCompanion
			; turn off workshop status when I become a companion
			SetWorkshopStatus(false)
		else
			; turn on workshop status when I stop being a companion
			SetWorkshopStatus(true)
		endif
	endif
EndEvent

Event OnWorkshopNPCTransfer(Location akNewWorkshopLocation, Keyword akActionKW)
	;debug.trace(self + " OnWorkshopNPCTransfer " + akNewWorkshopLocation + " keyword=" + akActionKW)
	;WorkshopParent.wsTrace(self + " has been directed to transfer to the workshop at " + akNewWorkshopLocation + " with the " + akActionKW + " action")
	; what kind of transfer?
	if akActionKW == WorkshopParent.WorkshopAssignCaravan
		WorkshopParent.AssignCaravanActorPUBLIC(self, akNewWorkshopLocation)
	else
		WorkshopScript newWorkshop = WorkshopParent.GetWorkshopFromLocation(akNewWorkshopLocation)
		if newWorkshop
			if akActionKW == WorkshopParent.WorkshopAssignHome
				WorkshopParent.AddActorToWorkshopPUBLIC(self, newWorkshop)
			elseif akActionKW == WorkshopParent.WorkshopAssignHomePermanentActor
				WorkshopParent.AddPermanentActorToWorkshopPUBLIC(self, newWorkshop.GetWorkshopID())
			endif
		endif
	endif
EndEvent

int timerIDCommandState = 1
float timerCommandStateSeconds = 5.0

int timerIDAssigned = 2 			; run a timer when assigned to a new work object
float timerAssignedSeconds = 120.0 ; how long to stay in the "just assigned" package

function StartAssignmentTimer(bool bStart = true)
	if bStart
		; set assigned actor value
		SetValue(WorkshopParent.WorkshopActorAssigned, 1)
		StartTimer(timerAssignedSeconds, timerIDAssigned)
		EvaluatePackage()
	else
		; clear assigned actor value
		SetValue(WorkshopParent.WorkshopActorAssigned, 0)
		CancelTimer(timerIDAssigned)
	endif
endFunction

function StartCommandState()
	;debug.trace(self + "StartCommandState")
	; clear "activate count"
	iSelfActivationCount = 0
	; set up distance check from workshop
	WorkshopScript myWorkshop = WorkshopParent.GetWorkshop(GetWorkshopID())
	if myWorkshop
		StartTimer(timerCommandStateSeconds, timerIDCommandState)
		setDoingFavor(abDoingFavor = true, abWorkShopMode = true)
	endif
endFunction

Event OnTimer(int aiTimerID)
    if aiTimerID == timerIDCommandState
		WorkshopScript myWorkshop = WorkshopParent.GetWorkshop(GetWorkshopID())
    	if myWorkshop && IsWithinBuildableArea(myWorkshop)
    		; if still within build area, keep waiting
			StartTimer(timerCommandStateSeconds, timerIDCommandState)
		else
			; kill command mode
			setDoingFavor(false, false)
		endif
	elseif aiTimerID == timerIDAssigned
		StartAssignmentTimer(false)
    endif
EndEvent

function SetCommandable(bool bFlag)
	;debug.trace(self + "SetCommandable: " + bFlag)
	; always save new state in "saved" variable
	bSavedCommandable = bFlag
	; if workshop status on, change commandable state
	if bWorkshopStatusOn
		bCommandable = bFlag
		if bCommandable
			;;debug.trace(self + " adding keyword " + WorkshopParent.WorkshopAllowCommand)
			AddKeyword(WorkshopParent.WorkshopAllowCommand)
		else
			RemoveKeyword(WorkshopParent.WorkshopAllowCommand)
		endif
		;;debug.trace(self + " HasKeyword " + WorkshopParent.WorkshopAllowCommand + "=" + HasKeyword(WorkshopParent.WorkshopAllowCommand))
	else
		;;debug.trace(self + " 	workshop status temporarily turned off - saving new state for when turned back on")
	endif
endFunction

function SetAllowCaravan(bool bFlag)
	; always save new state in "saved" variable
	bSavedAllowCaravan = bFlag
	; if workshop status on, change commandable state
	if bWorkshopStatusOn
		bAllowCaravan = bFlag
		if bAllowCaravan
			AddKeyword(WorkshopParent.WorkshopAllowCaravan)
		else
			RemoveKeyword(WorkshopParent.WorkshopAllowCaravan)
		endif

	else
		;;debug.trace(self + " 	workshop status temporarily turned off - saving new state for when turned back on")
	endif
endFunction

function SetAllowMove(bool bFlag)
	; always save new state in "saved" variable
	bSavedAllowMove = bFlag
	; if workshop status on, change commandable state
	if bWorkshopStatusOn
		bAllowMove = bFlag
		if bAllowMove
			AddKeyword(WorkshopParent.WorkshopAllowMove)
		else
			RemoveKeyword(WorkshopParent.WorkshopAllowMove)
		endif
	else
		;;debug.trace(self + " 	workshop status temporarily turned off - saving new state for when turned back on")
	endif
endFunction

function SetWorkshopStatus(bool setWorkshopStatusOn)
	;;debug.trace(self + " SetWorkshopStatus " + setWorkshopStatusOn)
	bWorkshopStatusOn = setWorkshopStatusOn
	if bWorkshopStatusOn
		; restore saved state
		SetCommandable(bSavedCommandable)
		SetAllowMove(bSavedAllowMove)
		SetAllowCaravan(bSavedAllowCaravan)
	else
		; save out current state (failsafe)
		bSavedAllowMove = bAllowMove
		bSavedAllowCaravan = bAllowCaravan
		bSavedCommandable = bCommandable
		; now turn it all off
		RemoveKeyword(WorkshopParent.WorkshopAllowCommand)
		RemoveKeyword(WorkshopParent.WorkshopAllowMove)
		RemoveKeyword(WorkshopParent.WorkshopAllowCaravan)
		bAllowMove = false
		bAllowCaravan = false
		bCommandable = false
		; unassign from any current work
		WorkshopParent.UnassignActor(self)
	endif
endFunction

function TestKill()
	KillEssential()
endFunction