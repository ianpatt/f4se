Scriptname FollowersScript extends Quest Conditional
{Script for handling follower and companion systems:
FOLLOWERS, COMPANIONS, COMMANDS
LOITERING
COMPANION AFFINITY
SITUATION AWARENESS
}

;/
This script is broken into sections for each "system" -
	each with its own properties, variables, and functions defined under the section heading:
		*** FOLLOWERS, COMPANIONS, COMMANDS
		*** LOITERING
		*** COMPANION AFFINITY
		*** SITUATION AWARENESS

Events and Remote Events however are kept together at top, as events are used by multiple systems.

Questions/concerns/changes:
	--speak with jduvall
/;

;*******************************************************************************************************
;**************************************     STATES    **************************************************
;*******************************************************************************************************

string HandlingOnHit = "HandlingOnHit"

State HandlingOnHit
	Event OnHit(ObjectReference RemoteSource, ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked, string asMaterialName)
	EndEvent
EndState

;*******************************************************************************************************
;**************************************     EVENTS    **************************************************
;*******************************************************************************************************


;**************************************     OnInit()   **************************************************

Quest Property Tutorial Auto Const
Quest Property Achievements Auto Const

WorkshopParentScript Property WorkshopParent Auto

LocationAlias Property DismissMessageLocation const auto

Message Property FollowersCompanionDismissMessage const auto
Message Property FollowersDogmeatCompanionDismissMessage const auto
Message Property FollowersDogmeatCompanionDismissMessageNameUnknown const auto
GlobalVariable Property PlayerKnowsDogmeatName const auto
keyword Property workshopItemKeyword const auto

Event OnInit()
	
playerRef = game.GetPlayer() ;used throughout

	;**** used by "LOITERING" *****
	LoiterBufferSize
	PlayerPosX = New float[LoiterBufferSize]
	PlayerPosY = New float[LoiterBufferSize]
	PlayerPosZ = New float[LoiterBufferSize]

	InitializeLoiterBuffer()	;to make game boot instant debugging easier, not really a practical in game need

	startTimer(TimerInterval_LoiterSample, iTimerID_LoiterSample)
	;******************************

	;**** used by Autonomy ****
	AutonomyDisallowedByObjects = new ScriptObject[0]


	RegisterForRemoteEvent(playerRef, "OnLocationChange") ; used by: "SITUATION AWARENESS"
	RegisterForRemoteEvent(playerRef, "OnCombatStateChanged") ; used by: "SITUATION AWARENESS"

	RegisterForHitEvent(playerRef) 	; used by: 	"SITUATION AWARENESSS"
												; 			& "FOLLOWER, COMPANION, COMMANDS"

	RegisterForRemoteEvent(playerRef, "OnPlayerFireWeapon") ;used by the Affinity Event system
	RegisterForRemoteEvent(playerRef, "OnPlayerFallLongDistance") ;used by the Affinity Event system

	RegisterForRemoteEvent(playerRef, "OnPlayerHealTeammate") ;used by the Affinity Event system


	RegisterForRemoteEvent(playerRef, "OnCripple") ;used by the Affinity Event system
	RegisterForRemoteEvent(Companion, "OnCripple") ;used by the Affinity Event system

	RegisterForRemoteEvent(PlayerRef, "OnItemEquipped") ;used by the Affinity Event system (for getting in Power Armor)

	RegisterForRemoteEvent(PlayerRef, "OnPlayerEnterVertibird") ;used by the Affinity Event system and to start scene to have companion get in
	RegisterForRemoteEvent(PlayerRef, "OnPlayerUseWorkBench") ;used by the Affinity Event system
	RegisterForRemoteEvent(PlayerRef, "OnPlayerSwimming") ;used by the Affinity Event system
	RegisterForRadiationDamageEvent(PlayerRef) ;used by the Affinity Event system
	RegisterForRemoteEvent(PlayerRef, "OnPlayerModArmorWeapon") ;used by the Affinity Event system

	RegisterForRemoteEvent(DogmeatCompanion, "OnEnterBleedout") ;used by the Affinity Event system

	RegisterForPlayerSleep() ;used to add the perk for sleeping near companions that are infatuated


	RegisterForRemoteEvent(Companion, "OnCommandModeGiveCommand") ;used to start quests for specific behavior
	RegisterForRemoteEvent(DogmeatCompanion, "OnCommandModeGiveCommand") ;used to change his archetype based on commands given
	RegisterForRemoteEvent(DogmeatCompanion, "OnCommandModeCompleteCommand") ;used to change his archetype based on commands given

	RegisterForMagicEffectApplyEvent(PlayerRef, akCasterFilter = none, akEffectFilter = CA_AddictionEffect)

	RegisterForPlayerTeleport() 

	trace(self, "OnInit() finished.")

EndEvent


;**************************************     TIMERS   **************************************************

int iTimerID_LoiterSample = 1 const ;TimerID
;NOTE: Timer Interval is TimerInterval_LoiterSample

int iTimerID_GameTime_SAECombatantExpiry = 3 const	;timer id
float TimerInterval_SAECombatantExpiry = 0.25 const			;in gametime hours


int iTimerID_GameTime_SAC_LastCombat = 4 const 	;timer id
float TimerInterval_SAC_LastCombat = 0.25 const ;in gametime hours -- NOTE: this should be smaller than the smallest change between numbers in SAC_LastCombat_Times


Event OnTimer(int aiTimerID)
	if aiTimerID == iTimerID_LoiterSample
		HandleTimer_LoiterSample()
	endif
EndEvent

Event OnTimerGameTime(int aiTimerID)
	if aiTimerID == iTimerID_GameTime_SAECombatantExpiry
		HandleTimer_GameTime_SAECombatantExpiry()

	elseif aiTimerID == iTimerID_GameTime_SAC_LastCombat
		Handletimer_GameTime_SAC_LastCombat()

	endif

EndEvent


;*******************************************************************************************************
;**************************************  REGISTERED REMOTE EVENTS **************************************
;*******************************************************************************************************

bool DebugTrace_OnHit = false
bool DebugTrace_OnCombatStateChanged = false


Event OnMagicEffectApply(ObjectReference akTarget, ObjectReference akCaster, MagicEffect akEffect)
	if akTarget == PlayerRef && akEffect == CA_AddictionEffect
		followersScript.SendAffinityEvent(self, AddictionKeyword)

	endif

EndEvent

Event Actor.OnLocationChange(Actor akSender, Location akOldLoc, Location akNewLoc)
	if akSender == playerRef
		trace(self, "Player OnLocationChange()")
		StartCompanionInteractionScene(CIS_Type_EnterNewLocation, Playerref, akNewLoc)
		SetSituationAwarenessBasedOnLocation(akNewLoc, SetSASData = true, SetSAEClutterData= false, SettingObject = akNewLoc)
	endif
EndEvent


Event OnHit(ObjectReference RemoteSource, ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked, string asMaterialName)

	GoToState(HandlingOnHit)
    
	if DebugTrace_OnHit
    	debug.traceFunction()
    endif

    Actor CompanionActor = Companion.GetActorReference()

    if RemoteSource == CompanionActor || RemoteSource == playerRef
		HandleCombatMessageToSituationAwareness(akAggressor as Actor) ;ultimately causes SitationalAwareness variables to update based on the actor we're in combat with
    endif

	GoToState(None)

	; listen for the next hit against the player or companion
	RegisterForHitEvent(playerRef)
	RegisterForHitEvent(Companion) ; may want to track if we are currently listening for companion hits and, if so, not re-register

EndEvent

Event Actor.OnCombatStateChanged(Actor RemoteSource, Actor akTarget, int aeCombatState)

	trace(self, "Actor.OnCombatStateChanged() RemoteSource: " + RemoteSource + ", akTarget:" + akTarget + ", aeCombatState:" + aeCombatState)

	if DebugTrace_OnCombatStateChanged
    	debug.traceFunction()
	endif

	actor CompanionActor = Companion.GetActorReference()

    if RemoteSource == CompanionActor || RemoteSource == playerRef
		if akTarget
			HandleCombatMessageToSituationAwareness(akTarget as Actor) ;ultimately causes SitationalAwareness variables to update based on the actor we're in combat with
		endif
    endif

EndEvent

Event Actor.OnPlayerFireWeapon(Actor akSender, Form akBaseObject)
	trace(self, "OnPlayerFireWeapon() akBaseObject:" + akBaseObject)

	float radiusToLookForActors = 3000

	;trace(self, "OnPlayerFireWeapon")

	;check location keywords for settelements
	;find all ActorTypeNPC refs nearby, see if they are all non-hostile
	;then send the dischargeweapon event

	location locToTest = akSender.GetCurrentLocation()

	if akBaseObject && akBaseObject.HasKeyword(Game.GetCommonProperties().WeaponTypeMelee1H) == false && akBaseObject.HasKeyword(Game.GetCommonProperties().WeaponTypeMelee2H) == false && akBaseObject.HasKeyword(Game.GetCommonProperties().WeaponTypeUnarmed) == false 

		if locToTest.HasKeyword(Game.GetCommonProperties().LocTypeSettlement) || locToTest.HasKeyword(Game.GetCommonProperties().LocTypeWorkshopSettlement) 
			;trace(self, "OnPlayerFireWeapon - player in settlement area")
		
			ObjectReference[] NPCs = akSender.FindAllReferencesWithKeyword(Game.GetCommonProperties().ActorTypeNPC, radiusToLookForActors) 

			bool AnyHostiles = CommonArrayFunctions.IsActorInArrayHostileToActor(akSender, NPCs)

			if AnyHostiles == false
				trace(self, "OnPlayerFireWeapon - no hostiles in the area, will send the CA_DischargeWeapon event")

				SendAffinityEvent(self,  DischargeWeapon)

			endif

		endif

	endif


endEvent

Event Actor.OnPlayerFallLongDistance(Actor akSender, float afDamage)
	trace(self, "OnPlayerFallLongDistance")
	SendAffinityEvent(self, JumpFromHeight)

endEvent

Event Actor.OnPlayerHealTeammate(Actor akSender, Actor akTeammate)
	trace(self, "OnPlayerHealTeammate akSender:" + akSender + " akTeammate:" + akTeammate)

	if akTeammate == Companion.GetActorReference()
		SendAffinityEvent(self, HealCompanion)
	elseif akTeammate == DogmeatCompanion.GetActorReference()
		SendAffinityEvent(self, HealDogmeat)
	endif
	
EndEvent


Event Actor.OnCripple(Actor akSender, ActorValue akActorValue, bool abCrippled)
	HandleOnCrippleEvent(akSender, akActorValue, abCrippled)
EndEvent

Event ReferenceAlias.OnCripple(ReferenceAlias akSender, ActorValue akActorValue, bool abCrippled)
	HandleOnCrippleEvent(akSender, akActorValue, abCrippled)
EndEvent

Event ReferenceAlias.OnEnterBleedout(ReferenceAlias akSender) 
	if akSender == DogmeatCompanion
		FollowersScript.SendAffinityEvent(self, DogmeatBleedoutKeyword)
	endif
endEvent

Event Actor.OnPlayerEnterVertibird(Actor askSender, ObjectReference akVertibird)
	trace(self, "OnPlayerEnterVertibird() " + akVertibird)
	LastPlayerVertibird.ForceRefTo(akVertibird)
	HandleVertibirdEvent(akVertibird)
EndEvent

Event Actor.OnPlayerUseWorkBench(Actor akSender, ObjectReference akWorkBench)
	trace(self, "OnPlayerUseWorkBench()" + akWorkBench)
	HandleOnWorkbench(akWorkBench)

EndEvent

Event Actor.OnPlayerSwimming(Actor akSender)
	trace(self, "OnPlayerSwimming()" + Game.GetPlayer().GetCurrentLocation())
	HandleOnSwimming(Game.GetPlayer().GetCurrentLocation())

EndEvent

Event OnRadiationDamage(ObjectReference akTarget, bool abIngested)
	trace(self, "OnRadiationDamage()")
	
	if Game.GetPlayer().IsInCombat() == false && abIngested == false
		HandleOnRadiation(akTarget.GetCurrentLocation())
	endif
	
	RegisterForRadiationDamageEvent(PlayerRef) ; Re-register so we get the next event that's sent
endEvent

Event Actor.OnPlayerModArmorWeapon(Actor akSender, Form akBaseObject, ObjectMod akModBaseObject)
	trace(self, "OnPlayerModArmorWeapon() akBaseObject:" + akBaseObject + ", akModBaseObject: " + akModBaseObject)
	HandleOnMod(akBaseObject)

endEvent


Event Actor.OnItemEquipped(Actor akSender, Form akBaseObject, ObjectReference akReference)
	trace(self, "OnItemEquipped()" + akSender + ", " + akBaseObject + ", " + akReference)
	HandleOnItemEquipped(akSender, akBaseObject)
EndEvent

Event OnPlayerSleepStart(float afSleepStartTime, float afDesiredSleepEndTime, ObjectReference akBed)
	trace(self, "OnSleepStart akBed:" + akBed)
	HandleOnSleepStart(akBed)	
EndEvent

Event OnPlayerSleepStop(bool abInterrupted, ObjectReference akBed)
	HandleOnSleepEnd()	
EndEvent

Event OnPlayerTeleport()
	trace(self, "OnPlayerTeleport, calling AllowAutonomyOnTeleport()")
	AllowAutonomyOnTeleport()

