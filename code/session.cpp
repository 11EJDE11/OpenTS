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

/* $Header: /counterstrike/SESSION.CPP 3     3/10/97 6:23p Steve_tall $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : SESSION.CPP                                                  *
 *                                                                                             *
 *                   Programmer : Bill R. Randolph                                             *
 *                                                                                             *
 *                   Start Date : 11/30/95                                                     *
 *                                                                                             *
 *                  Last Update : September 10, 1996 [JLB]                                     *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 *   SessionClass::SessionClass -- Constructor                                                 *
 *   SessionClass::~SessionClass -- Destructor                                                 *
 *   SessionClass::One_Time -- one-time initializations                                        *
 *   SessionClass::Init -- Initializes all values                                              *
 *   SessionClass::Create_Connections -- forms connections to other players                    *
 *   SessionClass::Am_I_Master -- tells if the local system is the "master"                    *
 *   SessionClass::Save -- Saves this class to a file                                          *
 *   SessionClass::Load -- Loads this class from a file                                        *
 *   SessionClass::Read_MultiPlayer_Settings -- reads settings from INI                        *
 *   SessionClass::Write_MultiPlayer_Settings -- writes settings to INI                        *
 *   SessionClass::Read_Scenario_Descriptions -- reads scen. descriptions                      *
 *   SessionClass::Free_Scenario_Descriptions -- frees scen. descriptions                      *
 *   SessionClass::Trap_Object -- searches for an object, for debugging                        *
 *   SessionClass::Compute_Unique_ID -- computes unique local ID number                        *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "always.h"

#include "session.h"

#include "_keyboar.h"
#include "_map.h"
#include "_rules.h"
#include "addon.h"
#include "ccini.h"
#include "conquer.h"
#include "data.h"
#include "dbgprint.h"
#include "globals.h"
#include "ipxmgr.h"
#include "language\language.h"
#include "msgloop.h"
#include "nullmgr.h"
#include "phone.h"
#include "progress.h"
#include "queue.h"
#include "rules.h"
#include "scenario.h"
#include "special.h"
#include "wonline.h"
#include "xstraw.h"

#include <ctime> // for station ID computation
#include <dos.h> // for station ID computation


/***************************** Globals *************************************/
/*---------------------------------------------------------------------------
Min & Max unit count values; index0 = bases OFF, index1 = bases ON
---------------------------------------------------------------------------*/
int SessionClass::CountMin[2] = {1,1};
int SessionClass::CountMax[2] = {50,10};

//---------------------------------------------------------------------------
// This is a list of all the names of the multiplayer scenarios
//---------------------------------------------------------------------------
char SessionClass::Descriptions[100][40];

#if _DEBUG

//---------------------------------------------------------------------------
// These values are used purely for the Mono debug display.  They show the
// names of the Global Channel packet types, and the event types.
//---------------------------------------------------------------------------
char const * SessionClass::GlobalPacketNames[] = {
	"NET_QUERY_GAME",
	"NET_ANSWER_GAME",
	"NET_QUERY_PLAYER",
	"NET_ANSWER_PLAYER",
	"NET_CHAT_ANNOUNCE",
	"NET_CHAT_REQUEST",
	"NET_QUERY_JOIN",
	"NET_CONFIRM_JOIN",
	"NET_REJECT_JOIN",
	"NET_GAME_OPTIONS",
	"NET_SIGN_OFF",
	"NET_GO",
	"NET_MESSAGE",
	"NET_PING",
	"NET_LOADGAME",
	"NET_PROGRESS_REPORT",
	"NET_REQ_SCENARIO",
	"NET_FILE_INFO",
	"NET_FILE_CHUNK",
	"NET_READY_TO_GO",
	"NET_NO_SCENARIO",
	"NET_FILE_INFO_ACK",
	"NET_PUB_GAMEOPT",
	"NET_PRIV_GAMEOPT",
	"NET_PREVIEW_MODE",
	"NET_PREVIEW_ACK",
	"NET_REQ_PREVIEW",
	"NET_PROPOSE_KICK",
};

char const * SessionClass::SerialPacketNames[] = {
	"CONNECT",
	"GAME_OPTIONS",
	"SIGN_OFF",
	"GO",
	"MESSAGE",
	"TIMING",
	"SCORE_SCREEN",
	"LOADGAME",
	"LAST_COMMAND",
	"REQ_SCENARIO",
	"SERIAL_FILE_INFO",
	"SERIAL_FILE_CHUNK",
	"SERIAL_FILE_INFO_ACK",
	"SERIAL_READY_TO_GO",
	"SERIAL_NO_SCENARIO"
};

#endif

char const * SessionClass::DialMethodCheck[ DIAL_METHODS ] = {
	"T",
	"P"
};

const int SessionClass::CallWaitStringIDs[ CALL_WAIT_STRINGS_NUM ] = {
	TXT_CWAIT_STAR70,
	TXT_CWAIT_70NUM,
	TXT_CWAIT_1170,
	TXT_CUSTOM,
};

/***************************************************************************
 * SessionClass::SessionClass -- Constructor                               *
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
 *   11/30/1995 BRR : Created.                                             *
 *=========================================================================*/
