Scriptname WorkshopScript extends ObjectReference Conditional
{script for Workshop reference}

;import WorkShopObjectScript
;import WorkshopParentScript

WorkshopParentScript Property WorkshopParent Auto Const mandatory
{ parent quest - holds most general workshop properties }

Location Property myLocation Auto Hidden
{workshop's location (filled onInit)
 this is a property so the WorkshopParent script can access it}

ObjectReference Property myMapMarker auto hidden
{workshop's map marker (filled by WorkshopParent.InitializeLocation) }

Group Optional
	Faction Property SettlementOwnershipFaction auto
	{ optional - if the workshop settlement has an ownership faction, set this here so the player can be added to that faction when workshop becomes player-owned }
	bool Property UseOwnershipFaction = true auto
	{ set to false to not use the ownership faction }

	ActorBase Property CustomWorkshopNPC Auto const
	{ Patch 1.4: the actor that gets created when a settlement makes a successful recruitment roll - overrides properties on WorkshopParentScript }

	Message Property CustomUnownedMessage const auto
	{ Patch 1.4: a custom unowned message, that overrides standard messages from WorkshopParentScript }

	bool Property AllowBrahminRecruitment = true auto const 
	{ Patch 1.6: set to false to prevent brahmin from being randomly recruited at this workshop settlement }
endGroup

group BuildingBudget
	int property MaxTriangles auto
	{ if > 0, initialize WorkshopMaxTriangles to this value }
	int property MaxDraws auto
	{ if > 0, initialize WorkshopMaxDraws to this value }
	int property CurrentTriangles auto
	{ if > 0, initialize WorkshopCurrentTriangles to this value }
	int property CurrentDraws auto
	{ if > 0, initialize WorkshopCurrentDraws to this value }
endGroup

Group Flags
	bool Property OwnedByPlayer = false Auto Conditional
	{ all workshops start "unowned" - activate after location is cleared to "own"
	}

	bool Property StartsHostile = false Auto Conditional
	{ set to true for workbench locations that start out with hostiles in control (to prevent it being counted as a valid Minuteman recruiting target)}

	bool Property EnableAutomaticPlayerOwnership = true Auto
	{ TRUE = workshop will automatically become usable when the location is cleared (or if there are no bosses)
	  FALSE = workshop won't be usable by player until SetOwnedByPlayer(true) is called on it
	 }

	bool Property AllowUnownedFromLowHappiness = false Auto
	{ 	TRUE = workshop can become unowned due to low happiness (<=minHappinessThreshold)
		FALSE (default) = workshop can never become unowned, no matter how low the happiness (for special cases like the Castle)
	}

	bool Property HappinessWarning Auto Hidden Conditional
	{ set to true when happiness warning given; set back to false when happiness goes above the warning level 
	  NOTE: only applies when AllowUnownedFromLowHappiness = true }

	int Property DaysSinceLastVisit Auto Hidden Conditional
	{ this gets cleared when player visits, incremented by daily update }

	bool Property PlayerHasVisited Auto Hidden Conditional
	{ this gets set to true the first time player visits - used by ResetWorkshop to initialize data on first visit }

	bool Property MinRecruitmentProhibitRandom = false Auto Conditional
	{ set to TRUE to prohibit random Minutemen recruitment quests from picking this workshop
	}

	bool Property MinRecruitmentAllowRandomAfterPlayerOwned = true Auto Conditional
	{ set to FALSE to prohibit random Minutemen quests from picking this workshop AFTER the player takes over (TRUE = random quests are allowed once owned by player)
	}

	bool Property AllowAttacksBeforeOwned = true auto conditional
	{ set to FALSE to prevent attacks when unowned
		NOTE: this always gets set to true when the player takes ownership for the first time
	}

	bool Property AllowAttacks = true auto conditional
	{ set to FALSE to prevent ALL random attacks (e.g. the Castle)
	}

	bool Property RadioBeaconFirstRecruit = false auto conditional hidden
	{ set to true after player first builds a radio beacon here and gets the first "quick" recruit }

	bool Property ShowedWorkshopMenuExitMessage = false auto conditional hidden
	{ set to true after player first exits the workshop menu here }
	
endGroup

int WorkshopID = -1 	; initialize to real workshop ID first time it's requested (workshopID = index of this workshop in WorkshopParent.Workshops)

Group VendorData
	ObjectReference[] Property VendorContainersMisc auto hidden
	{
		array of Misc vendor containers, indexed by vendor level
	 }
	ObjectReference[] Property VendorContainersArmor auto hidden
	ObjectReference[] Property VendorContainersWeapons auto hidden
	ObjectReference[] Property VendorContainersBar auto hidden
	ObjectReference[] Property VendorContainersClinic auto hidden
	ObjectReference[] Property VendorContainersClothing auto hidden
endGroup

; 1.6: optional radio override data
Group WorkshopRadioData
	ObjectReference Property WorkshopRadioRef Auto Const
	{ if WorkshopRadioRef exists, it will override the default WorkshopRadioRef from WorkshopParent }

	float property workshopRadioInnerRadius = 9000.0 auto const
	{ override workshop parent values }
	float property workshopRadioOuterRadius = 20000.0 auto const
	{ override workshop parent values }

	Scene Property WorkshopRadioScene Auto Const
	{ if WorkshopRadioRef exists, WorkshopRadioScene will be started instead of the default scene from WorkshopParent }

	bool property bWorkshopRadioRefIsUnique = true auto const 
	{ TRUE: WorkshopRadioScene is unique to this workshop, so it should be stopped/disabled when radio is shut off (completely) }
endGroup


;******************
; moved from workshopparent
; productivity formula stuff
float minProductivity = 0.25 const
float productivityHappinessMult = 0.75 const

; happiness formula stuff
float maxHappinessNoFood = 30.0 const
float maxHappinessNoWater = 30.0 const
float maxHappinessNoShelter = 60.0 const

float happinessBonusFood = 20.0 const
float happinessBonusWater = 20.0 const
float happinessBonusBed = 10.0 const
float happinessBonusShelter = 10.0 const
float happinessBonusSafety = 20.0 const

int minHappinessChangePerUpdate = 1	const			; what's the min happiness can change in one update?
float happinessChangeMult = 0.20 const				; multiplier on happiness delta

int minHappinessThreshold = 10 Const 				; if happiness drops <= to this value, player ownership is cleared
int minHappinessWarningThreshold = 15 const 		; if happiness drops <= to this value, give happiness warning
int minHappinessClearWarningThreshold = 20 const 	; if happiness >= this value, clear happiness warning

int happinessBonusChangePerUpdate = 2 const 		; happiness bonus trends back to 0 (from positive or negative)

; production
int maxStoredFoodBase = 10 const 					; stop producing when we reach this amount stored
int maxStoredFoodPerPopulation = 1 const 			; increase max for each population

int maxStoredWaterBase = 5 const 					; stop producing when we reach this amount stored
float maxStoredWaterPerPopulation = 0.25 const 			; increase max for each population

int maxStoredScavengeBase = 100 const 				; stop producing when we reach this amount stored
int maxStoredScavengePerPopulation = 5 const 		; increase max for each population

float brahminProductionBoost = 0.5					; what percent increase per brahmin
int maxProductionPerBrahmin = 10 					; each brahmin can only boost this much food (so max 10 * 0.5 = 5)
int maxBrahminFertilizerProduction = 3				; max fertilizer production per settlement per day
int maxStoredFertilizerBase = 10					; stop producing when we reach this amount stored

; vendor income
int minVendorIncomePopulation = 5 					; need at least this population to get any vendor income
float maxVendorIncome = 50.0 						; max daily vendor income from any settlement
float vendorIncomePopulationMult = 0.03 			; multiplier on population, added to vendor income
float vendorIncomeBaseMult = 2.0					; multiplier on base vendor income

; radio/attracting NPC stuff
int iMaxSurplusNPCs = 5 const 						; for now, max number of unassigned NPCs - if you have this many or more, no new NPCs will arrive.
float attractNPCDailyChance = 0.1 const   			; for now, roll <= to this to attract an NPC each day, modified by happiness
int iMaxBonusAttractChancePopulation = 5 const 		; for now, there's a bonus attract chance until the total population reaches this value
int iBaseMaxNPCs = 10 const 							; base total NPCs that can be at a player's town - this is used in GetMaxWorkshopNPCs formula
float attractNPCHappinessMult = 0.5 const 			; multiplier on happiness to attraction chance

; attack chance formula
float attackChanceBase = 0.02 const
float attackChanceResourceMult = 0.001 const
float attackChanceSafetyMult = 0.01 const
float attackChancePopulationMult = 0.005 const
float minDaysSinceLastAttack = 7.0 const 			;	minimum days before another attack can be rolled for

; damage
float damageDailyRepairBase = 5.0 const				;	amount of damage repaired per day (overall)
float damageDailyPopulationMult = 0.20 const		;	multiplier on population for repair:  repair = population * damageDailyPopulationMult * damageDailyRepairBase

; timer IDs
int buildWorkObjectTimerID = 0 const
int dailyUpdateTimerID = 1 const

; randomized wait for next try at DailyUpdate if system is busy
float maxDailyUpdateWaitHours = 1.0
float minDailyUpdateWaitHours = 0.2

;----------------STEVE CONQUEST SYSTEM ADDITIONS----------------
;/
Activator property Conquest_MinutemenFlag Auto
Activator property Conquest_BoSFlag Auto
Activator property Conquest_RailroadFlag Auto
Activator property Conquest_InstituteFlag Auto
Keyword property LocTypeControlPointOwner Auto
Quest property ConquestMasterQuest Auto
Quest property ConquestControlPoint Auto
/;
;-----------------------------------------------------------

;/
	OBSOLETE - no need to init actor value data on workshop
	(this was needed for keyword data because conditions wouldn't work at all for keyword data on locations if they weren't initialized to 0)

Event OnInit()
	;WorkshopParent.wsTrace(self + " initializing workshop")

	myLocation = GetCurrentLocation()
	; initialize keyword data on location
	if myLocation && myLocation.HasKeyword(WorkshopParent.LocTypeWorkshopSettlement)
		int i = 0
		; reference the WorkshopRatings from the parent using a local array to reduce hits on WorkshopParent.
		WorkshopDataScript:WorkshopRatingKeyword[] ratings = WorkshopParent.WorkshopRatings
		while i < ratings.Length
			myLocation.SetResourceData(ratings[i].resourceValue, 0.0)
			i = i + 1
		endWhile
	endif

endEvent
/;

; utility function to return vendor container array
; create containers as necessary
ObjectReference[] function GetVendorContainersByType(int vendorType)
	;WorkshopParent.wsTrace(self + " GetVendorContainersByType " + vendorType)
	if vendorType == 0
		if VendorContainersMisc == NONE
			VendorContainersMisc = InitializeVendorChests(vendorType)
		endif
		return VendorContainersMisc
	elseif vendorType == 1
		;WorkshopParent.wsTrace("  VendorContainersArmor = NONE?" + (VendorContainersArmor == NONE) )
		;WorkshopParent.wsTrace("  VendorContainersArmor.Length =" + VendorContainersArmor.Length)
		if VendorContainersArmor == NONE
			VendorContainersArmor = InitializeVendorChests(vendorType)
		endif
		return VendorContainersArmor
	elseif vendorType == 2
		if VendorContainersWeapons == NONE
			VendorContainersWeapons = InitializeVendorChests(vendorType)
		endif
		return VendorContainersWeapons
	elseif vendorType == 3
		if VendorContainersBar == NONE
			VendorContainersBar = InitializeVendorChests(vendorType)
		endif
		return VendorContainersBar
	elseif vendorType == 4
		if VendorContainersClinic == NONE
			VendorContainersClinic = InitializeVendorChests(vendorType)
		endif
		return VendorContainersClinic
	elseif vendorType == 5
		if VendorContainersClothing == NONE
			VendorContainersClothing = InitializeVendorChests(vendorType)
		endif
		return VendorContainersClothing
	else
		;WorkshopParent.wsTrace(self + " GetVendorContainersByType: WARNING - invalid vendorType=" + vendorType)
	endif
endFunction

ObjectReference[] function InitializeVendorChests(int vendorType)
	;WorkshopParent.wsTrace(self + " InitializeVendorChests: vendorType=" + vendorType)
	; initialize array
	int containerArraySize = WorkshopParent.VendorTopLevel + 1
	ObjectReference[] vendorContainers = new ObjectReference[containerArraySize]

	; create the chests
	FormList vendorContainerList = WorkshopParent.WorkshopVendorContainers[vendorType]
	int vendorLevel = 0
	while vendorLevel <= WorkshopParent.VendorTopLevel
		;WorkshopParent.wsTrace(" vendorLevel=" + vendorLevel)
		; create ref for each vendor level
		vendorContainers[vendorLevel] = WorkshopParent.WorkshopHoldingCellMarker.PlaceAtMe(vendorContainerList.GetAt(vendorLevel))
		;WorkshopParent.wsTrace(" 		container=" + vendorContainers[vendorLevel])
		vendorLevel += 1
	endWhile

	return vendorContainers
endFunction

Event OnInit()
    ; initialize building budget actor values if necessary
	if MaxTriangles > 0
		SetValue(WorkshopParent.WorkshopMaxTriangles, MaxTriangles)
	endif
	if MaxDraws > 0
		SetValue(WorkshopParent.WorkshopMaxDraws, MaxDraws)
	endif
	if CurrentTriangles > 0
		SetValue(WorkshopParent.WorkshopCurrentTriangles, CurrentTriangles)
	endif
	if CurrentDraws > 0
		SetValue(WorkshopParent.WorkshopCurrentDraws, CurrentDraws)
	endif

	; happiness target (don't want to set a default value)
	SetValue(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingHappinessTarget].resourceValue, WorkshopParent.startingHappinessTarget)
EndEvent

Event OnLoad()
	; for now, block activation if not "owned" by player
	BlockActivation(!OwnedByPlayer)
	; grab inventory from linked container if I'm a container myself
	if (GetBaseObject() as Container)
		; get linked container
		ObjectReference linkedContainer = GetLinkedRef(WorkshopParent.WorkshopLinkContainer)
		if linkedContainer
			linkedContainer.RemoveAllItems(self)
		endif

		; get all linked containers (children)
		ObjectReference[] linkedContainers = GetLinkedRefChildren(WorkshopParent.WorkshopLinkContainer)
		int i = 0
		while i < linkedContainers.Length
			linkedContainer = linkedContainers[i]
			if linkedContainer
				linkedContainer.RemoveAllItems(self)
			endif
			i += 1
		endWhile
	endif

	; if no location (this is not a settlement location so skipped WorkshopParent init process), get current location
	if !myLocation
		myLocation = GetCurrentLocation()
	endif
EndEvent

Event OnUnload()
	; reset days since last visit whenever you leave
	DaysSinceLastVisit = 0
endEvent

; block activation until player "owns" this workbench
Event OnActivate(ObjectReference akActionRef)
	;;debug.trace(self + "OnActivate")
	if akActionRef == Game.GetPlayer()
		CheckOwnership()

		if OwnedByPlayer
			; go into workshop mode
			StartWorkshop()
		endif
	endif
EndEvent

; called by OnActivate and workbench scripts to check if player ownership should change
function CheckOwnership()
		; if location is cleared, automatically count this as owned by player
		if myLocation.IsCleared() && !OwnedByPlayer && EnableAutomaticPlayerOwnership && !WorkshopParent.PermanentActorsAliveAndPresent(self)
			SetOwnedByPlayer(true)
			; go into workshop mode
;			StartWorkshop()
		endif

		if !OwnedByPlayer
			; which message to show?
			; Patch 1.4: handle custom unowned message
			if CustomUnownedMessage
				CustomUnownedMessage.Show()
			else
				int totalPopulation = GetBaseValue(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingPopulation].resourceValue) as int

				if totalPopulation > 0
					WorkshopParent.WorkshopUnownedSettlementMessage.Show()
				elseif myLocation.IsCleared() == false && EnableAutomaticPlayerOwnership
					WorkshopParent.WorkshopUnownedHostileMessage.Show()
				else
					WorkshopParent.WorkshopUnownedMessage.Show()
				endif
			endif
		endif
