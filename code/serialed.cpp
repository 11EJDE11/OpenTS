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

#include "serialed.h"

#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "modemreg.h"
#include "msgbox.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "trim.h"
#include "winfix.h"

static char const *baudname[] = {
	"14400",
	"19200",
	"28800",
	"38400",
	"57600",
	"115200",
};

BOOL CALLBACK Com_Settings_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);


/// <summary>
/// Lets the player set up the serial port.
/// This routine puts up the settings dialog for a modem or null-modem game, where the
/// port, IRQ and baud rate are chosen and the modem initialization strings are edited.
/// </summary>
/// <param name="settings">The serial settings to edit, updated in place when the
/// player accepts.</param>
/// <returns>bool; Did the player accept the settings?</returns>
bool SerialEdit::Dialog(SerialSettingsType *settings)
{
	RC = -1;
	_Dialog = OwnerDraw::Begin_Dialog(IDD_SERIAL_SETTINGS, (DLGPROC)Com_Settings_Dialog_Proc);
	if (_Dialog) {

		SetWindowLong(_Dialog, DWL_USER, (LONG)this);

		Init(settings);

		OwnerDraw::Display_Dialog(_Dialog);

		while (RC < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(_Dialog);

		/*------------------------------------------------------------------------
		Save values into the Settings structure
		------------------------------------------------------------------------*/
		if (RC == 1) {
			memcpy(settings, &TempSettings, sizeof(SerialSettingsType));
		}
	}
	if (RC != 1) {
		return(false);
	}
	return(true);
}


/// <summary>
/// Handles a serial settings dialog message that the owner draw system passed up.
/// This routine deals with the control notifications by passing each of them along to the
/// SerialEdit object that the dialog was created for.
/// </summary>
/// <returns>Returns with zero, the dialog procedure's "message handled" result.</returns>
BOOL SerialEdit::Handle_Message(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	HWND handle;
	int i;
	SerialEdit * _this = (SerialEdit *)GetWindowLong(window, DWL_USER);

	switch (message) {
		case WM_INITDIALOG:
			handle = GetDlgItem(window, IDC_SERIAL_INITSTRING);
			if (handle != 0) {
				Edit_LimitText(handle, INITSTRBUF_MAX-1);
			}
			handle = GetDlgItem(window, IDC_SERIAL_PORT_EDIT);
			if (handle != 0) {
				Edit_LimitText(handle, PORTBUF_MAX-1-1);
			}
			handle = GetDlgItem(window, IDC_SERIAL_CALLWAITING_EDIT);
			if (handle != 0) {
				Edit_LimitText(handle, CWAITSTRBUF_MAX-1);
			}
			return(FALSE);

		case WM_COMMAND:
			{
				switch (LOWORD(wparam)) {
					case IDC_SERIAL_PULSE:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
							SendDlgItemMessage(window, IDC_SERIAL_TONE, BM_SETCHECK, 0, 0);
						}
						break;

					case IDC_SERIAL_DATA_COMPRESSION:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
						}
						break;

					case IDOK:
						if (HIWORD(wparam) == 0 && _this->Save() == true) {
							_this->RC = IDOK;
						}
						break;

					case IDCANCEL:
						if (HIWORD(wparam) == 0) {
							_this->RC = IDCANCEL;
						}
						break;

					case IDC_SERIAL_ERROR_CORRECTION:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
						}
						break;

					case IDC_SERIAL_PORT:
						if (HIWORD(wparam) == 1) {
							i = ComboBox_GetCurSel((HWND)lparam);
							if (ComboBox_GetItemData((HWND)lparam, i) == 4) {
								ComboBox_GetLBText((HWND)lparam, i, _this->TempBuf);
								_this->Set_String(IDC_SERIAL_PORT_EDIT);
							} else {
								SetFocus(0);
							}
						}
						break;

					case IDC_SERIAL_CALLWAITING:
						if (HIWORD(wparam) == 1) {
							i = ComboBox_GetCurSel((HWND)lparam);
							if (ComboBox_GetItemData((HWND)lparam, i) == 3) {
								ComboBox_GetLBText((HWND)lparam, i, _this->TempBuf);
								_this->Set_String(IDC_SERIAL_CALLWAITING_EDIT);
							} else {
								SetFocus(0);
							}
						}
						break;

					case IDC_SERIAL_CALLWAITING_EDIT:
						if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_KILLFOCUS) {
							_this->Set_Init_String();
						}
						break;

					case IDC_SERIAL_PORT_EDIT:
						if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_KILLFOCUS) {
							_this->Set_Port();
						}
						break;

					case IDC_SERIAL_INITLIST:
						if (HIWORD(wparam) == LBN_DBLCLK) {
							if (ListBox_GetCount((HWND)lparam) > 0) {
								i = ListBox_GetCurSel((HWND)lparam);
								if (i != LB_ERR) {
									ListBox_GetText((HWND)lparam, i, _this->TempBuf);
									handle = GetDlgItem(window, IDC_SERIAL_INITSTRING);
									if (handle != 0) {
										Edit_SetText(handle, _this->TempBuf);
										SetFocus(handle);
									}
								}
							}
						}
						break;

					case IDC_SERIAL_INITSTRING:
						if (HIWORD(wparam) == EN_MAXTEXT) {
							_this->Add_Init_String();
						}
						break;

					case IDC_SERIAL_ADD:
						if (HIWORD(wparam) == 0) {
							_this->Add_Init_String();
						}
						break;

					case IDC_SERIAL_INIT_DELETE:
						if (HIWORD(wparam) == 0) {
							_this->Delete_Init_String();
						}
						break;

					case IDC_SERIAL_TONE:
						if (HIWORD(wparam) != 0) {
							Button_SetCheck((HWND)lparam, TRUE);
							SendDlgItemMessage(window, IDC_SERIAL_PULSE, BM_SETCHECK, 0, 0);
						}
						break;
				}
			}
			break;
	}

	return(0);
}


