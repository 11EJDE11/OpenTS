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
 *                     $Archive:: /Commando/Code/Tools/pluglib/jshell.cpp                     $*
 *                                                                                             *
 *                      $Author:: Greg_h                                                      $*
 *                                                                                             *
 *                     $Modtime:: 11/07/00 2:32p                                              $*
 *                                                                                             *
 *                    $Revision:: 28                                                          $*
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   Fatal -- General purpose fatal error handler.                                             *
 *   Set_Bit -- Set bit in a bit array.                                                        *
 *   Get_Bit -- Fetch the bit value from a bit array.                                          *
 *   First_True_Bit -- Return with the first true bit index.                                   *
 *   First_False_Bit -- Find the first false bit in the bit array.                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "_mono.h"
#include "conquer.h"
#include "mono.h"

#include <cstdio>
#include <cstdlib>

/***********************************************************************************************
 * Fatal -- General purpose fatal error handler.                                               *
 *                                                                                             *
 *    This is a very simple general purpose fatal error handler. It goes directly to text      *
 *    mode, prints the error, and then aborts with a failure code.                             *
 *                                                                                             *
 * INPUT:   message  -- The text message to display.                                           *
 *                                                                                             *
 *          ...      -- Any optional parameters that are used in formatting the message.       *
 *                                                                                             *
 * OUTPUT:  none                                                                               *
 *                                                                                             *
 * WARNINGS:   This routine never returns. The game exits immediately.                         *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   10/17/1994 JLB : Created.                                                                 *
 *=============================================================================================*/
void __cdecl Fatal(char const * message, ...)
{
	va_list	va;

	va_start(va, message);
	Emergency_Exit();
	vfprintf(stderr, message, va);
	Mono_Printf(message);
	exit(EXIT_FAILURE);
}
