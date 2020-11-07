#include "f4se/PapyrusUI.h"

#include "f4se/PapyrusVM.h"
#include "f4se/PapyrusNativeFunctions.h"

#include "f4se/PapyrusDelayFunctors.h"
#include "f4se/PapyrusEvents.h"

#include "f4se/Serialization.h"
#include "f4se/PapyrusSerialization.h"

#include "f4se/GameMenus.h"
#include "f4se/PapyrusScaleformAdapter.h"

#include "f4se/CustomMenu.h"

namespace papyrusUI
{
	DECLARE_STRUCT(MenuData, "UI")

	bool RegisterCustomMenu(StaticFunctionTag *, BSFixedString menuName, BSFixedString menuPath, BSFixedString rootPath, MenuData menuData)
	{
		if(!(*g_ui)->IsMenuRegistered(menuName) && !menuData.IsNone())
		{
			BSWriteLocker locker(&g_customMenuLock);
			g_customMenuData[menuName.c_str()].menuPath = menuPath;
			g_customMenuData[menuName.c_str()].rootPath = rootPath;
			menuData.Get("menuFlags", &g_customMenuData[menuName.c_str()].menuFlags);
			menuData.Get("movieFlags", &g_customMenuData[menuName.c_str()].movieFlags);
			menuData.Get("extendedFlags", &g_customMenuData[menuName.c_str()].extFlags);
			menuData.Get("depth", &g_customMenuData[menuName.c_str()].depth);
			(*g_ui)->Register(menuName.c_str(), CreateCustomMenu);
			return true;
		}
		
		return false;
	}

	bool IsMenuOpen(StaticFunctionTag *, BSFixedString menuName)
	{
		return (*g_ui)->IsMenuOpen(menuName);
	}