/// <summary>
/// Handles the messages for the serial settings dialog.
/// This routine gives the owner draw system first refusal on every message, and passes
/// whatever is left over up to the handler that works the dialog's own controls.
/// </summary>
/// <returns>Returns with the owner draw handler's result, or zero when this routine dealt
/// with the message itself.</returns>
BOOL CALLBACK Com_Settings_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc;

	rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc != 0) {
		return(rc);
	}

	return(SerialEdit::Handle_Message(window, message, wparam, lparam));
}


/// <summary>
/// Reads the serial settings dialog back into the working settings.
/// This routine is called when the user presses OK. It gathers the port, baud rate, init
/// string, call waiting and dialing choices from the controls, and then has the null modem
/// manager confirm that the chosen port can actually be opened.
/// </summary>
/// <returns>bool; Are the chosen settings usable? A message box explains the trouble to
/// the user if they are not.</returns>
bool SerialEdit::Save(void)
{
	char * temp;
	int index;
	int initstr_index;  /// index of currently-selected modem init
	int cwaitstr_index; /// index of currently-selected call waiting
	int port_index;     /// index of currently-selected port
	DetectPortType dpstatus;

	DebugString("Reading serial settings dialog\n");
	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_PORT);
	if (handle) {
		index = ComboBox_GetCurSel(handle);
		port_index = ComboBox_GetItemData(handle, index);

		TempSettings.Port = 1;
		TempSettings.ModemName[0] = '\0';
		switch (port_index) {
			case ( 0 ):
				TempSettings.Port = 0x3f8;
				break;

			case ( 1 ):
				TempSettings.Port = 0x2f8;
				break;

			case ( 2 ):
				TempSettings.Port = 0x3e8;
				break;

			case ( 3 ):
				TempSettings.Port = 0x2e8;
				break;

			case ( 4 ):
				ComboBox_GetLBText(handle, index, TempBuf);
				TempBuf[MODEM_NAME_MAX-1] = '\0';
				temp = strchr(TempBuf, '-');
				if (temp) {
					strcpy(TempSettings.ModemName, temp + 1);
				}
				break;

			default:
				ComboBox_GetLBText(handle, index, TempBuf);
				TempBuf[MODEM_NAME_MAX-1] = '\0';
				strcpy(TempSettings.ModemName, TempBuf);
				break;
		}
	}

	DebugString("Port: %lx\n", TempSettings.Port);
	DebugString("Modem: %s\n", TempSettings.ModemName);

	handle = GetDlgItem(_Dialog, IDC_SERIAL_BAUD);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		i = ComboBox_GetItemData(handle, i);
		sscanf(baudname[i], "%d", &TempSettings.Baud);
	}

	DebugString("Baud: %ld\n", TempSettings.Baud);

	handle = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);
	if (handle) {
		initstr_index = ListBox_GetCurSel(handle);
		if (initstr_index != LB_ERR && Session.InitStrings.Count()) {
			TempSettings.InitStringIndex = initstr_index;
		} else {
			TempSettings.InitStringIndex = -1;
		}
	}

	handle = GetDlgItem(_Dialog, IDC_SERIAL_CALLWAITING);
	if (handle) {
		index = ComboBox_GetCurSel(handle);
		cwaitstr_index = ComboBox_GetItemData(handle, index);
		TempSettings.CallWaitStringIndex = cwaitstr_index;
		TempSettings.CallWaitString[0] = '\0';

		if (TempSettings.CallWaitStringIndex == CALL_WAIT_CUSTOM) {
			ComboBox_GetLBText(handle, index, TempBuf);
			TempBuf[CWAITSTRBUF_MAX-1] = '\0';
			temp = strchr(TempBuf, '-');
			if (temp) {
				strcpy(TempSettings.CallWaitString, temp + 1);
			}
		}
	}

	TempSettings.DialMethod = DIAL_TOUCH_TONE;
	handle = GetDlgItem(_Dialog, IDC_SERIAL_PULSE);
	if ( handle )
	{
		if (Button_GetCheck(handle) == TRUE) {
			TempSettings.DialMethod = DIAL_PULSE;
		}
	}
	DebugString("DialMethod: %s\n", TempSettings.DialMethod == DIAL_TOUCH_TONE ? "Tone" : "Pulse");

	handle = GetDlgItem(_Dialog, IDC_SERIAL_ERROR_CORRECTION);
	if (handle) {
		if (Button_GetCheck(handle) == 1) {
			TempSettings.ErrorCorrection = 1;
		} else {
			TempSettings.ErrorCorrection = 0;
		}
	}
	handle = GetDlgItem(_Dialog, IDC_SERIAL_DATA_COMPRESSION);
	if (handle) {
		if (Button_GetCheck(handle) == 1) {
			TempSettings.Compression = 1;
		} else {
			TempSettings.Compression = 0;
		}
	}

	dpstatus = NullModem.Detect_Port(&TempSettings);

	switch (dpstatus) {
		case PORT_VALID:
			return(true);

		case PORT_INVALID:
			WWMessageBox().Process(TXT_UNABLE_TO_OPEN_PORT, TXT_OK);
			break;

		case PORT_IRQ_INUSE:
			WWMessageBox().Process(TXT_IRQ_ALREADY_IN_USE, TXT_OK);
			break;
	}

	return(false);
}