endFunction

Event OnWorkshopMode(bool aStart)
	;debug.trace(self + " OnWorkshopMode " + aStart)
	if aStart
		if OwnedByPlayer
			; make this the current workshop
			WorkshopParent.SetCurrentWorkshop(self)
		endif

		Var[] kargs = new Var[2]
		kargs[0] = NONE
		kargs[1] = self
		;WorkshopParent.wsTrace(" 	sending WorkshopEnterMenu event")
		WorkshopParent.SendCustomEvent("WorkshopEnterMenu", kargs)		
	endif

	; Dogmeat scene
	if aStart && WorkshopParent.DogmeatAlias.GetRef()
		WorkshopParent.WorkshopDogmeatWhileBuildingScene.Start()
	else
		WorkshopParent.WorkshopDogmeatWhileBuildingScene.Stop()
	endif

	; Companion scene
	;debug.trace(self + " OnWorkshopMode " + WorkshopParent.CompanionAlias.GetRef())
	if aStart && WorkshopParent.CompanionAlias.GetRef()
		;debug.trace(self + " starting WorkshopParent.WorkshopCompanionWhileBuildingScene")
		WorkshopParent.WorkshopCompanionWhileBuildingScene.Start()
	else
		WorkshopParent.WorkshopCompanionWhileBuildingScene.Stop()
	endif

	if !aStart
		; show message if haven't before
		if ShowedWorkshopMenuExitMessage == false
			ShowedWorkshopMenuExitMessage = true
			WorkshopParent.WorkshopExitMenuMessage.ShowAsHelpMessage("WorkshopMenuExit", 10.0, 0, 1, "NoMenu")
		endif
		; make sure resources stay assigned
		WorkshopParent.TryToAssignResourceObjectsPUBLIC(self)
	endif

	; try recalcing when you enter/exit workshop mode
	DailyUpdate(bRealUpdate = false)
