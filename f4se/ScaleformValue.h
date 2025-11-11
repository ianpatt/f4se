#pragma once

#include "f4se_common/Utilities.h"
#include "f4se/GameEvents.h"
#include "f4se/ScaleformTypes.h"
#include "f4se/NiTypes.h"

class GFxMovieView;
class GFxMovieRoot;

// 20
class GFxValue
{
public:
	GFxValue()					: objectInterface(NULL), type(kType_Undefined), unk18(nullptr)	{ data.obj = nullptr; }
	GFxValue(int v)				: objectInterface(NULL), type(kType_Int), unk18(nullptr)		{ data.number = v; }
	GFxValue(UInt32 v)			: objectInterface(NULL), type(kType_UInt), unk18(nullptr)		{ data.u32 = v; }
	GFxValue(SInt32 v)			: objectInterface(NULL), type(kType_Int), unk18(nullptr)		{ data.s32 = v; }
	GFxValue(double v)			: objectInterface(NULL), type(kType_Number), unk18(nullptr)		{ data.number = v; }
	GFxValue(bool v)			: objectInterface(NULL), type(kType_Bool), unk18(nullptr)		{ data.boolean = v; }
	GFxValue(const char* ps)	: objectInterface(NULL), type(kType_String), unk18(nullptr)		{ data.string = ps; }

	GFxValue(GFxValue * value)  : objectInterface(value->objectInterface), type(value->type), unk18(value->unk18) { data.obj = value->data.obj; }
	~GFxValue();

	enum Type
	{
		kType_Undefined =	0,
		kType_Null,
		kType_Bool,
		kType_Int,
		kType_UInt,
		kType_Number,
		kType_String,
		kType_Unknown7,
		kType_Object,
		kType_Array,
		kType_DisplayObject,
		kType_Function,

		kTypeFlag_Managed =	1 << 6,

		kMask_Type =		0x8F,	// not sure why it checks the top bit
	};

	union Data
	{
		UInt32			u32;
		SInt32			s32;
		double			number;
		bool			boolean;
		const char		* string;
		const char		** managedString;
		void			* obj;
	};

	// D8
	class DisplayInfo
	{
	public:
		DisplayInfo() : _varsSet(0), unkD0(0), unkD6(0) {}
		enum
		{
			kChange_x				= (1 << 0),
			kChange_y				= (1 << 1),
			kChange_rotation		= (1 << 2),
			kChange_xscale			= (1 << 3),
			kChange_yscale			= (1 << 4),
			kChange_alpha			= (1 << 5),
			kChange_visible			= (1 << 6),
			kChange_z				= (1 << 7),
			kChange_xrotation		= (1 << 8),
			kChange_yrotation		= (1 << 9),
			kChange_zscale			= (1 << 10),
			kChange_FOV				= (1 << 11),
			kChange_projMatrix3D	= (1 << 12),
			kChange_viewMatrix3D	= (1 << 13)
		};

		double		_x;				// 00
		double		_y;				// 08
		double		_rotation;		// 10
		double		_xScale;		// 18
		double		_yScale;		// 20
		double		_alpha;			// 28
		bool		_visible;		// 30
		double		_z;				// 38
		double		_xRotation;		// 40
		double		_yRotation;		// 48
		double		_zScale;		// 50
		double		_perspFOV;		// 58
		GMatrix3F	_viewMatrix3D;	// 60
		GMatrix4F	_perspectiveMatrix3D;	// A0
		UInt32		unkD0;			// D0
		UInt16		_varsSet;		// D4
		UInt16		unkD6;			// D6

		void SetX(double x)					{ SetFlags(kChange_x); _x = x; }
		void SetY(double y)					{ SetFlags(kChange_y); _y = y; }
		void SetRotation(double degrees)	{ SetFlags(kChange_rotation); _rotation = degrees; }
		void SetXScale(double xscale)		{ SetFlags(kChange_xscale); _xScale = xscale; }
		void SetYScale(double yscale)		{ SetFlags(kChange_yscale); _yScale = yscale; }
		void SetAlpha(double alpha)			{ SetFlags(kChange_alpha); _alpha = alpha; }
		void SetVisible(bool visible)		{ SetFlags(kChange_visible); _visible = visible; }
		void SetZ(double z)					{ SetFlags(kChange_z); _z = z; }
		void SetXRotation(double degrees)	{ SetFlags(kChange_xrotation); _xRotation = degrees; }
		void SetYRotation(double degrees)	{ SetFlags(kChange_yrotation); _yRotation = degrees; }
		void SetZScale(double zscale)		{ SetFlags(kChange_zscale); _zScale = zscale; }
		void SetFOV(double fov)				{ SetFlags(kChange_FOV); _perspFOV = fov; }
		void SetProjectionMatrix3D(const GMatrix4F *pmat)  
		{ 
			if (pmat) {
				SetFlags(kChange_projMatrix3D);
				_perspectiveMatrix3D = *pmat;
			} else
				ClearFlags(kChange_projMatrix3D); 
		}
		void SetViewMatrix3D(const GMatrix3F *pmat) 
		{ 
			if (pmat) { 
				SetFlags(kChange_viewMatrix3D); 
				_viewMatrix3D = *pmat;
			} else
				ClearFlags(kChange_viewMatrix3D); 
		}

