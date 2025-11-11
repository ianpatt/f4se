#pragma once

#include "f4se_common/Utilities.h"
#include "f4se/GameAPI.h"
#include "f4se/GameUtilities.h"

class TESForm;

// 04 or 08 depending alignment
struct BSIntrusiveRefCounted
{
public:
	mutable UInt32	refCount;	// 00
};

// Not actually implemented, just a wrapper
template <typename T>
class BSTSmartPointer
{
public:
	operator bool() const noexcept { return static_cast<bool>(_ptr); }

	T* get() const noexcept { return _ptr; }

	T& operator*() const
	{
		return *_ptr;
	}

	T* operator->() const
	{
		return _ptr;
	}

	T* _ptr;
};

// 04
struct BSNonReentrantSpinLock
{
public:
	volatile UInt32	uiLock;		// 00
};

class SimpleLock
{
	enum
	{
		kFastSpinThreshold = 10000
	};

	volatile SInt32	threadID;	// 00
	volatile SInt32	lockCount;	// 04

public:
	SimpleLock() : threadID(0), lockCount(0) {}

	void Lock(UInt32 pauseAttempts = 0);
	void Release(void);
};
STATIC_ASSERT(sizeof(SimpleLock) == 0x8);

// 08
class BSReadWriteLock
{
	enum
	{
		kFastSpinThreshold = 10000,
		kLockWrite = 0x80000000,
		kLockCountMask = 0xFFFFFFF
	};

	volatile SInt32	threadID;	// 00
	volatile SInt32	lockValue;	// 04

public:
	BSReadWriteLock() : threadID(0), lockValue(0) {}

	DEFINE_MEMBER_FN_0(LockForRead, void, 0x01653A70);
	DEFINE_MEMBER_FN_0(LockForWrite, void, 0x01653AF0);

	DEFINE_MEMBER_FN_0(UnlockRead, void, 0x01653D40);
	DEFINE_MEMBER_FN_0(UnlockWrite, void, 0x01653D50);
};
STATIC_ASSERT(sizeof(BSReadWriteLock) == 0x8);

class BSReadLocker
{
public:
	BSReadLocker(BSReadWriteLock * lock) { m_lock = lock; m_lock->LockForRead(); }
	~BSReadLocker() { m_lock->UnlockRead(); }

protected:
	BSReadWriteLock	* m_lock;
};

class BSWriteLocker
{
public:
	BSWriteLocker(BSReadWriteLock * lock) { m_lock = lock; m_lock->LockForWrite(); }
	~BSWriteLocker() { m_lock->UnlockWrite(); }

protected:
	BSReadWriteLock	* m_lock;
};

class SimpleLocker
{
public:
	SimpleLocker(SimpleLock * dataHolder) { m_lock = dataHolder; m_lock->Lock(); }
	~SimpleLocker() { m_lock->Release(); }

protected:
	SimpleLock	* m_lock;
};

// 80808
class StringCache
{
public:
	// 18+
	struct Entry
	{
		enum
		{
			kState_RefcountMask =	0x3FFF,
			kState_External =		0x4000,
			kState_Wide =			0x8000
		};

		Entry	* next;				// 00
		UInt32	state;				// 08 - refcount, hash, flags
		UInt32	length;				// 0C
		Entry	* externData;		// 10
		char	data[0];			// 18

		bool IsWide()
		{
			Entry * iter = this;

			while(iter->state & kState_External)
				iter = iter->externData;

			if((iter->state & kState_Wide) == kState_Wide)
				return true;

			return false;
		}

		template<typename T>
		T * Get()
		{
			Entry * iter = this;

			while(iter->state & kState_External)
				iter = iter->externData;

			return (T*)iter->data;
		}
	};

	struct Ref
	{
		Entry	* data;

		MEMBER_FN_PREFIX(Ref);
		// 
		DEFINE_MEMBER_FN(ctor, Ref *, 0x01676850, const char * buf);
		// 
		DEFINE_MEMBER_FN(ctor_w, Ref *, 0x01677720, const wchar_t * buf);
		// 
		DEFINE_MEMBER_FN(Set, Ref *, 0x01676990, const char * buf);
		DEFINE_MEMBER_FN(Set_w, Ref *, 0x01678910, const wchar_t * buf);

		DEFINE_MEMBER_FN(Release, void, 0x01677B10);

		Ref();
		Ref(const char * buf);
		Ref(const wchar_t * buf);
		
		void Release();

		bool operator==(const char * lhs) const;
		bool operator==(const Ref& lhs) const { return data == lhs.data; }
		bool operator<(const Ref& lhs) const { return data < lhs.data; }

		const char * c_str() const { return operator const char *(); }
		operator const char *() const { return data ? data->Get<char>() : nullptr; }

		const wchar_t * wc_str() { return operator const wchar_t *(); }
		operator const wchar_t *() { return data ? data->Get<wchar_t>() : nullptr; }
	};

	// 10
	struct Lock
	{
		UInt32	unk00;	// 00 - set to 80000000 when locked
		UInt32	pad04;	// 04
		UInt64	pad08;	// 08
	};

	Entry	* lut[0x10000];	// 00000
	Lock	lock[0x80];		// 80000
	UInt8	isInit;			// 80800
};

typedef StringCache::Ref BSFixedString;
typedef StringCache::Ref BSFixedStringW;

class BSAutoFixedString : public BSFixedString
{
public:
	BSAutoFixedString() : BSFixedString() { }
	BSAutoFixedString(const char * buf) : BSFixedString(buf) { }

	~BSAutoFixedString()
	{
		Release();
	}
};

// 10
class BSString
{
public:
	BSString() :m_data(NULL), m_dataLen(0), m_bufLen(0) { }
	~BSString()
	{
		if(m_data) {
			Heap_Free(m_data);
			m_data = nullptr;
			m_dataLen = 0;
			m_bufLen = 0;
		}
	};

