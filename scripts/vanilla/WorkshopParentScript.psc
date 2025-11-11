Scriptname WorkshopParentScript extends Quest Hidden Conditional
{parent workshop script to hold global data}

;/
General workshop documentation: http://institute.bethsoft.com/index.php?title=Workshop_Systems_Design

Overview:
* Workshops array is the master list of all workshops in the game. This is set up in the editor.
* Most workshop data is not stored here - in general, full data (actors, objects) is available only for the current (loaded) workshop
* For unloaded workshops, data is stored on the workshop's location using Keyword data.

The main things driving this script:
* Daily update - currently based on a 24-hour timer.
  Primary function:
	DailyWorkshopUpdate
		This loops through all the workshops in the Workshop array and updates them (production, consumption, happiness, repair, etc.)

* Building/Removing something - called by WorkshopScript when it receives the build/remove event
  Primary functions:
	BuildObjectPUBLIC
	RemoveObjectPUBLIC

* Reset current workshop data - when player arrives at a new workshop location, data is refreshed - workshop script only holds full data for a single workshop
  Primary function:
	ResetWorkshop

* Assign an actor to a workshop - currently done by test NPC spawner, or by daily update if the radio is "on"
  Primary function:
	CreateActorPUBLIC

* Assign an actor to a resource object/bed or caravan
  Primary functions:
	AssignActorToObjectPUBLIC
	AssignCaravanActorPUBLIC

* Random raider attacks can resolve off-screen if necessary
  Primary function:
	ResolveAttack
/;

import CommonArrayFunctions
import WorkshopDataScript

