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

/* $Header: /counterstrike/NULLDLG.CPP 14    3/17/97 1:05a Steve_tall $ */
/***************************************************************************
 **   C O N F I D E N T I A L --- W E S T W O O D    S T U D I O S        **
 ***************************************************************************
 *                                                                         *
 *                 Project Name : Command & Conquer                        *
 *                                                                         *
 *                    File Name : NULLDLG.CPP                              *
 *                                                                         *
 *                   Programmer : Bill R. Randolph                         *
 *                                                                         *
 *                   Start Date : 04/29/95                                 *
 *                                                                         *
 *                  Last Update : Jan. 21, 1997 [V.Grippi]                 *
 *                                                                         *
 *-------------------------------------------------------------------------*
 * Functions:                                                              *
 *   Build_InitString_Listbox -- [re]builds the initstring entry listbox   *
 *   Build_Phone_Listbox -- [re]builds the phone entry listbox             *
 *   Com_Scenario_Dialog -- Serial game scenario selection dialog          *
 *   Com_Settings_Dialog -- Lets user select serial port settings          *
 *   Destroy_Null_Connection -- destroys the given connection              *
 *   Edit_Phone_Dialog -- lets user edit a phone book entry                *
 *   Init_Null_Modem -- Initializes Null Modem communications              *
 *   Init_String_Compare -- for qsort                                      *
 *   Phone_Compare -- for qsort                                            *
 *   Phone_Dialog -- Lets user edit phone directory & dial                 *
 *   Reconnect_Null_Modem -- allows user to reconnect                      *
 *   Select_Serial_Dialog -- Serial Communications menu dialog             *
 *   Shutdown_Modem -- Shuts down modem/null-modem communications          *
 *   Test_Null_Modem -- Null-Modem test routine                            *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "nulldlg.h"

#include "_map.h"
#include "_rules.h"
#include "_timer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "house.h"
#include "houstype.h"
#include "init.h"
#include "modemgst.h"
#include "modemhst.h"
#include "modemreg.h"
#include "msgbox.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "phone.h"
#include "phoneed.h"
#include "rules.h"
#include "serialed.h"
#include "stimer.h"
#include "wincomm.h"

#include <ctime>


ModemRegistryEntryClass *ModemRegistry = NULL;		//Ptr to modem registry data

static bool Reconnect_Null_Modem( void );
static bool Com_Settings_Dialog( SerialSettingsType *settings );
static bool Com_Scenario_Dialog(bool skirmish=false);
static bool Com_Show_Scenario_Dialog(void);
static bool Phone_Dialog (void);
//static void Build_Init_String_Listbox (ListClass *list, EditClass *edit, char *buf, int *index);
static int Init_String_Compare (const void *p1, const void *p2);
//static void Build_Phone_Listbox (ListClass *list, EditClass *edit, char *buf);
static int Phone_Compare (const void *p1, const void *p2);
static int Edit_Phone_Dialog (PhoneEntryClass *phone);
static bool Dial_Modem( SerialSettingsType *settings, bool reconnect );
static bool Answer_Modem( SerialSettingsType *settings, bool reconnect );
static void Modem_Echo( char c );
static int Test_Null_Modem( void );

BOOL CALLBACK Select_Serial_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
void Select_Serial_On_INITDIALOG(HWND window);

void __cdecl Smart_Printf( char *format, ... );
void Hex_Dump_Data( char *buffer, int length );
void itoh( int i, char *s);


char TheirName[MPLAYER_NAME_MAX];
int TheirColor;
HousesType TheirHouse;
static char DialString[ CWAITSTRBUF_MAX + PhoneEntryClass::PHONE_MAX_NUM - 1 ];
static SerialSettingsType *DialSettings;

#define PCOLOR_BROWN	PCOLOR_GREY


//#define SHOW_MONO		0			/// Replaced with _DEBUG


/***************************************************************************
 * Select_Serial_Dialog -- Serial Communications menu dialog               *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      GAME_MODEM            user wants to play a modem game              *
 *      GAME_NULL_MODEM      user wants to play a null-modem game          *
 *      GAME_NORMAL            user hit Cancel                             *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *=========================================================================*/
