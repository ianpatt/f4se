#pragma once

#include "f4se/NiObjects.h"
#include "f4se/BSLight.h"

// 140
class NiNode : public NiAVObject
{
public:
	virtual void	Unk_39(void);
	virtual void	AttachChild(NiAVObject * obj, bool firstAvail);
	virtual void	InsertChildAt(UInt32 index, NiAVObject * obj);
	virtual void	DetachChild(NiAVObject * obj, NiPointer<NiAVObject> & out);
	virtual void	RemoveChild(NiAVObject * obj);
	virtual void	DetachChildAt(UInt32 i, NiPointer<NiAVObject> & out);
	virtual void	RemoveChildAt(UInt32 i);
	virtual void	SetAt(UInt32 index, NiAVObject * obj, NiPointer<NiAVObject> & replaced);
	virtual void	SetAt(UInt32 index, NiAVObject * obj);
	virtual void	Unk_42(void);

	NiTArray <NiAVObject *>	m_children;	// 120
	float					unk138;
	float					unk13C;

	static NiNode * Create(UInt16 children = 0);

	MEMBER_FN_PREFIX(NiNode);
	DEFINE_MEMBER_FN(ctor, NiNode*, 0x01B98920, UInt16 children);
};
STATIC_ASSERT(sizeof(NiNode) == 0x140);

// 1C0
class BSFadeNode : public NiNode
{
public:
	virtual ~BSFadeNode();

	struct FlattenedGeometryData
	{
		NiBound					kBound;		// 00
		NiPointer<BSGeometry>	spGeometry;	// 10
		UInt32					uiFlags;	// 18
	};

	BSShaderPropertyLightData		kLightData;					// 140
	tArray<FlattenedGeometryData*>	kGeomArray;					// 168
	tArray<NiBound>					MergedGeomBoundsA;			// 180
	float							fNearDistSqr;				// 198
	float							fFarDistSqr;				// 19C
	float							fCurrentFade;				// 1A0
	float							fCurrentDecalFade;			// 1A4
	float							fBoundRadius;				// 1A8
	float							fTimeSinceUpdate;			// 1AC
	SInt32							iFrameCounter;				// 1B0
	float							fPreviousMaxA;				// 1B4
	float							fCurrentShaderLODLevel;		// 1B8
	float							fPreviousShaderLODLevel;	// 1BC
};

// 190
class BSFaceGenNiNode : public NiNode
{
public:
	UInt64	unk140[(0x178 - 0x140) >> 3];	// 140
	UInt32	unk178;							// 178

	// 1FA2A8F9E63D0F771FC6A9BAB875E88A9215810B
	enum
	{
		kFlag_UpdateModel	= (1 << 0),
		kFlag_Unk1			= (1 << 7),
		kFlag_Unk2			= (1 << 8)
	};

	UInt32					flags;			// 17C
	UInt64					unk180;			// 180
	UInt64					unk188;			// 188
};
