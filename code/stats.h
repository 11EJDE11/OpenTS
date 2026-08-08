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

#pragma once

/*************************************************************
**	Internet specific externs
*/
extern bool	ConnectionLost;
extern void *PacketLater;
extern int WestwoodOnline_PortNumber;

void	Register_Game_Start_Time(void);
void	Register_Game_End_Time(void);
void	Send_Statistics_Packet(void);
