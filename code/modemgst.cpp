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

#include "modemgst.h"

#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "lzostraw.h"
#include "mapgen.h"
#include "mplayer.h"
#include "msgbox.h"
#include "msgloop.h"
#include "netshare.h"
#include "nulldlg.h"
#include "nullmgr.h"
#include "ownrdraw.h"
#include "rules.h"
#include "scenario.h"
#include "sendfile.h"
#include "stimer.h"
#include "timer.h"
#include "trim.h"
#include "winstub.h"
#include "xstraw.h"


ModemGuest * _ModemGuest;
BOOL CALLBACK Modem_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);


/// <summary>
/// Handles the modem game setup dialog for the joining player.
/// This routine puts up the dialog and runs it until the host starts the game or one
/// side gives up. While it is up, the player's settings are sent across and the host's
/// options, chat and map preview are taken in.
/// </summary>
/// <returns>bool; Is a game about to start?</returns>
bool ModemGuest::Dialog(void)
{
	bool started = false;

	DebugString("ModemGuest.Dialog(enter)\n");

	RC = -1;
	_Dialog = OwnerDraw::Begin_Dialog(IDD_MODEM_GUEST, Modem_Guest_Dialog_Proc);

	if (_Dialog != NULL) {
		SetWindowLong(_Dialog, DWL_USER, (LONG)this);

		InitDialog();
		OwnerDraw::Display_Dialog(_Dialog);

		Changed = false;
		Transmit = true;
		TransmitTime = TickCount;
		TheirResponseTime = 10000;				// initialize to an invalid value
		LastMsgTime = TickCount;
		MsgTimeout = 20 * TIMER_SECOND;
		TimingTime = 0;
		TheirName[0] = '\0';
		TheirColor = -1;
		TheirHouse = HOUSE_NONE;

		CheatCheck = RulesClass::Get_Rule_Unique_ID();
		RulesClass::Load_Art_INI();
		ArtCheatCheck = RulesClass::Get_Art_Unique_ID();
		AICheatCheck = RulesClass::Get_AI_Unique_ID();

		while (RC < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Send(false);
			if (Receive() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(_Dialog);
		if (Changed == true) {
			Session.Write_MultiPlayer_Settings();
		}

		if (RC == 1) {
			started = true;
		} else if (RC == IDCANCEL) {
			started = false;
			SignOff();
		}
	}

	DebugString("ModemGuest.Dialog(exit %s)\n", started == true ? "true" : "false");
	return(started);
}


/// <summary>
/// Handles the Windows messages for the modem guest dialog.
/// Each message is offered to the owner draw layer first; this routine deals with what
/// is left over, passing the control notifications along to the ModemGuest object that
/// owns the dialog.
/// </summary>
/// <returns>Returns with nonzero if the owner draw layer claimed the message for itself.</returns>
BOOL CALLBACK Modem_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc == 0) {
		ModemGuest * _this = (ModemGuest *)GetWindowLong(window, DWL_USER);

		switch (message) {
			case WM_COMMAND: {
				switch (LOWORD(wparam)) {
					case IDC_MODEM_YOURSIDE:
						if (HIWORD(wparam) == CBN_SELCHANGE) {
							_this->SetHouse();
						}
						break;

					case IDC_NAME:
						if (HIWORD(wparam) == EN_MAXTEXT || HIWORD(wparam) == EN_CHANGE) {
							_this->SetName(HIWORD(wparam) == EN_CHANGE);
						}
						break;

					case IDC_INPUT:
						if (HIWORD(wparam) == EN_MAXTEXT) {
							_this->SendCurrentMessage();
						}
						break;

					case IDC_MODEM_YOURCOLOR:
						if (HIWORD(wparam) == CBN_SELCHANGE) {
							_this->SetColor();
						}
						break;

					case IDOK:
						if (HIWORD(wparam) == BN_CLICKED) {
							_this->AcceptOptions();
							//_this->RC = IDOK;
						}
						break;

					case IDCANCEL:
						if (HIWORD(wparam) == BN_CLICKED) {
							_this->ReadDialog();
							_this->RC = IDCANCEL;
						}
						break;
				}
			}
			break;

			case WM_DESTROY:
				if (MultiplayerMapPreview != NULL) {
					delete MultiplayerMapPreview;
					MultiplayerMapPreview = NULL;
				}
				break;

			case WM_PAINT:
				if (MultiplayerMapPreview != NULL) {
					MultiplayerMapPreview->Blit_Preview(window);
				}
				ValidateRect(window, NULL);
				break;
		}
		rc = 0;
	}

	return(rc);
}