SessionClass::SessionClass(void)
{
	Type = GAME_NORMAL;
	CommProtocol = DEFAULT_COMM_PROTOCOL;

	Options.ScenarioIndex = 0;
	Options.Bases = false;
	Options.Credits = 0;
	//Options.Tiberium = 0;
	Options.BridgeDestruction = 0;
	Options.Goodies = 0;
	Options.Ghosts = 0;
	Options.UnitCount = 0;
	Options.ShortGame = 0;
	Options.GameSpeed = 0;
	Options.CrapEngineers = 0;

	Options.AlliesAllowed=0;
	Options.HarvTruce=0;
	Options.CTF=0;
	Options.FogOfWar=false;
	Options.MCVRedeploy=false;
	Options.AIDifficulty = DIFF_NORMAL;

	UniqueID = 0;

	Handle[0] = 0;
	PrefColor = 0;
	ColorIdx = 0;
	House = HOUSE_FIRST;

	ObiWan = 0;
	Solo = 0;

	PreferredServer = NULL;

	Locale = 0;
	LastLocale = 0;

	StoreNickname = true;
	LastNicknameSlot = -1;

	MasterPlayerID = -1;
	memset(MasterPlayerName, 0, sizeof(MasterPlayerName));

	SquadAlliances = false;
	SawGameCompletion = false;
	OutOfSync = false;

	MaxPlayers = MAX_PLAYERS;
	NumPlayers = 0;

	FrameSendRate = DEFAULT_FRAME_SEND_RATE;

	MaxAhead = FrameSendRate * 3;
	MaxMaxAhead = MaxAhead;

	memset(ConnectionStats, 0, sizeof(ConnectionStats));

	PrecalcMaxAhead = 0;
	PrecalcDesiredFrameRate = 0;

	ShowInternetDebug = false;

	LoadGame = 0;
	EmergencySave = 0;

	LastMessage[0] = 0;
	WWChat = 0;

	RecordFile.Set_Name("RECORD.BIN");  // always uses this name
	Record= 0;                          // set via command line
	Play = 0;                           // set via command line
	Attract = 0;                        // set via command line

	IsBridge = 0;
	NetStealth = 0;
	NetProtect = 1;
	NetOpen = 0;
	GameName[0] = 0;
	GProductID = 0;
	Suspended = 0;

	LatencyFudge = 1;

	MetaSize = MAX_IPX_PACKET_SIZE;

	PlayerIsGDI = true;

	memset(KickVoteCount, 0, sizeof(KickVoteCount));
	memset(KickVoteWho, -1, sizeof(KickVoteWho));

	ModemService = true;
	CurPhoneIdx = 0;                                    // set from INI file
	SerialDefaults.Port = 0x2f8;                        // set from INI file
	SerialDefaults.IRQ = 3;                             // set from INI file
	SerialDefaults.Baud = 9600;                         // set from INI file
	SerialDefaults.DialMethod = DIAL_TOUCH_TONE;        // set from INI file
	SerialDefaults.InitStringIndex = -1;                // set from INI file
	SerialDefaults.CallWaitStringIndex = 0;             // set from INI file
	strcpy(SerialDefaults.CallWaitString,"");
	ModemType = MODEM_NULL_HOST;						// set from INI file

	TrapFrame = 0x7fffffff;     // frame to start trapping object values at
	TrapObjType = RTTI_NONE;    // type of object to trap
	TrapObject.Ptr.All = NULL;  // ptr to object being trapped
	TrapCoord = COORD_NONE;     // COORDINATE of object to trap
	TrapTarget.Invalidate();    // TARGET value of object to trap
	TrapCell = NULL;            // for trapping a cell
	TrapCheckHeap = 0;          // start checking the Heap
	TrapPrintCRC = 0;           // output CRC file

	for (int i = 0; i < CALL_WAIT_STRINGS_NUM; i++) {
		CallWaitStrings.Add(strdup(Fetch_String(CallWaitStringIDs[i])));
	}
}	// end of SessionClass


/***************************************************************************
 * SessionClass::~SessionClass -- Destructor                               *
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
 *   11/30/1995 BRR : Created.                                             *
 *=========================================================================*/
SessionClass::~SessionClass(void)
{
	while (InitStrings.Count()) {
		delete[] InitStrings[0];
		InitStrings.Delete_Index(0);
	}

	while (CallWaitStrings.Count()) {
		free((void *)CallWaitStrings[0]);
		CallWaitStrings.Delete_Index(0);
	}

	if (PreferredServer != NULL) {
		delete [] PreferredServer;
		PreferredServer = NULL;
	}
}	// end of ~SessionClass


/***************************************************************************
 * SessionClass::One_Time -- one-time initializations                      *
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
 *   12/01/1995 BRR : Created.                                             *
 *=========================================================================*/
void SessionClass::One_Time(void)
{
	//Read_MultiPlayer_Settings();
	Read_Scenario_Descriptions();

	UniqueID = Compute_Unique_ID();
	DebugString("Session one time init. UniqueID is %08x\n", UniqueID);
}	// end of One_Time


/***************************************************************************
 * SessionClass::Init -- Initializes all values                            *
 *                                                                         *
 * This function should be called for every new game played; it only sets  *
 * those variables that should be set for a new game.                      *
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
 *   11/30/1995 BRR : Created.                                             *
 *=========================================================================*/
void SessionClass::Init(void)
{

}	// end of Init


/***************************************************************************
 * SessionClass::Create_Connections -- forms connections to other players  *
 *                                                                         *
 * This routine uses the contents of the Players vector, combined with     *
 * that of the Houses array, to create connections to each other player.   *
 * It is assumed that 'Players' contains all the other players to connect  *
 * to, and that the HouseClass's have been filled in with players' data.   *
 *                                                                         *
 * INPUT:                                                                  *
 *      none.                                                              *
 *                                                                         *
 * OUTPUT:                                                                 *
 *      1 = success, 0 = failure                                           *
 *                                                                         *
 * WARNINGS:                                                               *
 *      none.                                                              *
 *                                                                         *
 * HISTORY:                                                                *
 *   11/30/1995 BRR : Created.                                             *
 *=========================================================================*/
int SessionClass::Create_Connections(void)
{
	int i;
	NetNumType net;
	NetNodeType node;

	DebugString("Entering Create_Connections\n");


	if (Session.Type != GAME_IPX && Session.Type != GAME_INTERNET) {
		return(0);
	}

	//------------------------------------------------------------------------
	// Loop through all entries in 'Players'
	//------------------------------------------------------------------------
	for (i = 0; i < Players.Count(); i++) {
		DebugString("Player %d, Name: %s, ID %d\n", i, Players[i]->Name, Players[i]->Player.ID);

		//.....................................................................
		// Make sure the name matches before creating the connection
		//.....................................................................
		if (!stricmp(Players[i]->Name,
			Houses[Players[i]->Player.ID]->IniName)) {

			if (stricmp(Players[i]->Name, MasterPlayerName) == 0) {
				MasterPlayerID = Players[i]->Player.ID;
			}

			Houses[Players[i]->Player.ID]->SquadID = Players[i]->Player.SquadID;

			Session.Players[i]->Address.Get_Address(net, node);

			unsigned int ip=0;
			for (int k = 0; k < 4; k++) {
				ip <<= 8;
				ip |= node[k];
			}

			DebugString("House[%d] IP = %X  Clan=%d\n",Players[i]->Player.ID, ip, Players[i]->Player.SquadID);

			Houses[Players[i]->Player.ID]->IPAddress = ip;
			Houses[Players[i]->Player.ID]->LostConnection = false;

			// To avoid connecting to myself, skip the 1st entry.
			if (i > 0) {
				Ipx.Create_Connection((int)Players[i]->Player.ID, Players[i]->Name,
					&(Players[i]->Address) );
				Players[i]->Player.ProcessTime = -1;
			}
		} else {
			return(0);
		}
	}

	DebugString("Leaving Create_Connections\n");

	return(1);

}	// end of Create_Connections


/***************************************************************************
 * SessionClass::Am_I_Master -- tells if the local system is the "master"  *
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
 *   11/29/1995 BRR : Created.                                             *
 *=========================================================================*/
bool SessionClass::Am_I_Master(void)
{
	int i;
	HouseClass *hptr;


	if (Session.Type == GAME_INTERNET) {
		if (MasterPlayerID != -1) {
			return(PlayerPtr->HeapID == MasterPlayerID);
		}

		if (PlayerPtr && stricmp(PlayerPtr->IniName, MasterPlayerName) == 0) {
			return(true);
		}
	}

	//------------------------------------------------------------------------
	// Check every house; if PlayerPtr points to the first human house, we're
	// the master.
	//------------------------------------------------------------------------
	for (i = 0; i < Houses.Count(); i++) {
		hptr = Houses[i];
		if (hptr->IsHuman) {
			if (PlayerPtr == hptr) {
				return(true);
			}
			else {
				return(false);
			}
		}
	}

	return(false);

}	// end of Am_I_Master


