/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2025 Electronic Arts Inc.
 * Copyright 2026 OpenTS contributors
 *
 * Contains material derived from Electronic Arts source code.
 * Modified by OpenTS contributors, 2026.
 * EA's GPLv3 Section 7 additional terms and supplemental warranty
 * disclaimers apply; see LICENSE.md.
 ******************************************************************************/

#include "always.h"

#include "dbgprint.h"

#include "_mono.h"
#include "misc.h"
#include "mono.h"
#include "msgbox.h"
#include "win.h"
#include "globals.h"
#include "data.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#ifdef _DEBUG

#define CONSOLE_WINDOW_NAME		"Debug Console"

HANDLE DebugConsole = INVALID_HANDLE_VALUE;
HANDLE DebugFile = INVALID_HANDLE_VALUE;
char DebugFileName[256];

/***********************************************************************************************
 * WWDebugString -- sends a string to the debugger and echos it to disk                        *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    string                                                                            *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    10/28/96 12:48PM ST : Created                                                            *
 *=============================================================================================*/
void __cdecl DebugString (const char *string, ...)
{
	static char buffer[1024];
	static char filebuf[1024];
	static char path_to_exe[512];
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];

	va_list	va;

	va_start(va, string);
	vsprintf(buffer, string, va);
	va_end(va);

	DWORD	actual;
	if (DebugFile == INVALID_HANDLE_VALUE){
		GetModuleFileName (GetModuleHandle(NULL), &path_to_exe[0], 512);
		_splitpath(path_to_exe, drive, dir, NULL, NULL);
		_makepath(DebugFileName, drive, dir, "DEBUG", "TXT");
		DebugFile = CreateFile(DebugFileName, GENERIC_WRITE, 0,
												NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}else{
		DebugFile = CreateFile(DebugFileName, GENERIC_WRITE, 0,
												NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (DebugFile != INVALID_HANDLE_VALUE){
		SetFilePointer (DebugFile, 0, NULL, FILE_END);
		WriteFile(DebugFile, buffer, strlen(buffer)/* +1 // Removed as +1 writes NUL */, &actual, NULL);
		CloseHandle (DebugFile);
	}

	OutputDebugString (buffer);

	/// Write the string to the console screen buffer.
	if (Debug_Console) {
		DWORD	actual;
		if (DebugConsole == INVALID_HANDLE_VALUE){
			AllocConsole();
			SetConsoleTitle(CONSOLE_WINDOW_NAME);
			DebugConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		}else{
			DebugFile = CreateFile(DebugFileName, GENERIC_WRITE, 0,
													NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		if (DebugConsole != INVALID_HANDLE_VALUE){
			WriteConsole(DebugConsole, buffer, lstrlen(buffer), &actual, NULL);
		}
	}
}

/// <summary>
/// Reports a formatted message with no identifying prefix, so the text appears exactly as
/// given. Callers use it to continue a line another call began, and for text such as the
/// startup banner that reads better unstamped. It reaches the same places DebugString does.
/// </summary>
/// <param name="string">The printf style format string to report.</param>
void __cdecl DebugStringNoPrefix(const char *string, ...)
{
	static char buffer[1024];
	static char filebuf[1024];
	static char path_to_exe[512];
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];

	va_list	va;

	va_start(va, string);
	vsprintf(buffer, string, va);
	va_end(va);

	DWORD	actual;
	if (DebugFile == INVALID_HANDLE_VALUE){
		GetModuleFileName (GetModuleHandle(NULL), &path_to_exe[0], 512);
		_splitpath(path_to_exe, drive, dir, NULL, NULL);
		_makepath(DebugFileName, drive, dir, "DEBUG", "TXT");
		DebugFile = CreateFile(DebugFileName, GENERIC_WRITE, 0,
												NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}else{
		DebugFile = CreateFile(DebugFileName, GENERIC_WRITE, 0,
												NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	if (DebugFile != INVALID_HANDLE_VALUE){
		SetFilePointer (DebugFile, 0, NULL, FILE_END);
		WriteFile(DebugFile, buffer, strlen(buffer)/* +1 // Removed as +1 writes NUL */, &actual, NULL);
		CloseHandle (DebugFile);
	}

	OutputDebugString (buffer);
}

/***********************************************************************************************
 * Last_Error_Text -- Get the system error text for GetLastError                               *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Nothing                                                                           *
 *                                                                                             *
 * OUTPUT:   Ptr to error string                                                               *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   8/14/98 11:11AM ST : Created                                                              *
 *=============================================================================================*/
char const * Last_Error_Text(void)
{

	static char message_buffer[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), &message_buffer[0], 256, NULL);
	return(message_buffer);
}
#endif


#ifdef _DEBUG_ASSERT
/***********************************************************************************************
 * Assert_Failure -- display the line and source file where a failed assert occurred           *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    line number in source file                                                        *
 *           name of source file                                                               *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    4/17/96 9:58AM ST : Created                                                              *
 *=============================================================================================*/
void Assert_Failure (char const *expression, int line, char const *file)
{
	char	assertbuf[256];
	char   timebuff[512];
	SYSTEMTIME	time;

	sprintf (assertbuf, "assert '%s' failed at line %d in module %s.\n", expression, line, file);

	if (!MonoClass::Is_Enabled()) MonoClass::Enable();

	Mono_Clear_Screen();
	Mono_Printf("%s", assertbuf);

	DebugString(assertbuf);

	GetLocalTime(&time);

	sprintf (timebuff, "%02d/%02d/%04d %02d:%02d:%02d - %s", time.wMonth, time.wDay, time.wYear,
																		time.wHour, time.wMinute, time.wSecond,
																		assertbuf);


	HMMIO handle = mmioOpen((char *)"ASSERT.TXT", NULL, MMIO_WRITE);
	if (!handle) {
		handle = mmioOpen((char *)"ASSERT.TXT", NULL, MMIO_CREATE | MMIO_WRITE);
		//mmioClose(handle, 0);
		//handle = mmioOpen("ASSERT.TXT", NULL, MMIO_WRITE);
	}

	if (handle) {

		mmioWrite(handle, timebuff, strlen(timebuff));
		mmioClose(handle, 0);
	}

	WWMessageBox().Process(assertbuf, TXT_OK);
//	WWMessageBox().Process("Red Alert demo timed out - Aborting");
	//Get_Key();

	Prog_End(/*assertbuf, false*/);
//	Invalidate_Cached_Icons();
	//PostQuitMessage( 0 );
	//ExitProcess(0);
}
#endif
