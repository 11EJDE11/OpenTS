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

/* $Header: /CounterStrike/TCPIP.H 1     3/03/97 10:25a Joe_bostic $ */
/***************************************************************************
 **   C O N F I D E N T I A L --- W E S T W O O D    S T U D I O S        **
 ***************************************************************************
 *                                                                         *
 *                 Project Name : Command & Conquer                        *
 *                                                                         *
 *                    File Name : TCPIP.CPP                                *
 *                                                                         *
 *                   Programmer : Steve Tall                               *
 *                                                                         *
 *                   Start Date : March 11th, 1996                         *
 *                                                                         *
 *                  Last Update : March 11th, 1996 [ST]                    *
 *                                                                         *
 *-------------------------------------------------------------------------*
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *-------------------------------------------------------------------------*
 * Functions:                                                              *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once

#define FORCE_WINSOCK				1

#define WINSOCK_MINOR_VER		1
#define WINSOCK_MAJOR_VER		1
#define PORTNUM						0x1000
#define UDP_PORT					0x1001
#define WS_NUM_TX_BUFFERS		16		//Must be a power of 2
#define WS_NUM_RX_BUFFERS		16		//MUst be a power of 2
#define WS_RECEIVE_BUFFER_LEN	1024
//#define WS_IN_BUFFER_LEN			8192
//#define WS_OUT_BUFFER_LEN		8192

#define PLANET_WESTWOOD_HANDLE_MAX 20
#define PLANET_WESTWOOD_PASSWORD_MAX 20
#define IP_ADDRESS_MAX 40
#define PORT_NUMBER_MAX 6

//...........................................................................
// Custom messages: WM_USER + 1 to WM_USER + 100
// These will be sent to the dialog procedure, for display only.
//...........................................................................
#define	WM_UPDATE_STATUS		(WM_USER + 1)	// update status text
#define	WM_UPDATE_CLIENTS		(WM_USER + 2)	// update client list box
#define	WM_UPDATE_MESSAGE		(WM_USER + 3)	// update received message list

//...........................................................................
// Messages for Async processing.
//...........................................................................
#define	WM_ACCEPT				(WM_USER + 101)	// client wants to connect
#define	WM_HOSTBYADDRESS		(WM_USER + 102)	// async get host by address
#define	WM_HOSTBYNAME			(WM_USER + 103)	// async get host by name
#define	WM_ASYNCEVENT			(WM_USER + 104)	// other Async event
#define	WM_UDPASYNCEVENT		(WM_USER + 105)	// UDP socket Async event

