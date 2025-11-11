#include "Hooks_Scaleform.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"

#include "f4se_common/f4se_version.h"
#include "common/IDirectoryIterator.h"
#include "f4se/PluginManager.h"

#include "Translation.h"

#include "ScaleformMovie.h"
#include "ScaleformValue.h"
#include "ScaleformCallbacks.h"
#include "ScaleformState.h"
#include "ScaleformLoader.h"
#include "ScaleformTranslator.h"

#include "PapyrusEvents.h"
#include "PapyrusScaleformAdapter.h"
#include "GameInput.h"
#include "NiTextures.h"

#include "CustomMenu.h"

#include <unordered_set>

class BSScaleformManager;

typedef BSScaleformManager * (* _BSScaleformManager_Ctor)(BSScaleformManager * mem);
RelocAddr <_BSScaleformManager_Ctor> BSScaleformManager_Ctor(0x01B14B20);
_BSScaleformManager_Ctor BSScaleformManager_Ctor_Original = nullptr;

typedef UInt32 (* _BSScaleformTint)(BSGFxShaderFXTarget * value, float * colors, float multiplier);
RelocAddr <_BSScaleformTint> BSScaleformTint(0x01AFF400);
_BSScaleformTint BSScaleformTint_Original = nullptr;

RelocAddr <uintptr_t> ScaleformInitHook_Start(0x01B15330 + 0x226);

RelocAddr <uintptr_t> IMenuCreateHook_Start(0x01A7A8A0 + 0xA0F);

// 
RelocAddr <uintptr_t> SetMenuName(0x016769F0);

//// plugin API
struct ScaleformPluginInfo
{
	const char	* name;
	F4SEScaleformInterface::RegisterCallback	callback;
};

typedef std::list <ScaleformPluginInfo> PluginList;
static PluginList s_plugins;

bool g_logScaleform = false;

static std::unordered_map<std::string, std::unordered_set<NiTexture*>> s_mountedTextures;
static BSReadWriteLock s_mountedTexturesLock;

bool RegisterScaleformPlugin(const char * name, F4SEScaleformInterface::RegisterCallback callback)
{
	// check for a name collision
	for(PluginList::iterator iter = s_plugins.begin(); iter != s_plugins.end(); ++iter)
	{
		if(!strcmp(iter->name, name))
		{
			_WARNING("scaleform plugin name collision: %s", iter->name);
			return false;
		}
	}

	ScaleformPluginInfo	info;

	info.name = name;
	info.callback = callback;

	s_plugins.push_back(info);

	return true;
}

void Hooks_Scaleform_Init()
{
	//
}

class F4SEScaleform_VisitMembers : public GFxValue::ObjectInterface::ObjVisitor
{
public:
	F4SEScaleform_VisitMembers(GFxMovieRoot * root, GFxValue * result) : m_root(root), m_result(result) { }
	virtual void Visit(const char * member, GFxValue * value) override
	{
		GFxValue str;
		m_root->CreateString(&str, member);
		m_result->PushBack(&str);
	}

	GFxMovieRoot * m_root;
	GFxValue * m_result;
};

class F4SEScaleform_GetMembers : public GFxFunctionHandler
{
public:
	virtual void	Invoke(Args* args)
	{
		if(args->numArgs >= 1)
		{
			args->movie->movieRoot->CreateArray(args->result);
			F4SEScaleform_VisitMembers dm(args->movie->movieRoot, args->result);
			args->args[0].VisitMembers(&dm);
		}
	}
};

class F4SEScaleform_AllowTextInput : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 1);
		ASSERT(args->args[0].GetType() == GFxValue::kType_Bool);

		(*g_inputMgr)->AllowTextInput(args->args[0].GetBool());
	}
};

class F4SEScaleform_SendExternalEvent : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 1);
		ASSERT(args->args[0].GetType() == GFxValue::kType_String);

		BSFixedString eventName(args->args[0].GetString());
		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;

		g_externalEventRegs.ForEach(
			eventName, 
			[&args, &vm](const EventRegistration<ExternalEventParameters> & reg)
			{
				VMValue receiver;
				if(GetIdentifier(&receiver, reg.handle, &reg.scriptName, vm)) {
					VMValue packedArgs;
					UInt32 length = args->numArgs - 1;
					VMValue::ArrayData * arrayData = nullptr;

					vm->CreateArray(&packedArgs, length, &arrayData);

					packedArgs.type.value = VMValue::kType_VariableArray;
					packedArgs.data.arr = arrayData;

					for(UInt32 i = 0; i < length; i++)
					{
						VMValue * var = new VMValue;
						PlatformAdapter::ConvertScaleformValue(var, &args->args[i + 1], vm);
						arrayData->arr.entries[i].SetVariable(var);
					}

					if(receiver.IsIdentifier()) {
						VMIdentifier * identifier = receiver.data.id;
						if(identifier) {
							CallFunctionNoWait_Internal(vm, 0, identifier, &reg.params.callbackName, &packedArgs);
						}
					}
				}
			}
		);
	}
};