EndEvent


;Companion command interface commands
Event ReferenceAlias.OnCommandModeGiveCommand(ReferenceAlias akSender, int aeCommandType, ObjectReference akTarget)
;	aeCommandType: Type of Command that is given, which is one of the following:
;
;    0 - None
;    1 - Call
;    2 - Follow
;    3 - Move
;    4 - Attack
;    5 - Inspect
;    6 - Retrieve
;    7 - Stay
;    8 - Release
;    9 - Heal 

	trace(self, "OnCommandModeGiveCommand()" + akSender + aeCommandType + akTarget)

	if akSender == Companion
		
		if (akTarget.GetBaseObject() is Container || akTarget.GetBaseObject() is Door) && akTarget.IsLocked() && akSender.GetActorReference().HasKeyword(Followers_Command_LockPick_Allowed)
			akSender.GetActorReference().EvaluatePackage(abResetAI=true) ;cancels the stay command
			Command_LockPick_Scene.stop()
			CommandActor.ForceRefTo(akSender.GetReference())
			CommandTarget.ForceRefTo(akTarget)
			Command_LockPick_Scene.start()

		elseif akTarget.GetBaseObject() is Terminal && akTarget.IsLocked() && akSender.GetActorReference().HasKeyword(Followers_Command_HackTerminal_Allowed)
			akSender.GetActorReference().EvaluatePackage(abResetAI=true) ;cancels the stay command
			Command_HackTerminal_Scene.stop()
			CommandActor.ForceRefTo(akSender.GetReference())
			CommandTarget.ForceRefTo(akTarget)
			Command_HackTerminal_Scene.start()

		endif


	elseif akSender == DogmeatCompanion && aeCommandType == 6
		DogmeatIdles.BarkPlayful()
		DogmeatIdles.SetDogmeatAlert()
	endif

EndEvent

;Companion command interface commands
Event ReferenceAlias.OnCommandModeCompleteCommand(ReferenceAlias  akSender, int aeCommandType, ObjectReference akTarget)
;	aeCommandType: Type of Command that is given, which is one of the following:
;
;    0 - None
;    1 - Call
;    2 - Follow
;    3 - Move
;    4 - Attack
;    5 - Inspect
;    6 - Retrieve
;    7 - Stay
;    8 - Release
;    9 - Heal 

	trace(self, "OnCommandModeCompleteCommand()" + akSender + aeCommandType + akTarget)

	if akSender == DogmeatCompanion && aeCommandType == 6 && !game.getplayer().isSneaking()
		; if Dogmeat has just completed a fetch, get happy!  Unless, of course, we're being sneaky.
		
		DogmeatIdles.BarkPlayful()
		DogmeatIdles.SetDogmeatPlayful(15)
		DogmeatIdles.FaceHappy()

	endif

EndEvent



;*******************************************************************************************************
;************************************** GENERIC FUNCTIONS ********************************
;*******************************************************************************************************

FollowersScript Function GetScript() global ;used for making functions global
	return (Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as Quest) as FollowersScript ;this script attached to Followers quest
EndFunction

bool Function Trace(ScriptObject CallingObject, string asTextToPrint, int aiSeverity = 0) global debugOnly
	;we are sending callingObject so we can in the future route traces to different logs based on who is calling the function
	string logName = "Followers"
	debug.OpenUserLog(logName) 
	RETURN debug.TraceUser(logName, CallingObject + ": " + asTextToPrint, aiSeverity)
	
EndFunction


bool Function TraceConditional(ScriptObject CallingObject, string asTextToPrint, bool ShowTrace, int aiSeverity = 0) global debugOnly
	;we are sending callingObject so we can in the future route traces to different logs based on who is calling the function
	string logName = "Followers"

	if ShowTrace
		debug.OpenUserLog(logName) 
		RETURN debug.TraceUser(logName, CallingObject + ": " + asTextToPrint, aiSeverity)
	else 
		return false
	endif
	
EndFunction



;*******************************************************************************************************
;************************************** FOLLOWERS, COMPANIONS, COMMANDS ********************************
;*******************************************************************************************************

;/
	Handles various Follower / Companion behavior.
	Much of which is setting variables used by FollowPlayer template based packages.

	Functions are exposed that can be called on any actor following the player.
	There is also a temporary messagebox menu interface for setting commands.

	See also:
		PACKAGES
			- FollowPlayer Template
				Sandboxes at "Go Home Sandbox Location" package data if:
					A) AllFollowerState var == iFollower_Com_GoHome, OR
					B) ActorValue == iFollower_Com_GoHome

				Travels to self (ie "Waits")  if:
					A) AllFollowerState var = iFollower_Com_Wait, OR
					B) ActorValue == iFollower_Com_Wait

				Sandboxes around player if Loitering (see LOITERING section for details)

				If not sandboxing:
					Flees if player gets to close.
					Follows closely if: ActorValue == iFollowerDistNear
					Follows normally if: ActorValue == iFollowerDistMedium
					Follows distantly if: ActorValue == iFollowerDistFar

/;



ReferenceAlias Property Companion Auto Const
ReferenceAlias Property DogmeatCompanion Auto Const
RefCollectionAlias Property ActiveCompanions const auto

ReferenceAlias Property AvailableMessageActor const auto
Message Property FollowersCompanionAvailableMessage const auto

ReferenceAlias Property SleepCompanion Auto Const
ReferenceAlias Property SleepCompanionBed Auto Const
keyword Property FollowersCompanionSleepNearPlayerFlag const auto

Faction property CurrentCompanionFaction Auto Const
Faction property HasBeenCompanionFaction Auto Const
Faction property DisallowedCompanionFaction Auto Const


ReferenceAlias Property CommandActor const auto
ReferenceAlias Property CommandTarget const auto

GlobalVariable Property PlayerHasActiveCompanion const auto
GlobalVariable Property PlayerHasActiveDogmeatCompanion const auto

ReferenceAlias Property LastPlayerVertibird Auto Const


Group CommandLockHackProperties
keyword Property Followers_Command_LockPick_Allowed const auto
{autofill - used to start the lockpick scene if you command an actor to inspect a locked door or container}

scene Property Command_LockPick_Scene const auto
{autofill - scene used for commanded lock pick scenes}

keyword Property Followers_Command_HackTerminal_Allowed const auto
{autofill - used to start the hack terminal  scene if you command an actor to inspect a locked terminal}

scene Property Command_HackTerminal_Scene const auto
{autofill - scene used for commanded hack terminal scenes}

actorvalue Property ValentineFailedToHack const auto ;actor value to put on TERMINAL when Valentine fails to hack it, so he won't try again.
int property CommandUnlockAttempts auto hidden conditional ;how many attempts to cait make to pick a lock?
bool  Property CommandUnlockSuccess auto hidden conditional
MiscObject Property BobbyPin const auto
Sound Property NPCHumanLockpickingPickBreak const auto
Sound Property NPCHumanLockpickingUnlock const auto
idle Property IdleLockPickingLowHeight const auto
idle Property IdleLockPickingMediumHeight const auto
idle Property IdleStop const auto
sound Property NPCHumanHackingPasswordBad const auto
sound Property NPCHumanHackingPasswordGood const auto
EndGroup

Function CommandUnlockStartNewAttempt()
	debug.trace(self + "CommandUnlockStartNewAttempt()")
	CommandUnlockSuccess = false
	CommandUnlockAttempts = 0
EndFunction

Function CommandUnlockPlayAnim()
	debug.trace(self + "CommandUnlockPlayAnim()")
	actor source = CommandActor.GetActorReference()
	objectReference target = CommandTarget.GetReference()

	;TO DO -- check if door, if so play medium, if not, check z offset
	if target.GetBaseObject() is Door
		source.playidle(IdleLockPickingMediumHeight)
	else
		float zOffset = target.Z - source.Z
		trace(self, "CommandUnlockPlayAnim() zOffset" + zOffset)

		if zOffset > 50
			source.playidle(IdleLockPickingMediumHeight)
		else
			source.playidle(IdleLockPickingLowHeight)
		endif
		
	endif
EndFunction

Function CommandUnlockAttempt()
	debug.trace(self + "CommandUnlockAttempt()")
	objectReference source = CommandActor.GetReference()
	objectReference target = CommandTarget.GetReference()
	int lockLevel = target.GetLockLevel()
	debug.trace(self + "lockLevel: " + lockLevel)

	int roll = Utility.RandomInt(0, 110)
	debug.trace(self + "roll: " + roll)

	CommandUnlockSuccess = roll > lockLevel
	debug.trace(self + "CommandUnlockSuccess: " + CommandUnlockSuccess)


	if CommandUnlockSuccess
		target.unlock()

		if target.GetBaseObject() is Terminal
			NPCHumanHackingPasswordGood.play(source)
		else
			NPCHumanLockpickingUnlock.play(source)	
		endif

	else
		if target.GetBaseObject() is Terminal
			NPCHumanHackingPasswordBad.play(source)
		else
			NPCHumanLockpickingPickBreak.play(source)	
			CommandActor.GetActorReference().RemoveItem(BobbyPin)
		endif
		
	endif

	CommandUnlockAttempts += 1	

EndFunction

Function CommandUnlockFailedHack()
	debug.trace(self + "CommandUnlockFailedHack()")
	objectReference target = CommandTarget.GetReference()
	target.setvalue(ValentineFailedToHack, 1)
EndFunction


int Property AllFollowerState = 1 auto conditional hidden
;/
iFollower_Com_Follow -- "allowed to follow" individually determined by each actor's ActorValue
iFollower_Com_Wait
FollowersSayCommandGoHome
/;


;holds cached values for these actor values when the actor becomes a companion, so we can change them and set them back
float Property CachedIdleChatterTimeMin hidden auto
float Property CachedIdleChatterTimeMax hidden auto

float CompanionIdleChatterTimeMin = 240.0
float CompanionIdleChatterTimeMax = 300.0


;************************************** COMMAND PROPERTY DATA   **************************************************

;/

Commands are made up of three parts (which should be STRUCTS in future implementations of Papyrus)
1) String Property AV_<CommandName>
	- a string which is used as a switch for which command data to set
	- in part this is because when implementing I didn't have appropriate actorvalues and had to map to other actorvalues (see TTP#3505)
	-*** IF ANY OF THE STRINGS SHOULD CHANGE BECAUSE THE ACTOR VALUE NAMES CHANGE  (which they will when TTP#3505))
	-*** YOU MUST CHANGE THE CONDITIONS IN FollowPlayer Template tree!!!
2) GlobalVariable Property iFollower_Com_<CommandName>
	- a GlobalVariable whose value acts like an Enum and is used to make conditions in packages human readable and more flexible
3) Topic Property FollowersSayCommand<CommandName>
	- a Topic that the follower will say when being switched into this command

Some commands have additional GlobalVariables for settings, such as:
-follow distance: near, medium, far
-stance: aggressive, defensive

/;

;THESE ARE USED BY FollowPlayer package template (and possibly others) in condition functions. Changes here will have ramifications in packages. 
;Not sure if these are going to their own actor values or what, so setting things up as string/value pairs will hopefully make it less painful to change later if needed

Group FollowerControl Collapsed

Message Property FollowersCompanionMenuMain const auto
Message Property FollowersCompanionMenuTactics const auto
Message Property FollowersCompanionMenuWait const auto
Message Property FollowersCompanionMenuFollow const auto

;NOTE: some of these actor values are remapping old ones that will be renamed. (Ex: "Fame" will be renamed "Follow")

;Follow State:
GlobalVariable Property iFollower_Com_Follow const auto
GlobalVariable Property iFollower_Com_Wait const auto
GlobalVariable Property iFollower_Com_GoHome const auto
Topic Property FollowersSayCommandFollow const auto
Topic Property FollowersSayCommandWait const auto
Topic Property FollowersSayCommandGoHome const auto

;Follow Distance:
GlobalVariable Property iFollower_Dist_Near const auto
GlobalVariable Property iFollower_Dist_Medium const auto
GlobalVariable Property iFollower_Dist_Far const auto
Topic Property FollowersSayCommandDistanceNear const auto
Topic Property FollowersSayCommandDistanceMedium const auto
Topic Property FollowersSayCommandDistanceFar const auto


;Stance:
GlobalVariable Property iFollower_Stance_Aggressive const auto
GlobalVariable Property iFollower_Stance_Defensive const auto
Topic Property FollowersSayCommandStanceAggressive const auto
Topic Property FollowersSayCommandStanceDefensive const auto

;Trade:
Topic Property FollowersSayCommandTrade const auto

;Favor:
Topic Property FollowersSayCommandFavor const auto

;Stance Combat Override (this is short lived flag for defensive stance companions that turns on/off their combat override package)

GlobalVariable Property iFollower_Stance_CombatOverride_True const auto
GlobalVariable Property iFollower_Stance_CombatOverride_False const auto

;"Aggression" actorValue Reminder:
;0 = Unaggressive 		Will not initiate combat.
;1 = Aggressive 		Will attack Enemies on sight.
;2 = Very Aggressive 	Will attack Enemies and Neutrals on sight.
;3 = Frenzied 			Will attack anyone on sight. Actors are rarely set to Frenzied by default; this is usually the result of some other spell or effect (like the Frenzy spell). 

Endgroup

CustomEvent CompanionChange ;register for this to listen for companion changes