/// <summary>
/// Fills in the modem guest dialog before it is shown.
/// This routine loads the name, color and side controls with the player's saved
/// preferences, shows the game options as they currently stand, and declares the game
/// open so that the host may talk to it.
/// </summary>
void ModemGuest::InitDialog(void)
{
	#define MP_MIN_MONEY 2500

	DebugString("ModemGuest.InitDialog(enter)\n");

	HWND handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		Edit_LimitText(handle, MPLAYER_NAME_MAX-1);
		Edit_SetText(handle, Session.Handle);
	}
	DebugString("Name: %s\n", Session.Handle);

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		ComboBox_ResetContent(handle);
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_GOLD));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_RED));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_BLUE));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_GREEN));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_ORANGE));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_SKY_BLUE));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_PURPLE));
		ComboBox_InsertString(handle, -1, Fetch_String(TXT_PINK));

		for (int i = 0; i < ARRAY_SIZE(PlayerColorTable); i++) {
			SendMessage(handle, OD_SETCOLOR, i, PlayerColorTable[i]);
		}

		Session.ColorIdx = Session.PrefColor;
		ComboBox_SetCurSel(handle, Session.PrefColor);
	}
	DebugString("Color: %d\n", Session.ColorIdx);

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		ComboBox_ResetContent(handle);
		for (int i = 0; i < HouseTypes.Count(); i++) {
			HouseTypeClass * house = HouseTypes[i];
			if (house->IsMultiplay) {
				int index = ComboBox_AddString(handle, house->GivenName);
				ComboBox_SetItemData(handle, index, i);
			}
		}
		int sel = ComboBox_Find_Item_Data(handle, -1, Session.House);
		ComboBox_SetCurSel(handle, sel);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Slider_SetRange(handle, SessionClass::CountMin[1], SessionClass::CountMax[1]);
		Slider_SetPos(handle, Session.Options.UnitCount);
	}
	DebugString("UnitCount: %d\n", Session.Options.UnitCount);

	handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		Slider_SetRange(handle, 1, MPLAYER_BUILD_LEVEL_MAX);
		Slider_SetPos(handle, BuildLevel);
	}
	DebugString("TechLevel: %d\n", BuildLevel);

	handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Slider_SetRange(handle, MP_MIN_MONEY, Rule->MPMaxMoney);
		Slider_SetPos(handle, Session.Options.Credits);
	}
	DebugString("Credits: %d\n", Session.Options.Credits);

	handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Slider_SetRange(handle, 0, 6);
		Slider_SetPos(handle, Session.Options.AIPlayers);
	}
	DebugString("AIPlayers: %d\n", Session.Options.AIPlayers);

	handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Slider_SetRange(handle, 0, 6);
		Slider_SetPos(handle, 6 - Session.Options.GameSpeed);
	}
	DebugString("Game Speed: %d\n", Session.Options.GameSpeed);

	handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Slider_SetRange(handle, 0, 2);
		Slider_SetPos(handle, Session.Options.AIDifficulty);
	}
	DebugString("AIDifficulty: %d\n", Session.Options.AIDifficulty);

	handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Button_SetCheck(handle, Session.Options.MCVRedeploy == true);
	}
	DebugString("MCVRedeploy: %s\n", Session.Options.MCVRedeploy == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Bases == true);
	}
	DebugString("Bases: %s\n", Session.Options.Bases == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Button_SetCheck(handle, Session.Options.ShortGame == true);
	}
	DebugString("Short Game: %s\n", Session.Options.ShortGame == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Button_SetCheck(handle, Session.Options.CrapEngineers == true);
	}
	DebugString("Crap Engineers: %s\n", Session.Options.CrapEngineers == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Goodies == true);
	}
	DebugString("Crates: %s\n", Session.Options.Goodies == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Button_SetCheck(handle, Session.Options.FogOfWar == true);
	}
	DebugString("FogOfWar: %s\n", Session.Options.FogOfWar == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Button_SetCheck(handle, Session.Options.BridgeDestruction == true);
	}
	DebugString("BridgeDestroy: %s\n", Session.Options.BridgeDestruction == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Button_SetCheck(handle, Session.Options.HarvTruce == true);
	}
	DebugString("Harvester Truce: %s\n", Session.Options.HarvTruce == true ? "ON" : "OFF");

	//------------------------------------------------------------------------
	// Init the version-clipping system
	//------------------------------------------------------------------------
	VerNum.Init_Clipping();

	if (MultiplayerMapPreview != NULL) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = NULL;
	}

	extern char ModemRXString[];

	if (strlen(ModemRXString) > 78) {
		ModemRXString[79] = 0;
	}

	if (strlen(ModemRXString) > 0) {
		DrawMessage(ModemRXString, -1, "");
	}

	ModemRXString[0] = '\0';

	NullModem.Reset_Response_Time(true);

	//------------------------------------------------------------------------
	// The game is now "open" for joining.  Close it as soon as we exit this
	// routine.
	//------------------------------------------------------------------------
	Session.NetOpen = 1;

	//------------------------------------------------------------------------
	// Clear the list of players
	//------------------------------------------------------------------------
	Clear_Vector(&Session.Players);

	Session.Options.ScenarioDescription[0] = '\0';

	DebugString("ModemGuest.InitDialog(exit)\n");
}


/// <summary>
/// Reads the player's choices back out of the dialog.
/// The name, color and side that the player settled on are copied into the session, so
/// that they can be saved and used when the game gets under way.
/// </summary>
void ModemGuest::ReadDialog(void)
{
	DebugString("ModemGuest.ReadDialog(enter)\n");

	HWND handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		Edit_GetText(handle, Session.Handle, sizeof(Session.Handle));
	}
	DebugString("Name: %s\n", Session.Handle);

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		Session.ColorIdx = ComboBox_GetCurSel(handle);
	}
	DebugString("Color: %d\n", Session.ColorIdx);

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int row = ComboBox_GetCurSel(handle);
		Session.House = ComboBox_GetItemData(handle, row);
	}

	DebugString("ModemGuest.ReadDialog(exit)\n");
}