class F4SEScaleform_CallFunctionNoWait : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 2);
		ASSERT(args->args[0].GetType() == GFxValue::kType_Object);
		ASSERT(args->args[1].GetType() == GFxValue::kType_String);

		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;
		VMValue receiver;
		PlatformAdapter::ConvertScaleformValue(&receiver, &args->args[0], vm);
		BSFixedString functionName(args->args[1].GetString());

		args->result->SetBool(false);
		if(receiver.IsIdentifier())
		{
			VMValue packedArgs;
			UInt32 length = args->numArgs - 2;
			VMValue::ArrayData * arrayData = nullptr;
			vm->CreateArray(&packedArgs, length, &arrayData);

			packedArgs.type.value = VMValue::kType_VariableArray;
			packedArgs.data.arr = arrayData;

			for(UInt32 i = 0; i < length; i++)
			{
				VMValue * var = new VMValue;
				PlatformAdapter::ConvertScaleformValue(var, &args->args[i + 2], vm);
				arrayData->arr.entries[i].SetVariable(var);
			}

			VMIdentifier * identifier = receiver.data.id;
			if(identifier) {
				CallFunctionNoWait_Internal(vm, 0, identifier, &functionName, &packedArgs);
				args->result->SetBool(true);
			}
		}
	}
};

class F4SEScaleform_CallGlobalFunctionNoWait : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 2);
		ASSERT(args->args[0].GetType() == GFxValue::kType_String);
		ASSERT(args->args[1].GetType() == GFxValue::kType_String);

		VirtualMachine * vm = (*g_gameVM)->m_virtualMachine;

		BSFixedString className(args->args[0].GetString());
		BSFixedString functionName(args->args[1].GetString());

		args->result->SetBool(false);

		VMValue packedArgs;
		UInt32 length = args->numArgs - 2;
		VMValue::ArrayData * arrayData = nullptr;
		vm->CreateArray(&packedArgs, length, &arrayData);

		packedArgs.type.value = VMValue::kType_VariableArray;
		packedArgs.data.arr = arrayData;

		for (UInt32 i = 0; i < length; i++)
		{
			VMValue * var = new VMValue;
			PlatformAdapter::ConvertScaleformValue(var, &args->args[i + 2], vm);
			arrayData->arr.entries[i].SetVariable(var);
		}

		CallGlobalFunctionNoWait_Internal(vm, 0, 0, &className, &functionName, &packedArgs);

		args->result->SetBool(true);
	}
};

class F4SEScaleform_GetDirectoryListing : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 1);
		ASSERT(args->args[0].GetType() == GFxValue::kType_String);

		const char * directory = args->args[0].GetString();
		const char * match = nullptr;
		if(args->numArgs >= 2)
			match = args->args[1].GetString();

		args->movie->movieRoot->CreateArray(args->result);

		for(IDirectoryIterator iter(directory, match); !iter.Done(); iter.Next())
		{
			std::string	path = iter.GetFullPath();
			WIN32_FIND_DATA * fileData = iter.Get();

			GFxValue fileInfo;
			args->movie->movieRoot->CreateObject(&fileInfo);

			GFxValue filePath;
			args->movie->movieRoot->CreateString(&filePath, path.c_str());
			fileInfo.SetMember("nativePath", &filePath);

			GFxValue fileName;
			args->movie->movieRoot->CreateString(&fileName, fileData->cFileName);
			fileInfo.SetMember("name", &filePath);

			SYSTEMTIME sysTime;
			FileTimeToSystemTime(&fileData->ftLastWriteTime, &sysTime);

			GFxValue date;
			GFxValue params[7];
			params[0].SetNumber(sysTime.wYear);
			params[1].SetNumber(sysTime.wMonth - 1); // Flash Month is 0-11, System time is 1-12
			params[2].SetNumber(sysTime.wDay);
			params[3].SetNumber(sysTime.wHour);
			params[4].SetNumber(sysTime.wMinute);
			params[5].SetNumber(sysTime.wSecond);
			params[6].SetNumber(sysTime.wMilliseconds);
			args->movie->movieRoot->CreateObject(&date, "Date", params, 7);
			fileInfo.SetMember("lastModified", &date);

			FileTimeToSystemTime(&fileData->ftCreationTime, &sysTime);
			params[0].SetNumber(sysTime.wYear);
			params[1].SetNumber(sysTime.wMonth - 1); // Flash Month is 0-11, System time is 1-12
			params[2].SetNumber(sysTime.wDay);
			params[3].SetNumber(sysTime.wHour);
			params[4].SetNumber(sysTime.wMinute);
			params[5].SetNumber(sysTime.wSecond);
			params[6].SetNumber(sysTime.wMilliseconds);
			args->movie->movieRoot->CreateObject(&date, "Date", params, 7);
			fileInfo.SetMember("creationDate", &date);

			fileInfo.SetMember("isDirectory", &GFxValue((fileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY));
			fileInfo.SetMember("isHidden", &GFxValue((fileData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == FILE_ATTRIBUTE_HIDDEN));
			args->result->PushBack(&fileInfo);
		}
	}
};

