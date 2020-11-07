Scriptname F4SE Native Hidden
; General F4SE-specific information

; get the major version of F4SE
int Function GetVersion() global native
; get the minor version of F4SE
int Function GetVersionMinor() global native
; get the beta version of F4SE
int Function GetVersionBeta() global native
; get the release index of F4SE.  This number is incremented every time
; F4SE is released outside of the development team
int Function GetVersionRelease() global native
; get the release index of this script file.
; Can be used to detect a script/runtime version mismatch
int Function GetScriptVersionRelease() global
	return 23
endFunction

; get a plugins version number, -1 if the plugin is not loaded
int Function GetPluginVersion(string name) global native