endEvent

Event OnTimer(int aiTimerID)
	;WorkshopParent.wsTrace(self + " OnTimer: timerID=" + aiTimerID)
	if aiTimerID == buildWorkObjectTimerID
		WorkshopParent.TryToAssignResourceObjectsPUBLIC(self)
	endif
EndEvent

Event OnTimerGameTime(int aiTimerID)
	if aiTimerID == dailyUpdateTimerID
		if WorkshopParent.IsEditLocked() || WorkshopParent.DailyUpdateInProgress
			float waitTime = utility.RandomFloat(minDailyUpdateWaitHours, maxDailyUpdateWaitHours)
			WorkshopParent.wsTrace(self + " DailyUpdate: system busy, try again in " + waitTime + " game hours.")
			; run another timer - system is too busy
			StartTimerGameTime(waitTime, dailyUpdateTimerID)
		else
			DailyUpdate()
		endif
	endif
endEvent

function SetOwnedByPlayer(bool bIsOwned)
	;WorkshopParent.wsTrace(self + " SetOwnedByPlayer(" + bIsOwned + ")")

	; is state changing?
	if !bIsOwned && OwnedByPlayer
		;WorkshopParent.wsTrace(self + " SetOwnedByPlayer: state changing to UNOWNED")
		OwnedByPlayer = bIsOwned ; do this here so workshop is updated for UpdatePlayerOwnership check

		; display loss of ownership message
		WorkshopParent.DisplayMessage(WorkshopParent.WorkshopLosePlayerOwnership, NONE, myLocation)
		; flag as "lost control"
		SetValue(WorkshopParent.WorkshopPlayerLostControl, 1.0)
		; clear farm discount faction if we can get actors
		ObjectReference[] WorkshopActors = WorkshopParent.GetWorkshopActors(self)
		int i = 0
		while i < WorkshopActors.Length
			WorkshopNPCScript theActor = (WorkshopActors[i] as Actor) as WorkshopNPCScript
			if theActor
				theActor.RemoveFromFaction(WorkshopParent.FarmDiscountFaction)
				; clear "player owned" actor value (used to condition trade items greetings)
				theActor.UpdatePlayerOwnership(self)
			endif
			i += 1
		endWhile

		; remove all caravans to/from this settlement
		WorkshopParent.ClearCaravansFromWorkshopPUBLIC(self)

	elseif bIsOwned && !OwnedByPlayer
		;WorkshopParent.wsTrace(self + " SetOwnedByPlayer: state changing to OWNED")
		OwnedByPlayer = bIsOwned ; do this here so workshop is updated for UpdatePlayerOwnership check

		; make sure owns a workshop flag is set first time you own one
		if !WorkshopParent.PlayerOwnsAWorkshop
			WorkshopParent.PlayerOwnsAWorkshop = true
		endif

		; make sure happiness (and happiness target) is set to minimum (so doesn't immediately become unowned again)
		float currentHappiness = GetValue(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingHappiness].resourceValue)
		float currentHappinessTarget = GetValue(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingHappinessTarget].resourceValue)
		if currentHappiness < minHappinessClearWarningThreshold || currentHappinessTarget < minHappinessClearWarningThreshold
			WorkshopParent.ModifyResourceData(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingHappiness].resourceValue, self, minHappinessClearWarningThreshold)		
			WorkshopParent.ModifyResourceData(WorkshopParent.WorkshopRatings[WorkshopParent.WorkshopRatingHappinessTarget].resourceValue, self, minHappinessClearWarningThreshold)		
		EndIf

		; display gain of ownership message
		;WorkshopParent.wsTrace(self + " SetOwnedByPlayer - display message for location " + myLocation)
		WorkshopParent.DisplayMessage(WorkshopParent.WorkshopGainPlayerOwnership, NONE, myLocation)

		; if this is the first time player owns this, increment stat
		if GetValue(WorkshopParent.WorkshopPlayerLostControl) == 0
			;debug.trace("Increment Workshops Unlocked stat")
			Game.IncrementStat("Workshops Unlocked")
		else
			; clear "lost control" flag
			SetValue(WorkshopParent.WorkshopPlayerLostControl, 0.0)
		EndIf

		; if allowed, allow random quests from now on
		if MinRecruitmentAllowRandomAfterPlayerOwned
			MinRecruitmentProhibitRandom = false
		EndIf

		; allow attacks when unowned now
		AllowAttacksBeforeOwned = true

		; add player owned actor value if possible
		ObjectReference[] WorkshopActors = WorkshopParent.GetWorkshopActors(self)
		int i = 0
		while i < WorkshopActors.Length
			WorkshopNPCScript theActor = (WorkshopActors[i] as Actor) as WorkshopNPCScript
			if theActor
				theActor.UpdatePlayerOwnership(self)
			endif
			i += 1
		endWhile

	endif

	OwnedByPlayer = bIsOwned
	BlockActivation(!OwnedByPlayer)
	; set player ownership value on myself
	SetValue(WorkshopParent.WorkshopPlayerOwnership, (bIsOwned as float))
	;WorkshopParent.SetResourceData(WorkshopParent.WorkshopPlayerOwnership, self, (bIsOwned as float))

	if bIsOwned
		SetActorOwner(Game.GetPlayer().GetActorBase())
		; don't want this flagged as cleared so story manager doesn't skip it
		myLocation.SetCleared(false)
		; add player to ownership faction is there is one
		if SettlementOwnershipFaction && UseOwnershipFaction
			Game.GetPlayer().AddToFaction(SettlementOwnershipFaction)
		endif
	else
		SetActorOwner(NONE)
		; remove player from ownership faction is there is one
		if SettlementOwnershipFaction && UseOwnershipFaction
			Game.GetPlayer().RemoveFromFaction(SettlementOwnershipFaction)
		endif
	endif

	; send custom event for this workshop
	WorkshopParent.SendPlayerOwnershipChangedEvent(self)
endFunction

Event OnWorkshopObjectPlaced(ObjectReference akReference)
	;WorkshopParent.wsTrace(self + " received OnWorkshopObjectPlaced event")
	if WorkshopParent.BuildObjectPUBLIC(akReference, self)
		; run timer for assigning resource objects
		StartTimer(3.0, buildWorkObjectTimerID)
	endif
endEvent

Event OnWorkshopObjectMoved(ObjectReference akReference)
	;WorkshopParent.wsTrace(self + " received OnWorkshopObjectMoved event")
	; for now, just tell the object it moved so it can update markers etc.
	WorkshopObjectScript workshopObjectRef = akReference as WorkShopObjectScript

	if workshopObjectRef
;		workshopObjectRef.UpdatePosition()
		; send custom event for this object
		Var[] kargs = new Var[2]
		kargs[0] = workshopObjectRef
		kargs[1] = self
		;WorkshopParent.wsTrace(" 	sending WorkshopObjectMoved event")
		WorkshopParent.SendCustomEvent("WorkshopObjectMoved", kargs)		
	endif
EndEvent

Event OnWorkshopObjectDestroyed(ObjectReference akReference)
	;WorkshopParent.wsTrace(self + " received OnWorkshopObjectDestroyed event")
	WorkshopParent.RemoveObjectPUBLIC(akReference, self)
endEvent

Event OnWorkshopObjectRepaired(ObjectReference akReference)
	;WorkshopParent.wsTrace(self + " received OnWorkshopObjectRepaired event")
	WorkshopObjectActorScript workshopObjectActor = akReference as WorkshopObjectActorScript
	if workshopObjectActor
		;WorkshopParent.wsTrace(self + " repairing object actor " + workshopObjectActor)
		; send destruction state changed event manually since actors aren't destructible objects
		WorkshopObjectScript workshopObject = (akReference as WorkshopObjectScript)
		workshopObject.OnDestructionStageChanged(1, 0)
	endif

	WorkshopObjectScript workshopObjectRef = akReference as WorkShopObjectScript

	if workshopObjectRef
;		workshopObjectRef.UpdatePosition()
		; send custom event for this object
		Var[] kargs = new Var[2]
		kargs[0] = workshopObjectRef
		kargs[1] = self
		;WorkshopParent.wsTrace(" 	sending WorkshopObjectMoved event")
		WorkshopParent.SendCustomEvent("WorkshopObjectRepaired", kargs)		
	endif

endEvent

ObjectReference function GetContainer()
	if (GetBaseObject() as Container)
		return self
	else
		return GetLinkedRef(WorkshopParent.WorkshopLinkContainer)
	endif
endFunction

Event WorkshopParentScript.WorkshopDailyUpdate(WorkshopParentScript akSender, Var[] akArgs)
	; calculate custom time interval for this workshop (to stagger out the update process throughout the day)
	float waitTime = WorkshopParent.dailyUpdateIncrement * workshopID
	;WorkshopParent.wsTrace(self + " WorkshopDailyUpdate event received with wait time " + waitTime + " hours")
	StartTimerGameTime(waitTime, dailyUpdateTimerID)
EndEvent