/// <summary>
/// Initializes the serial settings dialog.
/// This routine takes a working copy of the caller's settings, supplies defaults for any
/// of them that were never filled in, and then brings every control in the dialog into
/// agreement with that copy.
/// </summary>
/// <param name="settings">The serial settings the dialog should start from.</param>
void SerialEdit::Init(SerialSettingsType *settings)
{
	/*
	**	Various Inits
	*/
	memcpy( &TempSettings, settings, sizeof(SerialSettingsType) );

	if (TempSettings.Port == 0) {
		TempSettings.Port = 0x2f8;
	}

	if (TempSettings.IRQ == -1) {
		TempSettings.IRQ = 3;
	}

	if (TempSettings.Baud == -1) {
		TempSettings.Baud = 57600;
	}

	Build_Port_Listbox();
	Build_Baud_Rate_Listbox();
	Build_Call_Wait_Listbox();

	Set_Dial_Method();

	if (!Session.InitStrings.Count()) {
		char * item = new char[8];
		strcpy(item, "ATZ");
		Session.InitStrings.Add(item);
		TempSettings.InitStringIndex = 0;
	}

	Build_Init_String_Listbox();

	HWND handle;

	handle = GetDlgItem(_Dialog, IDC_SERIAL_DATA_COMPRESSION);
	if (handle) {
		Button_SetCheck(handle, (settings->Compression != 0));
	}
	handle = GetDlgItem(_Dialog, IDC_SERIAL_ERROR_CORRECTION);
	if (handle) {
		Button_SetCheck(handle, (settings->ErrorCorrection != 0));
	}
}


