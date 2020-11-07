Scriptname FavoritesManager Native Hidden
; All the following functions are latent

; Returns all forms that are tagged for search
Form[] Function GetTaggedForms() global native

; Adds all forms in the array to tagged search (Be careful what form types you add, some may crash)
Function AddTaggedForms(Form[] forms) global native

; Removes all forms in the array from tagged search
Function RemoveTaggedForms(Form[] forms) global native

; Returns whether the specified form is tagged for search
bool Function IsTaggedForm(Form akForm) global native

; Returns all 12 favorites, may contain None entries
Form[] Function GetFavorites() global native

; Sets all 12 favorites, may contain None entries
Function SetFavorites(Form[] favorites) global native