/***************************************************************************
 * SessionClass::Read_MultiPlayer_Settings -- reads settings INI           *
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
 *   02/14/1995 BR : Created.                                              *
 *=========================================================================*/
void SessionClass::Read_MultiPlayer_Settings(void)
{
	char *tokenptr;         // ptr to token
	PhoneEntryClass *phone; // a phone book entry
	char *entry;            // a phone book entry
	char buf[128];          // buffer for parsing INI entry
	int i;

	Rule->Do_HouseTypes(*RuleINI);

	//------------------------------------------------------------------------
	// Clear the initstring entries
	//------------------------------------------------------------------------
	for (i = 0; i < InitStrings.Count(); i++) {
		delete[] InitStrings[i];
	}
	InitStrings.Clear();

	// Clear the dialing entries
	for (i = 0; i < PhoneBook.Count(); i++) {
		delete PhoneBook[i];
	}
	PhoneBook.Clear();

	// Create filename and read the file.
	CCFileClass file(CONFIG_FILE_NAME);

	// Get the player's last-used Handle
	ConfigINI.Get_String("MultiPlayer", "Handle", Fetch_String(TXT_NONAME), Handle, sizeof(Handle));

	// Get the player's last-used Color
	PrefColor = ConfigINI.Get_Int("MultiPlayer", "Color", 0);

	if (!Session.IsWDT) {
		House = (int)ConfigINI.Get_HousesType("MultiPlayer", "Side", (HousesType)House);
	}

	CurPhoneIdx = ConfigINI.Get_Int("MultiPlayer", "PhoneIndex", -1);
	TrapCheckHeap = ConfigINI.Get_Int("MultiPlayer", "CheckHeap", 0);

	buf[0] = 0;
	ConfigINI.Get_String("MultiPlayer", "PreferredServer", "", buf, sizeof(buf));
	if (strlen(buf) != 0) {
		PreferredServer = new char [strlen(buf) + 1];
		strcpy(PreferredServer, buf);
	}

	Locale = ConfigINI.Get_Int("MultiPlayer", "Locale", 0);
	LastLocale = Locale;

	StoreNickname = ConfigINI.Get_Bool("MultiPlayer", "StoreNick", true);
	LastNicknameSlot = ConfigINI.Get_Int("MultiPlayer", "LastNickSlot", -1);

	// Read in default serial settings
	ConfigINI.Get_String("SerialDefaults", "ModemName", Fetch_String(TXT_NONAME), SerialDefaults.ModemName, MODEM_NAME_MAX);
	if (strcmp(SerialDefaults.ModemName, Fetch_String(TXT_NONAME)) == 0) {
		SerialDefaults.ModemName[0] = 0;
	}
	SerialDefaults.Port = ConfigINI.Get_Int("SerialDefaults", "Port", 0);
	SerialDefaults.IRQ = ConfigINI.Get_Int("SerialDefaults", "IRQ", -1);
	SerialDefaults.Baud = ConfigINI.Get_Int("SerialDefaults", "Baud", -1);
	SerialDefaults.Compression = ConfigINI.Get_Int("SerialDefaults", "Compression", 0);
	SerialDefaults.ErrorCorrection = ConfigINI.Get_Int("SerialDefaults", "ErrorCorrection", 0);

	ConfigINI.Get_String("SerialDefaults", "DialMethod", "T", buf, 2);

	// find dial method
	for (i = 0; i < DIAL_METHODS; i++) {
		if ( !strcmpi( buf, DialMethodCheck[ i ]) ) {
			SerialDefaults.DialMethod = (DialMethodType)i;
			break;
		}
	}

	// if method not found set to touch tone
	if (i == DIAL_METHODS) {
		SerialDefaults.DialMethod = DIAL_TOUCH_TONE;
	}

	SerialDefaults.InitStringIndex = ConfigINI.Get_Int("SerialDefaults", "InitStringIndex", -1);

	SerialDefaults.CallWaitStringIndex = ConfigINI.Get_Int("SerialDefaults", "CallWaitStringIndex", CALL_WAIT_CUSTOM);

	ConfigINI.Get_String("SerialDefaults", "CallWaitString", "", SerialDefaults.CallWaitString, CWAITSTRBUF_MAX);

	if (SerialDefaults.IRQ == 0 || SerialDefaults.Baud == 0) {
		SerialDefaults.Port = 0;
		SerialDefaults.IRQ = -1;
		SerialDefaults.Baud = -1;
	}

	int initcount = ConfigINI.Entry_Count("InitStrings");

	int index;
	for (index = 0; index < initcount; index++) {
		entry = new char[ INITSTRBUF_MAX ];

		if (entry != NULL) {
			entry[0] = 0;
			ConfigINI.Get_String("InitStrings", ConfigINI.Get_Entry("InitStrings", index), NULL, entry, INITSTRBUF_MAX);
			strupr( entry );
			InitStrings.Add( entry );
		}
	}

	//	if no entries then have at least one
	if (initcount == 0) {
		SerialDefaults.InitStringIndex = -1;
	} else {
		SerialDefaults.InitStringIndex = 0;
	}

	// Read the entry names in
	int phonecount = ConfigINI.Entry_Count("PhoneBook");
	for (index = 0; index < phonecount; index++) {
		// Create a new phone book entry
		phone = new PhoneEntryClass();

		// Read the entire entry in
		ConfigINI.Get_String("PhoneBook", ConfigINI.Get_Entry("PhoneBook", index), NULL, buf, sizeof(buf));

		//	Extract name, phone # & serial port settings
		tokenptr = strtok( buf, "|" );
		if (tokenptr) {
			strcpy( phone->Name, tokenptr );
			strupr( phone->Name );
		} else {
			phone->Name[0] = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			strcpy( phone->Number, tokenptr );
			strupr( phone->Number );
		} else {
			phone->Number[0] = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			sscanf( tokenptr, "%x", &phone->Settings.Port );
		} else {
			phone->Settings.Port = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.IRQ = atoi( tokenptr );
		} else {
			phone->Settings.IRQ = -1;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.Baud = atoi( tokenptr );
		} else {
			phone->Settings.Baud = -1;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.Compression = atoi( tokenptr );
		} else {
			phone->Settings.Compression = -1;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.ErrorCorrection = atoi( tokenptr );
		} else {
			phone->Settings.ErrorCorrection = 0;
		}

		/*
		**	Find out if this phonebook entry has the new settings included. If not
		**	then we need to skip this section.
		*/
		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			strcpy( buf, tokenptr );

			// find dial method
			for (i = 0; i < DIAL_METHODS; i++) {
				if ( !strcmpi( buf, DialMethodCheck[ i ]) ) {
					phone->Settings.DialMethod = (DialMethodType)i;
					break;
				}
			}

			//	if method not found set to touch tone
			if (i == DIAL_METHODS) {
				phone->Settings.DialMethod = DIAL_TOUCH_TONE;
			}
		} else {
			phone->Settings.DialMethod = DIAL_TOUCH_TONE;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.InitStringIndex = atoi( tokenptr );
		} else {
			phone->Settings.InitStringIndex = -1;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.CallWaitStringIndex = atoi( tokenptr );
		} else {
			phone->Settings.CallWaitStringIndex = CALL_WAIT_CUSTOM;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			strcpy(phone->Settings.CallWaitString, tokenptr);
		} else {
			phone->Settings.CallWaitString[0] = 0;
		}

		// Add it to our list
		PhoneBook.Add(phone);
	}

	// Read special recording playback values, to help find sync bugs
	if (Session.Play) {
		TrapFrame = ConfigINI.Get_Int("SyncBug", "Frame", 0x7fffffff);

		ConfigINI.Get_String("SyncBug", "Type", "NONE", buf, 80);

		if (!stricmp(buf,"AIRCRAFT"))
			TrapObjType = RTTI_AIRCRAFT;
		else if (!stricmp(buf,"ANIM"))
			TrapObjType = RTTI_ANIM;
		else if (!stricmp(buf,"BUILDING"))
			TrapObjType = RTTI_BUILDING;
		else if (!stricmp(buf,"BULLET"))
			TrapObjType = RTTI_BULLET;
		else if (!stricmp(buf,"INFANTRY"))
			TrapObjType = RTTI_INFANTRY;
		else if (!stricmp(buf,"UNIT"))
			TrapObjType = RTTI_UNIT;
		else {
			TrapObjType = RTTI_NONE;
		}

		ConfigINI.Get_String("SyncBug", "Coord", "0", buf, 80);
		int x;
		int y;
		int z;
		sscanf(buf,"%d,%d,%d", &x, &y, &z);
		int target = ConfigINI.Get_Int("SyncBug", "Target", -1);
		TrapTarget.Decode(target);

		ConfigINI.Get_String("SyncBug", "Cell", "0,0", buf, 80);
		sscanf(buf, "%d,%d", &x, &y);
		Cell cell(x, y);
		if (cell != CELL_NONE) {
			TrapCell = &(Map[cell]);
		}

		TrapPrintCRC = ConfigINI.Get_Int("SyncBug", "PrintCRC", 0x7fffffff);
	}
}


