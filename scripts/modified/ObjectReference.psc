; Returns all the mods for this reference
ObjectMod[] Function GetAllMods() native

; If this object is a BendableSpline it will link to the two ObjectReferences it is connect to, otherwise it will link to other BendableSplines
ObjectReference[] Function GetConnectedObjects() native

; Attachs a BendableSpline to two other references
; Restrictions:
;	- akRef must not be None
;	- akRef must not be the same as the caller (i.e. no wire to itself)
;	- Spline must be a BendableSpline (None will use FormID 0001D971)
;	- Calling object must be linked to a Workshop (Uses keyword 00054BA6)
;	- Calling object must not be another BendableSpline (i.e. no wire to wire)
; Returns the newly created wire ref (The wire ref is disabled)
ObjectReference Function AttachWire(ObjectReference akRef, Form spline = None) native

; Convenience function to AttachWire to both create the wire and enable it
ObjectReference Function CreateWire(ObjectReference akRef, Form spline = None)
	ObjectReference wireRef = self.AttachWire(akRef, spline)
	If wireRef
		wireRef.EnableNoWait()
	Endif
	return wireRef
EndFunction

; Scraps this object reference and disconnects any wires if necessary
; This function is debug only as it is not ready for release, it scraps
; but it has problems unpowering connected objects, also doesn't give
; the materials back yet
bool Function Scrap(ObjectReference akWorkshop) native

; Returns the display name of this reference
string Function GetDisplayName() native

; Returns all the base forms from the inventory
Form[] Function GetInventoryItems() native

; Returns the total weight of this objects inventory
float Function GetInventoryWeight() native

struct ConnectPoint
	string parent
	string name
	float roll
	float pitch
	float yaw
	float x
	float y
	float z
	float scale
	ObjectReference object
EndStruct

; Returns connect points to an object, if used on wired objects you may only 
; get one wire ref unless the object has multiple attach points
; 'object' is not None when there is another object directly connected
; to the attach point that also has a workshop connect point
; Object 3d must be loaded for this function to work at all
ConnectPoint[] Function GetConnectPoints() native

; Links power to any objects connected by attach point (The two attach points are touching)
; e.g. progammatically placed conduits
; Should be called on each programmatically placed conduit/junction/radiatior
bool Function TransmitConnectedPower() native

; Returns the source/target pairs that succeeded in swapping
; Rename material allows independent control over
; [General]
; bRenamePropertiesOnMaterialSwap=0
MatSwap:RemapData[] Function ApplyMaterialSwap(MatSwap mSwap, bool renameMaterial = false) native

; Sets the internal persistent material swap
; Adding persistent material swaps will increase save-size by a small amount
; Setting this to None will remove persistence and free up save-space
; This function does not apply a visual update, you must use ApplyMaterialSwap
; This will make the game take care of setting the material swap when the game is loaded
; ApplyMaterialSwap is a more greedy method of applying materials so the nif objects
; the game applies materials to may not match up with the ApplyMaterialSwap function
; Behavior may not be as expected when applied to living references, weapons, or armor placed in the world
Function SetMaterialSwap(MatSwap mSwap) native

; Gets the internal persistent material swap
; ObjectReferences placed in the world via Editor may have this already filled
MatSwap Function GetMaterialSwap() native