; return max NPCs for this workshop
int function GetMaxWorkshopNPCs()
	; base + player's charisma
	int iMaxNPCs = iBaseMaxNPCs + (Game.GetPlayer().GetValue(WorkshopParent.Charisma) as int)
	return iMaxNPCs
endFunction

; Data struct for passing data to/from DailyUpdate helper functions
struct DailyUpdateData
	int totalPopulation
	int robotPopulation
	int brahminPopulation
	int unassignedPopulation
	float vendorIncome
	float currentHappiness
	float damageMult
	float productivity
	int availableBeds
	int shelteredBeds
	int bonusHappiness
	int happinessModifier
	int safety
	int safetyDamage
	int foodProduction
	int waterProduction
	int availableFood
	int availableWater
	int safetyPerNPC
	float totalHappiness
endStruct

; process daily update
; 	bRealUpdate:
;		TRUE = normal daily update - consume/produce resources, etc.
;		FALSE = recalculate happiness etc. but don't produce/consume
; Updated to handle lock, actor, and other related issues. 
; bDailyUpdateInProgress bool has been removed, and function
; just uses WorkshopParent.DailyUpdateInProgress 
function DailyUpdate(bool bRealUpdate = true)
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
	WorkshopParent.wsTrace(self + " 	DAILY UPDATE: bRealUpdate=" + bRealUpdate, bNormalTraceAlso = true)
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")

	; wait for update lock to be released
	if WorkshopParent.DailyUpdateInProgress
		if bRealUpdate
			while WorkshopParent.DailyUpdateInProgress
				WorkshopParent.wsTrace(self + "		waiting for update lock to clear...")
				utility.wait(0.5)
			endWhile
		else
			; just bail if not a real update - no need
			WorkshopParent.wsTrace(self + "		update already in progress - don't try again right now")
			return
		endif
	EndIf
	WorkshopParent.DailyUpdateInProgress = true

	; create local pointer to WorkshopRatings array to speed things up
	WorkshopDataScript:WorkshopRatingKeyword[] ratings = WorkshopParent.WorkshopRatings
	DailyUpdateData updateData = new DailyUpdateData

	; NOTE: GetBaseValue for these because we don't care if they can "produce" - actors that are wounded don't "produce" their population resource values
	updateData.totalPopulation = GetBaseValue(ratings[WorkshopParent.WorkshopRatingPopulation].resourceValue) as int
	updateData.robotPopulation = GetBaseValue(ratings[WorkshopParent.WorkshopRatingPopulationRobots].resourceValue) as int
	updateData.brahminPopulation = GetBaseValue(ratings[WorkshopParent.WorkshopRatingBrahmin].resourceValue) as int
	updateData.unassignedPopulation = GetBaseValue(ratings[WorkshopParent.WorkshopRatingPopulationUnassigned].resourceValue) as int

	updateData.vendorIncome = GetValue(ratings[WorkshopParent.WorkshopRatingVendorIncome].resourceValue) * vendorIncomeBaseMult
	updateData.currentHappiness = GetValue(ratings[WorkshopParent.WorkshopRatingHappiness].resourceValue)

	updateData.damageMult = 1 - GetValue(ratings[WorkshopParent.WorkshopRatingDamageCurrent].resourceValue)/100.0
	updateData.productivity = GetProductivityMultiplier(ratings)
	updateData.availableBeds = GetBaseValue(ratings[WorkshopParent.WorkshopRatingBeds].resourceValue) as int
	updateData.shelteredBeds = GetValue(ratings[WorkshopParent.WorkshopRatingBeds].resourceValue) as int
	updateData.bonusHappiness = GetValue(ratings[WorkshopParent.WorkshopRatingBonusHappiness].resourceValue) as int
	updateData.happinessModifier = GetValue(ratings[WorkshopParent.WorkshopRatingHappinessModifier].resourceValue) as int
	updateData.safety = GetValue(ratings[WorkshopParent.WorkshopRatingSafety].resourceValue) as int
	updateData.safetyDamage = GetValue(WorkshopParent.GetDamageRatingValue(ratings[WorkshopParent.WorkshopRatingSafety].resourceValue)) as int
	updateData.totalHappiness = 0.0	; sum of all happiness of each actor in town

	;WorkshopParent.wsTrace(self + "       total population=" + updateData.totalPopulation)

	; REAL UPDATE ONLY
	if bRealUpdate
		DailyUpdateAttractNewSettlers(ratings, updateData)
	EndIf

	ObjectReference containerRef = GetContainer()
	if !containerRef
		WorkshopParent.wsTrace(self + " ERROR - no container linked to workshop " + self + " with " + WorkshopParent.WorkshopLinkContainer, 2)
		WorkshopParent.DailyUpdateInProgress = false
		return
	endif

	;WorkshopParent.wsTrace(self + "		happiness: " + updateData.currentHappiness + ", productivity mult=" + updateData.productivity + ", damage mult=" + updateData.damageMult)

	; if this is current workshop, update actors (in case some have been wounded since last update)
	if GetWorkshopID() == WorkshopParent.WorkshopCurrentWorkshopID.GetValue()
		;WorkshopParent.wsTrace(self + "		Current workshop - update actors' work objects")

		ObjectReference[] WorkshopActors = WorkshopParent.GetWorkshopActors(self)
		int i = 0
		while i < WorkshopActors.Length
			; extra check to make sure only valid actors are used (to match similar GetWorkshopActors() calls in script)
			WorkshopNPCScript theActor = (WorkshopActors[i] as Actor) as WorkshopNPCScript
			if theActor
				WorkshopParent.UpdateActorsWorkObjects(theActor, self)
			endif
			i += 1
		endWhile
	endif

	DailyUpdateProduceResources(ratings, updateData, containerRef, bRealUpdate)

	DailyUpdateConsumeResources(ratings, updateData, containerRef, bRealUpdate)


	; REAL UPDATE ONLY:
	if bRealUpdate
		DailyUpdateSurplusResources(ratings, updateData, containerRef)

		RepairDamage()
	
		; now recalc all workshop resources if the current workshop - we don't want to do this when unloaded or everything will be 0
		RecalculateWorkshopResources()

		CheckForAttack()
	endif

	; update trade caravan list
	if updateData.totalPopulation >= WorkshopParent.TradeCaravanMinimumPopulation && GetValue(ratings[WorkshopParent.WorkshopRatingCaravan].resourceValue) > 0
		WorkshopParent.TradeCaravanWorkshops.AddRef(self)
	else
		WorkshopParent.TradeCaravanWorkshops.RemoveRef(self)
	EndIf

	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
	WorkshopParent.wsTrace(self + "	DAILY UPDATE - DONE - bRbRealUpdate=" + bRealUpdate, bNormalTraceAlso = true)
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")

	; clear update lock
	WorkshopParent.DailyUpdateInProgress = false
endFunction


; **********************************************************************************************
; DAILY UPDATE HELPER FUNCTIONS - to reduce memory footprint of DailyUpdate process
; **********************************************************************************************
function DailyUpdateAttractNewSettlers(WorkshopDataScript:WorkshopRatingKeyword[] ratings, DailyUpdateData updateData)
	; increment last visit counter each day
	DaysSinceLastVisit += 1

	; attract new NPCs
	; if I have a radio station
	int radioRating = GetValue(ratings[WorkshopParent.WorkshopRatingRadio].resourceValue) as int
	if radioRating > 0 && HasKeyword(WorkshopParent.WorkshopType02) == false && updateData.unassignedPopulation < iMaxSurplusNPCs && updateData.totalPopulation < GetMaxWorkshopNPCs()
		;WorkshopParent.wsTrace(self + "       RADIO - unassigned population=" + updateData.unassignedPopulation)
		float attractChance = attractNPCDailyChance + updateData.currentHappiness/100 * attractNPCHappinessMult
		if updateData.totalPopulation < iMaxBonusAttractChancePopulation
			attractChance += (iMaxBonusAttractChancePopulation - updateData.totalPopulation) * attractNPCDailyChance
		endif
		; roll to see if a new NPC arrives
		float dieRoll = utility.RandomFloat()
		;WorkshopParent.wsTrace(self + "			dieRoll=" + dieRoll + ", attract NPC chance=" + attractChance)

		if dieRoll <= attractChance
			WorkshopNPCScript newWorkshopActor = WorkshopParent.CreateActor(self)
			updateData.totalPopulation += 1

			if newWorkshopActor.GetValue(WorkshopParent.WorkshopGuardPreference) == 0
				; see if also generate a brahmin
				; for now just roll if no brahmin here yet
				if GetValue(ratings[WorkshopParent.WorkshopRatingBrahmin].resourceValue) == 0.0 && AllowBrahminRecruitment
					int brahminRoll = utility.RandomInt()
					;WorkshopParent.wsTrace(self + "			brahminRoll=" + brahminRoll + ", attract brahmin chance=" + WorkshopParent.recruitmentBrahminChance)
					if brahminRoll <= WorkshopParent.recruitmentBrahminChance
						actor newBrahmin = WorkshopParent.CreateActor(self, true)
						; NOTE: don't increment total population - brahmin aren't counted as population
					endif
				endif
			endif

		endif
	endif
endFunction