/// <summary>
/// Writes the session settings out to a log file.
/// This routine is used when a sync bug or some other multiplayer failure is being
/// recorded, so that the game options and the players' network addresses are captured
/// alongside it.
/// </summary>
/// <param name="out">The file to write the report to.</param>
bool SessionClass::Log_To_File(FILE *out)
{
	int i;
	int j;
	NetNumType net;
	NetNodeType node;

	if (Session.Options.ScenarioIndex < 0) {
		fprintf(out, "BOGUS scenario index!?!\n");
		Session.Options.ScenarioIndex = 0;
	}

	fprintf(out, "Scenario = %s\n", Session.Scenarios[Session.Options.ScenarioIndex]->Description());
	fprintf(out, "Handle = %s\n", Handle);

	for (i = 0; i < NumPlayers; i++) {
		Session.Players[i]->Address.Get_Address(net,node);
		fprintf(out, "Player Addr[%d] = ", i);
		for (j = 0; j < sizeof(NetNumType); j++) {
			fprintf(out, "%02X", net[j]);
		}
		fprintf(out,":");
		for (j = 0; j < sizeof(NetNodeType); j++) {
			fprintf(out, "%02X", node[j]);
		}
		fprintf(out, "\n");
	}
	fprintf(out, "\n");

	Session.HostAddress.Get_Address(net, node);
	fprintf(out, "Address = ");
	for (i = 0; i < sizeof(NetNumType); i++) {
		fprintf(out, "%02X", net[i]);
	}

	fprintf(out,":");
	for (i = 0; i < sizeof(NetNodeType); i++) {
		fprintf(out, "%02X", node[i]);
	}

	fprintf(out,"\n\n");
	fprintf(out,"MaxAhead = %d\n", MaxAhead);
	fprintf(out,"LoadGame = %d\n", LoadGame);
	fprintf(out,"PrefColor = %d\n", PrefColor);
	fprintf(out,"ColorIdx = %d\n", ColorIdx);
	fprintf(out,"House = %d\n", House);
	fprintf(out,"NumPlayers = %d\n", NumPlayers);
	fprintf(out,"Options.Bases = %d\n", Options.Bases);
	fprintf(out,"Options.Credits = %d\n", Options.Credits);
	fprintf(out,"Options.BridgeDestruction = %d\n", Options.BridgeDestruction);
	fprintf(out,"Options.Goodies = %d\n", Options.Goodies);
	fprintf(out,"Options.Ghosts = %d\n", Options.Ghosts);
	fprintf(out,"Options.UnitCount = %d\n", Options.UnitCount);
	fprintf(out,"Options.AIPlayers = %d\n", Options.AIPlayers);
	fprintf(out,"Options.AIDifficulty = %d\n", Options.AIDifficulty);
	fprintf(out,"ObiWan = %d\n", ObiWan);

	if (Session.Type == GAME_IPX) {
		fprintf(out, "Type = IPX\n");
	} else if (Session.Type == GAME_INTERNET) {
		fprintf(out, "Type = INTERNET\n");
	} else {
		fprintf(out, "Type = !!Unknown!!\n");
	}

	return(true);
}


/***************************************************************************
 * SessionClass::Write_MultiPlayer_Settings -- writes settings INI         *
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
 *   02/14/1995 BR : Created.                                              *
 *=========================================================================*/
