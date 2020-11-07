#pragma once

#include "f4se/ScaleformState.h"
#include "f4se/GameTypes.h"
#include "f4se/GameUtilities.h"

class GFxTranslator : public GFxState
{
public:
	virtual ~GFxTranslator();

	virtual void Unk_01(void);
	virtual void Unk_02(void);
	virtual void Unk_03(void);
	virtual void Unk_04(void);
};

// 10
class TranslationTableItem
{
public:
	BSFixedString	key;
	BSFixedStringW	translation;

	TranslationTableItem(BSFixedString a_key, BSFixedStringW a_translation)
		: key(a_key), translation(a_translation) {}

	bool operator==(const BSFixedString & a_name) const	{ return key == a_name; }
	operator BSFixedString() const								{ return key; }

	static inline UInt32 GetHash(BSFixedString * key)
	{
		UInt32 hash;
		CalculateCRC32_64(&hash, (UInt64)key->data, 0);
		return hash;
	}

	void Dump(void)
	{
		_MESSAGE("\t\tkey: %S ", key.data ? key.data->Get<char>() : "");
		_MESSAGE("\t\ttranslation: %S", translation.data ? translation.data->Get<wchar_t>() : L"");
	}
};

typedef tHashSet<TranslationTableItem, BSFixedString> TranslationTable;

// 50
class BSScaleformTranslator : public GFxTranslator
{
public:
	UInt64 unk18[(0x20 - 0x18) / 8];	// 18
	TranslationTable	translations;	// 20
};

typedef void (__cdecl * _CreateEmptyString)(BSFixedString * pOut);
extern RelocAddr<_CreateEmptyString> CreateEmptyString;

typedef void (__cdecl * _SetWideString)(BSFixedString * pOut, wchar_t * bufIn);
extern RelocAddr<_SetWideString> SetWideString;
