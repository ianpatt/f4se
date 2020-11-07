#pragma once

// 10
class GRefCountImplCore
{
public:
	GRefCountImplCore() :refCount(1) { }
	virtual ~GRefCountImplCore() { }

//	void	** _vtbl;				// 00
	volatile	SInt32	refCount;	// 08
	UInt32		pad0C;				// 0C
};

class GRefCountImpl : public GRefCountImplCore
{
public:
	GRefCountImpl() { }
	virtual ~GRefCountImpl() { }
};

class GRefCountBaseStatImpl : public GRefCountImpl
{
public:
	GRefCountBaseStatImpl() { }
	virtual ~GRefCountBaseStatImpl() { }
};

class GRefCountBase : public GRefCountBaseStatImpl
{
public:
	GRefCountBase() { }
	virtual ~GRefCountBase() { }
};

template <typename T>
class GRect
{
public:
	T	left;
	T	top;
	T	right;
	T	bottom;
};

class GMatrix3F
{
public:
	float	m[3][4];
};

class GMatrix4F
{
public:
	float	m[4][4];
};
