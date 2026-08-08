/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "comtypes.h"

#include <comdef.h>

/// Names and comments from TLBs

EXTERN_C const IID IID_IPowerEvents;

struct IHouse;

struct __declspec(novtable)
IPowerEvents : public IUnknown
{
public:
	/*
	 * Triggered when power first becomes sufficient.
	 */
	virtual HRESULT STDMETHODCALLTYPE Power_Activated(void) = 0;

	/*
	 * Triggered when power first becomes insufficient.
	 */
	virtual HRESULT STDMETHODCALLTYPE Power_Lost(void) = 0;
};

/*
 * IPowerEvents com smart pointer declaration.
 */
_COM_SMARTPTR_TYPEDEF(IPowerEvents, IID_IPowerEvents);