Group WorkshopRatingsGroup

	WorkshopRatingKeyword[] Property WorkshopRatings Auto Const
	{
		0 = food - base production
		1 = happiness (town's happiness rating)
		2 = population
		3 = safety
		4 = water
		5 = power
		6 = beds
		7 = bonus happiness (output from town, used when calculating actual Happiness rating)
		8 = unassigned population (people not assigned to a specific job)
		9 = radio (for now just 1/0 - maybe later it will be "strength" of station or something)
		10 = current damage (current % damage from raider attacks)
		11 = max damage (max % damage from last attack)
		12 = days since last attack (it will be 0 if just attacked)
		13 = current damage to food - resource points that are damaged
		14 = current damage to water - resource points that are damaged
		15 = current damage to safety - resource points that are damaged
		16 = current damage to power - resource points that are damaged
		17 = extra damage to population - number of wounded people. NOTE: total damage = base population value - current population value + extra population damage
		18 = quest-related happiness modifier
		19 = food - actual production
		20 = happiness target - where is happiness headed
		21 = artillery
		22 = current damage to artillery - resource points that are damaged
		23 = last attacking faction ID (see Followers.GetEncDefinition() for list of factions)
		24 = robot population (so, number of humans = population - robot population)
		25 = base income from vendors
		26= brahmin population - used for food use plus increasing food production
		27 = MISSING food - amount needed for 0 unhappiness from food
		28 = MISSING water - amount needed for 0 unhappiness from water
		29 = MISSING beds - amount needed for 0 unhappiness from beds
		30 = scavenging - general
		31 = scavenging - building materials
		32 = scavenging - machine parts
		33 = scavenging - rare items
		34 = caravan - greater than 0 means on caravan route
		35 = food type - carrot - so that production can match crops
		36 = food type - corn - so that production can match crops
		37 = food type - gourd - so that production can match crops
		38 = food type - melon - so that production can match crops
		39 = food type - mutfruit - so that production can match crops
		40 = food type - razorgrain - so that production can match crops
		41 = food type - tarberry - so that production can match crops
		42 = food type - tato - so that production can match crops
		43 = synth population (meaning hostile Institute agents): > 0 means there's a hidden synth at the settlement
		44 = MISSING safety - amount needed for minimum risk of attack
	}


	; location data keywords
	ActorValue[] Property WorkshopRatingValues Auto
	{ for now this is created at runtime from the actor values in WorkshopRatings array, so we can use Find to get index with actor values }

	WorkshopActorValue[] Property WorkshopResourceAVs Auto
	{ created at runtime - list of resource actor values used by WorkshopObjects }

	; index "enums" to match the above
	int Property WorkshopRatingFood = 0 autoReadOnly
	int Property WorkshopRatingHappiness = 1 autoReadOnly
	int Property WorkshopRatingPopulation = 2 autoReadOnly
	int Property WorkshopRatingSafety = 3 autoReadOnly
	int Property WorkshopRatingWater = 4 autoReadOnly
	int Property WorkshopRatingPower = 5 autoReadOnly
	int Property WorkshopRatingBeds = 6 autoReadOnly
	int Property WorkshopRatingBonusHappiness = 7 autoReadOnly
	int Property WorkshopRatingPopulationUnassigned = 8 autoReadOnly
	int Property WorkshopRatingRadio = 9 autoReadOnly
	int Property WorkshopRatingDamageCurrent = 10 autoReadOnly
	int Property WorkshopRatingDamageMax = 11 autoReadOnly
	int Property WorkshopRatingLastAttackDaysSince = 12 autoReadOnly
	int Property WorkshopRatingDamageFood = 13 autoReadOnly
	int Property WorkshopRatingDamageWater = 14 autoReadOnly
	int Property WorkshopRatingDamageSafety = 15 autoReadOnly
	int Property WorkshopRatingDamagePower = 16 autoReadOnly
	int Property WorkshopRatingDamagePopulation = 17 autoReadOnly
	int Property WorkshopRatingHappinessModifier = 18 autoReadOnly
	int Property WorkshopRatingFoodActual = 19 autoReadOnly
	int Property WorkshopRatingHappinessTarget = 20 autoReadOnly
	int Property WorkshopRatingArtillery = 21 autoReadOnly
	int Property WorkshopRatingDamageArtillery = 22 autoReadOnly
	int Property WorkshopRatingLastAttackFaction = 23 autoReadOnly
	int Property WorkshopRatingPopulationRobots = 24 autoReadOnly
	int Property WorkshopRatingVendorIncome = 25 autoReadOnly
	int Property WorkshopRatingBrahmin = 26 autoReadOnly
	int Property WorkshopRatingMissingFood = 27 autoReadOnly
	int Property WorkshopRatingMissingWater = 28 autoReadOnly
	int Property WorkshopRatingMissingBeds = 29 autoReadOnly
	int Property WorkshopRatingScavengeGeneral = 30 autoReadOnly
	int Property WorkshopRatingScavengeBuilding = 31 autoReadOnly
	int Property WorkshopRatingScavengeParts = 32 autoReadOnly
	int Property WorkshopRatingScavengeRare = 33 autoReadOnly
	int Property WorkshopRatingCaravan = 34 autoReadOnly
	int Property WorkshopRatingFoodTypeCarrot = 35 autoReadOnly
	int Property WorkshopRatingFoodTypeCorn = 36 autoReadOnly
	int Property WorkshopRatingFoodTypeGourd = 37 autoReadOnly
	int Property WorkshopRatingFoodTypeMelon = 38 autoReadOnly
	int Property WorkshopRatingFoodTypeMutfruit = 39 autoReadOnly
	int Property WorkshopRatingFoodTypeRazorgrain = 40 autoReadOnly
	int Property WorkshopRatingFoodTypeTarberry = 41 autoReadOnly
	int Property WorkshopRatingFoodTypeTato = 42 autoReadOnly
	int Property WorkshopRatingPopulationSynths = 43 autoReadOnly
	int Property WorkshopRatingMissingSafety = 44 autoReadOnly

EndGroup

; array of all workshops - set in the editor
; index is the "workshopID" of that workshop
Group WorkshopMasterList
	RefCollectionAlias Property WorkshopsCollection Auto Const
	{ pointer to ref collection of workshops }

	WorkshopScript[] Property Workshops Auto
	{ Array of all workshops
	  index is the "workshopID" of that workshop
	  initialized at runtime
	}
	Location[] Property WorkshopLocations Auto
	{ associated locations - initialized at runtime }
EndGroup

FormList Property WorkshopCrimeFactions Auto const mandatory
{ used to set crime faction on all workshops that don't have one }

; Data related to current workshop = workshop the player is currently interacting with
int currentWorkshopID = -1
; global tracking this value
GlobalVariable Property WorkshopCurrentWorkshopID const auto

Group CurrentWorkshopData
	ReferenceAlias Property CurrentWorkshop auto 
	{ current workshop - in this alias }

	ReferenceAlias Property WorkshopCenterMarker Auto
	{center marker of the current workshop, used for packages
	}

	ReferenceAlias Property WorkshopNewSettler Auto const
	{ used for new settler intro scenes }

	ReferenceAlias Property WorkshopSpokesmanAfterRaiderAttack Auto const
	{ used for post raider attack scenes }

EndGroup

Group VendorTypes
	int Property WorkshopTypeMisc = 0 autoReadOnly
	int Property WorkshopTypeArmor = 1 autoReadOnly
	int Property WorkshopTypeWeapons = 2 autoReadOnly
	int Property WorkshopTypeBar = 3 autoReadOnly
	int Property WorkshopTypeClinic = 4 autoReadOnly
	int Property WorkshopTypeClothing = 5 autoReadOnly
	int Property WorkshopTypeChems = 6 autoReadOnly

	WorkshopVendorType[] Property WorkshopVendorTypes Auto Const
	{ array of flags indicating whether the top level vendor is now available }

	int Property VendorTopLevel = 2 auto const
	{ what level makes you a "top level" vendor? Currently there are exactly 3 levels of vendor: 0-2 }

	FormList[] Property WorkshopVendorContainers Auto Const
	{ list of form lists, indexed by VendorType 
		- each form list is indexed by vendor level
	}
EndGroup

Group FarmDiscount 
	Faction Property FarmDiscountFaction const auto mandatory
	{ remove from all settlement NPCs when settlement becomes unallied }

	FarmDiscountVendor[] Property FarmDiscountVendors Auto Const
	{ list of discount vendors }
EndGroup


RefCollectionAlias Property PermanentActorAliases Auto const mandatory
{ref alias collection of non-workshop actors who have been permanently moved to workshops
 the alias gives them a package to keep them at their new workshop "home"
 }

Group TradeCaravans
	RefCollectionAlias Property TradeCaravanWorkshops Auto Const
	{ pointer to ref collection of workshops }

	int Property TradeCaravanMinimumPopulation = 5 auto const
	{ minimum population for a settlement to count as a valid trade caravan destination }
EndGroup

ReferenceAlias Property WorkshopActorApply Auto const mandatory
{used to "stamp" workshop NPCs with alias data (packages, etc.) that they will retain without having to be in the aliases
}

Group Dogmeat
	ReferenceAlias Property DogmeatAlias Auto const mandatory
	{ Dogmeat companion alias - used to check when turning idle scene on and off }

	Scene Property WorkshopDogmeatWhileBuildingScene Auto const mandatory
	{ Dogmeat idle scene }
EndGroup

Group Companion
	ReferenceAlias Property CompanionAlias Auto const mandatory
	{ Companion alias - used to check when turning idle scene on and off }

	Scene Property WorkshopCompanionWhileBuildingScene Auto const mandatory
	{ Companion idle scene }
EndGroup


Group Messages
	ReferenceAlias Property MessageRefAlias Const Auto  mandatory
	{ used for inserting text into messages }

	LocationAlias Property MessageLocationAlias Const Auto mandatory
	{ used for inserting text into messages }

	Message Property WorkshopLosePlayerOwnership const auto mandatory
	Message Property WorkshopGainPlayerOwnership const auto mandatory
	Message Property WorkshopUnhappinessWarning const auto mandatory

	Message Property WorkshopUnownedMessage const auto mandatory
	Message Property WorkshopUnownedHostileMessage const auto mandatory
	Message Property WorkshopUnownedSettlementMessage const auto mandatory
	Message Property WorkshopOwnedMessage const auto mandatory

	Message Property WorkshopTutorialMessageBuild const auto

	Message Property WorkshopResourceAssignedMessage const auto mandatory
	{ message when a resource is successfully assigned }

	Message Property WorkshopResourceNoAssignmentMessage Auto Const mandatory
	{ message that this object can't be assigned to this NPC }

	Message Property WorkshopExitMenuMessage Auto Const mandatory
	{ message the first time you exit workshop mode at each workshop }
EndGroup

Group CaravanActorData
	RefCollectionAlias Property CaravanActorAliases Auto const mandatory
	{ref alias collection of actors assigned to caravans
	 }

	RefCollectionAlias Property CaravanActorRenameAliases Auto const mandatory
	{ref alias collection of actors assigned to caravans - these get renamed "Provisioner"
		(subset of CaravanActorAliases)
	 }

	RefCollectionAlias Property CaravanBrahminAliases Auto const mandatory
	{ref alias collection of brahmins assigned to caravans
	 }

	Keyword Property WorkshopLinkCaravanStart const auto
	{ keyword for linked ref to start marker - used for caravan packages }

	Keyword Property WorkshopLinkCaravanEnd const auto
	{ keyword for linked ref to end marker - used for caravan packages }

	ActorBase Property CaravanBrahmin Auto const mandatory
	{ the pack brahmin that gets autogenerated by caravan actors }
EndGroup
; index to track the highest used index in the CaravanActor array
int caravanActorMaxIndex = 0



Group Keywords
	Keyword Property WorkshopWorkObject Auto Const mandatory
	{ keyword on built object that indicates it is a "work" object for an actor }

	Keyword Property WorkshopAllowCaravan Auto Const mandatory
	{ put this keyword on actors that can be assigned to caravan duty }

	Keyword Property WorkshopAllowCommand Auto Const mandatory
	{ put this keyword on actors that can be commanded to resource objects }

	Keyword Property WorkshopAllowMove Auto Const mandatory
	{ put this keyword on actors that can be moved to different settlements }

	keyword Property WorkshopLinkContainer const auto mandatory
	{ keyword for the linked container that holds workshop resources }

	keyword Property WorkshopLinkSpawn const auto mandatory
	{ keyword for the linked spawn marker for creating new NPCs }

	keyword Property WorkshopLinkCenter const auto mandatory
	{ keyword for the linked center marker }

	keyword Property WorkshopLinkSandbox const auto mandatory
	{ keyword for the linked sandbox primitive }

	keyword Property WorkshopLinkWork const auto mandatory
	{ keyword for actors to editor-set work objects }

	; event keywords:
;	Keyword Property WorkshopEventChangeLocation const auto
;	{ keyword for workshop change location radiant quests }

	Keyword Property WorkshopEventAttack const auto mandatory
	{ keyword for workshop attack radiant quests }

	Keyword Property WorkshopEventRadioBeacon const auto mandatory
	{ keyword for workshop radio beacon quest }

	Keyword Property WorkshopEventInitializeLocation const auto mandatory
	{ keyword for workshop initialization story manager events }

	Keyword Property LocTypeWorkshopSettlement const auto mandatory
	{ keyword used to test for workshop locations }

	Keyword Property LocTypeWorkshopRobotsOnly const auto mandatory
	{ keyword used to initialize "robots only" locations (e.g. Graygarden) }

	Keyword Property WorkshopCaravanKeyword const auto mandatory
	{ keyword used for links between workshop locations }

	Keyword Property WorkshopLinkFollow const auto mandatory
	{ keyword used to create dynamic linked refs for follow packages (e.g. caravan brahmin)}

	Keyword Property WorkshopLinkHome const auto mandatory
	{ keyword used to create dynamic linked refs for persistent workshop NPCs - center of their default sandbox package }

	Keyword Property WorkshopItemKeyword Auto Const mandatory
	{ keyword that links all workshop-created items to their workshop }

	FormList Property VendorContainerKeywords Auto Const mandatory
	{ form list of keywords that link vendor containers to the vendor - indexed by vendor level }

	Keyword Property WorkshopAssignCaravan const auto mandatory
	{ keyword sent by interface when assigning an NPC to a caravan destination }

	Keyword Property WorkshopAssignHome const auto mandatory
	{ keyword sent by interface when assigning an NPC to a new home }

	Keyword Property WorkshopAssignHomePermanentActor const auto mandatory
	{ keyword sent by interface when assigning a "permanent" NPC to a new home }

	Keyword Property WorkshopType02 const auto mandatory
	{ default keyword to flag secondary settlement type }

	Keyword Property WorkshopType02Vassal const auto mandatory
	{ default keyword to flag secondary settlement vassals }

	FormList Property WorkshopSettlementMenuExcludeList Auto const mandatory
	{ form list of keywords for secondary settlement types (to exclude them from settlement menu) }

EndGroup

group Globals
	GlobalVariable Property MinutemenRecruitmentAvailable const auto mandatory
	{ number of settlements available for Minutemen recruiting. Updated by UpdateMinutemenRecruitmentAvailable() }

	GlobalVariable Property MinutemenOwnedSettlements const auto mandatory
	{ number of populated settlements owned by Minutemen. Updated by UpdateMinutemenRecruitmentAvailable() }

	GlobalVariable property WorkshopMinRansom auto const mandatory
	GlobalVariable property WorkshopMaxRansom auto const mandatory

	GlobalVariable Property GameHour Auto Const

	GlobalVariable Property PlayerInstitute_Destroyed auto const mandatory
	GlobalVariable Property PlayerInstitute_KickedOut auto const mandatory
	GlobalVariable Property PlayerBeenToInstitute auto const mandatory
	{ used for attack quests - to know if synths can teleport }
EndGroup

group ActorValues
	ActorValue Property Charisma const auto mandatory
	ActorValue Property WorkshopIDActorValue const auto mandatory
	ActorValue Property WorkshopCaravanDestination const auto mandatory
	ActorValue Property WorkshopActorWounded const auto mandatory
	ActorValue Property PowerGenerated const auto mandatory
	ActorValue Property PowerRequired const auto mandatory

	ActorValue Property WorkshopGuardPreference const auto mandatory
	{ base actors with this value will try to guard when first created (instead of farming)}

	ActorValue Property WorkshopActorAssigned const auto mandatory
	{ actors get this value temporarily after being assigned so they will always run their work package }

	ActorValue Property WorkshopFloraHarvestTime const auto mandatory
	{ flora objects get this when harvested, to track when they should "regrow" }

	ActorValue Property WorkshopPlayerOwnership const auto mandatory
	{ actor value used to flag player-owned workshops for use by condition checks }

	ActorValue Property WorkshopPlayerLostControl const auto mandatory
	{ set to 1 on workshops that was friendly to player then became "unfriendly" - set back to 0 when owned status restored
		set to 2 on workshops after first Reset - to indicate that anything that needs to be cleared is taken care of
	 }

	ActorValue Property WorkshopResourceObject Auto Const mandatory
	{ actor value on built object that indicates it is a resource object of some kind
		all objects that the scripted workshop system cares about should have this actor value }

	ActorValue Property WorkshopAttackSAEFaction auto const mandatory
	{ actor value to record the attack faction for WorkshopAttackDialogueFaction
		uses SAE_XXX globals for faction values }

	ActorValue Property WorkshopFastTravel const auto mandatory
	{ actor resource value to prevent building multiple fast travel targets in a single workshop location}

	ActorValue Property WorkshopMaxTriangles auto const
	{ used to set build budget on workshop ref}
	ActorValue Property WorkshopCurrentTriangles auto const
	{ used to set build budget on workshop ref}
	ActorValue Property WorkshopMaxDraws auto const
	{ used to set build budget on workshop ref}
	ActorValue Property WorkshopCurrentDraws auto const
	{ used to set build budget on workshop ref}

	ActorValue Property WorkshopProhibitRename const auto mandatory
	{ actors with this > 0 will not be put in the CaravanActorRenameAliases collection }

EndGroup

Group AchievementData
	globalVariable property AlliedSettlementAchievementCount auto const
	int property AlliedSettlementsForAchievement = 3 auto const
	int property AlliedSettlementAchievementID = 23 auto const

	int property HappinessAchievementValue = 100 auto const
	int property HappinessAchievementID = 24 auto const

endGroup

ReferenceAlias Property PlayerCommentTarget Auto const
{ used for player comment }

Scene Property WorkshopPlayerCommentScene auto const
{ player comment scene }

ReferenceAlias Property WorkshopRecruit Const Auto mandatory

bool Property PlayerOwnsAWorkshop auto Conditional
{ set to true when the player owns ANY workshop - used for dialogue conditions }

int Property CurrentNewSettlerCount auto Conditional
{ how many new settlers at current workshop? }

int Property MaxNewSettlerCount = 4 auto const hidden
{ max number to put into "new settler" collection - don't want massive crowd following player around }

ActorBase Property WorkshopNPC Auto const mandatory
{ the actor that gets created when a settlement makes a successful recruitment roll }

ActorBase Property WorkshopNPCGuard Auto const mandatory
{ sometimes a "guard" NPC gets created instead }

Topic Property WorkshopParentAssignConfirm auto const mandatory hidden
{ OBSOLETE }
Keyword Property WorkshopParentAssignConfirmTopicType auto const mandatory
{ replaces WorkshopParentAssignConfirm topic - shared topic type allows DLC to add new lines more easily }


int property recruitmentGuardChance = 20 auto const hidden  		; % chance of getting a "guard" NPC
int property recruitmentBrahminChance = 20 auto const Hidden 		; % chance of getting a brahmin with a "farmer" NPC
int property recruitmentSynthChance = 10 auto const Hidden  		; % chance of getting a synth
int property recruitmentMinPopulationForSynth = 4 auto const Hidden  ; min population for rolling for a synth

ActorBase Property WorkshopBrahmin Auto mandatory
{ the workshop brahmin that can be randomly created during recruitment }

Quest Property WorkshopInitializeLocation const auto mandatory
{ quest that initializes workshop locations }

String Property userlogName = "Workshop" Auto Const Hidden

MiscObject Property SynthDeathItem auto const mandatory
{ death item for synths }

Group LeveledItems
	; used when producing resources from workshop objects
	LeveledItem Property WorkshopProduceFood Auto Const mandatory
	LeveledItem Property WorkshopProduceWater Auto Const mandatory
	LeveledItem Property WorkshopProduceScavenge Auto Const mandatory
	LeveledItem Property WorkshopProduceVendorIncome Auto Const mandatory
	LeveledItem Property WorkshopProduceFertilizer Auto Const mandatory

	WorkshopFoodType[] Property WorkshopFoodTypes auto const
	{ array of food types used to produce appropriate food type }
EndGroup

Group Resources
	; used when consuming resources from workshop objects
	Keyword Property WorkshopConsumeFood Auto Const mandatory
	Keyword Property WorkshopConsumeWater Auto Const mandatory
	FormList Property WorkshopConsumeScavenge Auto Const  mandatory			; list of components
EndGroup

Group LocRefTypes
	; used when checking for bosses in order to clear a location
	FormList Property BossLocRefTypeList auto const mandatory

	LocationRefType Property MapMarkerRefType Auto Const mandatory

	LocationRefType Property Boss Auto Const mandatory

	LocationRefType Property WorkshopCaravanRefType Auto Const mandatory
	LocationRefType Property WorkshopSynthRefType Auto Const mandatory
	{ used to flag created synth settlers }
endGroup

Group factions
	Faction Property REIgnoreForCleanup Const Auto mandatory
	{ add actors to this faction to have them ignored during RE cleanup check
	  i.e. quest can clean up even if they are loaded/alive
	}
	Faction Property REDialogueRescued Const Auto mandatory
	{ remove from this faction after RE NPCs are added to workshop }

	Faction Property RaiderFaction const auto mandatory
	{ used for random attacks }

	Faction Property RobotFaction const auto mandatory
	{ used to check for robot actors in daily update etc. }

	Faction Property BrahminFaction const auto mandatory
	{ used to check for brahmin actors in daily update etc. }

	Faction Property PlayerFaction const auto mandatory
	{ assign default ownership to non-assigned workshop objects }

	Faction Property WorkshopAttackDialogueFaction const auto mandatory
	{ used to condition "grateful" dialogue after player helps fight off attackers }

	Faction Property MinRadiantDialogueDisappointed const auto mandatory
	Faction Property MinRadiantDialogueWorried const auto mandatory
	Faction Property MinRadiantDialogueHopeful const auto mandatory
	Faction Property MinRadiantDialogueThankful const auto mandatory
	Faction Property MinRadiantDialogueFailure const auto mandatory

EndGroup

ObjectReference Property WorkshopHoldingCellMarker Const Auto mandatory
{ marker for holding cell - use to place vendor chests }

FollowersScript Property Followers const auto mandatory
{ pointer to Followers quest script for utility functions }

; how large a radius to search for workshop objects/actors? (should be whole loaded area)
int findWorkshopObjectRadius = 5000 const

float startingHappiness = 50.0 const					; happiness of a new workshop starts here
float startingHappinessMin = 20.0 const 				; when resetting happiness, don't start lower than this
float property startingHappinessTarget = 50.0 auto const 				; init happiness target to this


; how many food points can each NPC work on?
int maxFoodProductionPerFarmer = 10 const

; timer IDs
int dailyUpdateTimerID = 0 const

; update timer
float updateIntervalGameHours = 24.0 const	 ; daily
float dailyUpdateSpreadHours = 12.0 const  	; how many hours to spread out (total) all the daily updates for workshops

float Property dailyUpdateIncrement = 0.0 auto
{ updated during daily update process - how much time in between each workshop's own daily update }

; happiness modifier when an actor dies
float actorDeathHappinessModifier = -20.0 const

; custom event sent each time an Initialize quest completes, to signal starting the next one
CustomEvent WorkshopInitializeLocation

; custom event sent when DailyUpdate is processed
CustomEvent WorkshopDailyUpdate

; custom event sent when a non-workshop actor is added to a workshop
CustomEvent WorkshopAddActor 

; custom event sent when an actor is assigned to a work object
CustomEvent WorkshopActorAssignedToWork

; custom event sent when an actor is unassigned from a work object
CustomEvent WorkshopActorUnassigned

; custom event sent when a workshop object is built
CustomEvent WorkshopObjectBuilt 

; custom event sent when a workshop object is destroyed (removed from the world)
CustomEvent WorkshopObjectDestroyed

; custom event sent when a workshop object is moved
CustomEvent WorkshopObjectMoved 

; custom event sent when a workshop object is damaged or repaired
CustomEvent WorkshopObjectDestructionStageChanged

; custom event sent when a workshop object is damaged or repaired
CustomEvent WorkshopObjectPowerStageChanged

; custom event sent when a workshop becomes player-owned
CustomEvent WorkshopPlayerOwnershipChanged

; custom event sent when player enters workshop menu
CustomEvent WorkshopEnterMenu 

; custom event sent when a workshop object is repaired
CustomEvent WorkshopObjectRepaired 

; 1.6 custom event sent when an NPC is assigned to a supply line
CustomEvent WorkshopActorCaravanAssign

; 1.6 custom event sent when an NPC is unassigned from a supply line
CustomEvent WorkshopActorCaravanUnassign


; set to true after initializition is complete - so other scripts don't try to use data on this script before then
bool property Initialized = false auto

; set to true while thread-sensitive functions are in progress
bool EditLock = false

; WorkshopScript sets this to true during daily update, other workshops won't do daily updates until this is clear to prevent script overload
bool property DailyUpdateInProgress = false auto hidden

Event Location.OnLocationCleared(Location akSender)
	WorkshopScript workshopRef = GetWorkshopFromLocation(akSender)
	if workshopRef && workshopRef.OwnedByPlayer
		UpdateMinutemenRecruitmentAvailable()
	endif
endEvent


; NOTE: changed from OnInit because of timing issues - startup stage will not be set until aliases are filled
; Updated to handle cases where the workshop grabbed from the WorkshopsCollection is invalid
int lastValidWorkshopIndex = 0 ; keep track of index of the last valid workshop added to the Workshops array
Event OnStageSet(int auiStageID, int auiItemID)
	wsTrace(self + " stage=" + auiStageID)
	if auiStageID == 10
		; open workshop log
		debug.OpenUserLog(userlogName)

		; initialize workshop arrays
		WorkshopLocations = new Location[WorkshopsCollection.GetCount()]
		Workshops = new WorkshopScript[WorkshopsCollection.GetCount()]
		WorkshopRatingValues = new ActorValue[WorkshopRatings.Length]

		; Initialize workshop and workshop location arrays
		int collectionIndex = 0
		int crimeFactionIndex = 0
		wsTrace(" OnInit: initializing Workshops and WorkshopLocations arrays...", bNormalTraceAlso = true)
		while collectionIndex < WorkshopsCollection.GetCount()
			wsTrace(" OnInit: " + collectionIndex + " of " + WorkshopsCollection.GetCount())
			
			; Grab the workshop reference from the WorkshopsCollection
			WorkshopScript workshopRef = WorkshopsCollection.GetAt(collectionIndex) as WorkshopScript

			; We only want to add a workshop reference to the Workshops array if it is valid
			if workshopRef
			wsTrace("	" + workshopRef + ": EnableAutomaticPlayerOwnership=" + workshopRef.EnableAutomaticPlayerOwnership)
			; add workshop to array
				Workshops[lastValidWorkshopIndex] = workshopref
			; initialize workshopID on this workshop
				workshopRef.InitWorkshopID(lastValidWorkshopIndex)
			; initialize location
				WorkshopLocations[lastValidWorkshopIndex] = workshopRef.GetCurrentLocation()
				workshopRef.myLocation = WorkshopLocations[lastValidWorkshopIndex]
			; initialize happiness to 50 for safety
			workshopRef.SetValue(WorkshopRatings[WorkshopRatingHappiness].resourceValue, startingHappiness)
			; register for location cleared events
				RegisterForRemoteEvent(WorkshopLocations[lastValidWorkshopIndex], "OnLocationCleared")

			; set ownership/crime faction if it doesn't have one already
			if workshopRef.SettlementOwnershipFaction == NONE && workshopRef.UseOwnershipFaction && crimeFactionIndex < WorkshopCrimeFactions.GetSize()
				workshopRef.SettlementOwnershipFaction = WorkshopCrimeFactions.GetAt(crimeFactionIndex) as Faction
				crimeFactionIndex += 1
			endif

				wsTrace(" OnInit: location " + lastValidWorkshopIndex + "=" + WorkshopLocations[lastValidWorkshopIndex])
			; register for daily update
				Workshops[lastValidWorkshopIndex].RegisterForCustomEvent(self, "WorkshopDailyUpdate")
				
				; Increment Workshop array index when we add a valid workshop to the array
				lastValidWorkshopIndex += 1
			endif

			; Increment collection index regardless of if we found a valid workshop reference
			collectionIndex += 1
		endWhile
		wsTrace(" OnInit: initializing Workshops and WorkshopLocations... DONE ", bNormalTraceAlso = true)

		; Initialize workshop rating values array using keywords
		int ratingIndex = 0
		int resourceAVCount = 0
		wsTrace(" OnInit: initializing WorkshopRatingValues array...", bNormalTraceAlso = true)
		while ratingIndex < WorkshopRatings.Length
			wsTrace(" OnInit: keyword " + ratingIndex + "=" + WorkshopRatings[ratingIndex].resourceValue)
			; add keyword to array
			WorkshopRatingValues[ratingIndex] = WorkshopRatings[ratingIndex].resourceValue
			; if this has a resource AV, increment count
			if WorkshopRatings[ratingIndex].resourceValue
				resourceAVCount += 1
				wsTrace(" OnInit: found resourceValue= " + WorkshopRatings[ratingIndex].resourceValue + ", resourceAVCount=" + resourceAVCount)
			endif
			ratingIndex += 1
		endWhile
		wsTrace(" OnInit: initializing WorkshopRatingValues... DONE.", bNormalTraceAlso = true)

		; Initialize workshop resource AV array using workshop ratings array
		wstrace(" OnInit: initializing WorkshopResourceAVs array to " + resourceAVCount)
		WorkshopResourceAVs = new WorkshopActorValue[resourceAVCount]

		ratingIndex = 0
		int resourceAVIndex = 0
		wsTrace(" OnInit: initializing workshop resource AV array...", bNormalTraceAlso = true)
		while ratingIndex < WorkshopRatings.Length
			; if this has a resource AV, add to resource AV array and increment rating index
			if WorkshopRatings[ratingIndex].resourceValue
				wsTrace(" OnInit: resource AV " + resourceAVIndex + "=" + WorkshopRatings[ratingIndex].resourceValue, bNormalTraceAlso = true)
				WorkshopResourceAVs[resourceAVIndex] = new WorkshopActorValue
				WorkshopResourceAVs[resourceAVIndex].workshopRatingIndex = ratingIndex
				WorkshopResourceAVs[resourceAVIndex].resourceValue = WorkshopRatings[ratingIndex].resourceValue
				wsTrace(" 		WorkshopResourceAVs[" + resourceAVIndex + "].resourceValue=" + WorkshopResourceAVs[resourceAVIndex].resourceValue)

				resourceAVIndex += 1
			endif
			ratingIndex += 1
		endWhile

		; initialize Minutemen recruitment available
		UpdateMinutemenRecruitmentAvailable()

		; get location change events for player
		RegisterForRemoteEvent(Game.GetPlayer(), "OnLocationChange")
		
		; start daily update timer
		wsTrace(" OnInit: starting workshop daily timer: updateIntervalGameHours=" + updateIntervalGameHours + ", dailyUpdateTimerID=" + dailyUpdateTimerID, bNormalTraceAlso = true)
		StartTimerGameTime(updateIntervalGameHours, dailyUpdateTimerID)

		; start initialize workshop locations process
		RegisterForCustomEvent(self, "WorkshopInitializeLocation")
		SendCustomEvent("WorkshopInitializeLocation")		

		Initialized = true
	endif
endEvent

function InitializeLocation(WorkshopScript workshopRef, RefCollectionAlias SettlementNPCs, ReferenceAlias theLeader, ReferenceAlias theMapMarker)
	wsTrace(" Initializing location START: " + workshopRef.myLocation)
	workshopRef.myMapMarker = theMapMarker.GetRef()

	; force recalc (unloaded workshop)
	workshopRef.RecalculateWorkshopResources(true)
	
	int initPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
	int initPopulationVal = workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
	wsTrace(" 		initPopulation (before)=" + initPopulation)
	wsTrace(" 		initPopulationVal (before)=" + initPopulationVal)
	if SettlementNPCs
		AddCollectionToWorkshopPUBLIC(SettlementNPCs, workshopRef, true)
	endif
	if theLeader && theLeader.GetActorRef()
		AddActorToWorkshopPUBLIC(theLeader.GetActorRef() as WorkshopNPCScript, workshopRef, true)
	endif 

	initPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
	initPopulationVal = workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
	wsTrace(" 		initPopulation (after)=" + initPopulation)
	wsTrace(" 		initPopulationVal (before)=" + initPopulationVal)
	int robotPopulation = 0
	if workshopRef.myLocation.HasKeyword(LocTypeWorkshopRobotsOnly)
		; this means everyone here (at game start) is a robot
		robotPopulation = initPopulation
	endif
	ModifyResourceData(WorkshopRatings[WorkshopRatingPopulationRobots].resourceValue, workshopRef, robotPopulation)

	; initialize ratings if it has population
	if initPopulation > 0
		wsTrace(" 		init population = " + initPopulation)
		; happiness
		SetResourceData(WorkshopRatings[WorkshopRatingHappiness].resourceValue, workshopRef, startingHappiness)
		; set food, water, beds equal to population (this will be corrected to reality the first time the player visits the location)
		SetResourceData(WorkshopRatings[WorkshopRatingFood].resourceValue, workshopRef, initPopulation)
		SetResourceData(WorkshopRatings[WorkshopRatingWater].resourceValue, workshopRef, initPopulation)
		SetResourceData(WorkshopRatings[WorkshopRatingBeds].resourceValue, workshopRef, initPopulation - robotPopulation)
		; set "last attacked" to a very large number (so they don't act like they were just attacked)
		SetResourceData(WorkshopRatings[WorkshopRatingLastAttackDaysSince].resourceValue, workshopRef, 99)
	endif

	wsTrace(" Initializing location DONE: " + workshopRef.myLocation + ": population=" + initPopulation, bNormalTraceAlso = true)

	; send "done" event
	Var[] kargs = new Var[1]
	kargs[0] = workshopRef
	wsTrace(" 	sending WorkshopInitializeLocation event")
	SendCustomEvent("WorkshopInitializeLocation", kargs)		
endFunction

int initializationIndex = 0
Event WorkshopParentScript.WorkshopInitializeLocation(WorkshopParentScript akSender, Var[] akArgs)
	wsTrace("WorkshopInitializeLocation event received " + initializationIndex, bNormalTraceAlso = true)
	WorkshopScript nextWorkshopRef = NONE
	; The akArgs.Length is expected to be 0 when this event is sent from OnStageSet for the first time
	if (akArgs.Length > 0)
		; Grab the workshop reference that was just initialized
		WorkshopScript workshopRef = akArgs[0] as WorkshopScript

		; If the workshop reference is valid, we find the next workshop since 
		; the current one successfully initialized
		if workshopRef
			; Try just going up through the array
			initializationIndex += 1
			int newWorkshopIndex = initializationIndex

			; If our new workshop index passes the last valid workshop added to the Workshops array,
			; or it is greater than the size of the Workshops array, we've finished initializing
			; all valid workshop locations
			if newWorkshopIndex >= lastValidWorkshopIndex || newWorkshopIndex >= Workshops.Length
				wsTrace(" WorkshopInitializeLocation: Finished all locations", bNormalTraceAlso = true)

				setStage(20) ; way to easily track when this is done
				
				; initial daily update
				DailyWorkshopUpdate(true)
				; reset daily update timer - failsafe
				wsTrace(" WorkshopInitializeLocation: resetting workshop daily timer: updateIntervalGameHours=" + updateIntervalGameHours + ", dailyUpdateTimerID=" + dailyUpdateTimerID, bNormalTraceAlso = true)
				StartTimerGameTime(updateIntervalGameHours, dailyUpdateTimerID)
			else
				wsTrace(" 	workshop done: " + workshopRef.myLocation + "(" + workshopRef.GetWorkshopID() + ")")
				; send story event for next workshop location
				nextWorkshopRef = workshops[newWorkshopIndex]
			endif

		else
			wsTrace(" WARNING: WorkshopInitializeLocation event received with invalid args", bNormalTraceAlso = true)
		endif
	else
		; In this case, there were no parameters sent with the event, which means this event is most likely coming from 
		; the OnStageSet() event
		wsTrace(" 	No parameters- start with workshop 0=" + Workshops[0].myLocation, bNormalTraceAlso = true)
		
		; If no parameters were sent, start with the first workshop in the Workshops array.
		; This should no longer have a chance to be an invalid reference as only valid 
		; elements were added to the Workshops array
		nextWorkshopRef = Workshops[0]
	endif

	; Send the event to initialize the workshop's location if the next workshop is valid
	if nextWorkshopRef
		wsTrace(" 	send story event for next workshop: " + nextWorkshopRef.myLocation + "(" + nextWorkshopRef.GetWorkshopID() + ")")
		; wait a bit for quest to finish shutting down
		int maxWait = 5
		int i = 0
		while i < maxWait && WorkshopInitializeLocation.IsStopped() == false
			wsTrace("	waiting for WorkshopInitializeLocation to shut down..." + i)
			utility.wait(1.0)
			i += 1
		endWhile
		bool bSuccess = WorkshopEventInitializeLocation.SendStoryEventAndWait(nextWorkshopRef.myLocation)
		if !bSuccess
			; quest failed to start for this location - skip it and move on
			wstrace("	WARNING: WorkshopInitializeLocation quest failed to start for " + nextWorkshopRef.myLocation)
			; send "done" event
			Var[] kargs = new Var[1]
			kargs[0] = nextWorkshopRef
			wsTrace(" 	sending WorkshopInitializeLocation event")
			SendCustomEvent("WorkshopInitializeLocation", kargs)		
		endif
	endif
EndEvent

; called when new locations are added (e.g. by DLC init quests)
; newWorkshops - array of new workshops to check for
; return value:
;		TRUE = initializeEventHandler needs to handle initialize events for new locations
;		FALSE = no locations need to be initialized (all are already in Workshops array)
bool function ReinitializeLocationsPUBLIC(WorkshopScript[] newWorkshops, Form initializeEventHandler)
	; wait for main initialization process to finish
	while GetStageDone(20) == false
		debug.trace( " ... waiting for primary WorkshopInitializeLocation process to finish...")
		utility.wait(2.0)
	endWhile

	; lock editing
	GetEditLock()

	; make sure to unregister WorkshopParent from this event - DLC will register and handle the event for themselves
	UnregisterForCustomEvent(self, "WorkshopInitializeLocation")

	; save the index of the last workshop added - this is the starting point for new init loop
	int startingNewWorkshopIndex = lastValidWorkshopIndex

	; run through newWorkshops to see if they're already in the workshop list
	int i = 0
	while i < newWorkshops.Length
		WorkshopScript workshopRef = newWorkshops[i]
		; We only add workshop references if they are valid
		if workshopRef
		; is this already in Workshops array?
		int workshopIndex = Workshops.Find(workshopRef)
		if workshopIndex == -1
			; not in list - add me to arrays and initialize
			; NOTE: this basically replicates code in OnStageSet, but safer to duplicate it here than change that to a function call
			; START:
				; add workshop to array
				Workshops.Add(workshopRef)
				int newIndex = Workshops.Length-1
				; initialize workshopID on this workshop to the index
				workshopRef.InitWorkshopID(newIndex)
				; initialize location
				WorkshopLocations.Add(workshopRef.GetCurrentLocation())
				workshopRef.myLocation = WorkshopLocations[newIndex]
				; initialize happiness to 50 for safety
				workshopRef.SetValue(WorkshopRatings[WorkshopRatingHappiness].resourceValue, startingHappiness)
				; register for location cleared events
				RegisterForRemoteEvent(WorkshopLocations[newIndex], "OnLocationCleared")

				; NOTE: ownership/crime faction must be set on new workshops manually

				debug.trace(" OnInit: location " + newIndex + "=" + WorkshopLocations[newIndex])
				; register for daily update
				Workshops[newIndex].RegisterForCustomEvent(self, "WorkshopDailyUpdate")

					; If we added a new workshop, increment our last valid workshop index so new locations
					; can successfully initialize
					lastValidWorkshopIndex += 1
			; END
		endif
		endif
		i += 1
	endWhile

	; if we added any new locations, need to initialize them
	bool bLocationsToInit = (startingNewWorkshopIndex < lastValidWorkshopIndex)
	if bLocationsToInit
		; whatever was passed in will handle this event
		initializeEventHandler.RegisterForCustomEvent(self, "WorkshopInitializeLocation")

		; start the process by sending the event with the LAST initialized workshop (last valid workshop, or top of original Workshops array)
		WorkshopScript lastInitializedWorkshop = Workshops[startingNewWorkshopIndex - 1]
		Var[] kargs = new Var[1]
		kargs[0] = lastInitializedWorkshop
		debug.trace(" 	sending WorkshopInitializeLocation event for " + lastInitializedWorkshop)
		SendCustomEvent("WorkshopInitializeLocation", kargs)		
	endif

	debug.trace(" ReinitializeLocationsPUBLIC DONE")

	; unlock editing
	EditLock = false

	return bLocationsToInit
endFunction

; returns the total in all player-owned settlements for the specified rating
float function GetWorkshopRatingTotal(int ratingIndex)
	wsTrace(self + " GetWorkshopRatingTotal:")
	ActorValue resourceValue = GetRatingAV(ratingIndex)

	; go through player-owned workshops
	int index = 0
	float total = 0.0
	while index < Workshops.Length
		if Workshops[index].GetValue(WorkshopPlayerOwnership) > 0
			total += Workshops[index].GetValue(resourceValue)
		endif
		index += 1
	endWhile
	wsTrace(self + "GetWorkshopRatingTotal: " + resourceValue + "=" + total)
	return total
endFunction

; returns the total of player-owned settlements for the specified rating
int function GetWorkshopRatingLocations(int ratingIndex, float ratingMustBeGreaterThan = 0.0)
	actorValue resourceValue = GetRatingAV(ratingIndex)
	wsTrace(self + " GetWorkshopRatingLocations for "  + resourceValue + ":")

	; go through player-owned locations
	int index = 0
	int locationCount = 0
	while index < Workshops.Length
		;debug.trace(self + " index: " + index + ": " + workshops[index])
		if Workshops[index].GetValue(WorkshopPlayerOwnership) > 0 && Workshops[index].GetValue(resourceValue) > ratingMustBeGreaterThan
			wsTrace(self + " 	found valid location " + Workshops[index].myLocation)
			locationCount += 1
		endif
		index += 1
	endWhile
	wsTrace(self + "GetWorkshopRatingLocations: location count = " + locationCount)
	return locationCount
endFunction


; utility function - updates flag used to indicate if there are any settlements available for recruitment
; Updated so the function does not attempt to use invalid workshop references when updating counts
function UpdateMinutemenRecruitmentAvailable()
	wsTrace(self + " UpdateMinutemenRecruitmentAvailable:", bNormalTraceAlso = true)
	; go through locations - looking for:
	; * not player-owned
	; * population > 0
	int index = 0
	int neutralCount = 0	 ; count the number of "neutral" settlements (not owned by player)
	int totalCount = 0		; count total number of populated settlements
	while index < Workshops.Length
		WorkshopScript workshopRef = Workshops[index]
		; We only want to increment count values when there is a valid workshop
		if workshopRef
		wstrace(self + " Workshops[" + index + "]=" + workshopRef)
		If workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) > 0 && workshopRef.HasKeyword(WorkshopType02) == false && workshopRef.HasKeyword(WorkshopType02Vassal) == false
			wsTrace(self + " 	" + workshopRef + ": valid workshop settlement, population rating = " + workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue), bNormalTraceAlso = true)
			totalCount += 1
			if workshopRef.GetValue(WorkshopPlayerOwnership) == 0
				wsTrace(self + " 		- not owned by player - add to neutral count", bNormalTraceAlso = true)
				neutralCount += 1
			endif
		endif
		endif
		index += 1
	endWhile
	wsTrace(self + "UpdateMinutemenRecruitmentAvailable: " + neutralCount + " neutral, " + totalCount + " total", bNormalTraceAlso = true)
	; update globals
	MinutemenRecruitmentAvailable.SetValue(neutralCount)
	MinutemenOwnedSettlements.SetValue(totalCount - neutralCount)
endFunction

Event OnTimerGameTime(int aiTimerID)
	wsTrace(" OnTimerGameTime: timerID=" + aiTimerID)
	if aiTimerID == dailyUpdateTimerID
		DailyWorkshopUpdate()
		; start timer again
		wsTrace(" OnTimerGameTime: restarting workshop daily timer: updateIntervalGameHours=" + updateIntervalGameHours + ", dailyUpdateTimerID=" + dailyUpdateTimerID, bNormalTraceAlso = true)
		StartTimerGameTime(updateIntervalGameHours, dailyUpdateTimerID)
	endif
EndEvent


Event Actor.OnLocationChange(Actor akSender, Location akOldLoc, Location akNewLoc)
	;;debug.trace(self + " OnLocationChange: akNewLoc=" + akNewLoc)
	if akNewLoc && akNewLoc.HasKeyword(LocTypeWorkshopSettlement)
		wsTrace(" OnLocationChange: entered workshop settlement location " + akNewLoc)
		; when player enters a workshop location, recalc the workbench ratings
		; get the workbench
		WorkshopScript workshopRef = GetWorkshopFromLocation(akNewLoc)
		if !workshopRef
			wsTrace(" ERROR - OnLocationChange: failed to find workshop matching " + akNewLoc + " which has the LocTypeWorkshopSettlement keyword", 2)
			return
		else
			ResetWorkshop(workshopRef)
			; send change location script event
			; OBSOLETE - moved to always use MinRadiantStart
;			WorkshopEventChangeLocation.SendStoryEvent(akNewLoc, workshopRef)
		endif
	endif


	if akOldLoc && akOldLoc.HasKeyword(LocTypeWorkshopSettlement)
		wsTrace(" OnLocationChange: exited workshop location " + akOldLoc)
		; when player leaves a workshop location, recalc the workbench ratings
		; get the workbench
		WorkshopScript workshopRef = GetWorkshopFromLocation(akOldLoc)
		if !workshopRef
			wsTrace(" ERROR - OnLocationChange: failed to find workshop matching " + akOldLoc + " which has the LocTypeWorkshopSettlement keyword", 2)
			return
		else
			; reset days since last visit for this workshop
			workshopRef.DaysSinceLastVisit = 0
		endif

	endif
EndEvent

WorkshopScript function GetWorkshopFromLocation(Location workshopLocation)
	int index = WorkshopLocations.Find(workshopLocation)
	if index < 0
		wsTrace(" ERROR - GetWorkshopFromLocation: workshop location " + workshopLocation + " not found in WorkshopLocations array", 2)
		return NONE
	else
		return Workshops[index] as WorkshopScript
	endif
endFunction

int tempBuildCounter = 0

; main function called by workshop when new object is built
bool function BuildObjectPUBLIC(ObjectReference newObject, WorkshopScript workshopRef)
	; lock editing
	GetEditLock()

	tempBuildCounter += 1
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" BuildObject: " + tempBuildCounter + " " + newObject + " from " + workshopRef)

	WorkshopObjectScript newWorkshopObject = newObject as WorkshopObjectScript

	; if this is a scripted object, check for input/output data
	if newWorkshopObject
		; tag with workshopID
		newWorkshopObject.workshopID = workshopRef.GetWorkshopID()
		AssignObjectToWorkshop(newWorkshopObject, workshopRef, true) ; reset mode = true --> don't try to assign work each time; this is handled by a timer on WorkshopScript

		; object handles any creation needs
		newWorkshopObject.HandleCreation(true)

		; send custom event for this object
		Var[] kargs = new Var[2]
		kargs[0] = newWorkshopObject
		kargs[1] = workshopRef
		wsTrace(" 	sending WorkshopObjectBuilt event")
		SendCustomEvent("WorkshopObjectBuilt", kargs)		

	endif

	wsTrace(" BuildObject DONE: " + tempBuildCounter + " (editLockCount=" + editLockCount + ") " + newObject)
	wsTrace("------------------------------------------------------------------------------ ")

	; unlock building
	EditLock = false

	; return true if this is a scripted object
	return ( newWorkshopObject != NONE )
endFunction

; called by WorkshopScript on timer periodically to keep new work objects assigned
function TryToAssignResourceObjectsPUBLIC(WorkshopScript workshopRef)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" TryToAssignResourceObjectsPUBLIC: " + workshopRef)
	; lock editing
	GetEditLock()

	TryToAssignResourceType(workshopRef, WorkshopRatings[WorkshopRatingFood].resourceValue)
	TryToAssignResourceType(workshopRef, WorkshopRatings[WorkshopRatingSafety].resourceValue)
	TryToAssignBeds(workshopRef)

	wsTrace(" TryToAssignResourceObjectsPUBLIC DONE: " + workshopRef)
	wsTrace("------------------------------------------------------------------------------ ")

	; unlock building
	EditLock = false
endFunction


function WoundActor(WorkShopNPCScript woundedActor, bool bWoundMe = true)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" WoundActor: " + woundedActor + ", bWoundMe=" + bWoundMe)

	; get actor's workshop
	WorkshopScript workshopRef = GetWorkshop(woundedActor.GetWorkshopID())
	; wound/heal actor
	woundedActor.SetWounded(bWoundMe)

	; increase or decrease damage?
	int damageValue = 1
	if !bWoundMe
		damageValue = -1
	endif

	; update damage rating
	; RESOURCE CHANGE:
	; reduce extra pop damage if > 0 ; otherwise, damage is normally tracked within WorkshopRatingPopulation (difference between base value and current value)
	if bWoundMe == false && workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue) > 0
		ModifyResourceData(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue, workshopRef, damageValue)
	endif

	; update my work objects for "can produce"
	UpdateActorsWorkObjects(woundedActor, workshopRef, true)

	wsTrace(" WoundActor: DONE")
	wsTrace("------------------------------------------------------------------------------ ")
