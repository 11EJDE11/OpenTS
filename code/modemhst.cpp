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

#include "modemhst.h"

#include "_rules.h"
#include "_timer.h"
#include "addon.h"
#include "ccrand.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "goptions.h"
#include "houstype.h"
#include "init.h"
#include "language\language.h"
#include "lzopipe.h"
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
#include "xpipe.h"


ModemHost * _ModemHost;
BOOL CALLBACK Modem_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);


/// <summary>
/// Runs the modem host dialog.
/// This routine drives the dialog for the machine hosting a modem game, servicing the
/// link and dealing with the guest's packets in between window messages. It stays up
/// until the player backs out or a game successfully starts, and the multiplayer
/// settings are written back out if anything was changed along the way.
/// </summary>
/// <returns>bool; Was a game started?</returns>
bool ModemHost::Dialog(void)
{
	bool started = false;

	DebugString("ModemHost.Dialog(enter)\n");

	RC = -1;
	_Dialog = OwnerDraw::Begin_Dialog(IDD_MODEM_HOST, Modem_Host_Dialog_Proc);

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
		GameOptions = false;
		TheirName[0] = '\0';
		TheirColor = -1;
		TheirHouse = HOUSE_NONE;
		_ModemHost = this;

		CheatCheck = RulesClass::Get_Rule_Unique_ID();
		RulesClass::Load_Art_INI();
		ArtCheatCheck = RulesClass::Get_Art_Unique_ID();
		AICheatCheck = RulesClass::Get_AI_Unique_ID();

		while (true) {
			RC = -1;
			while (RC < 0) {
				if (OwnerDraw::Dialog_Message_Handler() == true) {
					break;
				}
				Send(false);
				if (Receive(false) == true) {
					break;
				}
				Title_Screen_Restore();
			}

			if (RC != IDOK && RC != IDC_LOAD_MP_GAME) { /// The load game button is not present in the shipped dialog.
				break;
			}
			ShowWindow(_Dialog, SW_HIDE);
			if (StartGame(RC == IDC_LOAD_MP_GAME) == true) {
				started = true;
				break;
			}
			ShowWindow(_Dialog, SW_SHOW);
		}

		OwnerDraw::End_Dialog(_Dialog);
		_ModemHost = NULL;
		if (Changed == true) {
			Session.Write_MultiPlayer_Settings();
		}
	}

	if (RC == IDCANCEL) {
		SignOff();
	}

	DebugString("ModemHost.Dialog(exit %s)\n", started == true ? "true" : "false");
	return(started);
}


