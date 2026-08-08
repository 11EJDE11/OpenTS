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

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                     $Archive:: /Commando/Code/wwlib/Except.cpp                             $*
 *                                                                                             *
 *                      $Author:: Steve_t                                                     $*
 *                                                                                             *
 *                     $Modtime:: 2/07/02 12:28p                                              $*
 *                                                                                             *
 *                    $Revision:: 14                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *                                                                                             *
 * Exception_Proc -- Windows dialog callback for the exception dialog                          *
 * Exception_Dialog -- Brings up the exception options dialog.                                 *
 * Add_Txt -- Add the given text to the machine state dump buffer.                             *
 * Dump_Exception_Info -- Dump machine state information into a buffer                         *
 * Exception_Handler -- Exception handler filter function                                      *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "_surface.h"
#include "assert.h"
#include "cdcntrl.h"
#include "data.h"
#include "dbgprint.h"
#include "getcpu.h"
#include "globals.h"
#include "language\language.h"
#include "ownrdraw.h"
#include "surface.h"
#include "version.h"
#include "win.h"
#include "windlg.h"
#include "winfix.h"
#include "winstub.h"

#include <conio.h>
#include <imagehlp.h>


#define EH_BUFFER_SIZE 65536
#define NUM_CODE_BYTES 32
#define MAX_STACK_DUMP 1024

/*
**	Buffer to dump machine state information to. We don't want to allocate this at run-time
**	in case the exception was caused by a malfunction in the memory system.
*/
static char ExceptionText [EH_BUFFER_SIZE];

/*
**	Register dump variables. These are used to allow the game to restart from an arbitrary
**	position after an exception occurs.
*/
unsigned int ExceptionReturnStack = 0;
unsigned int ExceptionReturnAddress = 0;
unsigned int ExceptionReturnFrame = 0;

/*
**	Number of times the exception handler has recursed. Recursions are bad.
*/
int ExceptionRecursions = -1;

bool UnusedEHBool = true;

/*
**	Definitions to allow run-time linking to the Imagehlp.dll functions.
**
*/

/// Declaring the entry points as separate statics leaves the pointer walk below dependent on
/// how the compiler lays them out. The struct in the other arm does not.
#if 0
typedef BOOL  (WINAPI *SymCleanupType) (HANDLE hProcess);
typedef BOOL  (WINAPI *SymGetSymFromAddrType) (HANDLE hProcess, DWORD Address, LPDWORD Displacement, PIMAGEHLP_SYMBOL Symbol);
typedef BOOL  (WINAPI *SymInitializeType) (HANDLE hProcess, LPSTR UserSearchPath, BOOL fInvadeProcess);
typedef BOOL  (WINAPI *SymLoadModuleType) (HANDLE hProcess, HANDLE hFile, LPSTR ImageName, LPSTR ModuleName, DWORD BaseOfDll, DWORD SizeOfDll);
typedef DWORD (WINAPI *SymSetOptionsType) (DWORD SymOptions);
typedef BOOL  (WINAPI *SymUnloadModuleType) (HANDLE hProcess, DWORD BaseOfDll);
typedef BOOL  (WINAPI *StackWalkType) (DWORD MachineType, HANDLE hProcess, HANDLE hThread, LPSTACKFRAME StackFrame, LPVOID ContextRecord, PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine, PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine, PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine, PTRANSLATE_ADDRESS_ROUTINE TranslateAddress);
typedef LPVOID (WINAPI *SymFunctionTableAccessType) (HANDLE hProcess, DWORD AddrBase);
typedef DWORD (WINAPI *SymGetModuleBaseType) (HANDLE hProcess, DWORD dwAddr);


static SymCleanupType							_SymCleanup = NULL;
static SymGetSymFromAddrType				_SymGetSymFromAddr = NULL;
static SymInitializeType						_SymInitialize = NULL;
static SymLoadModuleType						_SymLoadModule = NULL;
static SymSetOptionsType						_SymSetOptions = NULL;
static SymUnloadModuleType					_SymUnloadModule = NULL;
static StackWalkType								_StackWalk = NULL;
static SymFunctionTableAccessType	_SymFunctionTableAccess = NULL;
static SymGetModuleBaseType				_SymGetModuleBase = NULL;