	const char *	Get(void);

private:
	char	* m_data;	// 00
	UInt16	m_dataLen;	// 08
	UInt16	m_bufLen;	// 0A
	UInt32	pad0C;		// 0C
};


// Container types

// 18
template <class T, int nGrow = 10, int nShrink = 10>
class tArray
{
public:
	T* entries;			// 00
	UInt32 capacity;	// 08
	UInt32 pad0C;		// 0C
	UInt32 count;		// 10
	UInt32 pad14;		// 14

	tArray() : entries(NULL), capacity(0), count(0), pad0C(0), pad14(0) { }

	T& operator[](UInt64 index)
	{
		return entries[index];
	}

	void Clear()
	{
		Heap_Free(entries);
		entries = NULL;
		capacity = 0;
		count = 0;
	}

	bool Allocate(UInt32 numEntries)
	{
		entries = (T *)Heap_Allocate(sizeof(T) * numEntries);
		if(!entries) return false;

		for(UInt32 i = 0; i < numEntries; i++)
			new (&entries[i]) T;

		capacity = numEntries;
		count = numEntries;

		return true;
	}

	bool Resize(UInt32 numEntries)
	{
		if(numEntries == capacity)
			return false;

		if(!entries) {
			Allocate(numEntries);
			return true;
		}
		if(numEntries < capacity) {
			// Delete the truncated entries
			for(UInt32 i = numEntries; i < capacity; i++)
				delete &entries[i];
		}
		
		T * newBlock = (T *)Heap_Allocate(sizeof(T) * numEntries);						// Create a new block
		memmove_s(newBlock, sizeof(T) * numEntries, entries, sizeof(T) * numEntries);	// Move the old memory to the new block
		if(numEntries > capacity) {														// Fill in new remaining entries
			for(UInt32 i = capacity; i < numEntries; i++)
				new (&entries[i]) T;
		}
		Heap_Free(entries);																// Free the old block
		entries = newBlock;																// Assign the new block
		capacity = numEntries;															// Capacity is now the number of total entries in the block
		count = (std::min)(capacity, count);													// Count stays the same, or is truncated to capacity
		return true;
	}

	bool Push(const T & entry)
	{
		if(!entries || count + 1 > capacity) {
			if(!Grow(nGrow))
				return false;
		}
 
		entries[count] = entry;
		count++;
		return true;
	};
 
	bool Insert(UInt32 index, const T & entry)
	{
		if(!entries)
				return false;

		UInt32 lastSize = count;
		if(count + 1 > capacity) // Not enough space, grow
		{
			if(!Grow(nGrow))
				return false;
		}
 
		if(index != lastSize)  // Not inserting onto the end, need to move everything down
		{
			UInt32 remaining = count - index;
			memmove_s(&entries[index + 1], sizeof(T) * remaining, &entries[index], sizeof(T) * remaining); // Move the rest up
		}
		
		entries[index] = entry;
		count++;
		return true;
	};

	bool Remove(UInt32 index)
	{
		if(!entries || index >= count)
			return false;
 
		// This might not be right for pointer types...
		(&entries[index])->~T();

		if(index + 1 < count) {
			UInt32 remaining = count - index;
			memmove_s(&entries[index], sizeof(T) * remaining, &entries[index + 1], sizeof(T) * remaining); // Move the rest up
		}
		count--;
 
		if(capacity > count + nShrink)
			Shrink();
 
		return true;
	}

	bool Shrink()
	{
		if(!entries || count == capacity) return false;
 
		try {
			UInt32 newSize = count;
			T * oldArray = entries;
			T * newArray = (T *)Heap_Allocate(sizeof(T) * newSize); // Allocate new block
			memmove_s(newArray, sizeof(T) * newSize, entries, sizeof(T) * newSize); // Move the old block
			entries = newArray;
			capacity = count;
			Heap_Free(oldArray); // Free the old block
			return true;
		}
		catch(...) {
			return false;
		}
 
		return false;
	}

	bool Grow(UInt32 numEntries)
	{
		if(!entries) {
			entries = (T *)Heap_Allocate(sizeof(T) * numEntries);
			count = 0;
			capacity = numEntries;
			return true;
		}
 
		try {
			UInt32 oldSize = capacity;
			UInt32 newSize = oldSize + numEntries;
			T * oldArray = entries;
			T * newArray = (T *)Heap_Allocate(sizeof(T) * newSize); // Allocate new block
			if(oldArray)
				memmove_s(newArray, sizeof(T) * newSize, entries, sizeof(T) * capacity); // Move the old block
			entries = newArray;
			capacity = newSize;

			if(oldArray)
				Heap_Free(oldArray); // Free the old block
 
			for(UInt32 i = oldSize; i < newSize; i++) // Allocate the rest of the free blocks
				new (&entries[i]) T;

			return true;
		}
		catch(...) {
			return false;
		}
 
		return false;
	}

	bool GetNthItem(UInt64 index, T& pT) const
	{
		if (index < count) {
			pT = entries[index];
			return true;
		}
		return false;
	}

	SInt64 GetItemIndex(T & pFind) const
	{
		for (UInt64 n = 0; n < count; n++) {
			T& pT = entries[n];
			if (pT == pFind)
				return n;
		}
		return -1;
	}

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)
};

// Because aliasing via using doesn't work in this version of C++
template <class T, int nGrow = 10, int nShrink = 10>
class BSTArray : public tArray<T, nGrow, nShrink>
{
public:
};

template<class T>
class tMutexArray : public tArray<T>
{
public:
	SimpleLock lock;	// 18
};

typedef tArray<UInt64> UnkArray;
typedef tArray<TESForm*> UnkFormArray;

// 10
template <class T>
class tList
{
	enum {
		eListCount = -3,
		eListEnd = -2,
		eListInvalid = -1,		
	};
	struct _Node
	{
		
		T *		item;	// 00
		_Node*	next;	// 04

