#include "f4se/PapyrusObjectReference.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"
#include "f4se/PapyrusUtilities.h"

#include "f4se/GameReferences.h"
#include "f4se/GameObjects.h"
#include "f4se/GameExtraData.h"
#include "f4se/GameRTTI.h"
#include "f4se/GameData.h"
#include "f4se/GameWorkshop.h"

#include "f4se/BSGeometry.h"
#include "f4se/NiExtraData.h"
#include "f4se/NiObjects.h"
#include "f4se/NiNodes.h"
#include "f4se/NiMaterials.h"
#include "f4se/bhkWorld.h"

#include "f4se/Serialization.h"

#include "f4se/PapyrusDelayFunctors.h"

#include <set>
#include <regex>
#include <unordered_map>

DECLARE_STRUCT(ConnectPoint, "ObjectReference");
DECLARE_EXTERN_STRUCT(RemapData);

namespace papyrusObjectReference {

	VMArray<BGSMod::Attachment::Mod*> GetAllMods(VMRefOrInventoryObj * thisObj)
	{
		TESForm * baseForm = nullptr;
		ExtraDataList * extraDataList = nullptr;
		thisObj->GetExtraData(&baseForm, &extraDataList);

		VMArray<BGSMod::Attachment::Mod*> result;
		if(extraDataList)
		{
			BSExtraData * extraData = extraDataList->GetByType(ExtraDataType::kExtraData_ObjectInstance);
			if(extraData)
			{
				BGSObjectInstanceExtra * objectModData = DYNAMIC_CAST(extraData, BSExtraData, BGSObjectInstanceExtra);
				if(objectModData)
				{
					auto data = objectModData->data;
					if(!data || !data->forms)
						return result;

					for(UInt32 i = 0; i < data->blockSize / sizeof(BGSObjectInstanceExtra::Data::Form); i++)
					{
						BGSMod::Attachment::Mod * objectMod = (BGSMod::Attachment::Mod *)Runtime_DynamicCast(LookupFormByID(data->forms[i].formId), RTTI_TESForm, RTTI_BGSMod__Attachment__Mod);
						result.Push(&objectMod);
					}
				}
			}
		}
		return result;
	}