/// <summary>
/// Sends whatever the player has typed into the chat box.
/// This routine is called when the player finishes a line of chat. The edit control is
/// emptied and the trimmed text goes out to the other machine.
/// </summary>
void ModemGuest::SendCurrentMessage(void)
{
	HWND handle;
	char buf[256];

	handle = GetDlgItem(_Dialog, IDC_INPUT);
	if (handle) {
		Edit_GetText(handle, buf, sizeof(buf) / 2);
		Edit_SetText(handle, "");
		if (strlen(buf) && buf[0] != '\r') {
			strtrim(buf);
			SendAMessage(buf);
		}
	}
}


/// <summary>
/// Records the side the player picked from the list.
/// The choice is flagged for transmission so that the host hears about it, and noted as
/// a settings change so that it will be remembered for the next game.
/// </summary>
void ModemGuest::SetHouse(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			Session.House = ComboBox_GetItemData(handle,i);
			Transmit = true;
			Changed = true;
		}
	}
}


/// <summary>
/// Records the color the player picked from the list.
/// If the choice clashes with the color the other player has already taken, the next
/// color along is used instead. The choice is flagged for transmission to the host.
/// </summary>
void ModemGuest::SetColor(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		int color = ComboBox_GetCurSel(handle);
		if (color != CB_ERR) {
			if (color == TheirColor) {
				color = TheirColor + 1;
				if (color > MAX_PLAYERS - 1) {
					color = 0;
				}
				ComboBox_SetCurSel(handle, color);
			}
			Session.ColorIdx = color;
			Transmit = true;
			Changed = true;
			SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
		}
	}
}


/// <summary>
/// Records the name the player typed into the dialog.
/// The name is flagged for transmission to the host and noted as a settings change, so
/// that it will be remembered for the next game.
/// </summary>
/// <param name="update">Is the player still typing? A finished name is trimmed and put
/// back into the control.</param>
void ModemGuest::SetName(bool update)
{
	char namebuf[20];
	HWND handle = GetDlgItem(_Dialog, IDC_NAME);
	if (handle) {
		Edit_GetText(handle, namebuf, sizeof(namebuf));
		strtrim(namebuf);
		if (!update) {
			Edit_SetText(handle, namebuf);
		}
		strcpy(Session.Handle, namebuf);
		Transmit = true;
		Changed = true;
	}
}


/// <summary>
/// Records the other player's name, color and side.
/// This routine is used when the host's game options come in. It refreshes the opponent
/// display and moves the local color choice out of the way if the two now clash.
/// </summary>
/// <param name="color">The player color index the other player has taken.</param>
/// <param name="house">The house index the other player has picked.</param>
void ModemGuest::SetOther(const char * name, int color, int house)
{
	HWND handle;
	char buf[80];

	strcpy(TheirName, name);
	TheirColor = color;

	handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			if (i == TheirColor) {
				SetColor();
			}
		}
	}

	TheirHouse = house;

	handle = GetDlgItem(_Dialog, IDC_MODEM_OPPONENT);
	if (handle) {
		sprintf(buf, "%s [%s]", name, (const char *)HouseTypes[house]->GivenName);
		SetWindowText(handle, buf);
		SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
	}
}


/// <summary>
/// Shows the host's game options in the dialog.
/// This routine is used when a fresh set of options arrives. Every control is brought up
/// to date, the map preview is rebuilt if need be, and the accept button is switched back
/// on so that the player can agree to the new terms.
/// </summary>
void ModemGuest::SetSettings(void)
{
	DebugString("Settings change\n");

	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Slider_SetPos(handle, Session.Options.UnitCount);
	}

	handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		Slider_SetPos(handle, BuildLevel);
	}

	handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Slider_SetPos(handle, Session.Options.Credits);
	}

	handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Slider_SetPos(handle, Session.Options.AIPlayers);
	}

	handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Slider_SetPos(handle, (6 - Session.Options.GameSpeed));
	}

	handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Slider_SetPos(handle, Session.Options.AIDifficulty);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Button_SetCheck(handle, Session.Options.MCVRedeploy == true);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Bases == true);
	}

	handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Button_SetCheck(handle, Session.Options.CrapEngineers == true);
	}

	handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Button_SetCheck(handle, Session.Options.ShortGame == true);
	}

	handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Goodies == true);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Button_SetCheck(handle, Session.Options.FogOfWar == true);
	}

	handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Button_SetCheck(handle, Session.Options.BridgeDestruction == true);
	}

	handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Button_SetCheck(handle, Session.Options.HarvTruce == true);
	}

	handle = GetDlgItem(_Dialog, IDC_SCENARIONAME);
	if (handle) {
		SetWindowText(handle, Session.Options.ScenarioDescription);
	}

	if (MultiplayerMapPreview == NULL || MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
		Update_Network_Dialog_Preview(_Dialog);
	}

	handle = GetDlgItem(_Dialog, IDOK);
	if (handle) {
		Button_Enable(handle, TRUE);
	}

	//.........................................................
	// Play a little sound effect
	//.........................................................
	Sound_Effect(Rule->OptionsChanged);
}


