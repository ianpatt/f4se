#include "f4se/GameMenus.h"

// 2CA5233612B3158658DB6DB9C90FD0258F1836E2+AD
RelocPtr <UI*> g_ui(0x058D0898);

RelocAddr <_HasHUDContext> HasHUDContext(0x00A4F390);

RelocAddr<_GetChildElement>		GetChildElement(0x020F0DC0);

// 2CA5233612B3158658DB6DB9C90FD0258F1836E2+F1
RelocPtr <UIMessageManager*>	g_uiMessageManager(0x058D0AC8);

// E8B45849BEED1FD76CD4D25F030C48F09D0B41F1+90
RelocPtr <BSReadWriteLock> g_menuTableLock(0x065774B8);

bool UI::IsMenuOpen(const BSFixedString & menuName)
{
	return CALL_MEMBER_FN(this, IsMenuOpen)(menuName);
}

bool UI::IsMenuRegistered(BSFixedString & menuName)
{
	BSReadLocker locker(g_menuTableLock);
	MenuTableItem * item = menuTable.Find(&menuName);
	if (item) {
		return true;
	}

	return false;
}

IMenu * UI::GetMenu(BSFixedString & menuName)
{
	if (!menuName.data->Get<char>())
		return nullptr;

	BSReadLocker locker(g_menuTableLock);
	MenuTableItem * item = menuTable.Find(&menuName);
	if (!item) {
		return nullptr;
	}

	IMenu * menu = item->menuInstance;
	if(!menu) {
		return nullptr;
	}

	return menu;
}

IMenu * UI::GetMenuByMovie(GFxMovieView * movie)
{
	if (!movie) {
		return nullptr;
	}

	BSReadLocker locker(g_menuTableLock);

	IMenu * menu = nullptr;
	menuTable.ForEach([movie, &menu](MenuTableItem * item)
	{
		IMenu * itemMenu = item->menuInstance;
		if(itemMenu) {
			GFxMovieView * view = itemMenu->movie;
			if(view) {
				if(movie == view) {
					menu = itemMenu;
					return false;
				}
			}
		}
		return true;
	});

	return menu;
}

bool UI::UnregisterMenu(BSFixedString & name, bool force)
{
	BSWriteLocker locker(g_menuTableLock);
	MenuTableItem * item = menuTable.Find(&name);
	if (!item || (item->menuInstance && !force)) {
		return false;
	}

	return menuTable.Remove(&name);
}

HUDComponentBase::HUDComponentBase(GFxValue * parent, const char * componentName, const HUDModeInitParams* initParams)
{
	Impl_ctor(parent, componentName, initParams);
}

HUDComponentBase::~HUDComponentBase()
{
}

void HUDComponentBase::UpdateVisibilityContext(void * unk1)
{
	HasHUDContext(&hudModes, unk1);
	bool bVisible = IsVisible();
	double alpha = 0.0;
	if(bVisible) {
		alpha = 100.0;
	}

	BSGFxDisplayObject::BSDisplayInfo dInfo;
	void * unk2 = GetExtDisplayInfo(&dInfo, this);
	SetExtDisplayInfoAlpha(unk2, alpha);
	SetExtDisplayInfo(&dInfo);

	fadeState = bVisible == false;
}

void HUDComponentBase::ColorizeComponent()
{
	SetFilterColor(bDisplayWarningColor);
}

GameMenuBase::GameMenuBase() : IMenu()
{
	Impl_ctor();
}

GameMenuBase::~GameMenuBase()
{
	Impl_dtor();
}

bool GameMenuBase::ShouldHandleEvent(InputEvent* inputEvent)
{
	if (inputEvent->handled != 2 && inputEvent->eventType == InputEvent::kEventType_Button)
	{
		ButtonEvent* buttonEvent = static_cast<ButtonEvent*>(inputEvent);

		if (buttonEvent->isDown == 0.0f)
		{
			if (buttonEvent->timer >= 0.0f)
			{
				return true;
			}
		}
		else if (buttonEvent->timer == 0.0f)
		{
			return true;
		}
	}

	return false;
}