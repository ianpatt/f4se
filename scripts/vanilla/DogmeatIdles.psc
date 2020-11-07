Scriptname DogmeatIdles extends Quest Const

GlobalVariable Property DogmeatIdles_FoundSomething_LastBehavior const auto

Group Idles

Idle Property Dogmeat_Bark_Alert const auto
Idle Property Dogmeat_Bark_Angry const auto
Idle Property Dogmeat_Bark_Playful const auto
Idle Property Dogmeat_Curious const auto
Idle Property Dogmeat_Growl_Playful const auto
Idle Property Dogmeat_Growl_Warning const auto
Idle Property Dogmeat_Howl_Mournful const auto
Idle Property Dogmeat_Howl_Playful const auto

Idle Property Dogmeat_Idle_Sniff const auto
Idle Property Dogmeat_Idle_Agitated const auto
Idle Property Dogmeat_Idle_Alert const auto
Idle Property Dogmeat_Idle_Playful const auto


Idle Property Dogmeat_PairedBiteDogStanding const auto
Idle Property Dogmeat_PairedBiteDogStanding2 const auto
Idle Property Dogmeat_PairedBiteHumanMoving const auto
Idle Property Dogmeat_PairedBiteHumanStanding const auto
Idle Property Dogmeat_PairedBiteMoleratStanding const auto
Idle Property Dogmeat_PairedHumanStanding_Kill const auto

Idle Property Dogmeat_Snarl_Vicious const auto
Idle Property Dogmeat_Whimper_Attention const auto
Idle Property Dogmeat_Whimper_Pained const auto
Idle Property Dogmeat_Whimper_Sad const auto
Idle Property DogmeatBark const auto
Idle Property DogmeatBarkLeading const auto
Idle Property DogmeatDraw const auto
Idle Property DogmeatFaceAngry const auto
Idle Property DogmeatFaceHappy const auto
Idle Property DogmeatFaceNeutral const auto
Idle Property DogmeatFaceSad const auto
Idle Property DogmeatIdleStop const auto
Idle Property DogmeatLeadingArrivalBeckon const auto
Idle Property DogmeatLeadingArrivalEmote const auto
Idle Property DogmeatLeadingArrivalInspect const auto
Idle Property DogmeatLeadingDoneEmote const auto
Idle Property DogmeatSheath const auto
Idle Property DogmeatSitIdle const auto
Idle Property DogmeatStaggerBackLarge const auto
Idle Property DogmeatStaggerBackMedium const auto
Idle Property DogmeatStaggerBackSmall const auto
EndGroup

DogmeatIdles Function GetScript() Global
	return Game.GetFormFromFile(0x000289E4, "Fallout4.esm") as DogmeatIdles
EndFunction



;**************************************************************************************
;*************************************** TIMERS ***************************************
;**************************************************************************************
int Property id_FaceTimer = 1 const auto hidden
int Property dur_FaceTimer = 4 const auto hidden

int Property id_AnimsArchetypeTimer = 2 const auto hidden
float Property dur_AnimsArchetypeTimer = 5.0 const auto hidden

Function StartFaceTimer() Global
	DogmeatIdles thisScript = GetScript()
	ThisScript.StartTimer(ThisScript.dur_FaceTimer, ThisScript.id_FaceTimer)
EndFunction

Function CancelFaceTimer() Global
	DogmeatIdles thisScript = GetScript()
	ThisScript.CancelTimer(ThisScript.id_FaceTimer)
EndFunction

Event OnTimer(int aiTimerID)
    if aiTimerID == id_FaceTimer
    	FaceNeutral()
    Elseif aiTimerID == id_AnimsArchetypeTimer
  	 	SetDogmeatNeutral()

    endif 
EndEvent



;*************************************************************************************
;*************************************** FACES ***************************************
;*************************************************************************************

Function FaceAngry(bool AutomaticallyBackToNeutralFace = true) Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		CancelFaceTimer()
		dogmeatActor.playIdle(GetScript().DogmeatFaceAngry)
		
		if AutomaticallyBackToNeutralFace
			StartFaceTimer()
		EndIf
	EndIf

EndFunction

Function FaceHappy(bool AutomaticallyBackToNeutralFace = true) Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor

		CancelFaceTimer()
		dogmeatActor.playIdle(GetScript().DogmeatFaceHappy)
		
		if AutomaticallyBackToNeutralFace
			StartFaceTimer()
		EndIf
	EndIf

EndFunction

Function FaceNeutral() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		CancelFaceTimer()
		dogmeatActor.playIdle(GetScript().DogmeatFaceNeutral)
	endif

EndFunction

Function FaceSad(bool AutomaticallyBackToNeutralFace = true) Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		CancelFaceTimer()
		dogmeatActor.playIdle(GetScript().DogmeatFaceSad)

		if AutomaticallyBackToNeutralFace
			StartFaceTimer()
		EndIf
	EndIf
EndFunction