/// <summary>
/// Fills in the serial port combo box.
/// This routine rebuilds the list from the standard COM ports plus whatever modems are
/// registered with Windows, and then selects the entry that the settings being edited call
/// for. A port or modem that is not among them is offered as the custom entry instead.
/// </summary>
void SerialEdit::Build_Port_Listbox(void)
{
	static const int portname[] = {
		TXT_COM1,
		TXT_COM2,
		TXT_COM3,
		TXT_COM4,
		TXT_CUSTOM,
	};

	int i;
	int index;
	int port_index;	/// index of currently-selected port
	char custom[128];
	char custom_port[128];

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_PORT);
	if (handle) {

		/*
		**	Set up the port list box & edit box
		*/
		ComboBox_ResetContent(handle);
		for (i = 0; i < ARRAY_SIZE(portname)-1; i++) {
			index = ComboBox_AddString(handle, Fetch_String(portname[i]));
			ComboBox_SetItemData(handle, index, i);
		}

		/*
		**	Loop through the first 10 possible modem entries in the registry. Frankly, its just
		**	tough luck if the user has more than 10 modems attached!
		*/
		ModemsFound = 0;
		for (i= 0; i < 10; i++) {
			ModemRegistryEntryClass *modemregistry = new ModemRegistryEntryClass (i);
			if (modemregistry) {
				if (modemregistry->Get_Modem_Name()) {
					strncpy(ModemNames[ModemsFound], modemregistry->Get_Modem_Name(), MODEM_NAME_MAX);
					index = ComboBox_AddString(handle, ModemNames[ModemsFound]);
					ComboBox_SetItemData(handle, index, ModemsFound + 5);
					ModemsFound++;
				}
				delete modemregistry;
			}
		}

		strcpy(custom, Fetch_String(TXT_CUSTOM));
		index = ComboBox_AddString(handle, custom);
		ComboBox_SetItemData(handle, index, 4);
		ComboBox_SetCurSel(handle, 0);

		/*
		**	Work out the current port index
		*/
		if (strlen(TempSettings.ModemName)) {
			port_index = ComboBox_FindString(handle, -1, TempSettings.ModemName);
			/*
			**	The modem name specified wasnt in the registry so add it as a custom entry
			*/
			if (port_index == CB_ERR) {
				port_index = ComboBox_Find_Item_Data(handle, -1, 4);
				if (port_index != CB_ERR) {
					sprintf(custom, "%s %s", Fetch_String(TXT_CUSTOM), TempSettings.ModemName);
					ComboBox_DeleteString(handle, port_index);
					port_index = ComboBox_AddString(handle, custom);
					ComboBox_SetItemData(handle, port_index, 4);
				}
			}

			ComboBox_SetCurSel(handle, port_index);

		} else {
			switch (TempSettings.Port) {
				case ( 0x3f8 ):
					port_index = 0;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				case ( 0x2f8 ):
					port_index = 1;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				case ( 0x3e8 ):
					port_index = 2;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				case ( 0x2e8 ):
					port_index = 3;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					break;

				default:
					port_index = 4;
					port_index = ComboBox_Find_Item_Data(handle, -1, port_index);
					if (port_index != CB_ERR) {
						sprintf(custom_port, "%s %x", Fetch_String(TXT_CUSTOM), TempSettings.Port);
						ComboBox_DeleteString(handle, port_index);
						port_index = ComboBox_AddString(handle, custom_port);
						ComboBox_SetItemData(handle, port_index, 4);
					}
					break;
			}
			ComboBox_SetCurSel(handle, port_index);
		}

	}
}


