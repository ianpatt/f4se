#pragma once

#include "f4se_common/Utilities.h"
#include "f4se_common/Relocation.h"

#include "f4se/GameInput.h"
#include "f4se/GameTypes.h"
#include "f4se/GameUtilities.h"
#include "f4se/ScaleformAPI.h"
#include "f4se/ScaleformCallbacks.h"
#include "f4se/ScaleformValue.h"

#include "f4se/NiTypes.h"

typedef GFxValue* (*_GetChildElement)(GFxValue * parent, GFxValue & child, const char * path);
extern RelocAddr<_GetChildElement>	GetChildElement;

enum MessageType
{
	kMessage_Refresh = 0,
	kMessage_Open,
	kMessage_Close = 3,
	kMessage_Scaleform = 5,//keydown/up
	kMessage_Message,
	kMessage_Platform = 11
};

class UIMessage
{
public:
	virtual ~UIMessage();

	BSFixedString				name;		// 08
	UInt32						type;		// 10
};

class UIMessageManager
{
public:
	MEMBER_FN_PREFIX(UIMessageManager);
	DEFINE_MEMBER_FN(SendUIMessage, void, 0x01A838C0, BSFixedString& menuName, UInt32 type);
	// 
	DEFINE_MEMBER_FN(SendUIMessageEx, void, 0x0107EE00, BSFixedString& menuName, UInt32 type, UIMessage * pExtraData);
};
extern RelocPtr<UIMessageManager*>	g_uiMessageManager;

