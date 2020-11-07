#include "f4se/GameStreams.h"

RelocAddr <_CreateFileStream> CreateFileStream(0x00535A60);

BSResourceNiBinaryStream::BSResourceNiBinaryStream()
{
	//
}

BSResourceNiBinaryStream::BSResourceNiBinaryStream(const char * path)
{
	CALL_MEMBER_FN(this, Construct)(path, 0, 0, 0);
}

BSResourceNiBinaryStream::~BSResourceNiBinaryStream()
{
	CALL_MEMBER_FN(this, Destroy)();
}

bool BSResourceNiBinaryStream::IsValid(void)
{
	return CALL_MEMBER_FN(this, IsValid)();
}

void BSResourceNiBinaryStream::Seek(SInt64 delta)
{
	CALL_MEMBER_FN(this, Seek)(delta);
}

UInt64 BSResourceNiBinaryStream::GetOffset(void)
{
	return offset;
}

SInt64 BSResourceNiBinaryStream::Unk_04(void * unk1)
{
	return CALL_MEMBER_FN(this, Unk_04)(unk1);
}

UInt32 BSResourceNiBinaryStream::Read(void * buf, UInt64 length)
{
	return CALL_MEMBER_FN(this, Read)(buf, length);
}

UInt32 BSResourceNiBinaryStream::Write(void * buf, UInt64 length)
{
	return CALL_MEMBER_FN(this, Write)(buf, length);
}

UInt32 BSResourceNiBinaryStream::ReadLine(char * dst, UInt32 dstLen, UInt32 terminator)
{
	return CALL_MEMBER_FN(this, ReadLine)(dst, dstLen, terminator);
}

UInt32 BSResourceNiBinaryStream::ReadLine_w(wchar_t * dst, UInt32 dstLen, UInt32 terminator)
{
	wchar_t	* iter = dst;

	if(dstLen == 0)
		return 0;

	for(UInt32 i = 0; i < dstLen - 1; i++)
	{
		wchar_t	data;

		if(Read(&data, sizeof(data)) != sizeof(data))
			break;

		if(data == terminator)
			break;

		*iter++ = data;
	}

	// null terminate
	*iter = 0;

	return iter - dst;
}
