/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "session.h"

class ModemHost
{
		friend BOOL CALLBACK Modem_Host_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
		friend bool ModemHostCallback(void);

	public:
		ModemHost(void){}
		bool Dialog(void);

	private:
		void InitDialog(void);
		bool ReadDialog(void);
		void SendCurrentMessage(void);
		void SetHouse(void);
		void SetColor(void);
		void SetName(bool update);
		void SetOther(const char * name, int color, int house);
		void SetUnitCount(void);
		void SetBuildLevel(void);
		void SetCredits(void);
		void SetAIPlayers(void);
		void SetGameSpeed(void);
		void SetAIDifficulty(void);
		void SetMCVRedeploy(void);
		void SetBases(void);
		void SetGoodies(void);
		void SetFogOfWar(void);
		void SetBridgeDestruction(void);
		void SetHarvTruce(void);
		void SetShortGame(void);
		void SetCrapEngineers(void);
		void SelectMultiMap(void);
		void SelectRandomMap(void);
		void Send(bool force);
		bool Receive(bool silent);
		void SendGameOptions(void);
		void SendAMessage(const char * message);
		void DrawMessage(const char * name, int color, const char * message);
		void SignOff(void);
		bool StartGame(bool load_game);
		void SendPreview(void);

	private:
		/*
		 * This is the window handle of the host dialog, which every option routine reaches
		 * its controls through.
		 */
		HWND _Dialog;

		/*
		 * This is the identifier of the button that closed the dialog, or -1 while it is
		 * still running. The message loop spins until a control notification puts one here.
		 */
		int RC;

		/*
		 * If the host has altered any of the multiplayer settings that belong to this
		 * machine -- its name, color or side -- then this flag will be true. The settings
		 * are written back out when the dialog closes.
		 */
		bool Changed;

		/*
		 * If the game options have been fiddled with since they were last sent, then this
		 * flag will be true, and the options are transmitted again so that the guest's
		 * dialog follows along.
		 */
		bool Transmit;

		/*
		 * This is the tick the game options were last transmitted at, which holds the
		 * retransmissions down to the retry interval.
		 */
		unsigned int TransmitTime;

		/*
		 * This is the guest's round trip time as reported in its timing packets, the larger
		 * of the two machines' times deciding the game's MaxAhead. Until a timing packet
		 * arrives it holds 10000, which stands for "not measured yet".
		 */
		unsigned int TheirResponseTime;

		/*
		 * This is the tick a packet last arrived at, compared against the MsgTimeout to
		 * decide that the link has gone dead.
		 */
		unsigned int LastMsgTime;

		/*
		 * This is how long, expressed in ticks, the link may stay quiet before the guest is
		 * declared unresponsive and the dialog torn down.
		 */
		unsigned int MsgTimeout;

		/*
		 * This is the tick a timing packet was last sent at. One goes out periodically so
		 * that both machines can agree on how sluggish the connection is.
		 */
		unsigned int TimingTime;

		/*
		 * If the guest has answered with a game options packet of its own, then this flag
		 * will be true. Until it does, every timing packet that arrives prods the host into
		 * transmitting the options again.
		 */
		bool GameOptions;

		/*
		 * These describe the guest as it last identified itself, and are displayed in the
		 * opponent field of the dialog. The color and house start out invalid so that the
		 * first packet always reads as a change.
		 */
		char TheirName[20];
		int TheirColor;
		int TheirHouse;

		/*
		 * These are the unique identifiers of our rules, art and AI control files. They are
		 * sent with the game options and compared against the guest's, so that a player
		 * running altered files is turned away rather than let into a game that would
		 * desynchronize.
		 */
		int CheatCheck;
		int ArtCheatCheck;
		int AICheatCheck;
};

bool ModemHostCallback(void);