class IMenu : 
	public SWFToCodeFunctionHandler,
	public BSInputEventUser
{
public:
	enum
	{
		kFlag_None = 0,
		kFlag_PausesGame = 1 << 0,
		kFlag_AlwaysOpen = 1 << 1,
		kFlag_UsesCursor = 1 << 2,
		kFlag_UsesMenuContext = 1 << 3,
		kFlag_Modal = 1 << 4,  // prevents lower movies with this flag from advancing
		kFlag_FreezeFrameBackground = 1 << 5,
		kFlag_OnStack = 1 << 6,
		kFlag_DisablePauseMenu = 1 << 7,
		kFlag_RequiresUpdate = 1 << 8,
		kFlag_TopmostRenderedMenu = 1 << 9,
		kFlag_UpdateUsesCursor = 1 << 10,
		kFlag_AllowSaving = 1 << 11,
		kFlag_RendersOffscreenTargets = 1 << 12,
		kFlag_InventoryItemMenu = 1 << 13,
		kFlag_DontHideCursorWhenTopmost = 1 << 14,
		kFlag_CustomRendering = 1 << 15,
		kFlag_AssignCursorToRenderer = 1 << 16,
		kFlag_ApplicationMenu = 1 << 17,
		kFlag_HasButtonBar = 1 << 18,
		kFlag_IsTopButtonBar = 1 << 19,
		kFlag_AdvancesUnderPauseMenu = 1 << 20,
		kFlag_RendersUnderPauseMenu = 1 << 21,
		kFlag_UsesBlurredBackground = 1 << 22,
		kFlag_CompanionAppAllowed = 1 << 23,
		kFlag_FreezeFramePause = 1 << 24,
		kFlag_SkipRenderDuringFreezeFrameScreenshot = 1 << 25,
		kFlag_LargeScaleformRenderCacheMode = 1 << 26,
		kFlag_UsesMovementToDirection = 1 << 27
	};
	virtual UInt32	ProcessMessage(UIMessage * msg) = 0;//???
	virtual void	AdvanceMovie(float unk0, void * unk1) = 0; //210E8C0
	virtual void	PreDisplay(void) { };
	virtual void	PostDisplay(void) { };
	virtual bool	PassesRenderConditionText(UInt32 menuRenderContext, const BSFixedString& unk1) = 0;
	virtual void	SetIsTopButtonBar(bool unk0) = 0;
	virtual void	OnMenuStackChanged(BSFixedString & menuName, bool unk1) = 0;            //UInt64 = 0;            //UInt64
	virtual void	OnMenuDisplayStateChanged(void) = 0;
	virtual void	OnAddedToMenuStack(void) = 0;
	virtual void	OnRemovedFromMenuStack(void) = 0;
	virtual bool	CanAdvanceMovie(bool unk0) = 0;
	virtual bool	CanHandleWhenDisabled(const ButtonEvent*) { return false; };
	virtual bool	OnButtonEventRelease(const BSFixedString& controlID) { return false; };
	virtual bool	CacheShaderFXQuadsForRenderer_Impl(void) = 0;
	virtual void	TransferCachedShaderFXQuadsForRenderer(const BSFixedString&) = 0;
	virtual void	SetViewportRect(const NiRect<float>& unk0) = 0;

	GFxValue		stage;			// 20
	GFxMovieView	* movie;		// 40
	BSFixedString	unk48;			// 48
	BSFixedString	menuName;		// 50
	UInt32			flags;			// 58

	/*
							A A A A A A A A B B B B B B B B C C C C C C C C D D D D D D D D
	LoadingMenu				0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 0 1 0 1 1 0 0 0 0 0 1		depth: 000E		context: 0003
	Console					0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 1 1 0 0 0 1 1 1		depth: 0013		context: 0006
	LevelUpMenu				0 0 0 0 0 1 0 0 0 0 0 0 1 1 0 1 0 0 0 0 0 1 0 0 1 1 0 0 0 1 1 1		depth: 0009		context: 0022
	FaderMenu				0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1 0		depth: 0006		context: 0022
	CursorMenu				0 0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0014		context: 0022
	VignetteMenu			0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0003		context: 0022
	MessageBoxMenu			0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 0 1 0 0 1 1 0 1 1 1 0 1		depth: 000A		context: 0022
	ContainerMenu			0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 1 1 0 1 0 0 1 0 1 0 1 0 0 1 1 0 1		depth: 0006		context: 0022
	ExamineMenu				0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 0 1 0 1 0 0 1 0 1 0 1 0 0 0 1 0 1		depth: 0009		context: 0022
	CookingMenu				0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 0 1 0 1 0 0 1 0 1 0 1 0 0 0 1 0 0		depth: 0009		context: 0022
	ExamineConfirmMenu		0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 1 0 0 0 1 0 1 1 1 0 1		depth: 0011		context: 0022
	RobotModMenu			0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 0 1 0 1 0 0 1 0 1 0 1 0 0 0 1 0 0		depth: 0009		context: 0022
	PowerArmorModMenu		0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 0 1 0 1 0 0 1 0 1 0 1 0 0 0 1 0 0		depth: 0009		context: 0022
	WorkshopMenu			0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 1 0 0 0 0 0 0 1 0 1 0 0 0 0 0 0		depth: 0006		context: 0010
	PromptMenu				0 0 0 0 0 0 0 0 1 0 0 0 0 1 0 0 1 1 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0005		context: 0022
	SitWaitMenu				0 0 0 0 0 0 0 0 1 0 0 0 1 1 0 0 1 1 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0006		context: 0012
	SleepWaitMenu			0 0 0 0 1 0 0 0 0 1 0 0 1 1 0 1 1 0 0 0 1 0 0 1 1 1 0 0 1 1 0 1		depth: 000A		context: 0022
	DialogueMenu			0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 0 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0006		context: 0022
	BarterMenu				0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 1 1 0 1 0 0 1 0 1 0 1 0 0 1 1 0 1		depth: 0006		context: 0022
	LockpickingMenu			0 0 0 0 0 0 0 0 0 1 0 0 1 1 0 0 1 0 0 0 0 0 0 0 0 1 1 0 0 0 0 1		depth: 0006		context: 000C
	BookMenu				0 0 0 0 1 0 0 0 0 1 1 0 1 1 0 0 1 0 0 0 0 0 0 1 0 1 1 0 1 0 0 1		depth: 0009		context: 0008
	SPECIALMenu				0 0 0 0 1 0 0 0 0 1 0 0 1 1 0 1 1 0 0 0 0 1 0 0 1 1 1 0 1 1 0 1		depth: 0006		context: 0022
	FavoritesMenu			0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 1 0 1 0 0 0 0 0 0 1 0 0 0 0 0 0		depth: 0006		context: 0001
	HUDMenu					0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0005		context: 0022
	PowerArmorHUDMenu		0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 0 1 0 0 0 0 1 0 0 0 0 0 0		depth: 0005		context: 0022
	PauseMenu				0 0 0 0 1 0 0 0 0 1 0 0 1 1 0 0 1 0 0 0 1 1 1 0 0 1 0 1 1 1 0 1		depth: 000B		context: 0022
	VATSMenu				0 0 0 0 0 0 0 0 1 0 0 0 1 1 0 1 1 0 0 0 0 1 0 0 0 1 0 0 0 1 0 0		depth: 0006		context: 000D
	PipboyMenu				0 0 0 0 0 0 0 0 1 0 1 0 1 1 0 0 1 0 1 0 0 0 0 1 0 1 0 0 0 1 0 1		depth: 0008		context: 0022
	PipboyHolotapeMenu		0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1 0 0 1 0 0 1		depth: 0009		context: 0022
	*/

	UInt32			unk5C;			// 5C
	UInt32			unk60;			// 60	init'd as DWord then Byte
	UInt8			depth;			// 64   defalut is 6.
	UInt32			context;		// 68	init'd in IMenu::IMenu
	UInt32			pad6C;			// 6C
};
STATIC_ASSERT(offsetof(IMenu, movie) == 0x40);
STATIC_ASSERT(offsetof(IMenu, flags) == 0x58);