endFunction

function HandleActorDeath(WorkShopNPCScript deadActor, Actor akKiller)
	; get actor's workshop
	WorkshopScript workshopRef = GetWorkshop(deadActor.GetWorkshopID())

	UnassignActor(deadActor, true)
	; consequences of death:
	; for people, count this as negative to happiness (blame player since they're all protected)
	if deadActor.bCountsForPopulation
		ModifyHappinessModifier(workshopRef, actorDeathHappinessModifier)
	endif
endFunction

function UpdateActorsWorkObjects(WorkShopNPCScript theActor, WorkshopScript workshopRef = NONE, bool bRecalculateResources = false)
	if workshopRef == NONE
		workshopRef = GetWorkshop(theActor.GetWorkshopID())
	endif

	; find my assigned object, if any
	; loop through work objects list, looking for things assigned to me - then update stats for each
	int i = 0
	ObjectReference[] ResourceObjects = workshopRef.GetWorkshopOwnedObjects(theActor)
	while i < ResourceObjects.Length
		WorkshopObjectScript theObject = ResourceObjects[i] as WorkshopObjectScript
		wsTrace("		i=" + i + ": found an owned object " + theObject + "- updating ratings (objectref=" + ResourceObjects[i] + ")")
		; update rating for this object - don't recalc for each object to save time
		if theObject
			UpdateWorkshopRatingsForResourceObject(theObject, workshopRef, bRecalculateResources = bRecalculateResources)
		endif
		i += 1
	endWhile
endFunction

; main function called by workshop when object is deleted
function RemoveObjectPUBLIC(ObjectReference removedObject, WorkshopScript workshopRef)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" RemoveObject: " + removedObject + " from " + workshopRef)

	; lock editing
	GetEditLock()
	WorkshopObjectScript workObject = removedObject as WorkshopObjectScript

	; if this is a scripted object, check for input/output data
	if workObject
		RemoveObjectFromWorkshop(workObject, workshopRef)

		; send custom event for this object
		Var[] kargs = new Var[2]
		kargs[0] = workObject
		kargs[1] = workshopRef
		SendCustomEvent("WorkshopObjectDestroyed", kargs)		
	endif

	; unlock building
	EditLock = false
endFunction

; called when a workshop object is deleted
function RemoveObjectFromWorkshop(WorkshopObjectScript workObject, WorkshopScript workshopRef)
	UnassignObject(workObject, true)

	; clear workshopID
	workObject.workshopID = -1

	; tell object it's being deleted
	workObject.HandleDeletion()

endFunction

; Create a new actor and assign it to the specified workshop
function CreateActorPUBLIC(WorkshopScript workshopRef, ObjectReference spawnMarker = NONE, bool bNewSettlerAlias = false)
	; lock editing
	GetEditLock()
	CreateActor(workshopRef, false, spawnMarker, bNewSettlerAlias)
	; unlock editing
	EditLock = false
endFunction

WorkshopNPCScript function CreateActor(WorkshopScript workshopRef, bool bBrahmin = false, ObjectReference spawnMarker = NONE, bool bNewSettlerAlias = false)
	wsTrace("  CreateActor for " + workshopRef)
	if spawnMarker == NONE
		spawnMarker = workshopRef.GetLinkedRef(WorkshopLinkSpawn)
	endif

	; create actor, set workshop ID on actor value
	actorBase newActorBase

	if bBrahmin
		newActorBase = WorkshopBrahmin
		;newActor = spawnMarker.PlaceAtMe(WorkshopBrahmin, abDeleteWhenAble = false) as Actor
	else
		if workshopRef.CustomWorkshopNPC
			newActorBase = workshopRef.CustomWorkshopNPC
		else
			; roll for farmer vs. guard
			if Utility.RandomInt(1, 100) <= recruitmentGuardChance
				newActorBase = WorkshopNPCGuard
				;newActor = spawnMarker.PlaceAtMe(WorkshopNPCGuard, abDeleteWhenAble = false) as Actor
			else
				newActorBase = WorkshopNPC
				;newActor = spawnMarker.PlaceAtMe(WorkshopNPC, abDeleteWhenAble = false) as Actor
			endif
		endif
	endif
	Actor newActor = spawnMarker.PlaceAtMe(newActorBase, abDeleteWhenAble = false) as Actor

	; flag as "new"
	WorkshopNPCScript newWorkshopActor = newActor as WorkShopNPCScript
	if !bBrahmin
		newWorkshopActor.bNewSettler = true
	endif

	; check for synth
	if workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulationSynths].resourceValue) == 0 && workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) >= recruitmentMinPopulationForSynth
		if !bBrahmin && Utility.RandomInt(1, 100) <= recruitmentSynthChance
			newWorkshopActor.SetSynth(true)
		endif
	endif
	AddActorToWorkshop(newWorkshopActor, workshopRef)

	; try to automatically assign to do something:
	if !bBrahmin
		TryToAutoAssignActor(workshopRef, newWorkshopActor)
	endif

	if bNewSettlerAlias
		WorkshopNewSettler.ForceRefTo(newWorkshopActor)
	endif
	return newWorkshopActor
endFunction

function TryToAutoAssignActor(WorkshopScript workshopRef, WorkshopNPCScript actorToAssign)
	ActorValue resourceValue
	if actorToAssign.GetValue(WorkshopGuardPreference) > 0
		resourceValue = WorkshopRatings[WorkshopRatingSafety].resourceValue
	else
		resourceValue = WorkshopRatings[WorkshopRatingFood].resourceValue
	endif
	actorToAssign.SetMultiResource(resourceValue)
	TryToAssignResourceType(workshopRef, resourceValue)
	if actorToAssign.multiResourceProduction == 0.0
		; if didn't pick up anything, clear it - nothing for him to do
		actorToAssign.SetMultiResource(NONE)
	endif
endFunction

; assign specified actor to specified workshop object - PUBLIC version (called by other scripts)
function AssignActorToObjectPUBLIC(WorkshopNPCScript assignedActor, WorkshopObjectScript assignedObject, bool bResetMode = false)
	; bResetMode: true means to ignore TryToAssignFarms call (ResetWorkshop calls it once at the end)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	AssignActorToObjectPUBLIC actor=" + assignedActor + ", object=" + assignedObject)
	; lock editing
	GetEditLock()

	AssignActorToObject(assignedActor, assignedObject, bResetMode = bResetMode)

	EditLock = false
endFunction

; private version - called only by this script
; bResetMode: TRUE means to skip trying to assign other resource objects of this type
; bAddActorCheck: TRUE is default; FALSE means to skip adding the actor - calling function guarantees that the actor is already assigned to this workshop (speed optimization)
function AssignActorToObject(WorkshopNPCScript assignedActor, WorkshopObjectScript assignedObject, bool bResetMode = false, bool bAddActorCheck = true)
	wsTrace("	AssignActorToObject actor=" + assignedActor + ", object=" + assignedObject + ", resetMode=" + bResetMode)
	WorkshopScript workshopRef
	if assignedObject.workshopID > -1
		workshopRef = GetWorkshop(assignedObject.workshopID)
	endif
	if workshopRef == NONE
		wsTrace(" 		AssignActorToObject: ERROR - " + assignedObject + " has invalid workshopID=" + assignedObject.workshopID + " - returning without assigning actor to object", 2)
		return
	endif

	; make sure I'm added to this workshop
	if bAddActorCheck
		AddActorToWorkshop(assignedActor, workshopRef, bResetMode)
	endif

	; get object's current owner
	WorkshopNPCScript previousOwner = assignedObject.GetAssignedActor()

	; is this a bed or work object?
	if assignedObject.IsBed()
		wsTrace("		Bed - clear ownership of any other bed, then assign")
		; bed
		; find bed that was assigned to this actor and clear ownership

		ObjectReference[] WorkshopBeds = GetBeds(workshopRef)
		int i = 0
		while i < WorkshopBeds.Length
			WorkshopObjectScript theBed = WorkshopBeds[i] as WorkshopObjectScript
			if theBed && theBed.GetActorRefOwner() == assignedActor
				theBed.AssignActor(NONE)
			endif
			i += 1
		endWhile

		; mark assigned object as assigned to this actor
		assignedObject.AssignActor(assignedActor)
		wsTrace("		Bed - assigned " + assignedObject + " to " + assignedActor +": owner=" + assignedObject.GetActorRefOwner())
	elseif assignedObject.HasKeyword(WorkshopWorkObject)
		; work object
		; actor no longer counts as "new"
		assignedActor.bNewSettler = false

		; is object already assigned to this actor?
		bool bAlreadyAssigned = (previousOwner == assignedActor)
		wsTrace("		bAlreadyAssigned=" + bAlreadyAssigned)
		; unassign actor from whatever he was doing
		actorValue multiResourceValue = assignedActor.assignedMultiResource
		; if assigned actor is assigned to multi-resource, AND this has that resource, don't unassign him - can work on multiple resource objects
		wsTrace("		assignedMultiResource=" + multiResourceValue + ", assignedObject.HasResourceValue=" + assignedObject.HasResourceValue(multiResourceValue))
		bool bShouldUnassign = true
		if (multiResourceValue && assignedObject.HasResourceValue(multiResourceValue))
			; same multi resource - may not need to unassign if actor has enough unused resource points left
			float totalProduction = assignedActor.multiResourceProduction + assignedObject.GetResourceRating(multiResourceValue)
			int resourceIndex = GetResourceIndex(multiResourceValue)
			wsTrace("		totalProduction (with new object)=" + totalProduction + ", allowed max production=" + WorkshopRatings[resourceIndex].maxProductionPerNPC)
			if totalProduction <= WorkshopRatings[resourceIndex].maxProductionPerNPC
				bShouldUnassign = false
				; don't unassign - can work on multiple resource objects
				wsTrace("		actor already assigned to this resource type - don't unassign")
			else
				wsTrace("		actor will exceed max production for this resource type - DO unassign")
			endif
		elseif bAlreadyAssigned
			; already assigned
			wsTrace("		actor already assigned to this object - don't unassign")
			bShouldUnassign = false
		endif

		if bShouldUnassign
			wsTrace("		bShouldUnassign = true: unassigning " + assignedActor + " from previous work object")
			; unassign actor from previous work object
			UnassignActor(assignedActor, bSendUnassignEvent = !bAlreadyAssigned)
		endif

		; unassign current owner, if any (and different from new owner)
		if previousOwner && previousOwner != assignedActor
			wsTrace("		unassign previous owner " + previousOwner)
			UnassignActorFromObject(previousOwner, assignedObject)
		endif

		; mark assigned object as assigned to this actor
		assignedObject.AssignActor(assignedActor)

		; flag actor as a worker
		assignedActor.SetWorker(true)

		; 1.5 - new 24-hour work flag
		if assignedObject.bWork24Hours
			assignedActor.bWork24Hours = true 
		endif

		; if assigned object has scavenge rating, flag worker as scavenger (for packages)
		if assignedObject.HasResourceValue(WorkshopRatings[WorkshopRatingScavengeGeneral].resourceValue)
			assignedActor.SetScavenger(true)
		endif

		; add vendor faction if any
		if assignedObject.VendorType > -1
			SetVendorData(workshopRef, assignedActor, assignedObject)
		endif

		; update workshop ratings for new assignment
		UpdateWorkshopRatingsForResourceObject(assignedObject, workshopRef)

		; remove "unassigned" resource value
		assignedActor.SetValue(WorkshopRatings[WorkshopRatingPopulationUnassigned].resourceValue, 0)

		; to save time, in reset mode we ignore this and do it at the end
		if !bResetMode
			; reset unassigned population count
			SetUnassignedPopulationRating(workshopRef)
		endif

		; special cases:
		; is this a multi-resource object?
		if assignedObject.HasMultiResource()
			multiResourceValue = assignedObject.GetMultiResourceValue()
			; flag actor with this keyword
			assignedActor.SetMultiResource(multiResourceValue)
			assignedActor.AddMultiResourceProduction(assignedObject.GetResourceRating(multiResourceValue))
			if !bResetMode
				TryToAssignResourceType(workshopRef, multiResourceValue)				
			endif
		endif

		; reset ai to get him to notice the new markers
		assignedActor.EvaluatePackage()

	wsTrace("	AssignActorToObject actor=" + assignedActor + ", object=" + assignedObject + ", resetMode=" + bResetMode + " DONE: bAlreadyAssigned=" + bAlreadyAssigned)

		; send custom event for this object
		; don't send event in reset mode, or if already assigned to this actor
		if bAlreadyAssigned == false
			Var[] kargs = new Var[2]
			kargs[0] = assignedObject
			kargs[1] = workshopRef
			wsTrace(" 	sending WorkshopActorAssignedToWork event")
			SendCustomEvent("WorkshopActorAssignedToWork", kargs)		
		endif
	endif
