#include "f4se/Hooks_Threads.h"
#include "common/ICriticalSection.h"
#include "f4se/GameThreads.h"
#include "f4se_common/Relocation.h"
#include "f4se_common/BranchTrampoline.h"
#include "xbyak/xbyak.h"

#include <queue>

ICriticalSection		s_taskQueueLock;
std::queue<ITaskDelegate*>	s_tasks;
std::vector<ITaskDelegate*>	s_tasksPermanent;

ICriticalSection		s_uiQueueLock;
std::queue<ITaskDelegate*>	s_uiQueue;

typedef bool (* _MessageQueueProcessTask)(void * messageQueue, float timeout, UInt32 unk1);
RelocAddr <_MessageQueueProcessTask> MessageQueueProcessTask(0x00C3FC30);
_MessageQueueProcessTask MessageQueueProcessTask_Original = nullptr;

RelocAddr <uintptr_t> ProcessEventQueue_HookTarget(0x01A7A8A0 + 0xFAC);
typedef void (* _ProcessEventQueue_Internal)(void * unk1);
RelocAddr <_ProcessEventQueue_Internal> ProcessEventQueue_Internal(0x01B1DFA0);

bool MessageQueueProcessTask_Hook(void * messageQueue, float timeout, UInt32 unk1)
{
	bool result = MessageQueueProcessTask_Original(messageQueue, timeout, unk1);

	s_taskQueueLock.Enter();
	
	for (auto it = s_tasksPermanent.begin(); it != s_tasksPermanent.end(); it++) {
		(*it)->Run();
	}
	
	while (!s_tasks.empty())
	{
		ITaskDelegate * cmd = s_tasks.front();
		s_tasks.pop();
		cmd->Run();
		delete cmd;
	}
	s_taskQueueLock.Leave();

	return result;
}

void TaskInterface::AddTask(ITaskDelegate * task)
{
	s_taskQueueLock.Enter();
	s_tasks.push(task);
	s_taskQueueLock.Leave();
}

void TaskInterface::AddTaskPermanent(ITaskDelegate* task)
{
	s_taskQueueLock.Enter();
	s_tasksPermanent.push_back(task);
	s_taskQueueLock.Leave();
}

void ProcessEventQueue_Hook(void * unk1)
{
	s_uiQueueLock.Enter();
	while (!s_uiQueue.empty())
	{
		ITaskDelegate * cmd = s_uiQueue.front();
		s_uiQueue.pop();
		cmd->Run();
		delete cmd;
	}
	s_uiQueueLock.Leave();

	ProcessEventQueue_Internal(unk1);
}

void TaskInterface::AddUITask(ITaskDelegate * task)
{
	s_uiQueueLock.Enter();
	s_uiQueue.push(task);
	s_uiQueueLock.Leave();
}

void Hooks_Threads_Init(void)
{
	
}

void Hooks_Threads_Commit(void)
{
	// hook message queue to pump our own messages
	{
		struct MessageQueueProcessTask_Code : Xbyak::CodeGenerator {
			MessageQueueProcessTask_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;

				mov(rax, rsp);
				mov(ptr[rax + 8], rbx);

				jmp(ptr [rip + retnLabel]);

				L(retnLabel);
				dq(MessageQueueProcessTask.GetUIntPtr() + 7);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		MessageQueueProcessTask_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		MessageQueueProcessTask_Original = (_MessageQueueProcessTask)codeBuf;

		g_branchTrampoline.Write6Branch(MessageQueueProcessTask.GetUIntPtr(), (uintptr_t)MessageQueueProcessTask_Hook);
	}

	g_branchTrampoline.Write5Call(ProcessEventQueue_HookTarget.GetUIntPtr(), (uintptr_t)ProcessEventQueue_Hook);
}