;### Status Functions ###
bool Function IsCompanion(Actor ActorToCheck)
	return Companion.GetActorReference() == ActorToCheck
EndFunction

bool Function IsPossibleCompanion(Actor ActorToCheck)
	return ActorToCheck as CompanionActorScript
EndFunction

bool Function IsFollowing(Actor ActorToCheck)
	return ActorToCheck.GetValue(Game.GetCommonProperties().FollowerState) == iFollower_Com_Follow.value
EndFunction

bool Function IsCompanionAndFollowing(Actor ActorToCheck)
	return IsCompanion(ActorToCheck) && IsFollowing(ActorToCheck)
EndFunction

Function TracePlayerFollowers() global
	debug.messagebox(Game.GetPlayerFollowers())
	trace(GetScript(), "TracePlayerFollowers(): " + Game.GetPlayerFollowers())

EndFunction

;### Companion Change Event ###
Function SendCompanionChangeEvent(Actor ActorThatChanged, bool IsNowCompanion)

	Var[] args = new Var[2]

	args[0] = ActorThatChanged
	args[1] = IsNowCompanion

	trace(self, "SendCompanionChangeEvent()" + args)

	sendCustomEvent("CompanionChange", args)
EndFunction

Function SetAvailableToBeCompanion(Actor ActorToMakeAvailable)
	ActorToMakeAvailable.AddToFaction(HasBeenCompanionFaction)

	if ActorToMakeAvailable.IsInFaction(CurrentCompanionFaction) == false
		AvailableMessageActor.ForceRefTo(ActorToMakeAvailable)
		FollowersCompanionAvailableMessage.show()
	endif

EndFunction

;************** NOTE ABOUT COMPANION AND DOGMEAT FUNCTIONS ******************
;Originally there were two slots for companion types, you could have both Dogmeat and a single human companion.
;That is why there are two aliases and two seperate functions.
;Now we are only letting you have either dogmeat or a single human companions.
;Rather than risk breaking everything by trying to unifying everything into a single alias/function call
;I am going to just put hooks in SetCompanion and SetDogmeatCompanion to auto dismiss the other

;### SetCompanion ####
;There is only ever one companion at a time
;This function handles "promoting" an actor to companion status, and "demoting" previously installed companions
;Much of the companion behavior and data is set up using the Companion alias.
;ALSO CALLED BY Actor.psc's SetCompanion() function

Actor[] AchievementCompanions ;add companions and dogmeat to this when hired for the first time, used to setstage on achievement quests
Function ManageAchievementCompanions(actor CompanionActor)

	if AchievementCompanions == None
		AchievementCompanions = New Actor[0]
	endif

	if AchievementCompanions.find(CompanionActor) < 0
		AchievementCompanions.add(CompanionActor)
		Achievements.setStage(320)
	endif
EndFunction

Function SetCompanion(Actor ActorToMakeCompanion, bool SetCompanion = true, bool FillCompanionAlias = true, bool SuppressDismissMessage = false)
		; Joel - Adding hook for Follower/Command Mode tutorial 
	if !tutorial.getStageDone(600)
		tutorial.setStage(600)
	endif

	Actor CurrentCompanionActor = Companion.GetActorReference() 

	if SetCompanion == false  
		if CurrentCompanionActor == ActorToMakeCompanion
			DismissCompanion(ActorToMakeCompanion, ShowLocationAssignmentListIfAvailable = true, SuppressDismissMessage = SuppressDismissMessage)
		endif

		RETURN
	endif

	;dismiss dogmeat - you can only have a single human companion or dogmeat
	DismissDogmeatCompanion(ShowLocationAssignmentListIfAvailable = true, SuppressDismissMessage = false)

	if FillCompanionAlias ;this is almost certainly going to be true

		;if we are changing companions, dismiss the previous companion so player get's a message knowing that's happened
		if CurrentCompanionActor && ActorToMakeCompanion.IsInFaction(CurrentCompanionFaction) == false
			DismissCompanion(Companion.GetActorReference(), ShowLocationAssignmentListIfAvailable = true)
		endif
	
		Companion.ForceRefTo(ActorToMakeCompanion)

		;if ActorToMakeCompanion isn't already in there, add him to the ActiveCompanions refcollection
		if ActiveCompanions.find(ActorToMakeCompanion) < 0
			ActiveCompanions.addRef(ActorToMakeCompanion)
		endif

	endif

	CompanionDataToggle(true, canDoFavor = true) ;canDoFavor is param that forces the no-dialogue command mode

	;hook for achievements
	ManageAchievementCompanions(ActorToMakeCompanion)

	ActorToMakeCompanion.addToFaction(HasBeenCompanionFaction) ;used primarily to conditionalize dialogue

	FollowerFollow(ActorToMakeCompanion)
	FollowerSetDistanceMedium(ActorToMakeCompanion)

	;used to handle companion in defensive stance responding to his or player's getting hit
	RegisterForHitEvent(playerRef)

	SendCompanionChangeEvent(ActorToMakeCompanion, IsNowCompanion = True)

	PlayerHasActiveCompanion.SetValue(1)

	CompanionActorScript CompanionActor = ActorToMakeCompanion as CompanionActorScript
	if CompanionActor.ShouldGivePlayerItems
		CompanionActor.StartHasItemTimer() 
	endif



EndFunction


Function DismissCompanion(Actor CompanionToDismiss, bool ShowLocationAssignmentListIfAvailable = true, bool SuppressDismissMessage = false)
	trace(self, "DismissCompanion()" + CompanionToDismiss)
	CompanionActorScript CAS = Companion.GetActorReference() as CompanionActorScript

	if CAS == CompanionToDismiss ;is actor we are trying to dismiss actually the current companion?
		
		if SuppressDismissMessage == false

			trace(self, "PlayerOwnsAWorkshop: " + WorkshopParent.PlayerOwnsAWorkshop)
			trace(self, "ShowLocationAssignmentListIfAvailable: " + ShowLocationAssignmentListIfAvailable)

			location WorkshopHome
			
			if ShowLocationAssignmentListIfAvailable
				; if AllowDismissSettlements exists, check that; otherwise check PlayerOwnsAWorkshop
				if ( (CAS.AllowDismissToSettlements == NONE && WorkshopParent.PlayerOwnsAWorkshop ) || ( CAS.AllowDismissToSettlements && CAS.AllowDismissToSettlements.GetValue() > 0 ) )
					if CAS.DismissCompanionSettlementKeywordList
						; use override version of settlement menu call:
						
						; need to get previous location for this call
						Location previousLocation = NONE
						int previousWorkshopID = (CompanionToDismiss as WorkshopNPCScript).GetWorkshopID()
						if previousWorkshopID > -1
							WorkshopScript previousWorkshop = WorkshopParent.GetWorkshop(previousWorkshopID)
							if previousWorkshop
								previousLocation = previousWorkshop.myLocation
							endif
						endif
						WorkshopHome = CompanionToDismiss.OpenWorkshopSettlementMenuEx(akActionKW=WorkshopParent.WorkshopAssignHomePermanentActor, aLocToHighlight=previousLocation, akIncludeKeywordList=CAS.DismissCompanionSettlementKeywordList)
					else 
						; use normal version
						WorkshopHome = WorkshopParent.AddPermanentActorToWorkshopPlayerChoice(Companion.GetActorReference())
					endif
				endif
			endif

			trace(self, "DismissCompanion() will show message")

			if WorkshopHome == None
				WorkshopHome = (CompanionToDismiss.GetLinkedRef(workshopItemKeyword) as WorkshopScript).myLocation
			endif

			trace(self, "DismissCompanion() WorkshopHome: " + WorkshopHome)
			if WorkshopHome
				CAS.HomeLocation = WorkshopHome
			endif

			DismissMessageLocation.ForceLocationTo(CAS.HomeLocation)
			FollowersCompanionDismissMessage.Show()
		
		endif

		trace(self, "DismissCompanion() will ClearCompanion()")
		ClearCompanion()

	else
		trace(self, CompanionToDismiss + " is NOT the current companion, so not dismissing.")

	endif
	
EndFunction

Function DismissDogmeatCompanion(bool ShowLocationAssignmentListIfAvailable = true, bool SuppressDismissMessage = false)

	trace(self, "DismissDogmeatCompanion()")
	DogmeatActorScript DAS = DogmeatCompanion.GetActorReference() as DogmeatActorScript

	trace(self, "DismissDogmeatCompanion() DAS = " + DAS)

	trace(self, "PlayerOwnsAWorkshop: " + WorkshopParent.PlayerOwnsAWorkshop)
	trace(self, "ShowLocationAssignmentListIfAvailable: " + ShowLocationAssignmentListIfAvailable)

	if DAS

		if SuppressDismissMessage == false
			
			location WorkshopHome

			if ShowLocationAssignmentListIfAvailable && WorkshopParent.PlayerOwnsAWorkshop
				WorkshopHome = WorkshopParent.AddPermanentActorToWorkshopPlayerChoice(DAS, bWaitForActorToBeAdded = false)

			endif

			trace(self, "DismissDogmeatCompanion() DAS.HomeLocation = " + DAS.HomeLocation)
			
			if WorkshopHome == None
				WorkshopHome = (DAS.GetLinkedRef(workshopItemKeyword) as WorkshopScript).myLocation
			endif

			if WorkshopHome
				DAS.HomeLocation = WorkshopHome
			endif

			DismissMessageLocation.ForceLocationTo(DAS.HomeLocation)

			;TEST FOR IF PLAYER KNOWS NAME
			if PlayerKnowsDogmeatName.GetValue() == 1
				FollowersDogmeatCompanionDismissMessage.Show()
			else
				FollowersDogmeatCompanionDismissMessageNameUnknown.Show()
			endif

		endif

		ClearDogmeatCompanion()

	endif

EndFunction

function SetDogmeatCompanion(Actor DogmeatRef = None)

	;NOTE: We allow dismissal of Dogmeat in the same way you dismiss any follower.(Oct 6 2014)
	;WE ASSUME THERE IS ONLY EVER ONE DOGMEAT THAT CAN BECOME YOUR COMPANION (debug testing versions aside)

	; Joel - Adding hook for Follower/Command Mode tutorial 
	if !tutorial.getStageDone(602)
		tutorial.setStage(602)
	endif

	if DogmeatRef.GetRace() != Game.GetCommonProperties().DogmeatRace
		Game.Warning(self + "SetDogmeatCompanion() DogmeatRef is not DogmeatRace. " + DogmeatRef)
		DogmeatRef = None
	endif

	if DogmeatRef == none
		DogmeatRef = Game.GetCommonProperties().DogmeatRef
	endif

	trace(self, "SetDogmeatCompanion(): " + DogmeatRef + "; will dismiss current companion")

	Actor CompanionActor = Companion.GetActorReference()
	if CompanionActor
		DismissCompanion(CompanionActor, ShowLocationAssignmentListIfAvailable = true, SuppressDismissMessage = false)
	endif

	;hook for achievements
	ManageAchievementCompanions(DogmeatRef)

	DogmeatCompanion.ForceRefTo(DogmeatRef)

	DogmeatRef.IgnoreFriendlyHits()
	DogmeatRef.SetPlayerTeammate(abTeammate = true, abCanDoFavor = true, abGivePlayerXP = true)
	
	
	DogmeatRef.AddtoFaction(HasBeenCompanionFaction)

	FollowerFollow(DogmeatRef)
	FollowerSetDistanceMedium(DogmeatRef)

	;start the AO dogmeat find story manager ping
	AOScript.GetScript().DogmeatLoading()

	;ADDED FOR SURVIVAL MODE - though it probably should have been in for the base game, no one apparently needed it until now
	SendCompanionChangeEvent(DogmeatRef, IsNowCompanion = True)

	PlayerHasActiveDogmeatCompanion.SetValue(1)

EndFunction

;If you are trying to clear a particular companion, call SetCompanion with the false parameter instead.
Function ClearCompanion()
	;check if there is a companion before doing any of this to avoid throwing errors

	Actor actorToClear = Companion.GetActorReference()

	Trace(self, "ClearCompanion() actorToClear: " + actorToClear)

	if actorToClear
		CompanionDataToggle(false, false)

		actorToClear.StopCombatAlarm()

		Companion.Clear()

		SendCompanionChangeEvent(actorToClear, IsNowCompanion = False)
		
		PlayerHasActiveCompanion.SetValue(0)

		;** Question:
		;Should I also remove the companion from the ActiveCompanions RefAliasCollection? At the moment I am not, but
		;down the road dependning on what all uses ActiveCompanions collection, I might want to.

	endif
EndFunction

;Reminder this is primarily called via function on Actor script
Function DisallowCompanion(Actor ActorToDisallow, bool SuppressDismissMessage = false)
	debug.trace(self + "DisallowCompanion()" + ActorToDisallow)
	ActorToDisallow.AddToFaction(DisallowedCompanionFaction)

	if ActorToDisallow.GetRace() == Game.GetCommonProperties().DogmeatRace
		DismissDogmeatCompanion(SuppressDismissMessage = SuppressDismissMessage)
	else
		DismissCompanion(ActorToDisallow, SuppressDismissMessage = SuppressDismissMessage)
	endif

	
EndFunction

