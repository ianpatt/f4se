#include "f4se/GameReferences.h"
#include "f4se/GameExtraData.h"
#include "f4se/GameRTTI.h"

// 
RelocPtr <PlayerCharacter*> g_player(0x032BC250);

RelocAddr <_HasDetectionLOS> HasDetectionLOS(0x010DB0B0);

RelocAddr <_GetLinkedRef_Native> GetLinkedRef_Native(0x00561620);

RelocAddr <_SetLinkedRef_Native> SetLinkedRef_Native(0x00561640);

RelocAddr <_MoveRefrToPosition> MoveRefrToPosition(0x0117BAC0);

bool Actor::GetEquippedExtraData(UInt32 slotIndex, ExtraDataList ** extraData)
{
	// Invalid slot id
	if (slotIndex >= BIPOBJECT::BIPED_OBJECT::kTotal)
		return false;

	// This should be possible but check anyway
	if (!biped)
		return false;

	// Make sure there is an item in this slot
	auto item = biped->object[slotIndex].parent.object;
	if (!item)
		return false;

	if (!inventoryList)
		return false;

	// Find the equipped form from the inventory
	for (UInt32 i = 0; i < inventoryList->items.count; i++)
	{
		BGSInventoryItem inventoryItem;
		inventoryList->items.GetNthItem(i, inventoryItem);
		if (inventoryItem.form != item || !inventoryItem.stack)
			continue;

		// Search stacks for the equipped stack
		bool ret = inventoryItem.stack->Visit([&](BGSInventoryItem::Stack * stack)
		{
			if (stack->flags & BGSInventoryItem::Stack::kFlagEquipped)
			{
				ExtraDataList * stackDataList = stack->extraData;
				if (stackDataList) {
					(*extraData) = stackDataList;
				}

				return false;
			}

			return true;
		});
		if (!ret) // Stack found
			break;
	}

	return (*extraData) != nullptr;
}