#else

struct tImageHelpStruct {
	BOOL  (WINAPI *SymCleanup)(HANDLE hProcess);
	BOOL  (WINAPI *SymGetSymFromAddr)(HANDLE hProcess, DWORD dwAddr, PDWORD pdwdisplacementacement, PIMAGEHLP_SYMBOL Symbol);
	BOOL  (WINAPI *SymInitialize)(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess);
	DWORD (WINAPI *SymLoadModule)(HANDLE hProcess, HANDLE hFile, PCSTR ImageName, PCSTR ModuleName, DWORD BaseOfDll, DWORD SizeOfDll);
	DWORD (WINAPI *SymSetOptions)(DWORD SymOptions);
	BOOL  (WINAPI *SymUnloadModule)(HANDLE hProcess, DWORD BaseOfDll);
} ImageHelp = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
#endif

static char const *ImagehelpFunctionNames[] =
{
	"SymCleanup",
	"SymGetSymFromAddr",
	"SymInitialize",
	"SymLoadModule",
	"SymSetOptions",
	"SymUnloadModule",
	NULL,
};


/// <summary>
/// Handles the messages for the exception report dialog.
/// This routine is the dialog procedure used by Exception_Dialog. It fills the detail
/// field with the machine state text the exception handler gathered, and centers the
/// window over the main game window so the player cannot miss it.
/// </summary>
int CALLBACK Exception_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message) {
		case WM_INITDIALOG:
			DebugString("WM_INITDIALOG\n");

			SetDlgItemText(window, IDC_EXCEPT_DETAILS, ExceptionText);
			SetFocus(window);
			if (MainWindow) {
				Center_Window_Within_Window(window, MainWindow);
			}
			ShowWindow(window, SW_NORMAL);
			break;

		case WM_CLOSE:
			EndDialog(window, IDC_EXCEPT_QUIT);
			break;

		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				case IDC_EXCEPT_SAVE:
					MessageBox(window, "Sorry, you can only attempt an emergency save if you were in the map editor when the exception occurred.\t", "Bummer", MB_ICONWARNING);
					break;

				case IDC_EXCEPT_QUIT:
					EndDialog(window, IDC_EXCEPT_QUIT);
					break;
			};
			break;

		case WM_MOVING:
			return(On_WM_MOVING(window, wparam, lparam));

		default:
			break;
	}

	return(0);
}


/// <summary>
/// Presents the exception report to the player.
/// This routine is called by the exception handler once the machine state has been
/// gathered. A recursive exception gets a progressively cruder response -- a console
/// notice instead of a dialog, and finally an outright exit -- so that a fault raised
/// inside the reporting code cannot trap the player. The CD trays are released on
/// every path out.
/// </summary>
/// <returns>Returns with the dialog control that dismissed the report. IDC_EXCEPT_QUIT
/// means the handler should take the exception rather than pass it along.</returns>
int Exception_Dialog(void)
{
	int retval = 0;

	switch (ExceptionRecursions) {

		default: {
			ShowCursor(TRUE);

			DLGTEMPLATE const * dialogt = (DLGTEMPLATE const *)Fetch_Resource(MAKEINTRESOURCE(IDD_EXCEPTION_SIMPLE), MAKEINTRESOURCE(5));
			if (dialogt != NULL) {
				DebugString("Resource found\n");
				retval = DialogBoxIndirectParam(ProgramInstance, dialogt, MainWindow, Exception_Proc, 0);
			} else {
				DebugString("Resource not found\n");
				retval = DialogBoxParam(ProgramInstance, MAKEINTRESOURCE(IDD_EXCEPTION_SIMPLE), MainWindow, Exception_Proc, 0);
			}
			CDControl.Unlock_All_CD_Trays();
			ShowCursor(FALSE);
			break;
		}

		case 3:
			CDControl.Unlock_All_CD_Trays();
			ExitProcess(EXIT_SUCCESS);
			retval = IDC_EXCEPT_QUIT;
			break;

		case 2:
			CDControl.Unlock_All_CD_Trays();
			retval = IDC_EXCEPT_QUIT;
			break;

		case 1:
			CDControl.Unlock_All_CD_Trays();
			if (!AllocConsole()) {
				FreeConsole();
				AllocConsole();
			}
			cprintf("Error - Recursive exception detected. Program halted.\r\n See EXCEPT.TXT file for details\r\r\n");
			Sleep(4000);
			getch();
			FreeConsole();
			retval = IDC_EXCEPT_QUIT;
			break;

	}

	return(retval);
}


