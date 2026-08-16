/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "win.h"


// How the presented frame is filtered when the window is larger than it.
enum VideoScaleMode {
	VIDEO_SCALE_NEAREST,
	VIDEO_SCALE_LINEAR,
	VIDEO_SCALE_PIXELART,
};


// Where the game's frame lands inside the window. The frame keeps its aspect ratio, so
// the destination is centered and the window may show bars on two of its sides.
struct VideoScaleInfo
{
	int GameWidth;
	int GameHeight;
	int WindowWidth;
	int WindowHeight;
	int DestX;
	int DestY;
	int DestWidth;
	int DestHeight;
	float ScaleX;
	float ScaleY;
};


bool Video_Init(HWND window);
void Video_Shutdown(void);

bool Video_Set_Mode(int width, int height);
void Video_On_Resize(int width, int height);
void Video_On_Display_Change(void);

void Video_Mark_Dirty(void);
void Video_Present(void);
void Video_Present_If_Dirty(void);

VideoScaleInfo const & Video_Get_Scale_Info(void);

int * EnumDisplayModes(int minwidth, int minheight, int maxwidth, int maxheight);
