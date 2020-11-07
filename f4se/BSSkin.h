#pragma once

#include <xmmintrin.h>
#include "NiObjects.h"

class BSSkin
{
public:
	// 28
	class BoneData : public NiObject
	{
	public:
		struct BoneTransforms
		{
			NiPoint3	boundingSphere;	// 00
			float		radius;			// 0C
			NiTransform	transform;		// 10
		};

		tArray<BoneTransforms>	transforms;		// 10
		void					* unk20;	// 20
	};

	// C0
	class Instance : public NiObject
	{
	public:
		tArray<NiNode*>			bones;	// 10
		tArray<NiTransform*>	worldTransforms;	// 28 - These are pointers to the corresponding bone's worldTransforms
		BoneData		* boneData;	// 40
		NiAVObject		* rootNode;	// 48
		void			* unk50;	// 50
		UInt64			unk58;	// 58
		__m128			unk60;	// 60
		__m128			unk70;	// 64
		__m128			unk80;	// 68
		__m128			unk90;	// 90
		void			* unkA0;	// A0
		void			* unkA8;	// A8
		UInt64			unkB0;	// B0
		UInt32			unkB8;	// B8
		SInt32			unkBC;	// BC
	};
};