/// <summary>
/// Handles the messages for the modem host dialog.
/// This routine gives the owner draw system first refusal on every message and passes
/// whatever is left to the ModemHost object attached to the window, where each control
/// notification lands on the option routine that owns it.
/// </summary>
/// <returns>Returns with the owner draw system's result, or zero when the message was
/// left to this routine to deal with.</returns>
BOOL CALLBACK Modem_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);

	if (rc == 0) {
		ModemHost * _this = (ModemHost *)GetWindowLong(window, DWL_USER);

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

					case IDC_MODEM_REDEPLOY_MCV:
						_this->SetMCVRedeploy();
						break;

					case IDC_MODEM_BASES:
						_this->SetBases();
						break;

					case IDC_BRIDGE_DESTROY:
						_this->SetBridgeDestruction();
						break;

					case IDC_HARVTRUCE:
						_this->SetHarvTruce();
						break;

					case IDC_MULTIMAP:
						if (HIWORD(wparam) == 0) {
							_this->SelectMultiMap();
						}
						break;

					case IDC_LOAD_MP_GAME:
						if (HIWORD(wparam) == 0) {
							_this->ReadDialog();
							_this->SendGameOptions();
							_this->RC = IDC_LOAD_MP_GAME;
						}
						break;

					case IDC_MODEM_FOG:
						_this->SetFogOfWar();
						break;

					case IDC_SHORT_GAME:
						_this->SetShortGame();
						break;

					case IDC_MULTI_ENGINEER:
						_this->SetCrapEngineers();
						break;

					case IDC_GOODIES:
						_this->SetGoodies();
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
							if (_this->ReadDialog() == true) {
								_this->SendGameOptions();
								_this->RC = IDOK;
							}
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

			case WM_HSCROLL:
			case WM_VSCROLL:
				if (GetDlgItem(window, IDC_MODEM_UNITCOUNT) == (HWND)lparam) {
					_this->SetUnitCount();
				} else if (GetDlgItem(window, IDC_TECHLEVEL) == (HWND)lparam) {
					_this->SetBuildLevel();
				} else if (GetDlgItem(window, IDC_CREDITS) == (HWND)lparam) {
					_this->SetCredits();
				} else if (GetDlgItem(window, IDC_AIPLAYERS) == (HWND)lparam) {
					_this->SetAIPlayers();
				} else if (GetDlgItem(window, IDC_AILEVEL_SLIDER) == (HWND)lparam) {
					_this->SetAIDifficulty();
				} else if (GetDlgItem(window, IDC_GAME_SPEED_SLIDER) == (HWND)lparam) {
					_this->SetGameSpeed();
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
/// Prepares the host dialog for display.
/// This routine fills the color and side lists, sets every slider and check box from the
/// current session options, empties the player list, selects the opening scenario and
/// puts its preview on screen. The game is marked as open for joining on the way out, so
/// a guest dialing in can find it.
/// </summary>
void ModemHost::InitDialog(void)
{
	#define MP_MIN_MONEY 2500

	//------------------------------------------------------------------------
	// Dialog variables
	//------------------------------------------------------------------------
	static bool first_time = true;	// 1 = 1st time this dialog is run

	DebugString("ModemHost.InitDialog(enter)\n");

	//------------------------------------------------------------------------
	// Init dialog values, only the first time through
	//------------------------------------------------------------------------
	if (first_time == true) {
		first_time = false;
		Special.Initialize();
		Session.Options.Credits = Rule->MPMoney;
		Session.Options.Bases = Rule->IsMPBasesOn;
		Session.Options.BridgeDestruction = Rule->IsMPBridgeDestruction;
		Session.Options.Goodies = Rule->IsMPCrates;
		Session.Options.AIPlayers = 0;
		Session.Options.ShortGame = false;
		Session.Options.UnitCount = SessionClass::CountMax[Session.Options.Bases];
		Session.Options.GameSpeed = Options.GameSpeed;
		Session.Options.CrapEngineers = false;
	}

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
		SetSliderRangeAndPos(handle, SessionClass::CountMin[1], SessionClass::CountMax[1], Session.Options.UnitCount);
	}
	DebugString("UnitCount: %d\n", Session.Options.UnitCount);

	handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		SetSliderRangeAndPos(handle, 1, MPLAYER_BUILD_LEVEL_MAX, BuildLevel);
	}
	DebugString("TechLevel: %d\n", BuildLevel);

	handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		SetSliderRangeAndPos(handle, MP_MIN_MONEY, Rule->MPMaxMoney, Session.Options.Credits);
		SendMessage(handle, OD_SETTRACKSTEP, 0, 250);
	}
	DebugString("Credits: %d\n", Session.Options.Credits);

	handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		SetSliderRangeAndPos(handle, 0, 6, Session.Options.AIPlayers);
	}
	DebugString("AIPlayers: %d\n", Session.Options.AIPlayers);

	handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Slider_SetRange(handle, 0, 6);
		Slider_SetPos(handle, 6 - Options.GameSpeed);
	}
	DebugString("Game Speed: %d\n", Session.Options.GameSpeed);

	handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		SetSliderRangeAndPos(handle, 0, 2, Session.Options.AIDifficulty);
	}
	DebugString("AIDifficulty: %d\n", Session.Options.AIDifficulty);

	handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Button_SetCheck(handle, Session.Options.MCVRedeploy == true);
	}
	DebugString("MCV Redeploy: %s\n", Session.Options.MCVRedeploy == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Button_SetCheck(handle, Session.Options.Bases == true);
	}
	DebugString("Bases: %s\n", Session.Options.Bases == true ? "ON" : "OFF");

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

	//------------------------------------------------------------------------
	// Init the version-clipping system
	//------------------------------------------------------------------------
	VerNum.Init_Clipping();

	//------------------------------------------------------------------------
	// Init random-number generator, & create a seed to be used for all random
	// numbers from here on out
	//------------------------------------------------------------------------
	srand(Sim_Random_Pick(1, 0x7FFF));
	Seed = rand();

	Special.IsCaptureTheFlag = Rule->IsMPCaptureTheFlag;

	if (MultiplayerMapPreview != NULL) {
		delete MultiplayerMapPreview;
		MultiplayerMapPreview = NULL;
	}

	//------------------------------------------------------------------------
	// Clear the list of players
	//------------------------------------------------------------------------
	Clear_Vector(&Session.Players);

	Set_Scenario_Info_From_Index(0);
	Session.Options.ScenarioIndex = 0;	// 1st scenario is selected
	SendDlgItemMessage(_Dialog, IDC_SCENARIONAME, WM_SETTEXT, 0, (LPARAM)Session.Options.ScenarioDescription);

	//------------------------------------------------------------------------
	// The game is now "open" for joining.  Close it as soon as we exit this
	// routine.
	//------------------------------------------------------------------------
	Session.NetOpen = 1;

	if (CCFileClass(Session.ScenarioFileName).Is_Available()) {
		MultiplayerMapPreview = new MapPreviewClass;
		MultiplayerMapPreview->Read_INI_Preview(Session.ScenarioFileName);
		PostMessage(_Dialog, WM_PAINT, 0, 0);
	}

	extern char ModemRXString[];

	if (strlen(ModemRXString) > 78) {
		ModemRXString[79] = 0;
	}

	if (strlen(ModemRXString) > 0) {
		DrawMessage(ModemRXString, -1, "");
	}

	ModemRXString[0] = '\0';

	HWND window = GetDlgItem(_Dialog, 1);
	if (window) {
		EnableWindow(window, FALSE);
	}

	NullModem.Reset_Response_Time(true);

	DebugString("ModemHost.InitDialog(exit)\n");
}