/// <summary>
/// Fills in the baud rate combo box.
/// This routine rebuilds the list of supported baud rates and selects the one that the
/// settings being edited call for.
/// </summary>
void SerialEdit::Build_Baud_Rate_Listbox(void)
{
	int i;
	int index;
	int baud_index;	/// index of currently-selected baud

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_BAUD);
	if (handle) {

		/*
		**	Set up the baud rate list box & edit box
		*/
		ComboBox_ResetContent(handle);
		for (i = 0; i < ARRAY_SIZE(baudname); i++) {
			index = ComboBox_AddString(handle, baudname[i]);
			ComboBox_SetItemData(handle, index, i);
		}

		/*
		**	Set the current indices
		*/
		switch (TempSettings.Baud) {
			case 14400:
				baud_index = 0;
				break;

			case 19200:
				baud_index = 1;
				break;

			case 28800:
				baud_index = 2;
				break;

			default:
			case 38400:
				baud_index = 3;
				break;

			case 57600:
				baud_index = 4;
				break;

			case 115200:
				baud_index = 5;
				break;
		}

		index = ComboBox_Find_Item_Data(handle, -1, baud_index);
		ComboBox_SetCurSel(handle, index);
	}
}


/// <summary>
/// Fills in the call waiting combo box.
/// This routine rebuilds the list from the session's call waiting strings and selects the
/// entry that the settings being edited call for.
/// </summary>
void SerialEdit::Build_Call_Wait_Listbox(void)
{
	int index;

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_CALLWAITING);

	/*
	**	Set up the cwait rate list box & edit box
	*/
	if (handle) {
		ComboBox_ResetContent(handle);
		for (int i = 0; i < CALL_WAIT_STRINGS_NUM; i++) {
			index = ComboBox_AddString(handle, Session.CallWaitStrings[i]);
			ComboBox_SetItemData(handle, index, i);
		}
		index = ComboBox_Find_Item_Data(handle, -1, TempSettings.CallWaitStringIndex);
		ComboBox_SetCurSel(handle, index);
	}
}


/***************************************************************************
 * Build_Init_String_Listbox -- [re]builds the initstring listbox          *
 *                                                                         *
 * This routine rebuilds the initstring list box from scratch; it also     *
 * updates the contents of the initstring edit field.                      *
 *                                                                         *
 * INPUT:                                                                  *
 *      list      ptr to list box                                          *
 *      edit      ptr to edit box                                          *
 *      buf      ptr to buffer for initstring                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      none.                                                              *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   06/08/1995 DRD : Created.                                             *
 *=========================================================================*/
void SerialEdit::Build_Init_String_Listbox(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);
	if (handle) {
		ListBox_ResetContent(handle);
		for (int i = 0; i < Session.InitStrings.Count(); i++) {
			ListBox_AddString(handle, Session.InitStrings[i]);
		}
		if (ListBox_GetCount(handle) && TempSettings.InitStringIndex >= -1) {
			if (TempSettings.InitStringIndex >= ListBox_GetCount(handle)) {
				TempSettings.InitStringIndex = 0;
			}
		} else {
			TempSettings.InitStringIndex = -1;
		}
		if (TempSettings.InitStringIndex != -1) {
			ListBox_SetCurSel(handle, TempSettings.InitStringIndex);
		}
	}
}