	TESObjectREFR* AttachWireLatent(UInt32 stackId, TESObjectREFR* refA, TESObjectREFR* refB, TESForm* splineForm)
	{
		TESObjectREFR * wireRef = nullptr;
		VirtualMachine* vm = (*g_gameVM)->m_virtualMachine;

		if(!splineForm) {
			BGSDefaultObject * splineDefault = (*g_defaultObjectMap)->GetDefaultObject("WorkshopSplineObject");
			if(splineDefault) {
				splineForm = splineDefault->form;
			}
		}

		// No specified spline, no refs, refs are same item, or no 3D loaded
		if(!splineForm || !refA || !refB || refA == refB || !refA->GetObjectRootNode() || !refB->GetObjectRootNode()) {
			return nullptr;
		}

		// See if the two references are already linked by the same wire i.e. they have the same entry in their PowerLinks listing
		std::set<UInt64> linkedWires;
		ExtraDataList * extraDataRefA = refA->extraDataList;
		ExtraDataList * extraDataRefB = refB->extraDataList;
		if(extraDataRefA && extraDataRefB)
		{
			ExtraPowerLinks * powerLinksA = (ExtraPowerLinks*)extraDataRefA->GetByType(kExtraData_PowerLinks);
			ExtraPowerLinks * powerLinksB = (ExtraPowerLinks*)extraDataRefB->GetByType(kExtraData_PowerLinks);
			if(powerLinksA && powerLinksB) // Both items must have power links to check
			{
				tArray<UInt64> * connectionSearch;
				tArray<UInt64> * connectionPopulate;
				if(powerLinksA->connections.count < powerLinksB->connections.count) // Pick the smaller list to be the set
				{
					connectionPopulate = &powerLinksA->connections;
					connectionSearch = &powerLinksB->connections;
				}
				else
				{
					connectionPopulate = &powerLinksB->connections;
					connectionSearch = &powerLinksA->connections;
				}

				// Add the items from the smaller list to the set
				for(int i = 0; i < connectionPopulate->count; i++)
				{
					UInt64 formID = 0;
					connectionPopulate->GetNthItem(i, formID);
					linkedWires.insert(formID);
				}

				// Search the other listing for items that exist in the set
				for(int i = 0; i < connectionSearch->count; i++)
				{
					UInt64 formID = 0;
					connectionSearch->GetNthItem(i, formID);

					// This wire exists in the other list, it is invalid to wire the same objects twice
					if(linkedWires.find(formID) != linkedWires.end()) {
						return nullptr;
					}
				}
			}
		}

		BGSBendableSpline * spline = DYNAMIC_CAST(splineForm, TESForm, BGSBendableSpline);
		BGSBendableSpline * splineA = DYNAMIC_CAST(refA->baseForm, TESForm, BGSBendableSpline);
		BGSBendableSpline * splineB = DYNAMIC_CAST(refB->baseForm, TESForm, BGSBendableSpline);

		BGSKeyword * keyword = nullptr;
		BGSDefaultObject * workshopItemDefault = (*g_defaultObjectMap)->GetDefaultObject("WorkshopItem");
		if(workshopItemDefault) {
			keyword = DYNAMIC_CAST(workshopItemDefault->form, TESForm, BGSKeyword);
		}

		// No workshop keyword is bad
		// Connecting a wire to another wire or connecting a non-wire is invalid
		if(!keyword || !spline || splineA || splineB) {
			return nullptr;
		}

		// Get the workshop by keyword
		TESObjectREFR * workshopRef = GetLinkedRef_Native(refA, keyword);
		if(!workshopRef) {
			return nullptr;
		}

		// Workshop ref isn't a workshop!
		Workshop::ExtraData* extraDataWorkshop = static_cast<Workshop::ExtraData*>(workshopRef->extraDataList->GetByType(ExtraDataType::kExtraData_WorkshopExtraData));
		if(!extraDataWorkshop) {
			return nullptr;
		}

		// Create our wire instance
		wireRef = PlaceAtMe_Native(vm, stackId, &refA, spline, 1, true, true, false);
		if(!wireRef) {
			return nullptr;
		}

		UInt32 nullHandle = *g_invalidRefHandle;
		TESObjectCELL* parentCell = wireRef->parentCell;
		TESWorldSpace* worldspace = CALL_MEMBER_FN(wireRef,GetWorldspace)();

		NiPoint3 rot;
		MoveRefrToPosition(wireRef, &nullHandle, parentCell, worldspace, &refA->pos, &rot);

		// Set the wire's linked ref to the workshop
		SetLinkedRef_Native(wireRef, workshopRef, keyword);

		BSPointerHandle<TESObjectREFR> currentWorkshop = *Workshop::hCurrentWorkshop;

		*Workshop::hCurrentWorkshop = workshopRef;

		Workshop::ContextData contextData(*g_player);
		SplineUtils::UpdateSpline(&contextData, wireRef, refB, 0, refA, 0);
		SplineUtils::ConnectSpline(refA, 0, refB, 0, wireRef);

		ExtraBendableSplineParams * splineParams = (ExtraBendableSplineParams*)wireRef->extraDataList->GetByType(kExtraData_BendableSplineParams);
		if(splineParams) {
			splineParams->thickness = 1.5f;
		}

		extraDataWorkshop->AddItem(wireRef);
		extraDataWorkshop->AddConnection(refA, refB, wireRef);
		PowerUtils::UpdateMovingWirelessItem(refA, extraDataWorkshop);
		PowerUtils::UpdateMovingWirelessItem(refB, extraDataWorkshop);
		TerminalUtils::EstablishTerminalLinks(wireRef);

		*Workshop::hCurrentWorkshop = currentWorkshop;
		return wireRef;
	}

	DECLARE_DELAY_FUNCTOR(F4SEAttachWireFunctor, 2, AttachWireLatent, TESObjectREFR, TESObjectREFR*, TESObjectREFR*, TESForm*);


