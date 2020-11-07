Scriptname AOScript extends Quest
{Temporary script to hold properties common to Attraction Object system}



formlist Property TempAOList const auto
{OBSOLETE? list of Objects that would be flagged as "Is Stim Object" in the real system}

keyword Property TempAOStart const auto
{OBSOLETE? keyword story manager is looking for}

keyword[] Property TempAOKeywords const auto
{OBSOLETE? List of "known" StimTypeXXX keywords... assists this script in checking if actor and object have matching keywords}

keyword Property AOKeywordForLockDoors const auto
{Set to: AO_Type_LockPick -- needed because doors don't have keywords (yet), we will assume if actor has this keyword, he will be interested in doors}

keyword Property AO_Finished const auto
{Keyword we apply to objects to mean we are finished with the object, don't pick it again for AO quests}

;NOTE: Properties for Dogmeat AO stuff is found below in that section of the script


;timer for the standard "ping" to the story manager
int property iTimerID_Dogmeat_Find = 1 const auto hidden 			;timer id
float property TimerInterval_Dogmeat_Find = 5.0 const auto hidden 	;in real time seconds

int property iTimerID_Companion_Search = 2 const auto hidden 			;timer id
float property TimerInterval_Companion_Search = 2.0 const auto hidden 	;in real time seconds


Event OnInit()
	debug.trace(self + "OnInit()")

	AO_Dogmeat_Find_Allowed.setValue(1)
	AO_Companion_Search_Allowed.setValue(1)

	RegisterForRemoteEvent(DogmeatCompanion, "OnLoad") 	;NOTE: it is highly unlikely but possible that DogmeatLoading() could be called twice - this shouldn't cause a problem, but just FYI future self

	ObjectReference DogmeatRef = DogmeatCompanion.GetReference()

	if DogmeatRef && DogmeatRef.is3DLoaded() ;in case we init after he loads
		debug.trace(self + "OnInit() DogmeatCompanion is already loaded, so calling DogmeatLoading()") ;this is currently impossible, but you never know
		DogmeatLoading(true)
	endif

	;used to toggle dogmeat AO script events
	RegisterForRemoteEvent(DogmeatCompanion, "OnUnload")

	RegisterForCustomEvent(FollowersScript.GetScript(), "Loitering")

EndEvent


Event OnTimer(int aiTimerID)
	debug.trace("AOScript OnTimer() aiTimerID: " + aiTimerID)

	if aiTimerID == iTimerID_Dogmeat_Find
		HandleTimer_DogmeatFind()

	elseif aiTimerID == iTimerID_Companion_Search
		HandleTimer_Companion_Search()
	endif

EndEvent

AOScript Function GetScript() Global
	return Game.GetFormFromFile(0x000260DA, "Fallout4.esm") as AOScript
	
EndFunction

Function FlagObjectFinished(ObjectReference ObjectToFlagWithKeyword) global
	debug.trace("Adding AO_Finished keyword to:" + ObjectToFlagWithKeyword)
	ObjectToFlagWithKeyword.addKeyword(GetScript().AO_Finished)
EndFunction

Function FlagObjectAliasFinished(ReferenceAlias AliasToFlagWithKeyword) global
	debug.trace("Adding AO_Finished keyword to:" + AliasToFlagWithKeyword)
	FlagObjectFinished(AliasToFlagWithKeyword.GetReference())
EndFunction


;*******************************************************************************************************
;**************************************	 Companion Search (AO Searches)	********************************
;*******************************************************************************************************

Group CompanionProperties

ReferenceAlias Property Companion const auto
{Linked to Companion in Followers quest}

globalvariable Property AO_Companion_Search_Allowed const auto
{Used as condition in story manager}

keyword Property AO_Companion_Search_Start const auto

EndGroup

Event FollowersScript.Loitering(FollowersScript akSender, Var[] args)
	;debug.trace(self + "Event FollowersScript.Loitering() " + args)


	bool isNowLoitering = args[0] as bool
	ReferenceAlias CompanionAlias = args[1] as ReferenceAlias

	if isNowLoitering && CompanionAlias.GetReference() ;player has started loitering
		startTimer(TimerInterval_Companion_Search, iTimerID_Companion_Search)
	else ;player has stopped loitering
		cancelTimer(iTimerID_Companion_Search)
	endif
EndEvent

Function HandleTimer_Companion_Search()

	Actor CompanionActor = Companion.GetActorReference()

	if CompanionActor

		debug.trace(self + "HandleTimer_Companion_Search() sending StoryManagerEvent()" + AO_Companion_Search_Start + CompanionActor)
		AO_Companion_Search_Start.SendStoryEventAndWait(akRef1 = CompanionActor, akLoc = CompanionActor.GetCurrentLocation())  ;"AndWait" just to make sure we don't spam the story manager

	endif

	startTimer(TimerInterval_Companion_Search, iTimerID_Companion_Search)
	
EndFunction



;*******************************************************************************************************
;**************************************	 DOGMEAT FIND STUFF (AO Finds)	********************************
;*******************************************************************************************************

;/
	Simulates long range Attraction Object style events. Sends out a story manager event every so often
	that fills aliases and does logic checking in the quest, then fires off scenes that handle behavior.
	Those quests and keywords are prefixed with the AO_ since they do largely the same type
	of thing.

/;

Struct AO_Dogmeat_Find_Datum
	globalvariable Type 
	globalvariable NextAllowed
	globalvariable CoolDown
EndStruct

Group DogmeatFind

globalvariable Property AO_Dogmeat_Find_Allowed const auto
{Used as condition in story manager}

ReferenceAlias Property DogmeatCompanion const auto
{Linked to DogmeatCompanion in Followers quest}

keyword Property AO_Dogmeat_Find_Start const auto
keyword Property AO_Dogmeat_FindContainer_Commanded_Start const auto
keyword Property AO_Dogmeat_FindHostile_Commanded_Start const auto
keyword Property AO_Dogmeat_FindItem_Commanded_Start const auto


AO_Dogmeat_Find_Datum[] Property AO_Dogmeat_Find_Data const auto
{Type filter: "AO_Dogmeat_FindType_"
NextAllowed filter: "AO_Dogmeat_FindNextAllowed_"
}

EndGroup

float MaxDogmeatDistanceForAOEvent = 1500.0

Event ReferenceAlias.OnLoad(ReferenceAlias akSender)
	debug.trace(self + "Receiving OnLoad event for " + akSender)
	if akSender == DogmeatCompanion
		DogmeatLoading(true)
	endif
EndEvent

Event ReferenceAlias.OnUnload(ReferenceAlias akSender)
	debug.trace(self + "Receiving OnUnload event for " + akSender)
	if akSender == DogmeatCompanion
		DogmeatLoading(false)
	endif
EndEvent

Function DogmeatLoading(bool isDogmeatLoading = true) ;false = unloading
	if isDogmeatLoading
		startTimer(TimerInterval_Dogmeat_Find, iTimerID_Dogmeat_Find)
	else
		cancelTimer(iTimerID_Dogmeat_Find)
	endif
EndFunction

Function HandleTimer_DogmeatFind()

	debug.trace(self + "Handletimer_DogmeatFind()")

	Actor DogmeatRef = DogmeatCompanion.GetActorReference()

	if DogmeatRef && DogmeatRef.GetDistance(Game.GetPlayer()) <= MaxDogmeatDistanceForAOEvent 
		debug.trace(self + "Handletimer_DogmeatFind() sending StoryManagerEvent()" + AO_Dogmeat_Find_Start + DogmeatRef)
		AO_Dogmeat_Find_Start.SendStoryEventAndWait(akRef1 = DogmeatRef, akLoc = DogmeatRef.GetCurrentLocation())  ;"AndWait" just to make sure we don't spam the story manager

	else 
		debug.trace(self + "Handletimer_DogmeatFind() Dogmeat to far away, **NOT sending** StoryManagerEvent()")
	endif

	startTimer(TimerInterval_Dogmeat_Find, iTimerID_Dogmeat_Find)
	
EndFunction

Function HandleTimer_DogmeatFindCoolDown()

	debug.trace(self + "Handletimer_DogmeatFindCoolDown() setting AO_Dogmeat_Find_Allowed to 1")

	AO_Dogmeat_Find_Allowed.SetValue(1)

EndFunction


Function DogmeatFindEventStarted()
	debug.trace("AO script DogmeatFindEventStarted()")

	AO_Dogmeat_Find_Allowed.SetValue(0)

EndFunction

Function DogmeatFindEventEnded(globalvariable AO_Dogmeat_FindType_GLOBAL)

	debug.trace("AO script DogmeatFindEventEnded()" + AO_Dogmeat_FindType_GLOBAL)

	int i = 0
	while (i < AO_Dogmeat_Find_Data.length)
		if AO_Dogmeat_Find_Data[i].Type == AO_Dogmeat_FindType_GLOBAL
			globalvariable NextAllowed = AO_Dogmeat_Find_Data[i].NextAllowed
			float GameDaysPassed = utility.GetCurrentGameTime() ;this is in terms of GameDaysPassed global
			float CoolDown = AO_Dogmeat_Find_Data[i].CoolDown.GetValue()
			float NextAllowedDay = GameDaysPassed + CoolDown

			debug.trace("AO script DogmeatFindEventEnded() setting " + NextAllowed + " to " + NextAllowedDay + " == GameDaysPassed: " + GameDaysPassed + " + CoolDown:" + CoolDown)

			NextAllowed.SetValue(NextAllowedDay)
		endif
		
		i += 1
	endwhile
	
	AO_Dogmeat_Find_Allowed.SetValue(1)

EndFunction

;called from dialogue to have player command dogmeat to try to find something
bool Function SendDogmeatFindContainerStoryManagerEvent()
;AO_Dogmeat_FindContainer_Commanded_Start
	ObjectReference DogmeatRef = DogmeatCompanion.GetReference()

	debug.trace(self + "SendDogmeatFindContainerStoryManagerEvent() sending StoryManagerEvent()" + AO_Dogmeat_FindContainer_Commanded_Start + DogmeatRef)
	return AO_Dogmeat_FindContainer_Commanded_Start.SendStoryEventAndWait(akRef1 = DogmeatRef, akLoc = DogmeatRef.GetCurrentLocation())  

EndFunction

bool Function SendDogmeatFindHostileStoryManagerEvent()
;AO_Dogmeat_FindHostile_Commanded_Start

	ObjectReference DogmeatRef = DogmeatCompanion.GetReference()

	debug.trace(self + "SendDogmeatFindHostileStoryManagerEvent() sending StoryManagerEvent()" + AO_Dogmeat_FindHostile_Commanded_Start + DogmeatRef)
	return AO_Dogmeat_FindHostile_Commanded_Start.SendStoryEventAndWait(akRef1 = DogmeatRef, akLoc = DogmeatRef.GetCurrentLocation())  

EndFunction

bool Function SendDogmeatFindItemStoryManagerEvent()
;AO_Dogmeat_FindItem_Commanded_Start

	ObjectReference DogmeatRef = DogmeatCompanion.GetReference()

	debug.trace(self + "SendDogmeatFindItemStoryManagerEvent() sending StoryManagerEvent()" + AO_Dogmeat_FindItem_Commanded_Start + DogmeatRef)
	return AO_Dogmeat_FindItem_Commanded_Start.SendStoryEventAndWait(akRef1 = DogmeatRef, akLoc = DogmeatRef.GetCurrentLocation())  

EndFunction