	bool OpenMenu(StaticFunctionTag *, BSFixedString menuName)
	{
		if((*g_ui)->IsMenuRegistered(menuName)) {
			CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Open);
			return true;
		}
		return false;
	}

	bool CloseMenu(StaticFunctionTag *, BSFixedString menuName)
	{
		if((*g_ui)->IsMenuRegistered(menuName)) {
			CALL_MEMBER_FN(*g_uiMessageManager, SendUIMessage)(menuName, kMessage_Close);
			return true;
		}
		return false;
	}

	bool IsMenuRegistered(StaticFunctionTag *, BSFixedString menuName)
	{
		return (*g_ui)->IsMenuRegistered(menuName);
	}

	bool UI_LatentSet(UInt32 stackId, StaticFunctionTag *, BSFixedString menuName, BSFixedString varPath, VMVariable var)
	{
		BSReadLocker locker(g_menuTableLock);
		IMenu * menu = (*g_ui)->GetMenu(menuName);
		if(!menu)
			return false;

		auto movie = menu->movie;
		if(!movie)
			return false;

		auto root = movie->movieRoot;
		if(!root)
			return false;

		GFxValue value;
		PlatformAdapter::ConvertPapyrusValue(&value, &var.GetValue(), root);
		return root->SetVariable(varPath.c_str(), &value);
	}

	VMVariable UI_LatentGet(UInt32 stackId, StaticFunctionTag *, BSFixedString menuName, BSFixedString varPath)
	{
		BSReadLocker locker(g_menuTableLock);
		VMVariable result;
		IMenu * menu = (*g_ui)->GetMenu(menuName);
		if(!menu)
			return result;

		auto movie = menu->movie;
		if(!movie)
			return result;

		auto root = movie->movieRoot;
		if(!root)
			return result;

		GFxValue value;
		root->GetVariable(&value, varPath.c_str());
		PlatformAdapter::ConvertScaleformValue(&result.GetValue(), &value, (*g_gameVM)->m_virtualMachine);
		return result;
	}

	VMVariable UI_LatentInvoke(UInt32 stackId, StaticFunctionTag *, BSFixedString menuName, BSFixedString varPath, VMArray<VMVariable> arguments)
	{
		BSReadLocker locker(g_menuTableLock);
		VMVariable result;
		IMenu * menu = (*g_ui)->GetMenu(menuName);
		if(!menu)
			return result;

		auto movie = menu->movie;
		if(!movie)
			return result;

		auto root = movie->movieRoot;
		if(!root)
			return result;

		UInt32 argCount = arguments.Length();
		GFxValue * args = nullptr;
		if(argCount > 0)
		{
			args = new GFxValue[argCount];
			for(UInt32 i = 0; i < argCount; i++)
			{
				VMVariable var;
				arguments.Get(&var, i);
				PlatformAdapter::ConvertPapyrusValue(&args[i], &var.GetValue(), root);
			}
		}

		GFxValue value;
		root->Invoke(varPath, &value, args, argCount);
		PlatformAdapter::ConvertScaleformValue(&result.GetValue(), &value, (*g_gameVM)->m_virtualMachine);
		if(args)
			delete [] args;
		return result;
	}

	class F4SEScaleform_OnLoadCompleted : public GFxFunctionHandler
	{
	public:
		F4SEScaleform_OnLoadCompleted(UInt64 handle, BSFixedString objectType, BSFixedString callback, BSFixedString menuName, BSFixedString varPath, BSFixedString assetPath) : _handle(handle), _objectType(objectType), _callback(callback), _menuName(menuName), _varPath(varPath), _assetPath(assetPath) { }

		virtual void Invoke(Args * args)
		{
			if(args->numArgs >= 1)
			{
				GFxValue target;
				args->args[0].GetMember("target", &target);

				GFxValue loader;
				target.GetMember("loader", &loader);

				GFxValue content;
				loader.GetMember("content", &content);

				GFxValue contentLoaderInfo;
				loader.GetMember("contentLoaderInfo", &contentLoaderInfo);

				GFxValue loaderURL;
				contentLoaderInfo.GetMember("loaderURL", &loaderURL);

				GFxValue url;
				contentLoaderInfo.GetMember("url", &url);

				GFxValue name;
				content.GetMember("name", &name);

				std::string contentInstance = name.GetString();

				GFxValue parent;
				content.GetMember("parent", &parent);

				while(parent.IsDisplayObject())
				{
					GFxValue name;
					parent.GetMember("name", &name);
					if(!name.IsString())
						break;

					std::string parentInstance = name.GetString();
					contentInstance.insert(0, parentInstance + ".");
					parent.GetMember("parent", &parent);
				}

				bool success = true;
				BSFixedString contentVar(contentInstance.c_str());
				SendPapyrusEvent5<bool, BSFixedString, BSFixedString, BSFixedString, BSFixedString>(_handle, _objectType, _callback, success, _menuName, _varPath, contentVar, _assetPath);
			}
		}
	protected:
		UInt64 _handle;
		BSFixedString _objectType;
		BSFixedString _callback;
		BSFixedString _menuName;
		BSFixedString _varPath;
		BSFixedString _assetPath;
	};

	class F4SEScaleform_OnLoadFailed : public GFxFunctionHandler
	{
	public:
		F4SEScaleform_OnLoadFailed(UInt64 handle, BSFixedString objectType, BSFixedString callback, BSFixedString menuName, BSFixedString varPath, BSFixedString assetPath) : _handle(handle), _objectType(objectType), _callback(callback), _menuName(menuName), _varPath(varPath), _assetPath(assetPath) { }

		virtual void Invoke(Args * args)
		{
			if(args->numArgs >= 1)
			{
				GFxValue target;
				args->args[0].GetMember("target", &target);
			
				// Load failed, remove the loader we created
				GFxValue loader;
				target.GetMember("loader", &loader);

				GFxValue loaderParent;
				loader.GetMember("parent", &loaderParent);

				loaderParent.Invoke("removeChild", nullptr, &loader, 1);

				bool success = false;
				BSFixedString contentVar;
				SendPapyrusEvent5<bool, BSFixedString, BSFixedString, BSFixedString, BSFixedString>(_handle, _objectType, _callback, success, _menuName, _varPath, contentVar, _assetPath);
			}
		}
	protected:
		UInt64 _handle;
		BSFixedString _objectType;
		BSFixedString _callback;
		BSFixedString _menuName;
		BSFixedString _varPath;
		BSFixedString _assetPath;
	};

	bool UI_LatentLoad(UInt32 stackId, StaticFunctionTag *, BSFixedString menuName, BSFixedString varPath, BSFixedString assetPath, VMObject receiver, BSFixedString callback)
	{
		BSReadLocker locker(g_menuTableLock);
		IMenu * menu = (*g_ui)->GetMenu(menuName);
		if(!menu)
			return false;

		auto movie = menu->movie;
		if(!movie)
			return false;

		auto root = movie->movieRoot;
		if(!root)
			return false;

		GFxValue target;
		if(!root->GetVariable(&target, varPath.c_str()))
			return false;

		GFxValue filePath;
		root->CreateString(&filePath, assetPath.c_str());

		// Create the Loader object
		GFxValue loader;
		root->CreateObject(&loader, "flash.display.Loader");

		GFxValue contentLoaderInfo;
		loader.GetMember("contentLoaderInfo", &contentLoaderInfo);

		GFxValue listener[2];

		F4SEScaleform_OnLoadCompleted * onCompleted = new F4SEScaleform_OnLoadCompleted(receiver.GetHandle(), receiver.GetObjectType(), callback, menuName, varPath, assetPath);
		F4SEScaleform_OnLoadFailed * onFailed = new F4SEScaleform_OnLoadFailed(receiver.GetHandle(), receiver.GetObjectType(), callback, menuName, varPath, assetPath);

		root->CreateString(&listener[0], "complete");
		root->CreateFunction(&listener[1], onCompleted);
		contentLoaderInfo.Invoke("addEventListener", nullptr, listener, 2);
		InterlockedDecrement(&onCompleted->refCount); // Let these be garbage collected

		root->CreateString(&listener[0], "ioError");
		root->CreateFunction(&listener[1], onFailed);
		contentLoaderInfo.Invoke("addEventListener", nullptr, listener, 2);
		InterlockedDecrement(&onFailed->refCount);

		// Add the loader to the target
		if(!target.Invoke("addChild", nullptr, &loader, 1))
			return false;

		GFxValue loadArgs[2];
		// Create a load request for the new movie
		root->CreateObject(&loadArgs[0], "flash.net.URLRequest", &filePath, 1);

		loadArgs[1].SetNull();

		return loader.Invoke("load", nullptr, loadArgs, 2);
	}

	DECLARE_DELAY_FUNCTOR(F4SEUISetFunctor, 3, UI_LatentSet, StaticFunctionTag, bool, BSFixedString, BSFixedString, VMVariable);
	DECLARE_DELAY_FUNCTOR(F4SEUIGetFunctor, 2, UI_LatentGet, StaticFunctionTag, VMVariable, BSFixedString, BSFixedString);
	DECLARE_DELAY_FUNCTOR(F4SEUIInvokeFunctor, 3, UI_LatentInvoke, StaticFunctionTag, VMVariable, BSFixedString, BSFixedString, VMArray<VMVariable>);
	DECLARE_DELAY_FUNCTOR(F4SEUILoadFunctor, 5, UI_LatentLoad, StaticFunctionTag, bool, BSFixedString, BSFixedString, BSFixedString, VMObject, BSFixedString);

	// These are the functions that enqueue the Latent functions
	bool Set(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag * tag, BSFixedString menuName, BSFixedString varPath, VMVariable var)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEUISetFunctor(UI_LatentSet, vm, stackId, tag, menuName, varPath, var));
		return true;
	}

	bool Get(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag * tag, BSFixedString menuName, BSFixedString varPath)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEUIGetFunctor(UI_LatentGet, vm, stackId, tag, menuName, varPath));
		return true;
	}

	bool Invoke(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag * tag, BSFixedString menuName, BSFixedString varPath, VMArray<VMVariable> args)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEUIInvokeFunctor(UI_LatentInvoke, vm, stackId, tag, menuName, varPath, args));
		return true;
	}

	bool Load(VirtualMachine * vm, UInt32 stackId, StaticFunctionTag * tag, BSFixedString menuName, BSFixedString varPath, BSFixedString assetPath, VMObject receiver, BSFixedString callback)
	{
		F4SEDelayFunctorManagerInstance().Enqueue(new F4SEUILoadFunctor(UI_LatentLoad, vm, stackId, tag, menuName, varPath, assetPath, receiver, callback));
		return true;
	}
}