	bool AttachWire(VirtualMachine * vm, UInt32 stackId, TESObjectREFR* refA, TESObjectREFR* refB, TESForm* baseSpline)
	{
		if(!refA || !refB)
			return false;

		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEAttachWireFunctor(AttachWireLatent, vm, stackId, refA, refB, baseSpline));
		return true;
	}

	// If this object is a BendableSpline it will link to the two ObjectReferences, otherwise it will link to other BendableSplines
	VMArray<TESObjectREFR*> GetConnectedObjects(TESObjectREFR* thisRef)
	{
		VMArray<TESObjectREFR*> result;
		if(!thisRef)
			return result;

		ExtraDataList * extraDataList = thisRef->extraDataList;
		if(extraDataList)
		{
			if(extraDataList->HasType(kExtraData_PowerLinks))
			{
				ExtraPowerLinks * powerLinks = (ExtraPowerLinks*)extraDataList->GetByType(kExtraData_PowerLinks);
				if(powerLinks)
				{
					for(int i = 0; i < powerLinks->connections.count; i++)
					{
						UInt64 formID = 0;
						powerLinks->connections.GetNthItem(i, formID);

						TESForm * form = LookupFormByID(formID);
						if(form)
						{
							TESObjectREFR * refr = DYNAMIC_CAST(form, TESForm, TESObjectREFR);
							if(refr)
								result.Push(&refr);
						}
					}
				}
			}
		}

		return result;
	}

	BSFixedString GetDisplayName(VMRefOrInventoryObj * thisObj)
	{
		TESForm * baseForm = nullptr;
		ExtraDataList * extraDataList = nullptr;
		thisObj->GetExtraData(&baseForm, &extraDataList);

		if(baseForm)
		{
			if(extraDataList)
			{
				BSExtraData * extraData = extraDataList->GetByType(ExtraDataType::kExtraData_TextDisplayData);
				if(extraData)
				{
					ExtraTextDisplayData * displayText = DYNAMIC_CAST(extraData, BSExtraData, ExtraTextDisplayData);
					if(displayText)
					{
						return *CALL_MEMBER_FN(displayText, GetReferenceName)(baseForm);
					}
				}
			}

			TESFullName* pFullName = DYNAMIC_CAST(baseForm, TESForm, TESFullName);
			if (pFullName)
				return pFullName->name;
		}

		return BSFixedString();
	}


	void SetMaterialSwap(VMRefOrInventoryObj * thisObj, BGSMaterialSwap * newSwap)
	{
		if(thisObj)
		{
			TESForm * baseForm = nullptr;
			ExtraDataList * extraDataList = nullptr;
			thisObj->GetExtraData(&baseForm, &extraDataList);

			if(baseForm)
			{
				if(extraDataList)
				{
					BSExtraData * extraData = extraDataList->GetByType(ExtraDataType::kExtraData_MaterialSwap);
					if(extraData)
					{
						if(newSwap)
						{
							ExtraMaterialSwap * materialSwap = DYNAMIC_CAST(extraData, BSExtraData, ExtraMaterialSwap);
							if(materialSwap)
							{
								materialSwap->materialSwap = newSwap;
							}
						}
						else
						{
							extraDataList->Remove(ExtraDataType::kExtraData_MaterialSwap, extraData);
							Heap_Free(extraData);
						}
					}
					else if(newSwap)
					{
						extraDataList->Add(ExtraDataType::kExtraData_MaterialSwap, ExtraMaterialSwap::Create(newSwap));
					}
				}
			}
		}
	}

	BGSMaterialSwap * GetMaterialSwap(VMRefOrInventoryObj * thisObj)
	{
		if(!thisObj)
			return nullptr;

		TESForm * baseForm = nullptr;
		ExtraDataList * extraDataList = nullptr;
		thisObj->GetExtraData(&baseForm, &extraDataList);

		if(baseForm)
		{
			if(extraDataList)
			{
				BSExtraData * extraData = extraDataList->GetByType(ExtraDataType::kExtraData_MaterialSwap);
				if(extraData)
				{
					ExtraMaterialSwap * materialSwap = DYNAMIC_CAST(extraData, BSExtraData, ExtraMaterialSwap);
					if(materialSwap)
					{
						return materialSwap->materialSwap;
					}
				}
			}
		}

		return nullptr;
	}


	VMArray<TESForm*> GetInventoryItemsLatent(UInt32 stackId, TESObjectREFR * refr)
	{
		VMArray<TESForm*> results;
		if(!refr)
			return results;

		auto inventory = refr->inventoryList;
		if(inventory) {
			inventory->inventoryLock.LockForRead();

			for(int i = 0; i < inventory->items.count; i++) {
				BGSInventoryItem item;
				inventory->items.GetNthItem(i, item);
				
				results.Push(&item.form);
			}

			inventory->inventoryLock.UnlockRead();
		}

		return results;
	}

	float GetInventoryWeight(TESObjectREFR * refr)
	{
		return refr ? CALL_MEMBER_FN(refr, GetInventoryWeight)() : 0.0f;
	}

	DECLARE_DELAY_FUNCTOR(F4SEInventoryFunctor, 0, GetInventoryItemsLatent, TESObjectREFR, VMArray<TESForm*>);

	bool GetInventoryItems(VirtualMachine * vm, UInt32 stackId, TESObjectREFR* refr)
	{
		if(!refr)
			return false;

		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEInventoryFunctor(GetInventoryItemsLatent, vm, stackId, refr));
		return true;
	}

	VMArray<ConnectPoint> GetConnectPointsLatent(UInt32 stackId, TESObjectREFR * refr)
	{
		VMArray<ConnectPoint> results;
		if(!refr)
			return results;

		NiNode * root = refr->GetObjectRootNode();
		if(!root)
			return results;

		BSConnectPoint::Parents * extraData = (BSConnectPoint::Parents *)Runtime_DynamicCast(root->GetExtraData("CPA"), RTTI_NiExtraData, RTTI_BSConnectPoint__Parents);
		if(extraData)
		{
			for(UInt32 i = 0; i < extraData->points.count; i++)
			{
				BSConnectPoint::Parents::ConnectPoint * connectPoint = extraData->points[i];
				if(connectPoint)
				{
					ConnectPoint point;

					point.Set<BSFixedString>("parent", connectPoint->parent);
					point.Set<BSFixedString>("name", connectPoint->name);

					float yaw, pitch, roll;
					connectPoint->rot.GetEulerAngles(roll, pitch, yaw);
					yaw *= 180.0 / MATH_PI;
					pitch *= 180.0 / MATH_PI;
					roll *= 180.0 / MATH_PI;

					point.Set<float>("roll", roll);
					point.Set<float>("pitch", pitch);
					point.Set<float>("yaw", yaw);

					NiPoint3 localPos = connectPoint->pos;

					NiAVObject * parent = nullptr;
					if(connectPoint->parent == BSFixedString(""))
					{
						parent = root;
					}
					else
					{
						NiAVObject * child = root->GetObjectByName(&connectPoint->parent);
						if(child)
							parent = child;
					}

					NiPoint3 worldPos = localPos;
					if(parent) {
						worldPos = parent->m_worldTransform.rot.Transpose() * localPos + parent->m_worldTransform.pos;
						point.Set<float>("x", worldPos.x);
						point.Set<float>("y", worldPos.y);
						point.Set<float>("z", worldPos.z);
					} else {
						point.Set<float>("x", 0.0f);
						point.Set<float>("y", 0.0f);
						point.Set<float>("z", 0.0f);
					}

					point.Set<float>("scale", connectPoint->scale);

					point.Set<TESObjectREFR*>("object", nullptr);

					if(parent != root && refr->parentCell) {
						bhkWorld * world = CALL_MEMBER_FN(refr->parentCell, GetHavokWorld)();
						if(world) {
							TESObjectREFR * connected = GetObjectAtConnectPoint(refr, &worldPos, world, 8.0f);
							if(connected) {
								point.Set<TESObjectREFR*>("object", connected);
							}
						}
					}

					results.Push(&point);
				}
			}
		}		
		
		return results;
	}

	DECLARE_DELAY_FUNCTOR(F4SEConnectPointsFunctor, 0, GetConnectPointsLatent, TESObjectREFR, VMArray<ConnectPoint>);

	bool GetConnectPoints(VirtualMachine * vm, UInt32 stackId, TESObjectREFR* refr)
	{
		if(!refr)
			return false;

		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEConnectPointsFunctor(GetConnectPointsLatent, vm, stackId, refr));
		return true;
	}

	bool TransmitConnectedPowerLatent(UInt32 stackId, TESObjectREFR * refr)
	{
		if(!refr)
			return false;

		NiNode * root = refr->GetObjectRootNode();
		if(!root) {
			return false;
		}

		BGSKeyword * keyword = nullptr;
		BGSDefaultObject * workshopItemDefault = (*g_defaultObjectMap)->GetDefaultObject("WorkshopItem");
		if(workshopItemDefault) {
			keyword = DYNAMIC_CAST(workshopItemDefault->form, TESForm, BGSKeyword);
		}
		
		// No workshop keyword is bad
		if(!keyword) {
			return false;
		}

		// Get the workshop by keyword
		TESObjectREFR * workshopRef = GetLinkedRef_Native(refr, keyword);
		if(!workshopRef) {
			return false;
		}

		// Workshop ref isn't a workshop!
		Workshop::ExtraData* extraDataWorkshop = static_cast<Workshop::ExtraData*>(workshopRef->extraDataList->GetByType(ExtraDataType::kExtraData_WorkshopExtraData));
		if(!extraDataWorkshop) {
			return false;
		}

		BSConnectPoint::Parents * extraData = (BSConnectPoint::Parents *)Runtime_DynamicCast(root->GetExtraData("CPA"), RTTI_NiExtraData, RTTI_BSConnectPoint__Parents);
		if(!extraData) {
			return false;
		}

		BSPointerHandle<TESObjectREFR> previousWorkshop = *Workshop::hCurrentWorkshop;

		*Workshop::hCurrentWorkshop = workshopRef;
		
		for(UInt32 i = 0; i < extraData->points.count; i++)
		{
			BSConnectPoint::Parents::ConnectPoint * connectPoint = extraData->points[i];
			if(!connectPoint)
				continue;

			NiPoint3 localPos = connectPoint->pos;
			NiAVObject * parent = nullptr;
			if(connectPoint->parent == "")
				parent = root;
			else
			{
				NiAVObject * child = root->GetObjectByName(&connectPoint->parent);
				if(child)
					parent = child;
			}

			NiPoint3 worldPos = localPos;
			if(parent) {
				worldPos = parent->m_worldTransform.rot.Transpose() * localPos + parent->m_worldTransform.pos;
			}

			float scale = connectPoint->scale;
			if(parent != root && refr->parentCell) {
				bhkWorld * world = CALL_MEMBER_FN(refr->parentCell, GetHavokWorld)();
				if(world) {
					TESObjectREFR * connected = GetObjectAtConnectPoint(refr, &worldPos, world, 8.0f);
					if(connected) {
						try // Probably wont make a difference but doesnt hurt to try
						{
							extraDataWorkshop->AddConnection(refr, connected, nullptr);
							PowerUtils::UpdateMovingWirelessItem(connected, extraDataWorkshop);
						}
						catch (...)
						{
							_MESSAGE("Power link error!");
						}
					}
				}
			}
		}

		PowerUtils::UpdateMovingWirelessItem(refr, extraDataWorkshop);

		*Workshop::hCurrentWorkshop = previousWorkshop;
		return true;
	}

	DECLARE_DELAY_FUNCTOR(F4SETransmitConnectedPowerFunctor, 0, TransmitConnectedPowerLatent, TESObjectREFR, bool);

	bool TransmitConnectedPower(VirtualMachine * vm, UInt32 stackId, TESObjectREFR* refr)
	{
		if(!refr)
			return false;

		F4SEDelayFunctorManagerInstance().Enqueue(new F4SETransmitConnectedPowerFunctor(TransmitConnectedPowerLatent, vm, stackId, refr));
		return true;
	}

	struct CompareMaterial {
		bool operator()(const std::pair<BGSMaterialSwap::MaterialSwap*,float> lhs, const std::pair<BGSMaterialSwap::MaterialSwap*,float> rhs) const {
			return lhs.first->source < rhs.first->source;
		}
	};

	VMArray<RemapData> ApplyMaterialSwapLatent(UInt32 stackId, VMRefOrInventoryObj * thisObj, BGSMaterialSwap * materialSwap, bool renameMaterial)
	{
		// If the item is a reference use that, otherwise use the owner
		TESObjectREFR * refr = thisObj->GetObjectReference();
		if(!refr)
			refr = thisObj->GetOwner();

		NiNode * rootNode[2];
		rootNode[0] = refr ? refr->GetActorRootNode(false) : nullptr;
		rootNode[1] = refr ? refr->GetActorRootNode(true) : nullptr;

		int maxRoots = 2;
		if(rootNode[0] == rootNode[1])
			maxRoots = 1;

		std::set<std::pair<BGSMaterialSwap::MaterialSwap*,float>, CompareMaterial> success;

		for(int i = 0; i < maxRoots; ++i)
		{
			if(!rootNode[i])
				continue;

			rootNode[i]->Visit([&](NiAVObject * object)
			{
				BSGeometry * geometry = object->GetAsBSGeometry();
				if(geometry) 
				{
					NiPointer<BSShaderProperty> shaderProperty = ni_cast(geometry->shaderProperty, BSShaderProperty);
					if(shaderProperty.get())
					{
						const char * shaderPath = shaderProperty->m_name.c_str();
						std::string fullPath(shaderPath ? shaderPath : "");
						if(fullPath.length() == 0)
							return false;

						fullPath = std::regex_replace(fullPath, std::regex("/+|\\\\+"), "\\"); // Replace multiple slashes or forward slashes with one backslash
						fullPath = std::regex_replace(fullPath, std::regex("^\\\\+"), ""); // Remove all backslashes from the front
						fullPath = std::regex_replace(fullPath, std::regex(".*?materials\\\\", std::regex_constants::icase), ""); // Remove everything before and including the materials path
						BSFixedString fixedPath(fullPath.c_str());

						auto materialLookup = materialSwap->materialSwaps.Find(&fixedPath);
						if(materialLookup)
						{
							// Pull the old colorRemapIndex
							float lastColorIndex = FLT_MAX;
							BSLightingShaderProperty * lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
							if(lightingShader) {
								BSLightingShaderMaterialBase * shaderMaterialBase = static_cast<BSLightingShaderMaterialBase *>(shaderProperty->shaderMaterial);
								lastColorIndex = shaderMaterialBase->fLookupScale;
							}

							std::string materialPath("Materials\\");
							materialPath.append(materialLookup->target.c_str());
							BSShaderData shaderData;
							if(!LoadMaterialFile(materialPath.c_str(), &shaderData, 0))
								CALL_MEMBER_FN(&shaderData, ApplyMaterialData)(geometry, false);

							// Shader property will be the same pointer unless it changes shader type
							BSShaderProperty * newShaderProperty = ni_cast(geometry->shaderProperty, BSShaderProperty);
							if(newShaderProperty)
							{
								// Renames the material after swap
								if(renameMaterial) {
									newShaderProperty->m_name = materialPath.c_str();
								}

								// Fill in the new colorRemapIndex
								if(materialLookup->colorRemapIndex != FLT_MAX)
								{
									BSLightingShaderProperty * newLightingShader = ni_cast(newShaderProperty, BSLightingShaderProperty);
									if(newLightingShader) {
										BSLightingShaderMaterialBase * shaderMaterialBase = static_cast<BSLightingShaderMaterialBase *>(newLightingShader->shaderMaterial);
										shaderMaterialBase->fLookupScale = materialLookup->colorRemapIndex;
									}
								}
							}

							success.emplace(std::make_pair(materialLookup, lastColorIndex));
						}
					}
					
				}
				return false;
			});
		}

		VMArray<RemapData> result;
		for(auto & material : success)
		{
			RemapData res;
			res.Set("source", material.first->target);
			res.Set("target", material.first->source);
			res.Set("colorIndex", material.second);
			result.Push(&res);
		}
		return result;
	}

	DECLARE_DELAY_FUNCTOR(F4SEMaterialSwapFunctor, 2, ApplyMaterialSwapLatent, VMRefOrInventoryObj, VMArray<RemapData>, BGSMaterialSwap*, bool);

	bool ApplyMaterialSwap(VirtualMachine * vm, UInt32 stackId, VMRefOrInventoryObj* refr, BGSMaterialSwap * materialSwap, bool renameMaterial)
	{
		if(!refr || !materialSwap)
			return false;

		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEMaterialSwapFunctor(ApplyMaterialSwapLatent, vm, stackId, refr, materialSwap, renameMaterial));
		return true;
	}

	bool ScrapLatent(UInt32 stackId, TESObjectREFR * refr, TESObjectREFR* akWorkshopRef)
	{
		Workshop::ContextData contextData(*g_player);

		BSPointerHandle<TESObjectREFR> currentWorkshop = *Workshop::hCurrentWorkshop;

		// Didn't specify a workshop
		if (!akWorkshopRef)
		{
			// We are not in workshop mode, find a workshop
			if (!currentWorkshop)
				akWorkshopRef = Workshop::FindNearestValidWorkshop(refr);
			else // Use the workshop from workshop mode
				akWorkshopRef = currentWorkshop.get();
		}
		
		if (!akWorkshopRef)
			return false;

		*Workshop::hCurrentWorkshop = akWorkshopRef;
		NiPointer<TESObjectREFR> refrPointer(refr);
		Workshop::ScrapReference(&contextData, &refrPointer, nullptr);
		*Workshop::hCurrentWorkshop = currentWorkshop;
		return true;
	}

	DECLARE_DELAY_FUNCTOR(F4SEScrapFunctor, 1, ScrapLatent, TESObjectREFR, bool, TESObjectREFR*);

	bool Scrap(VirtualMachine * vm, UInt32 stackId, TESObjectREFR* refr, TESObjectREFR* akWorkshopRef)
	{
		if(!refr)
			return false;

		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEScrapFunctor(ScrapLatent, vm, stackId, refr, akWorkshopRef));
		return true;
	}
}

