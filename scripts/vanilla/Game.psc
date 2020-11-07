Scriptname Game Native Hidden

; Adds the specified achievement to the player's profile
Function AddAchievement(int aiAchievementID) native global

; Add the specified number of perk points to the player
Function AddPerkPoints(int aiPerkPoints) native global

; Advance the given skill on the player by the provided amount of skill usage
Function AdvanceSkill(string asSkillName, float afMagnitude) native global

; Clears the prison variables on the player
Function ClearPrison() native global

; Clears temp effects from game
Function ClearTempEffects() native global

; Enables/disables the HDM mask for the pipboy screen
Function EnablePipboyHDRMask(bool abEnable = true) native global

; Issue an error with stack trace to the script log and warning system
Function Error(string asMessage) native global betaOnly

; Fades out the game to black, or vice versa
Function FadeOutGame(bool abFadingOut, bool abBlackFade, float afSecsBeforeFade, float afFadeDuration, bool abStayFaded = false) native global

; Fast-travels the player to the specified object's location
Function FastTravel(ObjectReference akDestination) native global

; Finds the closest reference of a given base object within a given radius of a location
ObjectReference Function FindClosestReferenceOfType(Form arBaseObject, float afX, float afY, float afZ, float afRadius) native global

; Finds a random reference of a given base object within a given radius of a location
ObjectReference Function FindRandomReferenceOfType(Form arBaseObject, float afX, float afY, float afZ, float afRadius) native global

; Finds the closest reference of any base object in the list within a given radius of a location
ObjectReference Function FindClosestReferenceOfAnyTypeInList(FormList arBaseObjects, float afX, float afY, float afZ, float afRadius) native global

; Finds a random reference of a any base object in the list within a given radius of a location
ObjectReference Function FindRandomReferenceOfAnyTypeInList(FormList arBaseObjects, float afX, float afY, float afZ, float afRadius) native global

; Finds the closest reference of a given base object within a given radius of a reference
ObjectReference Function FindClosestReferenceOfTypeFromRef(Form arBaseObject, ObjectReference arCenter, float afRadius) global
	return FindClosestReferenceOfType(arBaseObject, arCenter.X, arCenter.Y, arCenter.Z, afRadius)
endFunction

; Finds a random reference of a given base object within a given radius of a reference
ObjectReference Function FindRandomReferenceOfTypeFromRef(Form arBaseObject, ObjectReference arCenter, float afRadius) global
	return FindRandomReferenceOfType(arBaseObject, arCenter.X, arCenter.Y, arCenter.Z, afRadius)
endFunction

; Finds the closest reference of a given base object within a given radius of a reference
ObjectReference Function FindClosestReferenceOfAnyTypeInListFromRef(FormList arBaseObjects, ObjectReference arCenter, float afRadius) global
	return FindClosestReferenceOfAnyTypeInList(arBaseObjects, arCenter.X, arCenter.Y, arCenter.Z, afRadius)
endFunction

; Finds a random reference of a given base object within a given radius of a reference
ObjectReference Function FindRandomReferenceOfAnyTypeInListFromRef(FormList arBaseObjects, ObjectReference arCenter, float afRadius) global
	return FindRandomReferenceOfAnyTypeInList(arBaseObjects, arCenter.X, arCenter.Y, arCenter.Z, afRadius)
endFunction

; Finds the closest actor within a given radius of a location
Actor Function FindClosestActor(float afX, float afY, float afZ, float afRadius) native global

; Finds a random actor within a given radius of a location
Actor Function FindRandomActor(float afX, float afY, float afZ, float afRadius) native global

; Finds the closest actor within a given radius of a reference
Actor Function FindClosestActorFromRef(ObjectReference arCenter, float afRadius) global
	return FindClosestActor(arCenter.X, arCenter.Y, arCenter.Z, afRadius)
endFunction

; Finds a random actor within a given radius of a reference
Actor Function FindRandomActorFromRef(ObjectReference arCenter, float afRadius) global
	return FindRandomActor(arCenter.X, arCenter.Y, arCenter.Z, afRadius)
endFunction

; Disable SSR, godrays, and/or directional lighting. Note: disable state reset after 240 frames if this function is not continually called.
Function ForceDisableSSRGodraysDirLight( bool abDisableSSR, bool abDisableGodrays, bool abDisableDirLight ) native global

; Make the player got to 3rd person camera mode
Function ForceThirdPerson() native global

; Make the player got to 1st person camera mode
Function ForceFirstPerson() native global

; Return the total amount of XP required to get to the particular level (not the gap between two levels)
int Function GetXPForLevel(int auiLevel) native global

; Show the players first person geometry.
Function ShowFirstPersonGeometry( bool abShow = true ) native global

; Show all the map markers on the world map.
Function ShowAllMapMarkers( ) native global

; Get the Aggression Actor Value
ActorValue Function GetAggressionAV() native global

; Get the Agility Actor Value
ActorValue Function GetAgilityAV() native global