;Reminder this is primarily called via function on Actor script
Function AllowCompanion(Actor ActorToAllow, bool MakeCompanionIfNoneCurrently = true, bool ForceCompanion = false)
	debug.trace(self + "AllowCompanion()" + ActorToAllow + ", MakeCompanionIfNoneCurrently:" + MakeCompanionIfNoneCurrently + ", ForceCompanion:" + ForceCompanion )
	ActorToAllow.RemoveFromFaction(DisallowedCompanionFaction)
	if (PlayerHasActiveCompanion.GetValue() == 0 && PlayerHasActiveDogmeatCompanion.GetValue() == 0 && MakeCompanionIfNoneCurrently) || ForceCompanion
		
		if ActorToAllow.GetRace() == Game.GetCommonProperties().DogmeatRace
			SetDogmeatCompanion(ActorToAllow)
		else
			SetCompanion(ActorToAllow)
		endif
	endif

EndFunction

;Note: you are probably looking for DismissDogmeatCompanion() instead
Function ClearDogmeatCompanion()
		;check if there is a companion before doing any of this to avoid throwing errors

	Actor actorToClear = DogmeatCompanion.GetActorReference()

	Trace(self, "ClearDogmeatCompanion() actorToClear: " + actorToClear)

	if actorToClear
		actorToClear.IgnoreFriendlyHits(false)
		actorToClear.SetPlayerTeammate(false)
		
		DogmeatCompanion.Clear()

		SendCompanionChangeEvent(actorToClear, IsNowCompanion = False)

		PlayerHasActiveDogmeatCompanion.SetValue(0)
	
	endif


EndFunction

Function CompanionDataToggle(bool toggleOn = true, bool canDoFavor = true, bool givePlayerXP = true, bool SetNotShowOnStealthMeter  = true)
	
	CompanionActorScript CompanionActor = Companion.GetActorReference() as CompanionActorScript

	CompanionActor.IgnoreFriendlyHits(toggleOn) 							; ***THIS SHOULD PROBABLY BE A FLAG ON ALIASES
	CompanionActor.SetPlayerTeammate(toggleOn, canDoFavor, givePlayerXP) 	; ***THIS SHOULD PROBABLY BE A FLAG ON ALIASES
	
	;reminder Companion is the alias, not the reference

	;cache the idle timer actor values
	ActorValue IdleChatterTimeMin = Game.GetCommonProperties().IdleChatterTimeMin
	ActorValue IdleChatterTimeMax = Game.GetCommonProperties().IdleChatterTimeMax


	if toggleOn
		if SetNotShowOnStealthMeter 
			CompanionActor.SetNotShowOnStealthMeter(true)
		endif


		;IDLE CHATTER - cache and set them
		CachedIdleChatterTimeMin = CompanionActor.GetValue(IdleChatterTimeMin)
		CachedIdleChatterTimeMax = CompanionActor.GetValue(IdleChatterTimeMax)
		CompanionActor.SetValue(IdleChatterTimeMin, CompanionIdleChatterTimeMin)
		CompanionActor.SetValue(IdleChatterTimeMax, CompanionIdleChatterTimeMax)

		;ADD KEYWORDS
		;we also add keywords directly on the Companion alias
		;this is for keywords that some but all companions should have
		;Example: playerCanStimpak should be added to human companions, but not robot ones 
		int i = 0
		while (i < CompanionActor.KeywordsToAddWhileCurrentCompanion.length)
			CompanionActor.AddKeyword(CompanionActor.KeywordsToAddWhileCurrentCompanion[i])			
			i += 1
		endwhile

		RegisterForHitEvent(Companion)
		RegisterForRemoteEvent(CompanionActor, "OnCombatStateChanged")
	else
		CompanionActor.SetNotShowOnStealthMeter (false)

		;IDLE CHATTER - set them back
		CompanionActor.SetValue(IdleChatterTimeMin, CachedIdleChatterTimeMin)
		CompanionActor.SetValue(IdleChatterTimeMax, CachedIdleChatterTimeMax)

		;REMOVE KEYWORDS
		int i = 0
		while (i < CompanionActor.KeywordsToAddWhileCurrentCompanion.length)
			CompanionActor.RemoveKeyword(CompanionActor.KeywordsToAddWhileCurrentCompanion[i])			
			i += 1
		endwhile

		UnregisterForAllHitEvents(Companion)
		UnRegisterForRemoteEvent(CompanionActor, "OnCombatStateChanged")
	endif

EndFunction

Function TryToTeleportCompanion(ObjectReference TeleportDestinationRef, bool ShouldPlayTeleportInEffect = true, bool ShouldPlayTeleportOutEffect = true)
	debug.trace(self + "TryToTeleportCompanion()")

	Actor CompanionActor = Companion.GetActorReference()

	if CompanionActor
		TeleportActor(CompanionActor, TeleportDestinationRef, ShouldPlayTeleportInEffect, ShouldPlayTeleportOutEffect)
	endif
EndFunction

Function TryToTeleportDogmeat(ObjectReference TeleportDestinationRef, bool ShouldPlayTeleportInEffect = true, bool ShouldPlayTeleportOutEffect = true)
	debug.trace(self + "TryToTeleportDogmeat()")

	Actor DogmeatActor = DogmeatCompanion.GetActorReference()

	if DogmeatActor
		TeleportActor(DogmeatActor, TeleportDestinationRef, ShouldPlayTeleportInEffect, ShouldPlayTeleportOutEffect)
	endif
EndFunction

Function TeleportActor(Actor ActorToTeleport, ObjectReference TeleportDestinationRef, bool ShouldPlayTeleportInEffect, bool ShouldPlayTeleportOutEffect)
	debug.trace(self + "TeleportActor() ActorToTeleport: " + ActorToTeleport + ", TeleportDestinationRef: " + TeleportDestinationRef)


	TeleportActorScript TeleportActor = ActorToTeleport as TeleportActorScript

	if ShouldPlayTeleportOutEffect
		if TeleportActor
			TeleportActor.TeleportIn()
		else
			game.Warning("FollowerScript: TeleportFollowers() actor doesn't have TeleportActorScript attached, can't play fx! " + ActorToTeleport)
		endif
	endif

	utility.wait(5)

	debug.trace(self + "TeleportActor() calling MoveTo()" + TeleportDestinationRef)
	TeleportActor.MoveTo(TeleportDestinationRef)
	debug.trace(self + "TeleportActor() done with moveto")

	if ShouldPlayTeleportInEffect
		if TeleportActor
			TeleportActor.TeleportIn()
		else
			game.Warning("FollowerScript: TeleportFollowers() actor doesn't have TeleportActorScript attached, can't play fx! " + ActorToTeleport)
		endif
	endif

	debug.trace(self + "TeleportActor() done")

EndFunction



;********************************* FOLLOWER COMMANDS ***************************************************

Function FollowerWait(Actor Follower)
	SetFollowerActorValue(Follower, Game.GetCommonProperties().FollowerState, iFollower_Com_Wait)
EndFunction

Function FollowerFollow(Actor Follower)
	SetFollowerActorValue(Follower, Game.GetCommonProperties().FollowerState, iFollower_Com_Follow)
EndFunction

Function FollowerSetDistanceNear(Actor Follower)
	SetFollowerActorValue(Follower, Game.GetCommonProperties().FollowerDistance, iFollower_Dist_Near)
EndFunction

Function FollowerSetDistanceMedium(Actor Follower)
	SetFollowerActorValue(Follower, Game.GetCommonProperties().FollowerDistance, iFollower_Dist_Medium)
EndFunction

Function FollowerSetDistanceFar(Actor Follower)
	SetFollowerActorValue(Follower, Game.GetCommonProperties().FollowerDistance, iFollower_Dist_Far)
EndFunction

Function SetFollowerActorValue(Actor Follower, ActorValue WhichValueToSet, GlobalVariable iFollower_Global)
	Follower.SetValue(WhichValueToSet, iFollower_Global.GetValue())
EndFunction

	

;*******************************************************************************************************
;**************************************   SCENES   **************************************************
;*******************************************************************************************************

;/
Companions will have various scenes that they are in, triggered by:
Attraction Object events
Actor Hello Events
Change Location Events
Quest Stage result scripts

To make things simple, they are all going to be script event quests started by a common function call
that those other quest types will call in their start up phase quest stage.

/;

Group SceneData
keyword Property CIS_Companion_Scene_Start const auto
{Keyword used to start Companion Interaction Scenes}

GlobalVariable Property CIS_Type_EnterNewLocation const auto
{AUTOFILL}

EndGroup



;Reminder:
;This sends out to story manager Script Event
;Look for "CIS" nodes in those to set timers and conditions.
bool Function StartCompanionInteractionScene(GlobalVariable SceneTypeGlobal, objectReference SceneRef, location SceneLoc, Actor CompanionActor = None) global
	;SceneTypeGlobal is a global that represents the scene type, which is the numeric variable we send with sendStoryEvent function call
	;SceneRef is the primary reference/actor in the scene with the companion
	;SceneLoc is the location where the scene is happening
	;CompanionActor default = whoever is in the Companion alias

	FollowersScript selfScript = GetScript()

	if CompanionActor == None
		CompanionActor = selfScript.Companion.GetActorReference()
	endif

	trace(selfScript, "StartCompanionInteractionScene()" + SceneTypeGlobal + ", " + SceneRef + ", " + SceneLoc + ", " + CompanionActor )

	RETURN selfScript.CIS_Companion_Scene_Start.SendStoryEventAndWait(akLoc = SceneLoc, akRef1 = CompanionActor, akRef2 = SceneRef, aiValue1 = SceneTypeGlobal.GetValue() as int)
	;*** SEE ALSO: CIS_ManagerScript ***

EndFunction



;*******************************************************************************************************
;**************************************   LOITERING   **************************************************
;*******************************************************************************************************

;/

Used to get anyone running the FollowPlayer package to Sandbox around the player if the player is loitering.

Adapted by sLiesegang's DLC1NPCMonitoringPlayerScript from Dawnguard (adding check against all buffered positions, not just the last)

See also:
	PACKAGES
	- FollowPlayer Template 
		Sandboxes around the Player if:
			1) AllowLoitering Package Data is True, and
			2) isPlayerLoitering is True (variable set by this script)
		Otherwise Follows the player 
/;


CustomEvent Loitering

bool DebugTrace_Loitering = false

bool DebugDropMarkers = false 	;SHOULD ALWAYS BE SET TO FALSE IN RELEASE ARCHIVES - set to true if you want to drop debug markers to help visualize the  buffer (need to TMK in console to see them)
ObjectReference[] debugMarkers

Actor playerRef
Bool Property isPlayerLoitering conditional auto hidden
  ;player isn't moving far from where he was a little while ago 

float LoiterRadius = 1500.0	const		;in units - isPlayerLoitering = true if he has not gone farther than this from his previously buffered sampled position
float TimerInterval_LoiterSample = 1.0 const	;in seconds


int LoiterBufferSize = 10  
;* How many LoiterSample's to store.
;* Each new sample shifts previous samples into higher index.
;* Larger LoiterBufferSize means it takes longer from traveling (in a straightline) to loitering when resting
;	and triggers traveling again faster if resting for brief period


float StandardLoiterCoolDownTime = 30.0  ;minimum amount of time before being allowed to loiter again
float FollowCommandLoiterCoolDownTime = 90.0 ; after a follower gets a follow command we make the cool down longer

float CurrentLoiterCoolDownTime


;These Arrays are buffers of players last positions
;index(0) is the most recent, index(LoiterBufferSize -1) is the oldest
float[] PlayerPosX
float[] PlayerPosY
float[] PlayerPosZ


Function SendLoiteringEvent(bool isNowLoitering)
	Var[] args = new Var[2]

	args[0] = isNowLoitering	;true =  started loitering, false = stopped loitering
	args[1] = Companion  ;the companion alias - note: you usually want the alias itself (not the actor in it), in case it changes out from under you


	traceconditional(self, "Sending LoiteringEvent. " + args, DebugTrace_Loitering)
	sendCustomEvent("Loitering", args)

EndFunction


Function InitializeLoiterBuffer()
	float PlayerX = PlayerRef.X
	float PlayerY = PlayerRef.Y
	float PlayerZ = PlayerRef.Z

	int i = 0
	while (i < LoiterBufferSize - 1)
		BufferCurrentPosition(i, PlayerX, PlayerY, PlayerZ)

		i += 1
	endwhile

EndFunction