struct HUDModeType
{
	BSFixedString modeString;
};

// E0
class GameMenuBase : public IMenu
{
public:
	GameMenuBase();
	virtual ~GameMenuBase();

	// BSInputEventUser overrides
	virtual bool ShouldHandleEvent(InputEvent* inputEvent /* = nullptr */) override;
	virtual void OnButtonEvent(ButtonEvent * inputEvent) override { Impl_OnGameMenuBaseButtonEvent(inputEvent); };

	// IMenu overrides
	virtual void	Invoke(Args * args) override { }
	virtual void	RegisterFunctions() override { }
	virtual UInt32	ProcessMessage(UIMessage * msg) override { return Impl_ProcessMessage(msg); };//???
	virtual void	AdvanceMovie(float unk0, void * unk1) override { return Impl_AdvanceMovie(unk0, unk1); }; //render,HUD menu uses this function to update its HUD components.
	virtual bool	PassesRenderConditionText(UInt32 menuRenderContext, const BSFixedString& unk1) override { return Impl_PassesRenderConditionText(menuRenderContext, unk1); };
	virtual void	SetIsTopButtonBar(bool unk0) override { return Impl_SetIsTopButtonBar(unk0); };
	virtual void	OnMenuStackChanged(BSFixedString & menuName, bool unk1)  override { return Impl_OnMenuStackChanged(menuName, unk1); };            //UInt64
	virtual void	OnMenuDisplayStateChanged(void) override { return Impl_OnMenuDisplayStateChanged(); };
	virtual void	OnAddedToMenuStack(void) override { return Impl_OnAddedToMenuStack(); }
	virtual void	OnRemovedFromMenuStack(void) override { return Impl_OnRemovedFromMenuStack(); };
	virtual bool	CanAdvanceMovie(bool unk0) override { return Impl_CanAdvanceMovie(unk0); }
	virtual bool	CanHandleWhenDisabled(const ButtonEvent*) override { return false; };
	virtual bool	OnButtonEventRelease(const BSFixedString & controlID) override { return false; };
	virtual bool	CacheShaderFXQuadsForRenderer_Impl(void) override { return Impl_CacheShaderFXQuadsForRenderer(); } //90 - E0
	virtual void	TransferCachedShaderFXQuadsForRenderer(const BSFixedString& unk1) override { return Impl_TransferCachedShaderFXQuadsForRenderer(unk1); };
	virtual void	SetViewportRect(const NiRect<float>& unk0) override { return Impl_SetViewportRect(unk0); }
	virtual void	AppendShaderFXInfos(BSTArray<UIShaderFXInfo>* colorFX, BSTArray<UIShaderFXInfo>* backgroundFX) { return Impl_AppendShaderFXInfos(colorFX, backgroundFX); }