GameType Select_Serial_Dialog( void )
{
	int rc = -1;
	GameType retval = (GameType)-1;
	SerialSettingsType *settings;

	HWND dialog = OwnerDraw::Begin_Dialog(IDD_SELECT_SERIAL, Select_Serial_Dialog_Proc);

	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		OwnerDraw::Move_Dialog(dialog, -1, ((HiddenSurface->Get_Height() - 400) / 2) + 147);
		OwnerDraw::Display_Dialog(dialog);

		if (Session.SerialDefaults.Port == 0 ||
			Session.SerialDefaults.IRQ == -1 ||
			Session.SerialDefaults.Baud == -1) {
			rc = IDC_SERIAL_SETTINGS_BTN;

		} else if ( NullModem.Detect_Port( &Session.SerialDefaults ) != PORT_VALID ) {
			rc = IDC_SERIAL_SETTINGS_BTN;
		}

		while (retval == -1) {
			while (rc < 0) {
				if (OwnerDraw::Dialog_Message_Handler() == true) {
					break;
				}
				Title_Screen_Restore();
			}

			switch (rc) {

				case IDC_SERIAL_DIAL:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);
					/*
					**	Remote-connect
					*/
					if (Phone_Dialog() == true) {
						if (Session.PhoneBook[Session.CurPhoneIdx]->Settings.Port == 0) {
							settings = &Session.SerialDefaults;
						} else {
							settings = &(Session.PhoneBook[Session.CurPhoneIdx]->Settings);
						}

						if (SerialPort) {
							delete SerialPort;
						}
						SerialPort = new WinModemClass;

						if ( Init_Null_Modem( settings ) ) {

							if (settings->CallWaitStringIndex == CALL_WAIT_CUSTOM) {
								strcpy( DialString, settings->CallWaitString );
							} else {
								strcpy( DialString,
									Session.CallWaitStrings[ settings->CallWaitStringIndex ] );
							}
							strcat( DialString, Session.PhoneBook[ Session.CurPhoneIdx ]->Number );

							if ( Dial_Modem( settings, false ) ) {
								Session.ModemType = MODEM_DIALER;
								Session.Type = GAME_MODEM;
								if ( Com_Scenario_Dialog() ) {
									retval = GAME_MODEM;
									break;
								} else {
									Session.Type = GAME_NORMAL;
									retval = GAME_NORMAL;
								}
							}
							// restore to default
							NullModem.Change_IRQ_Priority( 0 );
						} else {
							WWMessageBox().Process(TXT_SELECT_SETTINGS, TXT_OK);
						}
					}
					ShowWindow(dialog, SW_SHOW);
					break;

				case IDC_SERIAL_ANSWER:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);

					/*
					**	Remote-connect
					*/
					settings = &Session.SerialDefaults;

					if (SerialPort) {
						delete SerialPort;
					}
					SerialPort = new WinModemClass;

					if ( Init_Null_Modem( settings ) ) {
						if ( Answer_Modem( settings, false ) ) {
							Session.ModemType = MODEM_ANSWERER;
							Session.Type = GAME_MODEM;
							if ( Com_Show_Scenario_Dialog() ) {
								retval = GAME_MODEM;
								break;
							} else {
								Session.Type = GAME_NORMAL;
								retval = GAME_NORMAL;
							}
						}
						NullModem.Change_IRQ_Priority( 0 );
					} else {
						WWMessageBox().Process(TXT_SELECT_SETTINGS, TXT_OK);
					}

					ShowWindow(dialog, SW_SHOW);
					break;

				case IDC_SERIAL_NULLMODEM:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);

					/*
					**	Otherwise, remote-connect; save values if we're recording
					*/
					if ( Init_Null_Modem( &Session.SerialDefaults ) ) {
						switch ( Test_Null_Modem() ) {
							case (1):
								Session.ModemType = MODEM_NULL_HOST;
								Session.Type = GAME_NULL_MODEM;
								if ( Com_Scenario_Dialog() == 1 ) {
									retval = GAME_NULL_MODEM;
									break;
								} else {
									Session.Type = GAME_NORMAL;
								}
								break;

							case (2):
								Session.ModemType = MODEM_NULL_JOIN;
								Session.Type = GAME_NULL_MODEM;
								if ( Com_Show_Scenario_Dialog() == 1 ) {
									retval = GAME_NULL_MODEM;
									break;
								} else {
									Session.Type = GAME_NORMAL;
								}
								break;

							case (3):
								WWMessageBox().Process( TXT_MODEM_OR_LOOPBACK, TXT_OK );
								break;
						}
						if (retval == -1) {
							// restore to default
							NullModem.Change_IRQ_Priority( 0 );
							ShowWindow(dialog, SW_SHOW);
						}
					} else {
						WWMessageBox().Process(TXT_SELECT_SETTINGS, TXT_OK);
						ShowWindow(dialog, SW_SHOW);
					}
					break;

				case IDC_SERIAL_SETTINGS_BTN:
					ShowWindow(dialog, SW_HIDE);
					UpdateWindow(MainWindow);
					if (Com_Settings_Dialog(&Session.SerialDefaults)) {
						Session.Write_MultiPlayer_Settings();
					}
					Select_Serial_On_INITDIALOG(dialog);
					ShowWindow(dialog, SW_SHOW);
					break;

				default:
					retval = GAME_NORMAL;
					break;
			}

			rc = -1;
		}

		OwnerDraw::End_Dialog(dialog);
		return(retval);
	}

	return(GAME_NORMAL);
}

void Select_Serial_On_INITDIALOG(HWND window);


/// <summary>
/// Handles the messages for the serial connection dialog.
/// This routine gives the owner draw code first refusal on every message, and only then
/// deals with the ones this dialog cares about -- initializing the buttons and recording
/// which connection method the player picked so the dialog's owner can act on it.
/// </summary>
/// <returns>Returns with the owner draw result, or zero if this routine handled it.</returns>
BOOL CALLBACK Select_Serial_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int *retval;
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		switch (message) {
			case WM_INITDIALOG:
				Select_Serial_On_INITDIALOG(window);
				break;

			case WM_COMMAND:
				retval = (int *)GetWindowLong(window, DWL_USER);
				if (wparam == 2 || wparam > IDC_DISCONNECT_MESSAGES && wparam <= IDC_SERIAL_NULLMODEM) {
					*retval = wparam;
				}
				break;
		}
		return(0);
	}
	return(rc);
}