		T *		Item() const { return item; }
		_Node*	Next() const { return next; }

		T *		Remove(_Node * pred)
		{
			T * pRemoved = item;
			_Node * pNext = next;

			// become the next entry and return my item
			if (pNext)
			{
				item = pNext->item;
				next = pNext->next;
				FormHeap_Free(pNext);
			}
			// tail?
			else
			{
				item = NULL;
				next = NULL;

				// has precedessor, so tail != head
				if (pred)
				{
					pred->next = NULL;
					FormHeap_Free(this);
				}
			}
			return pRemoved;
		}
	};

	_Node m_listHead;	// 00

private:

	_Node * Head() const { return const_cast<_Node*>(&m_listHead); }

	_Node * Tail() const
	{
		_Node * node = const_cast<_Node*>(&m_listHead);

		if (node)
			while (node->next) node = node->next;
		
		return node;
	}

	template <class Op>
	UInt32 FreeNodes(Op &compareOp)
	{
		UInt32 numFreed = 0;

		_Node * pPred = NULL;
		_Node * pCur = Head();

		while (pCur)
		{
			if (pCur->next)
			{
				if (compareOp.Accept(pCur->item))
				{
					pCur->Remove(pPred);
					numFreed++;
				}
				else
				{
					pPred = pCur;
					pCur = pCur->next;
				}
			}
			// List Tail?
			else
			{
				if (compareOp.Accept(pCur->item))
				{
					pCur->Remove(pPred);
					numFreed++;
				}
				break;
			}
		}

		return numFreed;
	}

	struct NodePos
	{
		NodePos(): node(NULL), index(eListInvalid) {}

		_Node*	node;
		SInt32	index;
	};


	NodePos GetNthNode(SInt32 index) const
	{
		NodePos pos;
		SInt32 n = 0;
		_Node* pCur = Head();

		while (pCur && pCur->Item())
		{
			if (n == index) break;
			if (eListEnd == index && !pCur->Next()) break;
			pCur = pCur->Next();
			++n;
		}

		pos.node = pCur;
		pos.index = n;

		return pos;
	}

public:

	// Allocate list as a single empty node
	static tList<T> * Create(void)
	{
		tList<T> * p = (tList<T> *)FormHeap_Allocate(sizeof(tList<T>));
		ASSERT(p);

		p->m_listHead.item = NULL;
		p->m_listHead.next = NULL;

		return p;
	}

	void Delete(void)
	{
		RemoveAll();
		FormHeap_Free(&m_listHead);
	}

	class Iterator
	{
		_Node *	m_cur;
	public:
		Iterator() : m_cur(NULL) {}
		Iterator(_Node* node) : m_cur(node) { }
		Iterator operator++()	{ if (!End()) m_cur = m_cur->Next(); return *this;}
		bool End()	{	return m_cur == NULL;	}
		const T * operator->() { return (m_cur) ? m_cur->Item() : NULL; }
		const T * operator*() { return (m_cur) ? m_cur->Item() : NULL; }
		const Iterator& operator=(const Iterator& rhs) {
			m_cur = rhs.m_cur;
			return *this;
		}
		T * Get() { return (m_cur) ? m_cur->Item() : NULL; }
	};
	
	const Iterator Begin() const { return Iterator(Head()); }

	void Insert(T * item)
	{
		// add new node if we aren't empty
		if (m_listHead.item)
		{
			// copy head in to new node
			_Node	* node = (_Node *)FormHeap_Allocate(sizeof(_Node));
			ASSERT(node);

			node->item = m_listHead.item;
			node->next = m_listHead.next;

			m_listHead.next = node;
		}

		m_listHead.item = item;
	}

	void Push(T * item)
	{
		_Node * tail = Tail();
		
		// add new node if we aren't empty
		if (tail->item)
		{
			_Node * node = (_Node *)FormHeap_Allocate(sizeof(_Node));
			ASSERT(node);

			tail->next = node;

			node->item = item;
			node->next = NULL;
		}
		else
		{
			tail->item = item;
		}
	}

	T * AddFront(void)
	{
		T	* item = (T *)FormHeap_Allocate(sizeof(T));
		if(!item)
			return NULL;

		new (item) T;

		Insert(item);
		return item;
	}

	T * AddBack(void)
	{
		T	* item = (T *)FormHeap_Allocate(sizeof(T));
		if(!item)
			return NULL;

		new (item) T;

		Push(item);
		return item;
	}

	void Append(Iterator source)
	{
		while (!source.End())
		{
			Push(source.Get());
			++source;
		}
	}

	UInt32 Count() const
	{
		NodePos pos = GetNthNode(eListCount);
		return (pos.index > 0) ? pos.index : 0;
	};

	T * GetNthItem(SInt32 n) const
	{
		NodePos pos = GetNthNode(n);
		return (pos.index == n && pos.node) ? pos.node->Item() : NULL;
	}

	T * GetLastItem() const
	{
		NodePos pos = GetNthNode(eListEnd);
		return pos.node->Item();
	}

	SInt32 AddAt(T * item, SInt32 index)
	{
		if (!m_listHead.item) {
			m_listHead.item = item;
			return 0;
		}

		NodePos pos = GetNthNode(index);
		_Node* pTargetNode = pos.node;
		_Node* newNode = (_Node*)FormHeap_Allocate(sizeof(_Node));
		if (newNode && pTargetNode) {
			if (index == eListEnd) {
				pTargetNode->next = newNode;
				newNode->item = item;
				newNode->next = NULL;
			} else {
				newNode->item = pTargetNode->item;
				newNode->next = pTargetNode->next;
				pTargetNode->item = item;
				pTargetNode->next = newNode;
			}
			return pos.index;
		}
		return eListInvalid;
	}

	template <class Op>
	void Visit(Op& op, _Node* prev = NULL) const {
		const _Node* pCur = (prev) ? prev->next : Head();
		bool bContinue = true;
		while (pCur && bContinue) {
			bContinue = op.Accept(pCur->Item());
			if (bContinue) {
				pCur = pCur->next;
			}
		}
	}

