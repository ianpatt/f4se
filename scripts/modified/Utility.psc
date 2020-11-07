
; The following functions are prone to cyclic references and may break games
; Cyclic reference example:
; Var[] c = new Var[1]
; c[0] = VarArrayToVar(c)
; variable c now has a reference to itself which may cause undefined 
; behavior when the game saves this information to the save game

; Converts Var to Var[]
Var[] Function VarToVarArray(Var v) native global

; Converts Var[] to Var
Var Function VarArrayToVar(Var[] v) native global