/// <summary>
/// Prepares the serial connection dialog for display.
/// This routine grays out the dial, answer and null modem buttons when the serial defaults
/// do not describe a port the game can actually open, so that the player cannot start a
/// connection that is bound to fail.
/// </summary>
/// <param name="window">Handle of the dialog being initialized.</param>
void Select_Serial_On_INITDIALOG(HWND window)
{
	HWND handle;

	bool enabled = true;
	if (Session.SerialDefaults.Port == 0) {
		enabled = false;
	} else if (Session.SerialDefaults.IRQ == -1) {
		enabled = false;
	} else if (Session.SerialDefaults.Baud == -1 || NullModem.Detect_Port(&Session.SerialDefaults)) {
		enabled = false;
	}

	handle = GetDlgItem(window, IDC_SERIAL_DIAL);
	if (handle) {
		EnableWindow(handle, enabled);
	}

	handle = GetDlgItem(window, IDC_SERIAL_ANSWER);
	if (handle) {
		EnableWindow(handle, enabled);
	}

	handle = GetDlgItem(window, IDC_SERIAL_NULLMODEM);
	if (handle) {
		EnableWindow(handle, enabled);
	}
}


/***************************************************************************
 * Init_Null_Modem -- Initializes Null Modem communications                *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      true = OK, false = error                                           *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *   8/2/96      ST : Win32 support added                                  *
 *=========================================================================*/
bool Init_Null_Modem( SerialSettingsType *settings )
{
	if ( NullModem.Init( settings->Port, settings->IRQ,
								settings->ModemName,
								settings->Baud, 0,  8, 1 ) ) {
		return(true);
	} else {
		return(false);
	}
}


/***************************************************************************
 * Shutdown_Modem -- Shuts down modem/null-modem communications            *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      none.                                                              *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *=========================================================================*/
void Shutdown_Modem( void )
{
	if (!Session.Play) {
		if (Session.Type == GAME_MODEM) {
			NullModem.Hangup_Modem();
		}
	}

	//NullModem.Change_IRQ_Priority( 0 );		// reset priority of interrupts

	//
	// close port
	//
	NullModem.Shutdown();
}


/***************************************************************************
 * Modem_Signoff -- sends EXIT event                                       *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      none.                                                              *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   08/03/1995 DRD : Created.                                             *
 *=========================================================================*/
void Modem_Signoff( void )
{
	unsigned int starttime;
	EventClass event;

	if (!Session.Play) {
		/*
		**	Send a sign-off packet
		*/
		memset (&event, 0, sizeof(EventClass));
		event.Type = EventClass::EXIT;
		NullModem.Send_Message (&event,sizeof(EventClass),0);
		NullModem.Send_Message (&event,sizeof(EventClass),0);

		starttime = TickCount;
		while ( (TickCount - starttime) < TIMER_SECOND / 2) {
			NullModem.Service();
		}
	}
}


/***************************************************************************
 * Test_Null_Modem -- Null-Modem test routine                              *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      0 = failure to connect; 1 = I'm the game owner, 2 = I'm not        *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *   8/2/96      ST : Win32 support added                                  *
 *=========================================================================*/
