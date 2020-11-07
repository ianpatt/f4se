Scriptname TeleportActorScript extends Actor

CustomEvent TeleportDone

Spell Property TeleportOutSpell Auto const mandatory
Spell Property TeleportInSpell Auto const mandatory

bool property teleportInOnLoad = true auto
{ TRUE = teleports in automatically on first load }

Event OnLoad()
    ;teleport an actor with this script on the first time they are enabled
    if Is3dLoaded() && teleportInOnLoad == true
        teleportInOnLoad = false
        teleportIn()
    endIf
EndEvent

function teleportIn()
	debug.trace(self + " teleportIn")
	self.AddSpell(teleportInSpell)
endFunction

function teleportOut()
	debug.trace(self + " teleportOut")
	self.AddSpell(teleportOutSpell)
endFunction