void SessionClass::Write_MultiPlayer_Settings(void)
{
	RawFileClass file(CONFIG_FILE_NAME);
	{
		// Save the player's last-used Handle & Color
		ConfigINI.Put_Int("MultiPlayer", "PhoneIndex", CurPhoneIdx);
		ConfigINI.Put_Int("MultiPlayer", "Color", (int)PrefColor);
		ConfigINI.Put_HousesType("MultiPlayer", "Side", (HousesType)House);
		ConfigINI.Put_String("MultiPlayer", "Handle", Handle);

		if (PreferredServer) {
			ConfigINI.Put_String("MultiPlayer", "PreferredServer", PreferredServer);
		}

		if (Locale == 0) {
			Locale = LastLocale;
		}

		ConfigINI.Put_Int("MultiPlayer", "Locale", Locale);
		ConfigINI.Put_Bool("MultiPlayer", "StoreNick", StoreNickname);
		ConfigINI.Put_Int("MultiPlayer", "LastNickSlot", LastNicknameSlot);

		// Clear all existing Settings.SerialDefault entries.
		ConfigINI.Clear("SerialDefaults");

		// Save default serial settings in opposite order you want to see them
		ConfigINI.Put_String("SerialDefaults", "CallWaitString", SerialDefaults.CallWaitString);
		ConfigINI.Put_Int("SerialDefaults", "CallWaitStringIndex", SerialDefaults.CallWaitStringIndex);
		ConfigINI.Put_Int("SerialDefaults", "InitStringIndex", SerialDefaults.InitStringIndex);
		ConfigINI.Put_String("SerialDefaults", "DialMethod", DialMethodCheck[ SerialDefaults.DialMethod ]);
		ConfigINI.Put_Int("SerialDefaults", "Baud", SerialDefaults.Baud);
		ConfigINI.Put_Int("SerialDefaults", "IRQ", SerialDefaults.IRQ);
		ConfigINI.Put_Int("SerialDefaults", "Port", SerialDefaults.Port, 1);
		ConfigINI.Put_Int("SerialDefaults", "Compression", SerialDefaults.Compression );
		ConfigINI.Put_Int("SerialDefaults", "ErrorCorrection", SerialDefaults.ErrorCorrection );
		ConfigINI.Put_String("SerialDefaults", "ModemName", SerialDefaults.ModemName);

		// Clear all existing InitString entries.
		ConfigINI.Clear("InitStrings");

		// Save all InitString entries.
		for (int index = 0; index < InitStrings.Count(); index++) {
			char buf[10];
			wsprintf( buf, "%03d", index);
			ConfigINI.Put_String("InitStrings", buf, InitStrings[index]);
		}

		// Clear all existing Phone Book entries.
		ConfigINI.Clear("PhoneBook");

		// Save all Phone Book entries.
		//	Format: Entry=Name,PhoneNum,Port,IRQ,Baud,InitString
		for (int i = (PhoneBook.Count() - 1); i >= 0; i--) {
			char buf[128];
			char entrytext[10];
			wsprintf(buf,"%s|%s|%x|%d|%d|%d|%d|%s|%d|%d|%s",
				PhoneBook[i]->Name,
				PhoneBook[i]->Number,
				PhoneBook[i]->Settings.Port,
				PhoneBook[i]->Settings.IRQ,
				PhoneBook[i]->Settings.Baud,
				PhoneBook[i]->Settings.Compression,
				PhoneBook[i]->Settings.ErrorCorrection,
				DialMethodCheck[ PhoneBook[i]->Settings.DialMethod ],
				PhoneBook[i]->Settings.InitStringIndex,
				PhoneBook[i]->Settings.CallWaitStringIndex,
				PhoneBook[i]->Settings.CallWaitString);
			wsprintf( entrytext, "%03d", i );
			ConfigINI.Put_String("PhoneBook", entrytext, buf);
		}

		// Write the INI data out to a file.
		ConfigINI.Save(file, false);
	}
}

/***************************************************************************
 * SessionClass::Read_Scenario_Descriptions -- reads scen. descriptions    *
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
 *   02/14/1995 BR : Created.                                              *
 *   09/10/1996 JLB : Searches using different method.                     *
 *=========================================================================*/
void SessionClass::Read_Scenario_Descriptions(void)
{
	INIClass ini;
	char name_buffer[64];

	// Clear the scenario description lists
	for (int index = 0; index < Scenarios.Count(); index++) {
		delete Scenarios[index];
	}
	Scenarios.Clear();

	/*
	**	Fetch the main multiplayer scenario packet data.
	*/
	CCFileClass file("MISSIONS.PKT");
	if (file.Is_Available()) {
		ini.Load(file);
		int count = ini.Entry_Count("MultiMaps");
//DebugString( "Found %i missions in Missions.pkt\n", count );
		for (int index = 0; index < count; index++) {
			if (ini.Get_String("MultiMaps", ini.Get_Entry("MultiMaps", index), "", name_buffer, sizeof(name_buffer))) {
				Scenarios.Add(new MultiMission(ini, name_buffer));
			}
		}
/*		//	ajw Copy file for viewing.
		CCFileClass fileCopy( "msns_pkt.txt" );
		file.Seek( 0, SEEK_SET );
		long lSize = file.Size();
		char * pData = new char[ lSize + 1 ];
		file.Read( pData, lSize );
		fileCopy.Write( pData, lSize );
		fileCopy.Close();
*/	}

	/*
	**	Fetch any scenario packet lists and apply them first.
	*/
	for (AddonType addon = ADDON_COUNT; addon > ADDON_BASE_GAME; --addon) {
		if (Addon_Enabled(addon) == true) {
			sprintf(name_buffer, "MULTI%02d.PKT", addon);
			file.Close();
			file.Set_Name(name_buffer);
			if (CCFileClass(name_buffer).Is_Available()) {
				ini.Clear();
				ini.Load(file);

				int count = ini.Entry_Count("MultiMaps");
				for (int index = 0; index < count; index++) {
					if (ini.Get_String("MultiMaps", ini.Get_Entry("MultiMaps", index), "", name_buffer, sizeof(name_buffer))) {
						Scenarios.Add(new MultiMission(ini, name_buffer));
					}
				}
			}
		}
	}

	WIN32_FIND_DATA block;
	HANDLE handle = FindFirstFile("*.PKT", &block);
	while (handle != INVALID_HANDLE_VALUE) {
		if ((block.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY)) == 0) {
			char const * name = &block.cAlternateFileName[0];
			if (strlen(name) == 0) name = &block.cFileName[0];
//Mono_Printf("Found file '%s'.\n", block.cAlternateFileName);
//Mono_Printf("Found file '%s'.\n", block.cFileName);
//DebugString("Found file '%s'.\n", block.cAlternateFileName);
//DebugString("Found file '%s'.\n", block.cFileName);
//DebugString( "Found alternate PKT file.\n" );

			if (stricmp(name, "MISSIONS.PKT")) {
				file.Close();
				file.Set_Name(name);
				ini.Clear();
				ini.Load(file);

				int count = ini.Entry_Count("MultiMaps");
				for (int index = 0; index < count; index++) {
					if (ini.Get_String("MultiMaps", ini.Get_Entry("MultiMaps", index), "", name_buffer, sizeof(name_buffer))) {
						Scenarios.Add(new MultiMission(ini, name_buffer));
					}
				}
			}
		}

		if (FindNextFile(handle, &block) == 0) break;
	}

	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
	}

	ini.Clear();
	file.Close();


	/*
	**	Scan the current directory for any loose .MPR files and build the appropriate entries
	**	into the scenario list list
	*/
	char const * file_name;
	char digest_buffer[32];

	handle = FindFirstFile( "*.MPR" , &block );
	while (handle != INVALID_HANDLE_VALUE) {
		if ((block.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_TEMPORARY)) == 0) {
			file_name = &block.cAlternateFileName[0];
			if (strlen(file_name) == 0) file_name = &block.cFileName[0];
//DebugString( "Found MPR '%s'\n", file_name );
			file.Set_Name(file_name);
			ini.Load(file);

			ini.Get_String("Basic", "Name", "No Name", name_buffer, sizeof(name_buffer) );
			ini.Get_String("Digest", "1", "No Digest", digest_buffer, sizeof(digest_buffer) );
			Scenarios.Add(new MultiMission(file_name, name_buffer, digest_buffer,ini.Get_Bool("Basic", "Official", false)));
		}

		if (FindNextFile(handle, &block) == 0) break;
	}

	if (handle != INVALID_HANDLE_VALUE) {
		FindClose(handle);
	}

	Options.ScenarioIndex = 0;
}


/***************************************************************************
 * SessionClass::Free_Scenario_Descriptions -- frees scen. descriptions    *
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
 *   06/05/1995 BRR : Created.                                             *
 *=========================================================================*/