function DailyUpdateProduceResources(WorkshopDataScript:WorkshopRatingKeyword[] ratings, DailyUpdateData updateData, ObjectReference containerRef, bool bRealUpdate)
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
	WorkshopParent.wsTrace(self + "	Produce resources: ")
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")


	; produce food and water
	updateData.foodProduction = GetValue(ratings[WorkshopParent.WorkshopRatingFood].resourceValue) as int
	updateData.waterProduction = GetValue(ratings[WorkshopParent.WorkshopRatingWater].resourceValue) as int
	WorkshopParent.wsTrace(self + "			Base food: " + updateData.foodProduction)
	WorkshopParent.wsTrace(self + "			Base water: " + updateData.waterProduction)

	; safety check: population = base needed safety
	int missingSafety = math.max(0, updateData.totalPopulation - updateData.safety) as int
	WorkshopParent.SetResourceData(ratings[WorkshopParent.WorkshopRatingMissingSafety].resourceValue, self, missingSafety)

	; subtract damage from food & water
	updateData.foodProduction = math.max(0, updateData.foodProduction - (GetValue(WorkshopParent.GetDamageRatingValue(ratings[WorkshopParent.WorkshopRatingFood].resourceValue)) as int)) as int
	updateData.waterProduction = math.max(0, updateData.waterProduction - (GetValue(WorkshopParent.GetDamageRatingValue(ratings[WorkshopParent.WorkshopRatingWater].resourceValue)) as int)) as int

	; each brahmin can assist with 10 food production
	if updateData.brahminPopulation > 0
		int brahminMaxFoodBoost = math.min(updateData.brahminPopulation * maxProductionPerBrahmin, updateData.foodProduction) as int
		int brahminFoodProduction = math.Ceiling(brahminMaxFoodBoost * brahminProductionBoost)
		WorkshopParent.wsTrace(self + "			Brahmin food boost: " + brahminFoodProduction)
		updateData.foodProduction = updateData.foodProduction + brahminFoodProduction
	endif
	; MOVE THIS TO CONSUME SECTION:
	; - confusing if food rating is higher than population but still red
	; - consume first, then multiply the surplus by productivity
	;/
	; food rating is multiplied by productivity 
	updateData.foodProduction = math.Ceiling(updateData.foodProduction * updateData.productivity)
	/;

	; NOTE: water doesn't need a productivity factor since it's produced by machines - happiness is irrelevant
	; for now, store this as rating for stats to use
	WorkshopParent.SetResourceData(ratings[WorkshopParent.WorkshopRatingFoodActual].resourceValue, self, updateData.foodProduction)
	WorkshopParent.wsTrace(self + "			Actual production:")
	WorkshopParent.wsTrace(self + "				Food: " + updateData.foodProduction)
	WorkshopParent.wsTrace(self + "				Water: " + updateData.waterProduction)


	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
	WorkshopParent.wsTrace(self + "	Consume resources and calculate happiness: ")
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")

	
	; reduce safety by current damage (food and water already got that treatment in the Production phase)
	updateData.safety = math.Max(updateData.safety -  updateData.safetyDamage, 0) as int
	updateData.safetyPerNPC = 0
	if updateData.totalPopulation > 0
		updateData.safetyperNPC = math.ceiling(updateData.safety/updateData.totalPopulation)
	endif

	updateData.availableFood = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeFood)
	updateData.availableWater = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeWater)

	WorkshopParent.wsTrace(self + "	Starting stats:")
	WorkshopParent.wsTrace(self + "		population=" + updateData.totalPopulation)
	WorkshopParent.wsTrace(self + "		food=" + updateData.availableFood)
	WorkshopParent.wsTrace(self + "		water=" + updateData.availableWater)
	WorkshopParent.wsTrace(self + "		beds=" + updateData.availableBeds + " (" + updateData.shelteredBeds + " sheltered)")
	WorkshopParent.wsTrace(self + "		bonus happiness=" + updateData.bonusHappiness)
	WorkshopParent.wsTrace(self + "		total safety=" + updateData.safety)
	WorkshopParent.wsTrace(self + "		safety per NPC=" + updateData.safetyPerNPC)
	WorkshopParent.wsTrace(self + "		unassignedPopulation=" + updateData.unassignedPopulation)

	; get local food and water totals (including current production)
	updateData.availableFood = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeFood) + updateData.foodProduction
	updateData.availableWater = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeWater) + updateData.waterProduction

	WorkshopParent.wsTrace(self + "	After production:")
	WorkshopParent.wsTrace(self + "		food=" + updateData.availableFood)
	WorkshopParent.wsTrace(self + "		water=" + updateData.availableWater)

	; how much food & water is needed? (robots don't need either)
	int neededFood = updateData.totalPopulation - updateData.robotPopulation - updateData.availableFood
	int neededWater = updateData.totalPopulation - updateData.robotPopulation - updateData.availableWater

	; add in food and water from linked workshops if needed
	if neededFood > 0 || neededWater > 0
		WorkshopParent.wsTrace(self + "	Resource shortage - try to get from linked workshops:")
		WorkshopParent.wsTrace(self + "		food=" + neededFood)
		WorkshopParent.wsTrace(self + "		water=" + neededWater)
		WorkshopParent.TransferResourcesFromLinkedWorkshops(self, neededFood, neededWater)
	endif

	; now, get again (now including any transfers from linked workshops)
	updateData.availableFood = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeFood) + updateData.foodProduction
	updateData.availableWater = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeWater) + updateData.waterProduction

	WorkshopParent.wsTrace(self + "	Final stats after transfers:")
	WorkshopParent.wsTrace(self + "		food=" + updateData.availableFood)
	WorkshopParent.wsTrace(self + "		water=" + updateData.availableWater)

	; for simplicity, brahmin use food & water first
	; KMK 5/4/15 - remove brahmin food/water use
	;/
	if updateData.brahminPopulation > 0
		; if current production is available, use that first
		if updateData.availableFood > 0
			updateData.availableFood = updateData.availableFood - 1
			if updateData.foodProduction > 0
				updateData.foodProduction = updateData.foodProduction - 1
			elseif bRealUpdate
				containerRef.RemoveItem(WorkshopParent.WorkshopConsumeFood, 1)
			endif
		EndIf

		if updateData.availableWater > 0
			updateData.availableWater = updateData.availableWater - 1
			; if current production is available, use that first
			if updateData.waterProduction > 0
				updateData.waterProduction = updateData.waterProduction - 1
			elseif bRealUpdate
				containerRef.RemoveItem(WorkshopParent.WorkshopConsumeWater, 1)
			endif
		EndIf
	EndIf
	/;
endFunction

