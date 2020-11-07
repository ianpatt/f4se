#include "f4se/GameObjects.h"
#include "f4se/GameReferences.h"
#include "f4se/GameUtilities.h"

RelocAddr <_PlaceAtMe_Native> PlaceAtMe_Native(0x0140B0E0);

bool TESObjectARMA::GetNodeName(char * dstBuff, TESNPC * npc, TESObjectARMO * armor)
{
	if(npc) {
		UInt32 sex = npc ? CALL_MEMBER_FN(npc, GetSex)() : 0;
		sprintf_s(dstBuff, MAX_PATH, " (%08X)[%d]/ (%08X)", formID, sex, armor->formID);
		return true;
	}

	return false;
}

void TESNPC::ChangeHeadPart(BGSHeadPart * headPart, bool bRemovePart, bool bRemoveExtraParts)
{
	if(bRemovePart)
		CALL_MEMBER_FN(this, ChangeHeadPartRemovePart)(headPart, bRemoveExtraParts);
	else
		CALL_MEMBER_FN(this, ChangeHeadPart)(headPart);
}

BGSHeadPart * TESNPC::GetHeadPartByType(UInt32 type, bool bOverlays)
{
	BGSHeadPart ** parts = headParts;
	UInt32 numParts = numHeadParts;
	if(bOverlays) {
		parts = CALL_MEMBER_FN(this, GetOverlayHeadParts)();
		numParts = CALL_MEMBER_FN(this, GetNumOverlayHeadParts)();
	}

	for(UInt32 i = 0; i < numParts; ++i)
	{
		BGSHeadPart * part = parts[i];
		if(part && part->type == type)
		{
			return part;
		}
	}

	return nullptr;
}

BGSColorForm * TESNPC::GetHairColor()
{
	if(!headData || headData->hairColor == nullptr)
	{
		TESRace * raceForm = race.race;
		if(raceForm) {
			CharacterCreation::CharGenData * charGenData = raceForm->chargenData[ CALL_MEMBER_FN(this, GetSex)()];
			if(charGenData) {
				return charGenData->defaultColor;
			}
		}
	}

	return headData->hairColor;
}