Function HandleTimer_LoiterSample()

	float FurthestDistance = GetFurthestDistanceAndPromoteSamples()
	traceconditional(self, "FurthestDistance = " + furthestDistance, DebugTrace_Loitering)

	if DebugDropMarkers == true ; THIS SHOULD ALWAYS BE FALSE IN RELEASE ARCHIVES
		DropDebugMarkers()
	EndiF

	bool LoiterCoolingDown

	CurrentLoiterCoolDownTime -= TimerInterval_LoiterSample
	LoiterCoolingDown = CurrentLoiterCoolDownTime > 0

	bool Traveled = FurthestDistance > LoiterRadius
	bool InCombat = PlayerRef.IsInCombat()
	bool Sneaking = PlayerRef.IsSneaking()
	bool Sprinting = PlayerRef.IsSprinting()
	bool WeaponOut = PlayerRef.IsWeaponDrawn() 
	bool NotSitting = PlayerRef.GetSitState() != 3 


	if DebugTrace_Loitering
		Trace(self, "SetIsPlayerLoitering():")
		Trace(self, "LoiterCoolingDown: " + LoiterCoolingDown)
		Trace(self, "Traveled: " + Traveled)
		Trace(self, "InCombat: " + InCombat)
		Trace(self, "Sneaking: " + Sneaking)
		Trace(self, "Sprinting: " + Sprinting)
		Trace(self, "WeaponOut: " + WeaponOut)
		Trace(self, "NotSitting: " + NotSitting)
	endif
	bool shouldEVPFollowers

	if ( Traveled || LoiterCoolingDown || InCombat || Sneaking || Sprinting || WeaponOut ) && NotSitting

		if (isPlayerLoitering == true)
			traceconditional(self, "*** PLAYER NO LONGER LOITERING ***", DebugTrace_Loitering)
		
			SendLoiteringEvent(isNowLoitering = false)

			shouldEVPFollowers = true

			SetLoiterCoolDown(StandardLoiterCoolDownTime)
		endif
		
		isPlayerLoitering = false

	else 
		if (isPlayerLoitering == false)
			traceconditional(self, "*** PLAYER IS NOW LOITERING ***", DebugTrace_Loitering)
			
			SendLoiteringEvent(isNowLoitering = true)
			
			shouldEVPFollowers = true
		endif

		isPlayerLoitering = true
	endif


	if isPlayerLoitering
		traceconditional(self, "* LOITERING *", DebugTrace_Loitering)
	else
		traceconditional(self, "* NOT LOITERING *", DebugTrace_Loitering)

		

	endif

	;**** TO DO ADD EVALUATE PACKAGE LOGIC HERE****
	;can certainly do this for companion, not sure if we'll have a way to get all the normal followers

	if Companion.GetActorReference() && shouldEVPFollowers
		Companion.GetActorReference().EvaluatePackage()

	endif

	if DogmeatCompanion.GetActorReference() && shouldEVPFollowers
		;**** TO DO ADD EVALUATE PACKAGE LOGIC HERE****
		;can certainly do this for companion, not sure if we'll have a way to get all the normal followers

		DogmeatCompanion.GetActorReference().EvaluatePackage()
	endif

	startTimer(TimerInterval_LoiterSample, iTimerID_LoiterSample)

EndFunction


float Function GetFurthestDistanceAndPromoteSamples()
	;check all positions against player's current poistion then
	;cycle all positions up one notch in the history arrays


	float furthestDistance
	float bufferedDistance

	float PlayerX = PlayerRef.X
	float PlayerY = PlayerRef.Y
	float PlayerZ = PlayerRef.Z

	int i = LoiterBufferSize - 1
	while (i >= 0)

		if i > 0 ;promote lower buffer into me
			PlayerPosX[i] = PlayerPosX[i - 1]
			PlayerPosY[i] = PlayerPosY[i - 1]
			PlayerPosZ[i] = PlayerPosZ[i - 1]

		else ;i'm the first buffer, so fill me with the player postion
			BufferCurrentPosition(0, PlayerX, PlayerY, PlayerZ)

		endif

		bufferedDistance = GetDistanceFromSamplePosition(i, PlayerX, PlayerY, PlayerZ)

		if bufferedDistance > furthestDistance
			furthestDistance = bufferedDistance
		endif

;		trace(self, "Buffer#" + i + " distance: " + bufferedDistance)

		i -= 1
	endwhile
	
	return furthestDistance

EndFunction

Function BufferCurrentPosition(int bufferToSet, float X, float Y, float Z)
	PlayerPosX[bufferToSet] = X
	PlayerPosY[bufferToSet] = Y
	PlayerPosZ[bufferToSet] = Z

EndFunction

;Called  in SetIsPlayerLoitering()
;**Also called with a longer cool down whenany follower receives a follow command )
Function SetLoiterCoolDown(float CoolDownTime)
	CurrentLoiterCoolDownTime = CoolDownTime

EndFunction


int function GetLastBufferIndex()
	return LoiterBufferSize - 1
EndFunction


float Function GetDistanceFromSamplePosition(int BufferIndex, float X, float Y, float Z)
	; calculate distance between buffered position and present
	;    sqrt((x2 - x1)^2 + (y2 - y1)^2 + (z2 - z1)^2)
	;thank you sLiesegang! :)

	float xFactor = (PlayerPosX[BufferIndex] - X)
	float yFactor = (PlayerPosY[BufferIndex] - Y)
	float zFactor = (PlayerPosZ[BufferIndex] - Z)
	
	xFactor = xFactor * xFactor
	yFactor = yFactor * yFactor
	zFactor = zFactor * zFactor

	return Math.sqrt(xFactor + yFactor + zFactor)
EndFunction

Function DropDebugMarkers() debugOnly
	if DebugDropMarkers == true
		Game.Warning(self + "WARNING!: DropDebugMarkers() THIS SHOULD NOT BE CALLED IN RELEASE BUILDS!")

		;create the markers if this is the first time
		if debugMarkers == None
			debugMarkers = New ObjectReference[LoiterBufferSize]
			
			static markerToDrop = Game.GetFormFromFile(0x0003180C, "Fallout4.esm") as Static ; Red Flag

			int i = 0
			while (i < debugMarkers.length - 1)
				debugMarkers[i] = playerRef.placeAtMe(markerToDrop)
				i += 1
			endwhile

		EndiF

		;move the markers to the locations in the position buffer
		int i = 0
		while (i < debugMarkers.length - 1)
			debugMarkers[i].setPosition(PlayerPosX[i], PlayerPosY[i], PlayerPosZ[i])

			i += 1
		endwhile


	EndiF

EndFunction




;*******************************************************************************************************
;************************************** COMPANION AFFINITY *********************************************
;*******************************************************************************************************

;/
	The Companion Affinity system is how we handle each Companion response
	to various events and player actions. And how they feel in total over time.
	
	Each event is more or less important.
	Each companion has a different feeling about each event.

	This information is supplied, and each event is handled by the scripts on the individual Companions.

	See: CompanionActorScript for details and implementation

/;


Struct AffinityEventData
	keyword 		EventKeyword
{Keyword associated with this event. Used as the primary event identifier.
Filter: "CA_Event_*"}

	GlobalVariable 	EventSize
{Global representing the size of the event.
Filter: "CA_Size_*" 
USUALLY: CA_Size_DialogueOnly for events that shouldn't change how the companion feels about the player.}

	GlobalVariable 	CoolDownDays	;in terms of game days passed
{Global representing the cool down period for the event. Value in terms of GameDaysPassed.
Filter: "CA_CoolDownDays_*"}

	keyword 		TopicSubType	;for dialogue to say
{Keyword TopicSubType. Dialogue to say when this event happens.
Filter: "CAT_*"}

	actorvalue 		AssociatedActorValue ;very often NONE
{USUALLY: NONE -- when in doubt leave this one blank.) 
The AssociatedActorValue.}

	float 			NextDayAllowed	hidden ;will store gamedayspassed + CoolDownDays when that event occurs

EndStruct

Group Affinity

actorvalue property CA_Affinity  const auto
actorvalue property CA_CurrentThreshold  const auto
actorvalue property CA_WantsToTalk  const auto
{Used for conditions on affinity scene greetings:
0 = doesn't want to talk
1 = wants to talk, will forcegreet player
2 = wants to talk, will not forcegreet playe
}
actorvalue property CA_WantsToTalkRomanceRetry  const auto


;Used by companion interjections
;actorvalue Property CA_LastDialogueBump const auto
GlobalVariable Property CA_DialogueBump_Dislike const auto
GlobalVariable Property CA_DialogueBump_Hate const auto
GlobalVariable Property CA_DialogueBump_Like const auto
GlobalVariable Property CA_DialogueBump_Love const auto
Keyword Property CAT_DialogueBump_Disliked const auto
Keyword Property CAT_DialogueBump_Hated const auto
Keyword Property CAT_DialogueBump_Liked const auto
Keyword Property CAT_DialogueBump_Loved const auto


actorvalue property CA_AffinitySceneToPlay  const auto
actorvalue property CA_Custom const auto
actorvalue property CA_HighestThreshold  const auto
actorvalue property CA_LowestThreshold  const auto
actorvalue property CA_LastChangePositive  const auto
actorvalue property CA_HighestReached  const auto
actorvalue property CA_LowestReached  const auto
actorvalue property CA_MurderSessionCount const auto
{how many murder sessions did the companion witness}
actorvalue property CA_MurderSessionDay const auto
{The last murder session occured on this day (in terms of game days passed)}
actorvalue property CA_MurderSessionVictimCount const auto
{The last murder sessions victim count - might be less than actual if people died in rapid succession}

