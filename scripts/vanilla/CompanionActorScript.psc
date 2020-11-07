Scriptname CompanionActorScript extends Actor conditional
{Attached to Actors that can be Player Companions, handles various things.

See also FollowersScript for AffinityEvent() function.

For example data, see COMMacCready actor
}

;Affinity Levels
;Affinity is measured on a scale of -1000 to 1000
;use globals to define thresholds
;we can change/create more thresholds if needed.
;Currently these are 100 points apart


Struct ThresholdData
	GlobalVariable Threshold_Global
	{which global is associated with this threshold}

	Bool IsMajorAffinityThreshold = true
	{if true, then the CA_CurrentThreshold actor values is set to this}

	Quest Controlling_Quest 
	{which quest to set stage in when threshold is reached}
	int Controlling_Quest_Stage
	{which stage to set in Threshold_Quest when threshold is reached}

	message ThresholdMessage
	{What message is associated with this affinity threshold}

	GlobalVariable AffinitySceneToPlay_FirstTimeReached
	{Which global's value represents the scene number for when this threshold is first reached? 
	For example, greeting could be conditionalized like:
	GetActorValue CA_AffinitySceneToPlay == CA_Scene_NeutralToAdmiration
	Naming convention: CA_Scene_XXX}
	GlobalVariable AffinitySceneToPlay_Repeat_GoingUp
	{Which global's value represents the scene number for when this threshol is reached again after the first time, and affinity is being gained.
	Naming convention: CA_Scene_Repeat_XXX}
	GlobalVariable AffinitySceneToPlay_Repeat_GoingDown
	{Which global's value represents the scene number for when this threshol is reached again after the first time, and affinity is being gained.
	Naming convention: CA_Scene_Repeat_XXX}

	bool ThresholdHasBeenPreviouslyReached = false
	{flag set by script so we know if this threshold has already been reached or not}

	GlobalVariable ConditionalGlobal = None;
	{Default: None. (Almost always should be)
	This global must be > 0 for the Companion to reach this affinity level.
	Use to gate affinity levels behind quest states.
	}

EndStruct

Struct EventData
Keyword Event_Keyword
{Keywords representing events that this Companion should respond to.}

GlobalVariable Disposition_Global  ;for likes, dislikes, etc.
{Globals that represent what to add/remove from Affinity.
CA_Loves
CA_Likes
CA_Indifferent
CA_Dislikes
;CA_Hates
}

Message Message_to_Display
{Messages that represent what to say.
Can be NONE}

GlobalVariable ConditionalGlobal = None;
{Default: None. (Almost always should be)
This global must be > 0 for the Companion to treat this Event as existing in the EventData_Array.
Used to turn on and off events. 
(Or "swap polarity" by putting in the event twice
once as a like and once as dislike and using different globals and toggleing them on/off appropriately)
}
	

EndStruct

Struct TraitPreference
	actorValue Trait
	bool Likes
EndStruct

float TraitPreferenceScaler = 1.0 


Group AffinitySettings
{}

ThresholdData[] Property ThresholdData_Array auto
{***Order these from Best to Worst*** (this is bubble sorted in onInit() now, so this is less important)}

int Property MinAffinityChangeFromLastThresholdCrossedRequired = 100 const auto
{How much affinity amount "padding" before changing affinity thresholds
Makes dipping slightly above/below the threshold back and forth not toggle between thresholds.}

GlobalVariable Property StartingThreshold const auto mandatory
{Which global represents this companion's starting Affinity.
Likely: CA_00}

