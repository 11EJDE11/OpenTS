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

class ModemGuest
{
	friend BOOL CALLBACK Modem_Guest_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	public:
		ModemGuest(void){}
		bool Dialog(void);

	private:
		void InitDialog(void);
		void ReadDialog(void);
		void SendCurrentMessage(void);
		void SetHouse(void);
		void SetColor(void);
		void SetName(bool update);
		void SetOther(const char * name, int color, int house);
		void SetSettings(void);
		void AcceptOptions(void);
		void Send(bool force);
		bool Receive(void);
		bool SetGameOptions(SerialPacketType * packet);
		void SendGameOptions(void);
		void SendAMessage(const char * message);
		void DrawMessage(const char * name, int color, const char * message);
		void SignOff(void);
		bool StartGame(bool load_game);
		void ReceivePreview(void);

	private:
		/*
		 * This is the window handle of the modem setup dialog, valid only while it is up.
		 */
		HWND _Dialog;

		/*
		 * This is the code the dialog will close with. It starts out negative and the dialog
		 * loop spins until the host's go signal makes it IDOK or the player backs out with
		 * IDCANCEL.
		 */
		int RC;

		/*
		 * If the player has altered any of their own settings, then this flag will be true.
		 * The multiplayer settings are written back out when the dialog closes, so that the
		 * choices carry over to the next game.
		 */
		bool Changed;

		/*
		 * If this player's settings are due to go out to the host, then this flag will be
		 * true. It is set whenever the name, color or side changes and cleared once the
		 * options packet has been sent.
		 */
		bool Transmit;

		/*
		 * This is when the options packet was last sent, expressed in game ticks. Nothing is
		 * retransmitted until PACKET_RETRANS_TIME has gone by, which keeps a slow modem link
		 * from being flooded with duplicates.
		 */
		unsigned int TransmitTime;

		/// Unused
		unsigned int TheirResponseTime;

		/*
		 * This is when the last packet arrived from the host, expressed in game ticks. If the
		 * silence stretches past MsgTimeout then the link is presumed dead.
		 */
		unsigned int LastMsgTime;

		/*
		 * This is how long a silence will be tolerated before the other system is declared
		 * unresponsive, expressed in game ticks (1200, or 20 seconds).
		 */
		unsigned int MsgTimeout;

		/*
		 * This is when the last timing packet was sent, expressed in game ticks. One goes out
		 * every PACKET_TIMING_TIMEOUT ticks so that the host can measure the round trip and
		 * pick the frame timing from it.
		 */
		unsigned int TimingTime;

		/*
		 * This is the host's player name, as it arrived with the game options.
		 */
		char TheirName[20];

		/*
		 * This is the color the host has taken. The guest is bumped along to the next color
		 * whenever it would otherwise clash.
		 */
		int TheirColor;

		/*
		 * This is the side the host has taken.
		 */
		int TheirHouse;

		/*
		 * These are the unique IDs of the local rules, art and AI control files. The host
		 * sends its own along with the game options and any mismatch aborts the connection --
		 * neither machine can simulate the same game if the other is running altered data.
		 */
		int CheatCheck;
		int ArtCheatCheck;
		int AICheatCheck;
};
