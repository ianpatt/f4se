#pragma once

#include "f4se/NiTypes.h"
#include "f4se/NiObjects.h"

class BSParticleShaderProperty;

// 60
class BSParticleShaderEmitter : public NiRefObject
{
protected:
	BSParticleShaderProperty	* pProperty;		// 10
	UInt16						uiEmitterType;		// 18
	UInt16						uiParticleCount;	// 1A
	float						fAlpha;				// 1C
	float						fGenerateRemainder;	// 20
	float						fMaxParticleRatio;	// 24

	// 8
	struct TextureAnimInfo
	{
		float fCurrentFrame;	// 00
		UInt32 usTotalFrames;	// 04
	};

	TextureAnimInfo				* pTextureAnims;	// 28

	// 30
	struct ParticleData
	{
		float fXPos;			// 00
		float fYPos;			// 04
		float fZPos;			// 08
		float fAge;				// 0C
		float fXVel;			// 10
		float fYVel;			// 14
		float fZVel;			// 18
		float fLifeAdjust;		// 1C
		float fRotationStart;	// 20
		float fRotationSpeed;	// 24
		float fSpeedMult;		// 28
		char ucTexCoordU;		// 2C
		char ucTexCoordV;		// 2D
		char ucVertexX;			// 2E
		char ucVertexY;			// 2F
	};

	ParticleData				InstanceDataA[78];	// 30
};

// 2B0
class BSParticleShaderCubeEmitter : public BSParticleShaderEmitter
{
public:
	float		kOcclusionWorldViewProj[4][4];	// 60
	NiPoint3	kCameraOffsetVector;			// A0
	NiPoint3	kOffsetVector;					// AC
	NiPoint3	kCompositeOffsetVector;			// B8
	NiPoint3	kFrameVelocityVector;			// C4
	NiPoint3	kWindVelocity;					// D0
	NiPoint3	kGravityVelocity;				// DC
	float		fRotation;						// C0
	float		fRotationVelocity;				// C4
	float		fDensity;						// C8
	float		fEnvCubeSize;					// CC
	NiPoint3	pInstanceOffset[40];			// D0
};