	template <class Op>
	T * Find(Op& op) const
	{
		const _Node* pCur = Head(); 

		bool bFound = false;
		while (pCur && !bFound)
		{
			if (!pCur->Item())
				pCur = pCur->Next();
			else
			{
				bFound = op.Accept(pCur->Item());
				if (!bFound)
					pCur = pCur->Next();
			}
		}
		return (bFound && pCur) ? pCur->Item() : NULL;
	}

	template <class Op>
	Iterator Find(Op& op, Iterator prev) const
	{
		Iterator curIt = (prev.End()) ? Begin() : ++prev;
		bool bFound = false;
		
		while(!curIt.End() && !bFound) {
			const T * pCur = *curIt;
			if (pCur) {
				bFound = op.Accept(pCur);
			}
			if (!bFound) {
				++curIt;
			}	
		}
		return curIt;
	}

	template <class Op>
	UInt32 CountIf(Op& op) const
	{
		UInt32 count = 0;
		const _Node* pCur = Head();
		while (pCur)
		{
			if (pCur->Item() && op.Accept(pCur->Item()))
				count++;
			pCur = pCur->Next();
		}
		return count;
	}

	class AcceptAll {
	public:
		bool Accept(T * item) {
			return true;
		}
	};

	void RemoveAll()
	{
		FreeNodes(AcceptAll());
	}

	T * RemoveNth(SInt32 n) 
	{
		T* pRemoved = NULL;
		if (n == 0) {
			pRemoved =  m_listHead.RemoveMe();
		} else if (n > 0) {
			NodePos nodePos = GetNthNode(n);
			if (nodePos.node && nodePos.index == n) {
				pRemoved = nodePos.node->RemoveMe();
			}
		}
		return pRemoved;
	};

	T * ReplaceNth(SInt32 n, T* item) 
	{
		T* pReplaced = NULL;
		NodePos nodePos = GetNthNode(n);
		if (nodePos.node && nodePos.index == n) {
			pReplaced = nodePos.node->item;
			nodePos.node->item = item;
		}
		return pReplaced;
	}

	template <class Op>
	UInt32 RemoveIf(Op& op)
	{
		return FreeNodes(op);
	}

	template <class Op>
	SInt32 GetIndexOf(Op& op)
	{
		SInt32 idx = 0;
		const _Node* pCur = Head();
		while (pCur && pCur->Item() && !op.Accept(pCur->Item()))
		{
			idx++;
			pCur = pCur->Next();
		}

		if (pCur && pCur->Item())
			return idx;
		else
			return -1;
	}

	class AcceptEqual {
	public:
		T * item;

		AcceptEqual(T * a_item) : item(a_item) {}

		bool Accept(T * a_item) {
			return *item == *a_item;
		}
	};

	bool Contains(T * item) const
	{
		return Find(AcceptEqual(item)) != NULL;
	}
	
	void	Dump(void)
	{
		_MESSAGE("tList:");
		_MESSAGE("> count: %d", Count());

		const _Node* pCur = Head();
		UInt32 i = 0;
		while (pCur)
		{
			_MESSAGE("* %d :", i);
			//_MESSAGE("\t\titem: %08X", pCur->item);
			if (pCur->item)
				_MESSAGE("\t\t*item: %d", *pCur->item);
			_MESSAGE("\t\tnext: %08X", pCur->next);

			i++;
			pCur = pCur->next;
		}
	}
};

STATIC_ASSERT(sizeof(tList<void *>) == 0x10);

// 30
template <typename Item, typename Key = Item>
class tHashSet
{
	class _Entry
	{
	public:
		Item	item;
		_Entry	* next;

		_Entry() : next(NULL) {}

		bool	IsFree() const	{ return next == NULL; }
		void	SetFree()		{ next = NULL; }

		void	Dump(void)
		{
			item.Dump();
			_MESSAGE("\t\tnext: %016I64X", next);
		}
	};

	// When creating a new tHashSet, init sentinel pointer with address of this entry
	static _Entry sentinel;

	void		* unk00;		// 000
	UInt32		unk_000;		// 008
	UInt32		m_size;			// 00C
	UInt32		m_freeCount;	// 010
	UInt32		m_freeOffset;	// 014
	_Entry 		* m_eolPtr;		// 018
	UInt64		unk_018;		// 020
	_Entry		* m_entries;	// 028


	_Entry * GetEntry(UInt32 hash) const
	{
		return (_Entry*) (((uintptr_t) m_entries) + sizeof(_Entry) * (hash & (m_size - 1)));
	}

	_Entry * GetEntryAt(UInt32 index) const
	{
		return (_Entry*) (((uintptr_t) m_entries) + sizeof(_Entry) * index);
	}

	_Entry * NextFreeEntry(void)
	{
		_Entry * result = NULL;

		if (m_freeCount == 0)
			return NULL;

		do
		{
			m_freeOffset = (m_size - 1) & (m_freeOffset - 1);
			_Entry * entry = GetEntryAt(m_freeOffset);

			if (entry->IsFree())
				result = entry;
		}
		while (!result);

		m_freeCount--;

		return result;
	}

	enum InsertResult
	{
		kInsert_Duplicate = -1,
		kInsert_OutOfSpace = 0,
		kInsert_Success = 1
	};