/// <summary>
/// Reads every game option out of the dialog controls.
/// This routine is used when the host is about to commit -- either to start the game or
/// to back out of it -- and copies the state of each control into the session options.
/// The scenario is vetted first; a map with too few starting points for the number of
/// players requested is refused with a message box.
/// </summary>
/// <returns>bool; Were the options accepted?</returns>
bool ModemHost::ReadDialog(void)
{
	DebugString("ModemHost.ReadDialog(enter)\n");

	int waypoint_count = RandomMapWaypointCount(Session.Options.ScenarioIndex);

	int player_count = 2;
	HWND handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		player_count += Slider_GetPos(handle);
	}

	if (waypoint_count < player_count) {
		char buffer[256];
		sprintf(buffer, Fetch_String(TXT_SCENARIO_TOO_SMALL), waypoint_count);
		WWMessageBox().Process(buffer, TXT_OK, TXT_NONE);
		return(false);
	}

	handle = GetDlgItem(_Dialog, IDC_NAME);
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
		int i = ComboBox_GetCurSel(handle);
		Session.House = ComboBox_GetItemData(handle, i);
	}

	handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Session.Options.UnitCount = Slider_GetPos(handle);
	}
	DebugString("UnitCount: %d\n", Session.Options.UnitCount);

	handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		BuildLevel = Slider_GetPos(handle);
	}
	DebugString("TechLevel: %d\n", BuildLevel);

	handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Session.Options.Credits = Slider_GetPos(handle);
	}
	DebugString("Credits: %d\n", Session.Options.Credits);

	handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Session.Options.AIPlayers = Slider_GetPos(handle);
	}
	DebugString("AIPlayers: %d\n", Session.Options.AIPlayers);

	handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Session.Options.GameSpeed = 6 - Slider_GetPos(handle);
	}
	DebugString("Game Speed: %d\n", Session.Options.GameSpeed);

	handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Session.Options.AIDifficulty = (DiffType)Slider_GetPos(handle);
	}
	DebugString("AIDifficulty: %d\n", Session.Options.AIDifficulty);

	handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Session.Options.MCVRedeploy = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("MCVRedeploy: %s\n", Session.Options.MCVRedeploy == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Session.Options.Bases = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Bases: %s\n", Session.Options.Bases == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Session.Options.Goodies = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Crates: %s\n", Session.Options.Goodies == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Session.Options.FogOfWar = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("FogOfWar: %s\n", Session.Options.FogOfWar == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Session.Options.BridgeDestruction = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("BridgeDestroy: %s\n", Session.Options.BridgeDestruction == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Session.Options.HarvTruce = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Harvester Truce: %s\n", Session.Options.HarvTruce == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Session.Options.ShortGame = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Short Game: %s\n", Session.Options.ShortGame == true ? "ON" : "OFF");

	handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Session.Options.CrapEngineers = Button_GetCheck(handle) == BST_CHECKED;
	}
	DebugString("Crap Engineers: %s\n", Session.Options.CrapEngineers == true ? "ON" : "OFF");

	DebugString("ModemHost.ReadDialog(exit)\n");
	return(true);
}


/// <summary>
/// Sends whatever the player has typed on the input line.
/// This routine is called when the input line is committed. The text is trimmed and
/// transmitted, and the line is cleared ready for the next message.
/// </summary>
void ModemHost::SendCurrentMessage(void)
{
	HWND handle;
	char buf[256];

	memset(buf, 0, sizeof(buf));

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
/// Reads the player's house from its drop down list.
/// This routine is called when the player picks a side, and flags the game options for
/// retransmission to the guest.
/// </summary>
void ModemHost::SetHouse(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURSIDE);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			i = ComboBox_GetItemData(handle,i);
			if (i != CB_ERR) {
				Session.House = i;
				Transmit = true;
				Changed = true;
			}
		}
	}
}


/// <summary>
/// Reads the player's color from its drop down list.
/// This routine is called when the player picks a color. The list itself is recolored
/// to show the choice, and the game options are flagged for retransmission to the guest.
/// </summary>
void ModemHost::SetColor(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_YOURCOLOR);
	if (handle) {
		int i = ComboBox_GetCurSel(handle);
		if (i != CB_ERR) {
			Session.ColorIdx = i;
			Transmit = true;
			Changed = true;
			SendMessage(handle, OD_SETCOLOR, 0, (i >= 0 && i < MAX_PLAYERS ? PlayerColorTable[i] : ColorMe));
		}
	}
}


/// <summary>
/// Reads the player's handle from the edit control.
/// This routine is called as the name is typed, and flags the game options for
/// retransmission to the guest.
/// </summary>
/// <param name="update">Is the player still typing? The trimmed name is not echoed back
/// into the control while he is.</param>
void ModemHost::SetName(bool update)
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
/// Records the guest's name, color and house.
/// This routine is called when a game options packet arrives from the other machine.
/// Whenever any of the three details actually change, the opponent's line in the dialog
/// is rewritten and a sound is played, so the host notices the guest fiddling with his
/// settings.
/// </summary>
/// <param name="name">The guest's player name.</param>
/// <param name="color">The guest's player color index.</param>
/// <param name="house">The house the guest has chosen to play.</param>
void ModemHost::SetOther(const char * name, int color, int house)
{
	if (stricmp(TheirName, name) != 0 || TheirColor != color || TheirHouse != house) {
		HWND handle;
		char buf[80];

		strcpy(TheirName, name);
		TheirColor = color;
		TheirHouse = house;

		handle = GetDlgItem(_Dialog, IDC_MODEM_OPPONENT);
		if (handle) {
			sprintf(buf, "%s [%s]", name, (const char *)HouseTypes[house]->GivenName);
			SetWindowText(handle, buf);
			SendMessage(handle, OD_SETCOLOR, 0, (color >= 0 && color < MAX_PLAYERS ? PlayerColorTable[color] : ColorMe));
		}

		Sound_Effect(Rule->OptionsChanged);
	}

	Transmit = true;
}


/// <summary>
/// Reads the starting unit count from its slider.
/// This routine is called as the player drags the control, and flags the game options
/// for retransmission to the guest.
/// </summary>
void ModemHost::SetUnitCount(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_UNITCOUNT);
	if (handle) {
		Session.Options.UnitCount = Slider_GetPos(handle);
		Transmit = true;
	}
}


/// <summary>
/// Reads the tech level from its slider.
/// This routine is called as the player drags the control, and flags the game options
/// for retransmission to the guest.
/// </summary>
void ModemHost::SetBuildLevel(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_TECHLEVEL);
	if (handle) {
		BuildLevel = Slider_GetPos(handle);
		Transmit = true;
	}
}


/// <summary>
/// Reads the starting credits from its slider.
/// This routine is called as the player drags the control, and flags the game options
/// for retransmission to the guest.
/// </summary>
void ModemHost::SetCredits(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_CREDITS);
	if (handle) {
		Session.Options.Credits = Slider_GetPos(handle);
		Transmit = true;
	}
}


