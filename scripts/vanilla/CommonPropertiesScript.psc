Scriptname CommonPropertiesScript extends Quest const
{Access these properties like this from any script without having set up any properties:
Game.GetCommonProperties().PropertyName}

Group Keywords

keyword Property ActorTypeNPC const auto mandatory
keyword Property ActorTypeSuperMutant const auto mandatory

keyword Property WeaponTypeMelee1H const auto mandatory
keyword Property WeaponTypeMelee2H const auto mandatory
keyword Property WeaponTypeUnarmed const auto mandatory

keyword Property AnimArchetypeConfident const auto mandatory
keyword Property AnimArchetypeDepressed const auto mandatory
keyword Property AnimArchetypeElderly const auto mandatory
keyword Property AnimArchetypeFriendly const auto mandatory
keyword Property AnimArchetypeIrritated const auto mandatory
keyword Property AnimArchetypeNervous const auto mandatory

keyword Property CA__CustomEvent_Generous const auto mandatory
keyword Property CA__CustomEvent_Mean const auto mandatory
keyword Property CA__CustomEvent_Nice const auto mandatory
keyword Property CA__CustomEvent_Peaceful const auto mandatory
keyword Property CA__CustomEvent_PeacefulFailed const auto mandatory
keyword Property CA__CustomEvent_Selfish const auto mandatory
keyword Property CA__CustomEvent_Violent const auto mandatory
keyword Property CA__Event_DonateItem const auto mandatory
keyword Property CA__Event_ReceiveReward const auto mandatory
keyword Property CA__Event_SpeechForMoreCaps const auto mandatory
keyword Property CA__Event_Murder const auto mandatory

keyword Property DogAnimArchetypeAgitated const auto mandatory
keyword Property DogAnimArchetypeAlert const auto mandatory
keyword Property DogAnimArchetypePlayful const auto mandatory

keyword Property LocTypeSettlement const auto mandatory
keyword Property LocTypeWorkshopSettlement const auto mandatory

EndGroup

Group Actors
Actor Property DogmeatRef const auto mandatory
Actor Property CaitRef const auto mandatory
Actor Property CodsworthRef const auto mandatory
Actor Property CurieRef const auto mandatory
Actor Property DanseRef const auto mandatory
Actor Property DeaconRef const auto mandatory
Actor Property HancockRef const auto mandatory
Actor Property MacCreadyRef const auto mandatory
Actor Property PiperRef const auto mandatory
Actor Property PrestonRef const auto mandatory
Actor Property StrongRef const auto mandatory
Actor Property ValentineRef const auto mandatory
Actor Property X688Ref const auto mandatory

EndGroup

Group ActorValues

ActorValue property CA_Affinity auto mandatory const
ActorValue property CA_CurrentThreshold auto mandatory const

ActorValue property FollowerDistance auto mandatory const
ActorValue property FollowerStance auto mandatory const
ActorValue property FollowerStanceAllowCombatOverride auto mandatory const
ActorValue property FollowerState auto mandatory const

ActorValue Property IdleChatterTimeMax const auto mandatory
ActorValue Property IdleChatterTimeMin const auto mandatory

ActorValue Property LeftAttackCondition auto mandatory const
ActorValue Property LeftMobilityCondition auto mandatory const
ActorValue Property PerceptionCondition auto mandatory const
ActorValue Property RightAttackCondition auto mandatory const
ActorValue Property RightMobilityCondition auto mandatory const

EndGroup

group Aliases

ReferenceAlias Property DogmeatCompanion const auto mandatory
{DogmeatCompanion in Followers quest}

EndGroup

Group Factions
Faction Property PlayerFaction const auto mandatory
Faction Property CurrentCompanionFaction const auto mandatory

EndGroup

Group Races

Race Property DogmeatRace const auto

EndGroup