endFunction

; utility function to calculate and set unassigned population rating on workshop
function SetUnassignedPopulationRating(WorkshopScript workshopRef)
	ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
	; count how many are unassigned
	int unassignedPopulation = 0
	int i = 0
	while i < WorkshopActors.Length
		WorkshopNPCScript theActor = WorkshopActors[i] as WorkShopNPCScript
		if theActor && theActor.bIsWorker == false
			unassignedPopulation += 1
		endif
		i += 1
	endWhile

	SetResourceData(WorkshopRatings[WorkshopRatingPopulationUnassigned].resourceValue, workshopRef, unassignedPopulation)
endFunction

; utility function for setting/clearing vendor data on an actor
function SetVendorData(WorkshopScript workshopRef, WorkshopNPCScript assignedActor, WorkshopObjectScript assignedObject, bool bSetData = true)
	wsTrace("	SetVendorData actor=" + assignedActor + ", object=" + assignedObject + ", bSetData=" + bSetData)

	if assignedObject.VendorType > -1
		wsTrace("		vendor type " + assignedObject.VendorType)
		WorkshopVendorType vendorData = WorkshopVendorTypes[assignedObject.VendorType]
		if vendorData
			; -- vendor faction
			wsTrace("		vendor faction " + vendorData.VendorFaction)
			if bSetData
				assignedActor.AddToFaction(vendorData.VendorFaction)
				if vendorData.keywordToAdd01
					assignedActor.AddKeyword(vendorData.keywordToAdd01)
				endif
			else
				assignedActor.RemoveFromFaction(vendorData.VendorFaction)
				if vendorData.keywordToAdd01
					assignedActor.RemoveKeyword(vendorData.keywordToAdd01)
				endif
			endif

			; -- assign vendor chests
			ObjectReference[] vendorContainers = workshopRef.GetVendorContainersByType(assignedObject.VendorType)
			int i = 0
			while i <= assignedObject.vendorLevel
				if bSetData
					wsTrace("		linking to " + vendorContainers[i] + " with keyword " + VendorContainerKeywords.GetAt(i) as Keyword)
					assignedActor.SetLinkedRef(vendorContainers[i], VendorContainerKeywords.GetAt(i) as Keyword)
				else
					assignedActor.SetLinkedRef(NONE, VendorContainerKeywords.GetAt(i) as Keyword)
				endif
				i += 1
			endWhile

			; special vendor data
			if bSetData
				if assignedActor.specialVendorType > -1 && assignedActor.specialVendorType == assignedObject.VendorType
					; link to special vendor containers
					if assignedActor.specialVendorContainerBase
						; create the container ref if it doesn't exist yet
						if assignedActor.specialVendorContainerRef == NONE
							assignedActor.specialVendorContainerRef = WorkshopHoldingCellMarker.PlaceAtMe(assignedActor.specialVendorContainerBase)
						endif
						wsTrace("		special vendor matching this type: linking to " + assignedActor.specialVendorContainerRef + " with keyword " + VendorContainerKeywords.GetAt(VendorTopLevel+1) as Keyword)
						; link using 4th keyword
						assignedActor.SetLinkedRef(assignedActor.specialVendorContainerRef, VendorContainerKeywords.GetAt(VendorTopLevel+1) as Keyword)
					endif
					if assignedActor.specialVendorContainerRefUnique
						wsTrace("		special vendor matching this type: linking to " + assignedActor.specialVendorContainerRefUnique + " with keyword " + VendorContainerKeywords.GetAt(VendorTopLevel+2) as Keyword)
						; link using 4th keyword
						assignedActor.SetLinkedRef(assignedActor.specialVendorContainerRefUnique, VendorContainerKeywords.GetAt(VendorTopLevel+2) as Keyword)
					endif
				endif
			else
				; always clear for safety
				if assignedActor.specialVendorContainerRef
					assignedActor.specialVendorContainerRef.Delete()
					assignedActor.specialVendorContainerRef = NONE
					; clear link
					assignedActor.SetLinkedRef(NONE, VendorContainerKeywords.GetAt(VendorTopLevel+1) as Keyword)
				endif
				if assignedActor.specialVendorContainerRefUnique
					; clear link
					assignedActor.SetLinkedRef(NONE, VendorContainerKeywords.GetAt(VendorTopLevel+2) as Keyword)
				endif

			endif

		else
			; ERROR
		endif
	endif

endFunction

function AssignCaravanActorPUBLIC(WorkshopNPCScript assignedActor, Location destinationLocation)
	; NOTE: package on alias uses two actor values to condition travel between the two workshops
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	AssignCaravanActorPUBLIC actor=" + assignedActor + ", destination=" + destinationLocation)
	; lock editing
	GetEditLock()

	; get destination workshop
	WorkshopScript workshopDestination = GetWorkshopFromLocation(destinationLocation)

	; current workshop
	WorkshopScript workshopStart = GetWorkshop(assignedActor.GetWorkshopID())
	; unassign this actor from any current job
	UnassignActor(assignedActor)

	; is this actor already assigned to a caravan?
	int caravanIndex = CaravanActorAliases.Find(assignedActor)
	if caravanIndex < 0
		; add to caravan actor alias collection
		wsTrace("		AssignCaravanActorPUBLIC actor=" + assignedActor + " IsUnique()=" + assignedActor.GetActorBase().IsUnique())
		CaravanActorAliases.AddRef(assignedActor)
		if assignedActor.GetActorBase().IsUnique() == false && assignedActor.GetValue(WorkshopProhibitRename) == 0
			wsTrace("		AssignCaravanActorPUBLIC actor=" + assignedActor + " not unique, putting in rename alias")
			; put in "rename" alias
			CaravanActorRenameAliases.AddRef(assignedActor)
		endif
	else
		; clear current location link
		Location oldDestination = GetWorkshop(assignedActor.GetCaravanDestinationID()).myLocation
		workshopStart.myLocation.RemoveLinkedLocation(oldDestination, WorkshopCaravanKeyword)
	endif
	
	int destinationID = workshopDestination.GetWorkshopID()

	; set destination actor value (used to find destination workshop from actor)
	assignedActor.SetValue(WorkshopCaravanDestination, destinationID)
	wsTrace("		AssignCaravanActorPUBLIC: destination=" + assignedActor.GetValue(WorkshopCaravanDestination) + " start=" + assignedActor.GetWorkshopID())

	; make caravan ref type
	if assignedActor.IsCreated()
		assignedActor.SetLocRefType(workshopStart.myLocation, WorkshopCaravanRefType)
	endif

	; add linked refs to actor (for caravan package)
	assignedActor.SetLinkedRef(workshopStart.GetLinkedRef(WorkshopLinkCenter), WorkshopLinkCaravanStart)
	assignedActor.SetLinkedRef(workshopDestination.GetLinkedRef(WorkshopLinkCenter), WorkshopLinkCaravanEnd)

	; add link between locations
	;debug.trace(self + " AssignCaravanActorPUBLIC: linking " + workshopStart.myLocation + "(" + workshopStart + ") to " + workshopDestination.myLocation + "(" + workshopDestination + ")")
	workshopStart.myLocation.AddLinkedLocation(workshopDestination.myLocation, WorkshopCaravanKeyword)

	; 1.6: send custom event for this actor
	Var[] kargs = new Var[2]
	kargs[0] = assignedActor
	kargs[1] = workshopStart
	wsTrace(" 	sending WorkshopActorCaravanAssign event")
	SendCustomEvent("WorkshopActorCaravanAssign", kargs)

	; stat update
	Game.IncrementStat("Supply Lines Created")

	; unlock editing
	EditLock = false
endFunction

; call this to temporarily turn on/off a caravan actor - remove brahmin and unlink
function TurnOnCaravanActor(WorkshopNPCScript caravanActor, bool bTurnOn, bool bBrahminCheck = true)
	; find linked locations
	WorkshopScript workshopStart = GetWorkshop(caravanActor.GetWorkshopID())

	Location startLocation = workshopStart.myLocation
	Location endLocation = GetWorkshop(caravanActor.GetCaravanDestinationID()).myLocation

	if bTurnOn
		; add link between locations
		startLocation.AddLinkedLocation(endLocation, WorkshopCaravanKeyword)
	else
		; unlink locations
		startLocation.RemoveLinkedLocation(endLocation, WorkshopCaravanKeyword)
	endif

	if bBrahminCheck
		CaravanActorBrahminCheck(caravanActor, bTurnOn)
	endif
endFunction

; check to see if this actor needs a new brahmin, or if current brahmin should be flagged for delete
function CaravanActorBrahminCheck(WorkshopNPCScript actorToCheck, bool bShouldHaveBrahmin = true)
	wsTrace(" CaravanActorBrahminCheck: caravan actor " + actorToCheck + ", bShouldHaveBrahmin=" + bShouldHaveBrahmin)

	; is my brahmin dead?
	if actorToCheck.myBrahmin && actorToCheck.myBrahmin.IsDead()
		; clear
		CaravanBrahminAliases.RemoveRef(actorToCheck.myBrahmin)
		actorToCheck.myBrahmin = NONE
	endif

	; should I have a brahmin?
	if CaravanActorAliases.Find(actorToCheck) > -1 && bShouldHaveBrahmin && actorToCheck.IsWounded() == false
		; if I don't have a brahmin, make me a new one
		if actorToCheck.myBrahmin == NONE && actorToCheck.IsWounded() == false
			actorToCheck.myBrahmin = actorToCheck.placeAtMe(CaravanBrahmin) as Actor
			actorToCheck.myBrahmin.SetActorRefOwner(actorToCheck)
			CaravanBrahminAliases.AddRef(actorToCheck.myBrahmin)
			actorToCheck.myBrahmin.SetLinkedRef(actorToCheck, WorkshopLinkFollow)
		endif
	else
		; clear and delete brahmin
		if actorToCheck.myBrahmin
			wsTrace(" CaravanActorBrahminCheck: actor " + actorToCheck + " has brahmin, removing")
			; clear this and mark brahmin for deletion
			Actor deleteBrahmin = actorToCheck.myBrahmin
			CaravanBrahminAliases.RemoveRef(deleteBrahmin)
			actorToCheck.myBrahmin = NONE
			deleteBrahmin.Delete()
			deleteBrahmin.SetLinkedRef(NONE, WorkshopLinkFollow)
		endif
	endif
endFunction

; called when player loses control of a workshop - clears all caravans to/from this workshop
function ClearCaravansFromWorkshopPUBLIC(WorkshopScript workshopRef)
	; NOTE: package on alias uses two actor values to condition travel between the two workshops
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	ClearCaravansFromWorkshopPUBLIC workshop=" + workshopRef)
	; lock editing
	GetEditLock()

	; check all caravan actors for either belonging to this workshop, or targeting it - unassign them
	int i = CaravanActorAliases.GetCount() - 1 ; start at top of list since we may be removing things from it

	while i	> -1
		WorkshopNPCScript theActor = CaravanActorAliases.GetAt(i) as WorkShopNPCScript
		if theActor
			; check start and end locations
			int destinationWorkshopID = theActor.GetCaravanDestinationID()
			wstrace("		destinationID=" + destinationWorkshopID)
			wstrace("		destinationworkshop=" + GetWorkshop(destinationWorkshopID))
			WorkshopScript endWorkshop = GetWorkshop(destinationWorkshopID)
			WorkshopScript startWorkshop = GetWorkshop(theActor.GetWorkshopID())
			wsTrace("		" + i + ": " + theActor + ": supply line between " + startWorkshop + " and " + endWorkshop)
			if endWorkshop == workshopRef || startWorkshop == workshopRef
				; unassign this actor
				UnassignActor(theActor)
			endif
		endif
		i += -1 ; decrement
	endWhile

	; unlock editing
	EditLock = false

endFunction


function AddToWorkshopRecruitAlias(Actor assignableActor)
	wsTrace(" AddToWorkshopRecruitAlias " + assignableActor)
	if assignableActor
		WorkshopRecruit.ForceRefTo(assignableActor)
	else
		WorkshopRecruit.Clear()
	endif
endFunction

; called by dialogue/quests to add non-persistent actor (not already in a dialogue quest alias) to workshop system using workshop settlement menu
; actorToAssign: if NONE, use the actor in WorkshopRecruit alias
location function AddActorToWorkshopPlayerChoice(Actor actorToAssign = NONE, bool bWaitForActorToBeAdded = true, bool bPermanentActor = false)
	if actorToAssign == NONE
		actorToAssign = WorkshopRecruit.GetActorRef()
	endif
	
	wsTrace(" AddActorToWorkshopPlayerChoice " + actorToAssign, bNormalTraceAlso = true)
	; this only works on actors with the workshop script
	WorkShopNPCScript workshopActorToAssign = actorToAssign as WorkShopNPCScript
	if !workshopActorToAssign
		wsTrace(" WARNING: AddActorToWorkshopPlayerChoice: invalid actor " + actorToAssign + " - NOT assigned", 2)
		return None
	endif

	keyword keywordToUse = WorkshopAssignHome
	if bPermanentActor
		keywordToUse = WorkshopAssignHomePermanentActor
	endif

	WorkshopScript previousWorkshop = GetWorkshop(workshopActorToAssign.GetWorkshopID())
	Location previousLocation = NONE
	if previousWorkshop
		previousLocation = previousWorkshop.myLocation
	endif

	; 102314: allow non-population actors to be assigned to any workshop
	FormList excludeKeywordList
	if workshopActorToAssign.bCountsForPopulation
		excludeKeywordList = WorkshopSettlementMenuExcludeList
	endif 
	Location newLocation = workshopActorToAssign.OpenWorkshopSettlementMenuEx(akActionKW=keywordToUse, aLocToHighlight=previousLocation, akExcludeKeywordList=excludeKeywordList)

	if bWaitForActorToBeAdded && newLocation
		; wait for menu to resolve (when called in scenes)
		int failsafeCount = 0
		while failsafeCount < 5 && workshopActorToAssign.GetWorkshopID() == -1
			wsTrace("...waiting...")
			failsafeCount += 1
			utility.wait(0.5)
		endWhile
	endif
	wsTrace("AddActorToWorkshopPlayerChoice DONE")

	return newLocation	
endFunction

; called by dialogue/quests to add an existing actor to a workshop by bringing up workshop settlement menu
; actorToAssign: if NONE, use the actor in WorkshopRecruit alias
location function AddPermanentActorToWorkshopPlayerChoice(Actor actorToAssign = NONE, bool bWaitForActorToBeAdded = true)
	return AddActorToWorkshopPlayerChoice(actorToAssign, bWaitForActorToBeAdded, true)
endFunction

; called by dialogue/quests to add an existing actor to a workshop
; actorToAssign: if NONE, use the actor in WorkshopRecruit alias
; newWorkshopID: if -1, bring up message box to pick the workshop (TEMP)
function AddPermanentActorToWorkshopPUBLIC(Actor actorToAssign = NONE, int newWorkshopID = -1, bool bAutoAssign = true)
	if actorToAssign == NONE
		actorToAssign = WorkshopRecruit.GetActorRef()
	endif
	
	wsTrace(" AddPermanentActorToWorkshopPUBLIC " + actorToAssign, bNormalTraceAlso = true)
	; this only works on actors with the workshop script
	WorkShopNPCScript workshopActorToAssign = actorToAssign as WorkShopNPCScript
	if !workshopActorToAssign
		wsTrace(" WARNING: AddPermanentActorToWorkshopPUBLIC: invalid actor " + actorToAssign + " - NOT assigned", 2)
		return
	endif


	if newWorkshopID < 0
		actorToAssign.OpenWorkshopSettlementMenu(WorkshopAssignHomePermanentActor)
		; NOTE: event from menu is handled by WorkshopNPCScript
	else
		GetEditLock()

		WorkshopScript newWorkshop = GetWorkshop(newWorkshopID)
		wstrace("	assigning " + actorToAssign+ " to " + newWorkshop)
		; put in "ignore for cleanup" faction so that RE quests can shut down
		actorToAssign.AddToFaction(REIgnoreForCleanup)
		; remove from rescued faction to stop those hellos
		actorToAssign.RemoveFromFaction(REDialogueRescued)

		; make Boss loc ref type for this location
		if workshopActorToAssign.IsCreated()
			workshopActorToAssign.SetAsBoss(newWorkshop.myLocation)
		endif
		; add to alias collection for existing actors - gives them packages to stay at new "home"
		PermanentActorAliases.AddRef(workshopActorToAssign)
		; add to the workshop
		AddActorToWorkshop(workshopActorToAssign, newWorkshop)

		; try to automatically assign to do something:
		if bAutoAssign
			TryToAutoAssignActor(newWorkshop, workshopActorToAssign)
		endif

		; send custom event for this actor
		Var[] kargs = new Var[2]
		kargs[0] = actorToAssign
		kargs[1] = newWorkshopID
		SendCustomEvent("WorkshopAddActor", kargs)		

		; unlock editing
		EditLock = false
	endif

endFunction

; utility function used to assign home marker to workshop actor
function AssignHomeMarkerToActor(Actor actorToAssign, WorkshopScript workshopRef)
	; if sandbox link exists, use that - otherwise use center marker
	ObjectReference homeMarker = workshopRef.GetLinkedRef(WorkshopLinkSandbox)
	if homeMarker == NONE
		homeMarker = workshopRef.GetLinkedRef(WorkshopLinkCenter)
	endif
	actorToAssign.SetLinkedRef(homeMarker, WorkshopLinkHome)
endFunction

function AddCollectionToWorkshopPUBLIC(RefCollectionAlias thecollection, WorkshopScript workshopRef, bool bResetMode = false)
	GetEditLock()
	int i = 0
	while i < theCollection.GetCount()
		WorkshopNPCScript theActor = theCollection.GetAt(i) as WorkshopNPCScript
		if theActor
			AddActorToWorkshop(theActor, workshopRef, bResetMode)
		endif
		i += 1
	endWhile

	; unlock editing
	EditLock = false
endFunction

; called by external scripts to assign or reassign a workshop actor to a new workshop location
function AddActorToWorkshopPUBLIC(WorkshopNPCScript assignedActor, WorkshopScript workshopRef, bool bResetMode = false)
	GetEditLock()

	AddActorToWorkshop(assignedActor, workshopRef, bResetMode)

	; unlock editing
	EditLock = false
endFunction

function AddActorToWorkshop(WorkshopNPCScript assignedActor, WorkshopScript workshopRef, bool bResetMode = false, ObjectReference[] WorkshopActors = NONE)
	; bResetMode: true means to ignore TryToAssignFarms/Beds calls (ResetWorkshop calls it once at the end)
	; WorkshopActors: if NONE, get new list, otherwise use passed in list (to save time)