function DailyUpdateConsumeResources(WorkshopDataScript:WorkshopRatingKeyword[] ratings, DailyUpdateData updateData, ObjectReference containerRef, bool bRealUpdate)

	; don't need to do any of this if no population
	if updateData.totalPopulation == 0
		return
	endif

	; variables used to track happiness for each actor
	float ActorHappiness
	bool ActorBed
	bool ActorShelter
	bool ActorFood
	bool ActorWater

	; track how much is missing (contributing to unhappiness)
	int missingFood
	int missingWater
	int missingBeds
	int missingShelter
	int missingSafety

	; calculate happiness (but not for robots)
	int i = 0
	while i < updateData.totalPopulation - updateData.robotPopulation

		; clear variables
		ActorHappiness = 0
		ActorFood = false
		ActorWater = false
		ActorBed = false
		ActorShelter = false

		; we found an actor assigned to this workshop
		WorkshopParent.wsTrace(self + "	CalculateHappiness: actor " + i)
		; do I have food?
		if updateData.availableFood > 0
		; if so, +20
			ActorFood = true
			updateData.availableFood = updateData.availableFood - 1
			; if current production is available, use that first
			if updateData.foodProduction > 0
				updateData.foodProduction = updateData.foodProduction - 1
			elseif bRealUpdate
				containerRef.RemoveItem(WorkshopParent.WorkshopConsumeFood, 1)
			endif
			ActorHappiness += happinessBonusFood
			WorkshopParent.wsTrace(self + "   +FOOD - happiness=" + ActorHappiness)
		else
			missingFood += 1
		endif

		; do I have water?
		if updateData.availableWater > 0
		; if so, +20
			ActorWater = true
			updateData.availableWater = updateData.availableWater - 1
			; if current production is available, use that first
			if updateData.waterProduction > 0
				updateData.waterProduction = updateData.waterProduction - 1
			elseif bRealUpdate
				containerRef.RemoveItem(WorkshopParent.WorkshopConsumeWater, 1)
			endif
			ActorHappiness += happinessBonusWater
			WorkshopParent.wsTrace(self + "   +WATER - happiness=" + ActorHappiness)
		else
			missingWater += 1
		endif

		; bed?
		; if so, +10
		if updateData.availableBeds > 0
			ActorBed = true
			updateData.availableBeds = updateData.availableBeds - 1
			ActorHappiness += happinessBonusBed
			WorkshopParent.wsTrace(self + "   +BED - happiness=" + ActorHappiness)
		else
			missingBeds += 1
		endif

		; shelter?
		if updateData.shelteredBeds > 0
			ActorShelter = true
			updateData.shelteredBeds = updateData.shelteredBeds - 1
			ActorHappiness += happinessBonusShelter
			WorkshopParent.wsTrace(self + "   +SHELTER - happiness=" + ActorHappiness)
		endif

		; safety
		if updateData.safetyperNPC > 0
			ActorHappiness += happinessBonusSafety
		endif

		; now update happiness based on current flags
		ActorHappiness = CheckActorHappiness(ActorHappiness, ActorFood, ActorWater, ActorBed, ActorShelter)
		WorkshopParent.wsTrace(self + "   Final actor happiness=" + ActorHappiness)

		; sum happiness
		updateData.totalHappiness = updateData.totalHappiness + ActorHappiness
		i += 1
	endWhile

	; add happiness from robots - always 50
	updateData.totalHappiness = updateData.totalHappiness + ( 50 * updateData.robotPopulation)

	; always update missing beds stat - this isn't handled by code
	WorkshopParent.SetResourceData(ratings[WorkshopParent.WorkshopRatingMissingBeds].resourceValue, self, missingBeds)

	; record missing food/water/etc.
	; NEW - now that code calculates missing food/water/safety, only need to do this on real update
	if bRealUpdate
		WorkshopParent.SetResourceData(ratings[WorkshopParent.WorkshopRatingMissingFood].resourceValue, self, missingFood)
		WorkshopParent.SetResourceData(ratings[WorkshopParent.WorkshopRatingMissingWater].resourceValue, self, missingWater)
	endif

	; add "bonus happiness" and any happiness modifiers
	updateData.totalHappiness = updateData.totalHappiness + updateData.bonusHappiness

	; calculate happiness
	WorkshopParent.wsTrace(self + "	CalculateHappiness: totalHappiness=" + updateData.totalHappiness + ", totalActors=" + updateData.totalPopulation + ", happiness modifier=" + updateData.happinessModifier)
	; add happiness modifier here - it isn't dependent on population
	updateData.totalHappiness = math.max(updateData.totalHappiness/updateData.totalPopulation + updateData.happinessModifier, 0)
	; don't let happiness exceed 100
	updateData.totalHappiness = math.min(updateData.totalHappiness, 100)

	WorkshopParent.wsTrace(self + "	CalculateHappiness: Happiness Target=" + updateData.totalHappiness)
	; for now, record this as a rating
	WorkshopParent.SetResourceData(ratings[WorkshopParent.WorkshopRatingHappinessTarget].resourceValue, self, updateData.totalHappiness)

	; REAL UPDATE ONLY:
	if bRealUpdate
		float deltaHappinessFloat = (updateData.totalHappiness - updateData.currentHappiness) * happinessChangeMult
	;	WorkshopParent.wsTrace(self + "	CalculateHappiness: delta happiness (float)=" + deltaHappinessFloat)

		int deltaHappiness
		if deltaHappinessFloat < 0
			deltaHappiness = math.floor(deltaHappinessFloat)	; how much does happiness want to change?
		else
			deltaHappiness = math.ceiling(deltaHappinessFloat)	; how much does happiness want to change?
		endif

	;	WorkshopParent.wsTrace(self + "	CalculateHappiness: delta happiness (int)=" + deltaHappiness)
		if deltaHappiness != 0 && math.abs(deltaHappiness) < minHappinessChangePerUpdate
			; increase delta to the min
			deltaHappiness = minHappinessChangePerUpdate * (deltaHappiness/math.abs(deltaHappiness)) as int
		endif
		WorkshopParent.wsTrace(self + "	CalculateHappiness: happiness change=" + deltaHappiness)

		; update happiness rating on workshop's location
		WorkshopParent.ModifyResourceData(ratings[WorkshopParent.WorkshopRatingHappiness].resourceValue, self, deltaHappiness)


		; what is happiness now?
		float finalHappiness = GetValue(ratings[WorkshopParent.WorkshopRatingHappiness].resourceValue)
		WorkshopParent.wsTrace(self + "	CalculateHappiness: final happiness=" + finalHappiness)

		; achievement
		if finalHappiness >= WorkshopParent.HappinessAchievementValue
			;debug.Trace(self + " HAPPINESS ACHIEVEMENT UNLOCKED!!!!")
			Game.AddAchievement(WorkshopParent.HappinessAchievementID)
		endif

		; if happiness is below threshold, no longer player-owned
		if OwnedByPlayer && AllowUnownedFromLowHappiness
			; issue warning?
			if finalHappiness <= minHappinessWarningThreshold && HappinessWarning == false
				HappinessWarning = true
				; always show warning first
				WorkshopParent.DisplayMessage(WorkshopParent.WorkshopUnhappinessWarning, NONE, myLocation)
			elseif finalHappiness <= minHappinessThreshold
				SetOwnedByPlayer(false)
			endif

			; clear warning if above threshold
			if finalHappiness > minHappinessClearWarningThreshold && HappinessWarning == true
				HappinessWarning = false
			endif
		endif

		; happiness modifier tends toward 0 over time
		if updateData.happinessModifier != 0
			float modifierSign = -1 * (updateData.happinessModifier/math.abs(updateData.happinessModifier))
			WorkshopParent.wsTrace(self + "	CalculateHappiness: modifierSign=" + modifierSign)
			int deltaHappinessModifier
			float deltaHappinessModifierFloat = math.abs(updateData.happinessModifier) * modifierSign * happinessChangeMult
			WorkshopParent.wsTrace(self + "	CalculateHappiness: deltaHappinessModifierFloat=" + deltaHappinessModifierFloat)
			if deltaHappinessModifierFloat > 0
				deltaHappinessModifier = math.floor(deltaHappinessModifierFloat)	; how much does happiness modifier want to change?
			else
				deltaHappinessModifier = math.ceiling(deltaHappinessModifierFloat)	; how much does happiness modifier want to change?
			EndIf
			WorkshopParent.wsTrace(self + "	CalculateHappiness: deltaHappinessModifier=" + deltaHappinessModifier)

			if math.abs(deltaHappinessModifier) < happinessBonusChangePerUpdate
				deltaHappinessModifier = (modifierSign * happinessBonusChangePerUpdate) as int
			endif

			WorkshopParent.wsTrace(self + "	CalculateHappiness: FINAL deltaHappinessModifier=" + deltaHappinessModifier)
			if deltaHappinessModifier > math.abs(updateData.happinessModifier)
				WorkshopParent.SetHappinessModifier(self, 0)
			else
				WorkshopParent.ModifyHappinessModifier(self, deltaHappinessModifier)
			endif
		endif

	EndIf

endFunction