int Test_Null_Modem( void )
{
	/*
	**	Button Enumerations
	*/
	enum {
		BUTTON_CANCEL = 100,
	};

	/*
	**	Dialog variables
	*/
	bool process = true;						// process while true

	int retval = 0;
	unsigned int starttime;
	int packetlen;

	char buffer[80*3];
	SerialPacketType SendPacket;
	SerialPacketType ReceivePacket;

	/*
	**	Determine the dimensions of the text to be used for the dialog box.
	**	These dimensions will control how the dialog box looks.
	*/
	//strcpy( buffer, Fetch_String( TXT_WAITING_CONNECT ) );

	/*
	**	Initialize
	*/
	process = true;

	/*
	**	This is supposed to be a direct connection so hang up any modem on this port
	**	just to annoy British Telecom
	*/
	/*
	**	Go into break mode
	*/
	SetCommBreak(SerialPort->Get_Port_Handle());

	/*
	**	Send hangup command
	*/
	SerialPort->Write_To_Serial_Port ((unsigned char*)"ATH\r", strlen("ATH\r"));
	CDTimerClass<SystemTimerClass> time;
	time = (2*TIMER_SECOND);
	while (time) {}

	/*
	**	Back out of break mode
	*/
	ClearCommBreak(SerialPort->Get_Port_Handle());

	/*
	**	Drop DTR as well - just in case the modem still hasnt got the message
	*/
	EscapeCommFunction(SerialPort->Get_Port_Handle(), CLRDTR);

	/*
	**	Check for a packet.  If we detect one, the other system has already been
	**	started.  Wait 1/2 sec for him to receive my ACK, then exit with success.
	**	Note: The initial time must be a little longer than the resend delay.
	**	Just in case we just missed the packet.
	*/
	starttime = TickCount;
	while ( TickCount - starttime < 80) {
		NullModem.Service();
		if ( NullModem.Get_Message( &ReceivePacket, &packetlen ) > 0) {
			if (ReceivePacket.Command == SERIAL_CONNECT) {
				starttime = TickCount;
				while (TickCount - starttime < TIMER_SECOND / 2)
					NullModem.Service();
				process = false;
				retval = 2;
				break;
			}
		}
	}

	/*
	**	Send a packet across.  As long as Num_Send() is non-zero, the other system
	**	hasn't received it yet.
	*/
	memset (&SendPacket, 0, sizeof(SerialPacketType));
	if (process == true) {
		SendPacket.Command = SERIAL_CONNECT;
		//
		// put time from start of game for determining the host in case of tie.
		//
		SendPacket.ScenarioInfo.Seed = TickCount;
		SendPacket.ID = (int) buffer;		// address of buffer for more uniqueness.

		NullModem.Send_Message (&SendPacket, sizeof(SendPacket), 1);

		starttime = TickCount;
		while (TickCount - starttime < 80) {
			NullModem.Service();
			if (NullModem.Get_Message (&ReceivePacket, &packetlen) > 0) {
				if (ReceivePacket.Command == SERIAL_CONNECT) {
					starttime = TickCount;
					while (TickCount - starttime < TIMER_SECOND / 2)
						NullModem.Service();

					//
					// whoever has the highest time is the host
					//
					if (ReceivePacket.ScenarioInfo.Seed > SendPacket.ScenarioInfo.Seed) {
						process = false;
						retval = 2;
					} else if (ReceivePacket.ScenarioInfo.Seed == SendPacket.ScenarioInfo.Seed) {
						if (ReceivePacket.ID > SendPacket.ID) {
							process = false;
							retval = 2;
						//
						// if they are equal then it's a loopback cable or a modem
						//
						} else if (ReceivePacket.ID == SendPacket.ID) {
							process = false;
							retval = 3;
						}
					}

					break;
				}
			}
		}

		if (process) {
			ModemDialogIsOpen = false;
			HWND dialog = OwnerDraw::Custom_Message_Box(Fetch_String(TXT_WAITING_CONNECT), Fetch_String(TXT_CANCEL), &ModemDialogIsOpen);

			if (dialog) {

				OwnerDraw::Display_Dialog(dialog);

				starttime = TickCount;

				/*
				**	Main Processing Loop
				*/
				while (true) {

					if (OwnerDraw::Dialog_Message_Handler() == 1 || ModemDialogIsOpen) {
						retval = 0;
						break;
					}

					/*
					**	If we have just received input focus again after running in the background then
					**	we need to redraw.
					*/
					Title_Screen_Restore();

					/*
					**	Service the connection.
					*/
					NullModem.Service();
					if (NullModem.Num_Send() == 0) {
						if (NullModem.Get_Message (&ReceivePacket, &packetlen) > 0) {
							if (ReceivePacket.Command == SERIAL_CONNECT) {
								starttime = TickCount;
								while (TickCount - starttime < TIMER_SECOND / 2)
									NullModem.Service();

								//
								// whoever has the highest time is the host
								//
								if (ReceivePacket.ScenarioInfo.Seed > SendPacket.ScenarioInfo.Seed) {
									process = false;
									retval = 2;

								} else if (ReceivePacket.ScenarioInfo.Seed == SendPacket.ScenarioInfo.Seed) {
									if (ReceivePacket.ID > SendPacket.ID) {
										process = false;
										retval = 2;

									//
									// if they are equal then it's a loopback cable or a modem
									//
									} else if (ReceivePacket.ID == SendPacket.ID) {
										process = false;
										retval = 3;
									}
								}

							} else {
								retval = 0;
								process = false;
							}
						} else {
							retval = 1;
							process = false;
						}
					}

					if (TickCount - starttime > TIMER_MINUTE) {		// only wait 1 minute
						retval = 0;
						break;
					}

					if (process != true) {
						break;
					}
				}	/* end of while */

				OwnerDraw::End_Dialog(dialog);
			}
		}
	}

	return( retval );
}


/***************************************************************************
 * Reconnect_Modem -- allows user to reconnect                             *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      0 = failure to connect; 1 = connect OK                             *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *=========================================================================*/
bool Reconnect_Modem( void )
{
	bool status = false;
	int modemstatus;

	switch (Session.ModemType) {
		case (MODEM_NULL_HOST):
		case (MODEM_NULL_JOIN):
			status = Reconnect_Null_Modem();
			break;

		case (MODEM_DIALER):
			modemstatus = NullModem.Get_Modem_Status();
			if ( (modemstatus & WinModemClass::CD_SET) ) {
				status = Reconnect_Null_Modem();
			} else {
				status = Dial_Modem( DialSettings, true );
			}
			break;

		case (MODEM_ANSWERER):
			modemstatus = NullModem.Get_Modem_Status();
			if ( (modemstatus & WinModemClass::CD_SET) ) {
				status = Reconnect_Null_Modem();
			} else {
				status = Answer_Modem( DialSettings, true );
			}
			break;
	}

	return( status );
}


/***************************************************************************
 * Reconnect_Null_Modem -- allows user to reconnect                        *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      0 = failure to connect; 1 = connect OK                             *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *=========================================================================*/