void SessionClass::Free_Scenario_Descriptions(void)
{
	int i;

	//------------------------------------------------------------------------
	// Clear the scenario descriptions & filenames
	//------------------------------------------------------------------------
	for (int index = 0; index < Scenarios.Count(); index++) {
		delete Scenarios[index];
	}
	Scenarios.Clear();
//	Filenum.Clear();

	//------------------------------------------------------------------------
	// Clear the initstring entries
	//------------------------------------------------------------------------
	for (i = 0; i < InitStrings.Count(); i++) {
		delete [] InitStrings[i];
	}
	InitStrings.Clear();

	//------------------------------------------------------------------------
	// Clear the dialing entries
	//------------------------------------------------------------------------
	for (i = 0; i < PhoneBook.Count(); i++) {
		if (PhoneBook[i] != NULL) {
			delete PhoneBook[i];
		}
	}
	PhoneBook.Clear();

}	/* end of Free_Scenario_Descriptions */


/***************************************************************************
 * SessionClass::Trap_Object -- searches for an object, for debugging      *
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
 *   06/02/1995 BRR : Created.                                             *
 *=========================================================================*/
void SessionClass::Trap_Object(void)
{
#ifdef TRAP_OBJECTS
	int i;

	//------------------------------------------------------------------------
	// Initialize
	//------------------------------------------------------------------------
	TrapObject.Ptr.All = NULL;

	//------------------------------------------------------------------------
	// Search for the object based upon its type, then its coordinate or
	// 'this' pointer value.
	//------------------------------------------------------------------------
	switch (TrapObjType) {
		case RTTI_AIRCRAFT:
			for (i = 0; i < Aircraft.Count(); i++) {
				if (Aircraft[i]->Coord == TrapCoord ||
					Aircraft[i]->As_Target()==TrapTarget) {
					TrapObject.Ptr.Aircraft = Aircraft[i];
					break;
				}
			}
			break;

		case RTTI_ANIM:
			for (i = 0; i < Anims.Count(); i++) {
				if (Anims[i]->Coord == TrapCoord ||
					Anims[i]->As_Target()==TrapTarget) {
					TrapObject.Ptr.Anim = Anims[i];
					break;
				}
			}
			break;

		case RTTI_BUILDING:
			for (i = 0; i < Buildings.Count(); i++) {
				if (Buildings[i]->Coord == TrapCoord ||
					Buildings[i]->As_Target()==TrapTarget) {
					TrapObject.Ptr.Building = Buildings[i];
					break;
				}
			}
			break;

		case RTTI_BULLET:
			for (i = 0; i < Bullets.Count(); i++) {
				if (Bullets[i]->Coord == TrapCoord ||
					Bullets[i]->As_Target()==TrapTarget) {
					TrapObject.Ptr.Bullet = Bullets[i];
					break;
				}
			}
			break;

		case RTTI_INFANTRY:
			for (i = 0; i < Infantry.Count(); i++) {
				if (Infantry[i]->Coord == TrapCoord ||
					Infantry[i]->As_Target()==TrapTarget) {
					TrapObject.Ptr.Infantry = Infantry[i];
					break;
				}
			}
			break;

		case RTTI_UNIT:
			for (i = 0; i < Units.Count(); i++) {
				if (Units[i]->Coord == TrapCoord ||
					Units[i]->As_Target()==TrapTarget) {
					TrapObject.Ptr.Unit = Units[i];
					break;
				}
			}
			break;

		//.....................................................................
		// Last-ditch find-the-object-right-now-darnit loop
		//.....................................................................
		case RTTI_NONE:
			for (i = 0; i < Aircraft.Count(); i++) {
				if (Aircraft.Raw_Ptr(i)->Coord == TrapCoord ||
					Aircraft.Raw_Ptr(i)->As_Target()==TrapTarget) {
					TrapObject.Ptr.Aircraft = Aircraft.Raw_Ptr(i);
					TrapObjType = RTTI_AIRCRAFT;
					return;
				}
			}
			for (i = 0; i < Anims.Count(); i++) {
				if (Anims.Raw_Ptr(i)->Coord == TrapCoord ||
					Anims.Raw_Ptr(i)->As_Target()==TrapTarget) {
					TrapObject.Ptr.Anim = Anims.Raw_Ptr(i);
					TrapObjType = RTTI_ANIM;
					return;
				}
			}
			for (i = 0; i < Buildings.Count(); i++) {
				if (Buildings.Raw_Ptr(i)->Coord == TrapCoord ||
					Buildings.Raw_Ptr(i)->As_Target()==TrapTarget) {
					TrapObject.Ptr.Building = Buildings.Raw_Ptr(i);
					TrapObjType = RTTI_BUILDING;
					return;
				}
			}
			for (i = 0; i < Bullets.Count(); i++) {
				if (Bullets.Raw_Ptr(i)->Coord == TrapCoord ||
					Bullets.Raw_Ptr(i)->As_Target()==TrapTarget) {
					TrapObject.Ptr.Bullet = Bullets.Raw_Ptr(i);
					TrapObjType = RTTI_BULLET;
					return;
				}
			}
			for (i = 0; i < Infantry.Count(); i++) {
				if (Infantry.Raw_Ptr(i)->Coord == TrapCoord ||
					Infantry.Raw_Ptr(i)->As_Target()==TrapTarget) {
					TrapObject.Ptr.Infantry = Infantry.Raw_Ptr(i);
					TrapObjType = RTTI_INFANTRY;
					return;
				}
			}
			for (i = 0; i < Units.Count(); i++) {
				if (Units.Raw_Ptr(i)->Coord == TrapCoord ||
					Units.Raw_Ptr(i)->As_Target()==TrapTarget) {
					TrapObject.Ptr.Unit = Units.Raw_Ptr(i);
					TrapObjType = RTTI_UNIT;
					return;
				}
			}

		default:
			break;
	}
#endif
}


/// <summary>
/// Converts a multiplayer color choice into a color scheme.
/// This routine turns the color a player picked in the lobby into the scheme their units
/// are actually drawn with. A choice that is not one of the multiplayer colors is passed
/// back untouched.
/// </summary>
/// <param name="id">The multiplayer color the player has chosen.</param>
/// <returns>Returns with the color scheme to draw that player with.</returns>
int SessionClass::Color_Index_To_Scheme(int id)
{
	/*
	 * The list holds each scheme twice, and the odd entry of each pair is the lighting-aware
	 * one, so the index is doubled and stepped one past the plain copy.
	 */
	static char _table[] = {
		(2 *  1) + 1,
		(2 * 10) + 1,
		(2 * 23) + 1,
		(2 * 36) + 1,
		(2 * 13) + 1,
		(2 * 27) + 1,
		(2 * 19) + 1,
		(2 * 16) + 1,
	};

	if (id < sizeof(_table)) {
		return(_table[id]);
	}
	return(id);
}


/***************************************************************************
 * SessionClass::Compute_Unique_ID -- computes unique local ID number      *
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
 *   12/07/1995 BRR : Created.                                             *
 *=========================================================================*/