/// <summary>
/// Sets the dialing method radio buttons.
/// This routine checks either the tone or the pulse button so that the dialog agrees with
/// the dialing method of the settings being edited.
/// </summary>
void SerialEdit::Set_Dial_Method(void)
{
	HWND handle;

	bool is_tone_dialing = TempSettings.DialMethod == DIAL_TOUCH_TONE;
	handle = GetDlgItem(_Dialog, IDC_SERIAL_TONE);
	if (handle) {
		Button_SetCheck(handle, is_tone_dialing == true);
	}
	handle = GetDlgItem(_Dialog, IDC_SERIAL_PULSE);
	if (handle) {
		Button_SetCheck(handle, is_tone_dialing == false);
	}
}


/// <summary>
/// Moves a custom combo box entry into its edit control.
/// This routine is used when the user picks the custom port or the custom call waiting
/// entry. The text that follows the dash in that entry is placed into the edit control,
/// which is then revealed and given the focus so it can be typed over.
/// </summary>
/// <param name="id">The dialog control identifier of the edit control to fill.</param>
void SerialEdit::Set_String(int id)
{
	HWND handle = GetDlgItem(_Dialog, id);
	if (handle) {
		ShowWindow(handle, SW_SHOW);
		Edit_Enable(handle, TRUE);
		char * temp = strchr(TempBuf, '-');
		if (temp) {
			temp = temp + 1;
			strtrim(temp);
		} else {
			temp = (char *)"";
		}
		Edit_SetText(handle, temp);
		SetFocus(handle);
	}
}


/// <summary>
/// Accepts the port typed into the custom port field.
/// This routine is called when the user is finished editing that field. The text is taken
/// as either one of the standard COM ports or a raw port address, and the port combo box
/// is brought into agreement with it. Anything unrecognizable earns a message box.
/// </summary>
void SerialEdit::Set_Port(void)
{
	int index;
	int port_index = -1;	/// index of currently-selected port

	HWND handle1 = GetDlgItem(_Dialog, IDC_SERIAL_PORT);
	HWND handle2 = GetDlgItem(_Dialog, IDC_SERIAL_PORT_EDIT);
	if (handle1 && handle2) {
		ShowWindow(handle2, SW_HIDE);
		Edit_Enable(handle2, FALSE);
		Edit_GetText(handle2, TempBuf, PORTBUF_MAX);
		TempBuf[PORTBUF_MAX-1] = '\0';
		strtrim(TempBuf);
		strupr(TempBuf);

		if ( stricmp(TempBuf, "3F8") == 0 ) {
			port_index = 0;

		} else if ( stricmp(TempBuf, "2F8") == 0 ) {
			port_index = 1;

		} else if ( stricmp(TempBuf, "3E8") == 0 ) {
			port_index = 2;

		} else if ( stricmp(TempBuf, "2E8") == 0 ) {
			port_index = 3;

		} else if ( strncmp(TempBuf, "COM", 3) == 0 ) {

			switch ( (TempBuf[3]) ) {
				case '1':
					port_index = 0;
					break;

				case '2':
					port_index = 1;
					break;

				case '3':
					port_index = 2;
					break;

				case '4':
					port_index = 3;
					break;

				default:
					if (TempBuf[3] <= '9' && TempBuf[3] >'0') {
						TempBuf[4] = 0;
						Set_Custom_Port(IDC_SERIAL_PORT, 4, TempBuf);
					} else {
						WWMessageBox().Process(TXT_INVALID_PORT_ADDRESS, TXT_OK);
					}
			}
		} else {
			Set_Custom_Port(IDC_SERIAL_PORT, 4, TempBuf);
		}

		if (port_index != -1) {
			index = ComboBox_Find_Item_Data(handle1, -1, port_index);
			ComboBox_SetCurSel(handle1, index);
		}
	}
}