; As called from ResetWorkshop:
;AddActorToWorkshop(actorRef, workshopRef, true, WorkshopActors)
	
	wsTrace("	AddActorToWorkshop actor=" + assignedActor + ", workshop=" + workshopRef + ", resetMode? " + bResetMode)
	
	bool bResetHappiness = false

	; if already in the list, do nothing
	if WorkshopActors == NONE
		WorkshopActors = GetWorkshopActors(workshopRef)
	endif

	bool bAlreadyAssigned = false
	if WorkshopActors.Find(assignedActor) > -1 && assignedActor.GetWorkshopID() == workshopRef.GetWorkshopID()
		wsTrace("	AddActorToWorkshop actor=" + assignedActor + " already assigned to this workshop")
		bAlreadyAssigned = true
	endif

	; if actor is currently assigned to a different workshop, remove from that one
	int oldWorkshopID = assignedActor.GetWorkshopID()
	wsTrace("	AddActorToWorkshop: oldWorkshopID=" + oldWorkshopID)
	if oldWorkshopID > -1 && oldWorkshopID != workshopRef.GetWorkshopID()
		; 89671: no need to remove actor from workshop completely when assigning to different workshop
		UnassignActor(assignedActor, false)
		; clear "new settler" flag on actor
		assignedActor.bNewSettler = false
	endif
	wsTrace("	AddActorToWorkshop: step 2")

	if bAlreadyAssigned == false
		assignedActor.SetWorkshopID(workshopRef.GetWorkshopID())
		if workshopRef.SettlementOwnershipFaction && workshopRef.UseOwnershipFaction && assignedActor.bApplyWorkshopOwnerFaction
			if assignedActor.bCountsForPopulation
				assignedActor.SetCrimeFaction(workshopRef.SettlementOwnershipFaction)
			else
				assignedActor.SetFactionOwner(workshopRef.SettlementOwnershipFaction)
			endif
		endif
		
		; add linked ref so workshop knows about me
		assignedActor.SetLinkedRef(workshopRef, WorkshopItemKeyword)

		; assign "home" linked ref (for packages)
		AssignHomeMarkerToActor(assignedActor, workshopRef)

		; "stamp" alias data so they keep it even after being in alias - allows them to initialize to correct package even if Reset hasn't finished running
		ApplyWorkshopAliasData(assignedActor)
		wsTrace("	AddActorToWorkshop: step 3")

		; set player ownership actor value
		assignedActor.UpdatePlayerOwnership(workshopRef)
		
		; 98730: Recalc workshop ratings on old workshop (if there is one) now that actor is linked to new workshop
		if oldWorkshopID > -1 && oldWorkshopID != workshopRef.GetWorkshopID()
			WorkshopScript oldWorkshopRef = GetWorkshop(oldWorkshopID)		
			if oldWorkshopRef
				oldWorkshopRef.RecalculateWorkshopResources()
			endif 
		endif

		; if this was the first actor, this is the first actor - set happiness to "baseline" and modifiers to 0
		if assignedActor.bCountsForPopulation
			int totalPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
			float currentHappiness = workshopRef.GetValue(WorkshopRatings[WorkshopRatingHappiness].resourceValue)
			wsTrace("	AddActorToWorkshop: step 4: totalPopulation=" + totalPopulation)
			if totalPopulation == 0
				wsTrace("	AddActorToWorkshop: total population was ZERO, resetting happiness")
				; clear happiness modifier
				SetResourceData(WorkshopRatings[WorkshopRatingHappinessModifier].resourceValue, workshopRef, 0)
				if bResetMode
					; just set happiness to baseline (nothing else we can do)
					SetResourceData(WorkshopRatings[WorkshopRatingHappiness].resourceValue, workshopRef, startingHappiness)
					SetResourceData(WorkshopRatings[WorkshopRatingHappinessTarget].resourceValue, workshopRef, startingHappiness)
				else
					; reset happiness to happiness target later (once everything is assigned)
					bResetHappiness = true
				endif
				; set "last attacked" to a very large number (so they don't act like they were just attacked)
				SetResourceData(WorkshopRatings[WorkshopRatingLastAttackDaysSince].resourceValue, workshopRef, 99)
			endif
			; update workshop rating - increment unassigned actors resource value
			assignedActor.SetValue(WorkshopRatings[WorkshopRatingPopulationUnassigned].resourceValue, 1)

			; update vendor data in all stores
			UpdateVendorFlagsAll(workshopRef)
		endif

		; assign persist location (if created)
		if assignedActor.IsCreated()
			assignedActor.SetPersistLoc(workshopRef.myLocation)
			; make Boss loc ref type for this location
			if assignedActor.bIsSynth
				wstrace(" 	" + assignedActor + " is a SYNTH - adding synth ref type")
				assignedActor.SetLocRefType(workshopRef.myLocation, WorkshopSynthRefType)
				wstrace(" 	" + assignedActor + " GetLocRefType = " + assignedActor.GetLocRefTypes())
				assignedActor.ClearFromOldLocations() ; 101931: make sure location data is correct
			elseif assignedActor.bCountsForPopulation
				assignedActor.SetAsBoss(workshopRef.myLocation)
			endif
		endif

	endif

	; reset mode - called by ResetWorkshop (so usually these actors will already be assigned to the workshop)
	if bResetMode
		; clear multiresource production VALUE on reset - otherwise we keep adding to the total each time the actor is added during Reset process
		wsTrace("	AddActorToWorkshop: clearing multiResourceProduction")
		assignedActor.multiResourceProduction = 0.0
	endif

	; clear actor worker flag
	if workshopRef.PlayerHasVisited && bAlreadyAssigned == false
		wsTrace("	AddActorToWorkshop: clearing worker flag")
		assignedActor.SetWorker(false)
	endif

	; try to assign a bed
	wsTrace("	AddActorToWorkshop: step 5 - try to assign a bed, maybe")

	if !bResetMode
		TryToAssignBedToActor(workshopRef, assignedActor)
	endif

	; pick up new package if necessary
	assignedActor.EvaluatePackage()

	if !workshopRef.RecalculateWorkshopResources()
		wsTrace(" 	RecalculateWorkshopResources returned false - add population manually")
		; add population manually if couldn't recalc
		ModifyResourceData(WorkshopRatings[WorkshopRatingPopulation].resourceValue, workshopRef, 1)
	endif

	if !bResetMode && bResetHappiness
		ResetHappiness(workshopRef)
	endif

	wsTrace("	AddActorToWorkshop: DONE")
endFunction

; reset happiness to happiness target - used when first adding population to a workshop location
function ResetHappiness(WorkshopScript workshopRef)
	wsTrace(" ResetHappiness for " + workshopRef)
	; recalc happiness, then set happiness to current happiness target
	workshopRef.DailyUpdate(bRealUpdate = false)		
	float happinessTarget = workshopRef.GetValue(WorkshopRatings[WorkshopRatingHappinessTarget].resourceValue)
	; if current target below min, set target to min
	if happinessTarget < startingHappinessMin
		happinessTarget = startingHappinessMin
		SetResourceData(WorkshopRatings[WorkshopRatingHappinessTarget].resourceValue, workshopRef, happinessTarget)
	endif
	; set happiness to target
	SetResourceData(WorkshopRatings[WorkshopRatingHappiness].resourceValue, workshopRef, happinessTarget)
endFunction

; call to stamp actor with WorkshopActorApply alias data
function ApplyWorkshopAliasData(actor theActor)
	WorkshopActorApply.ApplyToRef(theActor)
endFunction


; specialized function to unassign actor from one object - called during AssignActor process on former owner of newly assigned object
function UnassignActorFromObject(WorkshopNPCScript theActor, WorkshopObjectScript theObject, bool bSendUnassignEvent = true)
	wsTrace("	UnassignActorFromObject " + theActor + " from " + theObject)

	WorkshopScript workshopRef = GetWorkshop(theActor.GetWorkshopID())

	; do I currently own this object?
	if theObject.GetActorRefOwner() == theActor
		wsTrace("		unassigning " + theObject)
		; this will also add the actor to the unassigned actor list (when it unassigns the last object)
		UnassignObject(theObject)
		if bSendUnassignEvent
			; send custom event for this object
			Var[] kargs = new Var[2]
			kargs[0] = theObject
			kargs[1] = workshopRef
			wsTrace(" 	sending WorkshopActorUnassigned event")
			SendCustomEvent("WorkshopActorUnassigned", kargs)
		endif

		; do I still have any work objects?

		; work object?
		ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
		int foundIndex = WorkshopActors.Find(theActor)
		if foundIndex > -1
			; unassign ownership of all work objects
			ObjectReference[] ResourceObjects = workshopRef.GetWorkshopOwnedObjects(theActor)
			if ResourceObjects.Length == 0
				; clear actor work flags
				theActor.SetMultiResource(NONE)
				theActor.SetWorker(false)
			endif
		endif
	endif
endFunction

function RemoveActorFromWorkshopPUBLIC(WorkshopNPCScript theActor)
	; lock editing
	GetEditLock()

	UnassignActor(theActor, true, true)

	; unlock editing
	EditLock = false
endFunction

; call this function to unassign this actor from any job
; (always called as part of assignment process)
; bRemoveFromWorkshop - use TRUE when you want to completely remove the actor from the workshop (e.g. when the actor dies) 
function UnassignActor(WorkshopNPCScript theActor, bool bRemoveFromWorkshop = false, bool bSendUnassignEvent = true)
	wsTrace("	UnassignActor " + theActor + " bRemoveFromWorkshop=" + bRemoveFromWorkshop)

	WorkshopScript workshopRef = GetWorkshop(theActor.GetWorkshopID())

	; am I currently assigned to something?
	int foundIndex = -1

	; caravan?
	foundIndex = CaravanActorAliases.Find(theActor)
	if foundIndex > -1
		; remove me from the caravan alias collection
		CaravanActorAliases.RemoveRef(theActor)
		CaravanActorRenameAliases.RemoveRef(theActor)

		Location startLocation = workshopRef.myLocation
		Location endLocation = GetWorkshop(theActor.GetCaravanDestinationID()).myLocation
		; unlink locations
		startLocation.RemoveLinkedLocation(endLocation, WorkshopCaravanKeyword)

		; set back to Boss
		if theActor.IsCreated()
			; Patch 1.4: allow custom loc ref type on workshop NPC
			theActor.SetAsBoss(startLocation)
		endif

		; update workshop rating - increment unassigned actors total
		theActor.SetValue(WorkshopRatings[WorkshopRatingPopulationUnassigned].resourceValue, 1)

		; clear caravan brahmin
		CaravanActorBrahminCheck(theActor)

		; 1.6: send custom event for this actor
		Var[] kargs = new Var[2]
		kargs[0] = theActor
		kargs[1] = workshopRef
		wsTrace(" 	sending WorkshopActorCaravanUnassign event")
		SendCustomEvent("WorkshopActorCaravanUnassign", kargs)
	endif

	; work object?
	;/
	ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
	foundIndex = WorkshopActors.Find(theActor)
	if foundIndex > -1
	/;
	if theActor.GetWorkshopID() == workshopRef.GetWorkshopID()
		; unassign ownership of all work objects
		ObjectReference[] ResourceObjects = workshopRef.GetWorkshopOwnedObjects(theActor)
		int i = 0
		while i < ResourceObjects.Length
			; TEMP
			wsTrace("		owned ref " + i + ": " + ResourceObjects[i])
			; end TEMP
			WorkshopObjectScript theObject = ResourceObjects[i] as WorkshopObjectScript
			wsTrace("		owned workshop object " + i + ": " + theObject)
			if theObject.RequiresActor()
				wsTrace("		unassigning " + theObject)
				; this will also add the actor to the unassigned actor list (when it unassigns the last object)
				UnassignObject(theObject)
				if bSendUnassignEvent
					; send custom event for this object
					Var[] kargs = new Var[2]
					kargs[0] = theObject
					kargs[1] = workshopRef
					wsTrace(" 	sending WorkshopActorUnassigned event")
					SendCustomEvent("WorkshopActorUnassigned", kargs)
				endif
			endif
			i += 1
		endWhile

		; clear actor work flags
		theActor.SetMultiResource(NONE)
		theActor.SetWorker(false)
	else
		wsTrace("		UnassignActor: NOTE - " + theActor + " is owned by workshop " + theActor.GetWorkshopID() + ", current workshop=" + currentWorkshopID + " - can't unassign ownership of work objects.")
	endif

	if bRemoveFromWorkshop
		wsTrace("	UnassignActor " + theActor + ": removing from workshop")
		; completely remove from workshop

		; clear workshop linked ref
		theActor.SetLinkedRef(NONE, WorkshopItemKeyword)

		; clear applied alias data
		WorkshopActorApply.RemoveFromRef(theActor)

		; remove from permanent actor collection alias
		PermanentActorAliases.RemoveRef(theActor)
		
		; remove ownership flag to prevent trading
		theActor.SetValue(WorkshopPlayerOwnership, 0)	

		; PATCH - remove workshop ID as well
		theActor.SetWorkshopID(-1)

		; update population rating on workshop's location
		if workshopRef.RecalculateWorkshopResources() == false
			; decrement population manually if couldn't recalc
			ModifyResourceData(WorkshopRatings[WorkshopRatingPopulation].resourceValue, workshopRef, -1)
		endif
	endif

endFunction

; call to unassign specified object from any actor
function UnassignObject(WorkshopObjectScript theObject, bool bRemoveObject = false)
	; bRemoveObject = true  - means object is being completely removed from the workshop system
	wsTrace("	UnassignObject " + theObject)
	if bRemoveObject
		wsTrace("		REMOVING...")
	endif
	
	WorkshopScript workshopRef = GetWorkshop(theObject.workshopID)

	; get my owner (if any)
	WorkshopNPCScript assignedActor = theObject.GetAssignedActor()

	if assignedActor
		; clear my ownership
		theObject.AssignActor(none)

		; 1.5 - new 24-hour work flag
		if theObject.bWork24Hours
			assignedActor.bWork24Hours = false 
		endif

		; clear link if it exists
		if theObject.AssignedActorLinkKeyword
			assignedActor.SetLinkedRef(NONE, theObject.AssignedActorLinkKeyword)
		endif

		;  clear vendor faction from actor
		if theObject.VendorType > -1
			SetVendorData(workshopRef, assignedActor, theObject, false)
		endif

		; is does my owner own anything else?
		ObjectReference[] ResourceObjects = workshopRef.GetWorkshopOwnedObjects(assignedActor)
		int i = 0
		bool ownedObject = false
		while i < ResourceObjects.Length && ownedObject == false
			WorkshopObjectScript resourceObject = ResourceObjects[i] as WorkshopObjectScript
			if resourceObject && resourceObject.IsBed() == 0
				; exit loop
				ownedObject = true
			endif
			i += 1
		endWhile

		if ownedObject == false
			; this was the only thing I owned - add owner back to unassigned actor list
			assignedActor.SetValue(WorkshopRatings[WorkshopRatingPopulationUnassigned].resourceValue, 1)
			assignedActor.SetMultiResource(NONE)
			assignedActor.SetWorker(false)
		endif
	endif

	if assignedActor || bRemoveObject
		; update ratings
		UpdateWorkshopRatingsForResourceObject(theObject, workshopRef, bRemoveObject)
	endif

endFunction

function AssignObjectToWorkshop(WorkshopObjectScript workObject, WorkshopScript workshopRef, bool bResetMode = false)
	; bResetMode: true means to ignore TryToAssignFarms/Beds calls (ResetWorkshop calls it once at the end)
	wsTrace("	AssignObjectToWorkshop: " + workObject)

	; is this a bed? if so, add to unassigned bed list
	if workObject.IsBed()
		wsTrace("		bed - check for ownership")

		; am I owned?
		if workObject.IsActorAssigned() && workObject.GetAssignedActor()
			WorkShopNPCScript owner = workObject.GetAssignedActor()
			; is this actor in our actor list?
			ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
			int actorIndex = WorkshopActors.Find(owner)
			wsTrace("		check actors list: index=" + actorIndex)
			if actorIndex == -1
				; clear ownership - this actor has gone missing somehow
				wsTrace("    " + workObject + " assigned actor not found - clearing assignment")
				workObject.AssignActor(None)
			endif
		endif

		if !bResetMode
			TryToAssignBeds(workshopRef)
		endif
	; work object?
	else
		wsTrace("		work object - check for ownership")
		if workObject.HasKeyword(WorkshopWorkObject)
			; is this already owned by an actor?
			WorkShopNPCScript owner = workObject.GetAssignedActor()
			wsTrace("		GetAssignedActor=" + owner)
			if workObject.IsActorAssigned() && owner
				; is this actor in our actor list?
				ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
				int actorIndex = WorkshopActors.Find(owner)
				wsTrace("		owner index=" + actorIndex)
				if actorIndex > -1
					; found the actor
					; assign actor
					AssignActorToObject(owner, workObject, bResetMode = bResetMode, bAddActorCheck = false)
					; NOTE don't need to call UpdateWorkshopRatingsForResourceObject - this is called in AssignActorToObject
				else
					; clear ownership - this actor has gone missing somehow
					wsTrace("    " + workObject + " assigned actor not found - clearing assignment")
					workObject.AssignActor(None)
				endif
			endif
		endif
		; update ratings
		UpdateWorkshopRatingsForResourceObject(workObject, workshopRef)
	endif

	if workObject.HasMultiResource()
		if !bResetMode
			TryToAssignResourceType(workshopRef, workObject.GetMultiResourceValue())
		endif
	endif

endFunction

; turn on/off radio
function UpdateRadioObject(WorkshopObjectScript radioObject)
	wsTrace("UpdateRadioObject for " + radioObject)
	WorkshopScript workshopRef = GetWorkshop(radioObject.workshopID)
	; radio
	if radioObject.bRadioOn && radioObject.IsPowered()
		wsTrace("	starting radio station")
		; make me a transmitter and start radio scene
		; 1.6: allow workshop-specific override
		if workshopRef.WorkshopRadioRef
			workshopRef.WorkshopRadioRef.Enable() ; enable in case this is a unique station
			radioObject.MakeTransmitterRepeater(workshopRef.WorkshopRadioRef, workshopRef.workshopRadioInnerRadius, workshopRef.workshopRadioOuterRadius)
			if workshopRef.WorkshopRadioScene.IsPlaying() == false
				workshopRef.WorkshopRadioScene.Start()
			endif
		else 
			radioObject.MakeTransmitterRepeater(WorkshopRadioRef, workshopRadioInnerRadius, workshopRadioOuterRadius)
			if WorkshopRadioScene01.IsPlaying() == false
				WorkshopRadioScene01.Start()
			endif
		endif
		if workshopRef.RadioBeaconFirstRecruit == false
			WorkshopEventRadioBeacon.SendStoryEvent(akRef1 = workshopRef)
		endif
	else
		wsTrace("	stopping radio station")
		radioObject.MakeTransmitterRepeater(NONE, 0, 0)
		; if unique radio, turn it off completely
		if workshopRef.WorkshopRadioRef && workshopRef.bWorkshopRadioRefIsUnique
			wsTrace("	custom station: disabling transmitter")
			workshopRef.WorkshopRadioRef.Disable()
			; stop custom scene if unique
			workshopRef.WorkshopRadioScene.Stop()
		endif
	endif
	; send power change event so quests can react to this
	workshopRef.RecalculateWorkshopResources()
	SendPowerStateChangedEvent(radioObject, workshopRef)
endFunction

; call any time an object's status changes
; adds/removes this object's ratings to the workshop's ratings
; also updates the object's production flag
function UpdateWorkshopRatingsForResourceObject(WorkshopObjectScript workshopObject, WorkshopScript workshopRef, bool bRemoveObject = false, bool bRecalculateResources = true)
	wsTrace("	UpdateWorkshopRatingsForObject " + workshopObject + " bRemoveObject=" + bRemoveObject)

	; do we need to add/remove this from the workshop ratings?
	int ratingMult = 0
	if bRemoveObject
		UpdateVendorFlags(workshopObject, workshopRef)
	else
		UpdateVendorFlags(workshopObject, workshopRef)
	endif

	; should we recalculate everything?
	if bRecalculateResources
		workshopRef.RecalculateWorkshopResources()
	endif

	; special case
	if workshopObject.HasKeyword(WorkshopRadioObject)
		UpdateRadioObject(workshopObject)
	endif
	wsTrace("	UpdateWorkshopRatingsForObject DONE")
endFunction

; possibly temp - helper function to recalculate total resource damage when an object is destroyed


function RecalculateResourceDamage(WorkshopScript workshopRef)
	RecalculateResourceDamageForResource(workshopRef, WorkshopRatings[WorkshopRatingFood].resourceValue)
	RecalculateResourceDamageForResource(workshopRef, WorkshopRatings[WorkshopRatingWater].resourceValue)
	RecalculateResourceDamageForResource(workshopRef, WorkshopRatings[WorkshopRatingSafety].resourceValue)
	RecalculateResourceDamageForResource(workshopRef, WorkshopRatings[WorkshopRatingPower].resourceValue)
	RecalculateResourceDamageForResource(workshopRef, WorkshopRatings[WorkshopRatingPopulation].resourceValue)
endFunction

function RecalculateResourceDamageForResource(WorkshopScript workshopRef, actorValue akResource)
	wstrace(" RecalculateResourceDamageForResource " + workshopRef + " for " + akResource)
	ActorValue damageRatingValue = GetDamageRatingValue(akResource)
	; if not a resource with a damage rating, don't need to do anything
	if damageRatingValue
		float totalDamage = workshopRef.GetWorkshopResourceDamage(akResource)
		; set new damage total
		wstrace(" 	total damage= " + totalDamage)
		SetResourceData(damageRatingValue, workshopRef, totalDamage)
	endif
endFunction

; call to update vendor flags on all stores (e.g. for when adding population)
function UpdateVendorFlagsAll(WorkshopScript workshopRef)
	; get stores
	ObjectReference[] stores = GetResourceObjects(workshopRef, WorkshopRatings[WorkshopRatingVendorIncome].resourceValue)			
	; update vendor data for all of them (might trigger top level vendor for increase in population)
	int i = 0
	while i < stores.Length
		WorkshopObjectScript theStore = stores[i] as WorkshopObjectScript
		if theStore
			UpdateVendorFlags(theStore, workshopRef)
		endif
		i += 1
	endWhile
endFunction

; helper function for UpdateWorkshopRatingsForResourceObject
; update vendor flags based on this object's production state
function UpdateVendorFlags(WorkshopObjectScript workshopObject, WorkshopScript workshopRef)
	; set this to true if we are going to change state
	bool bShouldVendorFlagBeSet = false
	if workshopObject.VendorType > -1
		WorkshopVendorType vendorType = WorkshopVendorTypes[workshopObject.VendorType]

		if vendorType
			; if a vendor object, increment global if necessary
			if workshopObject.VendorType > -1 && workshopObject.vendorLevel >= VendorTopLevel
				; check for minimum connected population
				int linkedPopulation = GetLinkedPopulation(workshopRef, false) as int
				int totalPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
				int vendorPopulation = linkedPopulation + totalPopulation

				; NOTE: known issue - we're not checking for population dropping below minimum to invalidate top vendor flag. Acceptable.
				
				if vendorType
					wsTrace("		vendor population=" + vendorPopulation + ", vendorType.minPopulationForTopVendor=" + vendorType.minPopulationForTopVendor)
					if vendorPopulation >= vendorType.minPopulationForTopVendor && workshopRef.OwnedByPlayer
						bShouldVendorFlagBeSet = true
					endif
				endif
			endif
			if bShouldVendorFlagBeSet
				if workshopObject.bVendorTopLevelValid == false
					; change state:
					; increment top vendor global
					vendorType.topVendorFlag.Mod(1.0)
					workshopObject.bVendorTopLevelValid = true
				endif
			else
				if workshopObject.bVendorTopLevelValid == true
					; change state:
					; increment top vendor global
					vendorType.topVendorFlag.Mod(-1.0)
					workshopObject.bVendorTopLevelValid = false
				endif
			endif
		endif
	endif
endFunction

; assign spare beds to any NPCs that don't have one
function TryToAssignBeds(WorkshopScript workshopRef)
	wsTrace(" 	TryToAssignBeds for " + workshopRef.GetWorkshopID() + " (currentworkshop=" + currentWorkshopID + ")")
	if workshopRef.GetWorkshopID() == currentWorkshopID
		; go through actors, see if any of them need beds
		ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
		wsTrace("		" + WorkshopActors.Length + " NPCs to check:")
		int i = 0
		while i < WorkshopActors.Length
			WorkshopNPCScript theActor = WorkshopActors[i] as WorkShopNPCScript
			TryToAssignBedToActor(workshopRef, theActor)
			i += 1
		endWhile
	endif
endFunction

; assign a spare bed to the specified actor if he needs one
function TryToAssignBedToActor(WorkshopScript workshopRef, WorkshopNPCScript theActor)
	wsTrace("	TryToAssignBedToActor: " + theActor)
;	wsTrace("		WorkshopCurrentBeds.IsOwnedObjectInList(theActor)=" + WorkshopCurrentBeds.IsOwnedObjectInList(theActor))
	if theActor
;		ObjectReference myBed = WorkshopCurrentBeds.GetFirstOwnedObject(theActor)
		if ActorOwnsBed(workshopRef, theActor) == false
			wsTrace("		TryToAssignBedToActor: " + theActor + " has no bed ")
			ObjectReference[] beds = GetBeds(workshopRef)
			; loop through beds until we find an unassigned one
			int i = 0
			while i < beds.Length
				WorkshopObjectScript bedToAssign = beds[i] as WorkshopObjectScript
				if bedToAssign && bedToAssign.IsActorAssigned() == false
					; assign actor to bed
					bedToAssign.AssignActor(theActor)
					; break out of loop
					i = beds.Length
				endif
				i += 1
			endWhile
		endif
	endif
endFunction


int function GetNextIndex(int currentIndex, int maxIndex)
	if currentIndex < maxIndex
		return currentIndex + 1
	else
		return 0
	endif
endFunction

; try to assign all objects of the specified resource types
function TryToAssignResourceType(WorkshopScript workshopRef, ActorValue resourceValue)
	wsTrace("	TryToAssignResourceType " + resourceValue)

	if !resourceValue
		return
	endif

	ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
	wsTrace("		Total actors: " + WorkshopActors.length)

	; if no actors, exit
	if WorkshopActors.Length == 0
		return
	endif

	int resourceIndex = GetResourceIndex(resourceValue)

	; first, make array of workers who are assigned to this resource type
	WorkshopNPCScript[] workers = new WorkShopNPCScript[WorkshopActors.Length]
	int maxWorkerIndex = -1

	; go through actor list, find workers
	int actorIndex = 0
	while actorIndex < WorkshopActors.Length
		WorkshopNPCScript theActor = WorkshopActors[actorIndex] as WorkshopNPCScript
		wsTrace("		actor " + theActor + " assigned to " + resourceValue + "? " + (theActor.assignedMultiResource == resourceValue) )
		if theActor && theActor.assignedMultiResource == resourceValue && theActor.multiResourceProduction < WorkshopRatings[resourceIndex].maxProductionPerNPC
			maxWorkerIndex += 1
			workers[maxWorkerIndex] = theActor
		endif
		actorIndex += 1
	endWhile

	wsTrace("		Found " + (maxWorkerIndex + 1) + " actors assigned to " + resourceValue)

	; shortcut - if no workers, we're done
	if maxWorkerIndex < 0
		return
	endif

	; we now have an array of workers - loop through object list until we can't assign any more workers (or run out of farms)
	int objectIndex = 0
	bool availableworkers = true 	; this gets set to false if we ever loop through the actor list and find no workers left with available production slots
	int currentWorkerIndex = 0

	; get resources of specified type, undamaged only (no point in auto-assigning damaged objects)
	ObjectReference[] ResourceObjects = GetResourceObjects(workshopRef, resourceValue, 2)
	wsTrace("		Found " + ResourceObjects.Length + " undamaged " + resourceValue + " objects")

	while (objectIndex < ResourceObjects.Length) && availableworkers
		WorkshopObjectScript workshopObject = ResourceObjects[objectIndex] as WorkshopObjectScript
		; this produces the resource - is it owned?
		if workshopObject.RequiresActor() && !workshopObject.IsActorAssigned()
			float resourceRating = workshopObject.GetResourceRating(resourceValue)
			; loop through actors looking for available workers
			bool assignedResource = false
			; save starting index so we only loop once
			int startingIndex = currentWorkerIndex
			bool exitLoop = false
			while !assignedResource && !exitLoop
				; get the next Worker
				WorkshopNPCScript theActor = workers[currentWorkerIndex]
				float resourceTotal = theActor.multiResourceProduction
				wsTrace("   	found Worker " + currentWorkerIndex + ": " + theActor + " at " + resourceTotal + " production")
				if (resourceTotal + resourceRating <= WorkshopRatings[resourceIndex].maxProductionPerNPC)
					wsTrace("   	found available Worker, resource rating=" + resourceRating + " - assign")
					assignedResource = true
					; I can produce this, so assign me to it
					; NOTE: resetMode = TRUE (so it skips calling this function again); addActorCheck = FALSE (since we already know this actor is assigned to this workshop)
					AssignActorToObject(theActor, workshopObject, bResetMode = true, bAddActorCheck = false) 
					; add to total
					resourceTotal += resourceRating
					; save out new resource total on me
					theActor.multiResourceProduction = resourceTotal
				endif
				; get next Worker index
				currentWorkerIndex = GetNextIndex(currentWorkerIndex, maxWorkerIndex)
				; if we're back to start, exit Worker loop
				if currentWorkerIndex == startingIndex
					exitLoop = true
				endif
			endWhile

			; if we didn't assign anything, AND the foodRating was 1 (minimum), then no workers left - we can stop looking
			if !assignedResource && resourceRating == 1
				wsTrace("	No more workers with spare production - stop looking")
				availableWorkers = false
			endif
		endif

		objectIndex += 1
	endwhile

endFunction

; called by each workshop on the timer update loop
function DailyWorkshopUpdate(bool bInitialize = false)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" DAILY WORKSHOP UPDATE", bNormalTraceAlso = true)
	wsTrace("------------------------------------------------------------------------------ ")
	
	; produce for all workshops
	int workshopIndex = 0


	; calculate time interval between each workshop update
	if bInitialize
		dailyUpdateIncrement = 0.0 ; short increment on initialization - FOR NOW just keep it at 0 so it happens as fast as possible
	else
	 	dailyUpdateIncrement = dailyUpdateSpreadHours/Workshops.Length
	endif

	SendCustomEvent("WorkshopDailyUpdate")		