function DailyUpdateSurplusResources(WorkshopDataScript:WorkshopRatingKeyword[] ratings, DailyUpdateData updateData, ObjectReference containerRef)
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
	WorkshopParent.wsTrace(self + "	Add surplus to workshop container: ")
	WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
	; add surplus (if any) to container
	; check for max stored resources
	int currentStoredFood = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeFood)
	int currentStoredWater = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeWater)
	int currentStoredScavenge = containerRef.GetItemCount(WorkshopParent.WorkshopConsumeScavenge)
	int currentStoredFertilizer = containerRef.GetItemCount(WorkshopParent.WorkshopProduceFertilizer)

	WorkshopParent.wsTrace(self + "		Check stored resources: food=" + currentStoredFood + ", water=" + currentStoredWater + ", scavenge=" + currentStoredScavenge)

	bool bAllowFoodProduction = true
	if currentStoredFood > maxStoredFoodBase + maxStoredFoodPerPopulation * updateData.totalPopulation
		bAllowFoodProduction = false
	endif

	bool bAllowWaterProduction = true
	if currentStoredWater > maxStoredWaterBase + math.floor(maxStoredWaterPerPopulation * updateData.totalPopulation)
		bAllowWaterProduction = false
	endif

	bool bAllowScavengeProduction = true
	if currentStoredScavenge > maxStoredScavengeBase + maxStoredScavengePerPopulation * updateData.totalPopulation
		bAllowScavengeProduction = false
	endif
	
	bool bAllowFertilizerProduction = true
	if currentStoredFertilizer > maxStoredFertilizerBase
		bAllowFertilizerProduction = false
	endif

	maxBrahminFertilizerProduction

	WorkshopParent.wsTrace(self + "		Allow production? food: " + bAllowFoodProduction + ", water: " + bAllowWaterProduction + ", scavenge: " + bAllowScavengeProduction)
	; add to workshop container
	if updateData.foodProduction > 0 && bAllowFoodProduction
		; MOVED FROM PRODUCTION SECTION:
		; - previously, multiplied all food production by productivity
		; - but, it was confusing if base food rating was higher than population but still red
		; - NOW: consume first, then multiply the surplus by productivity
		; food rating is multiplied by productivity 
		updateData.foodProduction = math.Floor(updateData.foodProduction * updateData.productivity)
		WorkshopParent.wsTrace(self + "		FOOD SURPLUS: +" + updateData.foodProduction)
		if updateData.foodProduction > 0
			WorkshopParent.ProduceFood(self, updateData.foodProduction)
		endif
	endif
	if updateData.waterProduction > 0 && bAllowWaterProduction
		WorkshopParent.wsTrace(self + "		WATER SURPLUS: +" + updateData.waterProduction)
		containerRef.AddItem(WorkshopParent.WorkshopProduceWater, updateData.waterProduction)
	endif
	if updateData.brahminPopulation > 0 && bAllowFertilizerProduction
		int fertilizerProduction = Math.Min(updateData.brahminPopulation, maxBrahminFertilizerProduction) as int
		WorkshopParent.wsTrace(self + "		FERTILIZER PRODUCTION: +" + fertilizerProduction)
		containerRef.AddItem(WorkshopParent.WorkshopProduceFertilizer, fertilizerProduction)
	endif

	; scavenging by unassigned population, minus wounded population (not quite accurate but good enough)
	int scavengePopulation = (updateData.unassignedPopulation - GetValue(ratings[WorkshopParent.WorkshopRatingDamagePopulation].resourceValue)) as int

	; add in general scavenging rating
	int scavengeProductionGeneral = GetValue(ratings[WorkshopParent.WorkshopRatingScavengeGeneral].resourceValue) as int

	; scavenging is multiplied by productivity (happiness) and damage (wounded people)
	int scavengeAmount = math.Ceiling(scavengePopulation * updateData.productivity * updateData.damageMult + scavengeProductionGeneral*updateData.productivity)
	WorkshopParent.wsTrace(self + "		scavenge population: " + scavengePopulation + " unassigned, " + scavengeProductionGeneral + " dedicated scavengers")
	if scavengeAmount > 0 && bAllowScavengeProduction
		WorkshopParent.wsTrace(self + "		SCAVENGING: +" + scavengeAmount)
		containerRef.AddItem(WorkshopParent.WorkshopProduceScavenge, scavengeAmount)
	endif


	if updateData.vendorIncome > 0
		WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
		WorkshopParent.wsTrace(self + "	Vendor income: ")
		WorkshopParent.wsTrace(self + "------------------------------------------------------------------------------ ")
		WorkshopParent.wsTrace(self + "		Productivity mult: +" + updateData.productivity, bNormalTraceAlso = showVendorTraces)

		int vendorIncomeFinal = 0

		; get linked population with productivity excluded
		float linkedPopulation = WorkshopParent.GetLinkedPopulation(self, false)
		WorkshopParent.wsTrace(self + "		Linked population: +" + linkedPopulation, bNormalTraceAlso = showVendorTraces)
		float vendorPopulation = linkedPopulation + updateData.totalPopulation
		WorkshopParent.wsTrace(self + "		Total population: +" + vendorPopulation, bNormalTraceAlso = showVendorTraces)
		; only get income if population >= minimum
		if vendorPopulation >= minVendorIncomePopulation
			; get productivity-adjusted linked population
			linkedPopulation = WorkshopParent.GetLinkedPopulation(self, true)

			WorkshopParent.wsTrace(self + "		Linked population (productivity adjusted): +" + linkedPopulation, bNormalTraceAlso = showVendorTraces)

			; our population also gets productivity factor
			vendorPopulation = updateData.totalPopulation * updateData.productivity + linkedPopulation
			WorkshopParent.wsTrace(self + "		Total vendor population: " + vendorPopulation, bNormalTraceAlso = showVendorTraces)
			WorkshopParent.wsTrace(self + "		Base income: +" + updateData.vendorIncome, bNormalTraceAlso = showVendorTraces)
			float incomeBonus = updateData.vendorIncome * vendorIncomePopulationMult * vendorPopulation
			WorkshopParent.wsTrace(self + "		Population bonus: +" + incomeBonus, bNormalTraceAlso = showVendorTraces)
			updateData.vendorIncome = updateData.vendorIncome + incomeBonus
			; vendor income is multiplied by productivity (happiness)
			vendorIncomeFinal = math.Ceiling(updateData.vendorIncome)
			; don't go above max allowed
			vendorIncomeFinal = math.Min(vendorIncomeFinal, maxVendorIncome) as int
			; add to workshop container
			if vendorIncomeFinal >= 1.0
				containerRef.AddItem(WorkshopParent.WorkshopProduceVendorIncome, vendorIncomeFinal)
			endif	
		endif
		WorkshopParent.wsTrace(self + "		VENDOR INCOME: " + vendorIncomeFinal, bNormalTraceAlso = showVendorTraces)
	EndIf

endFunction
; ***********************************************
; END DAILY UPDATE HELPER FUNCTIONS
; ***********************************************



; TEMP:
bool showVendorTraces = true

