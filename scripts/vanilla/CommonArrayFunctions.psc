Scriptname CommonArrayFunctions
{Holds common functions for dealing with arrays of various types}

;REMINDERS
;A Zero Length array is treated as NONE


;jduvall
bool Function CheckObjectReferenceAgainstArray(ObjectReference ObjectToCheck, ObjectReference[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false) global

;	Debug.Trace("CommonArrayFunctions.CheckObjectReferenceAgainstArray() ObjectToCheck: " + ObjectToCheck + " ArrayToCheck: " + ArrayToCheck)
	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE, we assume you should fail to find what you are looking for.

	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true
	
	
	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectReferenceAgainstArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectReferenceAgainstArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	int findVal = ArrayToCheck.find(ObjectToCheck)
	bool returnVal = false

	if findVal >= 0
		returnVal = true
	endif

;	Debug.Trace("CommonArrayFunctions.CheckObjectReferenceAgainstArray() returning: " + returnVal)


	RETURN returnVal

EndFunction

;kmk
int Function FindInReferenceAliasArray(ObjectReference ObjectToCheck, ReferenceAlias[] ArrayToCheck) global
	;if ObjectToCheck is NONE, will return -1
	;if ArrayToCheck = NONE, will return -1

	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectReferenceAgainstReferenceAliasArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN -1
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectReferenceAgainstReferenceAliasArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN -1
	endif

	int returnVal = -1
	if ObjectToCheck
		int i = 0
		while (returnVal == -1 && i < ArrayToCheck.length)
			if ArrayToCheck[i].GetReference() == ObjectToCheck
				returnVal = i
			endif
			i += 1
		endwhile
	endif


	;Debug.Trace("CommonArrayFunctions.FindInReferenceAliasArray() returning: " + returnVal)

	RETURN returnVal

EndFunction

;jduvall
bool Function CheckObjectReferenceAgainstReferenceAliasArray(ObjectReference ObjectToCheck, ReferenceAlias[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false) global

	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE, we assume you should fail to find what you are looking for.

	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true
	
	
	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectReferenceAgainstReferenceAliasArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectReferenceAgainstReferenceAliasArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	bool returnVal = FindInReferenceAliasArray(ObjectToCheck, ArrayToCheck) > -1

;	Debug.Trace("CommonArrayFunctions.CheckObjectReferenceAgainstReferenceAliasArray() returning: " + returnVal)

	RETURN returnVal

EndFunction



;jduvall
bool Function CheckActorAgainstFactionArray(Actor ObjectToCheck, Faction[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false) global

	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE, we assume you should fail to find what you are looking for.

	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true
	
	
	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckActorAgainstFactionArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckActorAgainstFactionArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	bool returnVal = false
	if ObjectToCheck
		int i = 0
		while (returnVal == false && i < ArrayToCheck.length)
			returnVal = ObjectToCheck.IsInFaction(ArrayToCheck[i])
			i += 1
		endwhile
	endif


;	Debug.Trace("CommonArrayFunctions.CheckActorAgainstFactionArray() returning: " + returnVal)

	RETURN returnVal

EndFunction


;jduvall
bool Function CheckObjectAgainstKeywordArray(ObjectReference ObjectToCheck, Keyword[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false) global

	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE,  we assume you should fail to find what you are looking for.
	
	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true

	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectAgainstKeywordArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectAgainstKeywordArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	bool returnVal = false
	if ObjectToCheck
		int i = 0
		while (returnVal == false && i < ArrayToCheck.length)
			returnVal = ObjectToCheck.HasKeyword(ArrayToCheck[i])
			i += 1
		endwhile
	endif


;	Debug.Trace("CommonArrayFunctions.CheckObjectAgainstKeywordArray() returning: " + returnVal)

	RETURN returnVal

EndFunction

;jduvall
bool Function CheckFormAgainstKeywordArray(Form ObjectToCheck, Keyword[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false) global

	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE,  we assume you should fail to find what you are looking for.
	
	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true

	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectAgainstKeywordArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckObjectAgainstKeywordArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	bool returnVal = false
	if ObjectToCheck
		int i = 0
		while (returnVal == false && i < ArrayToCheck.length)
			returnVal = ObjectToCheck.HasKeyword(ArrayToCheck[i])
			i += 1
		endwhile
	endif


;	Debug.Trace("CommonArrayFunctions.CheckObjectAgainstKeywordArray() returning: " + returnVal)

	RETURN returnVal

EndFunction


;dbrigner
bool Function CheckFormAgainstArray(Form FormToCheck, Form[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false) global

	;!!! IMPORTANT ASSUMPTIONS
	;if FormToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE, we assume you should fail to find what you are looking for.

	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true
	
	
	if FormToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckFormAgainstArray(): FormToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckFormAgainstArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	int findVal = ArrayToCheck.find(FormToCheck)
	bool returnVal = false

	if findVal >= 0
		returnVal = true
	endif


;	Debug.Trace("CommonArrayFunctions.CheckFormAgainstArray() returning: " + returnVal)

	RETURN returnVal

EndFunction

;kmk
bool Function CheckLocationAgainstArray(Location ObjectToCheck, Location[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false, bool matchIfChildLocation = false) global

;	Debug.Trace("CommonArrayFunctions.CheckLocationAgainstArray() ObjectToCheck: " + ObjectToCheck + " ArrayToCheck: " + ArrayToCheck)
	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE, we assume you should fail to find what you are looking for.

	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true
	
	; matchIfChildLocation: if true, will also consider a match if ObjectToCheck is a child of anything in ArrayToCheck
	
	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckLocationAgainstArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckLocationAgainstArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	int findVal = ArrayToCheck.find(ObjectToCheck)
	bool returnVal = false

	if findVal >= 0
		returnVal = true
	elseif matchIfChildLocation
		; we need to check if ObjectToCheck is a child of anything in ArrayToCheck
		int i = 0
		while (returnVal == false && i < ArrayToCheck.length)
			if ArrayToCheck[i].IsChild(ObjectToCheck)
			;	debug.trace("CommonArrayFunctions.CheckLocationAgainstArray() found that " + ObjectToCheck + " is child of " + ArrayToCheck[i])
				returnVal = true
			endif
			i = i + 1
		endWhile
	endif

;	Debug.Trace("CommonArrayFunctions.CheckLocationAgainstArray() returning: " + returnVal)


	RETURN returnVal

EndFunction

;kmk
bool Function CheckLocationAgainstLocationAliasArray(Location ObjectToCheck, LocationAlias[] ArrayToCheck, bool returnValueIfArrayIsEmpty = false, bool matchIfChildLocation = false) global

	;!!! IMPORTANT ASSUMPTIONS
	;if ObjectToCheck is NONE, will return FALSE
	;if ArrayToCheck = NONE, will return returnValueIfArrayIsEmpty (defaults to false, though you MIGHT want it to return true in such cases)

	;Rationale:
	;If you are checking on behalf of an object that is NONE, we assume you should fail to find what you are looking for.

	;A zero length array is treated as NONE - depending on circumstance, you may want that to be true or false
	;We assume this in error and so return false, but for cases where such an array might be an optional property, you might want this to return true
	
	; matchIfChildLocation: if true, will also consider a match if ObjectToCheck is a child of anything in ArrayToCheck
	
	if ObjectToCheck == NONE
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckLocationAgainstLocationAliasArray(): ObjectToCheck is NONE -- RETURNING: FALSE", 1)
		RETURN false
	elseif ArrayToCheck.length == 0
		;Debug.TraceStack("CAUTION! CommonArrayFunctions.CheckLocationAgainstLocationAliasArray(): ArrayToCheck is NONE -- RETURNING: " + returnValueIfArrayIsEmpty, 1)
		RETURN returnValueIfArrayIsEmpty
	endif

	bool returnVal = false
	if ObjectToCheck
		int i = 0
		while (returnVal == false && i < ArrayToCheck.length)
			returnVal = ArrayToCheck[i].GetLocation() == ObjectToCheck
			if !returnVal && matchIfChildLocation
				; check if ObjectToCheck is a child, if so return true
				returnVal = ArrayToCheck[i].GetLocation().IsChild(ObjectToCheck)
				if returnVal
				;	debug.trace("CommonArrayFunctions.CheckLocationAgainstLocationAliasArray() found that " + ObjectToCheck + " is child of " + ArrayToCheck[i].GetLocation())
				endif
			endif
			i += 1
		endwhile
	endif


;	Debug.Trace("CommonArrayFunctions.CheckLocationAgainstLocationAliasArray() returning: " + returnVal)

	RETURN returnVal

EndFunction

;jduvall
keyword Function GetFirstFoundKeywordInArrayForLocation(Location LocationToCheck, Keyword[] ArrayToCheck) Global
	
Debug.Trace("CommonArrayFunctions.GetFirstFoundKeywordInArrayForLocation() LocationToCheck: " + LocationToCheck + " ArrayToCheck: " + ArrayToCheck)

	keyword returnVal

	int i = 0
	while (i < ArrayToCheck.length && returnVal == None)
		
		if LocationToCheck.HasKeyword(ArrayToCheck[i])
			returnVal = ArrayToCheck[i]
		endif

		i += 1
	endwhile

;	Debug.Trace("CommonArrayFunctions.GetFirstFoundKeywordInArrayForLocation() returning: " + returnVal)

	RETURN ReturnVal

EndFunction

;jduvall
Faction Function GetFirstFoundFactionInArrayForActor(Actor ActorToCheck, Faction[] ArrayToCheck) Global
	
Debug.Trace("CommonArrayFunctions.GetFirstFoundFactionInArrayForActor() ActorToCheck: " + ActorToCheck + " ArrayToCheck: " + ArrayToCheck)

	Faction returnVal

	int i = 0
	while (i < ArrayToCheck.length && returnVal == None)
		
		if ActorToCheck.IsInFaction(ArrayToCheck[i])
			returnVal = ArrayToCheck[i]
		endif

		i += 1
	endwhile

;	Debug.Trace("CommonArrayFunctions.GetFirstFoundFactionInArrayForActor() returning: " + returnVal)

	RETURN ReturnVal

EndFunction


;jduvall
bool Function IsActorInArrayHostileToActor (Actor ActorToCheck, ObjectReference[] ArrayToCheck) Global
	
Debug.Trace("CommonArrayFunctions.IsActorInArrayHostileToActor () ActorToCheck: " + ActorToCheck + " ArrayToCheck: " + ArrayToCheck)

	bool returnVal = false

	int i = 0
	while (i < ArrayToCheck.length && returnVal == false)

		if ((ArrayToCheck[i] as Actor).IsHostileToActor(ActorToCheck))
			returnVal = true
		endif

		i += 1
	endwhile

;	Debug.Trace("CommonArrayFunctions.IsActorInArrayHostileToActor () returning: " + returnVal)

	RETURN ReturnVal

EndFunction