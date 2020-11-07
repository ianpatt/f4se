#pragma once

#include "f4se_common/Utilities.h"

class NiBinaryStream;
class NiObject;

// 708
class NiStream
{
public:
	virtual ~NiStream();

	virtual bool	LoadStream(NiBinaryStream * stream);
	virtual bool	LoadBuf(char * buf, UInt32 len);
	virtual bool	LoadPath(const char * path);
	virtual bool	SaveStream(NiBinaryStream * stream);
	virtual bool	SaveBuf(char ** buf, UInt32 * len);
	virtual bool	SavePath(const char * path);

	virtual void	Unk_07(void);
	virtual void	Unk_08(void);
	virtual void	Unk_09(void);
	virtual void	Unk_0A(void);
	virtual void	Unk_0B(void);
	virtual void	Unk_0C(void);
	virtual void	Unk_0D(void);
	virtual void	Unk_0E(void);
	virtual void	Unk_0F(void);
	virtual void	Unk_10(void);
	virtual void	Unk_11(void);
	virtual void	Unk_12(void);
	virtual void	Unk_13(void);
	virtual void	Unk_14(void);
	virtual void	Unk_15(void);
	virtual void	Unk_16(void);

	UInt64	unk08[(0x708 - 0x08) >> 3];	// 08

	MEMBER_FN_PREFIX(NiStream);
	DEFINE_MEMBER_FN(ctor, NiStream *, 0x01BB8F90);
	DEFINE_MEMBER_FN(dtor, void, 0x01BB91C0);
	DEFINE_MEMBER_FN(AddObject, void, 0x01BB92F0, NiObject * object);
};