function RepairDamage()
	WorkshopParent.wsTrace("	Repair damage: " + self)
	; create local pointer to WorkshopRatings array to speed things up
	WorkshopDataScript:WorkshopRatingKeyword[] ratings = WorkshopParent.WorkshopRatings

	; repair damage to each resource
	RepairDamageToResource(ratings[WorkshopParent.WorkshopRatingFood].resourceValue)
	RepairDamageToResource(ratings[WorkshopParent.WorkshopRatingWater].resourceValue)
	RepairDamageToResource(ratings[WorkshopParent.WorkshopRatingSafety].resourceValue)
	RepairDamageToResource(ratings[WorkshopParent.WorkshopRatingPower].resourceValue)
	RepairDamageToResource(ratings[WorkshopParent.WorkshopRatingPopulation].resourceValue)

	; repair damage (this is an overall rating that won't exactly match each resource rating)
	float currentDamage = GetValue(ratings[WorkshopParent.WorkshopRatingDamageCurrent].resourceValue)
	if currentDamage > 0
		; update current damage rating
		WorkshopParent.UpdateCurrentDamage(self)
	endif
endFunction

function RepairDamageToResource(ActorValue resourceValue)
	; get corresponding damage actor value
	ActorValue damageRating = WorkshopParent.GetDamageRatingValue(resourceValue)

	; create local pointer to WorkshopRatings array to speed things up
	WorkshopDataScript:WorkshopRatingKeyword[] ratings = WorkshopParent.WorkshopRatings

	bool bPopulationDamage = ( damageRating == ratings[WorkshopParent.WorkshopRatingDamagePopulation].resourceValue )

	; get current damage - population is a special case
	float currentDamage
	if bPopulationDamage
		currentDamage = WorkshopParent.GetPopulationDamage(self)
	else
		currentDamage = GetValue(damageRating)
	endif

	WorkshopParent.wsTrace(self + "   RepairDamageToResource: damageRating=" + damageRating + " bPopulationDamage=" + bPopulationDamage)


	int currentWorkshopID = WorkshopParent.WorkshopCurrentWorkshopID.GetValueInt()
	if currentDamage > 0
		; amount repaired
		WorkshopParent.wsTrace(self + "   RepairDamageToResource: " + currentDamage + " for " + " resourceValue=" + resourceValue)

		
		; scale this by population (uninjured) - unless this is population
		float repairAmount = 1
		bool bHealedActor = false 	; set to true if we find an actor to heal
		if damageRating != ratings[WorkshopParent.WorkshopRatingDamagePopulation].resourceValue
			repairAmount = CalculateRepairAmount(ratings)
			repairAmount = math.Max(repairAmount, 1.0)
			WorkshopParent.wstrace("		repair amount=" + repairAmount)
		else
			; if this is population, try to heal an actor:
			; are any of this workshop's NPC assigned to caravans?
			Location[] linkedLocations = myLocation.GetAllLinkedLocations(WorkshopParent.WorkshopCaravanKeyword)
			if linkedLocations.Length > 0
				; there is at least 1 actor - find them
				; loop through caravan actors
				int index = 0
				while (index < WorkshopParent.CaravanActorAliases.GetCount())
					; check this actor - is he owned by this workshop?
					WorkShopNPCScript caravanActor = WorkshopParent.CaravanActorAliases.GetAt(index) as WorkshopNPCScript
					if caravanActor && caravanActor.GetWorkshopID() == workshopID && caravanActor.IsWounded()
					; is this actor wounded? if so heal and exit
						bHealedActor = true
						WorkshopParent.WoundActor(caravanActor, false)
						return
					endif
					index += 1
				endwhile
			endif

			if !bHealedActor
				; if this is the current workshop, we can try to heal one of the actors (otherwise we don't have them)
				if workshopID == currentWorkshopID
					int i = 0
					ObjectReference[] WorkshopActors = WorkshopParent.GetWorkshopActors(self)
					while i < WorkshopActors.Length && !bHealedActor
						WorkShopNPCScript theActor = WorkshopActors[i] as WorkShopNPCScript
						if theActor && theActor.IsWounded()
							bHealedActor = true
							WorkshopParent.WoundActor(theActor, false)
						endif
						i += 1
					endWhile
				endif
			endif
		endif

		if !bHealedActor
			; if we healed an actor, keyword data already modified
			repairAmount = math.min(repairAmount, currentDamage)
			WorkshopParent.ModifyResourceData(damageRating, self, repairAmount*-1.0)
			WorkshopParent.wsTrace("		workshopID=" + workshopID + ", currentWorkshopID=" + currentWorkshopID)
			; if this is the current workshop, find an object to repair (otherwise we don't have them)
			if workshopID == currentWorkshopID && damageRating != ratings[WorkshopParent.WorkshopRatingDamagePopulation].resourceValue
				WorkshopParent.wsTrace("		Current workshop - find item(s) to repair " + repairAmount + " damage...")
				int i = 0
				; want only damaged objects that produce this resource
				ObjectReference[] ResourceObjects = GetWorkshopResourceObjects(akAV = resourceValue, aiOption = 1)
				while i < ResourceObjects.Length && repairAmount > 0
					WorkShopObjectScript theObject = ResourceObjects[i] as WorkShopObjectScript
					float damage = theObject.GetResourceDamage(resourceValue)
					WorkshopParent.wstrace("		" + theObject + "=" + damage + " damage")
					if damage > 0
						float modDamage = math.min(repairAmount, damage)*-1.0
						if theObject.ModifyResourceDamage(resourceValue, modDamage)
							repairAmount += modDamage
						endif
					endif
					i += 1
				endWhile
			endif
		endif
	endif
endFunction

; NOTE: pass in ratings array to speed things up since this is often part of an iteration
float function CalculateRepairAmount(WorkshopDataScript:WorkshopRatingKeyword[] ratings)
	; RESOURCE CHANGE: now GetValue(population) is the unwounded population; GetBaseValue() is total population
	float uninjuredPopulation = GetValue(ratings[WorkshopParent.WorkshopRatingPopulation].resourceValue)
	float productivityMult = GetProductivityMultiplier(ratings)
	float amountRepaired = math.Ceiling(uninjuredPopulation * damageDailyPopulationMult * damageDailyRepairBase * productivityMult)
	;WorkshopParent.wstrace("		CalculateRepairAmount " + self + ": uninjured population=" + uninjuredPopulation + ", productivityMult=" + productivityMult + ":  amount repaired=" + amountRepaired)
	return amountRepaired
endFunction

function CheckForAttack(bool bForceAttack = false)
	; bForceAttack = true  - don't roll, automatically trigger attack

	WorkshopParent.wsTrace("------------------------------------------------------------------------------ ")
	WorkshopParent.wsTrace("	Check for attack: " + self)
	WorkshopParent.wsTrace("------------------------------------------------------------------------------ ")

	; create local pointer to WorkshopRatings array to speed things up
	WorkshopDataScript:WorkshopRatingKeyword[] ratings = WorkshopParent.WorkshopRatings

	; PREVIOUS ATTACKS:
	; increment days since last attack
	WorkshopParent.ModifyResourceData(ratings[WorkshopParent.WorkshopRatingLastAttackDaysSince].resourceValue, self, 1.0)

	; attacks allowed at all?
	if AllowAttacks == false
		WorkshopParent.wsTrace("		attacks not allowed - no attack roll for " + self)
		return
	EndIf

	; don't attack unowned workshop if flag unless allowed
	if AllowAttacksBeforeOwned == false && OwnedByPlayer == false && bForceAttack == false
		WorkshopParent.wsTrace("		attacks on unowned workshop not allowed - no attack roll for " + self)
		return
	endif

	; NEW ATTACK:
	ObjectReference containerRef = GetContainer()
	if !containerRef
		WorkshopParent.wsTrace(self + " ERROR - no container linked to workshop " + self + " with " + WorkshopParent.WorkshopLinkContainer, 2)
		return
	endif

	int totalPopulation = GetBaseValue(ratings[WorkshopParent.WorkshopRatingPopulation].resourceValue) as int
	int safety = GetValue(ratings[WorkshopParent.WorkshopRatingSafety].resourceValue) as int
	int safetyPerNPC = 0
	if totalPopulation > 0
		safetyperNPC = math.ceiling(safety/totalPopulation)
	elseif bForceAttack
		safetyperNPC = safety
	else
		; no population - no attack
		WorkshopParent.wsTrace("		0 population - no attack roll")
		return
	endif

	int daysSinceLastAttack = GetValue(ratings[WorkshopParent.WorkshopRatingLastAttackDaysSince].resourceValue) as int
	if minDaysSinceLastAttack > daysSinceLastAttack && !bForceAttack
		; attack happened recently - no new attack
		WorkshopParent.wsTrace("		" + daysSinceLastAttack + " days since last attack - no attack roll")
		return
	endif

	int foodRating = GetTotalFoodRating(ratings)
	int waterRating = GetTotalWaterRating(ratings)

	WorkshopParent.wsTrace("	Starting stats:")
	WorkshopParent.wsTrace("		population=" + totalPopulation)
	WorkshopParent.wsTrace("		food rating=" + foodRating)
	WorkshopParent.wsTrace("		water rating=" + waterRating)
	WorkshopParent.wsTrace("		total safety=" + safety)
	WorkshopParent.wsTrace("		safety per NPC=" + safetyPerNPC)

	; chance of attack:
	; 	base chance + (food/water rating) - safety - total population
	WorkshopParent.wsTrace("	Attack chance:")
	WorkshopParent.wsTrace("		base chance=" + attackChanceBase)
	WorkshopParent.wsTrace("		resources=+" + attackChanceResourceMult * (foodRating + waterRating))
	WorkshopParent.wsTrace("		safety=-" + attackChanceSafetyMult*safety)
	WorkshopParent.wsTrace("		population=-" + attackChancePopulationMult * totalPopulation)

	float attackChance = attackChanceBase + attackChanceResourceMult * (foodRating + waterRating) - attackChanceSafetyMult*safety - attackChancePopulationMult * totalPopulation
	if attackChance < attackChanceBase
		attackChance = attackChanceBase
	endif
	WorkshopParent.wsTrace("		TOTAL=" + attackChance)

	float attackRoll = Utility.RandomFloat()
	WorkshopParent.wsTrace("	Attack roll = " + attackRoll)
	if attackRoll <= attackChance || bForceAttack
		int attackStrength = WorkshopParent.CalculateAttackStrength(foodRating, waterRating)
		WorkshopParent.TriggerAttack(self, attackStrength)
	endif
endFunction

; helper function to calculate total food = food production + inventory
int function GetTotalFoodRating(WorkshopDataScript:WorkshopRatingKeyword[] ratings)
	int foodRating = GetValue(ratings[WorkshopParent.WorkshopRatingFood].resourceValue) as int
	foodRating = foodRating + GetContainer().GetItemCount(WorkshopParent.WorkshopConsumeFood)
	;WorkshopParent.wstrace(self + " GetTotalFoodRating=" + foodRating)
	return foodRating
endFunction

; helper function to calculate total water = water production + inventory
int function GetTotalWaterRating(WorkshopDataScript:WorkshopRatingKeyword[] ratings)
	int waterRating = GetValue(ratings[WorkshopParent.WorkshopRatingWater].resourceValue) as int
	waterRating = waterRating + GetContainer().GetItemCount(WorkshopParent.WorkshopConsumeWater)
	;WorkshopParent.wstrace(self + " GetTotalWaterRating=" + waterRating)
	return waterRating
endFunction

; helper function - add modValue to the specified actor's happiness
; holds the rules for how happiness can go up based on the actor's various ratings (food, water, shelter, etc.)
float function CheckActorHappiness(float currentHappiness, bool bFood, bool bWater, bool bBed, bool bShelter)
	; check rules
	if !bWater && currentHappiness > maxHappinessNoWater
		; max happiness with no water is maxHappinessNoWater
		currentHappiness = maxHappinessNoWater
	endif

	if !bFood && currentHappiness > maxHappinessNoFood
		; max happiness with no food is maxHappinessNoFood
		currentHappiness = maxHappinessNoFood
	endif

	if !bShelter && currentHappiness > maxHappinessNoShelter
		; max happiness with no shelter is maxHappinessNoShelter
		currentHappiness = maxHappinessNoShelter
	endif

	return currentHappiness
endFunction

; get productivity multiplier for this workshop
float function GetProductivityMultiplier(WorkshopDataScript:WorkshopRatingKeyword[] ratings)
	float currentHappiness = GetValue(ratings[WorkshopParent.WorkshopRatingHappiness].resourceValue)
	return minProductivity + (currentHappiness/100) * (1 - minProductivity)
endFunction

int function GetWorkshopID()
	if workshopID < 0
		InitWorkshopID(WorkshopParent.GetWorkshopID(self))
	endif
	return workshopID
endFunction

function InitWorkshopID(int newWorkshopID)
	if workshopID < 0
		workshopID = newWorkshopID
	endif
endFunction

; helper function to recalc
; we don't normally want to do this when unloaded or everything will be 0
; TRUE = we did recalc; FALSE = we didn't
bool function RecalculateWorkshopResources(bool bOnlyIfLocationLoaded = true)
	if bOnlyIfLocationLoaded == false || myLocation.IsLoaded()
		;WorkshopParent.wstrace(self + " RecalculateWorkshopResources=TRUE")
		RecalculateResources()
		return true
	else
		;WorkshopParent.wstrace(self + " RecalculateWorkshopResources=FALSE")
		return false
	endif
endFunction
