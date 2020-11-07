Scriptname InstanceData Native Hidden

int Property Flag_IgnoresNormalResist	= 0x0000002 AutoReadOnly
int Property Flag_MinorCrime			= 0x0000004 AutoReadOnly
int Property Flag_ChargingReload		= 0x0000008 AutoReadOnly
int Property Flag_HideBackpack			= 0x0000010 AutoReadOnly
int Property Flag_NonHostile			= 0x0000040 AutoReadOnly
int Property Flag_NPCsUseAmmo			= 0x0000200 AutoReadOnly
int Property Flag_RepeatableSingleFire	= 0x0000800 AutoReadOnly
int Property Flag_HasScope				= 0x0001000 AutoReadOnly
int Property Flag_HoldInputToPower		= 0x0002000 AutoReadOnly
int Property Flag_Automatic				= 0x0004000 AutoReadOnly
int Property Flag_CantDrop				= 0x0008000 AutoReadOnly
int Property Flag_ChargingAttack		= 0x0010000 AutoReadOnly
int Property Flag_NotUsedInNormalCombat	= 0x0020000 AutoReadOnly
int Property Flag_BoundWeapon			= 0x0040000 AutoReadOnly
int Property Flag_SecondaryWeapon		= 0x0200000 AutoReadOnly
int Property Flag_BoltAction			= 0x0400000 AutoReadOnly
int Property Flag_NoJamAfterReload		= 0x0800000 AutoReadOnly
int Property Flag_DisableShells			= 0x1000000 AutoReadOnly

struct Owner
	Form owner			; Can be owning Actor/Weapon/Armor
	int slotIndex		; If owning actor, slotIndex must be provided
EndStruct


int Function GetAttackDamage(Owner akOwner) native global
Function SetAttackDamage(Owner akOwner, int damage) native global

struct DamageTypeInfo
	Form type
	int damage
EndStruct

; Works for both Armor and Weapon instances
; Gets/sets this Weapon or Armor's base damage type and damage value
DamageTypeInfo[] Function GetDamageTypes(Owner akOwner) native global
Function SetDamageTypes(Owner akOwner, DamageTypeInfo[] dts) native global

int Function GetAmmoCapacity(Owner akOwner) native global
Function SetAmmoCapacity(Owner akOwner, int capacity) native global

Ammo Function GetAmmo(Owner akOwner) native global
Function SetAmmo(Owner akOwner, Ammo akAmmo) native global

LeveledItem Function GetAddAmmoList(Owner akOwner) native global
Function SetAddAmmoList(Owner akOwner, LeveledItem akAmmo) native global

int Function GetAccuracyBonus(Owner akOwner) native global
Function SetAccuracyBonus(Owner akOwner, int bonus) native global

float Function GetActionPointCost(Owner akOwner) native global
Function SetActionPointCost(Owner akOwner, float cost) native global

float Function GetAttackDelay(Owner akOwner) native global
Function SetAttackDelay(Owner akOwner, float delay) native global

float Function GetOutOfRangeMultiplier(Owner akOwner) native global
Function SetOutOfRangeMultiplier(Owner akOwner, float mult) native global

float Function GetReloadSpeed(Owner akOwner) native global
Function SetReloadSpeed(Owner akOwner, float speed) native global

float Function GetReach(Owner akOwner) native global
Function SetReach(Owner akOwner, float reach) native global

float Function GetMinRange(Owner akOwner) native global
Function SetMinRange(Owner akOwner, float minRange) native global

float Function GetMaxRange(Owner akOwner) native global
Function SetMaxRange(Owner akOwner, float maxRange) native global

float Function GetSpeed(Owner akOwner) native global
Function SetSpeed(Owner akOwner, float speed) native global

int Function GetStagger(Owner akOwner) native global
Function SetStagger(Owner akOwner, int stagger) native global

ActorValue Function GetSkill(Owner akOwner) native global
Function SetSkill(Owner akOwner, ActorValue skill) native global

ActorValue Function GetResist(Owner akOwner) native global
Function SetResist(Owner akOwner, ActorValue resist) native global

; Gets/sets the weapons crit multiplier
float Function GetCritMultiplier(Owner akOwner) native global
Function SetCritMultiplier(Owner akOwner, float crit) native global

; Gets/sets the weapons crit charge bonus
float Function GetCritChargeBonus(Owner akOwner) native global
Function SetCritChargeBonus(Owner akOwner, float bonus) native global

Projectile Function GetProjectileOverride(Owner akOwner) native global
Function SetProjectileOverride(Owner akOwner, Projectile proj) native global

int Function GetNumProjectiles(Owner akOwner) native global
Function SetNumProjectiles(Owner akOwner, int numProj) native global

float Function GetSightedTransition(Owner akOwner) native global
Function SetSightedTransition(Owner akOwner, float seconds) native global

bool Function GetFlag(Owner akOwner, int flag) native global
Function SetFlag(Owner akOwner, int flag, bool set) native global

; Only works for Armor instances
int Function GetArmorHealth(Owner akOwner) native global
Function SetArmorHealth(Owner akOwner, int health) native global

; Only works for Armor instances
int Function GetArmorRating(Owner akOwner) native global
Function SetArmorRating(Owner akOwner, int health) native global

; Works for both Armor and Weapon instances
float Function GetWeight(Owner akOwner) native global
Function SetWeight(Owner akOwner, float weight) native global

; Works for both Armor and Weapon instances
int Function GetGoldValue(Owner akOwner) native global
Function SetGoldValue(Owner akOwner, int value) native global

; Works for both Armor and Weapon instances
Keyword[] Function GetKeywords(Owner akOwner) native global
Function SetKeywords(Owner akOwner, Keyword[] kwds) native global