	BSTArray<BSGFxShaderFXTarget*>	shaderFXObjects;
	BSGFxShaderFXTarget*			filterHolder;
	class ButtonHintBar*			buttonHintBar;
	BSTArray<UIShaderFXInfo>		cachedColorFX;
	BSTArray<UIShaderFXInfo>		cachedBackgroundFX;
	BSReadWriteLock					cachedQuadsLock;
	BSTOptional<HUDModeType>		MenuHUDMode;

	DEFINE_STATIC_HEAP(ScaleformHeap_Allocate, ScaleformHeap_Free)
private:
	// ??_7GameMenuBase@@6B@

	DEFINE_MEMBER_FN_0(Impl_ctor, void *, 0x00A6F410);
	DEFINE_MEMBER_FN_0(Impl_dtor, void *, 0x00A6F4D0);
	DEFINE_MEMBER_FN_2(Impl_AdvanceMovie, void, 0x01B135F0, float unk0, void * unk1);
	DEFINE_MEMBER_FN_1(Impl_ProcessMessage, UInt32, 0x01B134C0, UIMessage * msg);
	DEFINE_MEMBER_FN_2(Impl_PassesRenderConditionText, bool, 0x01B13A40, UInt32 unk0, const BSFixedString& unk1);
	DEFINE_MEMBER_FN_1(Impl_SetIsTopButtonBar, void, 0x00A6FA00, UInt8 unk0);
	DEFINE_MEMBER_FN_2(Impl_OnMenuStackChanged, void, 0x01B13C80, BSFixedString & menuName, bool unk1);
	DEFINE_MEMBER_FN_0(Impl_OnMenuDisplayStateChanged, void, 0x00A6FA70);
	DEFINE_MEMBER_FN_0(Impl_OnAddedToMenuStack, void, 0x00A6FB30);
	DEFINE_MEMBER_FN_0(Impl_OnRemovedFromMenuStack, void, 0x00A6FB70);
	DEFINE_MEMBER_FN_1(Impl_CanAdvanceMovie, bool, 0x01B13DD0, bool unk0);
	DEFINE_MEMBER_FN_0(Impl_CacheShaderFXQuadsForRenderer, bool, 0x00A6F810);
	DEFINE_MEMBER_FN_1(Impl_TransferCachedShaderFXQuadsForRenderer, void, 0x00A6F8A0, const BSFixedString& unk1);
	DEFINE_MEMBER_FN_1(Impl_SetViewportRect, void, 0x00A6F910, const NiRect<float>& unk0);
	DEFINE_MEMBER_FN_2(Impl_AppendShaderFXInfos, void, 0x00A6F960, BSTArray<UIShaderFXInfo>* colorFX, BSTArray<UIShaderFXInfo>* backgroundFX);
};
STATIC_ASSERT(offsetof(GameMenuBase, filterHolder) == 0x88);
STATIC_ASSERT(sizeof(GameMenuBase) == 0xE0);

// 218
class LooksMenu : public GameMenuBase
{
public:
	BSTEventSink<ChargenCharacterUpdateEvent> eventSink; // E0
	UInt64	unkE8;	// E8
	void	* unkF0; // F0 - LooksInputRepeatHandler
	UInt64 unkF8[(0x150-0xF8)/8];
	UInt32	nextBoneID;			// 150
	UInt32	currentBoneID;		// 154
	UInt64	unk158[(0x1E0-0x158)/8];
	UInt32	unk1E0;				// 1E0
	UInt32	unk1E4;				// 1E4
	UInt64	unk1E8[(0x218-0x1E8)/8];

	DEFINE_MEMBER_FN_0(LoadCharacterParameters, void, 0x00A81870);	// This function updates all the internals from the current character
																	// It's followed by a call to onCommitCharacterPresetChange
};
STATIC_ASSERT(offsetof(LooksMenu, nextBoneID) == 0x150);

enum PowerArmorHUDVisibilityRule
{
	kPowerArmorHUDVisibilityRule_Ignores_PA_HUD_Visibility = 0x0,
	kPowerArmorHUDVisibilityRule_OnlyWithPAHUDVisible,
	kPowerArmorHUDVisibilityRule_OnlyWithPAHUDNotVisible
};

