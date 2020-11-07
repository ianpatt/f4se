Scriptname WorkshopDataScript Hidden Const
{holds struct definitions to allow the compiler to work}

struct WorkshopRatingKeyword
	ActorValue resourceValue
	{ the actor value for this rating }

	bool clearOnReset
	{
		true = clear this value when the workshop resets
		false = persistent value (e.g. hope, radio, damage)
	}

	int maxProductionPerNPC = 0
	{ if non-zero, indicates a type of resource where a single NPC can work on multiple resource object (e.g. food)}

endStruct

struct FarmDiscountVendor
	ActorBase VendorBaseActor
	{ vendor's base actor }

	Location VendorLocation
	{ vendor's location }
endStruct

struct WorkshopActorValue
	int workshopRatingIndex
	{ index to WorkshopRatings array that matches this actor value}

	ActorValue resourceValue
	{ the actor value }
endStruct

struct WorkshopVendorType
	Faction VendorFaction
	{ what faction to assign to an NPC who is assigned to work on this object }

	int minPopulationForTopVendor
	{ min population at settlement to qualify for top vendor }

	GlobalVariable topVendorFlag
	{ global tracks count of valid top-level vendor objects }

	Keyword keywordToAdd01
	{ keyword to add to the vendor NPC }
endStruct

struct WorkshopFoodType
	ActorValue resourceValue
	{ what resource value matches this food type }

	LeveledItem foodObject
	{ leveled item to use to create this food type }
endStruct
