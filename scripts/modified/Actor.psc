struct WornItem
	Form item ; Can be ARMO, WEAP or ARMA
	Form model ; Can be ARMA or WEAP depending on item
	string modelName ; Model override name
	Form materialSwap ; Material swap form
	TextureSet texture ; TextureSet override 
EndStruct

; Slot index is valid from 0-43
; Item can be none and still contain other information at a particular slot
; When the item is an ARMA, it means that the item takes multiple slots,
; and that particular slot is not where the ARMO lives
WornItem Function GetWornItem(int slotIndex, bool firstPerson = false) native

; Returns all the mods for the item at the particular index
; returns None if there is no item at this index
ObjectMod[] Function GetWornItemMods(int slotIndex) native

InstanceData:Owner Function GetInstanceOwner(int slotIndex)
	InstanceData:Owner inst = new InstanceData:Owner
	inst.owner = self
	inst.slotIndex = slotIndex
	return inst
EndFunction

; Updates the entire actor from the ground-up
; Using zero flag is very expensive, to update only the body use flags=0xC
Function QueueUpdate(bool bDoEquipment = false, int flags = 0) native

; Returns the current furniture reference
ObjectReference Function GetFurnitureReference() native