/// <summary>
/// Tells the host that the game options are agreeable.
/// The accept button is switched off until the host sends over a fresh set of options
/// for the player to consider.
/// </summary>
void ModemGuest::AcceptOptions(void)
{
	HWND handle = GetDlgItem(_Dialog, IDOK);
	if (handle) {
		Button_Enable(handle, FALSE);
	}

	SerialPacketType packet;
	memset ((void*)&packet, 0, sizeof (packet));
	packet.Command = SERIAL_ACCEPT_OPTIONS;
	NullModem.Send_Message (&packet, sizeof (packet), 1);
}


/// <summary>
/// Sends this player's settings to the host if they are due.
/// This routine is called from the dialog loop. It repeats the player's options whenever
/// they have changed or the last transmission has gone stale, and keeps the timing
/// packets flowing so that both sides can measure the link.
/// </summary>
/// <param name="force">Should the settings be sent whether or not they are due?</param>
void ModemGuest::Send(bool force)
{
	NullModem.Service();

	if ((Transmit == 1 && (TickCount - TransmitTime) > PACKET_RETRANS_TIME) || force == true) {
		SendGameOptions();
		TransmitTime = TickCount;
		Transmit = false;
	}

	if ((TickCount - TimingTime) > PACKET_TIMING_TIMEOUT) {
		SerialPacketType packet;
		memset (&packet, 0, sizeof(SerialPacketType));
		packet.Command = SERIAL_TIMING;
		packet.ScenarioInfo.ResponseTime = NullModem.Response_Time();
		packet.ID = Session.ModemType;

		NullModem.Send_Message (&packet, sizeof(packet), 0);
		TimingTime = TickCount;
	}
}