/// <summary>
/// Are any of the drawing surfaces currently locked?
/// This routine is used by the exception handler to decide whether it dares put a window
/// on the screen. A locked surface holds the Win16 mutex under Windows 9x, and the crash
/// report dialog would hang the machine outright rather than report anything.
/// </summary>
/// <returns>bool; Is at least one of the game surfaces locked?</returns>
static bool Any_Locked(void)
{
	if (VisibleSurface && VisibleSurface->Is_Locked() ||
		HiddenSurface && HiddenSurface->Is_Locked() ||
		CompositeSurface && CompositeSurface->Is_Locked() ||
		TileSurface && TileSurface->Is_Locked() ||
		SidebarSurface && SidebarSurface->Is_Locked() ||
		AlternateSurface && AlternateSurface->Is_Locked()) {
		return(true);
	}

	return(false);
}


/***********************************************************************************************
 * Add_Txt -- Add the given text to the machine state dump buffer.                             *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    Text                                                                              *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    7/22/97 12:21PM ST : Created                                                             *
 *=============================================================================================*/
static void Add_Txt (char const *txt)
{
	if (strlen(ExceptionText) + strlen(txt) < EH_BUFFER_SIZE - 1) {
		strcat(ExceptionText, txt);
	}
#if (0)
	/*
	**	Log to debug output too.
	*/
	static char _debug_output_txt[512];
	const char *in = txt;
	char *out = _debug_output_txt;
	bool done = false;

	if (strlen(txt) < sizeof(_debug_output_txt)) {
		for (int i=0 ; i<sizeof(_debug_output_txt) ; i++) {

			switch (*in) {
				case '\r':
					in++;
					continue;

				case 0:
					done = true;
					// fall through

				default:
					*out++ = *in++;
					break;
			}

			if (done) {
				break;
			}
		}

		DebugString(_debug_output_txt);
	}
#endif //(0)
}


/***********************************************************************************************
 * Dump_Exception_Info -- Dump machine state information into a buffer                         *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    ptr to exception information                                                      *
 *                                                                                             *
 * OUTPUT:   Nothing                                                                           *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    7/22/97 12:21PM ST : Created                                                             *
 *=============================================================================================*/
