#pragma once

class ITaskDelegate;

void Hooks_Threads_Init(void);
void Hooks_Threads_Commit(void);

namespace TaskInterface
{
	void AddTask(ITaskDelegate * task);
	void AddUITask(ITaskDelegate * task);
}