		// Convenience functions
		void SetPosition(double x, double y) { SetFlags(kChange_x | kChange_y); _x = x; _y = y; }
		void SetScale(double xscale, double yscale)  { SetFlags(kChange_xscale | kChange_yscale); _xScale = xscale; _yScale = yscale; }

		void SetFlags(UInt32 flags) { _varsSet |= flags; }
		void ClearFlags(UInt32 flags) { _varsSet &= ~flags; }
	};

	class ObjectInterface
	{
	public:
		class ArrayVisitor
        {
        public:
            virtual ~ArrayVisitor() {}
            virtual void Visit(UInt32 idx, GFxValue * val) = 0;
        };
		class ObjVisitor
		{
		public:
			virtual ~ObjVisitor() { }
			virtual bool ShowDisplayMembers(void) { return false; }
			virtual void Visit(const char * member, GFxValue * value) = 0;
		};

		void			* unk00;
		GFxMovieView	* view;

		MEMBER_FN_PREFIX(ObjectInterface);
		DEFINE_MEMBER_FN(HasMember, bool, 0x01AC8410, void* pData, const char* name);
		DEFINE_MEMBER_FN(GetMember, bool, 0x01AC2D50, void* pData, const char* name, GFxValue* pValue, bool isDisplayObj);
		DEFINE_MEMBER_FN(SetMember, bool, 0x01AE1170, void* pData, const char* name, const GFxValue* pValue, bool isDisplayObj);
		DEFINE_MEMBER_FN(Invoke, bool, 0x01ACA510, void * pData, GFxValue * result, const char * name, GFxValue * args, UInt64 numArgs, UInt8 isDisplayObj);
		DEFINE_MEMBER_FN(CreateEmptyMovieClip, bool, 0x01AA88C0, void* pData, GFxValue* pValue, const char* instanceName, SInt32 depth);
		DEFINE_MEMBER_FN(AttachMovie, bool, 0x01AA1D60, void* pData, GFxValue* pValue, const char* symbolName, const char* instanceName, SInt32 depth, const void * initArgs);
		DEFINE_MEMBER_FN(GetArraySize, UInt32, 0x01ABECD0, void * pData);
		DEFINE_MEMBER_FN(SetArraySize, bool, 0x01B8B630, void * pData, UInt32 size);
		DEFINE_MEMBER_FN(GetElement, bool, 0x01AC1790, void * pData, UInt32 index, GFxValue * value);
		DEFINE_MEMBER_FN(PushBack, bool, 0x01AD7AD0, void * pData, GFxValue * value);
		DEFINE_MEMBER_FN(PopBack, bool, 0x01AD4D40, void * pData, GFxValue * value);
		DEFINE_MEMBER_FN(VisitElements, void, 0x01AEADE0, void * pData, ArrayVisitor * visitor, UInt32 idx, SInt32 count);
		DEFINE_MEMBER_FN(GotoLabeledFrame, bool, 0x01AC8190, void * pData, const char * frameLabel, bool stop);
		DEFINE_MEMBER_FN(VisitMembers, void, 0x01AEAF10, void * pData, ObjVisitor * visitor, bool isDisplayObj);
		DEFINE_MEMBER_FN(GetText, bool, 0x01AC6990, void * pData, GFxValue * value, bool html);
		DEFINE_MEMBER_FN(SetText, bool, 0x01AE33E0, void * pData, const char * text, bool html);
		DEFINE_MEMBER_FN(GetDisplayInfo, bool, 0x01AC10A0, void * pData, DisplayInfo * displayInfo);
		DEFINE_MEMBER_FN(SetDisplayInfo, bool, 0x01ADFA40, void * pData, DisplayInfo * displayInfo);
		DEFINE_MEMBER_FN(AddManaged_Internal, void, 0x01AD0DE0, GFxValue * value, void * pData);
		DEFINE_MEMBER_FN(ReleaseManaged_Internal, void, 0x01AD0E20, GFxValue * value, void * pData);
	};

	ObjectInterface	* objectInterface;	// 00
	Type			type;				// 08
	Data			data;				// 10
	void			* unk18;			// 18

	UInt32	GetType(void) const		{ return type & kMask_Type; }
	bool	IsManaged(void) const	{ return (type & kTypeFlag_Managed) != 0; }
	void	CleanManaged(void);
	void	AddManaged(void);