endFunction

; used by test terminal to automatically calculate and force an attack
function TestForceAttack()
	GetWorkshop(currentWorkshopID).CheckForAttack(true)
endFunction

; trigger story manager attack
function TriggerAttack(WorkshopScript workshopRef, int attackStrength)
	wsTrace("   TriggerAttack on " + workshopRef)
	;#102677 - Don't throw workshop attacks for vassal locations
	if workshopRef.HasKeyword(WorkshopType02Vassal) == false
	if !WorkshopEventAttack.SendStoryEventAndWait(akLoc = workshopRef.myLocation, aiValue1 = attackStrength, akRef1 = workshopRef)
		; Removed - now that we have an attack message, don't do fake attacks
		;/
		wsTrace(" 	no attack quest started - resolve if player is not nearby")
		; no quest started - resolve if player is not at this location
		if workshopRef.Is3DLoaded() == false && Game.Getplayer().GetDistance(workshopRef) > 4000
			ResolveAttack(workshopRef, attackStrength, RaiderFaction)
		endif
		/;
	endif
	endif
	wsTrace("   TriggerAttack DONE")
endFunction

int Property maxAttackStrength = 100 const auto Hidden
int Property maxDefenseStrength = 100 const auto Hidden

int function CalculateAttackStrength(int foodRating, int waterRating)
	; attack strength: based on "juiciness" of target
	int attackStrength = math.min(foodRating + waterRating, maxAttackStrength) as int
	int attackStrengthMin = attackStrength/2 * -1
	int attackStrengthMax = attackStrength/2
	wsTrace("		Base attackStrength=" + attackStrength)
	wsTrace("		  attack strength variation=" + attackStrengthMin + " to " + attackStrengthMax)

	attackStrength = math.min(attackStrength + utility.randomInt(attackStrengthMin, attackStrengthMax), maxAttackStrength) as int
	wsTrace("		attackStrength=" + attackStrength)
	return attackStrength
endFunction

int function CalculateDefenseStrength(int safety, int totalPopulation)
	int defenseStrength = math.min(safety + totalPopulation, maxDefenseStrength) as int
	wsTrace("		defenseStrength=" + defenseStrength)
	return defenseStrength
endFunction

; called to set lastAttack, lastAttack faction
function RecordAttack(WorkshopScript workshopRef, Faction attackingFaction)
	; only reset last attack days if we can find the attacking faction
	FollowersScript:EncDefinition encDef = Followers.GetEncDefinition(factionToCheck = attackingFaction)
	if encDef
		; set days since last attack to 0
		SetResourceData(WorkshopRatings[WorkshopRatingLastAttackDaysSince].resourceValue, workshopRef, 0)
		; set attacking faction ID
		SetResourceData(WorkshopRatings[WorkshopRatingLastAttackFaction].resourceValue, workshopRef, encDef.LocEncGlobal.GetValue())
	endif
endFunction

int property resolveAttackMaxAttackRoll = 150 auto const 		; max allowed attack roll
float property resolveAttackAllowedDamageMin = 25.0 auto const 	; this is as low as max allowed damage can go

; called by attack quests if they need to be resolved "off stage"
; return value: TRUE = attackers won, FALSE = defenders won (to match CheckResolveAttackk on WorkshopAttackScript)
bool function ResolveAttack(WorkshopScript workshopRef, int attackStrength, Faction attackFaction)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("   ResolveAttack on " + workshopRef + " attack strength=" + attackStrength)
	ObjectReference containerRef = workshopRef.GetContainer()
	if !containerRef
		wsTrace(self + " ERROR - no container linked to workshop " + workshopRef + " with " + WorkshopLinkContainer, 2)
		return false
	endif

	bool attackersWin = false

	int totalPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
	int safety = workshopRef.GetValue(WorkshopRatings[WorkshopRatingSafety].resourceValue) as int

	; record attack in location data
	RecordAttack(workshopRef, attackFaction)

	; defense strength: safety + totalPopulation 
	int defenseStrength = CalculateDefenseStrength(safety, totalPopulation)
	wsTrace("   ResolveAttack on " + workshopRef + ":		attack strength=" + attackStrength)
	wsTrace("   ResolveAttack on " + workshopRef + ":		defenseStrength=" + defenseStrength)

	; "combat resolution" - each roll 1d100 + strength, if attack > defense that's the damage done.
	int attackRoll = utility.randomInt() + attackStrength
	wsTrace("   ResolveAttack on " + workshopRef + ":		original attack roll=" + attackRoll)
	; don't let attack roll exceed 150 - makes high defense more likely to win
	attackRoll = math.min(attackRoll, resolveAttackMaxAttackRoll) as int

	int defenseRoll = utility.randomInt() + defenseStrength
	wsTrace("   ResolveAttack on " + workshopRef + ":		attack roll=" + attackRoll)
	wsTrace("   ResolveAttack on " + workshopRef + ":		defense roll=" + defenseRoll)

	if attackRoll > defenseRoll
		attackersWin = true

		; limit max damage based on defense - but max can't go below 25
		float maxAllowedDamage = math.max(resolveAttackAllowedDamageMin, 100-defenseStrength)
		wsTrace("   ResolveAttack on " + workshopRef + ":		maxAllowedDamage=" + maxAllowedDamage)
		float damage = math.min(attackRoll - defenseRoll, maxAllowedDamage)

		; get current damage - ignore if already more than this attack
		float currentDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageCurrent].resourceValue)
		if currentDamage < damage
			wsTrace("   ResolveAttack on " + workshopRef + ":	New damage=" + damage)
			float totalDamagePoints = 0.0
			; now set damage to all the resources
			totalDamagePoints += SetRandomDamage(workshopRef, WorkshopRatings[WorkshopRatingFood].resourceValue, damage)	; use total rating for food, water, safety, power
			totalDamagePoints += SetRandomDamage(workshopRef, WorkshopRatings[WorkshopRatingWater].resourceValue, damage)
			totalDamagePoints += SetRandomDamage(workshopRef, WorkshopRatings[WorkshopRatingSafety].resourceValue, damage)
			totalDamagePoints += SetRandomDamage(workshopRef, WorkshopRatings[WorkshopRatingPower].resourceValue, damage)
			totalDamagePoints += SetRandomDamage(workshopRef, WorkshopRatings[WorkshopRatingPopulation].resourceValue, damage)
			; now calc total points to get "real" max damage
			float totalResourcePoints = GetTotalResourcePoints(workshopRef)
			float maxDamage = 0.0
			if totalResourcePoints > 0
				maxDamage = totalDamagePoints/totalResourcePoints * 100
			endif

			wsTrace("   ResolveAttack on " + workshopRef + ":	Actual max damage=" + maxDamage + "=" + totalDamagePoints + "/" + totalResourcePoints)
			; max damage = starting "maximum" damage inflicted by the attack
			SetResourceData(WorkshopRatings[WorkshopRatingDamageMax].resourceValue, workshopRef, maxDamage)
			; current damage starts out at the max, then goes down as repairs are made during the daily update
			SetResourceData(WorkshopRatings[WorkshopRatingDamageCurrent].resourceValue, workshopRef, maxDamage)
		else
			wsTrace("   ResolveAttack on " + workshopRef + ":	Current damage=" + currentDamage + ", ignore new damage=" + damage)
		endif

		; in any case, remove resources from container based on current damage
		if containerRef
			int stolenFood = math.ceiling(containerRef.GetItemCount(WorkshopConsumeFood) * damage/100)
			int stolenWater = math.ceiling(containerRef.GetItemCount(WorkshopConsumeWater) * damage/100)
			int stolenScrap = math.ceiling(containerRef.GetItemCount(WorkshopConsumeScavenge) * damage/100)
			int stolenCaps = math.ceiling(containerRef.GetItemCount(Game.GetCaps()) * damage/100)
			
			wsTrace("   ResolveAttack on " + workshopRef + ":	Destroy stored resources: " + stolenFood + " food, " + stolenWater + " water, " + stolenScrap + " scrap, " + stolenCaps + " caps")
			containerRef.RemoveItem(WorkshopConsumeFood, stolenFood)
			containerRef.RemoveItem(WorkshopConsumeWater, stolenWater)
			containerRef.RemoveItemByComponent(WorkshopConsumeScavenge, stolenScrap)
			containerRef.RemoveItem(Game.GetCaps(), stolenCaps)
		endif
	endif

	return attackersWin
endFunction

; utility function - get current population damage
float function GetPopulationDamage(WorkshopScript workshopRef)
	; difference between base value and current value
	float populationDamage = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) - workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue)
	; add in any extra damage (recorded but not yet processed into wounded actors)
	populationDamage += workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue)

	return populationDamage
endFunction

; utility function - return total resource rating (potential), used for damage % calculation
float function GetTotalResourcePoints(WorkshopScript workshopRef)
	; total resource points = sum of all potential resource points + total population
	float totalPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) ; total population is base value
	float foodTotal = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingFood].resourceValue)
	float waterTotal = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingWater].resourceValue)
	float safetyTotal = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingSafety].resourceValue)
	float powerTotal = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPower].resourceValue)
	wstrace("GetTotalResourcePoints for " + workshopRef + ": ")
	wstrace("	" + totalPopulation + " population")
	wstrace("	" + foodTotal + " food")
	wstrace("	" + waterTotal + " water")
	wstrace("	" + safetyTotal + " safety")
	wstrace("	" + powerTotal + " power")
	return (totalPopulation + foodTotal + waterTotal + safetyTotal + powerTotal)

endFunction

; return total damage points
float function GetTotalDamagePoints(WorkshopScript workshopRef)
	; RESOURCE CHANGE: population damage is recorded in difference between base and current population value
	float populationDamage = GetPopulationDamage(workshopRef)
	float foodDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageFood].resourceValue)
	float waterDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageWater].resourceValue)
	float safetyDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageSafety].resourceValue)
	float powerDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamagePower].resourceValue)

	wstrace("GetTotalDamagePoints for " + workshopRef + ": ")
	wstrace("	" + populationDamage + " population")
	wstrace("	" + foodDamage + " food")
	wstrace("	" + waterDamage + " water")
	wstrace("	" + safetyDamage + " safety")
	wstrace("	" + powerDamage + " power")

	return (populationDamage + foodDamage + waterDamage + safetyDamage + powerDamage)
endFunction

function ProduceFood(WorkshopScript workshopref, int totalFoodToProduce)
	if totalFoodToProduce > 0
		; randomly produce food based on what kinds of crops are at this workshop

		; get % chance of each food type
		float[] foodTypeChance = new float[WorkshopFoodTypes.Length]

		int i = 0
	;	float totalFoodToProduce = workshopRef.GetValue(WorkshopRatings[WorkshopRatingFood].resourceValue)
		while i < WorkshopFoodTypes.Length
			ActorValue foodType = WorkshopFoodTypes[i].resourceValue
			foodTypeChance[i] = workshopRef.GetValue(foodType)/totalFoodToProduce
			wsTrace(self + " " + foodType + "=" + foodTypeChance[i] + " chance")
			i += 1
		endWhile

		; now for each food to produce, pick randomly from each valid type
		ObjectReference containerRef = workshopRef.GetContainer()
		int foodProduced = 0
		while foodProduced < totalFoodToProduce
			float randomRoll = Utility.RandomFloat()
			wsTrace(workshopRef + ": food production " + foodProduced + ": random roll=" + randomRoll)
			i = 0
			float currentChance = 0
			int foodTypeIndex = -1
			while i < foodTypeChance.Length && foodTypeIndex == -1
				currentChance += foodTypeChance[i]
				wsTrace(workshopRef + ": 	" + i + " currentChance=" + currentChance)
				if randomRoll <= currentChance
					foodTypeIndex = i
					wsTrace(workshopRef + ": 	" + i + " picking food index " + i)
				endif
				i += 1
			endWhile
			if foodTypeIndex > -1
				; found a food type - produce it
				wsTrace(workshopRef + " producing food type " + WorkshopFoodTypes[foodTypeIndex].resourceValue)
				containerRef.AddItem(WorkshopFoodTypes[foodTypeIndex].foodObject)
			else
				; didn't - produce random food
				wsTrace(workshopRef + " producing generic food ")
				containerRef.AddItem(WorkshopProduceFood)
			endif
			foodProduced += 1
		endWhile
	endif
endFunction


float function SetRandomDamage(WorkshopScript workshopRef, ActorValue resourceValue, float baseDamage)
	; get current rating

	; always use base value = total max production (ignoring things that are out of production/wounded/etc.)
	float rating = workshopRef.GetBaseValue(resourceValue)

	; randomize baseDamage a bit
	float realDamageMult = (baseDamage + utility.RandomFloat(baseDamage/2.0 * -1, baseDamage/2.0))/100
	realDamageMult = math.min(realDamageMult, 1.0)
	realDamageMult = math.max(realDamageMult, 0.0)
	wsTrace("		SetRandomDamage for " + resourceValue + ": " + rating + " * " + realDamageMult)

	int damage = math.Ceiling(rating * realDamageMult)
	; figure out damage rating:
	actorValue damageRatingValue = GetDamageRatingValue(resourceValue)
	if damageRatingValue
		wsTrace("			setting damage " + damage + " for " + damageRatingValue)
		SetResourceData(damageRatingValue, workshopRef, damage)
		; adjust resource value down for this damage - except for population (since we display the base value in the interface)
		if resourceValue != WorkshopRatings[WorkshopRatingPopulation].resourceValue
			ModifyResourceData(resourceValue, workshopRef, damage*-1)
		endif
		return damage
	else
		wsTrace("		ERROR - no damage rating found for actor value " + resourceValue)
		return 0
	endif
endFunction

; return population from linked workshops
;  if bIncludeProductivityMult = true, return the population * productivityMult for each linked workshop
float function GetLinkedPopulation(WorkshopScript workshopRef, bool bIncludeProductivityMult = false)
	int workshopID = workshopRef.GetWorkshopID()
	float totalLinkedPopulation = 0

	; get all linked workshop locations
	Location[] linkedLocations = workshopRef.myLocation.GetAllLinkedLocations(WorkshopCaravanKeyword)
	int index = 0
	while (index < linkedLocations.Length)
		; NOTE: wounded caravan actors "unlink" their location, then relink it when healed (so we don't have to worry about that here)
		; get linked workshop from location
		int linkedWorkshopID = WorkshopLocations.Find(linkedLocations[index])
		if linkedWorkshopID > 0
			; get the linked workshop
			WorkshopScript linkedWorkshop = GetWorkshop(linkedWorkshopID)
			wsTrace(workshopRef + " found linked workshop " + linkedWorkshop)
			; for this, we will use only unwounded population
			float population = Math.max(linkedWorkshop.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) - GetPopulationDamage(workshopRef), 0.0)
			if bIncludeProductivityMult
				float productivity = linkedWorkshop.GetProductivityMultiplier(WorkshopRatings)
				wsTrace(workshopRef + " productivity=" + productivity)
				population = population * productivity
			endif
			; add linked population to total
			totalLinkedPopulation += population
		else
			wsTrace("GetLinkedPopulation: ERROR - workshop location " + linkedLocations[index] + " not found in workshop location array", 2)
		endif
		index += 1
	endwhile

	wsTrace(workshopRef + " total linked population=" + totalLinkedPopulation)

	return totalLinkedPopulation