Location[] Property LocationsToIgnoreMurderIn auto hidden
{When it's okay to kill anyone in this location, add it to this list. For example, end game "attack XYZ faction" quests}


actorvalue property CA_IsRomantic const auto
actorvalue property CA_IsRomanceableNow const auto
 
;Used by AffinityEvent
GlobalVariable Property SmallAffinityEvent const auto
GlobalVariable Property NormalAffinityEvent const auto
GlobalVariable Property LargeAffinityEvent const auto

spell Property SleepNearInfatuatedCompanionBonus const auto
{The spell to cast on the player if he sleeps near a companion who is infatuated with him.}

MagicEffect Property CA_AddictionEffect const auto

AffinityEventData[] property AffinityEvents auto



EndGroup

struct CrippleEventDatum
	ActorValue 	LimbConditionActorValue
	Keyword 	AffinityKeyword_Companion
	Keyword 	AffinityKeyword_Player
EndStruct
	
Group AffinityEventSending
{Used by scripting to respond to game events.}

keyword Property AskedForMoreCapsKeyword const auto
{CA__Event_SpeechForMoreCaps}

keyword Property GenerousKeyword const auto
{CA__CustomEvent_Generous}

keyword Property SelfishKeyword const auto
{CA__CustomEvent_Selfish}

keyword Property NiceKeyword const auto
{CA__CustomEvent_Nice}

keyword Property MeanKeyword const auto
{CA__CustomEvent_Mean}

keyword Property PeacefulKeyword const auto
{CA__CustomEvent_Generous}

keyword Property ViolentKeyword const auto
{CA__CustomEvent_Generous}

keyword Property AddictionKeyword const auto
{CA_Event_ChemAddiction}

keyword Property DogmeatBleedoutKeyword const auto
{CA_Event_DogmeatBleedout}

keyword Property VertibirdKeyword const auto
{CA_Event_EnterVertibird{}

keyword Property WorkbenchKeyword const auto
{CA_Event_UseWorkbench{}

keyword Property SwimmingKeyword const auto
{CA_Event_Swim}

keyword Property RadiationKeyword const auto
{CA_Event_RadDamage}

keyword Property ModArmorKeyword const auto
{CA_Event_ModArmor}

keyword Property ModWeaponKeyword const auto
{CA_Event_ModWeapon}

CrippleEventDatum[] Property CrippleEventData const auto 
{Holds mapping between actor values and affinity event keyword}

keyword Property EnterPowerArmorKeyword const auto
{CA_Event_EnterPowerArmor}

keyword Property isPowerArmorFrame const auto
{Pointer to Power Armor Furniture keyword so Companion can comment when you get into it.}

keyword Property DischargeWeapon const auto
{CA_Event_DischargeWeapon}


keyword Property JumpFromHeight const auto
{CA_Event_JumpFromHeight}


keyword Property HealCompanion const auto
{CA_Event_HealCompanion}

keyword Property HealDogmeat const auto
{CA_Event_HealDogmeant}


EndGroup


CustomEvent AffinityEvent
CustomEvent PossibleMurderEvent

Actor SleepCompanionActor

;GLOBAL VERSION
bool Function SendAffinityEvent(scriptobject Sender, keyword EventKeyword, objectReference Target = None, GlobalVariable EventSizeOverride = None, bool CheckCompanionProximity = true, bool ShouldSuppressComment = false, bool IsDialogueBump = false, float ResponseDelay = 1.0) global
	return GetScript().LocalSendAffinityEvent(Sender, EventKeyword, Target, EventSizeOverride, CheckCompanionProximity, ShouldSuppressComment, IsDialogueBump, ResponseDelay)
EndFunction

;LOCAL VERSION
bool LockSendAffinityEvent
bool Function LocalSendAffinityEvent(scriptobject Sender, keyword EventKeyword, objectReference Target = None, GlobalVariable EventSizeOverride = None, bool CheckCompanionProximity = true, bool ShouldSuppressComment = false, bool IsDialogueBump = false, float ResponseDelay = 1.0)

	while LockSendAffinityEvent
		utility.wait(0.1)
	endwhile
	LockSendAffinityEvent = true


	trace(self, "FollowersScript: SendAffinityEvent(" + EventKeyword + ", " + EventSizeOverride + ", " + CheckCompanionProximity + ") sender:" + sender)


	AffinityEventData CurrentAffinityEventData = GetAffinityEventData(EventKeyword)
	if !CurrentAffinityEventData
		LockSendAffinityEvent = false
		RETURN false
	endif

	if CurrentAffinityEventData.NextDayAllowed > Utility.GetCurrentGameTime()
		trace(self, "FollowersScript: SendAffinityEvent() NOT sending event as CheckAffinityEventCooldown() == False. NextDayAllowed: " + CurrentAffinityEventData.NextDayAllowed + ". Current GameDaysPassed:" + Utility.GetCurrentGameTime())
		LockSendAffinityEvent = false
		RETURN false
	endif


	float NextDayAllowed = CurrentAffinityEventData.CoolDownDays.GetValue() + Utility.GetCurrentGameTime()

	Trace(self, "FollowersScript: SendAffinityEvent() NextDayAllowed for event = " + NextDayAllowed)

	CurrentAffinityEventData.NextDayAllowed = NextDayAllowed

	globalvariable EventSize

	if EventSizeOverride
		EventSize = EventSizeOverride
	else
		EventSize = CurrentAffinityEventData.EventSize
	endif

	Var[] args = new Var[8]
	args[0] = EventKeyword
	args[1] = EventSize
	;args[7] = Target - see below

	args[2] = CheckCompanionProximity
	args[3] = CurrentAffinityEventData.AssociatedActorValue

	if ShouldSuppressComment == false
		args[4] = CurrentAffinityEventData.TopicSubType
	endif

	args[5] = IsDialogueBump
	args[6] = ResponseDelay
	args[7] = Target ;the reason this args number is out of order is because it was added later

	sendCustomEvent("AffinityEvent", args)

	LockSendAffinityEvent = false
	RETURN TRUE

EndFunction



AffinityEventData Function GetAffinityEventData(keyword EventKeyword) global
	
	AffinityEventData[] myAffinityEvents = GetScript().AffinityEvents

	int i = 0
	while (i < myAffinityEvents.length)
		if myAffinityEvents[i].EventKeyword == EventKeyword
			RETURN myAffinityEvents[i]
		endif

		i += 1
	endwhile

	game.error("WARNING: FollowersScript.GetAffinityEventData() didn't find EventKeyword in AffinityEvents: " + EventKeyword)

EndFunction

function SendPossibleMurderEvent(Actor ActorVictim, Actor ActorKiller, bool GameConsidersMurder) global

	followersScript thisScript = GetScript()

	var[] args = New Var[3]

	args[0] = ActorVictim
	args[1] = ActorKiller
	args[2] = GameConsidersMurder ;the Storymanager indicated the kill event was murder

	if thisScript.IsActorInLocationWhereMurderShouldBeIgnored(ActorKiller)
		debug.trace("FollowersScript.SendPossibleMurderEvent() ignoring murder IsActorInLocationWhereMurderShouldBeIgnored() == true")
		RETURN
	endif

	thisScript.sendCustomEvent("PossibleMurderEvent", args)

EndFunction

Function SetLocationForCompanionsToIgnoreMurder(location LocationToIgnoreMurderIn)
	
	if LocationsToIgnoreMurderIn == None
		LocationsToIgnoreMurderIn = new location[0]
	endif

	if LocationsToIgnoreMurderIn.find(LocationToIgnoreMurderIn) < 0
		LocationsToIgnoreMurderIn.add(LocationToIgnoreMurderIn)
	endif
EndFunction

bool Function IsActorInLocationWhereMurderShouldBeIgnored(actor ActorToTest)
	int i = 0
	while (i < LocationsToIgnoreMurderIn.Length)
		if ActorToTest.IsInLocation(LocationsToIgnoreMurderIn[i])
			debug.trace(self + "IsActorInLocationWhereMurderShouldBeIgnored() actor is in location where murder should be ignored: " + ActorToTest + ", " + LocationsToIgnoreMurderIn[i])
			RETURN true
		endif
		
		i += 1
	endwhile

	RETURN false

EndFunction

Function HandleOnCrippleEvent(Var akSender, ActorValue akActorValue, bool abCrippled) ;sent by Remotely Registered OnCripple events

	if abCrippled ;if we're crippled not healed
		if (akSender as Actor) == PlayerRef
			SendAffinityCrippledEvent(akSender as Actor, akActorValue, isPlayer = true)

		elseif akSender as ReferenceAlias == Companion
			SendAffinityCrippledEvent((akSender as ReferenceAlias).GetActorReference(), akActorValue, isCompanion = true)

		endif
	endif

EndFunction

function HandleVertibirdEvent(ObjectReference akVertibird)

	;*** We'll need to add handling for making sure companion and dogmeat get into the veribird if they are near
	; Check that companion / dogmeat are nearby, if not ignore them
	; Find the available seat keyword since we don't know where player is sitting
	; Give the companion that keyword
	; Shove the vertibird reference into an alias on followers quest
	; Give companion and dogmeat package to mount the vertibird alias
	; have a while loop that waits for them to be ismounted (with a bailout timer)
	; expose a "CompanionsVertibirdAccountedFor" that can be checked in the scene to make the vertibird take off
	; add a condition for "player is in vertibird" condition to the wait at self in the FollowPlayer package template
	; when player dismounts vertibird teleport people who are following him but not loaded
	; and remove the mount package from companion and dogmeat
	
	;but for now just have them say something:
	SendAffinityEvent(akVertibird, VertibirdKeyword)

EndFunction

Function HandleOnWorkbench(objectReference akWorkBench)
	SendAffinityEvent(akWorkBench, WorkbenchKeyword)

EndFunction

Function HandleOnSwimming(Location akLocation)
	SendAffinityEvent(akLocation, SwimmingKeyword)
EndFunction

Function HandleOnRadiation(Location akLocation)
	SendAffinityEvent(akLocation, RadiationKeyword)
EndFunction

Function HandleOnMod(Form ModdedBaseObject)
	if ModdedBaseObject is Weapon
		SendAffinityEvent(ModdedBaseObject, ModWeaponKeyword)
	elseif ModdedBaseObject is Armor
		SendAffinityEvent(ModdedBaseObject, ModArmorKeyword)
	else
		trace(self, "HandleOnMod ModdedBaseObject is not a weapon or armor:" + ModdedBaseObject)
	endif

EndFunction


Function SendAffinityCrippledEvent(Actor akSenderActor, ActorValue akActorValue, bool isPlayer = false, bool isCompanion = false)

	keyword EventKeyword

	int i = 0
	while (i < CrippleEventData.length)
			if CrippleEventData[i].LimbConditionActorValue == akActorValue

				if isCompanion
					EventKeyword = CrippleEventData[i].AffinityKeyword_Companion
				
				elseif isPlayer
					EventKeyword = CrippleEventData[i].AffinityKeyword_Player
				
				endif
			endif
		i += 1
	endwhile

	if EventKeyword == none
		game.warning(self + "SendAffinityCrippledEvent() couldn't find LimbCondition event! LimbConditionActorValue: " + akActorValue)
		RETURN
	endif

	SendAffinityEvent(akSenderActor, EventKeyword)

EndFunction

Function HandleOnItemEquipped(ObjectReference akSender, Form akBaseObject)

	if akSender == Game.GetPlayer() && akBaseObject.HasKeyword(isPowerArmorFrame)
		SendAffinityEvent(akBaseObject, EnterPowerArmorKeyword)
	endif
EndFunction


actor Function GetNearbyInfatuatedRomanticCompanion()
	Actor NearbyInfatuatedActor

	CompanionActorScript CompanionActor = Companion.GetActorReference() as CompanionActorScript

	;if your current companion is infatuated, then use that
	if CompanionActor && CompanionActor.IsInfatuated() && CompanionActor.IsRomantic() && CompanionActor.IsPlayerNearby()
		NearbyInfatuatedActor = CompanionActor
	else
		;loop through active companions seeing who is nearby and infatuated, use the closest
		int i = 0
		while (i < ActiveCompanions.GetCount())
			CompanionActor = (ActiveCompanions.GetAt(i) as Actor) as CompanionActorScript

			if CompanionActor && CompanionActor.IsInfatuated() && CompanionActor.IsRomantic() && CompanionActor.IsPlayerNearby()
				if NearbyInfatuatedActor == false
					NearbyInfatuatedActor = CompanionActor
				else
					if NearbyInfatuatedActor.GetDistance(PlayerRef) > CompanionActor.GetDistance(PlayerRef)
						NearbyInfatuatedActor = CompanionActor
					endif
				endif
			endif
			i += 1
		endwhile
	endif

	trace(self, "GetNearbyInfatuatedRomanticCompanion() NearbyInfatuatedActor: " + NearbyInfatuatedActor)

	return NearbyInfatuatedActor
	
EndFunction

Function HandleOnSleepStart(objectReference bedRef)
	;if there's a nearby infatuated companion, put them in temprary alias to get them to sleep in the same bed as the player clicked on

	SleepCompanionActor = GetNearbyInfatuatedRomanticCompanion()

	if SleepCompanionActor
		SleepCompanionActor.AddKeyword(FollowersCompanionSleepNearPlayerFlag)
		SleepCompanion.ForceRefTo(SleepCompanionActor)
		SleepCompanionBed.ForceRefTo(BedRef)
		SleepCompanionActor.EvaluatePackage()
		;actor will say custom dialogue subtype CAT_Event_SleepTogetherWake when package finishes
	endif


EndFunction

Function HandleOnSleepEnd()
	;give the player an exp bonus if he slepted near a infatuated companion
	;Note: we may decide later to only have romantic infatuated companions sleep in same bed, but we want to give plutonic buddies the exp bonus

	if SleepCompanionActor
		SleepNearInfatuatedCompanionBonus.Cast(PlayerRef, PlayerRef)
		SleepCompanionActor.RemoveKeyword(FollowersCompanionSleepNearPlayerFlag)
	endif
		
	SleepCompanion.Clear()		;make the actor "wake up"

EndFunction


;******************** FUNCTIONS CALLED BY OTHER SCRIPTS ***********************
;these are global functions to make the calling PlayerSaidXXX.psc scripts not require properties so designers can just "tag" their dialogue with the script and move on.

Function AskedForMoreCaps(scriptObject Sender) global
	SendAffinityEvent(Sender, GetScript().AskedForMoreCapsKeyword, ShouldSuppressComment = true)
EndFunction

Function PlayerSaidGenerous(TopicInfo CallingTopicInfo) global
	SendAffinityEvent(CallingTopicInfo, GetScript().GenerousKeyword, ShouldSuppressComment = true)
EndFunction

Function PlayerSaidSelfish(TopicInfo CallingTopicInfo) global
	SendAffinityEvent(CallingTopicInfo, GetScript().SelfishKeyword, ShouldSuppressComment = true)
EndFunction

Function PlayerSaidNice(TopicInfo CallingTopicInfo) global
	SendAffinityEvent(CallingTopicInfo, GetScript().NiceKeyword, ShouldSuppressComment = true)
EndFunction

Function PlayerSaidMean(TopicInfo CallingTopicInfo) global
	SendAffinityEvent(CallingTopicInfo, GetScript().MeanKeyword, ShouldSuppressComment = true)
EndFunction

Function PlayerSaidPeaceful(TopicInfo CallingTopicInfo) global
	SendAffinityEvent(CallingTopicInfo, GetScript().PeacefulKeyword, ShouldSuppressComment = true)
EndFunction

Function PlayerSaidViolent(TopicInfo CallingTopicInfo) global
	SendAffinityEvent(CallingTopicInfo, GetScript().ViolentKeyword, ShouldSuppressComment = true)
EndFunction


Function FlagCompanionChatEvent(actorvalue ActorValueRepresentingEvent) global
	;get the player's current companion, check proximity, then flag them by setting this actor value to 1

	CompanionActorScript CompanionActor = GetScript().Companion.GetActorReference() as CompanionActorScript

	if CompanionActor && CompanionActor.IsPlayerNearby()
		debug.trace("FollowersScript.FlagCompanionChatEvent() CompanionActor is nearby, setting ActorValue to 1: " + ActorValueRepresentingEvent + ", " + CompanionActor)
		CompanionActor.SetValue(ActorValueRepresentingEvent, 1)
	endif

EndFunction



;*******************************************************************************************************
;************************************** SITUATION AWARENESS ********************************************
;*******************************************************************************************************

;/

For overview see:
http://institute.bethsoft.com/index.php?title=Category:Companions#Situational_Awareness

The basic gist of the system is:
There's arrays of structs of related data.

When we need to set data because the player enters/leaves triggerbox, or encounters an actor
We look for matching data in the struct array
We then look to see if any of those keywords/factions are on the locations/triggerbox scripts/actors we are asking about
	If so we set appropriate SAS/SAE/SAC prefixed variables.
		The value we set them to, is:
		The value in the GlobalVariable held at the found keyword/faction's index in the related array.

When the player:
	enters a triggerbox:
		SAS:
			we set  SAS variable and global based on keywords supplied by triggerbox meaning the "type of location encompassed by this triggerbox"
		SAE:
			we set SAEClutter variable and global based on keywords supplied by triggerbox that mean the "type of enemy based on the clutter we can see from this triggerbox"
	leaves a triggerbox:
		SAS:
			we set variables back to data derived from keywords that are on the Location (we call the same function as when player changes location)
		SAE:
			we clear SAEClutter variable and global 
	gets into combat :
		(presently handles as when he or his Companion is hit, or when his cCompanion changes combat state)
		SAE:
			We set SAECombatant variable and global based on actor's faction (which maps to location keyword)
			There is a timer that then clears these
		SAC:
			We also set a SAC "last time combat happened" variable and global
			A timer increments this data so that it "promotes" through recent, less recent, very ancient status

	While we really only need to store the Value of the associated GlobalVariables for all this data. (and it's only a float that is exposed via the "conditional" variable flag)
		We store the actual GlobalVariable itself as well, so there's something human readable in logs and console inspection


see also:
	REGISTERED REMOTE EVENTS:
		- ObjectReference.OnHit temporarily sets SAE variables

	OTHER SCRIPTS/OBJECTS:
		- SituationAwarenessTriggerScript -- handles triggers sending messages to this script
	

/;


Struct SetDefinition
	keyword LocSetKeyword
;{Holds LocType Keywords}
	globalvariable LocSetGlobal
;{Holds Globals used as "enums" in dialogue conditions}
EndStruct


Struct EncDefinition
	keyword LocEncKeyword
;{Holds LocType Keywords}
	faction Associated_Faction
;{Holds Factions that correspond to LocEncKeywords}
	globalvariable LocEncGlobal
;{Holds Globals used as "enums" in dialogue conditions}

EndStruct


Struct LastCombatDefinition
	globalvariable SAC_Global
	float SAC_Time

EndStruct

Group SituationAwareness

SetDefinition[] property SetDefinitions const auto
{Holds definitions for a Location Set - Keyword on locations and an associated Global used in dialogue conditions}

EncDefinition[] property EncDefinitions const auto
{Holds definitions for a Location Encounter Type - keyword on locations, a faction associated with that keyword, and an associated Global used in dialogue conditions}

Formlist property SA_CurrentSAEClutterTriggersList const auto
{Autofill 
list of triggers the player is in 
see SAEClutterTriggerScript 
needed to make sure we know exactly which trigger box the player actually in, in cases where triggerboxes might over lap and player teeters in/out of them
}

Formlist property SA_CurrentSASTriggersList const auto
{Autofill 
list of triggers the player is in 
see SASTriggerScript 
needed to make sure we know exactly which trigger box the player actually in, in cases where triggerboxes might over lap and player teeters in/out of them
}


LastCombatDefinition[] property LastCombatDefinitions const auto
{Holds Globals that are human readable and associated floats which are in terms of GameDaysPassed
Globals named like: SAC_LastCombat_ShortTimeAgo | MediumTimeAgo | LongTimeAgo
Floats are in terms of GameDaysPassed (1 = 24 hours, 0.04166 = 1 hour)
Suggested values: Short: 0.0, Medium: 0.2, Long: 3.0

IMPORTANT: Arrange in ascending order of distance
Short time ago in index 0, very long time ago in last index
Note: first value is always treated as 0.0
}


Endgroup

bool DebugTrace_SA = true

;S.A.S. - [S]ituation [A]warenes location [S]et type
;	This is for follower comments talking about the "the place we are walking around"
;		Primarily set by keywords on the location
;		Secondarily set by triggerboxes
Float SAS = -1.0 conditional 	;used by conditions - holds a value (based on global variable "enums" in LocSetGlobals)
GlobalVariable SAS_Global  		;For ease of debugging in console: holds the Global who's value is stored in SAS var
float SAS_TimeStamp 			;in game days passed - for debugging purposes, when did we last set this
scriptObject SAS_ScriptObject	;for debugging purpses, what was the last thing to set his



;S.A.E. - [S]ituation [A]warenes location [E]ncounter type
	;'THINGS THAT LIVE HERE':
Float SAEClutter = -1.0 conditional 	;used by conditions - holds a value (based on global variable "enums" in LocEncGlobals)
GlobalVariable SAEClutter_Global  		;For ease of debugging in console: holds the Global who's value is stored in SAE var
float SAEClutter_TimeStamp 			;in game days passed - for debugging purposes, when did we last set this
scriptObject SAEClutter_ScriptObject	;for debugging purpses, what was the last thing to set his
	;'THING WE'VE RECENTLY FOUGHT':
Float SAECombatant = -1.0 conditional 		;used by conditions - holds a value (based on global variable "enums" in LocEncGlobals)
GlobalVariable SAECombatant_Global  		;For ease of debugging in console: holds the Global who's value is stored in SAE var
float SAECombatant_TimeStamp 			;in game days passed - for debugging purposes, when did we last set this
scriptObject SAECombatant_ScriptObject	;for debugging purpses, what was the last thing to set his


;S.A.C. - [S]ituation [A]warenes location [C]ombat data
;holds information about how long ago the last combat was
GlobalVariable SAC_LastCombat_Global 	;For ease of debugging in console: holds the Global who's value is stored in SAC_LastCombat var
float SAC_LastCombat = -1.0				;how long ago was the last combat - uses SAC_LastCombat_xxx globals as enums for values

float SAC_LastCombat_Timestamp	;as GameDaysPassed used when setting SAC_LastCombat variables

Function ShowSA()
	string msg = "SITUATION AWARENESS:" + "\n"
	msg += "SAS = " + SAS + "\n"
	msg += "SAS_Global = " + SAS_Global + "\n"
	msg += "SAS_TimeStamp = " + SAS_TimeStamp + "\n"
	msg += "SAS_ScriptObject = " + SAS_ScriptObject + "\n"
	msg += "\n"
	msg += "SAEClutter = " + SAEClutter + "\n"
	msg += "SAEClutter_Global = " + SAEClutter_Global + "\n"
	msg += "SAEClutter_TimeStamp = " + SAEClutter_TimeStamp + "\n"
	msg += "SAEClutter_ScriptObject = " + SAEClutter_ScriptObject + "\n"
	msg += "\n"
	msg += "SAECombatant = " + SAECombatant + "\n"
	msg += "SAECombatant_Global = " + SAECombatant_Global + "\n"
	msg += "SAECombatant_TimeStamp = " + SAECombatant_TimeStamp + "\n"
	msg += "SAECombatant_ScriptObject = " + SAECombatant_ScriptObject + "\n"
	msg += "\n"
	msg += "SAC_LastCombat = " + SAC_LastCombat + "\n"
	msg += "SAECombatant_Global = " + SAECombatant_Global + "\n"
	msg += "SAC_LastCombat_Timestamp = " + SAC_LastCombat_Timestamp + "\n"

	trace(self, msg)
	debug.messagebox(msg)
EndFunction


SetDefinition Function GetSetDefinition(keyword KeywordToFind = None, location LocationToCheck = None)
	;finds the first matching item
	int i = 0
	while (i < SetDefinitions.length)
	
		if KeywordToFind && SetDefinitions[i].LocSetKeyword == KeywordToFind
			RETURN SetDefinitions[i]
		endif

		if LocationToCheck && LocationToCheck.HasKeyword(SetDefinitions[i].LocSetKeyword)
			RETURN SetDefinitions[i]
		EndIf

		i += 1
	endwhile

EndFunction


EncDefinition Function GetEncDefinition(keyword KeywordToFind = None, location LocationToCheck = None, Faction FactionToCheck = None, Actor ActorToCheck = None)
	;finds the first matching item
	int i = 0
	while (i < EncDefinitions.length)
		
		if KeywordToFind && EncDefinitions[i].LocEncKeyword == KeywordToFind
			RETURN EncDefinitions[i]
		endif
		
		if LocationToCheck && LocationToCheck.HasKeyword(EncDefinitions[i].LocEncKeyword)
			RETURN EncDefinitions[i]
		endif

		if FactionToCheck && EncDefinitions[i].Associated_Faction == FactionToCheck
			RETURN EncDefinitions[i]
		endif

		if ActorToCheck && ActorToCheck.IsInFaction(EncDefinitions[i].Associated_Faction)
			RETURN EncDefinitions[i]
		endif

		i += 1
	endwhile

EndFunction


int Function GetIndexOfLastCombatDefinition(GlobalVariable GlobalToFind = None)
	;finds the first matching item
	int i = 0
	while (i < LastCombatDefinitions.length)

		if GlobalToFind && LastCombatDefinitions[i].SAC_Global == GlobalToFind
			RETURN i
		endif
	
		i += 1
	endwhile
EndFunction



Function SetSAS(keyword LocSetKeyword, scriptObject SettingObject)
	;sets the conditional variables used by conditions to flavor dialogue about what type of location set they are walking through

	traceconditional(self, "SetSAS() LocSetKeyword:" + LocSetKeyword, DebugTrace_SA)

	;find the appropriate definition
	SetDefinition FoundSetDefinition = GetSetDefinition(KeywordToFind = LocSetKeyword)

	if FoundSetDefinition == false
		traceconditional(self, "Caution: SetSAS() did not find keyword." + LocSetKeyword, DebugTrace_SA, 1)

		SAS_Global = None
		SAS = -1.0
	else
		SAS_Global = FoundSetDefinition.LocSetGlobal
		SAS = SAS_Global.value
	endif

		SAS_TimeStamp = Utility.GetCurrentGameTime()
		SAS_ScriptObject = SettingObject

	traceconditional(self, "SetSAS() SAS_Global is now: " + SAS_Global, DebugTrace_SA)
	traceconditional(self, "SetSAS() SAS is now: " + SAS, DebugTrace_SA)

EndFunction

Function SetSAE(keyword LocEncKeyword, bool shouldSetSAEClutter, bool shouldSetSAECombatant, scriptObject SettingObject)
		if shouldSetSAEClutter
			SetSAEClutter(LocEncKeyword, SettingObject)
		endif

		if shouldSetSAECombatant
			SetSAECombatant(LocEncKeyword, SettingObject)
		endif


EndFunction

Function SetSAEClutter(keyword LocEncKeyword, scriptObject SettingObject)
	;find the appropriate definition
	EncDefinition FoundEncDefinition = GetEncDefinition(KeywordToFind = LocEncKeyword)

	if FoundEncDefinition == false
		traceconditional(self, "Caution: SetSAEClutter() did not find keyword." + LocEncKeyword, DebugTrace_SA, 1)
	
		SAEClutter_Global = None
		SAEClutter = -1.0
	else
		SAEClutter_Global = FoundEncDefinition.LocEncGlobal
		SAEClutter = FoundEncDefinition.LocEncGlobal.value	

	endif

	SAEClutter_TimeStamp = utility.GetCurrentGameTime()
	SAEClutter_ScriptObject = SettingObject
	
	traceconditional(self, "SetSAEClutter() SAEClutter_Global is now: " + SAEClutter_Global, DebugTrace_SA)
	traceconditional(self, "SetSAEClutter() SAEClutter is now: " + SAEClutter, DebugTrace_SA)

EndFunction

Function ClearSAEClutter()
	SAEClutter_Global = None
	SAEClutter = -1.0

	traceconditional(self, "ClearSAEClutter() SAEClutter_Global is now: " + SAEClutter_Global, DebugTrace_SA)
	traceconditional(self, "ClearSAEClutter() SAEClutter is now: " + SAEClutter, DebugTrace_SA)
EndFunction

Function SetSAECombatant(keyword LocEncKeyword, scriptObject SettingObject)
	;find the appropriate definition
	EncDefinition FoundEncDefinition = GetEncDefinition(KeywordToFind = LocEncKeyword)

	if FoundEncDefinition == false
		traceconditional(self, "Caution: SetSAECombatant() did not find keyword." + LocEncKeyword, DebugTrace_SA, 1)
		
		SAECombatant_Global = None
		SAECombatant = -1.0
	else
		SAECombatant_Global = FoundEncDefinition.LocEncGlobal
		SAECombatant = FoundEncDefinition.LocEncGlobal.value

	endif

	SAECombatant_TimeStamp = utility.GetCurrentGameTime()
	SAECombatant_ScriptObject = SettingObject

	traceconditional(self, "SetSAECombatant() SAECombatant_Global is now: " + SAECombatant_Global, DebugTrace_SA)
	traceconditional(self, "SetSAECombatant() SAECombatant is now: " + SAECombatant, DebugTrace_SA)

	;start timer that will set the values back to permValues
	startTimerGameTime(TimerInterval_SAECombatantExpiry, iTimerID_GameTime_SAECombatantExpiry)
EndFunction

Function ClearSAECombatant()
	SAECombatant_Global = None
	SAECombatant = -1.0

	traceconditional(self, "ClearSAECombatant() SAECombatant_Global is now: " + SAECombatant_Global, DebugTrace_SA)
	traceconditional(self, "ClearSAECombatant() SAECombatant is now: " + SAECombatant, DebugTrace_SA)
EndFunction

;### SetSituationAwarenessBasedOnLocation()
;called by:
;	FUNCTIONS: 	
;		- HandleMessageFromSituationAwarenessTrigger()
;			[ oringinating from: SituationAwarenessTriggerScript.OnTriggerEnter/Leave() ]
;
;	REGISTERED REMOTE EVENTS:
;		- Actor.OnLocationChange
Function SetSituationAwarenessBasedOnLocation(Location LocationToCheck, bool SetSASData, bool SetSAEClutterData, scriptObject settingObject)
	;**similar to SetSituationAwarenessBasedOnActor

	traceconditional(self, "SetSituationAwarenessBasedOnLocation() LocationToCheck:" + LocationToCheck, DebugTrace_SA)

	SetDefinition FoundSetDefinition = GetSetDefinition(LocationToCheck = LocationToCheck)
	EncDefinition FoundEncDefinition = GetEncDefinition(LocationToCheck = LocationToCheck)

	if SetSASData
		if FoundSetDefinition
			traceconditional(self, "SetSituationAwarenessBasedOnLocation() FoundSetDefinition:" + FoundSetDefinition, DebugTrace_SA)
			keyword LocSet = FoundSetDefinition.LocSetKeyword
			SetSAS(LocSet, LocationToCheck)
		else
			traceconditional(self, "SetSituationAwarenessBasedOnLocation() didn't find a matching SetDefinition", DebugTrace_SA)
			SetSAS(None, LocationToCheck)
		endif

	endif

	if SetSAEClutterData
		if FoundEncDefinition
			keyword LocEnc = FoundEncDefinition.LocEncKeyword
			SetSAE(LocEnc, shouldSetSAEClutter = true, shouldSetSAECombatant = false, settingObject = settingObject)
		else
			traceconditional(self, "SetSituationAwarenessBasedOnLocation() didn't find a matching EncDefinition", DebugTrace_SA)
			SetSAE(None, shouldSetSAEClutter = true, shouldSetSAECombatant = false, settingObject = settingObject)
		endif
	endif
EndFunction


;### SetSituationAwarenessBasedOnActor()
;called by:
;	FUNCTIONS: 	
;		- HandleCombatMessageToSituationAwareness()
;			[ oringinating from: ObjectReference.OnHit ]
Function SetSituationAwarenessBasedOnActor(Actor ActorToCheck, bool shouldSetLastCombatTime, bool shouldSetSAEClutter, bool shouldSetSAECombatant, scriptObject SettingObject)
	;**similar to SetSituationAwarenessBasedOnLocation

	traceconditional(self, "SetSituationAwarenessBasedOnActor() ActorToCheck:" + ActorToCheck, DebugTrace_SA)

	EncDefinition FoundEncDefinition = GetEncDefinition(ActorToCheck = ActorToCheck)

	if FoundEncDefinition
		Faction LocEncFaction = FoundEncDefinition.Associated_Faction
		SetSituationAwarenessBasedOnFaction(LocEncFaction, shouldSetSAEClutter, shouldSetSAECombatant, SettingObject)
	else
		traceconditional(self, "SetSituationAwarenessBasedOnActor() didn't find a matching EncDefinition", DebugTrace_SA)
		SetSituationAwarenessBasedOnFaction(None, shouldSetSAEClutter, shouldSetSAECombatant, SettingObject)
	endif

	if shouldSetLastCombatTime
		SAC_LastCombat_Timestamp = utility.GetCurrentGameTime() ;returns GameDaysPassed
		SAC_LastCombat_Global = LastCombatDefinitions[0].SAC_Global
		SAC_LastCombat = SAC_LastCombat_Global.value

		if DebugTrace_SA
			trace(self, "SetSituationAwarenessBasedOnActor() SAC_LastCombat_Timestamp: " + SAC_LastCombat_Timestamp)
			trace(self, "SetSituationAwarenessBasedOnActor() SAC_LastCombat_Global: " + SAC_LastCombat_Global)
			trace(self, "SetSituationAwarenessBasedOnActor() SAC_LastCombat: " + SAC_LastCombat)
		endif

		startTimerGameTime(TimerInterval_SAC_LastCombat, iTimerID_GameTime_SAC_LastCombat)
	endif

EndFunction

;### SetSituationAwarenessBasedOnFaction()
;called by:
;	FUNCTIONS: 	
;		- SetSituationAwarenessBasedOnActor()
Function SetSituationAwarenessBasedOnFaction(faction FactionToUse, bool shouldSetSAEClutter, bool shouldSetSAECombatant, scriptObject settingObject)
	trace(self, "SetSituationAwarenessBasedOnFaction() Faction:" + FactionToUse + ", shouldSetSAEClutter: " + shouldSetSAEClutter + ", shouldSetSAECombatant: " + shouldSetSAECombatant)

	Keyword FoundKeyword

	 EncDefinition definition = GetEncDefinition(factionToCheck = FactionToUse)

  	if definition
    	FoundKeyword = definition.LocEncKeyword
  	EndIf

	if FoundKeyword == False	
		traceconditional(self, "Caution: SetSituationAwarenessBasedOnFaction didn't find faction: " + FactionToUse, DebugTrace_SA, 1 )
		SetSAE(None, shouldSetSAEClutter, shouldSetSAECombatant, settingObject = settingObject) 
	endif

	SetSAE(FoundKeyword, shouldSetSAEClutter, shouldSetSAECombatant, settingObject = settingObject) 

EndFunction

;### HandleMessageFromSAEClutterTrigger()
;called by:
;	EVENTS:
;		OnTimerGameTime()
Function HandleTimer_GameTime_SAECombatantExpiry() ;called by Event OnGameTimer()
	ClearSAECombatant()
EndFunction

;### HandleMessageFromSAEClutterTrigger()
;called by:
;	OTHER OBJECTS: 	
;		- HandleMessageFromSAEClutterTrigger.OnTriggerEnter/Leave()
Function HandleMessageFromSAEClutterTrigger(SAEClutterTriggerScript triggerbox, bool AddMe)
		;addMe false = remove me

		;**** !!!! VERY SIMILAR TO HandleMessageFromSASTrigger !!! ***

		if AddMe
			SA_CurrentSAEClutterTriggersList.addForm(triggerbox)
		else 
			SA_CurrentSAEClutterTriggersList.RemoveAddedForm(triggerbox)
		endif

		;DEBUG DUMP CONTENTS
		bool debugMe = true
		if debugMe
			if SA_CurrentSAEClutterTriggersList.getSize() <= 0
				traceconditional(self, "SA_CurrentSAEClutterTriggersList is EMPTY", DebugTrace_SA)
			endif

			int i = 0
			while (i < SA_CurrentSAEClutterTriggersList.getSize())
				traceconditional(self, "SA_CurrentSAEClutterTriggersList[" + i + "]-->" + SA_CurrentSAEClutterTriggersList.getAt(i), DebugTrace_SA)

				i += 1
			endwhile
		endif


		;If the list isn't empty get the first item's (triggerbox) keywords
		if SA_CurrentSAEClutterTriggersList.getSize() > 0
			SAEClutterTriggerScript FirstInList = SA_CurrentSAEClutterTriggersList.getAt(0) as SAEClutterTriggerScript

			if FirstInList.LocEncKeyword
				SetSAE(FirstInList.LocEncKeyword, shouldSetSAEClutter = true, shouldSetSAECombatant = false, SettingObject = triggerbox)
			else
				traceconditional(self, "SA_CurrentSAEClutterTriggersList FirstInList.LocEncKeyword == None", DebugTrace_SA, 1)
			endif

		else ;the list is empty, so clear the variables
			ClearSAEClutter()

		endif


EndFunction


;### HandleMessageFromSASTrigger()
;called by:
;	OTHER OBJECTS: 	
;		- HandleMessageFromSASTrigger.OnTriggerEnter/Leave()
Function HandleMessageFromSASTrigger(SASTriggerScript triggerbox, bool AddMe)
		;addMe false = remove me

		;**** !!!! VERY SIMILAR TO HandleMessageFromSAEClutterTrigger !!! ***

		if AddMe
			SA_CurrentSASTriggersList.addForm(triggerbox)
		else 
			SA_CurrentSASTriggersList.RemoveAddedForm(triggerbox)
		endif

		;DEBUG DUMP CONTENTS
		bool debugMe = true
		if debugMe
			if SA_CurrentSASTriggersList.getSize() <= 0
				traceconditional(self, "SA_CurrentSASTriggersList is EMPTY", DebugTrace_SA)
			endif

			int i = 0
			while (i < SA_CurrentSASTriggersList.getSize())
				traceconditional(self, "SA_CurrentSASTriggersList[" + i + "]-->" + SA_CurrentSASTriggersList.getAt(i), DebugTrace_SA)

				i += 1
			endwhile
		endif


		;If the list isn't empty get the first item's (triggerbox) keywords
		if SA_CurrentSASTriggersList.getSize() > 0
			SASTriggerScript FirstInList = SA_CurrentSASTriggersList.getAt(0) as SASTriggerScript

			if FirstInList.LocSetKeyword
				SetSAS(FirstInList.LocSetKeyword, triggerbox)
			else
				traceconditional(self, "SA_CurrentSASTriggersList FirstInList.LocSetKeyword == None", DebugTrace_SA, 1)
	
			endif

		else ;the list is empty, so clear the variables
			SetSituationAwarenessBasedOnLocation(playerRef.GetCurrentLocation(), SetSASData = true, SetSAEClutterData = false, SettingObject = triggerbox)

		endif


EndFunction


;### HandleCombatMessageToSituationAwareness
;Called by:
;	REGISTED REMOTE EVENTS:
;		- ObjectReference.OnHit
Function HandleCombatMessageToSituationAwareness(Actor Enemy) 
	SetSituationAwarenessBasedOnActor(Enemy, shouldSetLastCombatTime = true, shouldSetSAEClutter = false, shouldSetSAECombatant = true, settingObject = Enemy)
EndFunction


Function Handletimer_GameTime_SAC_LastCombat() ;called by Event OnGameTimer()
	trace(self, "OnTimerGameTime() checking SAC last combat values")

	;check to see if the time stamp exceeds the next highest global/float pair.
	;If so, set the new data accordingly
	;If we're not at the end of the array, throw the timer again, otherwise don't

	int i = GetIndexOfLastCombatDefinition(SAC_LastCombat_Global)

	if i < 0
		;Note: this should be impossible since SAC_LastCombat_Global is set by finding an actual item out of the array it's checking for.
		trace(self, "WARNING: Handletimer_GameTime_SAC_LastCombat() could not find SAC_LastCombat_Global in the array:" + SAC_LastCombat_Global)
		
		RETURN
	endif

	if i + 1 >= LastCombatDefinitions.length
		;Note: this out of range shouldn't be possible because of we don't throw the timer again if isLastItemInArrayCheck below
		trace(self, "WARNING: Handletimer_GameTime_SAC_LastCombat() -- i >= SAC_LastCombat_Globals.length. Bailing out.")
		
		RETURN
	endif


	float TimeForNextSetting = SAC_LastCombat_Timestamp + LastCombatDefinitions[i + 1].SAC_Time

	trace(self, "SAC_LastCombat_Global: " + SAC_LastCombat_Global)
	trace(self, "Time to change to Next: " + TimeForNextSetting)
	trace(self, "Current Time: " + utility.GetCurrentGameTime())

	bool isLastItemInArray

	if utility.GetCurrentGameTime() >= TimeForNextSetting
		SAC_LastCombat_Global = LastCombatDefinitions[i + 1].SAC_Global
		SAC_LastCombat = SAC_LastCombat_Global.value

		isLastItemInArray = (LastCombatDefinitions.length - 1) <= i + 1

		if DebugTrace_SA
			trace(self, "setting new data...: " + SAC_LastCombat_Global)
			trace(self, "SAC_LastCombat_Global: " + SAC_LastCombat_Global)
			trace(self, "SAC_LastCombat: " + SAC_LastCombat)
		endif

	endif

	if isLastItemInArray == false
		startTimerGameTime(TimerInterval_SAC_LastCombat, iTimerID_GameTime_SAC_LastCombat)
		trace(self, "isLastItemInArray == false. Restarting timer.")
	else
		;we are at the longest time - do nothing
		trace(self, "isLastItemInArray == true. Not restarting timer.")
	
	endif
EndFunction



;*******************************************************************************************************
;**************************************     Autonomy    ************************************************
;*******************************************************************************************************
;used to help control when companions/dogmeat are allowed to "do their own thing" without direct input from player

bool property AutonomyAllowed = true auto hidden conditional 	;used to conditional scene manager nodes and quests

ScriptObject[] property AutonomyDisallowedByObjects auto hidden	;array holding all the callers who requested to turn off autonomy - these same objects will need to turn it back on

CustomEvent AutonomyDisallowed  			;Registered by quests to shutdown if their primary actor loses autonomy, etc.

Function SetAutonomy(ScriptObject CallingObject, bool AutonomousActivityAllowed = True) global
	GetScript().SetAutonomyPrivate(CallingObject, AutonomousActivityAllowed)
EndFunction

Function SetAutonomyPrivate(ScriptObject CallingObject, bool AutonomousActivityAllowed = True)
	trace(self, "SetAutonomy() CallingObject:" + CallingObject + ", AutonomousActivityAllowed:" + AutonomousActivityAllowed)
	
	int foundIndex = AutonomyDisallowedByObjects.find(CallingObject)

	if AutonomousActivityAllowed && foundIndex >= 0 ;if allowed and already in array, remove it
		AutonomyDisallowedByObjects.remove(foundIndex)
	
	elseif AutonomousActivityAllowed == false && foundIndex < 0 ;if not allowed and if not already in array, add it
		AutonomyDisallowedByObjects.add(CallingObject)
	
	endif

	;set flag based on whether any objects still want to prevent autonomy
	if AutonomyDisallowedByObjects.length == 0
		AutonomyAllowed = true
	else
		AutonomyAllowed = false

		;Kill running autonomous activities who have registered for the event (likely via quest scripts and aliases)
		var[] akArgs = new var[3]
		akArgs[0] = CallingObject 		;ScriptObject who doesn't want actor allowed autonomy
		akArgs[1] = Companion 			;convenient pointer to Companion alias
		akArgs[2] = DogmeatCompanion 	;convenient pointer to DogmeatCompanion alias

		SendCustomEvent("AutonomyDisallowed", akArgs)
	endif

EndFunction

Function AllowAutonomyOnTeleport()
	;this is to prevent problems that occur if SetAutonomy() allow events come in before disallow events
	AutonomyDisallowedByObjects.clear()
	AutonomyAllowed = true;

EndFunction