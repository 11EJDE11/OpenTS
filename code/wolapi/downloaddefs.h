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

#ifndef _DOWNLOADDEFS_H
#define _DOWNLOADDEFS_H

// CDownload statuses

#define DOWNLOADSTATUS_NONE				0
#define DOWNLOADSTATUS_GO				1
#define DOWNLOADSTATUS_CONNECTING		2
#define DOWNLOADSTATUS_LOGGINGIN		3
#define DOWNLOADSTATUS_FINDINGFILE		4
#define DOWNLOADSTATUS_QUERYINGRESUME	5
#define DOWNLOADSTATUS_DOWNLOADING		6
#define DOWNLOADSTATUS_DISCONNECTING	7
#define	DOWNLOADSTATUS_DONE				0

// CDownload return codes

#define DOWNLOAD_SUCCEEDED		S_OK
#define	DOWNLOAD_PARAMERROR		MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 1 )
#define DOWNLOAD_STATUSERROR	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 2 )
#define DOWNLOAD_NETWORKERROR	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 3 )
#define DOWNLOAD_FILEERROR		MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 4 )
#define DOWNLOAD_REENTERERROR	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 4 )

// CDownloadEvent return codes.

#define DOWNLOADEVENT_SUCCEEDED		S_OK
#define	DOWNLOADEVENT_FAILED		MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 5 )
#define	DOWNLOADEVENT_RESUME		S_OK
#define	DOWNLOADEVENT_DONOTRESUME	MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 6 )

// CDownloadEvent error codes for OnError()

#define DOWNLOADEVENT_NOSUCHSERVER			1
#define DOWNLOADEVENT_COULDNOTCONNECT		2
#define DOWNLOADEVENT_LOGINFAILED			3
#define DOWNLOADEVENT_NOSUCHFILE			4
#define DOWNLOADEVENT_LOCALFILEOPENFAILED	5
#define DOWNLOADEVENT_TCPERROR				6
#define DOWNLOADEVENT_DISCONNECTERROR		7

#endif