/// <summary>
/// Handles whatever the host has sent to this machine.
/// This routine is called from the dialog loop. It takes care of the host's game
/// options, chat, map preview and go signal, and puts up a message box when the other
/// machine signs off, proves incompatible, or simply stops answering.
/// </summary>
/// <returns>bool; Should the setup dialog be torn down?</returns>
bool ModemGuest::Receive(void)
{
	SerialPacketType receivepacket;
	SerialPacketType sendpacket;
	unsigned int version;
	unsigned int starttime;
	int packetlen;
	bool go = false;

	/*---------------------------------------------------------------------
	Check for an incoming message
	---------------------------------------------------------------------*/
	NullModem.Service();
	if (NullModem.Get_Message (&receivepacket, &packetlen) > 0) {

		LastMsgTime = TickCount;
		MsgTimeout = 20 * TIMER_SECOND;		/// reset timeout value to 20 seconds

		// are we getting our own packets back??

		if (receivepacket.Command >= SERIAL_CONNECT &&
			receivepacket.Command < SERIAL_LAST_COMMAND &&
			receivepacket.Command != SERIAL_MESSAGE &&
			receivepacket.ID == Session.ModemType) {

			DebugString("Received own packet back\n");

			// say we did receive sign off to keep from sending one
			WWMessageBox().Process (TXT_SYSTEM_NOT_RESPONDING, TXT_OK);

			return(true);
		}

		switch (receivepacket.Command) {
			/*..................................................................
			Other system signs off:  Give it time to receive my ACK, then show
			a message.
			..................................................................*/
			case (SERIAL_SIGN_OFF):
				DebugString("Received SignOff\n");
				starttime = TickCount;
				while (TickCount - starttime < TIMER_SECOND) {
					Call_Back();
					NullModem.Service();
				}

				WWMessageBox().Process(TXT_USER_SIGNED_OFF, TXT_OK);

				// to skip the other system not responding msg
				LastMsgTime = TickCount;

				Shutdown_Modem();
				return(true);

			/*..................................................................
			Game Options: Store all options; check my color & game version.
			..................................................................*/
			case (SERIAL_GAME_OPTIONS):
				DebugString("Received game options\n");

				//.........................................................
				// "Clip" the other system's version range to our own
				// ........................................................
				version = VerNum.Clip_Version(receivepacket.ScenarioInfo.MinVersion,
					receivepacket.ScenarioInfo.MaxVersion);
				// ........................................................
				// If the greatest-common-version comes back 0, the other
				// system's range is too low for ours
				// ........................................................
				if (version == 0) {
					WWMessageBox().Process (TXT_DESTGAME_OUTDATED, TXT_OK);

					// to skip the other system not responding msg
					LastMsgTime = TickCount;

					return(true);
				} else if (version == 0xffffffff) {
				// ........................................................
				// If the greatest-common-version comes back 0xffffffff,
				// the other system's range is too high for ours
				// ........................................................
					WWMessageBox().Process (TXT_YOURGAME_OUTDATED, TXT_OK);

					// to skip the other system not responding msg
					LastMsgTime = TickCount;

					return(true);
				} else {

					bool ok = true;

					if (receivepacket.ScenarioInfo.IsFirestorm == true && Addon_Enabled(ADDON_FIRESTORM) == false) {
						if (Addon_Installed(ADDON_FIRESTORM) == true) {
							WWMessageBox().Process (TXT_FIRESTORM_MUST_ENABLE, TXT_OK);

							// to skip the other system not responding msg
							LastMsgTime = TickCount;

							return(true);
						}

						WWMessageBox().Process (TXT_FIRESTORM_REQUIRED, TXT_OK);

						// to skip the other system not responding msg
						LastMsgTime = TickCount;

						return(true);
					}

					if (receivepacket.ScenarioInfo.IsFirestorm == false && Addon_Enabled(ADDON_FIRESTORM) == true) {
						WWMessageBox().Process (TXT_FIRESTORM_NO_JOIN_TS, TXT_OK);

						// to skip the other system not responding msg
						LastMsgTime = TickCount;

						return(true);
					}

					if (receivepacket.ScenarioInfo.CheatCheck != CheatCheck) {
						ok = false;
					}
					if (receivepacket.ScenarioInfo.AICheatCheck != AICheatCheck) {
						ok = false;
					}
					if (receivepacket.ScenarioInfo.ArtCheatCheck != ArtCheatCheck) {
						ok = false;
					}

					if (!ok) {

						WWMessageBox().Process (TXT_MISMATCH, TXT_OK);

						// to skip the other system not responding msg
						LastMsgTime = TickCount;

						return(true);
					}

					// ........................................................
					// Otherwise, 'version' is the highest version we have in
					// common; look up the protocol that goes with this version.
					// ........................................................
					Session.CommProtocol = VerNum.Version_Protocol(version);

					SetOther(receivepacket.Name, receivepacket.ScenarioInfo.Color, receivepacket.ScenarioInfo.House);
					if (SetGameOptions(&receivepacket) == true) {
						SetSettings();
					}
				}
				break;

			/*..................................................................
			GO: Exit this routine with a success code.
			..................................................................*/
			case (SERIAL_LOADGAME):
			case (SERIAL_GO):
				DebugString("Received go\n");
				go = false;

				/*
				**	If the scenario that the host wants to play doesnt exist locally then we
				**	need to request that it is sent. If we can identify the scenario locally then
				**	we need to fix up the file name so we load the right one.
				*/
				if (Find_Local_Scenario (Session.ScenarioFileName,
										Session.ScenarioFileLength,
										Session.ScenarioDigest,
										Session.ScenarioIsOfficial,
										true) == true) {

					/*
					**	We have the scenario. Tell the host that I am ready to go.
					*/
					memset ((void*)&sendpacket, 0, sizeof (sendpacket));
					sendpacket.Command = SERIAL_READY_TO_GO;
					NullModem.Send_Message (&sendpacket, sizeof (sendpacket), 1);
					go = true;
				} else {
					/*
					**	Oh dear. Thats a scenario I dont have. Request that the host sends the
					**	scenario to me provided it isnt an official Westwood scenario.
					**
					**	If the file is received OK then we will get a true return value and the
					**	actual file name to load will be in Session.ScenarioFileName
					*/
					if (Session.ScenarioIsOfficial && stricmp(Session.ScenarioFileName, RANDOM_MAP_FILE_NAME) != 0) {
						/*
						**	We dont have the scenario and we dont want to request that it gets
						**	sent because its an official one.
						**	Print up a message saying we cant play this scenario and reply to the
						**	host, telling him to select another.
						*/
						memset ((void*)&sendpacket, 0, sizeof (sendpacket));
						sendpacket.Command = SERIAL_NO_SCENARIO;
						NullModem.Send_Message (&sendpacket, sizeof (sendpacket), 1);
						Call_Back();
						NullModem.Service();
						WWMessageBox().Process (TXT_UNABLE_PLAY_WAAUGH, TXT_OK);
						LastMsgTime = TickCount;
						return(true);
					} else {
						if (!Get_File_From_Host(Session.ScenarioFileName, GAME_MODEM, false)) {
							return(true);
						} else {
							/*
							**	Make sure we dont time-out because of the download
							*/
							LastMsgTime = TickCount;
						}
					}
				}

				/*
				**	Fall through here...
				*/
				strcpy (Scen->ScenarioName, Session.ScenarioFileName);

				Session.FrameSendRate = 3;
				Session.LatencyFudge = 0;
				Session.PrecalcMaxAhead = 0;
				Session.PrecalcDesiredFrameRate = 0;

				//
				// calculated one way delay for a packet and overall delay
				// to execute a packet
				//
				if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
					Session.MaxAhead = MAX( ((receivepacket.ScenarioInfo.ResponseTime / 8 + 2) / 3) * 3, 6 );
				} else {
					Session.MaxAhead = receivepacket.ScenarioInfo.ResponseTime / 8;
					Session.MaxAhead = MAX( (int)(Session.MaxAhead), MODEM_MIN_MAX_AHEAD );
				}

				if (StartGame(receivepacket.Command == SERIAL_LOADGAME) == true) {
					RC = IDOK;
				} else {
					RC = IDCANCEL;
				}
				break;

			/*..................................................................
			Incoming message: add to our list
			..................................................................*/
			case (SERIAL_MESSAGE):
				DebugString("Received serial message\n");
				DrawMessage(receivepacket.Name, receivepacket.ID, receivepacket.Message.Message);
				Sound_Effect(Rule->IncomingMessage);
				break;

			//
			// throw away timing packet
			//
			case (SERIAL_TIMING):
				DebugString("Received timing packet\n");
				break;

			//
			// print msg waiting for opponent
			//
			case (SERIAL_SCORE_SCREEN):
				DebugString("Received score screen packet\n");
				Transmit = true;
				TransmitTime = TickCount;
				break;

			case SERIAL_PREVIEW_MODE:
				DebugString("Received SERIAL_PREVIEW_MODE packet\n");
				ReceivePreview();
				LastMsgTime = TickCount;
				break;

			default:
				DebugString("Received packet\n");
				break;
		}
	}

	// if we haven't received a msg for 20 seconds exit

	unsigned int time = TickCount - LastMsgTime;
	if (time > MsgTimeout) {
		DebugString("Timeout! %d ticks since last incoming message\n", time);

		WWMessageBox().Process (TXT_SYSTEM_NOT_RESPONDING, TXT_OK);

		// say we did receive sign off to keep from sending one
		return(true);
	}
	return(false);
}