/// <summary>
/// Reads the number of computer players from its slider.
/// This routine is called as the player drags the control, and flags the game options
/// for retransmission to the guest.
/// </summary>
void ModemHost::SetAIPlayers(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_AIPLAYERS);
	if (handle) {
		Session.Options.AIPlayers = Slider_GetPos(handle);
		Transmit = true;
	}
}


/// <summary>
/// Reads the game speed from its slider.
/// This routine is called as the player drags the control, and flags the game options
/// for retransmission to the guest. The slider runs the other way around from the game
/// speed itself -- the further right it goes, the faster the game plays.
/// </summary>
void ModemHost::SetGameSpeed(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_GAME_SPEED_SLIDER);
	if (handle) {
		Session.Options.GameSpeed = 6 - Slider_GetPos(handle);
		Transmit = true;
	}
}


/// <summary>
/// Reads the computer skill level from its slider.
/// This routine is called as the player drags the control, and flags the game options
/// for retransmission to the guest.
/// </summary>
void ModemHost::SetAIDifficulty(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_AILEVEL_SLIDER);
	if (handle) {
		Session.Options.AIDifficulty = (DiffType)Slider_GetPos(handle);
		Transmit = true;
	}
}


/// <summary>
/// Reads the MCV redeploy option from its check box.
/// This routine is called when the player toggles the control, and flags the game
/// options for retransmission to the guest.
/// </summary>
void ModemHost::SetMCVRedeploy(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_REDEPLOY_MCV);
	if (handle) {
		Session.Options.MCVRedeploy = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}


/// <summary>
/// Reads the bases option from its check box.
/// This routine is called when the player toggles the control. A short game cannot be
/// played without bases, so switching bases off switches the short game off too. The
/// game options are flagged for retransmission to the guest.
/// </summary>
void ModemHost::SetBases(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
	if (handle) {
		Session.Options.Bases = Button_GetCheck(handle) == TRUE;
		if (!Session.Options.Bases) {
			HWND handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
			if (handle) {
				Session.Options.ShortGame = false;
				SendDlgItemMessage(_Dialog, IDC_SHORT_GAME, BM_SETCHECK, FALSE, 0);
			}
		}
		Transmit = true;
	}
}


/// <summary>
/// Reads the crates option from its check box.
/// This routine is called when the player toggles the control, and flags the game
/// options for retransmission to the guest.
/// </summary>
void ModemHost::SetGoodies(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_GOODIES);
	if (handle) {
		Session.Options.Goodies = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}


/// <summary>
/// Reads the fog of war option from its check box.
/// This routine is called when the player toggles the control, and flags the game
/// options for retransmission to the guest.
/// </summary>
void ModemHost::SetFogOfWar(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MODEM_FOG);
	if (handle) {
		Session.Options.FogOfWar = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}


/// <summary>
/// Reads the bridge destruction option from its check box.
/// This routine is called when the player toggles the control, and flags the game
/// options for retransmission to the guest.
/// </summary>
void ModemHost::SetBridgeDestruction(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_BRIDGE_DESTROY);
	if (handle) {
		Session.Options.BridgeDestruction = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}


/// <summary>
/// Reads the harvester truce option from its check box.
/// This routine is called when the player toggles the control, and flags the game
/// options for retransmission to the guest.
/// </summary>
void ModemHost::SetHarvTruce(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_HARVTRUCE);
	if (handle) {
		Session.Options.HarvTruce = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}


/// <summary>
/// Reads the short game option from its check box.
/// This routine is called when the player toggles the control. A short game makes no
/// sense without bases, so switching it on switches bases on as well. The game options
/// are flagged for retransmission to the guest.
/// </summary>
void ModemHost::SetShortGame(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_SHORT_GAME);
	if (handle) {
		Session.Options.ShortGame = Button_GetCheck(handle) == TRUE;
		Transmit = true;
		if (Session.Options.ShortGame) {
			HWND handle = GetDlgItem(_Dialog, IDC_MODEM_BASES);
			if (handle) {
				Session.Options.Bases = true;
				SendDlgItemMessage(_Dialog, IDC_MODEM_BASES, BM_SETCHECK, TRUE, 0);
			}
		}
	}
}


/// <summary>
/// Reads the multiple engineers option from its check box.
/// This routine is called when the player toggles the control, and flags the game
/// options for retransmission to the guest.
/// </summary>
void ModemHost::SetCrapEngineers(void)
{
	HWND handle = GetDlgItem(_Dialog, IDC_MULTI_ENGINEER);
	if (handle) {
		Session.Options.CrapEngineers = Button_GetCheck(handle) == TRUE;
		Transmit = true;
	}
}


/// <summary>
/// Runs the scenario selection dialog.
/// This routine steps the host dialog aside while the player browses the multiplayer
/// maps. The previous scenario is put back if the selection is cancelled, and either way
/// the scenario name and the map preview are brought up to date afterwards.
/// </summary>
void ModemHost::SelectMultiMap(void)
{
	DebugString("ModemHost.SelectMultiMap(enter)\n");

	char old_scenario_desc[132];
	strcpy(old_scenario_desc, Session.ScenarioFileName);

	int old_scenario_index = Session.Options.ScenarioIndex;

	strcpy(old_scenario_desc, Session.Options.ScenarioDescription);

	ShowWindow(_Dialog, SW_HIDE);

	if (Scenario_Dialog(MainWindow) == IDCANCEL) {
		Session.Options.ScenarioIndex = old_scenario_index;
		Set_Scenario_Info_From_Index(old_scenario_index);
		Update_Network_Dialog_Preview(_Dialog);
		ShowWindow(_Dialog, SW_SHOW);

		if (stricmp(Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename(), "RandMap.Sed") == 0) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = new MapPreviewClass;
			MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
			if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
				Update_Network_Dialog_Preview(_Dialog);
			}
			InvalidateRect(_Dialog, NULL, FALSE);
		} else {
			Update_Network_Dialog_Preview(_Dialog);
		}
		InvalidateRect(_Dialog, NULL, FALSE);
	} else {
		ShowWindow(_Dialog, SW_SHOW);
		if (Set_Scenario_Info_From_Index(Session.Options.ScenarioIndex) == true) {
			SendDlgItemMessage(_Dialog, IDC_SCENARIONAME, WM_SETTEXT, 0, (LPARAM)Session.Options.ScenarioDescription);

			if (stricmp(Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename(), "RandMap.Sed") == 0) {
				if (MultiplayerMapPreview != NULL) {
					delete MultiplayerMapPreview;
					MultiplayerMapPreview = new MapPreviewClass;
					MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
				}
				if (MultiplayerMapPreview->Get_Preview_Surface() == NULL) {
					Update_Network_Dialog_Preview(_Dialog);
				}
				InvalidateRect(_Dialog, NULL, FALSE);
			} else {
				Update_Network_Dialog_Preview(_Dialog);
			}
		} else {
			Session.Options.ScenarioIndex = old_scenario_index;
		}
	}

	Transmit = true;
	DebugString("ModemHost.SelectMultiMap(exit)\n");
}


