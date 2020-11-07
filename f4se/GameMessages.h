#pragma once

#include "f4se_common/Utilities.h"

class ScrapHeap;
class BSGeometry;
class BGSTextureSet;

struct BSPackedTask
{

};

template<typename T>
class BSTMessageQueue
{
public:
	virtual void Unk_01() = 0;
	virtual void Unk_02() = 0;
	virtual void Unk_03() = 0;
	virtual void Unk_04() = 0;
};

template<typename T>
class BSTCommonMessageQueue : public BSTMessageQueue<T>
{
public:
	virtual void Unk_01();
	virtual void Unk_02();
	virtual void Unk_03();
	virtual void Unk_04();
	virtual void Unk_05();
	virtual void Unk_06();

	volatile UInt32	m_refCount;	// 08
	UInt32			pad0C;		// 0C
};

template<typename T>
class BSTCommonScrapHeapMessageQueue : public BSTCommonMessageQueue<T>
{
public:
	virtual void Unk_05();
	virtual void Unk_06();

	ScrapHeap	* m_pScrapHeap;

	MEMBER_FN_PREFIX(BSTCommonScrapHeapMessageQueue);
	DEFINE_MEMBER_FN(SetTextureSet, void, 0x00D5DF60, BSGeometry * geometry, BGSTextureSet * textureSet);
};

extern RelocPtr <BSTCommonScrapHeapMessageQueue<BSPackedTask>*> g_messageQueue;