class F4SEScaleform_MountImage : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 3);
		ASSERT(args->args[0].GetType() == GFxValue::kType_String);
		ASSERT(args->args[1].GetType() == GFxValue::kType_String);
		ASSERT(args->args[2].GetType() == GFxValue::kType_String);

		const char * menuName = args->args[0].GetString();
		const char * filePath = args->args[1].GetString();
		const char * imageName = args->args[2].GetString();

		NiTexture * texture = nullptr;
		LoadTextureByPath(filePath, true, texture, 0, 0, 0);

		bool result = false;
		if(texture)
		{
			BSWriteLocker locker(&s_mountedTexturesLock);

			auto & textures = s_mountedTextures[menuName];
			auto sit = textures.find(texture);
			if(sit != textures.end())
			{
				// Already mounted
				result = true;
			}
			else
			{
				auto imageLoader = (*g_scaleformManager)->imageLoader;
				if(imageLoader)
				{
					texture->name = imageName;
					texture->IncRef();
					textures.insert(texture);
					result = imageLoader->MountImage(&texture);
				}
			}

			// LoadTextureByPath increases refcount
			texture->DecRef();
		}

		args->result->SetBool(result);
	}
};

class F4SEScaleform_UnmountImage : public GFxFunctionHandler
{
public:
	virtual void Invoke(Args * args)
	{
		ASSERT(args->numArgs >= 2);
		ASSERT(args->args[0].GetType() == GFxValue::kType_String);
		ASSERT(args->args[1].GetType() == GFxValue::kType_String);

		const char * menuName = args->args[0].GetString();
		const char * filePath = args->args[1].GetString();

		NiTexture * texture = nullptr;
		LoadTextureByPath(filePath, true, texture, 0, 0, 0);

		bool result = false;
		if(texture)
		{
			BSWriteLocker locker(&s_mountedTexturesLock);
			auto it = s_mountedTextures.find(menuName);
			if(it != s_mountedTextures.end())
			{
				auto sit = it->second.find(texture);
				if(sit != it->second.end())
				{
					auto imageLoader = (*g_scaleformManager)->imageLoader;
					if(imageLoader)
					{
						result = imageLoader->UnmountImage(&texture);
						it->second.erase(texture);
						texture->DecRef();
					}
				}

				if(it->second.empty())
				{
					s_mountedTextures.erase(it);
				}
			}

			// LoadTextureByPath increases refcount
			texture->DecRef();
		}

		args->result->SetBool(result);
	}
};

class F4SEOpenCloseHandler : public BSTEventSink<MenuOpenCloseEvent>
{
public:
	virtual ~F4SEOpenCloseHandler() { };
	virtual	EventResult	ReceiveEvent(MenuOpenCloseEvent * evn, void * dispatcher) override
	{
		// Unmount textures if the menu is being destroyed
		if(!evn->isOpen)
		{
			BSWriteLocker locker(&s_mountedTexturesLock);
			auto it = s_mountedTextures.find(evn->menuName.c_str());
			if(it != s_mountedTextures.end())
			{
				auto imageLoader = (*g_scaleformManager)->imageLoader;
				if(imageLoader)
				{
					for(auto & texture : it->second)
					{
						imageLoader->UnmountImage(&const_cast<NiTexture*>(texture));
						texture->DecRef();
					}
				}
				
				it->second.clear();
				s_mountedTextures.erase(it);
			}
		}

		return kEvent_Continue;
	};
};