/// <summary>
/// Adopts the game options the host has sent over.
/// The scenario and rule settings carried by the packet are copied into the session. If
/// the host has switched to a different scenario, its description is looked up and the
/// map preview is thrown away so that a fresh one gets built.
/// </summary>
/// <param name="packet">Pointer to the game options packet received from the host.</param>
/// <returns>bool; Did any of the options actually change?</returns>
bool ModemGuest::SetGameOptions(SerialPacketType * packet)
{
	if (Session.Options.Bases == (bool)packet->ScenarioInfo.IsBases &&
		Session.Options.MCVRedeploy == (bool)packet->ScenarioInfo.IsMCVRedeploy &&
		Session.Options.HarvTruce == (bool)packet->ScenarioInfo.IsHarvTruce &&
		Session.Options.BridgeDestruction == (bool)packet->ScenarioInfo.IsBridgeDestruction &&
		Session.Options.Goodies == (bool)packet->ScenarioInfo.IsGoodies &&
		Session.Options.FogOfWar == (bool)packet->ScenarioInfo.IsFogOfWar &&
		Session.Options.AIPlayers == packet->ScenarioInfo.AIPlayers &&
		Session.Options.AIDifficulty == packet->ScenarioInfo.AIDifficulty &&
		BuildLevel == packet->ScenarioInfo.BuildLevel &&
		Session.Options.Credits == (int)packet->ScenarioInfo.Credits &&
		Session.Options.UnitCount == packet->ScenarioInfo.UnitCount &&
		Seed == packet->ScenarioInfo.Seed &&
		Special == packet->ScenarioInfo.Special &&
		Session.Options.GameSpeed == packet->ScenarioInfo.GameSpeed &&
		Session.Options.ShortGame == (bool)packet->ScenarioInfo.IsShortGame &&
		Session.Options.CrapEngineers == (bool)packet->ScenarioInfo.IsCrapEngineers &&
		stricmp(Session.ScenarioFileName, packet->ScenarioInfo.ShortFileName) == 0 &&
		strcmp(Session.ScenarioDigest, (char const *)packet->ScenarioInfo.FileDigest) == 0 &&
		Session.ScenarioFileLength == packet->ScenarioInfo.FileLength) {

		return(false);
	}

	DebugString("Decoding game options\n");

	/*...............................................................
	Save scenario settings.
	...............................................................*/
	Session.Options.Bases = packet->ScenarioInfo.IsBases;
	Session.Options.MCVRedeploy = packet->ScenarioInfo.IsMCVRedeploy;
	Session.Options.BridgeDestruction = packet->ScenarioInfo.IsBridgeDestruction;
	Special.IsDestroyBridges = packet->ScenarioInfo.IsBridgeDestruction;
	Session.Options.Goodies = packet->ScenarioInfo.IsGoodies;
	Session.Options.FogOfWar = packet->ScenarioInfo.IsFogOfWar;
	Session.Options.HarvTruce = packet->ScenarioInfo.IsHarvTruce;
	Session.Options.AIPlayers = packet->ScenarioInfo.AIPlayers;
	Session.Options.AIDifficulty = (DiffType)packet->ScenarioInfo.AIDifficulty;
	BuildLevel = packet->ScenarioInfo.BuildLevel;
	Session.Options.Credits = packet->ScenarioInfo.Credits;
	Session.Options.UnitCount = packet->ScenarioInfo.UnitCount;
	Seed = packet->ScenarioInfo.Seed;
	Special = packet->ScenarioInfo.Special;
	Options.GameSpeed = Session.Options.GameSpeed = packet->ScenarioInfo.GameSpeed;
	Session.Options.ShortGame = packet->ScenarioInfo.IsShortGame;
	Session.Options.CrapEngineers = packet->ScenarioInfo.IsCrapEngineers;
	Special.IsTGrowth = true;
	Special.IsTSpread = true;
	Scen->Special.IsAllianceFixed = false;
	Session.Options.AlliesAllowed = true;
	Special.Apply_To_Game();

	/*
	**	If the scenario name changed then we need to redraw the whole lot.
	*/
	if (stricmp(Session.ScenarioFileName, packet->ScenarioInfo.ShortFileName) != 0 ||
		stricmp(Session.ScenarioDigest,(char const *)(packet->ScenarioInfo.FileDigest)) != 0 ||
		Session.ScenarioIsOfficial != (bool)packet->ScenarioInfo.OfficialScenario ||
		Session.ScenarioFileLength != packet->ScenarioInfo.FileLength ||
		Session.Options.ScenarioDescription[0] == '\0') {

		DebugString("Scenario has changed\n");

		/*...............................................................
		Copy the information about the scenario that the host wants to
		play so ee can request this scenario from the host if we don't
		have it locally.
		...............................................................*/
		strcpy(Session.ScenarioFileName, packet->ScenarioInfo.ShortFileName);
		strcpy(Session.ScenarioDigest, (char const *)packet->ScenarioInfo.FileDigest);
		Session.ScenarioIsOfficial = packet->ScenarioInfo.OfficialScenario;
		Session.ScenarioFileLength = packet->ScenarioInfo.FileLength;

		bool found = false;
		if (strcmp(Session.ScenarioFileName, RANDOM_MAP_FILE_NAME) != 0 || !Session.ScenarioIsOfficial) {
			for (int i = 0; i < Session.Scenarios.Count(); i++) {
				MultiMission * mm = Session.Scenarios[i];
				if (stricmp(Session.ScenarioFileName, mm->Get_Filename()) == 0) {
					strcpy(Session.Options.ScenarioDescription, mm->Description());
					found = true;
					break;
				}
			}
		}

		if (!found) {
			strcpy(Session.Options.ScenarioDescription, packet->ScenarioInfo.Scenario);
		}

		if (MultiplayerMapPreview != NULL) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = NULL;
		}
	}

	return(true);
}