// 20
struct HUDModes
{
	BSTArray<HUDModeType>		validHUDModes;
	PowerArmorHUDVisibilityRule powerArmorHUDVisibilityRule;
	bool						bCanBeVisible;
};

// 
struct HUDModeInitParams
{
	const HUDModeType*			validHUDModes;
	UInt32						numHUDModes;
	PowerArmorHUDVisibilityRule	powerArmorHUDVisibilityRule;
};

struct CountdownTimer
{
	UInt64 StartTime;
	UInt64 EndTime;
	bool IsActive;
};

// F8
class HUDComponentBase : public BSGFxShaderFXTarget
{
public:
	HUDComponentBase(GFxValue * parent, const char * componentName, const HUDModeInitParams* initParams);
	virtual ~HUDComponentBase();

	virtual bool Unk_02(void * unk1) { return false; }
	virtual void Unk_03() { }
	virtual void UpdateComponent() { Impl_UpdateComponent(); } // Does stuff
	virtual void UpdateVisibilityContext(void * unk1);
	virtual void ColorizeComponent();
	virtual bool IsVisible() { return Impl_IsVisible(); }
	virtual bool Unk_08() { return hudModes.bCanBeVisible; }

	CountdownTimer					fadeOutTimer;				// B0
	HUDModes						hudModes;					// C8
	float							fadePerSecond;				// E8
	UInt32							fadeState;					// EC
	bool							bFlashElementsRegistered;	// F0
	bool							bWidgetDisplayEnabled;		// F1
	bool							bDisplayWarningColor;		// F2 - This chooses the warning color over the default color
	UInt8							padF3[5];					// F3

	MEMBER_FN_PREFIX(HUDComponentBase);
	DEFINE_MEMBER_FN_3(Impl_ctor, HUDComponentBase *, 0x009B9200, GFxValue * parent, const char * componentName, const HUDModeInitParams* initParams);
	DEFINE_MEMBER_FN_0(Impl_IsVisible, bool, 0x009B9520);
	DEFINE_MEMBER_FN_0(Impl_UpdateComponent, void, 0x009B92A0);
};
STATIC_ASSERT(offsetof(HUDComponentBase, hudModes) == 0xC8);
STATIC_ASSERT(offsetof(HUDComponentBase, fadePerSecond) == 0xE8);
STATIC_ASSERT(sizeof(HUDComponentBase) == 0xF8);

typedef bool (* _HasHUDContext)(HUDModes* contexts, void * unk1);
extern RelocAddr <_HasHUDContext> HasHUDContext;


// 110
class HUDComponents
{
public:
	UInt64								unk00;					// 00
	HUDComponentBase					* components[0x1E];		// 08
	UInt64								unk98;					// 98
	UInt64								unk100;					// 100
	UInt32								numComponents;			// 108 - 0x1E
};

// 220
class HUDMenu : public GameMenuBase
{
public:
	BSTEventSink<UserEventEnabledEvent> inputEnabledSink;		// E0
	BSTEventSink<RequestHUDModesEvent>	requestHudModesSink;	// E8
	HUDComponents						children;				// F0
	UInt64								unk200;					// 200
	UInt64								unk208;					// 208
	UInt64								unk210;					// 210
	UInt64								unk218;					// 218
};
STATIC_ASSERT(offsetof(HUDMenu, unk200) == 0x200);

// 18
class PipboySubMenu : public BSTEventSink<struct PipboyValueChangedEvent>
{
public:

	virtual ~PipboySubMenu();

	virtual	void Unk02();	// Pure, called by PipboySubMenu::ReceiveEvent

	GFxValue	*value;
	UInt64		unk10;
};

// 18
class PipboyQuestMenu : public PipboySubMenu
{
public:

	virtual ~PipboyQuestMenu();	
};


// 18 
class PipboyValue
{
public:
	virtual ~PipboyValue();
	virtual	void Unk01();	// Sets unk0C to 0
	virtual void Unk02();	// pure
	virtual void Unk03(void *arg1);
	virtual void Unk04();	// pure