GlobalVariable Property InfatuationThreshold const auto mandatory
{Which global represents this Companion's "Infatuation" affinity level}

Perk Property InfatuationPerk const auto mandatory
{What Perk does the player gain after unlocking Infatuation level (awared during the Infatuation scene)}

Message Property InfatuationPerkMessage const auto mandatory
{What message do we show the player when he gains the InfatuationPerk?}

Message Property InfatuationRomanticMessage const auto
{What message do we show the player when companion is at Romantic Infatuation}

ThresholdData[] Property CustomThresholdData_Array auto
{***Order these from Best to Worst*** (this is bubble sorted in onInit() now, so this is less important)
Custom quest stages to fire at various affinity values}

float Property MinAffinity = -1100.0 const auto
{default: -1100; affinity can never go below this}

float Property MaxAffinity = 1100.0 const auto
{default 1100; affinity can never go below this}

TraitPreference[] property TraitPreference_Array const auto
{If they like = true, dislike = false, neutral  = don't add it.}

EventData[] property EventData_Array const auto
;{Stages to set for Quest in Quests at this index for the event at this index in Keywords.
;Can be 0 = don't set stage}

EndGroup

Group AffinityCustomEvents
{Be sure to add these to the standard AffinitySetting's EventData_Array above && To AffinityEvents array on FollowersScript}

keyword Property DislikesEvent const auto mandatory
{CA_CustomEvent_<Companion>Dislikes}

keyword Property HatesEvent const auto mandatory
{CA_CustomEvent_<Companion>Hates}

keyword Property LikesEvent const auto mandatory
{CA_CustomEvent_<Companion>Likes}

keyword Property LovesEvent const auto mandatory
{CA_CustomEvent_<Companion>Loves}

EndGroup

Struct MurderThresholdData
	int 	MaxVictimsAllowed = 1000 ;people have written lines in the second murder quit scene like "I told you before..." so we shouldn't skip right to that.
	float 	AffinityPenalty = -250.0
	Quest 	QuestToSet 
	int 	StageToSet			;set quest stage when threshold is reached
EndStruct

Group MurderSettings

Formlist Property ConsideredMurderFactionList const auto
{If the player kills an ActorTypeNPC that is in a faction in this formlist, and the game considers it murder the companion considers it murder.}

faction[] property NotConsideredMurder_Array auto
{Exceptions to ConsideredMurderFactionList:
If the player kills an ActorTypeNPC  in a faction in this list, the companion does not considers it murder even if the game does}

MurderThresholdData[] property MurderThreshold_Array const auto
{Sets up data for murder counts and consequences}

globalVariable Property MurderToggle = none const auto
{If set, then if this global == 0, the Companion will ignore murders.}

int Property BodyCountAllowedBeforeMurderSession = 0 const auto
{Default: 0;
the number of actors needed to be murdered with in a span of time before the companion considers it a murder session}

EndGroup

Group GivePlayerItemData
bool Property ShouldGivePlayerItems = false auto
{Default: false, does this character give player items?
Note: Needs idles and hellos to make this function properly.
Conditionalize dialogue on ActorValue HasItemForPlayer == 1
Then in dialogue results add this script: CompanionGivePlayerItemInfoScript}

ActorValue Property HasItemForPlayer const auto mandatory
{AUTOFILL THIS}

LeveledItem Property ItemToGive const auto
{Leveled list that contains the item to give to the player}

EndGroup

Group MiscData

Location Property HomeLocation auto mandatory
{initial location for companion's home, this will be changed to whereever the player assigns them when dismissing the companion}

keyword[] Property KeywordsToAddWhileCurrentCompanion auto  ;this can't be const because Curie needs to change when she becomes human
{Add these keywords when a companion, and remove them when not.
Note: we also add keywords directly on the Companion alias
this is for keywords that some but all companions should have
Example: playerCanStimpak should be added to human companions, but not robot ones 
}

ActorValue Property TemporaryAngerLevel const auto mandatory
{AUTO FILL
When this == 1 it means the player did something recently the Companion didn't like.
It will automatically set itself back to 0 after a period of time.
Useful for conditionalizing cheery and happy sounding hellos and idles not to play.
}

keyword Property CA_Event_Murder const auto mandatory
{AUTOFILL}

scene property DismissScene const auto mandatory
{the companion's dismiss scene, so that when hitting the B button the start this scene}

Quest Property Tutorial Const Auto mandatory
{AUTOFILL}

ActorValue Property Experience const auto mandatory
{AUTOFILL - used to track player experience to track time spent adventuring with player}

globalVariable Property MQComplete const auto mandatory
{AUTOFILL - used to limit automatic "with player" affinity bumps from crossing thresholds until MQ is finished}

EndGroup




Group PrototypeData
	topic property IdleTopic const auto
	{TEMP - the companion's idle topic, so that when hitting the Y button they say this.}


EndGroup

; Patch 1.7 - add ability to override normal DismissCompanion
Group DismissCompanionData
	GlobalVariable property AllowDismissToSettlements auto const 
	{ OPTIONAL - if set, DismissCompanion will check if this global > 0 instead of WorkshopParent.PlayerOwnsAWorkshop}

	FormList Property DismissCompanionSettlementKeywordList auto const 
	{ OPTIONAL - if set, pass in this formlist of keywords to OpenWorkshopSettlementMenu call in DismissCompanion }
endGroup

FollowersScript Followers ;set in onInit()

;------Affinity Tracking Stats:--------------------------
;These might be needed for conditions thus they are conditional properties


GlobalVariable LastThresholdCrossed   
;stores the last threshold crossed, so we know if we need to set the stage or not when checking for new Affinity value
;see CheckAffinityAndSetThresholdAndStage()

float  PreviousAffinity  conditional 
;stores the previous affinity value whenever it changes

Globalvariable AffinitySceneToPlay
;stores the last AffinitySceneToPlay global so we have a human readable version of the CA_AffinitySceneToPlay actor value
;--------------------------------------------------------

;--------With Player Affinity Bumps -----------------------
;award affinity based on the companion being out adventuring with the player
;we check to see if the player is gaining XP to get around potential exploit of just standing around or waiting

float WithPlayerXPCache						;stores the player's XP the last time we checked, used to make sure the player is doing stuff not just waiting around
int WithPlayerXPGainMin	= 1	 				;player must have gained at least this much XP since the last check to qualify as adventuring with the companion not just waiting around

;used to curve affinity gain
;IMPORTANT! If affinity ranges go higher or lower than 1000, these numbers and formula may need to change because affinity could become negative
float WithPlayerBaseAffinityGainPerTick = 40.0
float WithPlayerCurrentAffinityMult = 0.033 ;puts a curve on the gain, so that gain slows down at ends of affinity spectrum

float Function GetWithPlayerAffinityGain()
	
	float affinityMod

	float currentAffinity = GetValue(Followers.CA_Affinity)

	;IF POSTIVE
	if currentAffinity >= 0
		affinityMod = WithPlayerBaseAffinityGainPerTick - (currentAffinity * WithPlayerCurrentAffinityMult)

	else ;IF NEGATIVE
		affinityMod = WithPlayerBaseAffinityGainPerTick + (currentAffinity * WithPlayerCurrentAffinityMult)
	endif

	;just incase the affinity curve formula would ever make it a negative number
	if affinityMod < 0
		affinityMod = 1
	endif

	;*Make sure you don't add anything if the player would be crossing a threshold
	;unless 50 hours have passed and main quest is done
	if TestValueForAffinityBump(affinityMod) && MQComplete.GetValue() == 0 && Game.GetRealHoursPassed() < 50
		debug.trace(self + "GetWithPlayerAffinityGain() return 0 because it would cause a bump in affinity threshold and we aren't done with MQ & 50 hours of play time yet.")
		affinityMod = 0
	endif

	return affinityMod

EndFunction

;--------------------------------------------------------

ThresholdData[] QueuedThresholdData_Array ;Holds the Queued up thresholds that haven't had their scenes resolved yet


bool ActiveMurderSessionOnGoing conditional		;used to determine murder session events, so the player can murder a bunch of people in a row and consider that a single "event" from the companion's perspective

int PreMurderSessionBodyCount		;used for companions that are okay with individual murders, but not mass murder
float LastMurderDay	;in terms of game days passed, when was last day player killed someone the companion considered murder
float DaysAllowedBetweenMurdersForBodyCount = 0.25 const ;added to LastMurderDay and compared against gamedayspassed to determine if we should increment the PreMurderSessionBodyCount or reset it


GlobalVariable LastDialogueBump   
;stores the last LastDialogueBump


bool InfatuationPerkUnlocked

;-------------- Some debugging variables --------------------
keyword LastAffinityEvent
globalVariable LastAffinityEventSize


bool HasLeftPlayerPermanently conditional

Event OnInit()

	Followers = FollowersScript.GetScript()

	LastThresholdCrossed = StartingThreshold
	setValue(Followers.CA_CurrentThreshold, LastThresholdCrossed.getValue())
	setValue(Followers.CA_Affinity, LastThresholdCrossed.getValue())

	if ThresholdData_Array == None
		Game.Warning(self + "WARNING: ThresholdData_Array has no values")
	else
		SortThesholdDataArray(ThresholdData_Array)	
	endif


	if CustomThresholdData_Array == None
		debug.trace(self + "Note: CustomThresholdData_Array has no values")
	else
		SortThesholdDataArray(CustomThresholdData_Array)	
	endif


	;create and dimension the QueuedThresholdData_Array
	QueuedThresholdData_Array = new ThresholdData[0] ;will grow and shrink, should definately not hit the 128 size limit

	RegisterForCustomEvent(Followers, "AffinityEvent")
	RegisterForCustomEvent(Followers, "PossibleMurderEvent")
	
	RegisterForCustomEvent(Followers, "CompanionChange")

EndEvent

Event OnDeath(Actor akKiller)
    UnregisterForCustomEvent(Followers, "AffinityEvent")
	UnregisterForCustomEvent(Followers, "PossibleMurderEvent")
	UnRegisterForCustomEvent(Followers, "CompanionChange")
EndEvent


;******************************* TIMERS *****************************

;GAME TIME TIMERS:
int TimerID_MurderSession = 1 const
float TimerInterval_MurderSession = 0.5  ;in game hours

int TimerID_WantsToTalkRomance = 2 const
float TimerInterval_WantsToTalkRomance_TriedAndFailed = 25.0  ;in game hours
float TimerInterval_WantsToTalkRomance_Declined = 168.0  ;in game hours


int TimerID_HasItemForPlayer = 3 const
float TimerIntervalMin_HasItemForPlayer = 12.0 ;in game hours
float TimerIntervalMax_HasItemForPlayer = 24.0 ;in game hours

int TimerID_TemporaryAngerLevel = 4 const
float TimerInterval_TemporaryAngerLevel = 1.0; in game hours - used anytime affinity changes negatively
float TimerInterval_TemporaryAngerLevel_ReallyMad = 8.0; in game hours - used for Murder and other major bummers

;REAL TIME TIMERS:
bool IsAffinityEventCommentAllowed = true ;for handling when are companion's allowed to say another comment about an affinity event
int TimerID_AffinityEventCommentAllowed = 10  const
float TimerInterval_AffinityEventCommentAllowed = 300.0 ;in real time seconds

int TimerID_WithPlayer = 11	const	;used set "WithPlayer" variables for affinity gained for spending time adventuring with player
float TimerInterval_WithPlayer = 600.0 ; 600 = 10 minutes -- (in real time seconds)


;GAME TIME
Event OnTimerGameTime(int aiTimerID)
	if aiTimerID == TimerID_MurderSession   ;STARTED IN MurderSession()
		EndMurderSession()

	elseif aiTimerID == TimerID_WantsToTalkRomance
		debug.trace(self + "SETTING CA_WantsToTalkRomanceRetry to 1 IN OnTimerGameTime()")
		SetValue(Followers.CA_WantsToTalkRomanceRetry, 1)	;will talk if player greets them

	elseif aiTimerID == TimerID_HasItemForPlayer
		HandleHasItemForPlayerTimer()

	elseif aiTimerID == TimerID_TemporaryAngerLevel
		SetValue(TemporaryAngerLevel, 0)

	endif

EndEvent

;REAL TIME
Event OnTimer(int aiTimerID)
	if aiTimerID == TimerID_AffinityEventCommentAllowed
		IsAffinityEventCommentAllowed = true

	elseif aiTimerID == TimerID_WithPlayer
		HandleWithPlayer()

	endif

EndEvent

;******************************* REMOTELY REGISTERED EVENTS *****************************

Event FollowersScript.CompanionChange(FollowersScript akSender, Var[] args)
	;args[0] = ActorThatChanged
	;args[1] = IsNowCompanion

	;store the player's xp
	if self == args[0] as Actor
		WithPlayerXPCache = Game.GetPlayer().GetValue(Experience)
	endif

	HandleWithPlayer()

EndEvent

;Sent by FollowersScript.SendAffinityEvent()
Event FollowersScript.AffinityEvent(FollowersScript akSender, Var[] args)
	debug.trace(self + "AffinityEvent()")

	;Find the index of the EventKeyword in the Keywords Array
	;then using that index, process the data in related Arrays at that same index
	;EventSize: (see TryToModAffinity)

	if isDead() ;just in case the event comes in after companion dies but before it's unregistered
		debug.trace(self + "AffinityEvent() Companion is DEAD -- IGNORING EVENT")
		RETURN
	endif

	if IsInFaction(Followers.HasBeenCompanionFaction) == false
		debug.trace(self + "AffinityEvent() Companion isn't in HasBeenCompanionFaction, IGNORING EVENT")
		RETURN
	endif


	if HasLeftPlayerPermanently
		debug.trace(self + "AffinityEvent() Companion HasLeftPlayerPermanently, IGNORING EVENT")
		RETURN
	endif

	bool CheckCompanionProximity = args[2] as bool

	if CheckCompanionProximity == true && isPlayerNearby() == false
		debug.trace(self + "AffinityEvent() Companion isn't nearby, IGNORING EVENT")
		RETURN
	endif

	debug.trace(self + "AffinityEvent() args:" + args)

	keyword EventKeyword = args[0] as keyword
	GlobalVariable EventSize = args[1] as GlobalVariable
	;CheckCompanionProximity = args[2] which is set above
	actorvalue associatedActorValue = args[3] as actorValue
	keyword CustomTopicKeyword = args[4] as keyword

	bool IsDialogueBump = args[5] as bool

	float ResponseDelay = args[6] as float
	
	objectReference Target = args[7] as objectReference

	;set LastDialogueBump if event is coming from dialogue:
	if IsDialogueBump

		;set data for this Companion:		
		if EventKeyword == DislikesEvent
			SetLastDialogueBump(Followers.CA_DialogueBump_Dislike)
		elseif EventKeyword == HatesEvent
			SetLastDialogueBump(Followers.CA_DialogueBump_Hate)
		elseif EventKeyword == LikesEvent
			SetLastDialogueBump(Followers.CA_DialogueBump_Like)
		elseif EventKeyword == LovesEvent
			SetLastDialogueBump(Followers.CA_DialogueBump_Love)
		endif

	endif


	EventData AffinityEventData = GetEventDataByKeyword(EventData_Array, EventKeyword)
	
	if !AffinityEventData ;not found
		debug.trace(self + "AffinityEvent() did not find event keyword: " + EventKeyword + " this might be a 'comment only' type event", 1)
		
		TryToSetTraitValues(associatedActorValue, EventSize, ShouldAlsoSetAffinity = true)

	else
		TryToModAffinity(AffinityEventData, EventSize)
		
		TryToSetTraitValues(associatedActorValue, EventSize, ShouldAlsoSetAffinity = false)
		

		Utility.wait(ResponseDelay)
		TryToShowMessage(AffinityEventData)

	endif

	;because many events are "commentOnly" we still need to try to say a line
	if IsAffinityEventCommentAllowed && CustomTopicKeyword && IsTalking() == false && IsInScene() == false
		;timer
		IsAffinityEventCommentAllowed = false
		StartTimer(TimerInterval_AffinityEventCommentAllowed, TimerID_AffinityEventCommentAllowed)

		;say comment
		TryToSayCustomTopic(CustomTopicKeyword, Target)
	endif

EndEvent

Event FollowersScript.PossibleMurderEvent(FollowersScript akSender, Var[] args)

	if isDead() ;just in case the event comes in after companion dies but before it's unregistered
		debug.trace(self + "PossibleMurderEvent() Companion is DEAD -- IGNORING EVENT")
		RETURN
	endif

	if  HasLeftPlayerPermanently
		debug.trace(self + "AffinityEvent() Companion HasLeftPlayerPermanently, IGNORING EVENT")
		RETURN
	endif


	actor ActorVictim = args[0] as Actor
	actor ActorKiller = args[1] as Actor
	bool GameConsidersMurder = args[2] as bool

	if ActorKiller == Game.GetPlayer() && IsInFaction(Followers.HasBeenCompanionFaction) && isPlayerNearby()

		debug.trace(self + "PossibleMurderEvent()" + akSender + args)

		if isMurder(ActorVictim, GameConsidersMurder) 
			debug.trace(self + "Murder: " + ActorVictim)

			bool ignoreMurderSession

			if BodyCountAllowedBeforeMurderSession > 0

				if LastMurderDay + DaysAllowedBetweenMurdersForBodyCount <= Utility.GetCurrentGameTime()
					PreMurderSessionBodyCount = 1
				else
					PreMurderSessionBodyCount += 1
				endif
	
				if PreMurderSessionBodyCount < BodyCountAllowedBeforeMurderSession
					debug.trace(self + "PreMurderSessionBodyCount [" + PreMurderSessionBodyCount + "] < BodyCountAllowedBeforeMurderSession [" + BodyCountAllowedBeforeMurderSession + "], NOT starting murder session. But will send a standard Murder event")
					FollowersScript.SendAffinityEvent(self, CA_Event_Murder)
					ignoreMurderSession = true	
				endif

			endif

			;***NOTE: in beta it was decided that the two strikes an you're out was too harsh, so we are going to turn off the toggle for everyone***
			if MurderToggle && MurderToggle.GetValue() == 0
				debug.trace(self + "MurderToggle == 0, NOT starting murder session. But will send a standard Murder event")
				FollowersScript.SendAffinityEvent(self, CA_Event_Murder)
				ignoreMurderSession = true
			endif

			LastMurderDay = Utility.GetCurrentGameTime()
			
			if ignoreMurderSession == false
				debug.trace(self + "Calling MurderSession() " + ActorVictim + ", " + ActorKiller)
				MurderSession(ActorVictim, ActorKiller)
			endif

		else
			debug.trace(self + "Not Murder: " + ActorVictim)
		endif

	endif

EndEvent





;******************************* EVENTS *****************************

;OBSOLETE?
Event OnCompanionDismiss()
	debug.trace(self + "OnCompanionDismiss()")

	DismissScene.Start()

EndEvent

;******************************* FUNCTIONS *****************************

;this is an inefficient bubble sort - use only on small arrays
Function SortThesholdDataArray(ThresholdData[] ThresholdDataArray)
	
	debug.trace(self + "SortThesholdDataArray()")
	Debug.Trace(self + "Unsorted: " + ThresholdDataArray)

	bool swapped = true
	int i = 0
	while (swapped == true)
		swapped = false
		i = 0

		while (i < ThresholdDataArray.length - 1)
			ThresholdData A = ThresholdDataArray[i]
			ThresholdData B = ThresholdDataArray[i + 1]

			if A.Threshold_Global.GetValue() < B.Threshold_Global.getValue()
				ThresholdDataArray[i] = B
				ThresholdDataArray[i + 1] = A
				swapped = true
			endif

			i += 1
		endwhile
	endwhile

	Debug.Trace(self + "Sorted: " + ThresholdDataArray)

EndFunction


bool Function IsInfatuated()
	return LastThresholdCrossed == InfatuationThreshold
EndFunction

bool Function IsRomantic()
	return GetValue(Followers.CA_IsRomantic) == 1

EndFunction

bool Function isPlayerNearby()
	bool returnVal = GetDistance(Game.GetPlayer()) <= 2500

	debug.trace(self + "isPlayerNearby():" + returnVal)

	RETURN returnVal

EndFunction


Function HandleWithPlayer()
	Actor playerRef = Game.GetPlayer()
	Int MaxPlayerDistance = 10000

	if IsInFaction(Followers.CurrentCompanionFaction)

		if GetDistance(playerRef) <= MaxPlayerDistance
			float CurrentXP = playerRef.GetValue(Experience)
			;if player gained enough xp in the meantime, consider the last chunk of time as spent adventuring with player
			if CurrentXP >= WithPlayerXPCache + WithPlayerXPGainMin
				
				WithPlayerXPCache = CurrentXP

				ModValue(Experience, WithPlayerXPCache)

				float affinityGain = GetWithPlayerAffinityGain()

				if affinityGain > 0
					ModAffinity(affinityGain)
				endif
			Endif
		
		else
			;make sure we don't continue to accumlate tracked xp
			WithPlayerXPCache = Game.GetPlayer().GetValue(Experience)

		endif

		startTimer(TimerInterval_WithPlayer, TimerID_WithPlayer)
	else 
		;i'm not canceling the timer here as a safety thing in case of timing issues with ChangeCompanion event and the timer firing.
		;and it doesn't matter that much if the player dismiss the companion before the full 15 minutes are up, that should still count toward affinity
	endif

EndFunction

EventData Function GetEventDataByKeyword(EventData[] ArrayToSearch, Keyword KeywordToFind)
	{This will return the first EventData in the array with the specified keyword where the conditional global is non-zero}

	int i = 0
	while (i < ArrayToSearch.length)
		
		if ArrayToSearch[i].Event_Keyword == KeywordToFind

			GlobalVariable ConditionalGlobal = ArrayToSearch[i].ConditionalGlobal

			if ConditionalGlobal != None ;Make sure the ConditionalGlobal is greater than zero, otherwise keep looking
				if ConditionalGlobal.GetValue() > 0
					RETURN ArrayToSearch[i]
				endif
			else ; there is no ConditionalGlobal so we want this event
				RETURN ArrayToSearch[i]
			endif
		endif

		i += 1
	endwhile

EndFunction


Function TryToModAffinity(EventData EventDataToUse, Globalvariable EventSize)
	;EventSize is a global that contains a float that will scale the value
	;At the moment this function is being written, these are:
	;CA_Size_Large (1.5)
	;CA_Size_Normal (1) - this is the default
	;CA_Size_Small (0.5)
	;CA_Size_CommentOnly(0.0)

	debug.trace(self + "TryToModAffinity() EventDataToUse: " + EventDataToUse + ", EventSize: " + EventSize)

	;debugging variables
	LastAffinityEvent = EventDataToUse.Event_Keyword
	LastAffinityEventSize = EventSize

	float Modify_Value =  GetAffinityModifyValue(EventDataToUse, EventSize)
	
	;assume 0.0 means don't mod affinity
	if Modify_Value == 0
		debug.trace(self + "TryToModAffinity() NOT modding affinity because Modify_Value == 0.0")
		RETURN
	endif

	debug.trace(self + "TryToModAffinity():")
	debug.trace(self + "Will ModAffinity() by: " + Modify_Value )
	
	ModAffinity(Modify_Value)


EndFunction

float function GetAffinityModifyValue(EventData EventDataToUse, Globalvariable EventSize )
	
	float EventSize_Value = EventSize.GetValue()

	if EventSize_Value <= 0
		debug.trace(self + "GetModAffinityValue() returning 0.0 because EventSize_Value <= 0.0")
		Return 0.0
	endif

	float Disposition_Value = EventDataToUse.Disposition_Global.GetValue()
	float Modify_Value =  Disposition_Value * EventSize_Value

	debug.trace(self + "Disposition_Global:" + EventDataToUse.Disposition_Global + " --> " + Disposition_Value)
	debug.trace(self + "EventSize:" + EventSize_Value + " --> " + EventSize_Value)
	debug.trace(self + "returning:" + Modify_Value)

	return Modify_Value

EndFunction


Function TryToSetTraitValues(actorValue TraitToFind, Globalvariable EventSize, bool ShouldAlsoSetAffinity = false) 
	;ShouldAlsoSetAffinity should only be true if we are certain the event isn't bumping affinity by any other means

	actorvalue Trait
	bool Likes ;assumed data is set up such that if the trait is in the array it means they either like or dislike it, so false here means dislikes


	int i = TraitPreference_Array.FindStruct("Trait", TraitToFind)

	if i > -1
		Trait = TraitPreference_Array[i].Trait
		Likes = TraitPreference_Array[i].Likes
	endif

	if Trait
		float Size = EventSize.GetValue()

		;we set the value regardless of if they like it or not. This just tracks how much of the trait the companion has observed
		float newValue = GetValue(Trait) + Size
		SetValue(Trait, newValue)

		if ShouldAlsoSetAffinity && Likes == true
			ModAffinity(Size * TraitPreferenceScaler)

		elseif ShouldAlsoSetAffinity && Likes == false
			ModAffinity( -Size * TraitPreferenceScaler)

		endif

	endif


EndFunction

Function TryToShowMessage(EventData EventDataToUse)

		Message RelatedMessage = EventDataToUse.Message_to_Display
		
		if RelatedMessage
			debug.trace(self + "will call show() on: " + RelatedMessage)
			RelatedMessage.show()

			;If Companion Affinity Tutorial hasn't shown yet, show it
			if !Tutorial.GetStageDone(640)
				Tutorial.SetStage(640)
			endif

		else
			debug.trace(self + "No related Message in EventDataToUse:" + EventDataToUse)
		endif

EndFunction

Function TryToSayCustomTopic(Keyword TopicKeyword, ObjectReference Target = None)
		if TopicKeyword
			debug.trace(self + "will Say(): " + TopicKeyword + ", Target = " + Target)
			SayCustom(TopicKeyword, akTarget = Target)
		endif

EndFunction

Function ModAffinity(float Amount)
	debug.trace(self + "ModAffinity()" + amount)

	if IsDead()
		debug.trace(self + "ModAffinity() IsDead!!! Ignoring call to ModAffinity()")
		RETURN
	endif

	if  HasLeftPlayerPermanently
		debug.trace(self + "ModAffinity() HasLeftPlayerPermanently so ignoring call to ModAffinity()")
		RETURN
	endif

	

	PreviousAffinity = GetValue(Followers.CA_Affinity)

	float NewAffinity = PreviousAffinity + Amount

	;if the companion is losing affinity, make them mad at the player
	if Amount < 0
		SetTemporaryAngerLevel()
	endif

	;CONSTRAIN AFFINITY IF IT GOES PASSED A THRESHOLD ITS NOT ALLOWED TO YET
	If NewAffinity > 0
		;if positive, make sure the NewAffinity isn't higher than the max highest allowed

		ThresholdData NextLockedThreshold = GetNextLockedThreshold(GetNextLockedPositiveThreshold = true)
		Globalvariable NextLockedThresholdGlobal 
		if NextLockedThreshold
			NextLockedThresholdGlobal = NextLockedThreshold.Threshold_Global
		endif
	
		if NextLockedThresholdGlobal && NextLockedThresholdGlobal.GetValue() <= NewAffinity
			NewAffinity = NextLockedThresholdGlobal.GetValue() - 1 ;constrain affinity to best possible, - 1
			debug.trace(self + "NextLockedThresholdValue <= NewAffinity, constraining affinity to: " + NewAffinity)
		endif

	elseif NewAffinity < 0
		;if negative, make sure the NewAffinity isn't lower than the max highest allowed

		ThresholdData NextLockedThreshold = GetNextLockedThreshold(GetNextLockedPositiveThreshold = false)
		Globalvariable NextLockedThresholdGlobal
		if NextLockedThreshold
			NextLockedThresholdGlobal = NextLockedThreshold.Threshold_Global
		endif
		
		if NextLockedThresholdGlobal && NextLockedThresholdGlobal.GetValue() >= NewAffinity
			NewAffinity = NextLockedThresholdGlobal.GetValue() + 1 ;constrain affinity to best possible, +1
			debug.trace(self + "NextLockedThresholdValue >= NewAffinity, constraining affinity to: " + NewAffinity)
		endif

	endif

	;*** CONSTRAIN AFFINITY IF IT'S > MAX or < MIN
	if NewAffinity > MaxAffinity
		NewAffinity = MaxAffinity
		debug.trace(self + "NewAffinity > MaxAffinity, setting NewAffinity to MaxAffinity: " + MaxAffinity)

	elseif NewAffinity < MinAffinity
		NewAffinity = MinAffinity
		debug.trace(self + "NewAffinity > MinAffinity, setting NewAffinity to MinAffinity: " + MinAffinity)

	endif


	ThresholdData ClosestThresholdData = GetClosestThresholdData(NewAffinity, ThresholdData_Array)

	;set tracking variables
	if PreviousAffinity < NewAffinity
		SetValue(Followers.CA_LastChangePositive, 1)
	elseif PreviousAffinity > NewAffinity
		SetValue(Followers.CA_LastChangePositive, 0)
	endif

	;set stage and cue scene if appropriate
	CheckAffinityAndSetThresholdAndStage(NewAffinity, ClosestThresholdData)

	;set tracking variables
	if NewAffinity > GetValue(Followers.CA_HighestReached)
		SetValue(Followers.CA_HighestReached, NewAffinity)
	endif

	if NewAffinity < GetValue(Followers.CA_LowestReached)
		SetValue(Followers.CA_LowestReached, NewAffinity)
	endif

	SetValue(Followers.CA_Affinity, NewAffinity)
	debug.trace(self + "New Affinity =" + NewAffinity)

	TraceAffinityStats()

EndFunction

;most of the time you want to use ModAffinity
Function SetAffinity(float amount)
	;get the difference between current affinity, and the amount you want to set it to, then mod affinity by that amount

	amount = amount - GetValue(Followers.CA_Affinity)

	ModAffinity(amount)

EndFunction

Function TraceAffinityStats()
	debug.trace(self + " AFFINITY STATS:")
	debug.trace("\t Affinity:" + GetValue(Followers.CA_Affinity) )
	debug.trace("\t LastThresholdCrossed: " + LastThresholdCrossed)
	debug.trace("\t PreviousAffinity: " + PreviousAffinity)
	debug.trace("\t LastAffinityChangePositive: " + GetValue(Followers.CA_LastChangePositive))
	debug.trace("\t HighestAffinityReached: " + GetValue(Followers.CA_HighestReached))
	debug.trace("\t LowestAffinityReached: " + GetValue(Followers.CA_LowestReached))
	debug.trace("\t AffinitySceneToPlay: " + GetValue(Followers.CA_AffinitySceneToPlay))
	debug.trace("\t AffinitySceneToPlay (Global): " + AffinitySceneToPlay)

	
EndFunction

Function CheckAffinityAndSetThresholdAndStage(float NewAffinity, ThresholdData ClosestThresholdData)
debug.trace(self + "CheckAffinityAndSetThresholdAndStage() NewAffinity: " + NewAffinity +", ClosestThresholdData:" + ClosestThresholdData)


	if ClosestThresholdData && IsThresholdChanging(ClosestThresholdData.Threshold_Global, NewAffinity)
		debug.trace(self + "CheckAffinityAndSetThresholdAndStage() ClosestThresholdData && IsThresholdChanging() == true ")

		;If it's a Major Threshold, set the Threshold actor values
		if ClosestThresholdData.IsMajorAffinityThreshold
			float NewThresholdValue = ClosestThresholdData.Threshold_Global.GetValue()

			debug.trace(self + "CheckAffinityAndSetThresholdAndStage() IsMajorAffinityThreshold is TRUE so setting CA_CurrentThreshold to " + NewThresholdValue )
			setValue(Followers.CA_CurrentThreshold, NewThresholdValue)

			;set highest/lowest threshold so we can use it in dialogue conditions for lines like "You're doing better, keep up the good work, but I remember when I didn't like you" etc
			if GetValue(Followers.CA_HighestThreshold) < NewThresholdValue
				debug.trace(self + "CheckAffinityAndSetThresholdAndStage() IsMajorAffinityThreshold is TRUE so setting CA_HighestThreshold to " + NewThresholdValue )
				SetValue(Followers.CA_HighestThreshold, NewThresholdValue)

			elseif GetValue(Followers.CA_LowestThreshold) > NewThresholdValue
				debug.trace(self + "CheckAffinityAndSetThresholdAndStage() IsMajorAffinityThreshold is TRUE so setting CA_LowestThreshold to " + NewThresholdValue )
				SetValue(Followers.CA_LowestThreshold, NewThresholdValue)

			endif

		else
			debug.trace(self + "CheckAffinityAndSetThresholdAndStage() IsMajorAffinityThreshold is FALSE so NOT setting CA_CurrentThreshold, nor CA_HighestThreshold, nor CA_LowestThreshold")

		endif


		AddThresholdToQueueAndSetSceneToPlay(ClosestThresholdData, NewAffinity)

		;set quest stage
		setStageBasedOnThresholdData(ClosestThresholdData)

		if ClosestThresholdData.ThresholdMessage && IsDead() == false
			ClosestThresholdData.ThresholdMessage.Show()
		else
			Game.Warning(self + "CheckAffinityAndSetThresholdAndStage() couldn't find a message for " + ClosestThresholdData)
		endif

	endif

EndFunction

Function AddThresholdToQueueAndSetSceneToPlay(ThresholdData ThresholdToQueue, float NewAffinity)
	;Queues up thresholds so we make sure to get the earlier scenes before the later scenes

	debug.trace(self + "AddThresholdToQueueAndSetSceneToPlay() " + ThresholdToQueue)

	;remove any non-relevant thresholds (negative thresholds when affinity is positve, etc.)
	int i = 0
	while (i < QueuedThresholdData_Array.length)
		bool removeCurrent

		float ThresholdGlobalValue = QueuedThresholdData_Array[i].Threshold_Global.GetValue()

		;if affinity is postive
		if NewAffinity >= 0
			;and if threshold is higher than current affinity, or threshold is negative
			if ThresholdGlobalValue > NewAffinity || ThresholdGlobalValue <= 0
				;then remove threshold from queue
				removeCurrent = true
			endif

		else ;if affinity is negative
			;and if threshold is lower than current affinity, or threshold is positive
			if ThresholdGlobalValue < NewAffinity || ThresholdGlobalValue > 0
				;then remove threshold from queue
				removeCurrent = true
			endif

		endif

		if removeCurrent 
			QueuedThresholdData_Array.Remove(i)
		else
			i += 1
		endif

	endwhile


	bool addToQueue

	;***** Make sure if the stage has already been reached, there is a scene to play, otherwise skip it
	if ThresholdToQueue.ThresholdHasBeenPreviouslyReached
		debug.trace(self + "AddThresholdToQueueAndSetSceneToPlay() ThresholdHasBeenPreviouslyReached, will check to make sure we have repeating scenes before adding to the queue." )

		if GetValue(Followers.CA_LastChangePositive) == 1
			;check to see if we have an upward repeating affinity scene and set actor value if we do
			if ThresholdToQueue.AffinitySceneToPlay_Repeat_GoingUp
				addToQueue = true
			else
				debug.trace(self + "AddThresholdToQueueAndSetSceneToPlay() NOT adding to queue because found no AffinitySceneToPlay_Repeat_GoingUp" )
			endif

		elseif ThresholdToQueue.AffinitySceneToPlay_Repeat_GoingDown
			addToQueue = true
		else
			debug.trace(self + "AddThresholdToQueueAndSetSceneToPlay() NOT adding to queue because found no AffinitySceneToPlay_Repeat_GoingDown" )
		endif
	else
		addToQueue = true
	endif

	;if it's not already in there, add it:
	if addToQueue && QueuedThresholdData_Array.find(ThresholdToQueue) < 0
		QueuedThresholdData_Array.add(ThresholdToQueue)
	endif

	;set the CA_AffinitySceneToPlay to the first thing in the array
	SetQueuedThresholdSceneToPlay()

EndFunction

;*** SHOULD BE CALLED WHEN AFFINITY SCENES END ***
Function FinishedSceneRemoveThresholdFromQueueAndSetSceneToPlay(globalVariable RelatedThreshold_Global)

	debug.trace(self + "SETTING CA_WantsToTalk to 0 IN FinishedSceneRemoveThresholdFromQueueAndSetSceneToPlay() RelatedThreshold_Global: " + RelatedThreshold_Global)
	SetValue(Followers.CA_WantsToTalk, 0)
	EvaluatePackage()
	
	;remove the thresdhold from the Queue
	int index = QueuedThresholdData_Array.findStruct("Threshold_Global", RelatedThreshold_Global)

	if index < 0
		Game.Warning(self + "FinishedSceneRemoveThresholdFromQueueAndSetSceneToPlay() couldn't find " + RelatedThreshold_Global)

	else
		;flag as having set this theshold once before
		QueuedThresholdData_Array[index].ThresholdHasBeenPreviouslyReached = true

		;remove from Queue
		QueuedThresholdData_Array.Remove(index)

	endif

	;set the CA_AffinitySceneToPlay to the first thing in the array - in case there's anything still in there
	SetQueuedThresholdSceneToPlay()

EndFunction

Function SetQueuedThresholdSceneToPlay()
	if QueuedThresholdData_Array.length == 0
		debug.trace(self + "SetQueuedThresholdSceneToPlay() setting wants to talk to 0 and BAILING OUT because QueuedThresholdData is empty.")
		setValue(Followers.CA_WantsToTalk, 0)
		RETURN

	else

		ThresholdData QueuedThresholdData = QueuedThresholdData_Array[0]

		;set variables used by Package and Greeting conditions to start scenes:
		if QueuedThresholdData.ThresholdHasBeenPreviouslyReached
			if GetValue(Followers.CA_LastChangePositive) == 1
				;check to see if we have an upward repeating affinity scene and set actor value if we do
				if QueuedThresholdData.AffinitySceneToPlay_Repeat_GoingUp
					AffinitySceneToPlay = QueuedThresholdData.AffinitySceneToPlay_Repeat_GoingUp
					debug.trace(self + "SetQueuedThresholdSceneToPlay()  setting CA_AffinitySceneToPlay to " + AffinitySceneToPlay.GetValue())
					setvalue(Followers.CA_AffinitySceneToPlay, AffinitySceneToPlay.GetValue())
					debug.trace(self + "SETTING CA_WantsToTalk to 1 IN SetQueuedThresholdSceneToPlay()")
					setValue(Followers.CA_WantsToTalk, 1)
					

				endif
			else

				;check to see if we have a downward repeating affinity scene and set actor value if we do
				if QueuedThresholdData.AffinitySceneToPlay_Repeat_GoingDown
					AffinitySceneToPlay = QueuedThresholdData.AffinitySceneToPlay_Repeat_GoingDown
					debug.trace(self + "SetQueuedThresholdSceneToPlay()  setting CA_AffinitySceneToPlay to " + AffinitySceneToPlay.GetValue())
					setvalue(Followers.CA_AffinitySceneToPlay, AffinitySceneToPlay.GetValue())
					debug.trace(self + "SETTING CA_WantsToTalk to 1 IN SetQueuedThresholdSceneToPlay()")
					setValue(Followers.CA_WantsToTalk, 1)
					

				endif
			endif
		
		else ;Threshold has NOT been previously reached:
			if QueuedThresholdData.AffinitySceneToPlay_FirstTimeReached
				AffinitySceneToPlay = QueuedThresholdData.AffinitySceneToPlay_FirstTimeReached
				debug.trace(self + "SetQueuedThresholdSceneToPlay()  setting CA_AffinitySceneToPlay to " + AffinitySceneToPlay.GetValue())
				setvalue(Followers.CA_AffinitySceneToPlay, AffinitySceneToPlay.GetValue())
				debug.trace(self + "SETTING CA_WantsToTalk to 1 IN SetQueuedThresholdSceneToPlay()")
				setValue(Followers.CA_WantsToTalk, 1)
				

			endif
		endif

	endif

	

EndFunction



ThresholdData Function GetClosestThresholdData(float NewAffinity, ThresholdData[] ThresholdDataArray)
	ThresholdData ClosestThresholdData

	if NewAffinity >= 0
		;Reminder:Thresholds is ordered Highest to Lowest, and contains globalVariables
		int i = 0
		while (i < ThresholdDataArray.length && !ClosestThresholdData)
			if ThresholdDataArray[i].Threshold_Global.GetValue() <= NewAffinity
				ClosestThresholdData = ThresholdDataArray[i]
			endif
			i += 1
		endwhile
	
	elseif NewAffinity < 0
		;Reminder:ThresholdDataArray is ordered Highest to Lowest, and contains globalVariables
		int i = ThresholdDataArray.length - 1  
		while (i >= 0 && !ClosestThresholdData)
			if NewAffinity <= ThresholdDataArray[i].Threshold_Global.GetValue()
				;make sure we are less than zero -- this prevents Neutral(0) AT exactly 0
				if ThresholdDataArray[i].Threshold_Global.GetValue() < 0
					ClosestThresholdData = ThresholdDataArray[i]
				endif
			endif

			i -= 1
		endwhile

	endif

	return ClosestThresholdData

EndFunction

ThresholdData Function GetNextThreshold(bool GetNextInThePositiveDirection = true)
	;gets the next threshold from the current affinity level. Can specify next in either positive or negative direction.

	;loop through find the closest threshold on either side of the current affinity, then return the one based on the param

	float currentAffinity = GetValue(Followers.CA_Affinity)

	ThresholdData NextThreshold
	
	if GetNextInThePositiveDirection

		;reminder ThresholdData_Array is sorted highest to lowest.
		if ThresholdData_Array[0].Threshold_Global.GetValue() <= currentAffinity
			;current affinity is equal or higher than the best affinity threshold
			NextThreshold = ThresholdData_Array[0]
		endif

		;loop from bottom of array to top, looking for the first threshold that is higher than currentaffinity
		int i = ThresholdData_Array.length - 1
		while (NextThreshold == NONE && i >= 0 )
			if ThresholdData_Array[i].Threshold_Global.GetValue() > currentAffinity 
				NextThreshold = ThresholdData_Array[i]
			endif
			i -= 1
		endwhile

	else ;we want the next one in the negative direction

		if currentAffinity <= ThresholdData_Array[ThresholdData_Array.length - 1].Threshold_Global.GetValue()
			;current affinity is less than or equal to the worst affinity threshold
			NextThreshold = ThresholdData_Array[ThresholdData_Array.length - 1]
		endif

		;loop from top of array to bottom, looking for the first threshold that is lower than the currentaffinity
		int i = 0
		while (NextThreshold == NONE && i < ThresholdData_Array.length)
			if ThresholdData_Array[i].Threshold_Global.GetValue() < currentAffinity
				NextThreshold = ThresholdData_Array[i]
			endif
			i += 1
		endwhile

	endif

	debug.trace(self + "GetNextThreshold():" + NextThreshold)

	return NextThreshold

EndFunction

ThresholdData Function GetNextLockedThreshold(bool GetNextLockedPositiveThreshold = true)
	;gets the next highest/lowest threshold that is not allowed (meaning doesn't have a ConditionalGlobal or it's ConditionalGlobal is > 0)
	;It' getting the first thing in here whose ConditionalGlobal isn't set yet. Once that's set, then it'll be allowed to go passed that, so the next time you call the function it'll get the "next" locked one.


	ThresholdData NextLockedThreshold = None
	
	if GetNextLockedPositiveThreshold

		;loop from bottom of array to top (ignore anything negative)
		int i = ThresholdData_Array.length - 1
		while (i >= 0 && NextLockedThreshold == none)

			ThresholdData CurrentThresholdData = ThresholdData_Array[i]
			GlobalVariable ConditionalGlobal = CurrentThresholdData.ConditionalGlobal

			;make sure it's positive, and the ConditionalGlobal is set and <= 0
			if CurrentThresholdData.Threshold_Global.GetValue() > 0 && (ConditionalGlobal && ConditionalGlobal.GetValue() <= 0)
				NextLockedThreshold = CurrentThresholdData
			endif

			i -= 1
		endwhile

	else 

		;loop from top of array to bottom (ignore anything positive)
		int i = 0
		while (i < ThresholdData_Array.length && NextLockedThreshold == none)

			ThresholdData CurrentThresholdData = ThresholdData_Array[i]
			GlobalVariable ConditionalGlobal = CurrentThresholdData.ConditionalGlobal

			;make sure it's negative, and the ConditionalGlobal is set and <= 0
			if CurrentThresholdData.Threshold_Global.GetValue() < 0 && (ConditionalGlobal && ConditionalGlobal.GetValue() <= 0)
				NextLockedThreshold = CurrentThresholdData
			endif

			i += 1
		endwhile

	endif

	debug.trace(self + "GetNextLockedThreshold():" + NextLockedThreshold)

	return NextLockedThreshold

EndFunction

bool Function IsThresholdChanging(GlobalVariable ClosestThreshold, float NewAffinity)

	if ClosestThreshold == None
		Game.Warning(self + "WARNING: IsThresholdChanging() ClosestThreshold == None")
		RETURN FALSE
	endif

	;if ClosestThreshold to the current Affinity is the same as the LastThresholdCrossed (we have stored when we crossed that threshold) it means we haven't gone past a new threshold yet, so do nothing
	;also, if a big enough change hasn't happened between the last threshold we set and the current affinity, don't do anything - this prevents slight changes of + and - affinity won't seesaw the compnaion across  an affinity threshold

	if ClosestThreshold != LastThresholdCrossed && math.abs(LastThresholdCrossed.getValue() - NewAffinity) > MinAffinityChangeFromLastThresholdCrossedRequired  
		LastThresholdCrossed = ClosestThreshold
		debug.trace(self + "IsThresholdChanging() " + ClosestThreshold + " IS CHANGING")
		RETURN TRUE
	else
		debug.trace(self + "IsThresholdChanging() " + ClosestThreshold + " IS *NOT* CHANGING")
		debug.trace(self + "ClosestThreshold: " + ClosestThreshold)
		debug.trace(self + "LastThresholdCrossed: " + LastThresholdCrossed)
		debug.trace(self + "math.abs(LastThresholdCrossed.getValue() - NewAffinity): " + math.abs(LastThresholdCrossed.getValue() - NewAffinity))
		debug.trace(self + "MinAffinityChangeFromLastThresholdCrossedRequired: " + MinAffinityChangeFromLastThresholdCrossedRequired)

		RETURN FALSE
	endif


EndFunction


Function setStageBasedOnThresholdData(ThresholdData ThresholdDataToUse)

	if ThresholdDataToUse.Controlling_Quest == None || ThresholdDataToUse.Controlling_Quest_Stage < 0
		debug.trace(self + "setStageBasedOnThresholdData() Controlling Quest is empty or Controlling_Quest_Stage < 1 so we are assuming we should not set a stage.")
		RETURN
	endif

	debug.trace(self + "setStageBasedOnThresholdData() setting stage: " + ThresholdDataToUse.Controlling_Quest_Stage + " in quest: " + ThresholdDataToUse.Controlling_Quest)

	ThresholdDataToUse.Controlling_Quest.SetStage(ThresholdDataToUse.Controlling_Quest_Stage)

EndFunction


Function SetAffinityBetweenThresholds(GlobalVariable ThresholdA, GlobalVariable ThresholdB)
	
	float A = ThresholdA.GetValue()
	float B = ThresholdB.GetValue()

	float SetAffinityTo = A - ((A-B)/2)

	Debug.Trace(self + "SetAffinityBetweenThresholds() A:" + A + ", B:" + B + "; SetAffinityTo: " + SetAffinityTo)

	SetAffinity(SetAffinityTo)
	
EndFunction

Function SetAffinityTowardNextLowestThreshold(float percentageBetweenCurrentAffinityValueAndNextLowestThreshold = 0.50)
	
	float currentAffinity = GetValue(Followers.CA_Affinity)
	float SetAffinityTo = currentAffinity - ((currentAffinity - GetNextThreshold(False).Threshold_Global.GetValue()) * percentageBetweenCurrentAffinityValueAndNextLowestThreshold)

	Debug.Trace(self + "SetAffinityTowardNextLowestThreshold() SetAffinityTo: " + SetAffinityTo)

	SetAffinity(SetAffinityTo)

EndFunction


;called by Infatuation and Romance scenes
Function RomanceSuccess()
	setValue(Followers.CA_IsRomantic, 1)
	setValue(Followers.CA_IsRomanceableNow, 0)
	SetValue(Followers.CA_WantsToTalk, 0)
	SetValue(Followers.CA_WantsToTalkRomanceRetry, 0)
	EvaluatePackage()

	;Fire off sleeping with romantic companion tutorial if previously unfired
	if !Tutorial.GetStageDone(680)
		Tutorial.SetStage(680)
	endif

EndFunction

;called by Infatuation and Romance scenes
Function RomanceFail()
	setValue(Followers.CA_IsRomanceableNow, 1)
	SetValue(Followers.CA_WantsToTalk, 0)
	SetValue(Followers.CA_WantsToTalkRomanceRetry, 0)
	EvaluatePackage()
	StartRomanceTalkTimer(TimerInterval_WantsToTalkRomance_TriedAndFailed)
EndFunction

;called by Infatuation and Romance scenes
Function RomanceDeclined(bool isPermanent = false)
	SetValue(Followers.CA_WantsToTalk, 0)
	SetValue(Followers.CA_WantsToTalkRomanceRetry, 0)
	EvaluatePackage()

	if isPermanent == false
		setValue(Followers.CA_IsRomanceableNow, 1)
		StartRomanceTalkTimer(TimerInterval_WantsToTalkRomance_Declined)

	else ;is permanent 
		setValue(Followers.CA_IsRomanceableNow, -1)
	endif

EndFunction

function StartRomanceTalkTimer(float interval)
	startTimerGameTime(interval, TimerID_WantsToTalkRomance)
EndFunction

Function SetTemporaryAngerLevel(bool isReallyAngry = false)
;sets the CA_TemporaryAngerLevel actor value to 1, and starts a time to clear it.
;isReallyAngry == true means set TemporaryAngerLevel to 2 and use longer timer inteval

	debug.trace(self + "SetTemporaryAngerLevel()")

	if isReallyAngry == false
		SetValue(TemporaryAngerLevel, 1)
		startTimerGameTime(TimerInterval_TemporaryAngerLevel, TimerID_TemporaryAngerLevel)
	else
		SetValue(TemporaryAngerLevel, 2)
		startTimerGameTime(TimerInterval_TemporaryAngerLevel_ReallyMad, TimerID_TemporaryAngerLevel )
	endif

EndFunction

Function SetHasLeftPlayerPermanently()
	HasLeftPlayerPermanently = True
EndFunction


;added for DLC01 for Codsworth to be quest giver if he hates player
Function UnsetHasLeftPlayerPermanently()
	HasLeftPlayerPermanently = False
EndFunction

bool function isMurder(actor ActorVictim, bool GameConsidersMurder)

	bool returnVal = false

	;if game considers it murder, double check to see I have different feelings
	if GameConsidersMurder 

		int i = 0
		int size = ConsideredMurderFactionList.GetSize() 
		while (i < size && returnVal == false)
			if ActorVictim.IsInFaction(ConsideredMurderFactionList.GetAt(i) as faction)
				returnVal = true
				debug.trace(self + "IsMurder found actor in ConsideredMurderFactionList faction:" + ConsideredMurderFactionList.GetAt(i))
			endif
			
			i += 1
		endwhile

		;unless actor is in a faction that's in the NotConsideredMurder_Array	
		i = 0
		while (i < NotConsideredMurder_Array.length && returnVal != false)
			if ActorVictim.IsInFaction(NotConsideredMurder_Array[i])
				returnVal = False
				debug.trace(self + "IsMurder found actor in NotConsideredMurder_Array faction:" + NotConsideredMurder_Array[i])
			endif

			i += 1
		endwhile

	endif

	RETURN returnVal

EndFunction


Function MurderSession(Actor ActorVictim, Actor ActorKiller)

	CancelTimerGameTime(TimerID_MurderSession) ;so it doesn't end if we are just about to start it again

	if ActiveMurderSessionOnGoing == false
		
		ActiveMurderSessionOnGoing = true

		debug.trace(self + "MURDER SESSION STARTED!!! ActorVictim:" + ActorVictim + ", ActorKiller:" + ActorKiller)

		NewMurderSession()


	else
		debug.trace(self + "MURDER SESSION ONGOING... ActorVictim:" + ActorVictim + ", ActorKiller:" + ActorKiller)

		SetValue(Followers.CA_MurderSessionVictimCount, GetValue(Followers.CA_MurderSessionVictimCount) + 1)	 ;*increase* the victim count - using setValue() instead of modValue() so I can later restart it my setting it to 1

		;if the victim count exceeds that amount allowed for the current murder session, bump up to the next session
		int murderSessionCount = GetValue(Followers.CA_MurderSessionCount) as int

		if MurderThreshold_Array.length < murderSessionCount
			murderSessionCount = MurderThreshold_Array.length - 1
		endif 


		if  MurderThreshold_Array[murderSessionCount].MaxVictimsAllowed < GetValue(Followers.CA_MurderSessionVictimCount)

			NewMurderSession()
		endif


	endif

	SetTemporaryAngerLevel(isReallyAngry = true)

	startTimerGameTime(TimerInterval_MurderSession, TimerID_MurderSession)

EndFunction

Function EndMurderSession()
	debug.trace(self + "MURDER SESSION ENDED.")
	ActiveMurderSessionOnGoing = false

EndFunction

Function NewMurderSession()
		FollowersScript.SendAffinityEvent(self, CA_Event_Murder)

		SetValue(Followers.CA_MurderSessionCount, GetValue(Followers.CA_MurderSessionCount) + 1)  ;*increase* the number of murder sessions witnessed
		SetValue(Followers.CA_MurderSessionVictimCount, 1)	 ;*set* the victim count (start it over - it holds the victim count of the most recent murder session)
		SetValue(Followers.CA_MurderSessionDay, Utility.GetCurrentGameTime()) ;*set* the current game time (it holds the start time of the most recent murder session)

		ProcessMurderThreshold(GetValue(Followers.CA_MurderSessionCount) as int - 1) ;-1 because it's used for a 0 based index

EndFunction

Function ProcessMurderThreshold(int Threshold)

	debug.trace(self + "ProcessMurderThreshold(): " + threshold)

	if MurderThreshold_Array.length < Threshold 
		debug.trace(self + "ProcessMurderThreshold() Doing nothing. found Threshold higher than total allowed.", 1)
		RETURN
	endif
	
	ModAffinity(MurderThreshold_Array[Threshold].AffinityPenalty)
	
	Quest QuestToSet = MurderThreshold_Array[Threshold].QuestToSet
	int StageToSet = MurderThreshold_Array[Threshold].StageToSet

	if QuestToSet && StageToSet > 0
		QuestToSet.SetStage(StageToSet)

	else
		debug.trace(self + "CAUTION: ProcessMurderThreshold found missing data for QuestToSet or StageToSet.", 1)

	endif

EndFunction

;NOTE: there are various quest stages that set this value directly
;Not sure if these functions are being called
Function AffinitySceneForceGreeted(Actor ActorToSet) Global
	debug.trace("CompanionActorScript SETTING CA_WantsToTalk to 2 IN AffinitySceneForceGreeted()")
	ActorToSet.Setvalue(FollowersScript.GetScript().CA_WantsToTalk, 2)
EndFunction

Function AffinitySceneFinished(Actor ActorToSet) Global
	debug.trace("CompanionActorScript SETTING CA_WantsToTalk to 0 IN AffinitySceneFinished()")
	ActorToSet.Setvalue(FollowersScript.GetScript().CA_WantsToTalk, 0)
EndFunction

Function HandleHasItemForPlayerTimer()
	debug.trace(self + "HandleHasItemForPlayerTimer()")

	SetValue(HasItemForPlayer, 1)	
EndFunction

Function StartHasItemTimer()
	debug.trace(self + "StartHasItemTimer()")

	if ShouldGivePlayerItems
		float nextTime = Utility.RandomFloat(TimerIntervalMin_HasItemForPlayer, TimerIntervalMax_HasItemForPlayer)
		startTimerGameTime(nextTime, TimerID_HasItemForPlayer)
	else
		Game.Error(self + "StartHasItemTimer() called but ShouldGivePlayerItems == false")
	endif
EndFunction

Function GivePlayerItem()
	debug.trace(self + "GivePlayerItem()")
	Game.GetPlayer().AddItem(ItemToGive)
	SetValue(HasItemForPlayer, 0)	

	StartHasItemTimer()
EndFunction

Function SetLastDialogueBump(GlobalVariable LastDialogueBumpToSet)
	debug.trace(self + "SetLastDialogueBump() " + LastDialogueBumpToSet)
	
	LastDialogueBump = LastDialogueBumpToSet  ;human readable debugging variable

EndFunction

Function SayLastDialogueBumpLine()
	debug.trace(self + "SayLastDialogueBumpLine() ... LastDialogueBump:" + LastDialogueBump)

	if LastDialogueBump == Followers.CA_DialogueBump_Dislike
		SayCustom(Followers.CAT_DialogueBump_Disliked)
		debug.trace(self + "SayLastDialogueBumpLine() saying: " + Followers.CAT_DialogueBump_Disliked)

	elseif LastDialogueBump == Followers.CA_DialogueBump_Hate
		SayCustom(Followers.CAT_DialogueBump_Hated)
		debug.trace(self + "SayLastDialogueBumpLine() saying: " + Followers.CAT_DialogueBump_Hated)
	
	elseif LastDialogueBump == Followers.CA_DialogueBump_Like
		SayCustom(Followers.CAT_DialogueBump_Liked)
		debug.trace(self + "SayLastDialogueBumpLine() saying: " + Followers.CA_DialogueBump_Like)
	
	elseif LastDialogueBump == Followers.CA_DialogueBump_Love
		SayCustom(Followers.CAT_DialogueBump_Loved)
		debug.trace(self + "SayLastDialogueBumpLine() saying: " + Followers.CA_DialogueBump_Love)
	
	endif

	;clear variable
	LastDialogueBump = None
	

EndFunction

bool Function TestEventForAffinityBump(Keyword EventToTest, GlobalVariable ThresholdToTestFor = none)
	;if ThresholdToTestFor == None, then return true if it would change to any threshold

	debug.trace(self + "TestEventForAffinityBump() EventToTest: " + EventToTest + ", ThresholdToTestFor: " + ThresholdToTestFor)

	EventData AffinityEventData = GetEventDataByKeyword(EventData_Array, EventToTest)

	if !AffinityEventData ;not found
		debug.trace(self + "TestEventForAffinityBump() did not find event keyword: " + EventToTest + " this might be a 'comment only' type event", 1)
		RETURN False

	else

		FollowersScript:AffinityEventData AffinityEventDataToUse  = FollowersScript.GetAffinityEventData(EventToTest)
		;FollowersScript:AffinityEventData holds among other things the standard size of the event.
		;See the AffinityEventData struct definition in FollowersScript for more details

		float Modify_Value =  GetAffinityModifyValue(AffinityEventData, AffinityEventDataToUse.EventSize)

		;*** test if modify value is enought to get to thresholdtotestfor
		RETURN TestValueForAffinityBump(Modify_Value, ThresholdToTestFor)

	endif


EndFunction

bool Function TestValueForAffinityBump(float ModifyValueToTest, GlobalVariable ThresholdToTestFor = none)

	debug.trace(self + "TestEventForAffinityBump() ModifyValueToTest: " + ModifyValueToTest + ", ThresholdToTestFor: " + ThresholdToTestFor)
	

	;*ASSUME* if ModifyValueToTest is postive we are looking for the next highest threshold, and if negative the next lowest
	bool IsDirectionPostive = ModifyValueToTest >= 0

	if ThresholdToTestFor == None ;then get whatever the next one is
		ThresholdToTestFor = GetNextThreshold(IsDirectionPostive).Threshold_Global
	endif 

	float CurrentAffinity = GetValue(Game.GetCommonProperties().CA_Affinity)
	float WouldBeAffinity = CurrentAffinity + ModifyValueToTest
	float ThresholdValue = ThresholdToTestFor.GetValue()

	debug.trace(self + "TestValueForAffinityBump() ModifyValueToTest: " + ModifyValueToTest + ", ThresholdToTestFor: " + ThresholdToTestFor + "(ThresholdValue: " + ThresholdValue + ")")
	debug.trace(self + "CurrentAffinity: " + CurrentAffinity + " + ModifyValueToTest: " + ModifyValueToTest + " == WouldBeAffinity: " + WouldBeAffinity)

	if IsDirectionPostive == true
		if  WouldBeAffinity >= ThresholdValue
			debug.trace(self + "TestValueForAffinityBump returning TRUE because IsDirectionPostive == true and WouldBeAffinity >= ThresholdValue")
			RETURN true
		else
			debug.trace(self + "TestValueForAffinityBump returning FALSE because IsDirectionPostive == true and WouldBeAffinity < ThresholdValue")
			RETURN false
		Endif
	
	else ;IsDirectionPostive == false, in otherwords we are assuming we want to see if this is a more negative number than the threshold we are testing for
		if  WouldBeAffinity <= ThresholdValue
			debug.trace(self + "TestValueForAffinityBump returning TRUE because IsDirectionPostive == false and WouldBeAffinity <= ThresholdValue")
			RETURN true
		else
			debug.trace(self + "TestValueForAffinityBump returning FALSE because IsDirectionPostive == false and WouldBeAffinity > ThresholdValue")
			RETURN false
		Endif

	endif


EndFunction


ThresholdData Function GetThresholdDataForThresholdGlobal(GlobalVariable ThresholdGlobalToFind)
	debug.trace(self + "GetThresholdDataForThresholdGlobal() looking for " + ThresholdGlobalToFind)

	int i = ThresholdData_Array.FindStruct("Threshold_Global", ThresholdGlobalToFind)

	if i >= 0
		debug.trace(self + "GetThresholdDataForThresholdGlobal() RETURNING " + ThresholdData_Array[i])		
		RETURN ThresholdData_Array[i]

	else
		debug.trace(self + "GetThresholdDataForThresholdGlobal() could not find ThresholdData for " + ThresholdGlobalToFind)
		RETURN None

	endif

EndFunction

Function ShowThresholdMessage(GlobalVariable ThresholdGlobal)
	debug.trace(self + "ShowThresholdMessage() ThresholdGlobal: " + ThresholdGlobal)
	ThresholdData foundThresholdData = GetThresholdDataForThresholdGlobal(ThresholdGlobal)

	if foundThresholdData
		if foundThresholdData.ThresholdMessage
			foundThresholdData.ThresholdMessage.Show()
		else
			game.Warning(self + "ShowThresholdMessage() didn't have ThresholdMessage in " + foundThresholdData)
		endif
	else
		Game.Warning(self + "ShowThresholdMessage() couldn't find threshold data for: " + ThresholdGlobal)
	endif

EndFunction

Function ShowRomanticInfatuationMessage()
	InfatuationRomanticMessage.show()
EndFunction

Function UnlockInfatuationPerk()
	debug.trace(self + "UnlockInfationPerk(): " + InfatuationPerk)
	Debug.TraceStack()

	if InfatuationPerkUnlocked == false
		Game.GetPlayer().AddPerk(InfatuationPerk)
		InfatuationPerkMessage.Show()
		Followers.Achievements.SetStage(330)
	endif

	InfatuationPerkUnlocked = true
EndFunction