#include "f4se/GameAPI.h"
#include <cstdio>

// 
RelocPtr <Heap> g_mainHeap(0x03E48060);

void * Heap_Allocate(size_t size)
{
	return CALL_MEMBER_FN(g_mainHeap, Allocate)(size, 0, false);
}

void Heap_Free(void * ptr)
{
	CALL_MEMBER_FN(g_mainHeap, Free)(ptr, false);
}

// 
RelocPtr <ConsoleManager *> g_console(0x030DC038);

// 
RelocAddr <UInt32 *> g_consoleHandle(0x032F1720);

void Console_Print(const char * fmt, ...)
{
	ConsoleManager * mgr = *g_console;
	if(mgr)
	{
		va_list args;
		va_start(args, fmt);

		char buf[4096];

		int len = vsprintf_s(buf, sizeof(buf), fmt, args);
		if(len > 0)
		{
			// add newline and terminator, truncate if not enough room
			if(len > (sizeof(buf) - 2))
				len = sizeof(buf) - 2;

			buf[len] = '\n';
			buf[len + 1] = 0;

			CALL_MEMBER_FN(mgr, Print)(buf);
		}

		va_end(args);
	}
}

LONGLONG GetPerfCounter(void)
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}
