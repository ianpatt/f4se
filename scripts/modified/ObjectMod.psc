
; Boolean type, only accepts
; Operator: Set/And/Or
; value1
Group TypeBool
	int Property Weapon_Target_bAlternateRumble = 22 AutoReadOnly
	int Property Weapon_Target_bAutomatic = 25 AutoReadOnly
	int Property Weapon_Target_bBoltAction = 81 AutoReadOnly
	int Property Weapon_Target_bCantDrop = 26 AutoReadOnly
	int Property Weapon_Target_bChargingAttack = 93 AutoReadOnly
	int Property Weapon_Target_bChargingReload = 18 AutoReadOnly
	int Property Weapon_Target_bDisableShells = 92 AutoReadOnly
	int Property Weapon_Target_bEffectOnDeath = 21 AutoReadOnly
	int Property Weapon_Target_bFixedRange = 20 AutoReadOnly
	int Property Weapon_Target_bHasScope = 48 AutoReadOnly
	int Property Weapon_Target_bHoldInputToPower = 85 AutoReadOnly
	int Property Weapon_Target_bIgnoreResist = 24 AutoReadOnly
	int Property Weapon_Target_bMinorCrime = 19 AutoReadOnly
	int Property Weapon_Target_bNonHostile = 23 AutoReadOnly
	int Property Weapon_Target_bNonPlayable = 27 AutoReadOnly
	int Property Weapon_Target_bNPCsUseAmmo = 17 AutoReadOnly
	int Property Weapon_Target_bPlayerOnly = 16 AutoReadOnly
	int Property Weapon_Target_bRepeatableSingleFire = 86 AutoReadOnly
EndGroup

; Integer type, only accepts
; Operator: Set/Add/Mul-Add
; value1
; value2
Group TypeInteger
	int Property Weapon_Target_iAmmoCapacity = 12 AutoReadOnly
	int Property Weapon_Target_iAttackDamage = 28 AutoReadOnly
	int Property Weapon_Target_iRank = 10 AutoReadOnly
	int Property Weapon_Target_iValue = 29 AutoReadOnly

	int Property Weapon_Target_uAimModelConeDecreaseDelayMs = 37 AutoReadOnly
	int Property Weapon_Target_uAimModelRecoilShotsForRunaway = 44 AutoReadOnly
	int Property Weapon_Target_uNumProjectiles = 51 AutoReadOnly

	int Property Armor_Target_iHealth = 267 AutoReadOnly
	int Property Armor_Target_iRating = 262 AutoReadOnly
	int Property Armor_Target_iValue = 261 AutoReadOnly

	int Property Actor_Target_iXPOffset = 514 AutoReadOnly
EndGroup

; Float type, only accepts
; Operator: Set/Add/Mul-Add
; value1
; value2
Group TypeFloat
	int Property Weapon_Target_fAimModelBaseStability = 66 AutoReadOnly
	int Property Weapon_Target_fAimModelConeDecreasePerSec = 36 AutoReadOnly
	int Property Weapon_Target_fAimModelConeIncreasePerShot = 35 AutoReadOnly
	int Property Weapon_Target_fAimModelConeIronSightsMultiplier = 47 AutoReadOnly
	int Property Weapon_Target_fAimModelConeSneakMultiplier = 38 AutoReadOnly
	int Property Weapon_Target_fAimModelMaxConeDegrees = 34 AutoReadOnly
	int Property Weapon_Target_fAimModelMinConeDegrees = 33 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilArgDeg = 45 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilArgRotateDeg = 46 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilDiminishSightsMult = 40 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilDiminishSpringForce = 39 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilHipMult = 43 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilMaxDegPerShot = 41 AutoReadOnly
	int Property Weapon_Target_fAimModelRecoilMinDegPerShot = 42 AutoReadOnly
	int Property Weapon_Target_fAttackActionPointCost = 79 AutoReadOnly
	int Property Weapon_Target_fAttackDelaySec = 4 AutoReadOnly
	int Property Weapon_Target_fColorRemappingIndex = 88 AutoReadOnly
	int Property Weapon_Target_fCriticalChargeBonus = 8 AutoReadOnly
	int Property Weapon_Target_fCriticalDamageMult = 90 AutoReadOnly
	int Property Weapon_Target_fFireSeconds = 50 AutoReadOnly
	int Property Weapon_Target_fFullPowerSeconds = 84 AutoReadOnly
	int Property Weapon_Target_fMaxRange = 3 AutoReadOnly
	int Property Weapon_Target_fMinPowerPerShot = 87 AutoReadOnly
	int Property Weapon_Target_fMinRange = 2 AutoReadOnly
	int Property Weapon_Target_fOutOfRangeDamageMult = 6 AutoReadOnly
	int Property Weapon_Target_fReach = 1 AutoReadOnly
	int Property Weapon_Target_fReloadSpeed = 76 AutoReadOnly
	int Property Weapon_Target_fSecondaryDamage = 7 AutoReadOnly
	int Property Weapon_Target_fSightedTransitionSeconds = 83 AutoReadOnly
	int Property Weapon_Target_fSoundLevelMult = 74 AutoReadOnly
	int Property Weapon_Target_fSpeed = 0 AutoReadOnly
	int Property Weapon_Target_fUnused = 5 AutoReadOnly
	int Property Weapon_Target_fWeight = 30 AutoReadOnly
	int Property Weapon_Target_fZoomDataCameraOffsetX = 70 AutoReadOnly
	int Property Weapon_Target_fZoomDataCameraOffsetY = 71 AutoReadOnly
	int Property Weapon_Target_fZoomDataCameraOffsetZ = 72 AutoReadOnly

	int Property Armor_Target_fColorRemappingIndex = 268 AutoReadOnly
	int Property Armor_Target_fWeight = 260 AutoReadOnly

	int Property Actor_Target_fColorRemappingIndex = 516 AutoReadOnly