static bool Reconnect_Null_Modem( void )
{
	/*
	**	Dialog variables
	*/
	bool retval = false;
	bool process = true;						// process while true
	ModemDialogIsOpen = false;

	unsigned int starttime;
	unsigned int lastmsgtime;
	int packetlen;
	SerialPacketType SendPacket;
	SerialPacketType ReceivePacket;

	/*
	**	Initialize
	*/
	HWND dialog = OwnerDraw::Custom_Message_Box( Fetch_String( TXT_NULL_CONNERR_CHECK_CABLES ), Fetch_String( TXT_CANCEL ), &ModemDialogIsOpen );

	if (dialog != NULL) {


		/*
		**	Draw the dialog
		*/
		OwnerDraw::Display_Dialog(dialog);

		/*
		**	Main Processing Loop
		*/
		starttime = lastmsgtime = TickCount;
		while (process == true) {

			if (OwnerDraw::Dialog_Message_Handler() == true || ModemDialogIsOpen != false) {
				retval = false;
				break;
			}

			/*
			**	Service the connection.
			*/
			NullModem.Service();

			/*
			**	Resend our message if it's time
			*/
			if (TickCount - starttime > PACKET_RETRANS_TIME) {
				starttime = TickCount;
				memset (&SendPacket, 0, sizeof(SerialPacketType));
				SendPacket.Command = SERIAL_CONNECT;
				SendPacket.ID = Session.ColorIdx;
				NullModem.Send_Message (&SendPacket, sizeof(SendPacket), 0);
			}

			/*
			**	Check for an incoming message
			*/
			if (NullModem.Get_Message (&ReceivePacket, &packetlen) > 0) {

				lastmsgtime = TickCount;

				if (ReceivePacket.Command == SERIAL_CONNECT) {

					// are we getting our own packets back??

					if (ReceivePacket.ID == Session.ColorIdx) {
						WWMessageBox().Process (TXT_SYSTEM_NOT_RESPONDING, TXT_OK);
						retval = false;
						process = false;
						break;
					}

					/*
					**	OK, we got our message; now we have to make certain the other
					**	guy gets his, so send him one with an ACK required.
					*/
					memset (&SendPacket, 0, sizeof(SerialPacketType));
					SendPacket.Command = SERIAL_CONNECT;
					SendPacket.ID = Session.ColorIdx;
					NullModem.Send_Message (&SendPacket, sizeof(SendPacket), 1);
					starttime = TickCount;
					while (TickCount - starttime < TIMER_SECOND)
						NullModem.Service();
					retval = true;
					process = false;
				}
			}

			//
			// timeout if we do not get any packets
			//
			if (TickCount - lastmsgtime > PACKET_CANCEL_TIMEOUT) {
				retval = false;
				process = false;
			}

		}	/* end of while */

		OwnerDraw::End_Dialog(dialog);
	}

	return( retval );

}


/***********************************************************************************************
 * Destroy_Null_Connection -- destroys the given connection                                    *
 *                                                                                             *
 * Call this routine when a connection goes bad, or another player signs off.                  *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      id         connection ID to destroy; this should be the HousesType of the player being *
 *             "destroyed".                                                                    *
 *      error      0 = user signed off; 1 = connection error                                   *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/31/1995 DRD : Created.                                                                 *
 *=============================================================================================*/
void Destroy_Null_Connection(int id, int error)
{
	int i;
	HouseClass *housep;
	char txt[80];


	if ( Session.NumPlayers == 1 ) {
		return;
	}

	/*
	**	Do nothing if the house isn't human.
	*/
	housep = Houses[id];
	if (!housep || !housep->IsHuman)
		return;

	/*
	**	Create a message to display to the user
	*/
	txt[0] = '\0';
	switch (error) {
		case 1:
			sprintf(txt,Fetch_String(TXT_CONNECTION_LOST), (const char *)housep->IniName);
			break;

		case 0:
			sprintf(txt,Fetch_String(TXT_LEFT_GAME), (const char *)housep->IniName);
			break;

		case -1:
			NullModem.Delete_Connection();
			break;
	}

	if (strlen(txt)) {
		Session.Messages.Add_Message (NULL, 0, txt, housep->Class->Scheme,
			TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD), Rule->MessageDelay * TICKS_PER_MINUTE);
		Map.Flag_To_Redraw();
	}

	/*
	**	Remove this player from the Players vector
	*/
	for (i = 0; i < Session.Players.Count(); i++) {
		if (!stricmp(Session.Players[i]->Name,housep->IniName)) {
			delete Session.Players[i];
			Session.Players.Delete(Session.Players[i]);
			break;
		}
	}

	/*
	**	Turn the player's house over to the computer's AI
	*/
	housep->AI_Takeover();

	Session.NumPlayers--;

	/*
	**	If we're the last player left, tell the user.
	*/
	if (Session.NumPlayers == 1) {
		sprintf(txt,"%s",Fetch_String(TXT_JUST_YOU_AND_ME));
		Session.Messages.Add_Message (NULL, 0, txt, housep->Class->Scheme,
			TextPrintType(TPF_USE_GRAD_PAL|TPF_FULLSHADOW|TPF_6PT_GRAD), Rule->MessageDelay * TICKS_PER_MINUTE);
		Map.Flag_To_Redraw();
	}
}


/***************************************************************************
 * Com_Settings_Dialog -- Lets user select serial port settings            *
 *                                                                         *
 *  +------------------------------------------------------+               *
 *  |                    Settings                          |               *
 *  |                                                      |               *
 *  |     Port:____       IRQ:__        Baud:______        |               *
 *  |  +------------+  +------------+  +------------+      |               *
 *  |  |            |  |            |  |            |      |               *
 *  |  |            |  |            |  |            |      |               *
 *  |  |            |  |            |  |            |      |               *
 *  |  |            |  |            |  |            |      |               *
 *  |  +------------+  +------------+  +------------+      |               *
 *  |                                                      |               *
 *  |   Initialization:        [Add]   [Delete]            |               *
 *  |    _____________________________                     |               *
 *  |   +--------------------------------------------+     |               *
 *  |   |                                            |     |               *
 *  |   |                                            |     |               *
 *  |   |                                            |     |               *
 *  |   +--------------------------------------------+     |               *
 *  |                                                      |               *
 *  |   Call Waiting:                                      |               *
 *  |    _______________                                   |               *
 *  |   +-----------------+          [Tone Dialing]        |               *
 *  |   |                 |                                |               *
 *  |   |                 |          [Pulse Dialing]       |               *
 *  |   |                 |                                |               *
 *  |   +-----------------+                                |               *
 *  |                                                      |               *
 *  |                   [OK]   [Cancel]                    |               *
 *  +------------------------------------------------------+               *
 *                                                                         *
 * INPUT:                                                                  *
 *      settings      ptr to SerialSettingsType structure                  *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      true = OK, false = Cancel                                          *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *=========================================================================*/