/// <summary>
/// Sends this player's settings over to the host.
/// The packet carries the player's name, color and side along with the version range and
/// the rules checksums that the host uses to spot a mismatched installation.
/// </summary>
void ModemGuest::SendGameOptions(void)
{
	SerialPacketType packet;
	memset (&packet, 0, sizeof(SerialPacketType));
	packet.Command = SERIAL_GAME_OPTIONS;
	packet.ScenarioInfo.CheatCheck = CheatCheck;
	packet.ScenarioInfo.ArtCheatCheck = ArtCheatCheck;
	packet.ScenarioInfo.AICheatCheck = AICheatCheck;
	packet.ScenarioInfo.IsFirestorm = Addon_Enabled(ADDON_FIRESTORM);
	packet.ScenarioInfo.BuildNumber = Build_Number();
	packet.ScenarioInfo.MinVersion = VerNum.Min_Version();
	packet.ScenarioInfo.MaxVersion = VerNum.Max_Version();
	packet.ID = Session.ModemType;
	strcpy(packet.Name, Session.Handle);
	packet.ScenarioInfo.House = (HousesType)Session.House;
	packet.ScenarioInfo.Color = Session.ColorIdx;

	NullModem.Send_Message(&packet, sizeof(packet), 1);
}


/// <summary>
/// Sends a chat message over to the other player.
/// The message is added to this machine's own message list as well, so that the player
/// can see what was said.
/// </summary>
/// <param name="message">The text to send. Nothing is sent if this is NULL.</param>
void ModemGuest::SendAMessage(const char * message)
{
	if (message != NULL) {
		SerialPacketType packet;
		memset (&packet, 0, sizeof(SerialPacketType));
		packet.Command = SERIAL_MESSAGE;
		strcpy (packet.Name, Session.Handle);
		packet.ID = Session.ColorIdx;
		strcpy (packet.Message.Message, message);

		/*..................................................................
		Send the message
		..................................................................*/
		NullModem.Send_Message (&packet, sizeof(packet), 1);
		NullModem.Service();

		/*..................................................................
		Add the message to our own screen
		..................................................................*/
		DrawMessage(Session.Handle, Session.ColorIdx, message);
	}
}


/// <summary>
/// Adds a chat message to the dialog's message list.
/// The line is drawn in the sender's player color, or in the default color when the
/// sender has no color of their own.
/// </summary>
/// <param name="color">The player color index to draw the line in.</param>
void ModemGuest::DrawMessage(const char * name, int color, const char * message)
{
	HWND handle;
	char buf[256];

	if (message != NULL) {
		handle = GetDlgItem(_Dialog, IDC_PMESSAGES);
		if (handle) {
			COLORREF c = (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe);
			sprintf(buf, "[%s] %s", name, message);
			_DrawMessage(c, buf, handle);
		}
	}
}


/// <summary>
/// Tells the host that this player is leaving.
/// A sign off packet is sent and this routine waits, with a notice up on the screen, for
/// the send queue to drain before the modem is shut down.
/// </summary>
void ModemGuest::SignOff(void)
{
	unsigned int starttime;
	int packetlen;
	SerialPacketType sendpacket;
	SerialPacketType receivepacket;

	memset (&sendpacket, 0, sizeof(SerialPacketType));
	sendpacket.Command = SERIAL_SIGN_OFF;
	sendpacket.ScenarioInfo.Color = Session.ColorIdx;		// use Color for ID
	sendpacket.ID = Session.ModemType;
	NullModem.Send_Message(&sendpacket, sizeof(sendpacket), 1);

	HWND dialog = OwnerDraw::Custom_Message_Box(Fetch_String(TXT_SIGNING_OFF), NULL, NULL);

	if (dialog) {
		OwnerDraw::Display_Dialog(dialog);
	}

	starttime = TickCount;
	while ( NullModem.Num_Send()
		&& ((TickCount - starttime) < SIGNOFF_PACKET_CANCEL_TIMEOUT) ) {
#ifdef _DEBUG
		NullModem.Mono_Debug_Print(0);
#endif

		if ( NullModem.Get_Message( &receivepacket, &packetlen ) > 0) {

			// are we getting our own packets back??

			if (receivepacket.Command == SERIAL_SIGN_OFF
				&& receivepacket.ID == Session.ModemType) {

				// exit while
				break;
			}
		}

		Call_Back();
		Windows_Message_Handler();
		NullModem.Service();
	}
	Shutdown_Modem();
	if (dialog) {
		OwnerDraw::End_Dialog(dialog);
	}
}