	InsertResult Insert(Item * item)
	{
		if (! m_entries)
			return kInsert_OutOfSpace;

		Key k = (Key)*item;
		_Entry * targetEntry = GetEntry(Item::GetHash(&k));

		// Case 1: Target entry is free
		if (targetEntry->IsFree())
		{
			targetEntry->item = *item;
			targetEntry->next = m_eolPtr;
			--m_freeCount;

			return kInsert_Success;
		}

		// -- Target entry is already in use

		// Case 2: Item already included
		_Entry * p = targetEntry;
		do
		{
			if (p->item == *item)
				return kInsert_Duplicate;
			p = p->next;
		}
		while (p != m_eolPtr);

		// -- Either hash collision or bucket overlap

		_Entry * freeEntry = NextFreeEntry();
		// No more space?
		if (!freeEntry)
			return kInsert_OutOfSpace;

		// Original position of the entry that currently uses the target position
		k = (Key)targetEntry->item;
		p = GetEntry(Item::GetHash(&k));

		// Case 3a: Hash collision - insert new entry between target entry and successor
        if (targetEntry == p)
        {
			freeEntry->item = *item;
			freeEntry->next = targetEntry->next;
			targetEntry->next = freeEntry;

			return kInsert_Success;
        }
		// Case 3b: Bucket overlap
		else
		{
			while (p->next != targetEntry)
				p = p->next;

			freeEntry->item = targetEntry->item;
			freeEntry->next = targetEntry->next;

			p->next = freeEntry;
			targetEntry->item = *item;
			targetEntry->next = m_eolPtr;

			return kInsert_Success;
		}
	}

	bool CopyEntry(_Entry * sourceEntry)
	{
		if (! m_entries)
			return false;

		Key k = (Key)sourceEntry->item;
		_Entry * targetEntry = GetEntry(Item::GetHash(&k));

		// Case 1: Target location is unused
		if (!targetEntry->next)
		{
			targetEntry->item = sourceEntry->item;
			targetEntry->next = m_eolPtr;
			--m_freeCount;

			return true;
		}

		// Target location is in use. Either hash collision or bucket overlap.

		_Entry * freeEntry = NextFreeEntry();
		k = (Key)targetEntry->item;
		_Entry * p = GetEntry(Item::GetHash(&k));

		// Case 2a: Hash collision - insert new entry between target entry and successor
		if (targetEntry == p)
		{
			freeEntry->item = sourceEntry->item;
			freeEntry->next = targetEntry->next;
			targetEntry->next = freeEntry;

			return true;
		}

		// Case 2b: Bucket overlap - forward until hash collision is found, then insert
		while (p->next != targetEntry)
			p = p->next;

		// Source entry takes position of target entry - not completely understood yet
		freeEntry->item = targetEntry->item;
		freeEntry->next = targetEntry->next;
		p->next = freeEntry;
		targetEntry->item = sourceEntry->item;
		targetEntry->next = m_eolPtr;

		return true;
	}

	void Grow(void)
	{
		UInt32 oldSize = m_size;
		UInt32 newSize = oldSize ? 2*oldSize : 8;

		_Entry * oldEntries = m_entries;
		_Entry * newEntries = (_Entry*)Heap_Allocate(newSize * sizeof(_Entry));
		ASSERT(newEntries);
		
		m_entries = newEntries;
		m_size = m_freeCount = m_freeOffset = newSize;

		// Initialize new table data (clear next pointers)
		_Entry * p = newEntries;
		for (UInt32 i = 0; i < newSize; i++, p++)
			p->SetFree();

		// Copy old entries, free old table data
		if (oldEntries)
		{
			_Entry * p = oldEntries;
			for (UInt32 i = 0; i < oldSize; i++, p++)
				if (p->next)
					CopyEntry(p);
			Heap_Free(oldEntries);
		}
	}

public:

	tHashSet() : m_size(0), m_freeCount(0), m_freeOffset(0), m_entries(NULL), m_eolPtr(&sentinel) { }
	~tHashSet() { if(m_entries) Heap_Free(m_entries); }

	UInt32	Size() const		{ return m_size; }
	UInt32	FreeCount() const	{ return m_freeCount; }
	UInt32	FillCount() const	{ return m_size - m_freeCount; }

	Item * Find(Key * key) const
	{
		if (!m_entries)
			return NULL;

		_Entry * entry = GetEntry(Item::GetHash(key));
		if (! entry->next)
			return NULL;

		while (!(entry->item == *key))
		{
			entry = entry->next;
			if (entry == m_eolPtr)
				return NULL;
		}

		return &entry->item;
	}

	bool Add(Item * item)
	{
		InsertResult result;

		for (result = Insert(item); result == kInsert_OutOfSpace; result = Insert(item))
			Grow();

		return result == kInsert_Success;
	}

	bool Remove(Key * key)
	{
		if ( !m_entries)
			return false;

		_Entry * entry = GetEntry(Item::GetHash(key));
		if (! entry->next)
			return NULL;

		_Entry * prevEntry = NULL;
		while (! (entry->item == *key))
		{
			prevEntry = entry;
			entry = entry->next;
			if (entry == m_eolPtr)
				return false;
		}

		// Remove tail?
		_Entry * nextEntry = entry->next;
		if (nextEntry == m_eolPtr)
		{
			if (prevEntry)
				prevEntry->next = m_eolPtr;
			entry->next = NULL;
		}
		else
		{
			entry->item = nextEntry->item;
			entry->next = nextEntry->next;
			nextEntry->next = NULL;
		}

		++m_freeCount;
		return true;
	}

	void Clear(void)
	{
		if (m_entries)
		{
			_Entry * p = m_entries;
			for (UInt32 i = 0; i < m_size; i++, p++)
				p->next = NULL;
		}
		else
		{
			m_size = 0;
		}
		m_freeCount = m_freeOffset = m_size;
	}

	template <typename T>
	void ForEach(const T& functor)
	{
		if (!m_entries)
			return;

		if(m_size == m_freeCount) // The whole thing is free
			return;

		_Entry * cur	= m_entries;
		_Entry * end	= GetEntryAt(m_size); // one index beyond the entries data to check if we reached that point

		if (cur == end)
			return;

		if (cur->IsFree())
		{
			// Forward to first non-free entry
			do cur++;
			while (cur != end && cur->IsFree());
		}

		do
		{
			if (! functor(&cur->item))
				return;

			// Forward to next non-free entry
			do cur++;
			while (cur != end && cur->IsFree());

		} while (cur != end);
	}