F4SEOpenCloseHandler g_menuOpenCloseHandler;

void ScaleformInitHook_Install(GFxMovieView * view)
{
	GFxValue		root;
	GFxMovieRoot	* movieRoot = view->movieRoot;

	bool result = movieRoot->GetVariable(&root, "root");
	if(!result)
	{
		_ERROR("couldn't get root");
		return;
	}

	GFxValue	f4se;
	movieRoot->CreateObject(&f4se);

	RegisterFunction<F4SEScaleform_GetMembers>(&f4se, movieRoot, "GetMembers");
	RegisterFunction<F4SEScaleform_AllowTextInput>(&f4se, movieRoot, "AllowTextInput");
	RegisterFunction<F4SEScaleform_SendExternalEvent>(&f4se, movieRoot, "SendExternalEvent");
	RegisterFunction<F4SEScaleform_CallFunctionNoWait>(&f4se, movieRoot, "CallFunctionNoWait");
	RegisterFunction<F4SEScaleform_CallGlobalFunctionNoWait>(&f4se, movieRoot, "CallGlobalFunctionNoWait");
	RegisterFunction<F4SEScaleform_GetDirectoryListing>(&f4se, movieRoot, "GetDirectoryListing");
	RegisterFunction<F4SEScaleform_MountImage>(&f4se, movieRoot, "MountImage");
	RegisterFunction<F4SEScaleform_UnmountImage>(&f4se, movieRoot, "UnmountImage");
	
	GFxValue	version;
	movieRoot->CreateObject(&version);
	version.SetMember("major", &GFxValue(F4SE_VERSION_INTEGER));
	version.SetMember("minor", &GFxValue(F4SE_VERSION_INTEGER_MINOR));
	version.SetMember("beta", &GFxValue(F4SE_VERSION_INTEGER_BETA));
	version.SetMember("releaseIdx", &GFxValue(F4SE_VERSION_RELEASEIDX));
	f4se.SetMember("version", &version);

	// plugins
	GFxValue	plugins;
	movieRoot->CreateObject(&plugins);

	for(PluginList::iterator iter = s_plugins.begin(); iter != s_plugins.end(); ++iter)
	{
		GFxValue	plugin;
		movieRoot->CreateObject(&plugin);

		iter->callback(view, &plugin);

		plugins.SetMember(iter->name, &plugin);
	}

	f4se.SetMember("plugins", &plugins);

	root.SetMember("f4se", &f4se);

	if(root.IsObject())
	{
		GFxValue funcObj;
		if(root.HasMember("onF4SEObjCreated") && root.GetMember("onF4SEObjCreated", &funcObj) && funcObj.IsFunction())
		{
			root.Invoke("onF4SEObjCreated", nullptr, &f4se, 1);
		}

		GFxValue child;
		GFxValue numChildren;
		if(root.GetMember("numChildren", &numChildren))
		{
			for(SInt32 i = 0; i < numChildren.GetInt(); ++i)
			{
				GFxValue index(i);
				root.Invoke("getChildAt", &child, &index, 1);

				if(child.HasMember("onF4SEObjCreated") && child.GetMember("onF4SEObjCreated", &funcObj) && funcObj.IsFunction())
				{
					child.Invoke("onF4SEObjCreated", nullptr, &f4se, 1);
				}
			}
		}
	}
	
	GFxValue dispatchEvent;
	GFxValue eventArgs[3];
	movieRoot->CreateString(&eventArgs[0], "F4SE::Initialized");
	eventArgs[1] = GFxValue(true);
	eventArgs[2] = GFxValue(false);
	movieRoot->CreateObject(&dispatchEvent, "flash.events.Event", eventArgs, 3);
	movieRoot->Invoke("root.dispatchEvent", nullptr, &dispatchEvent, 1);
}