/// <summary>
/// Prepares to start the game the host has called for.
/// Both players are added to the player list and the agreed scenario settings are applied
/// to the game. This routine waits for the acknowledgement to reach the host before it
/// allows play to begin. A request to resume a saved game is refused.
/// </summary>
/// <param name="load_game">Is the host resuming a saved game rather than starting a
/// fresh one?</param>
/// <returns>bool; May the game go ahead?</returns>
bool ModemGuest::StartGame(bool load_game)
{
	unsigned int starttime;
	NodeNameType * who;					// node to add to Players

	if (load_game == true) {
		return(false);
	}

	ReadDialog();

	/*------------------------------------------------------------------------
	Prepare to load the scenario
	------------------------------------------------------------------------*/
	Session.NumPlayers = 0;

	/*.....................................................................
	Add both players to the Players vector; the local system is always
	index 0.
	.....................................................................*/
	who = new NodeNameType;
	if (who != NULL) {
		strcpy(who->Name, Session.Handle);
		who->Player.House = Session.House;
		who->Player.Color = Session.ColorIdx;
		who->Player.ProcessTime = -1;
		Session.Players.Add (who);
		Session.NumPlayers++;
	}

	who = new NodeNameType;
	if (who != NULL) {
		strcpy(who->Name, TheirName);
		who->Player.House = TheirHouse;
		who->Player.Color = TheirColor;
		who->Player.ProcessTime = -1;
		Session.Players.Add (who);
		Session.NumPlayers++;
	}

	if (Session.NumPlayers != 2) {
		return(false);
	}

	HWND msgbox = OwnerDraw::Custom_Message_Box(Fetch_String(TXT_SENDING_GO_ACK));
	if (msgbox) {
		OwnerDraw::Display_Dialog(msgbox);
	}
	starttime = TickCount;
	while ( ( NullModem.Num_Send()
		&& ((TickCount - starttime) < PACKET_SENDING_TIMEOUT) )
		|| ((TickCount - starttime) < TIMER_SECOND) ) {

		Call_Back();
		Windows_Message_Handler();
		NullModem.Service();
	}
	if (msgbox) {
		OwnerDraw::End_Dialog(msgbox);
	}

	// clear queue to keep from doing any resends
	NullModem.Init_Send_Queue();

	Session.NetOpen = false;
	Scen->Scenario = Session.Options.ScenarioIndex;
	Session.NumPlayers = Session.Players.Count();
	Frame = 0;
	Session.CommProtocol = DEFAULT_COMM_PROTOCOL;
	Special.IsHarvesterImmune = Session.Options.HarvTruce;
	Special.IsDestroyBridges = Session.Options.BridgeDestruction;
	Special.IsTGrowth = true;
	Special.IsTSpread = true;
	Special.Apply_To_Game();
	return(true);
}


/// <summary>
/// Receives the map preview picture from the host.
/// This routine acknowledges the host's offer, takes delivery of the compressed preview
/// file, decompresses it and hands the result to a fresh preview object so that the
/// dialog can show off the map the host has chosen.
/// </summary>
void ModemGuest::ReceivePreview(void)
{
	CDTimerClass<SystemTimerClass> response_timer;

	DebugString("Starting map preview download\n");

	SerialPacketType packet;
	memset(&packet, 0, sizeof(packet));
	packet.Command = SERIAL_PREVIEW_ACK;

	DebugString("Sending preview mode ack\n");
	NullModem.Send_Message(&packet, sizeof(packet), 1);
	response_timer = 10 * TIMER_SECOND;
	while (NullModem.Num_Send() > 0 && response_timer) {
		Call_Back();
	}
	DebugString("Preview mode ack sent\n");

	DebugString("Calling Get_File_From_Host to receive the file download\n");
	char preview_name[256];
	bool got_file = Get_File_From_Host(preview_name, GAME_MODEM, false);
	if (!got_file) {
		DebugString("got_file is false. Download failed\n");
		return;
	}

	DebugString("Loading the compressed preview image\n");
	RawFileClass file(preview_name);
	int size = file.Size();
	char * buffer = new char[size];
	file.Read(buffer, size);
	int preview_size = ((int *)buffer)[0];

	DebugString("Decompressing the preview image\n");

	BufferStraw bstraw(&((int *)buffer)[1], size);
	LZOStraw lzostraw(LZOStraw::DECOMPRESS);
	lzostraw.Get_From(&bstraw);
	char * preview = new char[2 * preview_size];
	lzostraw.Get(preview, preview_size);

	DebugString("Creating the new preview surface\n");
	if (MultiplayerMapPreview) {
		delete MultiplayerMapPreview;
	}
	MultiplayerMapPreview = new MapPreviewClass;
	MultiplayerMapPreview->Create_Preview_Surface(preview);
	InvalidateRect(_Dialog, NULL, FALSE);

	DebugString("Cleaning up the temporary decompression buffers\n");
	delete [] preview;
	delete [] buffer;
}
