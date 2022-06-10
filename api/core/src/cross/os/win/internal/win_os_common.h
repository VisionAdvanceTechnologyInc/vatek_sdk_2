
#ifndef _CROSS_OS_WIN_TOOL_
#define _CROSS_OS_WIN_TOOL_

#include <cross/cross_os_api.h>
#include <windows.h>
#include <conio.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

	const char* win_get_global_name(const char* tag);
	void win_get_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