unsigned int SessionClass::Compute_Unique_ID(void)
{
//	time_t tm;
	unsigned int id;
//	struct diskfree_t dtable;
	char *path;
	unsigned i;

	//------------------------------------------------------------------------
	// Start with the seconds since Jan 1, 1970 (system local time)
	//------------------------------------------------------------------------
//	time(&tm);
//	id = (unsigned long)tm;
	id = timeGetTime();

	//------------------------------------------------------------------------
	// Now add in the free space on the hard drive
	//------------------------------------------------------------------------
	unsigned int space_available = Disk_Space_Available();
	Add_CRC(&id, space_available);

	//------------------------------------------------------------------------
	// Add in every byte in the user's path environment variable
	//------------------------------------------------------------------------
	path = getenv("PATH");
	if (path) {
		for (i = 0; i < strlen(path); i++) {
			Add_CRC(&id, (unsigned int)path[i]);
		}
	}

	return(id);
}	// end of Compute_Unique_ID


/// <summary>
/// Constructs a mission from an entry in the mission list.
/// The description, player limits and disc list come out of the section named, while the
/// digest is fetched from the tail of the map file so that the other players' copies can be
/// checked against it. A map that cannot be located is still listed, merely without a
/// digest to its name.
/// </summary>
/// <param name="ini">The INI database that holds the mission list.</param>
/// <param name="name">Name of the section that describes this mission.</param>
MultiMission::MultiMission(INIClass const & ini, char const * name)
{
	char buffer[128];
	char *token;

	assert(name != NULL);

	ScenarioDescription[0] = '0';
	Digest[0] = '0';
	Set_Digest(NULL);
	IsOfficial = true;
	MinPlayers = 2;
	MaxPlayers = 4;
	Disks.Clear();

	if (name != NULL) {

		strcpy(Filename, name);
		strcat(Filename, ".MAP");

		ini.Get_String(name, "Description", "", ScenarioDescription, sizeof(ScenarioDescription));
		MinPlayers = ini.Get_Int(name, "MinPlayers", MinPlayers);
		MaxPlayers = ini.Get_Int(name, "MinPlayers", MaxPlayers);

		if (ini.Get_String(name, "CD", "", buffer, sizeof(buffer))) {
			token = strtok(buffer, ",");

			while (token != NULL && *token != '\0') {
				Disks.Add((DiskID)atoi(token));
				token = strtok(NULL, ",");
			}
		}

		CCFileClass file(Filename);

		if (!file.Is_Available()) {
			DebugString("Unable to locate scenario %s - No digest info will be loaded\n", Filename);
		}else{
			if (!file.Open()) {
				return;
			}
			char digestbuf[128];

			file.Seek(-((int)sizeof(digestbuf) - 28), SEEK_END);
			memset(digestbuf, 0, sizeof(digestbuf));
			file.Read(digestbuf, sizeof(digestbuf) - 28);
			file.Close();

			char *digest = strstr(digestbuf, "[Digest]");

			if (digest != NULL) {
				CCINIClass mapini;
				BufferStraw straw(digestbuf, sizeof(digestbuf) - 28);
				mapini.Load(straw, false, false);
				mapini.Get_String("Digest", "1", NULL, digestbuf, sizeof(digestbuf));
				Set_Digest(digestbuf);
			}
		}
	}
}


/// <summary>
/// Constructs a mission from a map file found on disk.
/// The map itself is consulted for a better description, for the player limits and for the
/// discs it expects to be found on. A map that says nothing about discs is presumed to live
/// on the hard drive.
/// </summary>
/// <param name="filename">The map file this mission is played from.</param>
/// <param name="description">The description to show in the scenario list.</param>
/// <param name="digest">The digest the other players' copies are checked against.</param>
/// <param name="official">Is this one of the missions that shipped with the game?</param>
MultiMission::MultiMission(char const * filename, char const * description, char const * digest, bool official)
{
	Set_Filename(filename);
	Set_Description(description);
	Set_Digest(digest);
	Set_Official(official);
	MinPlayers = 2;
	MaxPlayers = 4;
	Disks.Clear();

	CCFileClass file(filename);

	if (file.Is_Available() == true) {
		char buffer[256];

		CCINIClass ccini;
		ccini.Load(file, false, false);
		ccini.Get_String( "Multiplay", "Description", "", buffer, sizeof(buffer));

		if (strlen(buffer) != 0) {
			Set_Description(buffer);
		}

		MinPlayers = ccini.Get_Int("Multiplay", "MinPlayers", MinPlayers);
		MaxPlayers = ccini.Get_Int("Multiplay", "MinPlayers", MaxPlayers);

		if (ccini.Get_String("Multiplay", "CD", "", buffer, sizeof(buffer))) {
			char *tok = strtok(buffer, ",");
			while (tok != NULL && tok[0] != '\0') {
				Disks.Add((DiskID)atoi(tok));

				tok = strtok(NULL, ",");
			}
		}
	}

	if (Disks.Count() == 0) {
		Disks.Add(DISK_LOCAL);
	}
}


/// <summary>
/// Can this mission be played from the specified disk?
/// This routine is used when deciding whether a scenario may be offered to the player,
/// since a multiplayer map might live on the hard drive or on either of the game discs.
/// </summary>
/// <param name="cd">The disk to look for the mission on.</param>
/// <returns>bool; Is the mission reachable from that disk?</returns>
bool MultiMission::Is_On_CD(DiskID cd) const
{
	if (cd == DISK_LOCAL) {
		return(RawFileClass(Filename).Is_Available());
	}

	if (Disks.Is_In_List(DISK_ANY) == true) {
		return(true);
	}

	if (Disks.Is_In_List(DISK_GDI) == true || Disks.Is_In_List(DISK_NOD) == true) {
		return(true);
	}

	return(Disks.Is_In_List(cd));
}


/// <summary>
/// Fetches the disk this mission should be looked for on.
/// </summary>
/// <returns>Returns with the first disk the mission was registered against.</returns>
DiskID MultiMission::On_Which_CD(void) const
{
	return(Disks[0]);
}


/// <summary>
/// Sets the description of this mission.
/// This is the text the player is shown in the scenario list, so a mission that arrives
/// without one is given a placeholder rather than an empty line.
/// </summary>
/// <param name="description">The description to record, or NULL if the mission has none.</param>
void MultiMission::Set_Description(char const * description)
{
	if (description != NULL) {
		strncpy(ScenarioDescription, description, ARRAY_SIZE(ScenarioDescription));
		ScenarioDescription[ARRAY_SIZE(ScenarioDescription) - 1] = '\0';
	} else {
		strcpy(ScenarioDescription, "No Description");
	}
}


/// <summary>
/// Sets the map filename for this mission.
/// </summary>
/// <param name="filename">The filename to record, or NULL if the mission has none.</param>
void MultiMission::Set_Filename(char const * filename)
{
	if (filename != NULL) {
		strncpy(Filename, filename, ARRAY_SIZE(Filename));
		Filename[ARRAY_SIZE(Filename) - 1] = '\0';
	} else {
		strcpy(Filename, "No File Name");
	}
}


