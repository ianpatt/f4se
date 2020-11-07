#pragma once

#include "f4se/GameMenus.h"
#include <unordered_map>
#include <string>

struct CustomMenuData
{
	BSFixedString	menuPath;
	BSFixedString	rootPath;
	UInt32			menuFlags;
	UInt32			movieFlags;
	UInt32			extFlags;
	UInt32			depth;

	enum ExtendedFlags
	{
		kExtFlag_InheritColors = 1,
		kExtFlag_CheckForGamepad = 2
	};
};

extern BSReadWriteLock									g_customMenuLock;
extern std::unordered_map<std::string, CustomMenuData>	g_customMenuData;

class CustomMenu : public GameMenuBase
{
public:
	CustomMenu();

	virtual void	RegisterFunctions() override;
	virtual void	Invoke(Args * args) override final;
};

void LoadCustomMenu_Hook(IMenu * menu);
IMenu * CreateCustomMenu();