; Returns the caps misc item
MiscObject Function GetCaps() global
	return GetForm(0x0000000F) as MiscObject ; Caps001 form ID is very unlikely to ever change
EndFunction

; Get the Charisma Actor Value
ActorValue Function GetCharismaAV() native global

;useful commonly used properties live on quest CommonProperties' CommonPropertiesScript
CommonPropertiesScript Function GetCommonProperties() global
	RETURN Game.GetFormFromFile(0x000A7D73, "Fallout4.esm") as CommonPropertiesScript
EndFunction


; Get the Confidence Actor Value
ActorValue Function GetConfidenceAV() native global

; Returns the game's current difficulty
; Type is as follows:
; 0 - Very Easy
; 1 - Easy
; 2 - Normal
; 3 - Hard
; 4 - Very Hard
; 5 - Survival (no Hardcore) DEFUNCT
; 6 - Survival w/ Hardcore
int Function GetDifficulty() native global

; Get the Endurance Actor Value
ActorValue Function GetEnduranceAV() native global

; Returns the form specified by the ID
Form Function GetForm(int aiFormID) native global

; Returns the form specified by the ID originating in the given file
Form Function GetFormFromFile(int aiFormID, string asFilename) native global

; Obtains the value of a game setting - one for each type of game setting
float Function GetGameSettingFloat(string asGameSetting) native global
int Function GetGameSettingInt(string asGameSetting) native global
string Function GetGameSettingString(string asGameSetting) native global

; Get the Health Actor Value
ActorValue Function GetHealthAV() native global

; Get the Intelligence Actor Value
ActorValue Function GetIntelligenceAV() native global

; Get the Luck Actor Value
ActorValue Function GetLuckAV() native global

; Get the Perception Actor Value
ActorValue Function GetPerceptionAV() native global

; Returns the player actor
Actor Function GetPlayer() native global

; Returns a list of actors that are following the player (including both those running
; a Follow procedure and those flagged by their package to be treated as followers).
Actor[] Function GetPlayerFollowers( ) native global

; Returns the player's level
int Function GetPlayerLevel() global
	; level exact is faster and it's the player anyway so adjustment doesn't mean anything
	return (GetPlayer().GetBaseObject() as ActorBase).GetLevelExact()
EndFunction

; Returns the reference the player is currently grabbing
ObjectReference Function GetPlayerGrabbedRef() native global

; Returns the current frequency setting of the player's radio
float Function GetPlayerRadioFrequency() native global

; Returns the horse last ridden by the player
Actor Function GetPlayersLastRiddenHorse() native global

; Returns the number of days spent in play
float Function GetRealHoursPassed() native global

; Get the Suspicious Actor Value
ActorValue Function GetSuspiciousAV() native global

; Get the Strength Actor Value
ActorValue Function GetStrengthAV() native global

; Adds a specified amount of caps to the player
Function GivePlayerCaps(int nCaps) global
	if ( nCaps > 0 )
		GetPlayer().AddItem(GetCaps(),nCaps)
	endif
endFunction

; Removes a specified amount of caps from the player
Function RemovePlayerCaps(int nCaps) global
	if ( nCaps > 0 )
		GetPlayer().RemoveItem(GetCaps(),nCaps)
	endif
endFunction

; Increment the given skill on the player by the given number of points
Function IncrementSkill(ActorValue akActorValue, int aiCount = 1) native global

; Modifies the specified MiscStat by the given amount.
Function IncrementStat(string asStatName, int aiModAmount = 1) native global

; Called to tell the Compass displays to refresh after gamesettings are updated related to hardcore mode
Function InitializeMarkerDistances() native global

; Are the activation controls enabled?
bool Function IsActivateControlsEnabled() native global

; Are the VATS controls enabled?
bool Function IsVATSControlsEnabled() native global

; Is VATS playback active?
bool Function IsVATSPlaybackActive() native global

; Are the camera switch controls enabled?
bool Function IsCamSwitchControlsEnabled() native global

; Is fast travel controls enabled? Returns false if EnableFastTravel(false) has been called
bool Function IsFastTravelControlsEnabled() native global

; Is fast travel enabled?
bool Function IsFastTravelEnabled() native global

; Is favorites menu enabled?
bool Function IsFavoritesControlsEnabled() native global

; Are the fighting controls enabled?
bool Function IsFightingControlsEnabled() native global

; Are the journal menu controls enabled?
bool Function IsJournalControlsEnabled() native global

; Are the jumping controls enabled? (Note: Movement also enables/disables jumping)
bool Function IsJumpingControlsEnabled() native global

; Are the looking controls enabled?
bool Function IsLookingControlsEnabled() native global

; Are the menu controls enabled?
bool Function IsMenuControlsEnabled() native global

; Are the movement controls enabled?
bool Function IsMovementControlsEnabled() native global

; Is the specified plugin (esm or esp) installed?
bool Function IsPluginInstalled(string asName) native global

; Is the player within the outer radius of a transmitter using the given frequency?
bool Function IsPlayerInRadioRange(float afFrequency) native global