void papyrusObjectReference::RegisterFuncs(VirtualMachine* vm)
{
	F4SEObjectRegistry& f4seObjRegistry = F4SEObjectRegistryInstance();
	f4seObjRegistry.RegisterClass<F4SEAttachWireFunctor>();
	f4seObjRegistry.RegisterClass<F4SEInventoryFunctor>();
	f4seObjRegistry.RegisterClass<F4SEConnectPointsFunctor>();
	f4seObjRegistry.RegisterClass<F4SETransmitConnectedPowerFunctor>();
	f4seObjRegistry.RegisterClass<F4SEMaterialSwapFunctor>();
	f4seObjRegistry.RegisterClass<F4SEScrapFunctor>();

	vm->RegisterFunction(
		new NativeFunction0<VMRefOrInventoryObj, VMArray<BGSMod::Attachment::Mod*>>("GetAllMods", "ObjectReference", papyrusObjectReference::GetAllMods, vm));

	vm->RegisterFunction(
		new NativeFunction0<TESObjectREFR, VMArray<TESObjectREFR*>>("GetConnectedObjects", "ObjectReference", papyrusObjectReference::GetConnectedObjects, vm));

	vm->RegisterFunction(
		new LatentNativeFunction2<TESObjectREFR, TESObjectREFR*, TESObjectREFR*, TESForm*>("AttachWire", "ObjectReference", papyrusObjectReference::AttachWire, vm));

	vm->RegisterFunction(
		new NativeFunction0<VMRefOrInventoryObj, BSFixedString>("GetDisplayName", "ObjectReference", papyrusObjectReference::GetDisplayName, vm));

	vm->RegisterFunction(
		new LatentNativeFunction0<TESObjectREFR, VMArray<TESForm*>>("GetInventoryItems", "ObjectReference", papyrusObjectReference::GetInventoryItems, vm));

	vm->RegisterFunction(
		new NativeFunction0<TESObjectREFR, float>("GetInventoryWeight", "ObjectReference", papyrusObjectReference::GetInventoryWeight, vm));

	vm->RegisterFunction(
		new NativeFunction1<VMRefOrInventoryObj, void, BGSMaterialSwap*>("SetMaterialSwap", "ObjectReference", papyrusObjectReference::SetMaterialSwap, vm));

	vm->RegisterFunction(
		new NativeFunction0<VMRefOrInventoryObj, BGSMaterialSwap*>("GetMaterialSwap", "ObjectReference", papyrusObjectReference::GetMaterialSwap, vm));

	vm->RegisterFunction(
		new LatentNativeFunction0<TESObjectREFR, VMArray<ConnectPoint>>("GetConnectPoints", "ObjectReference", papyrusObjectReference::GetConnectPoints, vm));

	vm->RegisterFunction(
		new LatentNativeFunction0<TESObjectREFR, bool>("TransmitConnectedPower", "ObjectReference", papyrusObjectReference::TransmitConnectedPower, vm));

	vm->RegisterFunction(
		new LatentNativeFunction2<VMRefOrInventoryObj, VMArray<RemapData>, BGSMaterialSwap*, bool>("ApplyMaterialSwap", "ObjectReference", papyrusObjectReference::ApplyMaterialSwap, vm));

	vm->SetFunctionFlags("ObjectReference", "AttachWire", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("ObjectReference", "GetInventoryItems", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("ObjectReference", "GetConnectPoints", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("ObjectReference", "TransmitConnectedPower", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("ObjectReference", "ApplyMaterialSwap", IFunction::kFunctionFlag_NoWait);

	vm->RegisterFunction(
		new LatentNativeFunction1<TESObjectREFR, bool, TESObjectREFR*>("Scrap", "ObjectReference", papyrusObjectReference::Scrap, vm));

	vm->SetFunctionFlags("ObjectReference", "Scrap", IFunction::kFunctionFlag_NoWait);
}