BSScaleformManager * BSScaleformManager_Ctor_Hook(BSScaleformManager * mgr)
{
	BSScaleformManager * result = BSScaleformManager_Ctor_Original(mgr);

	BSScaleformTranslator * translator = (BSScaleformTranslator*)result->stateBag->GetStateAddRef(GFxState::kInterface_Translator);
	if(translator) {
		Translation::ImportTranslationFiles(translator);
	}

	if(g_logScaleform)
	{
		GFxLogState * logger = (GFxLogState*)result->stateBag->GetStateAddRef(GFxState::kInterface_Log);
		logger->logger = new F4SEGFxLogger();
	}

	if(*g_ui)
	{
		(*g_ui)->menuOpenCloseEventSource.AddEventSink(&g_menuOpenCloseHandler);
	}

	return result;
}

UInt32 BSScaleformTint_Hook(BSGFxShaderFXTarget * value, float * colors, float multiplier)
{
	if(value->HasMember("onApplyColorChange"))
	{
		GFxValue result;
		GFxValue args[4];
		args[0] = GFxValue(colors[0]);
		args[1] = GFxValue(colors[1]);
		args[2] = GFxValue(colors[2]); 
		args[3] = GFxValue(multiplier);
		value->Invoke("onApplyColorChange", &result, args, 4);
		if(result.IsArray() && result.GetArraySize() >= 4)
		{
			result.GetElement(0, &args[0]);
			result.GetElement(1, &args[1]);
			result.GetElement(2, &args[2]);
			result.GetElement(3, &args[3]);

			colors[0] = args[0].GetNumber();
			colors[1] = args[1].GetNumber();
			colors[2] = args[2].GetNumber();
			multiplier = args[3].GetNumber();
		}
	}
	
	return BSScaleformTint_Original(value, colors, multiplier);
}

void Hooks_Scaleform_Commit()
{
	UInt32	logScaleform = 0;
	if(GetConfigOption_UInt32("Interface", "bEnableGFXLog", &logScaleform))
	{
		if(logScaleform)
		{
			g_logScaleform = true;
		}
	}

	// hook creation of each menu
	{
		struct ScaleformInitHook_Code : Xbyak::CodeGenerator {
			ScaleformInitHook_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				mov(eax, 1);
				lock(); xadd(ptr [rcx + 8], eax);
				jmp((void *)&ScaleformInitHook_Install);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		ScaleformInitHook_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		g_branchTrampoline.Write5Call(ScaleformInitHook_Start, uintptr_t(code.getCode()));
	}

	// hook creation of BSScaleformManager
	{
		struct BSScaleformManager_Code : Xbyak::CodeGenerator {
			BSScaleformManager_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				push(rbp);
				push(rbx);
				push(rsi);
				push(rdi);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(BSScaleformManager_Ctor.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		BSScaleformManager_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		BSScaleformManager_Ctor_Original = (_BSScaleformManager_Ctor)codeBuf;

		g_branchTrampoline.Write5Branch(BSScaleformManager_Ctor.GetUIntPtr(), (uintptr_t)BSScaleformManager_Ctor_Hook);
	}

	// hook global tinting of objects
	{
		struct BSScaleformTint_Code : Xbyak::CodeGenerator {
			BSScaleformTint_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(ptr[rsp+0x18], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(BSScaleformTint.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		BSScaleformTint_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		BSScaleformTint_Original = (_BSScaleformTint)codeBuf;

		g_branchTrampoline.Write5Branch(BSScaleformTint.GetUIntPtr(), (uintptr_t)BSScaleformTint_Hook);
	}

	// Hook menu construction
	{
		struct MenuConstruction_Code : Xbyak::CodeGenerator {
			MenuConstruction_Code(void * buf, uintptr_t originFuncAddr, uintptr_t funcAddr) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel, funcLabel1, funcLabel2;

				// Put the original call back
				call(ptr [rip + funcLabel1]);

				// Pull the IMenu off the stack and call our new function
				mov(rcx, ptr[rsp + 0x40]);
				call(ptr [rip + funcLabel2]);

				// Jump back to the original location
				jmp(ptr [rip + retnLabel]);

				L(funcLabel1);
				dq(originFuncAddr);

				L(funcLabel2);
				dq(funcAddr);

				L(retnLabel);
				dq(IMenuCreateHook_Start.GetUIntPtr() + 0x5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		MenuConstruction_Code code(codeBuf, SetMenuName.GetUIntPtr(), (uintptr_t)LoadCustomMenu_Hook);
		g_localTrampoline.EndAlloc(code.getCurr());
		g_branchTrampoline.Write5Branch(IMenuCreateHook_Start.GetUIntPtr(), uintptr_t(code.getCode()));
	}
}