/// <summary>
/// Runs the random map generator dialog.
/// This routine lets the host build a map from scratch, then installs the result in the
/// scenario list -- replacing the previous random map entry if there already is one --
/// selects it, and brings up its preview.
/// </summary>
void ModemHost::SelectRandomMap(void)
{
	DebugString("ModemHost.SelectRandomMap(enter)\n");
	IsRandomMap = false;

	if (Do_Random_Map_Dialog(ModemHostCallback) == IDOK) {

		IsRandomMap = true;
		RandomMapGen.SeedData.Save(RANDOM_MAP_FILE_NAME);

		if (MultiplayerMapPreview != NULL) {
			delete MultiplayerMapPreview;
			MultiplayerMapPreview = NULL;
		}

		SendDlgItemMessage(_Dialog, IDC_SCENARIONAME, WM_SETTEXT, 0, (LPARAM)RandomMapGen.SeedData.MapDescription);
		bool found = false;

		int index = 0;
		for (; index < Session.Scenarios.Count(); index++) {
			if (stricmp(Session.Scenarios[index]->Get_Filename(), RANDOM_MAP_FILE_NAME) == 0) {
				found = true;
				break;
			}
		}

		char * digest = CalcRandomMapDigest();

		if (!found) {
			Session.Scenarios.Add(new MultiMission(RANDOM_MAP_FILE_NAME, RandomMapGen.SeedData.MapDescription, digest, true));
		} else {
			Session.Scenarios[index]->Set_Digest(digest);
			Session.Scenarios[index]->Set_Description(RandomMapGen.SeedData.MapDescription);
		}

		delete digest;

		if (Set_Scenario_Info_From_Index(index) == true) {
			Session.Options.ScenarioIndex = index;
			strcpy(Session.ScenarioFileName, RANDOM_MAP_FILE_NAME);
			strcpy(Scen->ScenarioName, Session.ScenarioFileName);
			if (CCFileClass(Session.ScenarioFileName).Is_Available()) {
				MultiplayerMapPreview = new MapPreviewClass;
				MultiplayerMapPreview->Read_PCX_Preview("RandMap.img");
				PostMessage(_Dialog, WM_PAINT, 0, 0);
			}
		}

		Transmit = true;
	}

	IsRandomMap = true;
	DebugString("ModemHost.SelectMultiMap(exit)\n");
}


