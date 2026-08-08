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

#ifndef __FTPDEFS_H_INCLUDED__
#define __FTPDEFS_H_INCLUDED__


// CFtp return codes.

#define FTP_SUCCEEDED		S_OK
#define	FTP_FAILED			MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 1 )
#define FTP_TRYING			MAKE_HRESULT( SEVERITY_ERROR, FACILITY_ITF, 2 )


#endif