	bool	IsUndefined() const		{ return GetType() == kType_Undefined; }
	bool	IsNull() const			{ return GetType() == kType_Null; }
	bool	IsBool() const			{ return GetType() == kType_Bool; }
	bool	IsNumber() const		{ return GetType() == kType_Number; }
	bool	IsString() const		{ return GetType() == kType_String; }
	bool	IsObject() const		{ return (GetType() == kType_Object) ||  GetType() == kType_Array ||  GetType() == kType_DisplayObject; }
	bool	IsArray() const			{ return GetType() == kType_Array; }
	bool	IsDisplayObject() const	{ return GetType() == kType_DisplayObject; }
	bool	IsFunction() const		{ return GetType() == kType_Function; }

	void	SetUndefined(void);
	void	SetNull(void);
	void	SetBool(bool value);
	void	SetInt(SInt32 value);
	void	SetUInt(UInt32 value);
	void	SetNumber(double value);
	void	SetString(const char * value);

	bool			GetBool(void) const;
	const char *	GetString(void) const;
	double			GetNumber(void) const;
	SInt32			GetInt(void) const;
	UInt32			GetUInt(void) const;

	bool	HasMember(const char * name);
	bool	SetMember(const char * name, GFxValue * value);
	bool	GetMember(const char * name, GFxValue * value);
	bool	Invoke(const char * name, GFxValue * result, GFxValue * args, UInt32 numArgs);

	bool	CreateEmptyMovieClip(GFxValue* pValue, const char* instanceName, SInt32 depth);
	bool	AttachMovie(GFxValue* pValue, const char* symbolName, const char* instanceName, SInt32 depth, const void * initArgs);
	bool	GotoLabeledFrame(const char * frameLabel, bool stop);

	UInt32	GetArraySize();
	bool	SetArraySize(UInt32 size);
	bool	GetElement(UInt32 index, GFxValue * value);
	bool	PushBack(GFxValue * value);
	bool	PopBack(GFxValue * value);
	void	VisitElements(ObjectInterface::ArrayVisitor * visitor, UInt32 idx, SInt32 count);
	void	VisitMembers(ObjectInterface::ObjVisitor * visitor);
	bool	GetText(GFxValue * value, bool html);
	bool	SetText(const char * text, bool html);
	bool	GetDisplayInfo(DisplayInfo * displayInfo);
	bool	SetDisplayInfo(DisplayInfo * displayInfo);

	MEMBER_FN_PREFIX(GFxValue);
	DEFINE_MEMBER_FN(RemoveChild_Internal, void, 0x01B11BD0, GFxValue * name);
};

// 38
class BSGFxObject : public GFxValue
{
public:
	BSGFxObject() : unk20(0), unk28(0), unk30(0) { }
	BSGFxObject(GFxValue * value) : GFxValue(value), unk20(0), unk28(0), unk30(0)
	{
		if(value->IsManaged())
			value->AddManaged();
	}

	UInt64	unk20;	// 20
	UInt64	unk28;	// 28
	UInt64	unk30;	// 30
};

// 50
class BSGFxDisplayObject : public BSGFxObject
{
public:
	BSGFxDisplayObject() : parent(nullptr) { }
	BSGFxDisplayObject(GFxValue * value) : BSGFxObject(value), parent(nullptr)
	{
		GFxValue width, height;
		GetMember("width", &width);
		GetMember("height", &height);
		initialState.originalWidth = width.GetNumber();
		initialState.originalHeight = height.GetNumber();
	}
	virtual ~BSGFxDisplayObject()
	{
		if(parent)
			CALL_MEMBER_FN(parent, RemoveChild_Internal)(this);
	};

	struct BSDisplayInfo
	{
		BSGFxDisplayObject		* displayObject;	// 00
		BSGFxDisplayObject		* unk08;			// 08
		GFxValue::DisplayInfo	displayInfo1;		// 10
		UInt64					unkE8;				// E8
		GFxValue::DisplayInfo	displayInfo2;		// F0
	};

	struct InitialDisplayState
	{
		InitialDisplayState() : originalWidth(0), originalHeight(0) {}

		float originalWidth;
		float originalHeight;
	};

	BSGFxDisplayObject*		parent;			// 40
	InitialDisplayState		initialState;	// 48
};
STATIC_ASSERT(sizeof(BSGFxDisplayObject) == 0x50);
STATIC_ASSERT(offsetof(BSGFxDisplayObject::BSDisplayInfo, displayInfo2) == 0xF0);

class BSGFxShaderFXTarget;

// 38
struct UIShaderColors
{
	UIShaderColors() : colorBrightness(0), enabledStates(0)
	{
		backgroundQuad.m_top = 0;
		backgroundQuad.m_bottom = 0;
		backgroundQuad.m_left = 0;
		backgroundQuad.m_right = 0;
	}