static bool Com_Settings_Dialog( SerialSettingsType *settings )
{
	return(SerialEdit().Dialog(settings));

}	/* end of Com_Settings_Dialog */


/***************************************************************************
 * Init_String_Compare -- for qsort                                        *
 *                                                                         *
 * INPUT:                                                                  *
 *      p1,p2      ptrs to elements to compare                             *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      0 = same, -1 = (*p1) goes BEFORE (*p2), 1 = (*p1) goes AFTER (*p2) *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   06/08/1995 DRD : Created.                                             *
 *=========================================================================*/
static int Init_String_Compare (const void *p1, const void *p2)
{
	return( strcmp( *((char **)p1), *((char **)p2) ) );
}


/***********************************************************************************************
 * Com_Scenario_Dialog -- Serial game scenario selection dialog                                *
 *                                                                                             *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      true = success, false = cancel                                                         *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/14/1995 BR : Created.
 *   01/21/97 V.Grippi added check for CS before sending scenario file                         *
 *=============================================================================================*/
bool Com_Scenario_Dialog(bool skirmish)
{
	return(ModemHost().Dialog());
}


/***********************************************************************************************
 * Com_Show_Scenario_Dialog -- Serial game scenario selection dialog                           *
 *                                                                                             *
 * The 'Players' vector is filled in by this routine, when the game starts; this               *
 * is for the Assign_Houses routine, which expects this vector to contain all                  *
 * players' names & houses & colors.  Other than that, the Players vector, Games               *
 * vector, and Chat vector aren't used at all by this routine.  The Game & Players             *
 * list boxes are filled in manually in the processing loop.                                   *
 *                                                                                             *
 *    +------------------------------------------------------------+                           *
 *    |                        Serial Game                         |                           *
 *    |                                                            |                           *
 *    |                   Your Name: __________                    |                           *
 *    |                       House: [GDI] [NOD]                   |                           *
 *    |               Desired Color: [ ][ ][ ][ ]                  |                           *
 *    |                                                            |                           *
 *    |                     Opponent: Name                         |                           *
 *    |                     Scenario: Description                  |                           *
 *    |                      Credits: xxxx                         |                           *
 *    |                        Bases: ON                           |                           *
 *    |                       Crates: ON                           |                           *
 *    |                     Tiberium: ON                           |                           *
 *    |                       Ghosts: ON                           |                           *
 *    |                                                            |                           *
 *    |                         [Cancel]                           |                           *
 *    |                                                            |                           *
 *    |   +----------------------------------------------------+   |                           *
 *    |   |                                                    |   |                           *
 *    |   |                                                    |   |                           *
 *    |   +----------------------------------------------------+   |                           *
 *    |                       [Send Message]                       |                           *
 *    +------------------------------------------------------------+                           *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      true = success, false = cancel                                                         *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
bool Com_Show_Scenario_Dialog(void)
{
	return(ModemGuest().Dialog());
}	/* end of Com_Show_Scenario_Dialog */


/***************************************************************************
 * Phone_Dialog -- Lets user edit phone directory & dial                   *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      true = dial the current phone book entry, false = cancel.          *
 *                                                                         *
 * WARNINGS:                                                               *
 *      Serial options must have been read from CC.INI.                    *
 *                                                                         *
 * HISTORY:                                                                *
 *   04/29/1995 BRR : Created.                                             *
 *=========================================================================*/
static bool Phone_Dialog (void)
{
	return(PhoneEdit().Dialog());
}	/* end of Phone_Dialog */


