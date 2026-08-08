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

/* $Header: /counterstrike/INTERNET.CPP 6     3/17/97 1:05a Steve_tall $ */
/*************************************************************************************
 **   C O N F I D E N T I A L --- W E S T W O O D    S T U D I O S                  **
 *************************************************************************************
 *                                                                                   *
 *                 Project Name : Command & Conquer - Red Alert                      *
 *                                                                                   *
 *                    File Name : INTERNET.CPP                                       *
 *                                                                                   *
 *                   Programmer : Steve Tall                                         *
 *                                                                                   *
 *                   Start Date : March 11th, 1996                                   *
 *                                                                                   *
 *                  Last Update : August 5th, 1996 [ST]                              *
 *                                                                                   *
 *-----------------------------------------------------------------------------------*
 * Overview:                                                                         *
 *                                                                                   *
 *  Miscellaneous junk related to H2H internet connection.                           *
 *                                                                                   *
 *-----------------------------------------------------------------------------------*
 * Functions:                                                                        *
 *  Check_From_WChat -- Interprets start game packet from WChat                      *
 *  Read_Game_Options -- Read the game setup options from the wchat packet           *
 *  Is_User_WChat_Registered -- retrieve the users wchat entry from registry         *
 *  Spawn_WChat -- spawns or switches focus to wchat                                 *
 *  Spawn_Registration_App -- spawns the C&C/Planet westwood registration app        *
 *  Do_The_Internet_Menu_Thang -- Handle case where user clicks on 'Internet' button *
 *                                                                                   *
 *                                                                                   *
 *                                                                                   *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"


/***************************************************************************
**	Internet specific globals
*/
bool	GameStatisticsPacketSent;							//Flag that game stats have been sent to wchat
