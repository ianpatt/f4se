Scriptname Message extends Form Native Hidden

; Show this message on the screen, substituting the values as appropriate. If a message box, it will wait until the user closes the box
; before returning - returning the button the user hit. If not a message box, or something went wrong, it will return -1
int Function Show(float afArg1 = 0.0, float afArg2 = 0.0, float afArg3 = 0.0, float afArg4 = 0.0, float afArg5 = 0.0, float afArg6 = 0.0, float afArg7 = 0.0, float afArg8 = 0.0, float afArg9 = 0.0) native


; Shows help message for a user action on screen.
Function ShowAsHelpMessage(string asEvent, float afDuration, float afInterval, int aiMaxTimes, string asContext="", int aiPriority=0) native

; Ends the showing of a help message and makes the message inactive.
Function UnshowAsHelpMessage() native

; Hides the help message without marking it as "complete"
Function ClearHelpMessages() native global

; Resets help message status for an input event so a new message can be displayed for that event.
Function ResetHelpMessage(string asEvent) native global