endFunction

function TransferResourcesFromLinkedWorkshops(WorkshopScript workshopRef, int neededFood, int neededWater)
	int workshopID = workshopRef.GetWorkshopID()
	ObjectReference containerRef = workshopRef.GetContainer()

	int totalPopulation = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
	int availableFood = containerRef.GetItemCount(WorkshopConsumeFood)
	int availableWater = containerRef.GetItemCount(WorkshopConsumeWater)

	; get all linked workshop locations
	Location[] linkedLocations = workshopRef.myLocation.GetAllLinkedLocations(WorkshopCaravanKeyword)
	int index = 0
	while (index < linkedLocations.Length)
		; NOTE: wounded caravan actors "unlink" their location, then relink it when healed (so we don't have to worry about that here)
		; get linked workshop from location
		int linkedWorkshopID = WorkshopLocations.Find(linkedLocations[index])
		if linkedWorkshopID > 0
			; get the linked workshop
			WorkshopScript linkedWorkshop = GetWorkshop(linkedWorkshopID)
			wsTrace(workshopRef + " found linked workshop " + linkedWorkshop)

			ObjectReference linkedContainer = linkedWorkshop.GetContainer()
			if linkedContainer
				; add needed resources to our container 
;				int neededFood = totalPopulation - availableFood
				if neededFood > 0
					wsTrace(workshopRef + " transferring " + neededFood + " FOOD from linked workshop " + linkedWorkshop)
					linkedContainer.RemoveItem(WorkshopConsumeFood, neededFood, true, containerRef)
					; recalc available food
					availableFood = containerRef.GetItemCount(WorkshopConsumeFood)
					wsTrace(workshopRef + " new FOOD=" + availableFood)
				endif
;				int neededWater = totalPopulation - availableWater
				if neededWater > 0
					wsTrace(workshopRef + " transferring " + neededWater + " WATER from linked workshop " + linkedWorkshop)
					linkedContainer.RemoveItem(WorkshopConsumeWater, neededWater, true, containerRef)
					; recalc available water
					availableWater = containerRef.GetItemCount(WorkshopConsumeWater)
					wsTrace(workshopRef + " new WATER=" + availableWater)
				endif
			endif

		else
			wsTrace("TransferResourcesFromLinkedWorkshops: ERROR - workshop location " + linkedLocations[index] + " not found in workshop location array", 2)
		endif
		index += 1
	endwhile
endFunction

; called by ResetWorkshop
; also called when activating workshop
function SetCurrentWorkshop(WorkshopScript workshopRef)
	CurrentWorkshop.ForceRefTo(workshopRef)
	currentWorkshopID = workshopRef.GetWorkshopID()
	WorkshopCurrentWorkshopID.SetValue(currentWorkshopID)
endFunction

; clear and recalculate workshop ratings
function ResetWorkshop(WorkshopScript workshopRef)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" RESET WORKSHOP", bNormalTraceAlso = true)
	wsTrace("    Begin reset for " + workshopRef)
	wsTrace("------------------------------------------------------------------------------ ")

	GetEditLock()

	wsTrace("	ResetWorkshop: " + workshopRef + " Total population: " + workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue))

;	Debug.StartStackProfiling()

	; set current workshopID to this workshop
	int workshopID = workshopRef.GetWorkshopID()

	; set current workshopID to this workshop (can be reset when player interacts with a workshop)
	SetCurrentWorkshop(workshopref)

	; clear parent alias collections
	WorkshopNewSettler.Clear()
	WorkshopSpokesmanAfterRaiderAttack.Clear()

	; get resources and actors
	; NOTE: this HAS to be done after the aliases are cleared - otherwise things flagged for deletion will still be persisting and we'll still find them
	ObjectReference[] WorkshopActors = GetWorkshopActors(workshopRef)
	ObjectReference[] ResourceObjectsDamaged = GetResourceObjects(workshopRef, NONE, 1)
	ObjectReference[] ResourceObjectsUndamaged = GetResourceObjects(workshopRef, NONE, 2)

	; if automatic ownership turned on, clear the location if there aren't any live bosses - this will allow the player to "own" the workshop by activating it
	if workshopRef.EnableAutomaticPlayerOwnership && !workshopRef.OwnedByPlayer
		int bossIndex = 0
		int bossCount = 0
		while bossIndex < BossLocRefTypeList.GetSize()
			LocationRefType bossRefType = BossLocRefTypeList.GetAt(bossIndex) as LocationRefType
			bossCount += WorkshopLocations[workshopID].GetRefTypeAliveCount(bossRefType)
			bossIndex += 1
		endWhile

		wsTrace("	ResetWorkshop: " + workshopRef + " Checking for automatic player ownership... bossCount=" + bossCount)
		if bossCount == 0
			wsTrace("	ResetWorkshop: " + workshopRef + " 	Clearing location " + WorkshopLocations[workshopID])
			WorkshopLocations[workshopID].SetCleared(true)
		endif
	endif

	; make sure to clear FarmDiscountFaction from NPCs in unowned farm
	bool bFirstResetAfterLostControl = false
	if !workshopRef.OwnedByPlayer && workshopRef.GetValue(WorkshopPlayerLostControl) == 1
		workshopRef.SetValue(WorkshopPlayerLostControl, 2)
		bFirstResetAfterLostControl = true
	endif

	; DAMAGE - how many NPCs/objects to damage?
	; get current damage rating - so we can damage/wound things as they come in
	float currentDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageCurrent].resourceValue)/100
	wsTrace("	ResetWorkshop: " + workshopRef + "   Current damage %=" + currentDamage)
	; get current food & water ratings (before we clear them) - use this to damage resource objects as they come in
	float foodToDamage = math.Ceiling(workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageFood].resourceValue))
	float waterToDamage = math.Ceiling(workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageWater].resourceValue))
	float safetyToDamage = math.Ceiling(workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageSafety].resourceValue))
	float powerToDamage = math.Ceiling(workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamagePower].resourceValue))
	; RESOURCE CHANGE: population damage rating is now EXTRA population damage, on top of difference between base and current population rating
	;  -- used to track damage to population when location isn't loaded
	float populationDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue) + workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue) - workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue)
	int populationToDamage = math.Ceiling(populationDamage)
	
	wsTrace("	ResetWorkshop: " + workshopRef + "   food to damage=" + foodToDamage)
	wsTrace("	ResetWorkshop: " + workshopRef + "   water to damage=" + waterToDamage)
	wsTrace("	ResetWorkshop: " + workshopRef + "   safety to damage=" + safetyToDamage)
	wsTrace("	ResetWorkshop: " + workshopRef + "   power to damage=" + powerToDamage)
	wsTrace("	ResetWorkshop: " + workshopRef + "   population to damage=" + populationToDamage)

	; recalculate workshop ratings for this workbench
	workshopRef.RecalculateWorkshopResources(false)

	; get center marker
	WorkshopCenterMarker.ForceRefTo(workshopRef.GetLinkedRef(WorkshopLinkCenter))
	
	; add the actors
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	ResetWorkshop: " + workshopRef + "  ACTORS: " + WorkshopActors.length + " in area", bNormalTraceAlso = true)

	; first pass - check for already wounded actors
	int i = 0
	while i < WorkshopActors.length
		WorkshopNPCScript actorRef = WorkshopActors[i] as WorkshopNPCScript
		if actorRef && (actorRef.GetWorkshopID() == workshopID || actorRef.GetWorkshopID() < 0)

			wsTrace("	------------------------------------------------------------------------------ ")
			wsTrace("	" + i + ": " + actorRef, bNormalTraceAlso = true)
			wsTrace("	------------------------------------------------------------------------------ ")

			if actorRef.IsDead()
				; failsafe - remove them
				UnassignActor(actorRef, bRemoveFromWorkshop = true, bSendUnassignEvent = false)
			else
				; on first reset after losing control, clear discount factions from NPCs
				if bFirstResetAfterLostControl
					actorRef.RemoveFromFaction(FarmDiscountFaction)
				endif

				; make sure actor value ownership is set
				actorRef.UpdatePlayerOwnership(workshopRef)			

				; clear any remaining assignment actor values
				actorRef.StartAssignmentTimer(false)

				if workshopRef.DaysSinceLastVisit > 3
					; clear Minutemen radiant quest factions from NPCs
					actorRef.RemoveFromFaction(MinRadiantDialogueThankful)
					actorRef.RemoveFromFaction(MinRadiantDialogueDisappointed)
					actorRef.RemoveFromFaction(MinRadiantDialogueFailure)
				endif

				if actorRef.IsWounded() && actorRef.IsDead() == false
					wsTrace("	ResetWorkshop: " + workshopRef + "   found WOUNDED actor " + actorRef)
					; make sure not a caravan actor - if so, don't add to unassigned actors list
					if CaravanActorAliases.Find(actorRef) < 0
						AddActorToWorkshop(actorRef, workshopRef, true, WorkshopActors)
						; set reset flag
						actorRef.bResetDone = true
						if populationToDamage > 0
							populationToDamage += -1
						else
							; heal me
							wsTrace("	ResetWorkshop: " + workshopRef + "   HEALING actor " + actorRef)
							actorRef.SetWounded(false)
						endif
					else
						wsTrace("	ResetWorkshop: " + workshopRef + "     assigned to caravan - ignore")
					endif
				endif
			endif
		endif
		i += 1
	endWhile

	; second pass - remaining actors
	i = 0
	while i < WorkshopActors.length
		WorkshopNPCScript actorRef = WorkshopActors[i] as WorkshopNPCScript
		if actorRef && actorRef.IsDead() == false && (actorRef.GetWorkshopID() == workshopID || actorRef.GetWorkshopID() < 0) && actorRef.bResetDone == false
			wsTrace("	ResetWorkshop: " + workshopRef + "   found actor " + actorRef)
			; make sure not a caravan actor - if so, don't add to unassigned actors list
			if CaravanActorAliases.Find(actorRef) < 0
				AddActorToWorkshop(actorRef, workshopRef, true, WorkshopActors)
				; DAMAGE - clear "wounded value"
				actorRef.SetWounded(false)
				if populationToDamage > 0
					wsTrace("	ResetWorkshop: " + workshopRef + "   WOUNDING actor " + actorRef)
					actorRef.SetWounded(true)
					populationToDamage += -1
				endif
			else
				wsTrace("	ResetWorkshop: " + workshopRef + "     assigned to caravan - ignore")
			endif
		endif
		; clear reset flag on all actors as we go
		actorRef.bResetDone = false
		i += 1
	endWhile

	; clear damaged population rating - it should have all been resolved now
	SetResourceData(WorkshopRatings[WorkshopRatingDamagePopulation].resourceValue, workshopRef, 0)

	wsTrace("	ResetWorkshop: " + workshopRef + " Total population: " + workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue))

	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	ResetWorkshop: " + workshopRef + "  RESOURCE OBJECTS: " + (ResourceObjectsDamaged.length + ResourceObjectsUndamaged.length) + " in area", bNormalTraceAlso = true)
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	Check damaged objects:" + ResourceObjectsDamaged.length)
	; FIRST - loop through looking for objects already flagged as damage - decrement damage totals from them first (so we don't change which objects are damaged if when you leave and return)
	i = 0
	while i < ResourceObjectsDamaged.length
		WorkshopObjectScript resourceRef = ResourceObjectsDamaged[i] as WorkshopObjectScript
		if resourceRef
			wsTrace("	" + i + ": " + resourceRef)
			if resourceRef.workshopID == -1
				; no workshop assigned = initially placed item (not player-created)
				; so assign this to the current workshop
				resourceRef.workshopID = workshopID
				; initialize any scripted creation stuff
				resourceRef.HandleCreation(false)
			endif
				
			; if damaged, apply damage
			wsTrace("	------------------------------------------------------------------------------ ")
			wsTrace("	" + i + ": " + resourceRef + ", owner=" + resourceRef.GetAssignedActor(), bNormalTraceAlso = true)
			wsTrace("	------------------------------------------------------------------------------ ")

			; damaged - what kind of resource(s) does this produce?
			foodToDamage = UpdateResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingFood].resourceValue, foodToDamage)
			waterToDamage = UpdateResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingWater].resourceValue, waterToDamage)
			safetyToDamage = UpdateResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingSafety].resourceValue, safetyToDamage)
			powerToDamage = UpdateResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingPower].resourceValue, powerToDamage)

			; flag this object as "done" so we ignore it on next pass
			resourceRef.bResetDone = true
			AssignObjectToWorkshop(resourceRef, workshopRef, true)

			; any reset stuff the object needs to do
			resourceRef.HandleWorkshopReset()
		endif
		i += 1
	endWhile


	; now we do another pass, looking at the rest of the objects in the list
	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace("	Check undamaged objects: " + ResourceObjectsUndamaged.Length)
	i = 0
	while i < ResourceObjectsUndamaged.length
		WorkshopObjectScript resourceRef = ResourceObjectsUndamaged[i] as WorkshopObjectScript
		if resourceRef
			if resourceRef.workshopID == -1
				; no workshop assigned = initially placed item (not player-created)
				; so assign this to the current workshop
				resourceRef.workshopID = workshopID
				; initialize any scripted creation stuff
				resourceRef.HandleCreation(false)
			endif
				

			wsTrace("	------------------------------------------------------------------------------ ")
			wsTrace("	" + i + ": " + resourceRef + ", owner=" + resourceRef.GetAssignedActor() + " damaged? " + resourceRef.HasResourceDamage(), bNormalTraceAlso = true)
			wsTrace("	------------------------------------------------------------------------------ ")

			; before assigning, see if should damage
			foodToDamage = ApplyResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingFood].resourceValue, foodToDamage)
			waterToDamage = ApplyResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingWater].resourceValue, waterToDamage)
			safetyToDamage = ApplyResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingSafety].resourceValue, safetyToDamage)
			powerToDamage = ApplyResourceDamage(resourceRef, WorkshopRatings[WorkshopRatingPower].resourceValue, powerToDamage)

			AssignObjectToWorkshop(resourceRef, workshopRef, true)

			; any reset stuff the object needs to do
			resourceRef.HandleWorkshopReset()

			; clear the object's reset flag since we don't have any more passes
			resourceRef.bResetDone = false
		endif
		i += 1
	endWhile

	wstrace("	Damage pass done: foodToDamage=" + foodToDamage + ", waterToDamage=" + waterToDamage + ", safetyToDamage=" + safetyToDamage + ", powerToDamage=" + powerToDamage)
	; remaining damage is invalid - clear it
;	if foodToDamage > 0
		wstrace("		Removing " + foodToDamage + " extraneous food damage")
		SetResourceData(WorkshopRatings[WorkshopRatingDamageFood].resourceValue, workshopRef, 0)
;	endif
;	if waterToDamage > 0
		wstrace("		Removing " + waterToDamage + " extraneous water damage")
		SetResourceData(WorkshopRatings[WorkshopRatingDamageWater].resourceValue, workshopRef, 0)
;	endif
;	if safetyToDamage > 0
		wstrace("		Removing " + safetyToDamage + " extraneous safety damage")
		SetResourceData(WorkshopRatings[WorkshopRatingDamageSafety].resourceValue, workshopRef, 0)
;	endif
;	if powerToDamage > 0
		wstrace("		Removing " + powerToDamage + " extraneous power damage")
		SetResourceData(WorkshopRatings[WorkshopRatingDamagePower].resourceValue, workshopRef, 0)
;	endif

	; assign beds, farms, safety objects
	TryToAssignBeds(workshopRef)
	TryToAssignResourceType(workshopRef, WorkshopRatings[WorkshopRatingFood].resourceValue)
	TryToAssignResourceType(workshopRef, WorkshopRatings[WorkshopRatingSafety].resourceValue)

	; reset unassigned population count
	SetUnassignedPopulationRating(workshopRef)

	; set "visited" flag
	workshopRef.PlayerHasVisited = true

	wsTrace("	ResetWorkshop: " + workshopRef + " Total population: " + workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue))

	; check for new settlers
	CurrentNewSettlerCount = 0
	if WorkshopActors.Length > 0
		i = 0
		while i <  WorkshopActors.Length
			workshopNPCScript theActor = WorkshopActors[i] as workshopNPCScript
			if theActor && theActor.bIsWorker == false && theActor.bNewSettler == true
				; put first into the new settler alias so he'll forcegreet
				if CurrentNewSettlerCount == 0
					WorkshopNewSettler.ForceRefTo(theActor)
				endif
				if i >= MaxNewSettlerCount
					theActor.bNewSettler = false
					theActor.EvaluatePackage()
				endif
				CurrentNewSettlerCount += 1
			endif
			i += 1
		endWhile
	endif

	wsTrace("	ResetWorkshop: " + workshopRef + " Total population: " + workshopRef.GetValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue))

	wsTrace("------------------------------------------------------------------------------ ")
	wsTrace(" RESET WORKSHOP for " + workshopRef + "   - DONE", bNormalTraceAlso = true)
	wsTrace("------------------------------------------------------------------------------ ")

	EditLock = false
;	Debug.StopStackProfiling()
endFunction

; utility function to send custom destruction state change event (because it has to be sent from the defining script)
function SendDestructionStateChangedEvent(WorkshopObjectScript workObject, WorkshopScript workshopRef)
	wsTrace(" SendDestructionStateChangedEvent " + workshopRef)
	; send custom event for this object
	Var[] kargs = new Var[2]
	kargs[0] = workObject
	kargs[1] = workshopRef
	SendCustomEvent("WorkshopObjectDestructionStageChanged", kargs)		
endFunction

; utility function to send custom ownership state change event (because it has to be sent from the defining script)
function SendPlayerOwnershipChangedEvent(WorkshopScript workshopRef)
	wsTrace(" SendPlayerOwnershipChangedEvent " + workshopRef)
	; send custom event for this object
	Var[] kargs = new Var[2]
	kargs[0] = workshopRef.OwnedByPlayer
	kargs[1] = workshopRef
	SendCustomEvent("WorkshopPlayerOwnershipChanged", kargs)		
endFunction

; utility function to send custom destruction state change event (because it has to be sent from the defining script)
function SendPowerStateChangedEvent(WorkshopObjectScript workObject, WorkshopScript workshopRef)
	wsTrace(" SendPowerStateChangedEvent " + workshopRef)
	; send custom event for this object
	Var[] kargs = new Var[2]
	kargs[0] = workObject
	kargs[1] = workshopRef
	SendCustomEvent("WorkshopObjectPowerStageChanged", kargs)		
endFunction


; helper function for ResetWorkshop
; pass in resourceRef, keyword, current damage
; return new damage (after applying damage to this resource)
float function ApplyResourceDamage(WorkshopObjectScript resourceRef, ActorValue resourceValue, float currentDamage)
	if currentDamage > 0
		float damageAmount = math.min(resourceRef.GetResourceRating(resourceValue), currentDamage)
		if damageAmount > 0
			wsTrace("		DAMAGING: " + resourceValue + " production -" + damageAmount)
			if resourceRef.ModifyResourceDamage(resourceValue, damageAmount)
				currentDamage = currentDamage - damageAmount
			endif
		endif
	endif
	return currentDamage
endFunction

; helper function for ResetWorkshop
; pass in resourceRef, keyword, current damage
; if resourceRef already damaged, either reduce current damage by that amount or repair excess
; return new damage
float function UpdateResourceDamage(WorkshopObjectScript resourceRef, ActorValue resourceValue, float currentDamage)
	float damageAmount = resourceRef.GetResourceDamage(resourceValue)
	if damageAmount > 0
		wsTrace("		ALREADY DAMAGED: " + resourceValue + " production -" + damageAmount)
		currentDamage = currentDamage - damageAmount
		if currentDamage < 0
			; excess damage - repair this object the excess amount
			resourceRef.ModifyResourceDamage(resourceValue, currentDamage)
			currentDamage = 0
		endif
	endif
	return currentDamage
endFunction


function ClearWorkshopRatings(WorkshopScript workshopRef)
	wsTrace(" ClearWorkshopRatings")
	int i = 0
	while i < WorkshopRatings.Length
		if WorkshopRatings[i].clearOnReset
			SetResourceData(WorkshopRatings[i].resourceValue, workshopRef, 0.0)
		endif
		wsTrace("   keyword=" + WorkshopRatings[i].resourceValue + ", current value=" + workshopRef.GetValue(WorkshopRatings[i].resourceValue))
		i += 1
	endWhile
endFunction

; test function to print current workshop ratings to the log
function OutputWorkshopRatings(WorkshopScript workshopRef)
	if workshopRef == NONE
		workshopRef = GetWorkshopFromLocation(Game.GetPlayer().GetCurrentLocation())
	endif
	
	wsTrace("------------------------------------------------------------------------------ ", bNormalTraceAlso = true)
	wsTrace(" OutputWorkshopRatings " + workshopRef, bNormalTraceAlso = true)
	int i = 0
	while i < WorkshopRatings.Length
		wsTrace("   " + WorkshopRatings[i].resourceValue + ": " + workshopRef.GetValue(WorkshopRatings[i].resourceValue) + " (" + workshopRef.GetBaseValue(WorkshopRatings[i].resourceValue) + ")", bNormalTraceAlso = true)
		i += 1
	endWhile
	wsTrace("------------------------------------------------------------------------------ ", bNormalTraceAlso = true)
endFunction