;***********************************************************************************************
;*************************************** ANIM ARCHETYPES ***************************************
;***********************************************************************************************

Function SetDogmeatAgitated(float failSafeBackToNeutralTimerInterval = 180.0) global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		GetScript().CancelTimer(GetScript().id_AnimsArchetypeTimer)
		dogmeatActor.SetDogAnimArchetypeAgitated()

		if failSafeBackToNeutralTimerInterval > 0
			SetDogmeatNeutralAfterTimer(failSafeBackToNeutralTimerInterval)
		endif
	EndIf

EndFunction

Function SetDogmeatAlert(float failSafeBackToNeutralTimerInterval = 180.0) global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		GetScript().CancelTimer(GetScript().id_AnimsArchetypeTimer)
		dogmeatActor.SetDogAnimArchetypeAlert()

		if failSafeBackToNeutralTimerInterval > 0
			SetDogmeatNeutralAfterTimer(failSafeBackToNeutralTimerInterval)
		endif
	EndIf

EndFunction

Function SetDogmeatNeutral() global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		GetScript().CancelTimer(GetScript().id_AnimsArchetypeTimer)
		dogmeatActor.SetDogAnimArchetypeNeutral()
	EndIf

EndFunction

Function SetDogmeatNeutralAfterTimer(float TimerInterval = -1.0) global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		if TimerInterval == -1.0
			TimerInterval = GetScript().dur_AnimsArchetypeTimer
		endif

		GetScript().StartTimer(TimerInterval, GetScript().id_AnimsArchetypeTimer)
	EndIf


EndFunction

Function SetDogmeatPlayful(float failSafeBackToNeutralTimerInterval = 180.0) global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		GetScript().CancelTimer(GetScript().id_AnimsArchetypeTimer)
		dogmeatActor.SetDogAnimArchetypePlayful()

		if failSafeBackToNeutralTimerInterval > 0
			SetDogmeatNeutralAfterTimer(failSafeBackToNeutralTimerInterval)
		endif
	EndIf
EndFunction



;*****************************************************************************************
;*************************************** BEHAVIORS ***************************************
;*****************************************************************************************

Function FoundSomething() Global
;bounces randomly (more or less) between idles indicating "I found something, come look"

	GlobalVariable LastBehavior = GetScript().DogmeatIdles_FoundSomething_LastBehavior

	int last = LastBehavior.getValue() as int

	int i = Utility.RandomInt(1,4)

	if last == i
		i = 1 ;one is the paw scratching, we're okay if he does that more than one time in row
	EndIf

	if i == 2
		HowlPlayful()

	elseif i == 3
		BarkPlayful()

	elseif i == 4
		Sniff()

	else ; i == 1
		LeadingArrivalInspect()
	EndIf

	LastBehavior.setValue(i)

EndFunction	



;**********************************************************************************************
;*************************************** LEADING EMOTES ***************************************
;**********************************************************************************************

Function LeadingArrivalBeckon() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().DogmeatLeadingArrivalBeckon)
	EndIf
EndFunction

Function LeadingArrivalEmote() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().DogmeatLeadingArrivalEmote)
	EndIf
EndFunction

Function LeadingArrivalInspect() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().DogmeatLeadingArrivalInspect)
	EndIf
EndFunction

Function Sniff() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Idle_Sniff)
	EndIf
EndFunction




;**************************************************************************************************************
;*************************************** BARKS, HOWLS, WHIMPERS, WHINES ***************************************
;**************************************************************************************************************

Function BarkNormal() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().DogmeatBark)
	EndIf
endFunction

Function BarkAlert() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Bark_Alert)
	EndIf
endFunction

Function BarkAngry() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Bark_Angry)
		FaceAngry()
	EndIf
endFunction

Function BarkPlayful() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Bark_Playful)
		FaceHappy()
	endif
endFunction

Function BarkCurious() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Curious)
	endif
endFunction

Function GrowlPlayful() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Growl_Playful)
		FaceHappy()
	endif
endFunction

Function GrowlWarning() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Growl_Warning)
		FaceAngry()
	EndIf
endFunction

Function HowlSad() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Howl_Mournful)
		FaceSad()
	EndIf
endFunction

Function HowlPlayful() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Howl_Playful)
		FaceHappy()
	EndIf
endFunction

Function SnarlVicious() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Snarl_Vicious)
		FaceAngry()
	EndIf
endFunction

Function WhimperAttention() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Whimper_Attention)
	endif
endFunction

Function WhimperPained() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Whimper_Pained)
		FaceSad()
	EndIf
endFunction

Function WhimperSad() Global
	Actor dogmeatActor = Game.GetCommonProperties().DogmeatCompanion.GetActorReference()

	if dogmeatActor
		Game.GetCommonProperties().DogmeatCompanion.GetActorReference().playIdle(GetScript().Dogmeat_Whimper_Sad)
		FaceSad()
	EndIf
endFunction


