Scriptname InputEnableLayer extends ScriptObject Native Hidden
{This is a single layer of input enable/disable. The end result of all layers in the game
determine the actual state of the player's controls.}

; Creates a new input enable layer and returns it. These are limited, so make sure
; to dispose of it when you're done (by setting variables to None)
InputEnableLayer Function Create() Native Global

; Force-deletes this input layer. Any variable pointing at this layer will become invalid
; and it should also clear any disables this layer had (unless code, for some reason, is
; keeping it alive).
Function Delete() Native

; Disables the user's controls
Function DisablePlayerControls(bool abMovement = true, bool abFighting = true, bool abCamSwitch = false, bool abLooking = false, \
	bool abSneaking = false, bool abMenu = true, bool abActivate = true, bool abJournalTabs = false, bool abVATS = true, bool abFavorites = true, bool abRunning = false) Native

; Enables/disables the player's ability to activate objects
Function EnableActivate(bool abEnable = true) Native

; Enables/disables the player's VATS
Function EnableVATS(bool abEnable = true) Native

; Enables/disables the player's ability to switch the camera between 1st and 3rd person
Function EnableCamSwitch(bool abEnable = true) Native

; Enables/disables the player's ability to fast travel
Function EnableFastTravel(bool abEnable = true) Native

; Enables/disables the player's ability to open the Favorites menu
Function EnableFavorites(bool abEnable = true) Native

; Enables/disables the player's ability to fight
Function EnableFighting(bool abEnable = true) Native

; Enables/disables the player's ability to use the journal menu
Function EnableJournal(bool abEnable = true) Native

; Enables/disables the player's ability to jump (Note: Movement disable/enable also touches this)
Function EnableJumping(bool abEnable = true) Native

; Enables/disables the player's ability to look around
Function EnableLooking(bool abEnable = true) Native

; Enables/disables the player's ability to use the main menu
Function EnableMenu(bool abEnable = true) Native

; Enables/disables the player's ability to move
Function EnableMovement(bool abEnable = true) Native

; Enables/disables the player's ability to run
Function EnableRunning(bool abEnable = true) Native
  
; Enables/disables the player's ability to sprint
Function EnableSprinting(bool abEnable = true) Native

; Enables/disables the player's ability to z-key
Function EnableZKey(bool abEnable = true) Native

; Enables the user's controls
Function EnablePlayerControls(bool abMovement = true, bool abFighting = true, bool abCamSwitch = true, bool abLooking = true, \
  bool abSneaking = true, bool abMenu = true, bool abActivate = true, bool abJournalTabs = true, bool abVATS = true, bool abFavorites = true, bool abRunning = true) Native

; Enables/disables the player's ability to sneak
Function EnableSneaking(bool abEnable = true) Native

; Returns whether activation is enabled on this layer
bool Function IsActivateEnabled() Native

; Returns whether VATS is enabled on this layer
bool Function IsVATSEnabled() Native

; Returns whether camera switch is enabled on this layer
bool Function IsCamSwitchEnabled() Native

; Returns whether fast travel is enabled on this layer
bool Function IsFastTravelEnabled() Native

; Returns whether the Favorites menu is enabled on this layer
bool Function IsFavoritesEnabled() Native

; Returns whether fighting is enabled on this layer
bool Function IsFightingEnabled() Native

; Returns whether the journal menu is enabled on this layer
bool Function IsJournalEnabled() Native

; Returns whether jumping is enabled on this layer (Note: Movement also controls this)
bool Function IsJumpingEnabled() Native

; Returns whether looking is enabled on this layer
bool Function IsLookingEnabled() Native

; Returns whether the menu is enabled on this layer
bool Function IsMenuEnabled() Native

; Returns whether movement is enabled on this layer
bool Function IsMovementEnabled() Native

; Returns whether running is enabled on this layer
bool Function IsRunningEnabled() Native

; Returns whether sprinting is enabled on this layer
bool Function IsSprintingEnabled() Native

; Returns whether z-key is enabled on this layer
bool Function IsZKeyEnabled() Native

; Returns whether sneaking is enabled on this layer
bool Function IsSneakingEnabled() Native

; Re-enables all controls currently disabled on this layer
Function Reset() Native