; *****************************************************************************************************
; HELPER FUNCTIONS
; *****************************************************************************************************

; returns the workshopID for the supplied workshop ref
WorkshopScript function GetWorkshop(int workshopID)
	return Workshops[workshopID]
endFunction

int function GetWorkshopID(WorkshopScript workshopRef)
	int workshopIndex = Workshops.Find(workshopRef)
	if workshopIndex < 0
		wsTrace(" ERROR - workshop " + workshopRef + " not found in workshop array", 2)
	endif
	return workshopIndex
endfunction

; helper function to modify keyword data on the specified workshop
function ModifyResourceData(ActorValue pValue, WorkshopScript pWorkshopRef, float modValue)
	wsTrace(" ModifyResourceData on " + pWorkshopRef + ": actor value " + pValue + ", modValue=" + modValue)
	if pWorkshopRef == NONE || pValue == NONE
		return
	endif
	float currentValue = pWorkshopRef.GetValue(pValue)
	; don't mod value below 0
	float newValue = math.Max(modValue + currentValue, 0)
	wsTrace(" ModifyResourceData on " + pWorkshopRef + ": actor value " + pValue + ", newValue=" + newValue)

	; NOTE: we don't want to actually call ModValue since ModValue changes the actor value "modifier" pool and SetValue changes the base value
	;  so instead we always use SetValue
	SetResourceData(pValue, pWorkshopRef, newValue)
	newValue = pWorkshopRef.GetValue(pValue)
	wsTrace(" ModifyResourceData on " + pWorkshopRef + ": actor value " + pValue + ", starting value=" + currentValue + ", final value=" + newValue)
endFunction

function SetResourceData(ActorValue pValue, WorkshopScript pWorkshopRef, float newValue)
	if pValue == NONE
		return
	endif
	wsTrace(" SetResourceData: " + pWorkshopRef + ": actor value " + pValue + ", new value " + newValue)
	float oldBaseValue = pWorkshopRef.GetBaseValue(pValue)
	float oldValue = pWorkshopRef.GetValue(pValue)
	; restore any damage first, then set
	if oldValue < oldBaseValue
		pWorkshopRef.RestoreValue(pValue, oldBaseValue-oldValue)
	endif
	; now set the value
	pWorkshopRef.SetValue(pValue, newValue)
	wsTrace(" SetResourceData: " + pWorkshopRef + ":   " + pValue + " was set to new value=" + newValue + ", old value=" + oldValue + ", current value (should match new value)=" + pWorkshopRef.GetValue(pValue))
endFunction

; update current damage rating for this workshop
function UpdateCurrentDamage(WorkshopScript workshopRef)
	float totalResourcePoints = GetTotalResourcePoints(workshopRef)
	float totalDamagePoints = GetTotalDamagePoints(workshopRef)
	float currentDamage = totalDamagePoints/totalResourcePoints * 100
	wsTrace("	UpdateCurrentDamage: " + totalDamagePoints + "/" + totalResourcePoints + "=" + currentDamage)
	SetResourceData(WorkshopRatings[WorkshopRatingDamageCurrent].resourceValue, workshopRef, currentDamage)
	; update max damage if current damage is bigger
	float maxDamage = workshopRef.GetValue(WorkshopRatings[WorkshopRatingDamageMax].resourceValue)
	if currentDamage > maxDamage
		SetResourceData(WorkshopRatings[WorkshopRatingDamageMax].resourceValue, workshopRef, currentDamage)
	endif		
endFunction

int function GetResourceIndex(ActorValue pValue)
	return WorkshopRatingValues.Find(pValue)
endFunction

ActorValue function GetRatingAV(int ratingIndex)
	if ratingIndex >= 0 && ratingIndex < WorkshopRatings.Length
		return WorkshopRatings[ratingIndex].resourceValue
	else
		return NONE
	endif
endFunction

; specialized helper function - pass in rating index to WorkshopRatingValues (food, water, etc.), get back index to corresponding damage rating 
; returns -1 if not a valid rating index
ActorValue function GetDamageRatingValue(ActorValue resourceValue)
	int damageIndex = -1
	int ratingIndex = WorkshopRatingValues.Find(resourceValue)
	if ratingIndex == WorkshopRatingFood
		damageIndex = WorkshopRatingDamageFood
	elseif ratingIndex == WorkshopRatingWater
		damageIndex = WorkshopRatingDamageWater
	elseif ratingIndex == WorkshopRatingSafety
		damageIndex = WorkshopRatingDamageSafety
	elseif ratingIndex == WorkshopRatingPower
		damageIndex = WorkshopRatingDamagePower
	elseif ratingIndex == WorkshopRatingPopulation
		damageIndex = WorkshopRatingDamagePopulation
	endif
	if damageIndex > -1
		return WorkshopRatings[damageIndex].resourceValue
	else
		return NONE
	endif
endFunction

ObjectReference[] Function GetWorkshopActors(WorkshopScript workshopRef)
	return workshopRef.GetWorkshopResourceObjects(WorkshopRatings[WorkshopRatingPopulation].resourceValue)
endFunction

; aiDamageOption:
;	0 = return all objects
;	1 = return only damaged objects (at least 1 damaged resource value)
;	2 = return only undamaged objects (NO damaged resource values)
ObjectReference[] Function GetResourceObjects(WorkshopScript workshopRef, ActorValue resourceValue = NONE, int aiDamageOption = 0)
;/
	if resourceValue == NONE
		resourceValue = WorkshopResourceObject
	endif
/;
	return workshopRef.GetWorkshopResourceObjects(resourceValue, aiDamageOption)
endFunction

ObjectReference[] Function GetBeds(WorkshopScript workshopRef)
	return workshopRef.GetWorkshopResourceObjects(WorkshopRatings[WorkshopRatingBeds].resourceValue)
endFunction

; return true if actor owns a bed on this workshop
bool Function ActorOwnsBed(WorkshopScript workshopRef, WorkshopNPCScript actorRef)
	ObjectReference[] beds = GetBeds(workshopRef)
	int i = 0
	while i < beds.Length
		WorkshopObjectScript theBed = beds[i] as WorkshopObjectScript
		; if bed has faction owner, count that if I'm in that faction
		if theBed.IsFactionOwner(actorRef) || (theBed.IsActorAssigned() && theBed.GetAssignedActor() == actorRef)
			return true
		endif
		i += 1
	endWhile
	return false
endFunction

; RESOURCE CHANGE: OBSOLETE - each resource actor value now stores both total and current value
;/
; specialized helper function - pass in rating index to WorkshopRatingValues (food, water, etc.), get back index to corresponding TOTAL rating 
; returns -1 if not a valid rating index
ActorValue function GetTotalRatingValue(ActorValue resourceValue)
	int totalIndex = -1
	int ratingIndex = WorkshopRatingValues.Find(resourceValue)
	if ratingIndex == WorkshopRatingFood
		totalIndex = WorkshopRatingTotalFood
	elseif ratingIndex == WorkshopRatingWater
		totalIndex = WorkshopRatingTotalWater
	elseif ratingIndex == WorkshopRatingSafety
		totalIndex = WorkshopRatingTotalSafety
	elseif ratingIndex == WorkshopRatingPower
		totalIndex = WorkshopRatingTotalPower
	elseif ratingIndex == WorkshopRatingBonusHappiness
		totalIndex = WorkshopRatingTotalBonusHappiness
	endif
	if totalIndex > -1
		return WorkshopRatings[totalIndex].resourceValue
	else
		return NONE
	endif
endFunction
/;

; utility function for all Workshop traces
function wsTrace(string traceString, int severity = 0, bool bNormalTraceAlso = false) DebugOnly
	debug.traceUser(userlogName, " " + traceString, severity)
;	if bNormalTraceAlso
;		;debug.Trace(self + " " + traceString, severity)
;	endif
endFunction

; utility function to wait for edit lock
; increase wait time while more threads are in here
int editLockCount = 1
function GetEditLock()
	editLockCount += 1
	while EditLock
		utility.wait(0.1 * editLockCount)
	endWhile
	EditLock = true
	editLockCount -= 1
endFunction

bool function IsEditLocked()
	return EditLock
endFunction

Group WorkshopRadioData
	Scene Property WorkshopRadioScene01 Auto Const
	ObjectReference Property WorkshopRadioRef Auto Const
	Keyword Property WorkshopRadioObject Auto Const
endGroup
float workshopRadioInnerRadius = 9000.0
float workshopRadioOuterRadius = 20000.0

function RegisterForWorkshopEvents(Quest questToRegister, bool bRegister = true)
	; register for build events from workshop
	if bRegister
		wsTrace(self + " RegisterForWorkshopEvents " + questToRegister + " = " + bRegister)
		questToRegister.RegisterForCustomEvent(self, "WorkshopObjectBuilt")
		questToRegister.RegisterForCustomEvent(self, "WorkshopObjectMoved")
		questToRegister.RegisterForCustomEvent(self, "WorkshopObjectDestroyed")
		questToRegister.RegisterForCustomEvent(self, "WorkshopActorAssignedToWork")
		questToRegister.RegisterForCustomEvent(self, "WorkshopActorUnassigned")
		questToRegister.RegisterForCustomEvent(self, "WorkshopObjectDestructionStageChanged")
		questToRegister.RegisterForCustomEvent(self, "WorkshopObjectPowerStageChanged")
		questToRegister.RegisterForCustomEvent(self, "WorkshopPlayerOwnershipChanged")
		questToRegister.RegisterForCustomEvent(self, "WorkshopEnterMenu")
		questToRegister.RegisterForCustomEvent(self, "WorkshopObjectRepaired")
	else
		questToRegister.UnregisterForCustomEvent(self, "WorkshopObjectBuilt")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopObjectMoved")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopObjectDestroyed")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopActorAssignedToWork")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopActorUnassigned")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopObjectDestructionStageChanged")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopObjectPowerStageChanged")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopPlayerOwnershipChanged")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopObjectRepaired")
		questToRegister.UnregisterForCustomEvent(self, "WorkshopEnterMenu")
	endif
endFunction

Struct WorkshopObjective
	int index	;{ objective number }
	int startStage	;{ stage which started the objective }
	int doneStage	;{ stage to set when objective complete }
	int ratingIndex	;{ WorkshopParent.WorkshopRatingKeyword index}
	Keyword requiredKeyword ; optional - a keyword to check on the new built object
	GlobalVariable currentCount	; global holding current count - if filled, use ModObjectiveGlobal when new object is created
	GlobalVariable maxCount	; global holding max we're looking for - needs to be filled if currentCount is filled
	GlobalVariable percentComplete ; global holding % complete (for objective display)
	int startingCount ; this is subtracted from currentCount and maxCount when displaying the percentage (if 0, will just use real totals)
	bool useBaseValue = false ; if true, check base value instead of current value (e.g. for beds)
	bool rollbackObjective = false ; if true, can uncomplete objectives if they are now below the target value
EndStruct

; call this if you don't care which workshop the event came from
function UpdateWorkshopObjectivesAny(Quest theQuest, WorkshopObjective[] workshopObjectives, Var[] akArgs)
	if (akArgs.Length > 0)
		WorkshopScript workshopRef = akArgs[1] as WorkshopScript
		UpdateWorkshopObjectives(theQuest, workshopObjectives, workshopRef, akArgs)
	endif
endFunction

function UpdateWorkshopObjectives(Quest theQuest, WorkshopObjective[] workshopObjectives, WorkshopScript theWorkshop, Var[] akArgs)
	debug.trace(" UpdateWorkshopObjectives from " + theWorkshop)
	if (akArgs.Length > 0)
		WorkshopObjectScript newObject = akArgs[0] as WorkshopObjectScript
		WorkshopScript workshopRef = akArgs[1] as WorkshopScript

		if workshopRef && workshopRef == theWorkshop
			UpdateWorkshopObjectivesSpecific(theQuest, workshopObjectives, theWorkshop)
		endif
	endif
endFunction

; call this function if you've already checked the event data or just want to initialize/update the objectives directly
function UpdateWorkshopObjectivesSpecific(Quest theQuest, WorkshopObjective[] workshopObjectives, WorkshopScript theWorkshop)
	debug.trace(" UpdateWorkshopObjectivesSpecific from " + theWorkshop)
	; wait for recalc to finish
	theWorkshop.WaitForWorkshopResourceRecalc()
	
	; check for objectives being completed
	int i = 0
	while (i < WorkshopObjectives.Length)
		WorkshopObjective theObjective = WorkshopObjectives[i]
		if theQuest.GetStageDone(theObjective.startStage) && (!theQuest.GetStageDone(theObjective.doneStage) || theObjective.rollbackObjective)
			debug.trace(theQuest + " valid objective: " + theObjective)
				float currentRating = 0
				if theObjective.useBaseValue
					currentRating = theWorkshop.GetBaseValue(WorkshopRatings[theObjective.ratingIndex].resourceValue)
				else
					currentRating = theWorkshop.GetValue(WorkshopRatings[theObjective.ratingIndex].resourceValue)
				endif
				debug.trace(theQuest + " rating " + WorkshopRatings[theObjective.ratingIndex].resourceValue + ": currentRating=" + currentRating)
				if theObjective.currentCount
					; update objective count if the current rating has increased by at least 1
					float objectiveCount = theObjective.currentCount.GetValue()
					int diff = Math.Floor(currentRating - objectiveCount)
					if diff != 0
						; get % complete - if there's a startingCount, reduce both current and max by that amount
						float percentComplete = ((currentRating  - theObjective.startingCount)/(theObjective.maxCount.GetValue() - theObjective.startingCount)) * 100
						percentComplete = math.min(percentComplete, 100)
						debug.trace(self + " currentRating=" + currentRating + ", maxCount=" + theObjective.maxCount.GetValue() + ", percentComplete=" + percentComplete)
						theObjective.percentComplete.SetValue(percentComplete)
						theQuest.UpdateCurrentInstanceGlobal(theObjective.percentComplete)
						if theQuest.ModObjectiveGlobal(afModValue = diff, aModGlobal = theObjective.currentCount, aiObjectiveID = theObjective.index, afTargetValue = theObjective.maxCount.GetValue(), abAllowRollbackObjective = theObjective.rollbackObjective)
							theQuest.setStage(theObjective.doneStage)
						endif
					endif
				else
					; just check if rating is positive
					if currentRating > 0
						theQuest.setStage(theObjective.doneStage)
					endif
				endif
		endif
		i += 1
	endwhile
endFunction


; returns rating for specified workshop and ratingIndex
float function GetRating(WorkshopScript workshopRef, int ratingIndex)
	float rating = workshopRef.GetValue(WorkshopRatings[ratingIndex].resourceValue)
	return rating
endFunction

; call this to randomize ransom value
function RandomizeRansom(GlobalVariable randomGlobal)
	int randomRansom = utility.randomInt(WorkshopMinRansom.GetValueInt(), WorkshopMaxRansom.GetValueInt())
	wsTrace(self + " RandomizeRansom: " + randomRansom)
	; round to closest 50
	float randomRounded = randomRansom/50 + 0.5
	randomRansom = math.floor(randomRounded) * 50
	wsTrace(self + " 	rounded to: " + randomRansom)
	randomGlobal.SetValue(randomRansom)
endFunction

; returns true if the passed in reference is in a "friendly" location (meaning the buildable area of a friendly settlement)
; * population > 0
; * workshop settlement
; * 1.5 not type02 settlement
bool function IsFriendlyLocation(ObjectReference targetRef)
	Location locationToCheck = targetRef.GetCurrentLocation()
	;debug.trace(self + " IsFriendlyLocation: targetRef=" + targetRef + ", locationToCheck=" + locationToCheck)
	if locationToCheck == NONE
		;debug.trace(self + " IsFriendlyLocation: FALSE")
		return false
	else
		WorkshopScript workshopRef = GetWorkshopFromLocation(locationToCheck)
		if workshopRef && workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) > 0 && targetRef.IsWithinBuildableArea(workshopRef) && ( workshopRef.HasKeyword(WorkshopType02) == false || workshopRef.OwnedByPlayer )
			;debug.trace(self + " IsFriendlyLocation: TRUE")
			return true
		else
			;debug.trace(self + " IsFriendlyLocation: FALSE")
			return false
		endif
	endif
endFunction

; utility functions to change the happiness modifier
; special handling: these all check for change in player ownership
; TODO - if we end up with multiple ownership, need to clear that here

float happinessModifierMax = 40.0 const
float happinessModifierMin = -50.0 const

function ModifyHappinessModifierAllWorkshops(float modValue, bool bPlayerOwnedOnly = true)
	; go through all workshops
	int index = 0
	while index < Workshops.Length
		WorkshopScript workshopRef = Workshops[index]
		; only ones with population matter
		if workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) > 0
			; player owned if specified
			if (!bPlayerOwnedOnly || (bPlayerOwnedOnly && workshopRef.GetValue(WorkshopPlayerOwnership) > 0))
				ModifyHappinessModifier(workshopRef, modValue)
			endif
		endif
		index += 1
	endWhile

endFunction

function ModifyHappinessModifier(WorkshopScript workshopRef, float modValue)
	if workshopRef
		wsTrace(self + "ModifyHappinessModifier " + modValue)
		float currentValue = workshopRef.GetValue(WorkshopRatingValues[WorkshopRatingHappinessModifier])
		float targetHappiness = workshopRef.GetValue(WorkshopRatingValues[WorkshopRatingHappinessTarget])
		float newValue = currentValue + modValue
		
		; don't modify past max/min limits so this doesn't overwhelm the base happiness value
		newValue = Math.Min(newValue, happinessModifierMax)
		newValue = Math.Max(newValue, happinessModifierMin)

		wsTrace(self + "	currentValue=" + currentValue + ", newValue=" + newValue + ", targetHappiness=" + targetHappiness)

		SetResourceData(WorkshopRatingValues[WorkshopRatingHappinessModifier], workshopRef, newValue)

		; recalc mod value to get the actual delta
		modValue = newValue - currentValue
		; if delta would reduce target to <= 0, end player ownership if it exists (and population is > 0)
		int population = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulation].resourceValue) as int
		int robots = workshopRef.GetBaseValue(WorkshopRatings[WorkshopRatingPopulationRobots].resourceValue) as int
		wsTrace("	ModifyHappinessModifier " + workshopRef + " population=" + (population - robots))

		if ( targetHappiness + modValue ) <= 0 && workshopRef.OwnedByPlayer && (population - robots) > 0 && workshopRef.AllowUnownedFromLowHappiness
			workshopRef.SetOwnedByPlayer(false)
		endif
	endif
endFunction

function SetHappinessModifier(WorkshopScript workshopRef, float newValue)
	if workshopRef
		wsTrace(self + "SetHappinessModifier " + newValue)
		float currentValue = workshopRef.GetValue(WorkshopRatingValues[WorkshopRatingHappinessModifier])
		float modValue = newValue - currentValue
		wsTrace(self + "	currentValue=" + currentValue + ", modValue=" + modValue)
		ModifyHappinessModifier(workshopRef, modValue)
	endif
endFunction

; utility function to display a message with text replacement from an object reference name
function DisplayMessage(Message messageToDisplay, ObjectReference refToInsert = NONE, Location locationToInsert = NONE)
	wsTrace("DisplayMessage " + refToInsert + ", " + locationToInsert)
	; insert ref into message alias - TODO - add more params as needed
	if refToInsert
		MessageRefAlias.ForceRefTo(refToInsert)
	endif

	if locationToInsert
		MessageLocationAlias.ForceLocationTo(locationToInsert)
	endif
	; display message
	messageToDisplay.Show()

	; clear aliases
	MessageRefAlias.Clear()
	MessageLocationAlias.Clear()
endFunction

function PlayerComment(WorkshopObjectScript targetObject)
	wstrace("PlayerComment " + targetObject)
	; only if at owned workshop
	WorkshopScript workshopRef = GetWorkshop(targetObject.workshopID)
	if workshopRef && workshopRef.OwnedByPlayer
		if WorkshopPlayerCommentScene.IsPlaying() == false
			PlayerCommentTarget.ForceRefTo(targetObject)
			WorkshopPlayerCommentScene.Start()
		endif
	endif
endFunction


;added by jduvall
function ToggleOnAllWorkshops()
	int i = 0
	while (i < WorkshopsCollection.GetCount() - 1)
		(WorkshopsCollection.GetAt(i) as WorkshopScript).SetOwnedByPlayer(true)
		i += 1
	endwhile

	PlayerOwnsAWorkshop = true
endFunction

bool Function PermanentActorsAliveAndPresent(WorkshopScript workshopRef)
	int i = 0
	int iCount = PermanentActorAliases.GetCount()

	;If there are permanent actors...
	if iCount > 0

		int iClearedWorkshopID = workshopRef.GetWorkshopID()

		;Then loop through all the permanent actors and get their workshop ID...
		while i < iCount
			Actor act = (PermanentActorAliases.GetAt(i) as Actor)
			wsTrace("PermanentActorsAliveAndPresent: Checking permanent actor: " + act)
			int iActorWorkshopID = (act as WorkshopNPCScript).GetWorkshopID()

			;If the selected Permanent Actor is assigned to a workshop location and isn't dead...
			if iActorWorkshopID > -1 && !act.IsDead()
				wsTrace("PermanentActorsAliveAndPresent: Comparing actor's workshop ID: " + iActorWorkshopID + "  and cleared workshop ID: " + iClearedWorkshopID)

				;And they're assigned to the cleared location, return true
				if iActorWorkshopID == iClearedWorkshopID
					return true
				endif

			endif

			i += 1
		endwhile
	endif

	return false
endFunction