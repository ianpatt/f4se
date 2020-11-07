Scriptname LocationAlias extends Alias Native Hidden

; Clears the alias - fails on non-optional aliases
Function Clear() native

; Get the object reference this alias refers to
Location Function GetLocation() native

; Forces this alias to the specified location
Function ForceLocationTo(Location akNewLocation) native