	void Dump(void)
	{
		_MESSAGE("tHashSet:");
		_MESSAGE("> size: %d", Size());
		_MESSAGE("> free: %d", FreeCount());
		_MESSAGE("> filled: %d", FillCount());
		if (m_entries)
		{
			_Entry * p = m_entries;
			for (UInt32 i = 0; i < m_size; i++, p++) {
				_MESSAGE("* %d %s:", i, p->IsFree()?"(free)" : "");
				if (!p->IsFree())
					p->Dump();
			}
		}
	}

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)
};

template <typename Key, typename Item>
typename tHashSet<Key,Item>::_Entry tHashSet<Key,Item>::sentinel = tHashSet<Key,Item>::_Entry();

STATIC_ASSERT(sizeof(tHashSet<UInt32, void*>) == 0x30);

template <typename T>
class SafeDataHolder
{
protected:
	SimpleLock	m_lock;
public:
	T			m_data;

	void	Lock(void) { m_lock.Lock(); }
	void	Release(void) { m_lock.Release(); }
};

#if 0
// Because we cant use 'using' in this version
template <typename Item, typename Key = Item>
class BSTHashMap : public tHashSet<Item, Key>
{
public:
};
#endif

template <class T1, class T2>
struct BSTTuple
{
public:
	BSTTuple() :
		first(),
		second()
	{}

	BSTTuple(const T1& a_first, const T2& a_second) :
		first(a_first),
		second(a_second)
	{}

	BSTTuple& operator=(const BSTTuple& a_rhs)
	{
		first = a_rhs.first;
		second = a_rhs.second;
	}


	BSTTuple& operator=(BSTTuple&& a_rhs)
	{
		first = std::move(a_rhs.first);
		second = std::move(a_rhs.second);
	}

	T1 first;	// 00
	T2 second;	// ??
};

template <class Traits, UInt32 N, template <class, UInt32> class Allocator, class Hash, class KeyEqual>
struct BSTScatterTable
{
public:
	typedef Traits traits_type;
	typedef typename traits_type::key_type key_type;
	typedef typename traits_type::mapped_type mapped_type;
	typedef typename traits_type::value_type value_type;
	typedef UInt32 size_type;
	typedef Hash hasher;
	typedef KeyEqual key_equal;

	struct BSTScatterTableEntry
	{
	public:
		BSTScatterTableEntry() :
			value(),
			next(0)
		{}


		BSTScatterTableEntry(const BSTScatterTableEntry& a_rhs) :
			value(a_rhs.value),
			next(a_rhs.next)
		{}


		BSTScatterTableEntry(BSTScatterTableEntry&& a_rhs) :
			value(std::move(a_rhs.value)),
			next(std::move(a_rhs.next))
		{
			a_rhs.next = 0;
		}


		BSTScatterTableEntry& operator=(const BSTScatterTableEntry& a_rhs)
		{
			value = a_rhs.value;
			next = a_rhs.next;
			return *this;
		}


		BSTScatterTableEntry& operator=(BSTScatterTableEntry&& a_rhs)
		{
			value = std::move(a_rhs.value);
			next = std::move(a_rhs.next);
			a_rhs.next = 0;

			return *this;
		}


		value_type			  value;  // 00
		BSTScatterTableEntry* next;	  // ??
	};


	typedef BSTScatterTableEntry entry_type;
	typedef Allocator<entry_type, N> allocator_type;

	static entry_type sentinel;

	template <class U>
	struct iterator_base
	{
	public:
		typedef std::ptrdiff_t difference_type;
		typedef U value_type;
		typedef U* pointer;
		typedef U& reference;
		typedef std::forward_iterator_tag iterator_category;


		iterator_base() :
			_entry(nullptr),
			_end(nullptr)
		{}


		iterator_base(const iterator_base& a_rhs) :
			_entry(a_rhs._entry),
			_end(a_rhs._end)
		{}


		iterator_base(iterator_base&& a_rhs) :
			_entry(std::move(a_rhs._entry)),
			_end(a_rhs._end)
		{
			a_rhs._entry = a_rhs._end;
		}


		iterator_base(entry_type* a_entry, entry_type* a_end) :
			_entry(a_entry),
			_end(a_end)
		{
			while (_entry != _end && !_entry->next) {
				++_entry;
			}
		}


		~iterator_base()
		{}


		iterator_base& operator=(const iterator_base& a_rhs)
		{
			ASSERT(_end == a_rhs._end);
			_entry = a_rhs._entry;
		}


		iterator_base& operator=(iterator_base&& a_rhs)
		{
			ASSERT(_end == a_rhs._end);
			_entry = std::move(a_rhs._entry);
			a_rhs._entry = a_rhs._end;
		}


		void swap(iterator_base& a_rhs)
		{
			ASSERT(_end == a_rhs._end);
			std::swap(_entry, a_rhs._entry);
		}


		reference operator*() const
		{
			ASSERT(_entry != _end);
			return _entry->value;
		}


		pointer operator->() const
		{
			return std::addressof(_entry->value);
		}


		bool operator==(const iterator_base& a_rhs) const
		{
			ASSERT(_end == a_rhs._end);
			return _entry == a_rhs._entry;
		}


		bool operator!=(const iterator_base& a_rhs) const
		{
			ASSERT(_end == a_rhs._end);
			return !operator==(a_rhs);
		}


		// prefix
		iterator_base& operator++()
		{
			ASSERT(_entry != _end);
			do {
				++_entry;
			} while (_entry != _end && !_entry->next);
			return *this;
		}


		// postfix
		iterator_base operator++(int)
		{
			iterator_base tmp{ *this };
			operator++();
			return tmp;
		}

	private:
		entry_type* _entry;
		entry_type* _end;
	};