void papyrusUI::RegisterFuncs(VirtualMachine* vm)
{
	F4SEObjectRegistry& f4seObjRegistry = F4SEObjectRegistryInstance();
	f4seObjRegistry.RegisterClass<F4SEUISetFunctor>();
	f4seObjRegistry.RegisterClass<F4SEUIGetFunctor>();
	f4seObjRegistry.RegisterClass<F4SEUIInvokeFunctor>();
	f4seObjRegistry.RegisterClass<F4SEUILoadFunctor>();

	vm->RegisterFunction(
		new LatentNativeFunction3 <StaticFunctionTag, bool, BSFixedString, BSFixedString, VMVariable>("Set", "UI", papyrusUI::Set, vm));

	vm->RegisterFunction(
		new LatentNativeFunction2 <StaticFunctionTag, VMVariable, BSFixedString, BSFixedString>("Get", "UI", papyrusUI::Get, vm));

	vm->RegisterFunction(
		new LatentNativeFunction3 <StaticFunctionTag, VMVariable, BSFixedString, BSFixedString, VMArray<VMVariable>>("Invoke", "UI", papyrusUI::Invoke, vm));

	vm->RegisterFunction(
		new LatentNativeFunction5 <StaticFunctionTag, bool, BSFixedString, BSFixedString, BSFixedString, VMObject, BSFixedString>("Load", "UI", papyrusUI::Load, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, BSFixedString>("IsMenuOpen", "UI", papyrusUI::IsMenuOpen, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, BSFixedString>("OpenMenu", "UI", papyrusUI::OpenMenu, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, BSFixedString>("CloseMenu", "UI", papyrusUI::CloseMenu, vm));

	vm->RegisterFunction(
		new NativeFunction1 <StaticFunctionTag, bool, BSFixedString>("IsMenuRegistered", "UI", papyrusUI::IsMenuRegistered, vm));

	vm->RegisterFunction(
		new NativeFunction4 <StaticFunctionTag, bool, BSFixedString, BSFixedString, BSFixedString, MenuData>("RegisterCustomMenu", "UI", papyrusUI::RegisterCustomMenu, vm));

	vm->SetFunctionFlags("UI", "Set", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("UI", "Get", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("UI", "Invoke", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("UI", "Load", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("UI", "IsMenuOpen", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("UI", "IsMenuRegistered", IFunction::kFunctionFlag_NoWait);
	vm->SetFunctionFlags("UI", "RegisterCustomMenu", IFunction::kFunctionFlag_NoWait);
}
