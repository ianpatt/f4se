Scriptname UI Native Hidden

bool Function IsMenuOpen(string menu) native global

; Returns whether the menu is registered
bool Function IsMenuRegistered(string menu) native global

struct MenuData
	int menuFlags = 0x801849D
	int movieFlags = 3
	int extendedFlags = 3
	int depth = 6
endStruct

bool Function RegisterBasicCustomMenu(string menuName, string menuPath, string rootPath) global
	MenuData mData = new MenuData
	return RegisterCustomMenu(menuName, menuPath, rootPath, mData)
EndFunction

; Registers a custom menu to be opened later
; e.g.
; UI.RegisterBasicCustomMenu("MyMenu", "TestMenu", "root1.Menu_mc")
; UI.OpenMenu("MyMenu")
; This would open Interface/TestMenu.swf and the root of the menu would be stage.Menu_mc
bool Function RegisterCustomMenu(string menuName, string menuPath, string rootPath, MenuData mData) native global

; Return value does not indicate the menu is ready, only that it sent the message
bool Function OpenMenu(string menuName) native global

; Return value does not indicate the menu is closed, only that it sent the message
bool Function CloseMenu(string menuName) native global

; Sets a variable in AS3 to the newly assigned type
; Can accept arrays using VarArrayToVar
bool Function Set(string menu, string path, Var arg) native global

; Returns a variable from AS3 to Papyrus
; Can return arrays using VarToVarArray
Var Function Get(string menu, string path) native global

; Top-level arguments are converted to their actual types, empty Vars are undefined
; Vars, Structs, and Forms are converted to AS3 objects
; Var example:
; {"__var__": 1.0}
; Struct Example:
; {"__struct__": {"__type__": "scriptowner#mystruct", "__data__": {"member1": 1.0, "member2": false}}}
; Form Example:
; {"__type__": "ScriptObject", "__handleHigh__": 0x0000FFFF, "__handleLow__": 0x00000000}
; Forms shouldn't be manipulated from AS3, they are container objects meant to be passed back from AS3 to Papyrus
Var Function Invoke(string menu, string path, Var[] args = None) native global

; Loads an asset (swf) as a child to the target variable
; Target variable must be a Container (or has the addChild function)
; This function will create a Loader object, add the Loader to the target
; Then load the asset as a child to the Loader, this function is not immediate
bool Function Load(string menu, string sourceVar, string assetPath, ScriptObject receiver = None, string callback = "") native global


;; Load Example:
; Scriptname MyScript extends ScriptObject ; Or any other type
; Function TestFunc()
;     ; Paths are already relative to Interface/ when you do a load
;     ; root1 is used here because "root" may already be cached as "Root" in Papyrus, and Scaleform is case-sensitive
;     ; root and root1 are the same object, the toplevel MovieClip that is not the stage (this is not the same as AS2's _root)
;     ; Self refers to MyScript
;     ; "OnLoadComplete" is the function that will be called on MyScript to notify that the clip is loaded (or not) and where it loaded to and from
;     UI.Load("HUDMenu", "root1", "something.swf", self, "OnLoadComplete")
; EndFunction

;; The parameters to this function are fixed (bool,string,string,string,string) but the function name can be changed by UI.Load
; Function OnLoadComplete(bool success, string menuName, string sourceVar, string destVar, string assetPath)
;     ; success = true/false if it failed or not
;     ; menuName = "HUDMenu"
;     ; sourceVar = "root1"
;     ; destVar = "root1.instanceX.instanceY" (non-empty on success) -- This is manually extracted by going up to parent until there are no more named parents, X and Y are incrementally assigned numbers but these are basically indeterminate
;     ; assetPath = "something.swf"
; EndFunction


;; Array of Arrays
;; example:
; Var[] a = new Var[3]
; a[0] = 1.0
; a[1] = "Str1"
; Var[] b = new Var[2]
; b[0] = Game.GetForm(0xF)
; b[1] = Game.GetForm(0x7)
; a[2] = Utility.VarArrayToVar(b)
; UI.Invoke("HUDMenu", "root.func", a)
;; AS3 expansion becomes:
;; func(Number=1.0, String="Str1", Array=[{"__type__": "MiscObject", "__handleHigh__": 0x0000FFFF, "__handleLow__": 0x0000000F}, {"__type__": "ActorBase", "__handleHigh__": 0x0000FFFF, "__handleLow__": 0x00000007}])

;; Scaleform to Papyrus
;; Top-level parameters are expanded directly as parameters

;; Scaleform Code:
; root.f4se.SendExternalEvent("myeventname", 1.0, "Str1", [{"__type__": "MiscObject", "__handleHigh__": 0x0000FFFF, "__handleLow__": 0x0000000F}, {"__type__": "ActorBase", "__handleHigh__": 0x0000FFFF, "__handleLow__": 0x00000007}])

;; Papyrus code:
;; Must have previously registered for the event and established a callback name
; RegisterForExternalEvent("myeventname", "OnMyCallback")
;
; Event OnMyCallback(float a1, string a2, Var[] a3)
;     MiscObject myMisc = a3[0] as MiscObject ; Cast as it is currently a Var
;     ActorBase myABase = a3[1] as ActorBase
; EndEvent