/// <summary>
/// Dials the modem and waits for the other end to pick up.
/// This routine detects the modem, hangs up any call already in progress, then dials the
/// number the player chose from the phone book. If the modem will not talk at the requested
/// baud rate, the rate is raised and the detection tried once more. Whatever goes wrong is
/// reported to the player before returning.
/// </summary>
/// <param name="settings">The serial settings to dial with. The baud rate may be raised.</param>
/// <param name="reconnect">Is this an attempt to restore a dropped game connection?</param>
/// <returns>bool; Was a connection established?</returns>
static bool Dial_Modem( SerialSettingsType *settings, bool reconnect )
{
	bool connected = false;
	DialStatusType dialstatus;
	int modemstatus;

	DebugString("Dial Modem\n");

	/*
	**	Turn modem servicing off in the callback routine.
	*/
	Session.ModemService = false;

	// save for later to reconnect

	DialSettings = settings;

	modemstatus = NullModem.Get_Modem_Status();

	DebugString("Get_Modem_Status indicates the following...\n");
	DebugString("CTS: %s\n", (modemstatus & WinModemClass::CTS_SET) ? "SET" : "CLEAR");
	DebugString("DSR: %s\n", (modemstatus & WinModemClass::DSR_SET) ? "SET" : "CLEAR");
	DebugString(" RI: %s\n", (modemstatus & WinModemClass::RI_SET) ? "SET" : "CLEAR");
	DebugString(" CD: %s\n", (modemstatus & WinModemClass::CD_SET) ? "SET" : "CLEAR");

	if (reconnect) {
		if ( (modemstatus & WinModemClass::CD_SET) ) {
			connected = true;
			Session.ModemService = true;
			return( connected );
		}
	} else if ( (modemstatus & WinModemClass::CD_SET) ) {
		DebugString("Modem already on-line - hanging up\n");
		NullModem.Hangup_Modem();
		Session.ModemService = false;
	}

	NullModem.Setup_Modem_Echo( Modem_Echo );

	modemstatus = NullModem.Detect_Modem( settings, reconnect );
	if ( !modemstatus ) {
		DebugString("Detect_Modem failed\n");
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();

		/*
		**	If our first attempt to detect the modem failed, and we're at
		**	14400 or 28800, bump up to the next baud rate & try again.
		*/
		switch (settings->Baud) {
			case 14400:
				DebugString("Upshifting to 19200\n");
				settings->Baud = 19200;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo( Modem_Echo );
				modemstatus = NullModem.Detect_Modem( settings, reconnect );
				if ( !modemstatus ) {
					DebugString("Detect_Modem failed at 19200\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process( TXT_UNABLE_FIND_MODEM, TXT_OK );
					Session.ModemService = true;
					return( connected );
				}
				break;

			case 28800:
				DebugString("Upshifting to 38400\n");
				settings->Baud = 38400;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo( Modem_Echo );
				modemstatus = NullModem.Detect_Modem( settings, reconnect );
				if ( !modemstatus ) {
					DebugString("Detect_Modem failed at 38400\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process( TXT_UNABLE_FIND_MODEM, TXT_OK );
					Session.ModemService = true;
					return( connected );
				}
				break;

			default:
				WWMessageBox().Process( TXT_UNABLE_FIND_MODEM, TXT_OK );
				Session.ModemService = true;
				return( connected );

		}
	} else if ( modemstatus == -1 ) {
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();
		WWMessageBox().Process( TXT_ERROR_IN_INITSTRING, TXT_OK );
//		WWMessageBox().Process( "Error in the InitString." );
		Session.ModemService = true;
		return( connected );
	}

	DebugString("Ready to dial\n");

	dialstatus = NullModem.Dial_Modem( DialString, settings->DialMethod, reconnect );

	if (reconnect) {
		/*
		--------------------------- Redraw the display ---------------------------
		*/
		Map.Flag_To_Redraw(GS_REDRAW_ALL);
		Map.Render();
	}

	switch ( dialstatus ) {
		case DIAL_CONNECTED:
			DebugString("Dial_Modem returned 'DIAL_CONNECTED'\n");
			connected = true;
			break;

		case DIAL_NO_CARRIER:
			DebugString("Dial_Modem returned 'DIAL_NO_CARRIER'\n");
			WWMessageBox().Process(TXT_NO_CARRIER, TXT_OK);
			connected = false;
			break;

		case DIAL_BUSY:
			DebugString("Dial_Modem returned 'DIAL_BUSY'\n");
			WWMessageBox().Process(TXT_LINE_BUSY, TXT_OK);
			connected = false;
			break;

		case DIAL_ERROR:
			DebugString("Dial_Modem returned 'DIAL_ERROR'\n");
			WWMessageBox().Process(TXT_NUMBER_INVALID, TXT_OK);
			connected = false;
			break;

		case DIAL_TIMEOUT:
			DebugString("Dial_Modem returned 'DIAL_TIMEOUT'\n");
			WWMessageBox().Process(TXT_ERROR_TIMEOUT, TXT_OK);
			connected = false;
			break;

		case DIAL_NO_DIAL_TONE:
			DebugString("Dial_Modem returned 'DIAL_NO_DIAL_TONE'\n");
			WWMessageBox().Process(TXT_NO_DIAL_TONE, TXT_OK);
			connected = false;
			break;

		case DIAL_CANCELED:
			DebugString("Dial_Modem returned 'DIAL_CANCELLED'\n");
			NullModem.Hangup_Modem();
			Session.ModemService = false;
			WWMessageBox().Process(TXT_DIALING_CANCELED, TXT_OK);
			connected = false;
			break;
	}

	NullModem.Remove_Modem_Echo();
	NullModem.Print_EchoBuf();
	NullModem.Reset_EchoBuf();

	Session.ModemService = true;

	return( connected );

}	/* end of Dial_Modem */


/// <summary>
/// Waits for an incoming call on the modem.
/// This routine puts the modem into answer mode and blocks until a caller connects or the
/// player gives up. The modem is detected first, upshifting the baud rate if the settings
/// ask for a rate the modem will not talk at. Whatever goes wrong is reported to the player
/// before returning.
/// </summary>
/// <param name="settings">The serial settings to answer with. The baud rate may be raised.</param>
/// <param name="reconnect">Is this an attempt to restore a dropped game connection?</param>
/// <returns>bool; Was a connection established?</returns>
static bool Answer_Modem( SerialSettingsType *settings, bool reconnect )
{
	bool connected = false;

	DialStatusType dialstatus;
	int modemstatus;

	DebugString("Answer Modem\n");

	/*
	**	Turn modem servicing off in the callback routine.
	*/
	Session.ModemService = false;

	// save for later to reconnect

	DialSettings = settings;

	modemstatus = NullModem.Get_Modem_Status();

	DebugString("Get_Modem_Status indicates the following...\n");
	DebugString("CTS: %s\n", (modemstatus & WinModemClass::CTS_SET) ? "SET" : "CLEAR");
	DebugString("DSR: %s\n", (modemstatus & WinModemClass::DSR_SET) ? "SET" : "CLEAR");
	DebugString(" RI: %s\n", (modemstatus & WinModemClass::RI_SET) ? "SET" : "CLEAR");
	DebugString(" CD: %s\n", (modemstatus & WinModemClass::CD_SET) ? "SET" : "CLEAR");

	if (reconnect) {
		if ( (modemstatus & WinModemClass::CD_SET) ) {
			connected = true;
			Session.ModemService = true;
			return( connected );
		}
	} else if ( (modemstatus & WinModemClass::CD_SET) ) {
		DebugString("Modem already on-line - hanging up\n");
		NullModem.Hangup_Modem();
		Session.ModemService = false;
	}

	NullModem.Setup_Modem_Echo( Modem_Echo );

	modemstatus = NullModem.Detect_Modem( settings, reconnect );
	if ( !modemstatus ) {
		DebugString("Detect_Modem failed\n");
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();

		/*
		**	If our first attempt to detect the modem failed, and we're at
		**	14400 or 28800, bump up to the next baud rate & try again.
		*/
		switch (settings->Baud) {
			case 14400:
				DebugString("Upshifting to 19200\n");
				settings->Baud = 19200;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo( Modem_Echo );
				modemstatus = NullModem.Detect_Modem( settings, reconnect );
				if ( !modemstatus ) {
					DebugString("Detect_Modem failed at 19200\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process( TXT_UNABLE_FIND_MODEM, TXT_OK );
					Session.ModemService = true;
					return( connected );
				}
				break;

			case 28800:
				DebugString("Upshifting to 38400\n");
				settings->Baud = 38400;
				Shutdown_Modem();
				Init_Null_Modem(settings);
				NullModem.Setup_Modem_Echo( Modem_Echo );
				modemstatus = NullModem.Detect_Modem( settings, reconnect );
				if ( !modemstatus ) {
					DebugString("Detect_Modem failed at 38400\n");
					NullModem.Remove_Modem_Echo();
					NullModem.Print_EchoBuf();
					NullModem.Reset_EchoBuf();
					WWMessageBox().Process( TXT_UNABLE_FIND_MODEM, TXT_OK );
					Session.ModemService = true;
					return( connected );
				}
				break;

			default:
				WWMessageBox().Process( TXT_UNABLE_FIND_MODEM, TXT_OK );
				Session.ModemService = true;
				return( connected );

		}
	} else if ( modemstatus == -1 ) {
		NullModem.Remove_Modem_Echo();
		NullModem.Print_EchoBuf();
		NullModem.Reset_EchoBuf();
		WWMessageBox().Process( TXT_ERROR_IN_INITSTRING, TXT_OK );
		Session.ModemService = true;
		return( connected );
	}

	DebugString("Ready to answer\n");

	dialstatus = NullModem.Answer_Modem( reconnect );

	switch ( dialstatus ) {
		case DIAL_CONNECTED:
			DebugString("Answer_Modem returned 'DIAL_CONNECTED'\n");
			connected = true;
			break;

		case DIAL_NO_CARRIER:
			DebugString("Answer_Modem returned 'DIAL_NO_CARRIER'\n");
			WWMessageBox().Process(TXT_NO_CARRIER, TXT_OK);
			connected = false;
			break;

//		case DIAL_BUSY:
//			WWMessageBox().Process(TXT_LINE_BUSY);
//			connected = false;
//			break;

		case DIAL_ERROR:
			DebugString("Answer_Modem returned 'DIAL_ERROR'\n");
			WWMessageBox().Process(TXT_NUMBER_INVALID, TXT_OK);
			connected = false;
			break;

		case DIAL_TIMEOUT:
			DebugString("Answer_Modem returned 'DIAL_TIMEOUT'\n");
			WWMessageBox().Process(TXT_ERROR_TIMEOUT, TXT_OK);
			connected = false;
			break;

		case DIAL_CANCELED:
			DebugString("Answer_Modem returned 'DIAL_CANCELLED'\n");
			WWMessageBox().Process(TXT_ANSWERING_CANCELED, TXT_OK);
			connected = false;
			break;
	}

	NullModem.Remove_Modem_Echo();
	NullModem.Print_EchoBuf();
	NullModem.Reset_EchoBuf();

	Session.ModemService = true;

	return( connected );

}	/* end of Answer_Modem */


/// <summary>
/// Adds a character to the modem echo buffer.
/// This routine is handed to the modem class as its echo callback, so that everything the
/// modem says back during dialing or answering can be gathered up and printed if something
/// goes wrong. Characters arriving after the buffer is full are discarded.
/// </summary>
/// <param name="c">The character echoed by the modem.</param>
/*static*/ void Modem_Echo( char c )
{
	if (NullModem.EchoCount < (NullModem.EchoSize - 1) ) {
		*(NullModem.EchoBuf + NullModem.EchoCount) = c;
		*(NullModem.EchoBuf + NullModem.EchoCount + 1) = 0;
		NullModem.EchoCount++;
	}

}	/* end of Modem_Echo */