/// <summary>
/// Services the modem link and sends out anything pending.
/// This routine is polled from the dialog loop. The game options are retransmitted
/// whenever the host has fiddled with them, and a timing packet goes out periodically so
/// both machines can agree on how sluggish the connection is.
/// </summary>
/// <param name="force">Should the game options be sent whether or not they changed?</param>
void ModemHost::Send(bool force)
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
/// Handles an incoming packet from the guest.
/// This routine is polled from the dialog loop. It processes sign-off, game option,
/// chat and timing packets, turns away a guest whose version, expansion or rules do not
/// agree with our own, and keeps an eye on a link that has gone quiet.
/// </summary>
/// <param name="silent">Should trouble be handled without putting a message box up in
/// front of the player?</param>
/// <returns>bool; Should the host dialog be torn down?</returns>
bool ModemHost::Receive(bool silent)
{
	SerialPacketType receivepacket;
	unsigned int version;
	unsigned int starttime;
	int packetlen;

	/*---------------------------------------------------------------------
	Check for an incoming message
	---------------------------------------------------------------------*/
	if (NullModem.Get_Message (&receivepacket, &packetlen) > 0) {

		LastMsgTime = TickCount;
		MsgTimeout = 20 * TIMER_SECOND;		/// reset timeout value to 20 seconds

		// are we getting our own packets back??

		if (receivepacket.Command >= SERIAL_CONNECT &&
			receivepacket.Command < SERIAL_LAST_COMMAND &&
			receivepacket.Command != SERIAL_MESSAGE &&
			receivepacket.ID == Session.ModemType) {

			DebugString("Received our own packet back\n");

			if (!silent) {
				WWMessageBox().Process (TXT_SYSTEM_NOT_RESPONDING, TXT_OK);
			}

			// say we did receive sign off to keep from sending one
			return(true);
		}

		switch (receivepacket.Command) {
			/*..................................................................
			Sign-off: Give the other machine time to receive my ACK, display a
			message, and exit.
			..................................................................*/
			case (SERIAL_SIGN_OFF):
				DebugString("Received SignOff!\n");
				starttime = TickCount;
				while (TickCount - starttime < TIMER_SECOND) {
					Call_Back();
					NullModem.Service();
				}

				if (!silent) {
					WWMessageBox().Process(TXT_USER_SIGNED_OFF, TXT_OK);
				}

				// to skip the other system not responding msg
				LastMsgTime = TickCount;

				Shutdown_Modem();
				return(true);

			/*..................................................................
			Game Options:  Store the other machine's name, color & house;
			If they've picked the same color as myself, re-transmit my settings
			to force him to choose a different color.  (Com_Show_Scenario_Dialog
			is responsible for ensuring the colors are different.)
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
					if (!silent) {
						WWMessageBox().Process (TXT_DESTGAME_OUTDATED, TXT_OK);
					}

					// to skip the other system not responding msg
					LastMsgTime = TickCount;

					return(true);
				} else if (version == 0xffffffff) {
				// ........................................................
				// If the greatest-common-version comes back 0xffffffff,
				// the other system's range is too high for ours
				// ........................................................
					if (!silent) {
						WWMessageBox().Process (TXT_YOURGAME_OUTDATED, TXT_OK);
					}

					// to skip the other system not responding msg
					LastMsgTime = TickCount;

					return(true);
				} else {

					bool ok = true;

					if (receivepacket.ScenarioInfo.IsFirestorm == true && Addon_Enabled(ADDON_FIRESTORM) == false) {
						if (!silent) {
							if (Addon_Installed(ADDON_FIRESTORM) == true) {
								WWMessageBox().Process (TXT_FIRESTORM_MUST_ENABLE, TXT_OK);
							} else {
								WWMessageBox().Process (TXT_FIRESTORM_REQUIRED, TXT_OK);
							}
						}

						// to skip the other system not responding msg
						LastMsgTime = TickCount;

						return(true);
					}

					if (receivepacket.ScenarioInfo.IsFirestorm == false && Addon_Enabled(ADDON_FIRESTORM) == true) {
						if (!silent) {
							WWMessageBox().Process (TXT_FIRESTORM_NO_JOIN_TS, TXT_OK);
						}

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

						if (!silent) {
							WWMessageBox().Process (TXT_MISMATCH, TXT_OK);
						}

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
					GameOptions = true;
				}
				break;

			case (SERIAL_ACCEPT_OPTIONS):
				DebugString("Received Accept Options\n");
				EnableWindow(GetDlgItem(_Dialog, 1), TRUE);
				break;

			/*..................................................................
			Incoming message: add to our list
			..................................................................*/
			case (SERIAL_MESSAGE):
				DebugString("Received serial message\n");
				DrawMessage(receivepacket.Name, receivepacket.ID, receivepacket.Message.Message);

				if (!silent) {
					Sound_Effect(Rule->IncomingMessage);
				}
				break;

			//
			// get their response time
			//
			case (SERIAL_TIMING):
				DebugString("Received timing packet\n");
				TheirResponseTime = receivepacket.ScenarioInfo.ResponseTime;

				if (!GameOptions) {

					// retransmit of game options packet again
					Transmit = true;
				}
				break;

			//
			// print msg waiting for opponent
			//
			case (SERIAL_SCORE_SCREEN):
				DebugString("Received score screen packet\n");
				break;

			case (SERIAL_REQ_PREVIEW):
				SendPreview();
				LastMsgTime = TickCount;
				break;

			default:
				DebugString("Received packet\n");
				break;
		}
	}

	// if we haven't received a msg for 20 seconds exit

	unsigned int time = (TickCount - LastMsgTime);
	if (time > MsgTimeout) {
		DebugString("Timeout! %d ticks since last incoming message\n", time);

		if (!silent) {
			WWMessageBox().Process (TXT_SYSTEM_NOT_RESPONDING, TXT_OK);
		}

		// say we did receive sign off to keep from sending one
		return(true);
	}
	return(false);
}


/// <summary>
/// Sends the current game options to the guest.
/// This routine packages up the host's identity, every game option, and the identity of
/// the chosen scenario. The guest uses the scenario details to locate the map on his own
/// machine or to ask for it to be sent across. The start button stays disabled until the
/// guest replies that he accepts what was sent.
/// </summary>
void ModemHost::SendGameOptions(void)
{
	SerialPacketType sendpacket;

	DebugString("Sending game options packet\n");
	EnableWindow(GetDlgItem(_Dialog, 1), FALSE);

	memset (&sendpacket, 0, sizeof(SerialPacketType));
	sendpacket.Command = SERIAL_GAME_OPTIONS;
	sendpacket.ScenarioInfo.CheatCheck = CheatCheck;
	sendpacket.ScenarioInfo.ArtCheatCheck = ArtCheatCheck;
	sendpacket.ScenarioInfo.AICheatCheck = AICheatCheck;
	sendpacket.ScenarioInfo.IsFirestorm = Addon_Enabled(ADDON_FIRESTORM);
	sendpacket.ScenarioInfo.BuildNumber = Build_Number();
	sendpacket.ScenarioInfo.MinVersion = VerNum.Min_Version();
	sendpacket.ScenarioInfo.MaxVersion = VerNum.Max_Version();
	sendpacket.ScenarioInfo.Seed = Seed;
	sendpacket.ScenarioInfo.Special = Special;
	sendpacket.ScenarioInfo.GameSpeed = Session.Options.GameSpeed;
	sendpacket.ScenarioInfo.IsCrapEngineers = Session.Options.CrapEngineers;
	sendpacket.ScenarioInfo.IsShortGame = Session.Options.ShortGame;
	sendpacket.ID = Session.ModemType;
	strcpy (sendpacket.Name, Session.Handle);
	sendpacket.ScenarioInfo.House = (HousesType)Session.House;
	sendpacket.ScenarioInfo.Color = Session.ColorIdx;
	sendpacket.ScenarioInfo.Credits = Session.Options.Credits;
	sendpacket.ScenarioInfo.IsBases = Session.Options.Bases;
	sendpacket.ScenarioInfo.IsMCVRedeploy = Session.Options.MCVRedeploy;
	sendpacket.ScenarioInfo.IsBridgeDestruction = Session.Options.BridgeDestruction;
	sendpacket.ScenarioInfo.IsGoodies = Session.Options.Goodies;
	sendpacket.ScenarioInfo.IsHarvTruce = Session.Options.HarvTruce;
	sendpacket.ScenarioInfo.IsFogOfWar = Session.Options.FogOfWar;
	sendpacket.ScenarioInfo.AIPlayers = Session.Options.AIPlayers;
	sendpacket.ScenarioInfo.AIDifficulty = Session.Options.AIDifficulty;
	sendpacket.ScenarioInfo.BuildLevel = BuildLevel;
	sendpacket.ScenarioInfo.UnitCount = Session.Options.UnitCount;

	/*
	**	Set up the scenario info so the remote player can match the scenario on his machine
	**	or request a download if it doesnt exist
	*/
	strcpy (sendpacket.ScenarioInfo.Scenario, Session.Scenarios[Session.Options.ScenarioIndex]->Description());
	CCFileClass file (Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename());

	sendpacket.ScenarioInfo.FileLength = file.Size();

	strncpy (sendpacket.ScenarioInfo.ShortFileName, Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename(), sizeof(sendpacket.ScenarioInfo.ShortFileName));
	strncpy ((char*)sendpacket.ScenarioInfo.FileDigest, Session.Scenarios[Session.Options.ScenarioIndex]->Get_Digest(), sizeof sendpacket.ScenarioInfo.FileDigest);
	sendpacket.ScenarioInfo.OfficialScenario = Session.Scenarios[Session.Options.ScenarioIndex]->Get_Official();
	NullModem.Send_Message (&sendpacket, sizeof(sendpacket), 1);
}