void Dump_Exception_Info(EXCEPTION_POINTERS *e_info)
{
	/*
	**	List of possible exceptions
	*/
	static const unsigned int _codes[] = {
		EXCEPTION_ACCESS_VIOLATION,
		EXCEPTION_ARRAY_BOUNDS_EXCEEDED,
		EXCEPTION_BREAKPOINT,
		EXCEPTION_DATATYPE_MISALIGNMENT,
		EXCEPTION_FLT_DENORMAL_OPERAND,
		EXCEPTION_FLT_DIVIDE_BY_ZERO,
		EXCEPTION_FLT_INEXACT_RESULT,
		EXCEPTION_FLT_INVALID_OPERATION,
		EXCEPTION_FLT_OVERFLOW,
		EXCEPTION_FLT_STACK_CHECK,
		EXCEPTION_FLT_UNDERFLOW,
		EXCEPTION_ILLEGAL_INSTRUCTION,
		EXCEPTION_IN_PAGE_ERROR,
		EXCEPTION_INT_DIVIDE_BY_ZERO,
		EXCEPTION_INT_OVERFLOW,
		EXCEPTION_INVALID_DISPOSITION,
		EXCEPTION_NONCONTINUABLE_EXCEPTION,
		EXCEPTION_PRIV_INSTRUCTION,
		EXCEPTION_SINGLE_STEP,
		EXCEPTION_STACK_OVERFLOW,
		0xffffffff
	};

	/*
	**	Information about each exception type.
	*/
	static char const * _code_txt[] = {
		"Error code: EXCEPTION_ACCESS_VIOLATION\r\r\nDescription: The thread tried to read from or write to a virtual address for which it does not have the appropriate access.",
		"Error code: EXCEPTION_ARRAY_BOUNDS_EXCEEDED\r\r\nDescription: The thread tried to access an array element that is out of bounds and the underlying hardware supports bounds checking.",
		"Error code: EXCEPTION_BREAKPOINT\r\r\nDescription: A breakpoint was encountered.",
		"Error code: EXCEPTION_DATATYPE_MISALIGNMENT\r\r\nDescription: The thread tried to read or write data that is misaligned on hardware that does not provide alignment. For example, 16-bit values must be aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and so on.",
		"Error code: EXCEPTION_FLT_DENORMAL_OPERAND\r\r\nDescription: One of the operands in a floating-point operation is denormal. A denormal value is one that is too small to represent as a standard floating-point value.",
		"Error code: EXCEPTION_FLT_DIVIDE_BY_ZERO\r\r\nDescription: The thread tried to divide a floating-point value by a floating-point divisor of zero.",
		"Error code: EXCEPTION_FLT_INEXACT_RESULT\r\r\nDescription: The result of a floating-point operation cannot be represented exactly as a decimal fraction.",
		"Error code: EXCEPTION_FLT_INVALID_OPERATION\r\r\nDescription: Some strange unknown floating point operation was attempted.",
		"Error code: EXCEPTION_FLT_OVERFLOW\r\r\nDescription: The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.",
		"Error code: EXCEPTION_FLT_STACK_CHECK\r\r\nDescription: The stack overflowed or underflowed as the result of a floating-point operation.",
		"Error code: EXCEPTION_FLT_UNDERFLOW\r\r\nDescription:\tThe exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.",
		"Error code: EXCEPTION_ILLEGAL_INSTRUCTION\r\r\nDescription:\tThe thread tried to execute an invalid instruction.",
		"Error code: EXCEPTION_IN_PAGE_ERROR\r\r\nDescription:\tThe thread tried to access a page that was not present, and the system was unable to load the page. For example, this exception might occur if a network connection is lost while running a program over the network.",
		"Error code: EXCEPTION_INT_DIVIDE_BY_ZERO\r\r\nDescription: The thread tried to divide an integer value by an integer divisor of zero.",
		"Error code: EXCEPTION_INT_OVERFLOW\r\r\nDescription: The result of an integer operation caused a carry out of the most significant bit of the result.",
		"Error code: EXCEPTION_INVALID_DISPOSITION\r\r\nDescription: An exception handler returned an invalid disposition to the exception dispatcher. Programmers using a high-level language such as C should never encounter this exception.",
		"Error code: EXCEPTION_NONCONTINUABLE_EXCEPTION\r\r\nDescription: The thread tried to continue execution after a noncontinuable exception occurred.",
		"Error code: EXCEPTION_PRIV_INSTRUCTION\r\r\nDescription: The thread tried to execute an instruction whose operation is not allowed in the current machine mode.",
		"Error code: EXCEPTION_SINGLE_STEP\r\r\nDescription: A trace trap or other single-instruction mechanism signaled that one instruction has been executed.",
		"Error code: EXCEPTION_STACK_OVERFLOW\r\r\nDescription: The thread used up its stack.",
		"Error code: ?????\r\r\nDescription: Unknown exception."
	};

	DebugString("Dump exception info\n");

	/*
	**	Scrap buffer for constructing dump strings
	*/
	char scrap [256];

	/*
	**	Clear out the dump buffer
	*/
	memset(ExceptionText, 0, sizeof (ExceptionText));

	/*
	**	If this is the first time through then fix up the imagehelp function pointers since imagehlp.dll
	**	can't be statically linked.
	*/
	HINSTANCE imagehelp = LoadLibrary("IMAGEHLP.DLL");

	if (imagehelp != NULL) {
		HINSTANCE handle = GetModuleHandle("IMAGEHLP.DLL");
		if (handle != NULL) {
			DebugString ("Found IMAGEHLP.DLL - linking to required functions\n");
			char const *function_name = NULL;
			unsigned int *fptr = (unsigned int*)&ImageHelp; /// The struct guarantees the layout this walk depends on.
			int count = 0;

			do {
				function_name = ImagehelpFunctionNames[count];
				if (function_name) {
					*fptr = (unsigned int) GetProcAddress(handle, function_name);
					fptr++;
					count++;
				}
			} while (function_name);
		} else {
			DebugString("Unable to locate IMAGEHLP.DLL\n");
		}
	} else {
		DebugString("Unable to load IMAGEHLP.DLL\n");
	}

	/*
	**	Retrieve the programs symbols if they are available
	*/
	if (ImageHelp.SymSetOptions != NULL) {
		ImageHelp.SymSetOptions(SYMOPT_DEFERRED_LOADS);
	}

	int symload = 0;
	int symbols_available = false;

	if (ImageHelp.SymInitialize != NULL && ImageHelp.SymInitialize(GetCurrentProcess(), NULL, false)) {
		//DebugString("Symbols are available\n");
		symbols_available = true;
	}

	if (!symbols_available)	{
		DebugString ("SymInitialize failed with code %d - %s\n", GetLastError(), Last_Error_Text());
	} else {
		if (ImageHelp.SymSetOptions != NULL) {
			ImageHelp.SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
		}

		if (ImageHelp.SymLoadModule != NULL) {
			symload = ImageHelp.SymLoadModule(GetCurrentProcess(), NULL, ".\\sun.exe", "Sun", 0, 0);
		}

		if (!symload) {
			assert(ImageHelp.SymLoadModule != NULL);
			DebugString ("SymLoad failed with code %d - %s\n", GetLastError(), Last_Error_Text());
		}
	}

	/// Deriving from the record rather than casting a raw byte array gives the symbol its
	/// proper type and alignment while keeping the whole record within 256 bytes.
	struct : IMAGEHLP_SYMBOL {
		BYTE buffer[(256 - sizeof(IMAGEHLP_SYMBOL))];
	} symbol;
	DWORD displacement;
	IMAGEHLP_SYMBOL *symptr = (IMAGEHLP_SYMBOL*)&symbol;

	/*
	**	Get the exception address and the machine context at the time of the exception
	*/
	CONTEXT *context = e_info->ContextRecord;

	/*
	**	The following are set for access violation only
	*/
	int access_read_write=-1;
	unsigned int access_address = 0;

	if (e_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		DebugString("Exception is access violation\n");
		access_read_write = e_info->ExceptionRecord->ExceptionInformation[0];  // 0=read, 1=write
		access_address = e_info->ExceptionRecord->ExceptionInformation[1];
	} else {
		DebugString ("Exception code is %d\n", e_info->ExceptionRecord->ExceptionCode);
	}

	/*
	**	Match the exception type with the error string and print it out
	*/
	int i;
	for (i=0 ; _codes[i] != 0xffffffff ; i++) {
		if (_codes[i] == e_info->ExceptionRecord->ExceptionCode) {
			DebugString("Found exception description\n");
			break;
		}
	}
	Add_Txt(_code_txt[i]);
	Add_Txt("\r\n");

	/*
	**	For access violations, print out the violation address and if it was read or write.
	*/
	if (e_info->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
		sprintf(scrap, "Access address:%08X ", access_address);
		Add_Txt(scrap);
		if (access_read_write) {
			Add_Txt("was written to.\r\n");
		} else {
			Add_Txt("was read from.\r\n");
		}
	}


	/*
	**	If symbols are available, print out the exception eip address and the name of the
	**	function it represents.
	*/
	memset(symptr, 0, sizeof (IMAGEHLP_SYMBOL));
	symptr->SizeOfStruct = sizeof (IMAGEHLP_SYMBOL);
	symptr->MaxNameLength = sizeof (symbol.buffer);
	symptr->Size = 0;
	symptr->Address = context->Eip;

	if (!IsBadCodePtr((FARPROC)context->Eip)) {
		if (ImageHelp.SymGetSymFromAddr != NULL && ImageHelp.SymGetSymFromAddr(GetCurrentProcess(), context->Eip, &displacement, &symbol)) {
			sprintf(scrap, "Exception occurred at %08X - %s + %08X\r\n", context->Eip, symptr->Name, displacement);
		} else {
			//DebugString ("Failed to get symbol for EIP\n");
			if (ImageHelp.SymGetSymFromAddr != NULL) {
				DebugString("SymGetSymFromAddr failed with code %d - %s\n", GetLastError(), Last_Error_Text());
			}
			sprintf (scrap, "Exception occurred at %08X\r\n", context->Eip);
		}
	} else {
		DebugString ("context->Eip is bad code pointer\n");
	}

	Add_Txt (scrap);

	/*
	**	Add in the version info.
	*/
	sprintf(scrap, "\r\nVersion %s\r\n", Version_Name());
	Add_Txt(scrap);

	sprintf(scrap, "Internal Version %s\r\n", VerNum.Version_Name());
	Add_Txt(scrap);

	char buildinfo[128];
	buildinfo[0] = 0;
	Build_Date_String(buildinfo, sizeof(buildinfo));

	char build_number[128];
	char build_name[128];

#ifdef _DEBUG
	sprintf(scrap, "Debug Build: %s by %s - %s\r\n",
#else
	sprintf(scrap, "Release Build: %s by %s - %s\r\n",
#endif
		Build_Number_String(build_number, sizeof(build_number)),
		Build_By_String(build_name, sizeof(build_name)),
		buildinfo);

	Add_Txt(scrap);

	/*
	**	CPU type
	*/
	int cpu_type = 5;
	bool mmx = false;
	Get_CPU_Type(cpu_type, mmx, NULL, 0);
	sprintf(scrap, "CPU %01d86, MMX %s, Vendor: %s\r\n", cpu_type, mmx ? "Yes" : "No", &VendorID);
	Add_Txt(scrap);


	Add_Txt("\r\nDetails:\r\n");

	DebugString("Register dump...\n");

	/*
	**	Dump the registers.
	*/
	sprintf(scrap, "Eip:%08X\tEsp:%08X\tEbp:%08X\r\n", context->Eip, context->Esp, context->Ebp);
	Add_Txt(scrap);
	sprintf(scrap, "Eax:%08X\tEbx:%08X\tEcx:%08X\r\n", context->Eax, context->Ebx, context->Ecx);
	Add_Txt(scrap);
	sprintf(scrap, "Edx:%08X\tEsi:%08X\tEdi:%08X\r\n", context->Edx, context->Esi, context->Edi);
	Add_Txt(scrap);
	sprintf(scrap, "EFlags:%08X \r\n", context->EFlags);
	Add_Txt(scrap);
	sprintf(scrap, "CS:%04x  SS:%04x  DS:%04x  ES:%04x  FS:%04x  GS:%04x\r\n", context->SegCs, context->SegSs, context->SegDs, context->SegEs, context->SegFs, context->SegGs);
	Add_Txt(scrap);

	/*
	**	Dump the bytes at EIP. This will make it easier to match the crash address with later versions of the game.
	*/
	DebugString("EIP bytes dump...\n");
	sprintf(scrap, "\r\nBytes at CS:EIP (%08X)  : ", context->Eip);

	unsigned char *eip_ptr = (unsigned char *) (context->Eip);
	char bytestr[NUM_CODE_BYTES];

	for (int c = 0 ; c < NUM_CODE_BYTES; c++) {
		if (IsBadReadPtr(eip_ptr, sizeof(*eip_ptr))) {
			strcat(scrap, "?? ");
		} else {
			sprintf(bytestr, "%02X ", *eip_ptr);
			strcat(scrap, bytestr);
		}
		eip_ptr++;
	}

	strcat(scrap, "\r\n\r\n");
	Add_Txt(scrap);

	/*
	**	Dump out the values on the stack.
	*/
	DebugString("Stack dump...\n");
	Add_Txt("Stack dump (* indicates possible code address) :\r\n");
	unsigned int *stackptr = (unsigned int*) context->Esp;

	for (int j = 0; j < MAX_STACK_DUMP; j++) {
		if (IsBadReadPtr(stackptr, sizeof(*stackptr))) {
			/*
			**	The stack contents cannot be read so just print up question marks.
			*/
			sprintf(scrap, "%08X: ", stackptr);
			strcat(scrap, "????????\r\n");
		} else {
			/*
			**	If this stack address is in our memory space then try to match it with a code symbol.
			*/
			sprintf(scrap, "%08X: %08X", stackptr, *stackptr);
			if (IsBadCodePtr((FARPROC)*stackptr)) {
				//strcat(scrap, "DATA_PTR\r\n");
			} else {
				if (symbols_available) {
					symptr->SizeOfStruct = sizeof(symbol);
					symptr->MaxNameLength = 128;
					symptr->Size = 0;
					symptr->Address = *stackptr;

					if (ImageHelp.SymGetSymFromAddr != NULL && ImageHelp.SymGetSymFromAddr(GetCurrentProcess(), *stackptr, &displacement, symptr)) {
						char symbuf[160];
						sprintf(symbuf, " - %s + %08X", symptr->Name, displacement);
						strcat(scrap, symbuf);
					}
				} else {
					strcat (scrap, " *");
				}
			}
			strcat(scrap, "\r\n");
		}
		Add_Txt(scrap);
		stackptr++;
	}

	/*
	**	Unload the symbols.
	*/
	if (symbols_available) {
		if (ImageHelp.SymCleanup != NULL) {
			ImageHelp.SymCleanup(GetCurrentProcess());
		}

		if (symload) {
			if (ImageHelp.SymUnloadModule != NULL) {
				ImageHelp.SymUnloadModule(GetCurrentProcess(), 0);
			}
		}
	}

	if (imagehelp != NULL) {
		FreeLibrary(imagehelp);
	}

	Add_Txt ("\r\n\r\n");
}


/***********************************************************************************************
 * Exception_Handler -- Exception handler filter function                                      *
 *                                                                                             *
 *                                                                                             *
 *                                                                                             *
 * INPUT:    exception code                                                                    *
 *           pointer to exception information pointers                                         *
 *                                                                                             *
 * OUTPUT:   EXCEPTION_EXECUTE_HANDLER -- Excecute the body of the __except construct          *
 *        or EXCEPTION_CONTINUE_SEARCH -- Pass this exception down to the debugger             *
 *        or EXCEPTION_CONTINUE_EXECUTION -- Continue to execute at the fault address          *
 *                                                                                             *
 * WARNINGS: None                                                                              *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *    7/22/97 12:29PM ST : Created                                                             *
 *=============================================================================================*/
int Exception_Handler(int exception_code, EXCEPTION_POINTERS *e_info)
{
	DebugString("Exception!\n");

	/*
	**	Track recursions because we need to know if something here is failing.
	*/
	ExceptionRecursions++;

	/*
	**	If there was a breakpoint then chances are it was set by a debugger. In _DEBUG mode
	**	we probably should ignore breakpoints. Breakpoints become more significant in release
	**	mode since there probably isn't a debugger present.
	*/
#ifdef _DEBUG
	if (exception_code == EXCEPTION_BREAKPOINT) {
		return(EXCEPTION_CONTINUE_SEARCH);
	}
#else
	exception_code = exception_code;
#endif	//_DEBUG

#ifdef WWDEBUG
	//CONTEXT *context;
#endif WWDEBUG

	if (ExceptionRecursions == 0) {

		/*
		**	Create a dump of the exception info.
		*/
		Dump_Exception_Info(e_info);

		/*
		**	Log the machine state to disk
		*/
		HANDLE debug_file;
		DWORD	actual;
		debug_file = CreateFile("except.txt", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (debug_file != INVALID_HANDLE_VALUE){
			WriteFile(debug_file, ExceptionText, strlen(ExceptionText), &actual, NULL);
			CloseHandle (debug_file);
		}
	}

	if (Any_Locked() && WinVersion.Is_Win9x() || GetCurrentThreadId() != (DWORD)MainThread) {
		DebugString("Can't bring up exception dialog due to Win16 mutex or threading issues\n");
		CDControl.Unlock_All_CD_Trays();
		return(EXCEPTION_CONTINUE_SEARCH);
	}

	DebugString("Exception_Dialog()\n");
	if (Exception_Dialog() != IDC_EXCEPT_QUIT) {
		return(EXCEPTION_CONTINUE_SEARCH);
	}

	UnusedEHBool = false;

	if (ExceptionRecursions == 0) {
		ExceptionRecursions = -1;
	} else if (ExceptionRecursions == 2) {
		return(EXCEPTION_CONTINUE_SEARCH);
	}
	if (g_TopWindow) {
		OwnerDraw::End_Dialog(g_TopWindow);
	}

	CDControl.Unlock_All_CD_Trays();

	return(EXCEPTION_EXECUTE_HANDLER);
}
