#pragma once

#include "f4se/GameTypes.h"
#include "f4se/NiTypes.h"
#include "f4se/NiObjects.h"

class BSLight;
class NiLight;
class BSMultiBoundNode;
class BSPortal;
class BSPortalGraph;
class BSCullingProcess;
class NiCamera;
class NiTexture;
class BSLensFlareRenderData;

// 28
class BSShaderPropertyLightData
{
public:
	UInt32				uiLightListFence;	// 00
	UInt32				uiShadowAccumFlags;	// 04
	UInt32				uiLightListChanged;	// 08
	tArray<BSLight *>	lLightList;			// 10
};

// 190
class BSLight : public NiRefObject
{
public:
	float						fLODDimmer;				// 10
	float						fLuminance;				// 14
	UInt32						usFrustumCull;			// 18
	UInt32						unk1C;					// 1C - padding
	float						kGoboProjection[4][4];	// 20 - aligned to 16
	NiMatrix43					kShapeRotation;			// 60
	NiPoint3A					bPointPosition;			// 90
	union
	{
		float fCenterAndHalfExtents[6];
		float fRadius;
		float fSpotParam[2];
	};													// A0
	NiPointer<NiLight>			spLight;				// B8
	void						* unkC0;				// C0
	void						* unkC8;				// C8
	void						* lFadeNodeList;		// D0 - FadeNodeListT<NiPointer<BSFadeNode> >
	void						* kGeomListFence;		// D8
	void						* unk0E0;				// E0
	void						* unkE8;				// E8
	tArray<BSMultiBoundNode *>	kMultiboundRooms;		// F0
	tArray<BSPortal *>			kPortals;				// 108
	tArray<NiNode *>			kProcessedNodes;		// 120
	BSPortalGraph				* pPortalGraph;			// 138
	BSCullingProcess			* pCullingProcess;		// 140
	NiPointer<BSGeometry>		spGeometry;				// 148
	NiPointer<NiCamera>			spCamera;				// 150
	NiPointer<NiTexture>		spGoboTexture;			// 158
	UInt32						uiAddFadeNodeLock;		// 160
	UInt32						LensFlareLastFrame;		// 164
	UInt32						LensFlareFrameCount;	// 168
	UInt32						LensFlareIndex;			// 16C
	bool						bPointLight;			// 170
	bool						bAmbientLight;			// 171
	bool						bDynamicLight;			// 172
	bool						bPortalStrict;			// 173
	bool						bShowDebugTexture;		// 174
	bool						bAffectLand;			// 175
	bool						bAffectWater;			// 176
	bool						bLODFade;				// 177
	bool						bSpecular;				// 178
	bool						bAttenuationOnly;		// 179
	bool						bIgnoreRoughness;		// 17A
	bool						bIgnoreRim;				// 17B
	bool						bOccluded;				// 17C
	bool						bStencilVolume;			// 17D
	bool						bTemporary;				// 17E
	UInt8						unk174;					// 17F
	UInt32						kShape;					// 180 BSLight::eShape
	UInt32						unk17C;					// 184
	BSLensFlareRenderData		* pLensFlare;			// 188
};
STATIC_ASSERT(sizeof(BSLight) == 0x190);
