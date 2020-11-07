Scriptname Input Native Hidden

; returns keycode bound to a control for given device
;
; Valid controls:
;	"Forward", "Back", "Strafe Left", "Strafe Right", "Move", "Look", "Left Attack/Block", "Right Attack/Block"
;	"Activate", "Ready Weapon", "Tween Menu", "Toggle POV", "Zoom Out", "Zoom In", "Jump", "Sprint", "Shout",
;	"Sneak", "Run", "Toggle Always Run", "Auto-Move", "Favorites", "Hotkey1", "Hotkey2", "Hotkey3", "Hotkey4",
;	"Hotkey5", "Hotkey6", "Hotkey7", "Hotkey8", "Quicksave", "Quickload", "Wait", "Journal", "Pause", "Screenshot",
;	"Multi-Screenshot", "Console", "CameraPath", "Quick Inventory", "Quick Magic", "Quick Stats", "Quick Map"
;
; Valid device types:
;	(default)	auto detect
;	0			keyboard
;	1			mouse
;	2			gamepad
int Function GetMappedKey(string control, int deviceType = 0xFF) global native

; returns name of control bound to given keycode, or "" if unbound
string Function GetMappedControl(int keycode) global native