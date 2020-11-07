; Base object for every script in the game, even if that script doesn't explicitly extend anything
Scriptname ScriptObject Native Hidden

; Add an inventory event filter to this effect. Item added/removed events matching the specified form (or in the specified form list) will now
; be let through. A "None" filter will let everything through.
; Objects without filters CANNOT receive inventory add/remove events!
Function AddInventoryEventFilter(Form akFilter) native

; Calls a member function on this script specified by name. Note that the compiler cannot check
; that the function exists, nor that the parameters are the right type. It will also not be able
; to auto-cast, so you must pre-cast the parameters to the right type (derived or parent types
; won't work). Will error if the function is not valid or doesn't exist on the object
Var Function CallFunction(string asFuncName, Var[] aParams) native

; Calls a member function on this script without waiting for it to return. Note that the compiler
; cannot check that the function exists, nor that the parameters are the right type. It will also not
; be able to auto-cast, so you must pre-cast the parameters to the right type (derived or parent
; types won't work). Will error if the function is not valid or doesn't exist on the object
Function CallFunctionNoWait(string asFuncName, Var[] aParams) native

; Cancels a pending timer on this script with the specified ID
Function CancelTimer(int aiTimerID = 0) native

; Cancels a pending game-time timer on this script with the specified ID
Function CancelTimerGameTime(int aiTimerID = 0) native

; Gets the value of a property on this script specified by name. Note that the compiler cannot
; check that the property exists. Will error if the property doesn't exist on the object, or
; is write-only
Var Function GetPropertyValue(string asPropertyName) native

; Obtains the script's current state
string Function GetState()
	return __state ; Return the value of the state intrinsic variable
EndFunction

; Sets the script's current state, calling the appropriate end/begin state events
Function GotoState(string asNewState)
	string soldState = __state
	OnEndState(asNewState) ; Tell the old state it is finished, passing in the new state
	__state = asNewState ; Change the script's state
	OnBeginState(soldState) ; Tell the new state it is starting, passing in the old state
EndFunction

; Attempts to cast this object as the specified script. The compiler won't type check this cast,
; and the cast may fail. But this will allow you to point at the specific script you want without
; adding a dependency on said script (for when it is part of a mod or DLC that the user may not
; have installed). If the type doesn't exist or the cast fails it will return None
ScriptObject Function CastAs(string asScriptName) native

; Returns true if this object is attached to an in-game object and that object
; is available to be operated on. If this returns false then any form, alias,
; or active magic effect native functions will most likely fail.
bool Function IsBoundGameObjectAvailable() native

; Register for the specified animation event from the specified object - returns true if it successfully registered
bool Function RegisterForAnimationEvent(ObjectReference akSender, string asEventName) native

; Register for a custom event to be sent from the specified sender. The event will be sent to:
; Event <type>.<event>(<type> akSender, Var[] akParams)
; For example, MySpecialEvent on MyQuest would be:
; Event MyQuest.MySpecialEvent(MyQuest akSender, Var[] akParams)
Function RegisterForCustomEvent(ScriptObject akSender, CustomEventName asEventName) native

; Register for only the first detection-based LOS gain event between the viewer and the target
; If the viewer is already looking at the target, an event will be received almost immediately
; If the viewer is not the player, the target must be another actor
Function RegisterForDetectionLOSGain(Actor akViewer, ObjectReference akTarget) native

; Register for only the first detection-based LOS lost event between the viewer and the target
; If the viewer is already not looking at the target, an event will be received almost immediately
; If the viewer is not the player, the target must be another actor
Function RegisterForDetectionLOSLost(Actor akViewer, ObjectReference akTarget) native

; Register for only the first direct LOS gain event between the viewer and the target
; If the viewer is already looking at the target, an event will be received almost immediately
Function RegisterForDirectLOSGain(ObjectReference akViewer, ObjectReference akTarget, string asViewerNode = "", string asTargetNode = "") native

; Register for only the first direct LOS lost event between the viewer and the target
; If the viewer is already not looking at the target, an event will be received almost immediately
Function RegisterForDirectLOSLost(ObjectReference akViewer, ObjectReference akTarget, string asViewerNode = "", string asTargetNode = "") native

; Register for OnDistanceClose when the two objects are less than then afDistance apart. The two objects are interchangable,
; and if you register for the two swapped it will simply update the distance. The objects may be reference or reference aliases.
Function RegisterForDistanceLessThanEvent(ScriptObject akObj1, ScriptObject akObj2, float afDistance) native

; Register for OnDistanceFar when the two objects are greater than then afDistance apart. The two objects are interchangable,
; and if you register for the two swapped it will simply update the distance. The objects may be reference or reference aliases.
Function RegisterForDistanceGreaterThanEvent(ScriptObject akObj1, ScriptObject akObj2, float afDistance) native

; Registers for a single hit event. Filters may be exact forms, base objects of references, keywords, factions, reference aliases,
; ref collection aliases, or form lists containing any or all of them. This also filters any remote hit events that come in.
; The power/sneak/bash/block filters are -1 for no filter, 0 (or false) for false only, and 1 (or true) for true only.
; Targets can be references, ref aliases, or ref collection aliases
; If abMatch is false, then it matches anything that DOESN'T match the filter (NOTs the whole thing).
; Objects without registrations CANNOT receive hit events!
Function RegisterForHitEvent(ScriptObject akTarget, ScriptObject akAggressorFilter = None, Form akSourceFilter = None, Form akProjectileFilter = None, \
	int aiPowerFilter = -1, int aiSneakFilter = -1, int aiBashFilter = -1, int aiBlockFilter = -1, bool abMatch = true) native

; Registers for a single magic effect apply event. Filters may be exact forms, base objects of references, keywords, factions,
; reference aliases, ref collection aliases, or form lists containing any or all of them. This also filters any remote magic
; effect apply events that come in.
; Targets can be references, ref aliases, or ref collection aliases
; If abMatch is false, then it matches anything that DOESN'T match the filter (NOTs the whole thing).
; Objects without registrations CANNOT receive magic effect apply events!
Function RegisterForMagicEffectApplyEvent(ScriptObject akTarget, ScriptObject akCasterFilter = None, Form akEffectFilter = None, bool abMatch = true) native

; Registers this alias to receive events when menus are opened / closed
Function RegisterForMenuOpenCloseEvent(string asMenuName) native

; Registers this form to receive events when the player sleeps and wakes up
Function RegisterForPlayerSleep() native

; Registers this form to receive events when the player teleports (load door, fast travel, moveto)
Function RegisterForPlayerTeleport() native

; Registers this form to receive events when the player waits
Function RegisterForPlayerWait() native

; Registers for a single radiation damage event from the specified target.
; Targets can be actors, ref aliases, or ref collection aliases
; Objects without registrations CANNOT receive radiation damage events!
Function RegisterForRadiationDamageEvent(ScriptObject akTarget) native

; Register for the specified event from the specified source. Event must be one sent by
; the game itself, and not an event defined in ScriptObject. The event will be sent to:
; Event <type>.<event>(<type> akSender, <event args>)
; For example, OnDeath on an Actor would be:
; Event Actor.OnDeath(Actor akSender, Actor akKiller)
; But OnDeath on a reference alias would be:
; Event ReferenceAlias.OnDeath(ReferenceAlias akSender, Actor akKiller)
; Returns true if the registration succeeded
bool Function RegisterForRemoteEvent(ScriptObject akEventSource, ScriptEventName asEventName) native

; Registers this script to receive a single-shot stat event when the stat meets or passes the threshold
; If the stat already meets or passes the threshold the event will be sent immediately
Function RegisterForTrackedStatsEvent(string asStat, int aiThreshold) native

; Registers to receive events from Looks menu
Function RegisterForLooksMenuEvent() native

; Registers to receive tutorial-related events
Function RegisterForTutorialEvent( String asEventName ) native

; Remove all inventory event filters from this effect - it will no longer receive any inventory add/remove events
Function RemoveAllInventoryEventFilters() native

; Remove an inventory event filter from this object. Item added/removed events matching the specified form (or in the
; specified form list) will no longer be let through.
; Objects without filters CANNOT receive inventory add/remove events!
Function RemoveInventoryEventFilter(Form akFilter) native

; Sends out a custom event from this object. The custom event must be defined using
; "CustomEvent <name of event>" somewhere in the script that sends it or its parents
Function SendCustomEvent(CustomEventName asEvent, Var[] akArgs = None) native

; Sets a property value on this script specified by name. Note that the compiler cannot check
; that the property exists, nor that the value is the right type. It will also not be able to
; auto-cast, so you must pre-cast the value to the right type (derived or parent types won't
; work). Will error if the property is not valid, doesn't exist on the object, is read-only
; or an auto-property specified as const.
Function SetPropertyValue(string asPropertyName, Var aValue) native

; Sets a property value on this script without waiting for it to return. Note that the compiler
; cannot check that the property exists, nor that the value is the right type. It will also not
; be able to auto-cast, so you must pre-cast the value to the right type (derived or parent
; types won't work). Will error if the property is not valid, doesn't exist on the object, is
; read-only or an auto-property specified as const.
Function SetPropertyValueNoWait(string asPropertyName, Var aValue) native

; Start a timer on this script with the specified ID. Interval is in seconds and the timer
; event will fire only once.
Function StartTimer(float afInterval, int aiTimerID = 0) native

; Start a game-time timer on this script with the specified ID. Interval is in hours and the
; timer event will fire only once.
Function StartTimerGameTime(float afInterval, int aiTimerID = 0) native

; Remove all event registrations - useful if you're trying to delete something and event registraitons are keeping it around
Function UnregisterForAllEvents() native

; Remove all custom event registrations
Function UnregisterForAllCustomEvents() native

; Remove all hit event registrations - if target is None - ALL event registrations for this script are removed
Function UnregisterForAllHitEvents(ScriptObject akTarget = None) native

; Remove all magic effect apply registrations - if target is None - ALL event registrations for this script are removed
Function UnregisterForAllMagicEffectApplyEvents(ScriptObject akTarget = None) native

; Remove all menu open/close event registrations
Function UnregisterForAllMenuOpenCloseEvents() native

; Remove all radiation damage event registrations
Function UnregisterForAllRadiationDamageEvents() native

; Remove all remote event registrations
Function UnregisterForAllRemoteEvents() native

; Remove all tracked stat event registrations
Function UnregisterForAllTrackedStatsEvents() native

; Unregister for the specified animation event from the specified object
Function UnregisterForAnimationEvent(ObjectReference akSender, string asEventName) native

; Unregister for the specified custom event from the specified object
Function UnregisterForCustomEvent(ScriptObject akSender, CustomEventName asEventName) native

; Unregister for any distance events between the two objects (which are interchangable)
Function UnregisterForDistanceEvents(ScriptObject akObj1, ScriptObject akObj2) native

; Unregister for the specified hit event.
Function UnregisterForHitEvent(ScriptObject akTarget, ScriptObject akAggressorFilter = None, Form akSourceFilter = None, Form akProjectileFilter = None, \
	int aiPowerFilter = -1, int aiSneakFilter = -1, int aiBashFilter = -1, int aiBlockFilter = -1, bool abMatch = true) native

; Unregister for any LOS events between the viewer and target
Function UnregisterForLOS(ObjectReference akViewer, ObjectReference akTarget) native

; Unregister for the specified magic effect apply event.
Function UnregisterForMagicEffectApplyEvent(ScriptObject akTarget, ScriptObject akCasterFilter = None, Form akEffectFilter = None, bool abMatch = true) native

; Unregister for the specified menu open/close event
Function UnregisterForMenuOpenCloseEvent(string asMenuName) native

; Unregisters this form to receive events when the player sleeps and wakes up
Function UnregisterForPlayerSleep() native

; Unregisters this form to receive events when the player teleports
Function UnregisterForPlayerTeleport() native

; Unregisters this form to receive events when the player waits
Function UnregisterForPlayerWait() native

; Unregister for the specified radiation damage event
Function UnregisterForRadiationDamageEvent(ScriptObject akTarget) native

; Unregister this script for the specified event from the specified sender
Function UnregisterForRemoteEvent(ScriptObject akEventSource, ScriptEventName asEventName) native

; Unregisters this script from receiving an event for the specified stat
Function UnregisterForTrackedStatsEvent(string asStat) native

; Unregisters from receiving events from Looks menu
Function UnregisterForLooksMenuEvent() native

; Unregisters from receiving tutorial-related events
Function UnregisterForTutorialEvent( String asEventName ) native

; Animation event, sent when an object we are listening to hits one of the events we are listening for
Event OnAnimationEvent(ObjectReference akSource, string asEventName)
EndEvent

; Event sent when you have been unregistered from receiving an animation event because the target
; object's animation graph has been unloaded
Event OnAnimationEventUnregistered(ObjectReference akSource, string asEventName)
EndEvent

; Event called after the script's state changes - passing in the old state
; Not called on auto states when the script is first initialized
Event OnBeginState(string asOldState)
EndEvent

; Distance event - sent when the two objects are less then the registered distance apart.
; Obj1 and Obj2 are interchangable.
Event OnDistanceLessThan(ObjectReference akObj1, ObjectReference akObj2, float afDistance)
EndEvent

; Distance event - sent when the two objects are greater then the registered distance apart.
; Obj1 and Obj2 are interchangable.
Event OnDistanceGreaterThan(ObjectReference akObj1, ObjectReference akObj2, float afDistance)
EndEvent

; Event called before the script's state changes - passing in the new state
Event OnEndState(string asNewState)
EndEvent

; LOS event, sent whenever the viewer first sees the target (after registering)
Event OnGainLOS(ObjectReference akViewer, ObjectReference akTarget)
EndEvent

; Event received when the target is hit by a source (weapon, spell, explosion) or projectile attack (after registering)
Event OnHit(ObjectReference akTarget, ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, \
	bool abBashAttack, bool abHitBlocked, string asMaterialName)
EndEvent

; Event sent as soon as all of the scripts properties have received a value, and before any other
; event can be received. This should be used to set up any internal variables you have, but keep
; in mind that the world may not exist or be completely loaded yet.
Event OnInit()
EndEvent

; Lost LOS event, sent whenever the viewer first loses sight of the target (after registering)
Event OnLostLOS(ObjectReference akViewer, ObjectReference akTarget)
EndEvent

; Event received when a magic affect is being applied to the target (after registering)
Event OnMagicEffectApply(ObjectReference akTarget, ObjectReference akCaster, MagicEffect akEffect)
EndEvent

; Event received when a menu opens/closes
Event OnMenuOpenCloseEvent(string asMenuName, bool abOpening)
EndEvent

; Received when the player sleeps. Start and desired end time are in game time days (after registering)
Event OnPlayerSleepStart(float afSleepStartTime, float afDesiredSleepEndTime, ObjectReference akBed)
EndEvent

; Received when the player stops sleeping - whether naturally or interrupted (after registering)
Event OnPlayerSleepStop(bool abInterrupted, ObjectReference akBed)
EndEvent

; Received when the player teleports (load door, fast travel, moveto) (after registering)
Event OnPlayerTeleport()
EndEvent

; Received when the player waits. Start and desired end time are in game time days (after registering)
Event OnPlayerWaitStart(float afWaitStartTime, float afDesiredWaitEndTime)
EndEvent

; Received when the player stops waiting - whether naturally or interrupted (after registering)
Event OnPlayerWaitStop(bool abInterrupted)
EndEvent

; Event received when a target takes radiation damage
Event OnRadiationDamage(ObjectReference akTarget, bool abIngested)
EndEvent

; Timer event, sent when a timer on this script expires. The ID of the expired timer is
; passed in as the parameter. This event is sent only once for each timer that expires.
Event OnTimer(int aiTimerID)
EndEvent

; Timer event, sent when a game-time timer on this script expires. The ID of the expired
; timer is passed in as the parameter. This event is sent only once for each timer that
; expires.
Event OnTimerGameTime(int aiTimerID)
EndEvent

; Event received when a tracked stat is updated for the player
Event OnTrackedStatsEvent(string arStatName, int aiStatValue)
EndEvent

; Event sent from the looks menu to cue dialogue
; 0 = General, uncontextualized banter
; 1 = Eye-related banter
; 2 = Nose-related banter
; 3 = Mouth-related banter
; 4 = Hair-related banter
; 5 = Beard-related banter
Event OnLooksMenuEvent(int aiFlavor)
EndEvent

; Occurrence of a tutorial-related event
Event OnTutorialEvent(String asEventName, Message aMessage)
EndEvent