/// <summary>
/// Accepts the string typed into the custom call waiting field.
/// This routine is called when the user is finished editing that field. The text is
/// trimmed and folded back into the call waiting combo box as its custom entry.
/// </summary>
void SerialEdit::Set_Init_String(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_CALLWAITING_EDIT);
	if (handle) {
		ShowWindow(handle, SW_HIDE);
		Edit_Enable(handle, FALSE);
		Edit_GetText(handle, TempBuf, CWAITSTRBUF_MAX);
		TempBuf[CWAITSTRBUF_MAX-1] = '\0';
		strtrim(TempBuf);
		strupr(TempBuf);
		Set_Custom_Port(IDC_SERIAL_CALLWAITING, 3, TempBuf);
	}
}


/// <summary>
/// Replaces a combo box's custom entry with a new value.
/// This routine is used by the port and call waiting edit fields to fold the value the
/// user typed back into the combo box it belongs to. The old custom entry is discarded and
/// the new one becomes the current selection.
/// </summary>
/// <param name="id">The dialog control identifier of the combo box to update.</param>
/// <param name="port">The item data value that marks the custom entry.</param>
/// <param name="str">The text to store as the custom entry.</param>
void SerialEdit::Set_Custom_Port(int id, int port, char *str)
{
	int item;
	char *temp;
	char buf[128];

	HWND handle = GetDlgItem(_Dialog, id);
	if (handle) {
		item = ComboBox_Find_Item_Data(handle, -1, port);
		ComboBox_GetLBText(handle, item, buf);
		temp = strchr(buf, '-');
		if (temp != NULL && stricmp(str, temp + 1)) {
			sprintf(buf, "%s ", Fetch_String(TXT_CUSTOM));
			strcat(buf, str);
			ComboBox_DeleteString(handle, item);
			int index = ComboBox_AddString(handle, buf);
			ComboBox_SetItemData(handle, index, port);
			ComboBox_SetCurSel(handle, index);
		}
	}
}


/// <summary>
/// Adds the typed modem init string to the list.
/// This routine is called by the Add button, and also when the init string edit field
/// fills up. The string is added to the session's init string list unless it is already
/// present there, and is then made the current selection.
/// </summary>
void SerialEdit::Add_Init_String(void)
{
	int initstr_index;	// index of currently-selected modem init (default = "ATZ")

	HWND handle1 = GetDlgItem(_Dialog, IDC_SERIAL_INITSTRING);
	HWND handle2 = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);

	if (handle1 && handle2) {
		Edit_GetText(handle1, TempBuf, sizeof(TempBuf));
		Edit_SetText(handle1, "");
		strtrim(TempBuf);
		if (strlen(TempBuf)) {
			strupr(TempBuf);
			initstr_index = ListBox_FindString(handle2, -1, TempBuf);
			if (initstr_index == LB_ERR) {
				char * item = new char[INITSTRBUF_MAX];
				strcpy(item, TempBuf);
				initstr_index = ListBox_AddString(handle2, item);
				if (initstr_index == LB_ERR) {
					return;
				}
				Session.InitStrings.Add(item);
			}
			ListBox_SetCurSel(handle2, initstr_index);
			ListBox_SetTopIndex(handle2, initstr_index);
			TempSettings.InitStringIndex = initstr_index;
		}
	}
}


/// <summary>
/// Removes the selected modem init string from the list.
/// This routine is called by the Delete button. The string is dropped from both the list
/// box and the session's init string list, and the selection then moves to a surviving
/// entry.
/// </summary>
void SerialEdit::Delete_Init_String(void)
{
	int initstr_index;	// index of currently-selected modem init (default = "ATZ")

	HWND handle = GetDlgItem(_Dialog, IDC_SERIAL_INITLIST);
	if (handle) {
		if (ListBox_GetCount(handle) > 0) {
			initstr_index = ListBox_GetCurSel(handle);
			if (initstr_index != LB_ERR) {
				ListBox_DeleteString(handle, initstr_index);
				Session.InitStrings.Delete_Index(initstr_index);
				if (initstr_index >= ListBox_GetCount(handle)) {
					initstr_index--;
				}
				ListBox_SetCurSel(handle, initstr_index);
				ListBox_SetTopIndex(handle, initstr_index);
				TempSettings.InitStringIndex = initstr_index;
			}
		}
	}
}
