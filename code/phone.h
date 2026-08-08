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

/* $Header:   F:\projects\c&c\vcs\code\phone.h_v   1.9   16 Oct 1995 16:47:58   JOE_BOSTIC  $ */
/***************************************************************************
 *                                                                         *
 *                 Project Name : Command & Conquer                        *
 *                                                                         *
 *                    File Name : PHONE.H                                  *
 *                                                                         *
 *                   Programmer : Bill R. Randolph                         *
 *                                                                         *
 *                   Start Date : 04/28/95                                 *
 *                                                                         *
 *                  Last Update : April 28, 1995 [BRR]                     *
 *                                                                         *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#include "session.h"

/*
***************************** Class Declaration *****************************
*/
class PhoneEntryClass
{
	public:
		enum PhoneEntryEnum {
			PHONE_MAX_NAME = 21,
			PHONE_MAX_NUM = 21
		};

		PhoneEntryClass(void) {};
		~PhoneEntryClass(void) {};

		bool operator == (PhoneEntryClass &obj)
			{ return(memcmp (Name,obj.Name,strlen(Name))==0);}
		bool operator != (PhoneEntryClass &obj)
			{ return(memcmp (Name,obj.Name,strlen(Name))!=0);}
		bool operator > (PhoneEntryClass &obj)
			{ return(memcmp(Name, obj.Name, strlen(Name)) > 0);}
		bool operator < (PhoneEntryClass &obj)
			{ return(memcmp(Name, obj.Name, strlen(Name)) < 0);}
		bool operator >= (PhoneEntryClass &obj)
			{ return(memcmp(Name, obj.Name, strlen(Name)) >= 0);}
		bool operator <= (PhoneEntryClass &obj)
			{ return(memcmp(Name, obj.Name, strlen(Name)) <= 0);}

		SerialSettingsType Settings;
		char Name[ PHONE_MAX_NAME ];		// destination person's name
		char Number[ PHONE_MAX_NUM ];		// phone #
};
