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

#include "phone.h"

class PhoneEdit
{
		friend BOOL CALLBACK Phone_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	public:
		PhoneEdit(void) {}
		bool Dialog(void);

	private:
		bool Dial(void);
		void Build_Phone_Listbox(void);
		void Select(void);
		void Add(void);
		void Edit(void);
		void Delete(void);

	private:
		/*
		 * This is the window handle of the phone directory dialog while it is up. Every
		 * control the dialog owns is fetched through it.
		 */
		HWND _Dialog;

		/*
		 * This is the result the dialog will report, and it stays negative for as long as
		 * the dialog is still running. The message pump spins until a button sets it, so
		 * it serves as the signal to take the dialog down as well as the answer.
		 */
		int RC;

		/*
		 * If the phone book has been altered while this dialog was up, then this flag will
		 * be true. The multiplayer settings are only written back out when it is, so
		 * merely looking at the directory costs nothing.
		 */
		bool Changed;

		/*
		 * This is the number about to be dialed, as it was typed into the edit box. It is
		 * compared against the selected listing so that a number nobody has stored yet can
		 * be recognized and given a phone book entry of its own.
		 */
		char Number[PhoneEntryClass::PHONE_MAX_NUM];
};