EndGroup

; Form type, only accepts
; Operator: Set/Add/Rem
; object
Group TypeForm
	int Property Weapon_Target_paAimModel = 32 AutoReadOnly
	int Property Weapon_Target_peEnchantments = 65 AutoReadOnly
	int Property Weapon_Target_pgZoomDataImageSpace = 69 AutoReadOnly
	int Property Weapon_Target_piBashImpactDataSet = 63 AutoReadOnly
	int Property Weapon_Target_piImpactDataSet = 60 AutoReadOnly
	int Property Weapon_Target_pkKeywords = 31 AutoReadOnly
	int Property Weapon_Target_plCritEffect = 62 AutoReadOnly
	int Property Weapon_Target_pmBlockMaterial = 64 AutoReadOnly
	int Property Weapon_Target_pnNPCAmmoList = 75 AutoReadOnly
	int Property Weapon_Target_poAmmo = 61 AutoReadOnly
	int Property Weapon_Target_ppOverrideProjectile = 80 AutoReadOnly
	int Property Weapon_Target_psAttackFailSound = 55 AutoReadOnly
	int Property Weapon_Target_psAttackLoop = 54 AutoReadOnly
	int Property Weapon_Target_psAttackSound = 52 AutoReadOnly
	int Property Weapon_Target_psAttackSound2D = 53 AutoReadOnly
	int Property Weapon_Target_psEquipSound = 57 AutoReadOnly
	int Property Weapon_Target_psFastEquipSound = 91 AutoReadOnly
	int Property Weapon_Target_psIdleSound = 56 AutoReadOnly
	int Property Weapon_Target_psUnEquipSound = 58 AutoReadOnly
	int Property Weapon_Target_ptEquipSlot = 73 AutoReadOnly
	int Property Weapon_Target_pwMaterialSwaps = 89 AutoReadOnly
	int Property Weapon_Target_pzZoomData = 67 AutoReadOnly

	int Property Armor_Target_peEnchantments = 256 AutoReadOnly
	int Property Armor_Target_piBashImpactDataSet = 257 AutoReadOnly
	int Property Armor_Target_pkKeywords = 259 AutoReadOnly
	int Property Armor_Target_pmBlockMaterial = 258 AutoReadOnly
	int Property Armor_Target_pwMaterialSwaps = 269 AutoReadOnly

	int Property Actor_Target_peEnchantments = 515 AutoReadOnly
	int Property Actor_Target_piForcedInventory = 513 AutoReadOnly
	int Property Actor_Target_pkKeywords = 512 AutoReadOnly
	int Property Actor_Target_pwMaterialSwaps = 517 AutoReadOnly
EndGroup

; Form float type, only accepts
; Operator: Set/Add/Mul-Add
; object
; value1
Group TypeFormFloat
	int Property Weapon_Target_vaActorValues = 94 AutoReadOnly
	int Property Weapon_Target_vdDamageTypeValues = 77 AutoReadOnly

	int Property Armor_Target_vaActorValues = 266 AutoReadOnly
	int Property Armor_Target_vdDamageTypeValues = 265 AutoReadOnly
EndGroup

; Enum type, only accepts
; Operator: Set
; value1
Group TypeEnum
	int Property Weapon_Target_ehHitBehavior = 9 AutoReadOnly
	int Property Weapon_Target_elSoundLevel = 59 AutoReadOnly
	int Property Weapon_Target_eoZoomDataOverlay = 68 AutoReadOnly
	int Property Weapon_Target_esStaggerValue = 82 AutoReadOnly
	int Property Weapon_Target_ewType = 15 AutoReadOnly
EndGroup

int Property Modifier_Operator_Set = 0 AutoReadOnly
int Property Modifier_Operator_Add = 1 AutoReadOnly
int Property Modifier_Operator_Mult_Add = 2 AutoReadOnly
int Property Modifier_Operator_And = 3 AutoReadOnly
int Property Modifier_Operator_Or = 4 AutoReadOnly
int Property Modifier_Operator_Rem = 5 AutoReadOnly

struct PropertyModifier
	int target
	int operator
	Form object ; Depends on target
	float value1 ; Depends on target
	float value2 ; Depends on target
EndStruct

PropertyModifier[] Function GetPropertyModifiers() native

int Function GetMaxRank() native
Function SetMaxRank(int rank) native

int Function GetPriority() native
Function SetPriority(int priority) native

; returns the loose misc object
MiscObject Function GetLooseMod() native