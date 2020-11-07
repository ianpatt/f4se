Scriptname Utility Native Hidden

; Calls a global function specified by name. Note that the compiler cannot check that the function
; exists, nor that the parameters are the right type. It will also not be able to auto-cast, so
; you must pre-cast the parameters to the right type (derived or parent types won't work). Will
; error if the function or script type are not valid or don't exist
Var Function CallGlobalFunction(string asScriptName, string asFuncName, Var[] aParams) native global

; Calls a global function specified by name without waiting for it to return. Note that the compiler
; cannot check that the function exists, nor that the parameters are the right type. It will also not
; be able to auto-cast, so you must pre-cast the parameters to the right type (derived or parent
; types won't work). Will error if the function or script type are not valid or don't exist
Function CallGlobalFunctionNoWait(string asScriptName, string asFuncName, Var[] aParams) native global

; Converts a float game time (in terms of game days passed) to a string detailing the date
; and time it represents in "MM/DD/YYYY HH:MM" format. A 24-hour clock is used, and the function
; is latent (due to issues in the current architecture with returning strings from code)
string Function GameTimeToString(float afGameTime) native global

; Obtains the current game time in terms of game days passed (same as the global variable)
float Function GetCurrentGameTime() native global

; Obtains the number of seconds since the application started (the same timer that WaitMenuMode uses)
; Does not take into account menu-mode, or VM frozen time
; Most useful for determining how long something took to run
float Function GetCurrentRealTime() native global

; Obtains the ID number of the current running stack
int Function GetCurrentStackID() native global debugOnly

; Returns whether the game is currently in menu mode or not
bool Function IsInMenuMode() native global

; Generates a random integer between aiMin and aiMax (inclusive)
int Function RandomInt(int aiMin = 0, int aiMax = 100) native global

; Generates a random floating point number between afMin and afMax (inclusive)
float Function RandomFloat(float afMin = 0.0, float afMax = 1.0) native global

; Set the given INI by type
function SetINIFloat(string ini, float value) native global debugOnly
function SetINIInt(string ini, int value) native global debugOnly
function SetINIBool(string ini, bool value) native global debugOnly
function SetINIString(string ini, string value) native global debugOnly

; Waits for the specified amount of time (latent). Timer will not run during menu mode
Function Wait(float afSeconds) native global

; Waits for the specified amount of game time (latent)
Function WaitGameTime(float afHours) native global

; Waits for the specified amount of time (latent) - Timer WILL run during menu mode
Function WaitMenuMode(float afSeconds) native global

; Frame rate capture functions only available in beta version

; Gets you a string describing the frame rate for a certain number of frames
; (String will be no longer than 1K characters long, separated by commas)
string Function CaptureFrameRate(int numFrames) native global debugOnly

; Sends the passed in test data to the tests website.
Function EnterTestData(string astestType, string astestMatter, string astestDetails, string astestResultContext, string astestResult) native global debugOnly

; Sends the start up times to the tests website.
Function PostStartUpTimes() native global debugOnly

; Starts or ends a frame rate capture -- then you can get the min or max since 
; frame capture started at any time
Function StartFrameRateCapture() native global debugOnly
Function EndFrameRateCapture() native global debugOnly
float Function GetAverageFrameRate() native global debugOnly
float Function GetMinFrameRate() native global debugOnly
float Function GetMaxFrameRate() native global debugOnly

; Memory tracking functions - only available if memory tracking is turned on
string Function GetCurrentMemory() native global debugOnly ; Must be called first, it sets up the memory stats used by the other functions
int Function GetBudgetCount() native global debugOnly
string Function GetCurrentBudget(int aiBudgetNumber) native global debugOnly
string Function GetBudgetLimit(int aiBudgetNumber) native global debugOnly
bool Function OverBudget(int aiBudgetNumber) native global debugOnly
string Function GetBudgetName(int aiBudgetNumber) native global debugOnly