#include "f4se/GameWorkshop.h"
#include "f4se/GameAPI.h"

RelocAddr <PowerUtils::_UpdateMovingWirelessItem> PowerUtils::UpdateMovingWirelessItem(0x0033A570); // Usually paired with LinkPower
// this was inlined everywhere
//RelocAddr <_GetObjectAtConnectPoint> GetObjectAtConnectPoint(0x001FF360); // Acquires objects that are touching attach points
RelocAddr <TerminalUtils::_EstablishTerminalLinks> TerminalUtils::EstablishTerminalLinks(0x0033B400);
RelocAddr <SplineUtils::_ConnectSpline> SplineUtils::ConnectSpline(0x003398A0);
RelocAddr <SplineUtils::_UpdateSpline> SplineUtils::UpdateSpline(0x00339410);

// 
RelocAddr <Workshop::_ScrapReference> Workshop::ScrapReference(0x0033F7F0);

RelocPtr<BSPointerHandle<TESObjectREFR>> Workshop::hCurrentWorkshop(0x02E75154);
RelocAddr<Workshop::_FindNearestValidWorkshop> Workshop::FindNearestValidWorkshop(0x00330FE0);

namespace BSConnectPoint
{
	class Parent :
		public BSIntrusiveRefCounted
	{
	public:
		// members
		BSAutoFixedString parentName;	// 08
		BSAutoFixedString name;			// 10
		NiQuaternion rotation;			// 18
		NiPoint3 position;				// 28
		float scale;					// 34

		void Release()
		{
			if(!InterlockedDecrement(&refCount))
			{
				this->~Parent();
				Heap_Free(this);
			}
		}
	};
	STATIC_ASSERT(sizeof(Parent) == 0x38);
}

enum SnappedReferencePointStatusEnum : std::int32_t
{
	kNoReference = 0,
	kNoSnapPoint,
	kSnapPointFound,
	kNonReferenceHit,
	kCount
};

struct SnappedReferencePointStatus
{
public:
	~SnappedReferencePointStatus()
	{
		// in clib this would be easy, kludge it up here
		if(foundSnapPoint)
			foundSnapPoint->Release();
	}

	// members
	SnappedReferencePointStatusEnum status{ SnappedReferencePointStatusEnum::kCount };	// 00
	BSConnectPoint::Parent * foundSnapPoint{ nullptr };									// 08 - BSTSmartPointer
};

STATIC_ASSERT(sizeof(SnappedReferencePointStatus) == 0x10);

TESObjectREFR * GetSnappedReferenceImpl(const TESObjectREFR & a_refr, const NiPoint3 & a_connectPointWS, const bhkWorld & a_physicsWorld, SnappedReferencePointStatus & a_status, float a_radius)
{
	using func_t = decltype(&GetSnappedReferenceImpl);
	RelocAddr <func_t> func(0x00369CC0);
	return func(a_refr, a_connectPointWS, a_physicsWorld, a_status, a_radius);
}

TESObjectREFR * GetObjectAtConnectPoint(const TESObjectREFR & a_refr, NiPoint3 & a_connectPointWS, const bhkWorld & a_physicsWorld, float a_radius)
{
	SnappedReferencePointStatus status;
	return GetSnappedReferenceImpl(a_refr, a_connectPointWS, a_physicsWorld, status, a_radius);
}