; Is the player actively listening to a transmitter using the given frequency.
; (Pipboy is on, set to a frequency that would receive the transmission, and within the transmitter's outer radius.)
bool Function IsPlayerListening(float afFrequency) native global

; Is the player's radio currently on?
bool Function IsPlayerRadioOn() native global

; Are the sneaking controls enabled?
bool Function IsSneakingControlsEnabled() native global

; Pass time as if the player has waited the given number of hours.
Function PassTime(int aiHours) native global

; Plays a bink video - does not return until bink has finished, use with care!
Function PlayBink(string asFileName, bool abInterruptible = false, bool abMuteAudio = true, bool abMuteMusic = true, \
  bool abLetterbox = true, bool abIsNewGameBink = false ) native global

; Precaches character gen data.
Function PrecacheCharGen() native global

; Clears Precached character gen data.
Function PrecacheCharGenClear() native global

; Queries the given stat and returns its value
int Function QueryStat(string asStat) native global

; Forces the game back to the main menu
Function QuitToMainMenu() native global

; Request that an auto-save be made
Function RequestAutoSave() native global

; Requests the specified model
Function RequestModel(string asModelName) native global

; Request that a normal save be made
Function RequestSave() native global

; Rewards the player with XP, if direct is true, then will ignore various XP adjustments like entry points and intelligence
Function RewardPlayerXP(int auiXPAmount, bool abDirect = false) native global

; Has the player serve their prison time
Function ServeTime() native global

; Sets the camera target actor
Function SetCameraTarget(Actor arTarget) native global

; Sets or clears CharGen-specific HUD modes.
Function SetCharGenHUDMode(int aiCGHUDMode) native global

; Sets or clears InsideMemory HUD Mode
Function SetInsideMemoryHUDMode(bool aInsideMemory) native global

; Plays the specified Perk VaultBoy Swf on the HUD
Function ShowPerkVaultBoyOnHUD(string aVaultBoySwf, Sound aSoundDescriptor = None) native global

; Informs the game whether we are in CharGen or not
Function SetInChargen(bool abDisableSaving, bool abDisableWaiting, bool abShowControlsDisabledMessage) native global

; Enables or disables the AI driven flag on Player
Function SetPlayerAIDriven(bool abAIDriven = true) native global

; Enables or disables the Player on Elevator flag
Function SetPlayerOnElevator(bool abOnElevator= true) native global


; Sets the player's radio's current frequency
Function SetPlayerRadioFrequency(float afFrequency) native global

; Enables or disables  crime reporting on Player
Function SetPlayerReportCrime(bool abReportCrime = true) native global

; Set the players sitting camera rotation - in degrees, offset from the standard angle.
Function SetSittingRotation(float afValue) native global

; Shakes the object from the location of the passed-in object. If none, it will shake the camera from the player's location.
; Strength is clamped from 0 to 1
; Duration in seconds. By default (0.0) use the game setting.
Function ShakeCamera(ObjectReference akSource = None, float afStrength = 0.5, float afDuration = 0.0) native global

; Shakes the controller for the specified length of time (in seconds). The strength values are clamped from 0 to 1
Function ShakeController(float afSmallMotorStrength, float afBigMotorStreangth, float afDuration) native global

; Causes the Fatigue Warning to show on the HUD
Function ShowFatigueWarningOnHUD() native global

; Displays the race/sex menu
Function ShowRaceMenu( ObjectReference akMenuTarget = None, int uiMode = 0, ObjectReference akMenuSpouseFemale = None, ObjectReference akMenuSpouseMale = None, ObjectReference akVendor = None ) native global

; Displays the SPECIAL menu
Function ShowSPECIALMenu() native global

; Title Sequence menu functions
Function ShowTitleSequenceMenu() native global
Function HideTitleSequenceMenu() native global
Function StartTitleSequence(string asSequenceName) native global

; Pipboy boot sequence
Function ShowPipboyBootSequence(string asAnimationName) native global

; Pipboy plugin sequence
Function ShowPipboyPlugin() native global

; Displays the training menu based on passed in trainer actor
Function ShowTrainingMenu(Actor aTrainer) native global

; Trigger screen blood with the given count
Function TriggerScreenBlood(int aiValue) native global

; Play an event camera
Function PlayEventCamera( CameraShot akCamera, ObjectReference akRef ) native global

; start dialogue camera if in third person or in dialogue camera or swing the camera to speaker if in 1st
Function StartDialogueCameraOrCenterOnTarget( ObjectReference akCameraTarget = None  ) native global

; Stop dialogue camera
Function StopDialogueCamera( bool abConsiderResume = false, bool abSwitchingTo1stP = false ) native global

; Turns on/off the player's radio
Function TurnPlayerRadioOn(bool abRadioOn = true) native global

; Returns true if we're using a gamepad
bool Function UsingGamepad() native global

; Issue a warning to the script log and warning system
Function Warning(string asMessage) native global betaOnly