	typedef iterator_base<value_type> iterator;
	typedef iterator_base<const value_type> const_iterator;


	BSTScatterTable() :
		_pad00(0),
		_pad08(0),
		_capacity(0),
		_freeCount(0),
		_freeIdx(0),
		_sentinel((const entry_type*)&sentinel),
		_allocator()
	{}

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)

	iterator begin()
	{
		return get_entries() ? make_iterator(get_entries()) : iterator();
	}


	const_iterator begin() const
	{
		return get_entries() ? make_iterator(get_entries()) : const_iterator();
	}


	const_iterator cbegin() const
	{
		return begin();
	}


	iterator end()
	{
		return get_entries() ? make_iterator(get_entries() + _capacity) : iterator();
	}


	const_iterator end() const
	{
		return get_entries() ? make_iterator(get_entries() + _capacity) : const_iterator();
	}


	const_iterator cend() const
	{
		return end();
	}


	bool empty() const
	{
		return !get_entries() || _freeCount == 0;
	}


	size_type size() const
	{
		return _capacity - _freeCount;
	}


	size_type max_size() const
	{
		return _allocator.max_size();
	}


	std::pair<iterator, bool> insert(const value_type& a_value)
	{
		return insert_impl(false, a_value);
	}


	std::pair<iterator, bool> insert(value_type&& a_value)
	{
		return insert_impl(false, std::move(a_value));
	}


	std::pair<iterator, bool> insert_or_assign(const value_type& a_value)
	{
		return insert_impl(true, a_value);
	}


	std::pair<iterator, bool> insert_or_assign(value_type&& a_value)
	{
		return insert_impl(true, std::move(a_value));
	}


	size_type erase(const key_type& a_key)
	{
		if (!get_entries()) {  // no entries
			return 0;
		}

		auto entry = calc_pos(a_key);
		if (!entry->next) {	 // key not in table
			return 0;
		}

		entry_type* tail = 0;
		while (!comp_key(get_key(entry->value), a_key)) {  // find key in table
			tail = entry;
			entry = entry->next;
			if (entry == _sentinel) {
				return 0;
			}
		}

		entry->value.~value_type();

		if (entry->next == _sentinel) {	 // if no chain
			if (tail) {
				tail->next = const_cast<entry_type*>(_sentinel);
			}
			entry->next = 0;
		}
		else {  // else move next entry into current
			new (entry) entry_type(std::move(*entry->next));
		}

		++_freeCount;
		return 1;
	}


	iterator find(const key_type& a_key)
	{
		auto entry = find_impl(a_key);
		return entry ? make_iterator(entry) : end();
	}


	const_iterator find(const key_type& a_key) const
	{
		auto entry = find_impl(a_key);
		return entry ? make_iterator(entry) : end();
	}


	void reserve(size_type a_count)
	{
		if (a_count <= _capacity) {
			return;
		}

		UInt32		   leftShifts = 0;
		while ((a_count & static_cast<UInt32>(1 << 31)) == 0) {
			a_count <<= 1;
			++leftShifts;
		}
		auto bitPos = 31 - leftShifts;
		auto newCount = static_cast<UInt32>(1 << bitPos);
		grow(newCount);
	}


	hasher hash_function() const
	{
		return hasher();
	}


	key_equal key_eq() const
	{
		return key_equal();
	}