/// <summary>
/// Sends a chat message to the other player.
/// This routine transmits the message across the modem link and echoes it into the
/// host's own message list, so the sender gets to see what he said.
/// </summary>
void ModemHost::SendAMessage(const char * message)
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
/// This routine tags the message with the sender's name and displays it in the color
/// belonging to that player, so the two sides of the conversation can be told apart.
/// </summary>
/// <param name="name">The name of the player who sent the message.</param>
/// <param name="color">The player color index to display the message in.</param>
void ModemHost::DrawMessage(const char * name, int color, const char * message)
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
/// Signs off from the other machine.
/// This routine sends the sign-off notice and then holds a "signing off" box on screen
/// while the send queue drains, giving the guest a chance to hear about it before the
/// modem is shut down.
/// </summary>
void ModemHost::SignOff(void)
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
	while (NullModem.Num_Send()
		&& ((TickCount - starttime) < SIGNOFF_PACKET_CANCEL_TIMEOUT)) {
#ifdef _DEBUG
		NullModem.Mono_Debug_Print(0);
#endif

		if (NullModem.Get_Message(&receivepacket, &packetlen) > 0) {

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
/// Starts the modem game.
/// This routine is used when the host commits to the game he has set up. Both players
/// are added to the session player list, the go-ahead is sent to the guest, and the
/// host then waits to hear back -- the guest may agree to start, refuse because he has
/// no copy of the scenario, or ask for the scenario to be sent across first. The
/// session and special options are applied on the way out so play can begin.
/// </summary>
/// <param name="load_game">Is this a request to resume a saved game rather than start a
/// fresh one?</param>
/// <returns>bool; Is the game ready to begin?</returns>
bool ModemHost::StartGame(bool load_game)
{
	SerialPacketType sendpacket;
	SerialPacketType receivepacket;
	unsigned int starttime;
	int packetlen;
	NodeNameType * who;					// node to add to Players

	DebugString("ModemHost.StartGame(enter)\n");
	DebugString("LoadGame = %s\n", load_game == true ? "true" : "false");

	if (load_game == true) {
		return(false);
	}

	/*------------------------------------------------------------------------
	Prepare to load the scenario
	------------------------------------------------------------------------*/
	Scen->Scenario = Session.Options.ScenarioIndex;
	strcpy (Scen->ScenarioName, Session.Scenarios[Session.Options.ScenarioIndex]->Get_Filename());

	/*
	 * Fetch the difficulty setting.
	 */
	Scen->CDifficulty = DIFF_NORMAL;
	Scen->Difficulty = DIFF_NORMAL;

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
	}

	who = new NodeNameType;
	if (who != NULL) {
		strcpy(who->Name, TheirName);
		who->Player.House = TheirHouse;
		who->Player.Color = TheirColor;
		who->Player.ProcessTime = -1;
		Session.Players.Add (who);
	}

	if (Session.Players.Count() != 2) {
		return(false);
	}

	/*.....................................................................
	Send all players a GO packet.
	.....................................................................*/
	memset (&sendpacket, 0, sizeof(SerialPacketType));
	//if (load_game) {
	//	sendpacket.Command = SERIAL_LOADGAME;
	//} else {
		sendpacket.Command = SERIAL_GO;
	//}

	sendpacket.ScenarioInfo.ResponseTime = NullModem.Response_Time();
	if ( TheirResponseTime == 10000 ) {
		;
	} else if (sendpacket.ScenarioInfo.ResponseTime < TheirResponseTime) {
		sendpacket.ScenarioInfo.ResponseTime = TheirResponseTime;
	}

	Session.FrameSendRate = 3;
	Session.LatencyFudge = 0;
	Session.PrecalcMaxAhead = 0;
	Session.PrecalcDesiredFrameRate = 0;
	Options.GameSpeed = Session.Options.GameSpeed;

	//
	// calculated one way delay for a packet and overall delay to execute
	// a packet
	//
	if (Session.CommProtocol == COMM_PROTOCOL_MULTI_E_COMP) {
		Session.MaxAhead = MAX( ((sendpacket.ScenarioInfo.ResponseTime / 8 + 2) / 3) * 3, 6 );
	} else {
		Session.MaxAhead = sendpacket.ScenarioInfo.ResponseTime / 8;
		Session.MaxAhead = MAX( (int)(Session.MaxAhead), MODEM_MIN_MAX_AHEAD );
	}
	sendpacket.ID = Session.ModemType;

	NullModem.Send_Message (&sendpacket, sizeof(sendpacket), 1);
	HWND msgbox = OwnerDraw::Custom_Message_Box(Fetch_String(TXT_SENDING_GO));
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
		OwnerDraw::Set_Custom_Message_Box_Text(msgbox, Fetch_String(TXT_WAITING_GO));
	}

	/*
	**	Wait for the go response. This will be either a 'GO' reply, a
	**	request for the scenario to be sent or a reply to say that the scenario
	**	cant be played.
	*/
	DebugString("Waiting for 'GO' response.\n");
	starttime = TickCount;

	do {
		Call_Back();
		Windows_Message_Handler();
		NullModem.Service();

		if (NullModem.Get_Message (&receivepacket, &packetlen) > 0) {
			DebugString("Received message\n");

			if (receivepacket.Command == SERIAL_READY_TO_GO) {
				DebugString("Received GO message\n");
				break;
			}

			if (receivepacket.Command == SERIAL_NO_SCENARIO) {
				WWMessageBox().Process(TXT_NO_EXPANSION_SCENARIO, TXT_CANCEL);
				/*
				**	We have to recover from this somehow so....
				*/
				LastMsgTime = TickCount;
				DebugString("***Guest doesn't have scenario!\n");
				return(false);
			}

			if (receivepacket.Command == SERIAL_REQ_SCENARIO) {
				DebugString ("About to call 'Send_Remote_File'.\n");
				Send_Remote_File (Scen->ScenarioName, GAME_MODEM, false, false);
				break;
			}
		}

	} while ( TickCount - starttime < 20 * TIMER_SECOND );

	if (msgbox) {
		OwnerDraw::End_Dialog(msgbox);
	}

	// clear queue to keep from doing any resends
	NullModem.Init_Send_Queue();

	Session.NetOpen = false;
	Scen->Scenario = Session.Options.ScenarioIndex;
	Session.NumPlayers = Session.Players.Count();
	Special.IsHarvesterImmune = Session.Options.HarvTruce;
	Special.IsDestroyBridges = Session.Options.BridgeDestruction;
	Special.IsTGrowth = true;
	Special.IsTSpread = true;
	Frame = 0;
	Session.CommProtocol = DEFAULT_COMM_PROTOCOL;
	Scen->Special.IsAllianceFixed = false;
	Session.Options.AlliesAllowed = true;
	Special.Apply_To_Game();
	DebugString("ModemHost.StartGame(exit)\n");
	return(true);
}