/// <summary>
/// Sets the digest for this mission.
/// The digest is what the other players' copies of the map are checked against, so a
/// mission that arrives without one is given a placeholder rather than a blank.
/// </summary>
/// <param name="digest">The digest to record, or NULL if the mission has none.</param>
void MultiMission::Set_Digest(char const * digest)
{
	if (digest != NULL) {
		strncpy(Digest, digest, ARRAY_SIZE(Digest));
		Digest[ARRAY_SIZE(Digest) - 1] = '\0';
	} else {
		strcpy(Digest, "No Digest");
	}
}


/// <summary>
/// Sets whether this is an official mission.
/// </summary>
/// <param name="official">Is this one of the missions that shipped with the game?</param>
void MultiMission::Set_Official(bool official)
{
	IsOfficial = official;
}


/// <summary>
/// Updates the scenario loading progress and reports it to the other players.
/// The local gauge only ever moves forward, and a random map is allowed half of it since
/// generating the map is only half of the job. In a multiplayer game the new figure is sent
/// out so the other machines can see how far behind we are.
/// </summary>
/// <param name="percent">How much of the load has been completed, as a percentage.</param>
void SessionClass::Update_Progress(int percent)
{
	if (Debug_Map) return;

	int new_percent = percent;
	if (Scen->IsRandom) {
		new_percent = new_percent / 2;
	}

	if (Progress.Get_Current_Progress(0) * 100.0 < new_percent) {
		Progress.Set_Progress_Percent(0, new_percent);
	}

	if ( Players.Count() == 1 ) return;

	CDTimerClass<SystemTimerClass> timer = 4 * TIMER_SECOND;

	switch (Type) {
		case GAME_IPX:
		case GAME_INTERNET: {
				GlobalPacketType prog_packet;
				memset((void *)&prog_packet, 0, sizeof(prog_packet));

				prog_packet.Command = NET_PROGRESS_REPORT;
				prog_packet.Progress.Percent = int(100.0 * Progress.Get_Current_Progress(0));

				if (prog_packet.Progress.Percent < 99.95) {
					Ipx.Send_Global_Message(&prog_packet, sizeof(prog_packet), 0, NULL);
				}else{
					for (int p=1 ; p<Session.Players.Count() ; p++) {
						Ipx.Send_Global_Message(&prog_packet, sizeof(prog_packet), 1, &Session.Players[p]->Address);
					}
				}

				Call_Back();

				while (Ipx.Global_Num_Send() > 5 && timer > 0) {
					Sleep(20);
					Windows_Message_Handler();
					Call_Back();
				}
			}
			break;

		case GAME_MODEM:
		case GAME_NULL_MODEM: {
				SerialPacketType send_packet;
				memset((void *)&send_packet, 0, sizeof(send_packet));

				send_packet.Command = SERIAL_PROGRESS_REPORT;
				send_packet.Progress.Percent = int(100.0 * Progress.Get_Current_Progress(0));
				send_packet.ID = Session.ModemType;

				NullModem.Send_Message(&send_packet, sizeof(send_packet), 0);
				NullModem.Service();

				while (NullModem.Num_Send() > 5 && timer > 0) {
					Sleep(20);
					Keyboard->Check();
					NullModem.Service();
				}
			}
			break;

		default:
			break;
	}

	Call_Back();
}


/// <summary>
/// Sets up the fixed alliances for a clan game.
/// Westwood Online hands down the two squad rosters, and this routine allies the players
/// within each squad and then fixes the alliances so that nobody can change sides for the
/// rest of the match. An ordinary game is left to sort out its own alliances.
/// </summary>
void SessionClass::Init_Fixed_Alliances(void)
{
	HouseClass *hptr;
	int i;
	int j;

	SquadAlliances = false;

	if (Type != GAME_INTERNET) {
		return;
	}

	Scen->Special.IsAllianceFixed = false;

	char * names1 = WestwoodOnline_Clan1_Players;
	char * names2 = WestwoodOnline_Clan2_Players;

	DebugString("Squad 1 = %s\n", names1);
	DebugString("Squad 2 = %s\n", names2);

	if (strlen(names1) == 0 || strlen(names2) == 0) {
		return;
	}

	DynamicVectorClass<HouseClass *> squad1;
	DynamicVectorClass<HouseClass *> squad2;

	char * buffer = new char [MAX(strlen(names1), strlen(names2)) + 32];

	strcpy(buffer, names1);
	char * token = strtok(buffer, ",");

	while (token != NULL && *token != '\0') {
		for (i = 0; i < Houses.Count(); i++) {
			hptr = Houses[i];
			if (hptr->IsHuman && (_stricmp(hptr->IniName, token) == 0)) {
				squad1.Add(hptr);
			}
		}
		token = strtok(NULL, ",");
	}

	strcpy(buffer, names2);
	token = strtok(buffer, ",");

	while (token != NULL && *token != '\0') {
		for (i = 0; i < Houses.Count(); i++) {
			hptr = Houses[i];
			if (hptr->IsHuman && (_stricmp(hptr->IniName, token) == 0)) {
				squad2.Add(hptr);
			}
		}
		token = strtok(NULL, ",");
	}

	if (squad1.Count() > 1) {
		for (i = 0; i<squad1.Count(); i++) {
			for (j = 0; j < squad1.Count(); j++) {
				if (squad1[i]->HeapID != squad1[j]->HeapID) {
					squad1[i]->Make_Ally(squad1[j]);
				}
			}
		}
	}

	if (squad2.Count() > 1) {
		for (i = 0; i<squad2.Count(); i++) {
			for (j = 0; j < squad2.Count(); j++) {
				if (squad2[i]->HeapID != squad2[j]->HeapID) {
					squad2[i]->Make_Ally(squad2[j]);
				}
			}
		}
	}

	delete [] buffer;

	SquadAlliances = true;
	Scen->Special.IsAllianceFixed = true;
}


/// <summary>
/// Saves the game options to a save game.
/// The block is written with its own size in front of it, so that a later version of the
/// game can recognize an incompatible save rather than misread it.
/// </summary>
/// <returns>bool; Were the options written successfully?</returns>
bool GameOptionsType::Save(IStream * stream) const
{
	if (stream != NULL) {
		unsigned int size = sizeof(*this);
		HRESULT res = stream->Write(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			res = stream->Write(this, size, NULL);
			if (SUCCEEDED(res)) {
				return(true);
			}
		}
	}

	return(false);
}


/// <summary>
/// Loads the game options from a save game.
/// A block of a different size belongs to another version of the game and is refused
/// rather than read. The scenario index is invalidated on the way in, since the saved game
/// brings its own scenario with it.
/// </summary>
/// <returns>bool; Were the options read back successfully?</returns>
bool GameOptionsType::Load(IStream * stream)
{
	if (stream != NULL) {
		unsigned int size;
		HRESULT res = stream->Read(&size, sizeof(size), NULL);
		if (SUCCEEDED(res)) {
			if (size == sizeof(*this)) {
				res = stream->Read(this, size, NULL);
				ScenarioIndex = -1;
				if (SUCCEEDED(res)) {
					return(true);
				}
			}
		}
	}

	return(false);
}

/************************** end of session.cpp *****************************/
