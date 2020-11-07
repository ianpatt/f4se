Scriptname Debug Native DebugOnly Hidden

; Note that these functions will do nothing in release console builds

; COC functionality
Function CenterOnCell(string asCellname) native global

; COC functionality
float Function CenterOnCellAndWait(string asCellname) native global

; player.moveto functionality
float Function PlayerMoveToAndWait(string asDestRef) native global

; Closes the specified user log
Function CloseUserLog(string asLogName) native global

; Dumps all alias fill information for the quest to the AliasDump log in Logs/Script/
Function DumpAliasData(Quest akQuest) native global

; Dumps all event registrations for the specified script to the Papyrus log
Function DumpEventRegistrations(ScriptObject akScript) native global

; Enable/disable AI processing
Function EnableAI(bool abEnable = true) native global

; Enable/disable collision detection
Function EnableCollisions(bool abEnable = true) native global

; Enable/disable AI detection
Function EnableDetection(bool abEnable = true) native global

; Enable/disable menu rendering
Function EnableMenus(bool abEnable = true) native global

; Returns the config name
string Function GetConfigName() native global

; Returns the platform name
string Function GetPlatformName() native global

; Returns the version number string
string Function GetVersionNumber() native global

; Displays an in-game message box
Function MessageBox(string asMessageBoxText) native global

; Displays an in-game notification
Function Notification(string asNotificationText) native global

; Opens a user log - fails if the log is already open
bool Function OpenUserLog(string asLogName) native global

; Quits the game
Function QuitGame() native global

; Toggles Foot IK on/off
Function SetFootIK(bool abFootIK) native global

; TGM functionality
Function SetGodMode(bool abGodMode) native global

; Start profiing a specific script - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StartScriptProfiling(string asScriptName) native global

; Start profiling the calling stack - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StartStackProfiling() native global

; Starts profiling all stacks that start in the specified script (and object, if any) - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StartStackRootProfiling(string asScriptName, ScriptObject akObj = None) native global

; Stop profiling a specific script - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StopScriptProfiling(string asScriptName) native global

; Stop profiling the calling stack - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StopStackProfiling() native global

; Stop profiling all stacks that start in the specified script (and object, if any) - setting doesn't persist across saves
; Will do nothing on release console builds, and if the Papyrus:bEnableProfiling ini setting is off
Function StopStackRootProfiling(string asScriptName, ScriptObject akObj = None) native global

; Outputs the string to the log
; Severity is one of the following:
; 0 - Info
; 1 - Warning
; 2 - Error
Function Trace(string asTextToPrint, int aiSeverity = 0) native global

; Outputs the current function to the log, including all variable values
Function TraceFunction(string asTextToPrint = "Tracing function on request", int aiSeverity = 0) native global

; Outputs the current stack to the log
Function TraceStack(string asTextToPrint = "Tracing stack on request", int aiSeverity = 0) native global

; Outputs the string to a user log - fails if the log hasn't been opened
bool Function TraceUser(string asUserLog, string asTextToPrint, int aiSeverity = 0) native global

Function TraceConditionalGlobal(string TextToPrint, GlobalVariable ShowTrace) Global
{As TraceConditional() but checks to make sure the global exists to avoid error messages in the log}
;kmk
	if ShowTrace && ShowTrace.value
		Trace(TextToPrint)
	endif
EndFunction

;Suppressable Trace
Function TraceConditional(string TextToPrint, bool ShowTrace, int Severity = 0) Global
{As Trace() but takes a second parameter bool ShowTrace (which if false suppresses the message). Used to turn off and on traces that might be otherwise annoying.}
;jduval
	if ShowTrace
		trace(TextToPrint, Severity)
	EndIf
EndFunction

Function TraceAndBox(string asTextToPrint, int aiSeverity = 0) global
{A convenience function to both throw a message box AND write to the trace log, since message boxes sometimes stack in weird ways and won't show up reliably.}
	;SJML
	MessageBox(asTextToPrint)
	Trace(asTextToPrint, aiSeverity)
EndFunction

;jduvall
Function TraceSelf(ScriptObject CallingScript, string FunctionName, string StringToTrace) global
{Convenient way to trace the script name and optional function name as a prefix to the trace string
Note, always pass in SELF as the CallingScript}

	FunctionName = "-->" + FunctionName + "():"

	trace(CallingScript + FunctionName + " " + StringToTrace)

EndFunction

; Used to add a tripod to a reference (non-release builds only)
Function ShowRefPosition(ObjectReference arRef) native global

;Prints out the players position to the database (non-release PC and Xenon builds only)
Function DBSendPlayerPosition() native global