	enum Flags
	{
		kBackgroundQuad			= (1 << 0),
		kColorMultiplier		= (1 << 1),
		kVerticalGradient		= (1 << 2),
		kUseAlphaForDropshadow	= (1 << 3)
	};

	NiRect<float> backgroundQuad;	// 00
	NiColorA backgroundColor;		// 10
	NiColorA colorMultipliers;		// 20
	float colorBrightness;			// 30
	int enabledStates;				// 34
};

// 48
struct UIShaderFXInfo
{
	NiRect<float> renderQuad;	// 00
	UIShaderColors shaderFX;	// 10
};

enum HUDColorTypes
{
	kHUDColorTypes_NoColorMultiplier = 0,
	kHUDColorTypes_MenuNoColorBackground,
	kHUDColorTypes_GameplayHUDColor,
	kHUDColorTypes_PlayerSetColor,
	kHUDColorTypes_PowerArmorColorOnly,
	kHUDColorTypes_WarningColor,
	kHUDColorTypes_AltWarningColor,
	kHUDColorTypes_CustomColor
};

// B0
class BSGFxShaderFXTarget : public BSGFxDisplayObject,
							public BSTEventSink<ApplyColorUpdateEvent>
{
public:
	BSGFxShaderFXTarget() { }
	BSGFxShaderFXTarget(GFxValue * source) : BSGFxDisplayObject(source), 
		HUDColorType(kHUDColorTypes_NoColorMultiplier), backgroundColorType(kHUDColorTypes_NoColorMultiplier) {  }//{ CALL_MEMBER_FN(this, Impl_ctor)(source); }
	virtual ~BSGFxShaderFXTarget();// { CALL_MEMBER_FN(this, Impl_dtor)(); };

	virtual void AppendShaderFXInfos(BSTArray<UIShaderFXInfo>* colorFX, BSTArray<UIShaderFXInfo>* backgroundFX)
	{
		Impl_AppendShaderFXInfos(colorFX, backgroundFX);
	};

	virtual	EventResult	ReceiveEvent(ApplyColorUpdateEvent * evn, void * dispatcher);

	UIShaderColors shaderFX;						// 38
	BSTArray<BSGFxShaderFXTarget*> shaderFXObjects;	// 
	HUDColorTypes HUDColorType;						// 
	HUDColorTypes backgroundColorType;

	DEFINE_STATIC_HEAP(Heap_Allocate, Heap_Free)

	void SetFilterColor(bool isHostile);

	DEFINE_MEMBER_FN_1(Impl_ctor, BSGFxShaderFXTarget *, 0x01AFE210, GFxValue * source);
	DEFINE_MEMBER_FN_2(Impl_ctor_2, BSGFxShaderFXTarget*, 0x01AFE2F0, GFxValue* source, const char* memberName);
	DEFINE_MEMBER_FN_2(Impl_ctor_3, BSGFxShaderFXTarget*, 0x01AFE3D0, GFxMovieView* source, const char* memberName);
	DEFINE_MEMBER_FN_0(Impl_dtor, void, 0x01AFE160);
	DEFINE_MEMBER_FN_2(Impl_AppendShaderFXInfos, void, 0x01AFE590, BSTArray<UIShaderFXInfo>* colorFX, BSTArray<UIShaderFXInfo>* backgroundFX);
	DEFINE_MEMBER_FN_1(GetColorMultipliersFromType, NiColor*, 0x01AFF710, NiColor* result); // This function acquires the HUD color by type e.g. normal, PA, hostile
	DEFINE_MEMBER_FN_2(EnableColorMultipliers, void, 0x01AFF400, const NiColor* color, float brightness); // Sets explicit component filter color
	DEFINE_MEMBER_FN_0(ApplyBackgroundColorFromType, void, 0x01AFF650);
};
STATIC_ASSERT(sizeof(BSGFxShaderFXTarget) == 0xB0);

typedef void * (* _GetExtDisplayInfo)(BSGFxDisplayObject::BSDisplayInfo * dInfo, BSGFxDisplayObject * target);
extern RelocAddr <_GetExtDisplayInfo> GetExtDisplayInfo;

typedef void (* _SetExtDisplayInfoAlpha)(void * dInfo, double alpha);
extern RelocAddr <_SetExtDisplayInfoAlpha> SetExtDisplayInfoAlpha;

typedef void (* _SetExtDisplayInfo)(BSGFxDisplayObject::BSDisplayInfo * dInfo);
extern RelocAddr <_SetExtDisplayInfo> SetExtDisplayInfo;

typedef void(* _PlayUISound)(const char *);
extern RelocAddr<_PlayUISound>	PlayUISound;

typedef void(*_CreateBaseShaderTarget)(BSGFxShaderFXTarget * & component, GFxValue & stage);
extern RelocAddr<_CreateBaseShaderTarget>	CreateBaseShaderTarget;