	UInt32		unk08;	// 08 - init'd to incremental variable
	UInt8		unk0C;	// 0C - init'd to 1
	UInt8		unk0D;	// 0D - init'd to 1
	UInt16		pad0E;	// 0E
	PipboyValue	*unk10;	// 10
};

template <class T>
class PipboyPrimitiveValue : public PipboyValue
{
public:

	T	value;	// 18	
};
STATIC_ASSERT(offsetof(PipboyPrimitiveValue<bool>, value) == 0x18);

class PipboyObject : public PipboyValue
{
public:
	struct PipboyTableItem
	{
		BSFixedString	key;
		PipboyValue		*value;

		bool operator==(const BSFixedString & a_name) const { return key == a_name; }
		operator BSFixedString() const { return key; }

		static inline UInt32 GetHash(BSFixedString * key)
		{
			UInt32 hash;
			CalculateCRC32_64(&hash, (UInt64)key->data, 0);
			return hash;
		}
	};

	virtual ~PipboyObject();

	tHashSet<PipboyTableItem, BSFixedString>	table;	// 18
	//...
};
STATIC_ASSERT(offsetof(PipboyObject, table) == 0x18);

// 00C
class MenuTableItem
{
public:
	typedef IMenu * (*CallbackType)(void);
	BSFixedString	name;				// 000
	IMenu			* menuInstance;		// 008	0 if the menu is not currently open
	CallbackType	menuConstructor;	// 010
	void			* unk18;			// 018

	bool operator==(const MenuTableItem & rhs) const	{ return name == rhs.name; }
	bool operator==(const BSFixedString a_name) const	{ return name == a_name; }
	operator UInt64() const								{ return (UInt64)name.data->Get<char>(); }

	static inline UInt32 GetHash(BSFixedString * key)
	{
		UInt32 hash;
		CalculateCRC32_64(&hash, (UInt64)key->data, 0);
		return hash;
	}

	void Dump(void)
	{
		_MESSAGE("\t\tname: %s", name.data->Get<char>());
		_MESSAGE("\t\tinstance: %08X", menuInstance);
	}
};

extern RelocPtr <BSReadWriteLock> g_menuTableLock;

// 250 ?
class UI
{
public:
	virtual ~UI();

	virtual void	Unk_01(void);

	typedef IMenu*	(*CreateFunc)(void);
	typedef tHashSet<MenuTableItem,BSFixedString> MenuTable;

	bool	IsMenuOpen(const BSFixedString & menuName);
	IMenu * GetMenu(BSFixedString & menuName);
	IMenu * GetMenuByMovie(GFxMovieView * movie);
	void	Register(const char* name, CreateFunc creator)
	{
		CALL_MEMBER_FN(this, RegisterMenu)(name, creator, 0);
	}
	bool IsMenuRegistered(BSFixedString & menuName);

	template<typename T>
	void ForEachMenu(T & menuFunc)
	{
		const BSWriteLocker l(g_menuTableLock.GetPtr());
		menuTable.ForEach(menuFunc);
	}

	bool UnregisterMenu(BSFixedString & name, bool force = false);

	UInt64									unk08;						// 08
	UInt64									unk10;						// 10
	BSTEventDispatcher<MenuOpenCloseEvent>	menuOpenCloseEventSource;	// 70
	UInt64									unk70[(0x190 - 0x70) / 8];
	tArray<IMenu*>							menuStack;		// 190
	MenuTable								menuTable;		// 1A8
	UInt64									unk1D8;         // 1D8
	UInt32									numPauseGame;   // 1E0 isInMenuMode
	volatile	SInt32						numFlag2000;	// 1E4
	volatile	SInt32						numFlag80;		// 1E8
	UInt32									numFlag20;		// 1EC
	// ...

protected:
	MEMBER_FN_PREFIX(UI);
	DEFINE_MEMBER_FN(RegisterMenu, void, 0x01A7C960, const char * name, CreateFunc creator, UInt64 unk1);
	DEFINE_MEMBER_FN(IsMenuOpen, bool, 0x01A7A5B0, const BSFixedString & name);
};

extern RelocPtr <UI*> g_ui;
