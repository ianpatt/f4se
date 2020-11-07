#include "f4se/CustomMenu.h"
#include "f4se/ScaleformLoader.h"

BSReadWriteLock									g_customMenuLock;
std::unordered_map<std::string, CustomMenuData>	g_customMenuData;

CustomMenu::CustomMenu() : GameMenuBase()
{
	
}

// Normally this code would happen in the constructor, but the menu name isn't set until after construction
void LoadCustomMenu_Hook(IMenu * menu)
{
	BSWriteLocker locker(&g_customMenuLock);
	auto menuData = g_customMenuData.find(menu->menuName.c_str());
	if(menuData != g_customMenuData.end())
	{
		BSFixedString menuPath = menuData->second.menuPath;
		BSFixedString rootPath = menuData->second.rootPath;
		UInt32 movieFlags = menuData->second.movieFlags;
		UInt32 extFlags = menuData->second.extFlags;
		menu->flags = menuData->second.menuFlags;
		menu->depth = menuData->second.depth;

		if((menu->flags & IMenu::kFlag_UsesCursor) && (extFlags & CustomMenuData::kExtFlag_CheckForGamepad))
		{
			if((*g_inputDeviceMgr)->IsGamepadEnabled())
				menu->flags &= ~IMenu::kFlag_UsesCursor;
		}

		if (CALL_MEMBER_FN((*g_scaleformManager), LoadMovie)(menu, menu->movie, menuPath.c_str(), rootPath.c_str(), movieFlags))
		{
			menu->stage.SetMember("menuFlags", &GFxValue(menu->flags));
			menu->stage.SetMember("movieFlags", &GFxValue(movieFlags));
			menu->stage.SetMember("extendedFlags", &GFxValue(extFlags));

			GameMenuBase * gameMenu = static_cast<GameMenuBase*>(menu);

			CreateBaseShaderTarget(gameMenu->filterHolder, menu->stage);

			if(extFlags & CustomMenuData::kExtFlag_InheritColors)
			{
				gameMenu->filterHolder->SetFilterColor(false);
				(*g_colorUpdateDispatcher)->eventDispatcher.AddEventSink(gameMenu->filterHolder);
			}

			if (menu->flags & IMenu::kFlag_CustomRendering)
			{
				gameMenu->shaderFXObjects.Push(gameMenu->filterHolder);
			}
		}
	}
}

void CustomMenu::RegisterFunctions() 
{
	RegisterNativeFunction("PlaySound", 0);
	RegisterNativeFunction("OpenMenu", 1);
	RegisterNativeFunction("CloseMenu", 2);
}

void CustomMenu::Invoke(Args * args)
{
	switch (args->optionID)
	{
	case 0:
		{
			if(args->numArgs >= 1)
			{
				if(args->args[0].IsString())
					PlayUISound(args->args[0].GetString());
			}
		}
		break;
	case 1:
		{
			if(args->numArgs >= 1)
			{
				if(args->args[0].IsString())
				{
					BSFixedString menuName(args->args[0].GetString());
					CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Open);
				}
			}
		}
		break;
	case 2:
		{
			if(args->numArgs >= 1)
			{
				if(args->args[0].IsString())
				{
					BSFixedString menuName(args->args[0].GetString());
					CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Close);
				}
			}
		}
		break;
	default:
		break;
	}
}

IMenu * CreateCustomMenu()
{
	return new CustomMenu();
}