private:
	entry_type* find_impl(const key_type& a_key) const
	{
		if (!get_entries()) {
			return nullptr;
		}

		auto probe = calc_pos(a_key);  // try ideal pos
		if (!probe->next) {
			return nullptr;	 // nothing there
		}

		do {
			if (comp_key(get_key(probe->value), a_key)) {
				return probe;
			}
			else {
				probe = probe->next;
			}
		} while (probe != _sentinel);  // follow chain

		return nullptr;
	}


	template <class Arg>
	std::pair<iterator, bool> insert_impl(bool a_overwrite, Arg&& a_value)
	{
		if (!get_entries() || !_freeCount) {
			if (!grow()) {
				return std::make_pair(end(), false);  // maybe throw?
			}
		}

		auto idealEntry = calc_pos(get_key(a_value));
		if (!idealEntry->next) {  // if slot empty
			new (std::addressof(idealEntry->value)) value_type(std::forward<Arg>(a_value));
			idealEntry->next = const_cast<entry_type*>(_sentinel);
			return std::make_pair(make_iterator(idealEntry), true);
		}

		for (auto iter = idealEntry; iter != _sentinel; iter = iter->next) {
			if (comp_key(get_key(iter->value), get_key(a_value))) {	 // if entry already in table
				if (a_overwrite) {
					iter->value.~value_type();
					new (std::addressof(iter->value)) value_type(std::forward<Arg>(a_value));
				}
				return std::make_pair(make_iterator(iter), false);
			}
		}

		auto freeEntry = get_free_entry();

		auto takenIdealEntry = calc_pos(get_key(idealEntry->value));
		if (takenIdealEntry == idealEntry) {  // if entry occupying our slot would've hashed here anyway
			freeEntry->next = idealEntry->next;
			idealEntry->next = freeEntry;
			new (std::addressof(freeEntry->value)) value_type(std::forward<Arg>(a_value));
			return std::make_pair(make_iterator(freeEntry), true);
		}

		while (takenIdealEntry->next != idealEntry) {  // find entry that links here
			takenIdealEntry = takenIdealEntry->next;
		}

		// move taken slot out, so we can move in
		new (std::addressof(freeEntry->value)) value_type(std::move(idealEntry->value));
		freeEntry->next = idealEntry->next;
		takenIdealEntry->next = freeEntry;
		new (std::addressof(idealEntry->value)) value_type(std::forward<Arg>(a_value));
		idealEntry->next = const_cast<entry_type*>(_sentinel);
		return std::make_pair(make_iterator(idealEntry), true);
	}


	iterator make_iterator(entry_type* a_entry)
	{
		return iterator(a_entry, get_entries() + _capacity);
	}


	const_iterator make_iterator(entry_type* a_entry) const
	{
		return const_iterator(a_entry, get_entries() + _capacity);
	}


	UInt32 calc_hash(const key_type& a_key) const
	{
		return hash_function()(a_key);
	}


	UInt32 calc_idx(const key_type& a_key) const
	{
		return calc_hash(a_key) & (_capacity - 1);	// capacity is always a factor of 2, so this is a faster modulo
	}


	entry_type* calc_pos(const key_type& a_key) const
	{
		return const_cast<entry_type*>(get_entries() + calc_idx(a_key));
	}


	// assumes not empty
	entry_type* get_free_entry()
	{
		entry_type* entry = nullptr;
		do {
			_freeIdx = (_capacity - 1) & (_freeIdx - 1);
			entry = get_entries() + _freeIdx;
		} while (entry->next);

		--_freeCount;
		return entry;
	}


	bool comp_key(const key_type& a_lhs, const key_type& a_rhs) const
	{
		return key_eq()(a_lhs, a_rhs);
	}


	bool grow()
	{
		if (_capacity == (UInt32)1 << 31) {
			return false;
		}

		UInt32 newCapacity = _capacity ? _capacity << 1 : min_size();
		return grow(newCapacity);
	}


	bool grow(UInt32 a_newCapacity)
	{
		auto oldEntries = get_entries();
		auto begIter = begin();
		auto endIter = end();

		auto newEntries = allocate(a_newCapacity);
		if (!newEntries) {
			return false;
		}
		else if (newEntries == oldEntries) {
			_capacity = a_newCapacity;
			return true;
		}
		else {
			_capacity = a_newCapacity;
			_freeCount = a_newCapacity;
			_freeIdx = a_newCapacity;
			set_entries(newEntries);

			while (begIter != endIter) {
				insert(std::move(*begIter));
				++begIter;
			}

			deallocate(oldEntries);
			return true;
		}
	}


	const key_type& get_key(const value_type& a_value) const
	{
		traits_type traits;
		return traits(a_value);
	}


	entry_type* allocate(std::size_t a_num)
	{
		return _allocator.allocate(a_num);
	}


	void deallocate(entry_type* a_ptr)
	{
		_allocator.deallocate(a_ptr);
	}


	entry_type* get_entries() const
	{
		return _allocator.get_entries();
	}


	void set_entries(entry_type* a_entries)
	{
		_allocator.set_entries(a_entries);
	}


	size_type min_size() const
	{
		return _allocator.min_size();
	}

	// members
	UInt64			  _pad00;	   // 00
	UInt32			  _pad08;	   // 08
	UInt32			  _capacity;   // 0C - this must be 2^n, or else terrible things will happen
	UInt32			  _freeCount;  // 10
	UInt32			  _freeIdx;	   // 14
	const entry_type* _sentinel;   // 18
	allocator_type	  _allocator;  // 20
};


template <class Key, class T>
struct BSTScatterTableTraits
{
public:
	typedef Key key_type;
	typedef T mapped_type;
	typedef BSTTuple<const Key, T> value_type;


	const key_type& operator()(const value_type& a_value) const
	{
		return a_value.first;
	}
};

template <class T, UInt32 N = 8>
struct BSTScatterTableHeapAllocator
{
public:
	typedef T entry_type;
	typedef UInt32 size_type;


	BSTScatterTableHeapAllocator() :
		_pad00(0),
		_entries(0)
	{}


	entry_type* allocate(std::size_t a_num)
	{
		auto size = a_num * sizeof(entry_type);
		auto mem = (entry_type*)Heap_Allocate(size);
		memset(mem, 0, size);
		return mem;
	}


	void deallocate(entry_type* a_ptr)
	{
		Heap_Free(a_ptr);
	}


	entry_type* get_entries() const
	{
		return _entries;
	}


	void set_entries(entry_type* a_entries)
	{
		_entries = a_entries;
	}


	size_type min_size() const
	{
		return static_cast<size_type>(1) << 3;
	}


	size_type max_size() const
	{
		return static_cast<size_type>(1) << 31;
	}

private:
	// members
	UInt64		_pad00;	   // 00 (20)
	entry_type* _entries;  // 08 (28)
};
STATIC_ASSERT(sizeof(BSTScatterTableHeapAllocator<void*, 8>) == 0x10);


template <class Key, class T, class Hash = BSHash::CRC32Hash<Key>, class KeyEqual = std::equal_to<Key>>
class BSTHashMap : public BSTScatterTable<BSTScatterTableTraits<Key, T>, 8, BSTScatterTableHeapAllocator, Hash, KeyEqual>
{
public:
};
STATIC_ASSERT(sizeof(BSTHashMap<UInt32, void*>) == 0x30);


template <class Key>
struct BSTSetTraits
{
public:
	typedef Key key_type;
	typedef void mapped_type;
	typedef Key value_type;

	const key_type& operator()(const value_type& a_value) const
	{
		return a_value;
	}
};


template <class Key, class Hash = BSHash::CRC32Hash<Key>, class KeyEqual = std::equal_to<Key>>
class BSTSet : public BSTScatterTable<BSTSetTraits<Key>, 8, BSTScatterTableHeapAllocator, Hash, KeyEqual>
{
public:
};
STATIC_ASSERT(sizeof(BSTSet<UInt32, void*>) == 0x30);

template <class Traits, UInt32 N, template <class, UInt32> class Allocator, class Hash, class KeyEqual>
typename BSTScatterTable<Traits,N, Allocator,Hash,KeyEqual>::BSTScatterTableEntry BSTScatterTable<Traits, N, Allocator, Hash, KeyEqual>::sentinel = BSTScatterTable<Traits, N, Allocator, Hash, KeyEqual>::BSTScatterTableEntry();

template <class T>
class BSTOptional
{
	T AlignedBuffer;
	bool hasValue;
};