/// <summary>
/// Services the modem link while another dialog is running.
/// This routine is handed to the random map generator dialog so the connection to the
/// guest stays alive while the host is busy elsewhere -- game options keep flowing and
/// incoming packets are quietly absorbed.
/// </summary>
/// <returns>bool; Should the calling dialog be aborted? This routine never asks for
/// one.</returns>
bool ModemHostCallback(void)
{
	if (_ModemHost != NULL) {
		_ModemHost->Send(false);
		_ModemHost->Receive(true);
	}
	Call_Back();
	return(false);
}


/// <summary>
/// Sends the random map preview image over to the guest.
/// This routine answers the guest's request for a preview he has no way of building
/// for himself. The preview image is compressed, written out to a temporary file, and
/// handed to the remote file transfer system. Nothing is sent unless the scenario
/// currently selected is the random map.
/// </summary>
void ModemHost::SendPreview(void)
{
	CDTimerClass<SystemTimerClass> response_timer;

	if (stricmp(Session.ScenarioFileName, RANDOM_MAP_FILE_NAME) == 0) {
		DebugString("Starting map preview upload\n");

		SerialPacketType sendpacket;
		memset ((void*)&sendpacket, 0, sizeof (sendpacket));
		sendpacket.Command = SERIAL_PREVIEW_MODE;

		NullModem.Send_Message(&sendpacket, sizeof(sendpacket), 1);
		response_timer = 10 * TIMER_SECOND;
		while (NullModem.Num_Send() > 0 && response_timer) {
			Call_Back();
		}
		response_timer = PACKET_SENDING_TIMEOUT;

		while (true) {
			Call_Back();

			SerialPacketType receivepacket;
			int packetlen;
			if (NullModem.Get_Message(&receivepacket, &packetlen) > 0 && receivepacket.Command == SERIAL_PREVIEW_ACK) {
				break;
			}

			if (response_timer == 0) {
				DebugString("Failed to get SERIAL_PREVIEW_ACK\n");
				return;
			}
		}

		int size = 0;
		unsigned * preview = MultiplayerMapPreview->Create_Paletted_Preview(64, size);
		DebugString("Preview size is %d bytes\n", size);

		unsigned * buffer = new unsigned[size];
		BufferPipe bpipe(buffer, size * sizeof(unsigned));

		LZOPipe lzopipe(LZOPipe::COMPRESS);
		lzopipe.Put_To(&bpipe);
		int comp_size = lzopipe.Put(preview, size);
		comp_size += lzopipe.End();

		DebugString("Compressed preview image is %d bytes\n", comp_size);

		RawFileClass file("Preview.bin");
		if (file.Is_Available()) {
			file.Delete();
		}

		file.Open(RawFileClass::WRITE);
		file.Write(&size, sizeof(size));
		file.Write(buffer, comp_size);
		file.Close();
		delete [] buffer;

		DebugString("Calling Send_Remote_File to send the preview\n");
		Send_Remote_File("Preview.bin", GAME_MODEM, false, false);
	}
}
