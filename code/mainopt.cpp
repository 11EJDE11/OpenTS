/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "mainopt.h"

#include "_map.h"
#include "_mixfile.h"
#include "_rect.h"
#include "_surface.h"
#include "convert.h"
#include "data.h"
#include "dbgprint.h"
#include "dsaudio.h"
#include "dsurface.h"
#include "gamedlg.h"
#include "globals.h"
#include "init.h"
#include "language\language.h"
#include "misc.h"
#include "mixfile.h"
#include "msgbox.h"
#include "newmenu.h"
#include "ownrdraw.h"
#include "sidebar.h"
#include "sounddlg.h"
#include "stimer.h"
#include "surface.h"
#include "wwmouse.h"

#include "color.hh"


BOOL CALLBACK Main_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
BOOL CALLBACK Display_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
bool Change_Display_Mode(int width, int height);
bool Test_Display_Mode_Dialog(int width, int height);
BOOL CALLBACK Test_Display_Mode_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

GameOptionsClass TempOptions;


/// <summary>
/// Brings up the main options dialog.
/// This routine drives the options menu, dispatching to the sound, display, network,
/// keyboard and game settings dialogs until the player backs out. A resolution change is
/// offered as a trial first, and the settings are written out when the player leaves.
/// </summary>
/// <remarks>Game logic is suspended for the duration of this routine.</remarks>
void Main_Options_Dialog(void)
{
	bool old_game_active = GameActive;
	GameActive = false;

	HWND main_handle;
	LONG main_rc;

	HWND in_handle;
	LONG in_rc;

	while (true) {
		do {
			main_rc = -1;
			main_handle = OwnerDraw::Begin_Dialog(IDD_OPT_MAIN, Main_Options_Dialog_Proc);
		} while (main_handle == 0);
		SetWindowLong(main_handle, DWL_USER, (LONG)&main_rc);

		OwnerDraw::Move_Dialog(main_handle, -1, (HiddenSurface->Get_Height() - 400) / 2 + 147);
		OwnerDraw::Display_Dialog(main_handle);

		while (main_rc < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
		}

		OwnerDraw::End_Dialog(main_handle);

		switch (main_rc) {
			case IDC_OPTMAIN_SOUND:
				SoundControlsClass().Dialog();
				break;

			case IDC_OPTMAIN_DISPLAY: {
				while (true) {
					do {
						TempOptions = Options;
						in_rc = -1;
						in_handle = OwnerDraw::Begin_Dialog(IDD_OPT_DISPLAY, Display_Options_Dialog_Proc);
					} while (in_handle == 0);
					SetWindowLong(in_handle, DWL_USER, (LONG)&in_rc);
					OwnerDraw::Display_Dialog(in_handle);

					while (in_rc < 0) {
						if (OwnerDraw::Dialog_Message_Handler() == true) {
							break;
						}
						Title_Screen_Restore();
					}

					OwnerDraw::End_Dialog(in_handle);

					if (in_rc != 1) {
						break;
					}
					if (TempOptions.ScreenWidth == Options.ScreenWidth && TempOptions.ScreenHeight == Options.ScreenHeight) {
						break;
					}

					if (Debug_IngameModeChange) {
						Options.ScreenWidth = TempOptions.ScreenWidth;
						Options.ScreenHeight = TempOptions.ScreenHeight;
					} else {
						if (WWMessageBox().Process(TXT_ABOUT_TO_TRY_MODE, TXT_OK, TXT_CANCEL) == 0) {
							if (!Test_Display_Mode_Dialog(TempOptions.ScreenWidth, TempOptions.ScreenHeight)) {
								continue;
							}
							Options.ScreenWidth = TempOptions.ScreenWidth;
							Options.ScreenHeight = TempOptions.ScreenHeight;
						}
					}

					break;
				}
			}
			break;

			case IDC_OPTMAIN_KEYBOARD:
				Options.Hotkey_Dialog();
				break;

			case IDC_OPTMAIN_GAME_SETTINGS:
				GameControlsClass().Dialog();
				break;

			default:
				Options.Save_Settings();
				GameActive = old_game_active;
				return;
		}
	}
}


/// <summary>
/// Handles the main options dialog.
/// This routine reports the button the player pressed back to the options dialog driver so
/// that it can bring up the appropriate sub dialog. The sound button is disabled when there
/// is no audio hardware to talk to.
/// </summary>
BOOL CALLBACK Main_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int *result;
	HWND handle;

	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		result = (int *)GetWindowLong(window, DWL_USER);
		switch (message) {

			case WM_COMMAND:
				*result = LOWORD(wparam);
				break;

			case WM_INITDIALOG:
				handle = GetDlgItem(window, IDC_OPTMAIN_SOUND);
				if (handle) {
					EnableWindow(handle, Audio_Available());
				}
				break;

		}
		return(0);
	}
	return(rc);
}


/// <summary>
/// Switches the game over to a new screen resolution.
/// This routine tears down every drawing surface and the mouse handler, sets the new video
/// mode, then builds all of them back up and hands the tactical map its new dimensions. The
/// hicolor remap tables are rebuilt as well, since the new mode may pack its pixels
/// differently.
/// </summary>
/// <param name="width">The width of the display mode to change to.</param>
/// <param name="height">The height of the display mode to change to.</param>
/// <returns>bool; Was the mode changed? If not, the previous mode has been restored.</returns>
bool Change_Display_Mode(int width, int height)
{
	DebugString("About to set video mode\n");

	int redleft = DSurface::Get_Red_Left();
	int redright = DSurface::Get_Red_Right();
	int greenleft = DSurface::Get_Green_Left();
	int greenright = DSurface::Get_Green_Right();
	int blueleft = DSurface::Get_Blue_Left();
	int blueright = DSurface::Get_Blue_Right();

	Hide_Mouse();

	Wait_Blit();

	if (WindowedMode == true) {
		VisibleRect = Rect(0, 0, width, height);
		VideoModeWidth = width;
		VideoModeHeight = height;
		DebugString("VisibleRect: %dx%d\n", width, height);
	} else {
		if (MouseCursor != NULL) {
			DebugString("Removing mouse handler\n");
			delete MouseCursor;
			MouseCursor = NULL;
		}

		if (VisibleSurface != NULL) {
			DebugString("Deleting primary surface\n");
			delete VisibleSurface;
			VisibleSurface = NULL;
		}

		if (!Set_Video_Mode(MainWindow, width, height, 16)) {
			DebugString("Set_Video_Mode failed.\n");

			DebugString("Restoring primary surface\n");
			VisibleSurface = DSurface::Create_Primary();

			DebugString("Restoring mouse handler\n");
			MouseCursor = new WWMouseClass(VisibleSurface, MainWindow);
			MouseCursor->Capture_Mouse();

			void const * mouseshp = MFCD::Retrieve("MOUSE.SHP");
			if (mouseshp != NULL) {
				Point2D hotspot = Point2D(0, 0);
				Set_Mouse_Cursor(hotspot, (ShapeSet const *)mouseshp, 0);
			}

			Map.Set_Default_Mouse(MOUSE_NORMAL);
			Map.Override_Mouse_Shape(MOUSE_NORMAL);

			Show_Mouse();

			return(false);
		}

		DebugString("Mode apparently set OK\n");
		VisibleRect = Rect(0, 0, width, height);
		DebugString("VisibleRect: %dx%d\n", width, height);
	}

	if (AlternateSurface != NULL) {
		DebugString("Deleting AlternateSurface\n");
		delete AlternateSurface;
		AlternateSurface = NULL;
	}

	if (HiddenSurface != NULL) {
		DebugString("Deleting HiddenSurface\n");
		delete HiddenSurface;
		HiddenSurface = NULL;
	}

	if (TileSurface != NULL) {
		DebugString("Deleting TileSurface\n");
		delete TileSurface;
		TileSurface = NULL;
	}

	if (SidebarSurface != NULL) {
		DebugString("Deleting SidebarSurface\n");
		delete SidebarSurface;
		SidebarSurface = NULL;
	}

	if (CompositeSurface != NULL) {
		DebugString("Deleting CompositeSurface\n");
		delete CompositeSurface;
		CompositeSurface = NULL;
	}

	if (WindowedMode == false) {
		DebugString("Setting new window size to %dx%d\n", width, height);
		SetWindowPos(MainWindow, HWND_TOP, 0, 0, width, height, SWP_NOMOVE);
		DebugString("Creating primary surface\n");
		VisibleSurface = DSurface::Create_Primary();
	} else {
		DebugString("Setting new window size to %dx%d\n", width, height);
		RECT windowrect;
		SetRect(&windowrect, 0, 0, width, height);
		AdjustWindowRectEx(&windowrect, GetWindowLong(MainWindow, GWL_STYLE), GetMenu(MainWindow) != 0, GetWindowLong(MainWindow, GWL_EXSTYLE));
		MoveWindow(MainWindow, 0, 0, windowrect.right - windowrect.left, windowrect.bottom - windowrect.top, TRUE);
	}

	Rect temp = VisibleRect;
	temp.X = ((Options.IsSidebarOnRight || Debug_Map) ? 0 : SidebarClass::SIDE_WIDTH);
	temp.Y = 16;
	temp.Width -= SidebarClass::SIDE_WIDTH;
	temp.Height -= 16;

	Allocate_Surfaces(VisibleRect, Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, temp.Width, VisibleRect.Height), Rect(0, 0, SidebarClass::SIDE_WIDTH, VisibleRect.Height));
	LogicalSurface = HiddenSurface;

	DebugString("Recalc color remap tables\n");
	ConvertClass::Reinitialize_Hicolor_Tables(redleft, redright, greenleft, greenright, blueleft, blueright);

	if (MouseCursor == NULL) {
		MouseCursor = new WWMouseClass(VisibleSurface, MainWindow);

		MouseCursor->Capture_Mouse();

		void const * mouseshp = MFCD::Retrieve("MOUSE.SHP");
		if (mouseshp != NULL) {
			Point2D hotspot = Point2D(0, 0);
			Set_Mouse_Cursor(hotspot, (ShapeSet const *)mouseshp, 0);
		}

		Map.Set_Default_Mouse(MOUSE_NORMAL);
		Map.Override_Mouse_Shape(MOUSE_NORMAL);

		Show_Mouse();

	} else {
		((WWMouseClass*)MouseCursor)->Calc_Confining_Rect();
	}

	Map.Set_View_Dimensions(temp);

	Map.Init_IO();
	Map.Activate(
#ifdef _DEBUG
		Debug_Map == true ? 1 : 0
#else
		1
#endif
	);
	Map.Reposition_Sidebar();
	Map.Flag_To_Redraw(GS_REDRAW_ALL);
	Show_Mouse();

	DebugString("Mode change complete.\n");

	return(true);
}


/// <summary>
/// Tries a display mode out and asks the player to confirm it.
/// This routine switches to the requested mode and puts up a confirmation dialog. If the
/// player does not accept the mode -- or says nothing at all, because a bad mode may well
/// leave the screen unreadable -- the previous resolution is restored.
/// </summary>
/// <param name="width">The width of the display mode to try.</param>
/// <param name="height">The height of the display mode to try.</param>
/// <returns>bool; Was the new display mode accepted and left in place?</returns>
bool Test_Display_Mode_Dialog(int width, int height)
{
	int rc = -1;

	DebugString("Testing display mode @ %dx%d\n", width, height);
	Hide_Mouse();
	HiddenSurface->Fill(TBLACK);
	Update_Visible_Surface();

	if (!Change_Display_Mode(width, height)) {
		return(false);
	}

	HiddenSurface->Fill(TBLACK);
	Update_Visible_Surface();
	Show_Mouse();
	Draw_Menu_Background();

	HWND dialog = OwnerDraw::Begin_Dialog(IDD_OPT_CONFIRM_MODE, Test_Display_Mode_Dialog_Proc);
	if (dialog) {
		SetWindowLong(dialog, DWL_USER, (LONG)&rc);
		OwnerDraw::Display_Dialog(dialog);

		CDTimerClass<SystemTimerClass> timer = 10 * TIMER_SECOND;
		while (rc < 0) {
			if (OwnerDraw::Dialog_Message_Handler() == true) {
				break;
			}
			Title_Screen_Restore();
			if (timer <= 0) {
				PostMessage(dialog, WM_COMMAND, WM_DESTROY, 0);
				timer = 5 * TIMER_SECOND;
			}
		}

		OwnerDraw::End_Dialog(dialog);
		if (rc != IDOK) {
			DebugString("Resetting display mode @ %dx%d\n", Options.ScreenWidth, Options.ScreenHeight);
			Change_Display_Mode(Options.ScreenWidth, Options.ScreenHeight);
			LogicalSurface = HiddenSurface;
			return(false);
		}
	}

	DebugString("Keeping display mode @ %dx%d\n", width, height);
	LogicalSurface = HiddenSurface;
	return(true);
}


/// <summary>
/// Handles the mode confirmation dialog.
/// This routine records the button the player pressed so that the mode test can tell
/// whether the new resolution was accepted or rejected.
/// </summary>
BOOL CALLBACK Test_Display_Mode_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int * result;
	int id;

	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		result = (int *)GetWindowLong(window, DWL_USER);
		switch (message) {
			case WM_COMMAND:
				id = LOWORD(wparam);
				if (id > 0 && id <= IDCANCEL) {
					*result = LOWORD(wparam);
				}
				break;
		}
		return(0);
	}
	return(rc);
}


/// <summary>
/// Handles the display options dialog messages.
/// This routine fills the resolution list with the display modes the hardware reports,
/// remembers which one the player picked, and tracks the movie stretching preference. The
/// chosen resolution is staged in the temporary options so that it can be tested before
/// being made permanent.
/// </summary>
static __forceinline BOOL Display_Options_Dialog_Body(HWND window, UINT message, WPARAM wparam)
{
	enum {
		MIN_WIDTH = 640,
		MIN_HEIGHT = 400,
		MAX_WIDTH = 800,
		MAX_HEIGHT = 600,
		MAX_HI_WIDTH = 4096,
		MAX_HI_HEIGHT = 4096,
	};

	static int * _modes = NULL;
	static int _current_mode = -1;
	static int _previous_mode = -1;
	static bool _initialized = true;

	int * result = (int *)GetWindowLong(window, DWL_USER);
	switch (message) {
		case WM_COMMAND:
			switch (LOWORD(wparam)) {
				default:
					return(0);

				case IDC_DISPLAY_RESLIST: {
					HWND list = GetDlgItem(window, IDC_DISPLAY_RESLIST);
					_current_mode = ListBox_GetCurSel(list);
				}
				return(0);

				case IDOK: {
					if (_previous_mode != _current_mode) {
						Center_Window_Within_Window(window, MainWindow);
						HWND list = GetDlgItem(window, IDC_DISPLAY_RESLIST);
						if (list) {
							int index = ListBox_GetItemData(list, _current_mode);
							int * modes = &_modes[2 * index];
							TempOptions.ScreenWidth = modes[0];
							TempOptions.ScreenHeight = modes[1];
						}
					}
					HWND button = GetDlgItem(window, IDC_STRETCH_MOVIES);
					if (button) {
						Options.StretchMovies = Button_GetCheck(button) == BST_CHECKED;
					}
				}
				break;

				case IDCANCEL:
					break;
			}
			delete [] _modes;
			*result = LOWORD(wparam);
			break;

		case WM_INITDIALOG: {
			HWND list = GetDlgItem(window, IDC_DISPLAY_RESLIST);
			if (Options.AllowHiResModes) {
				_modes = EnumDisplayModes(MIN_WIDTH, MIN_HEIGHT, MAX_HI_WIDTH, MAX_HI_HEIGHT, 16);
			} else {
				_modes = EnumDisplayModes(MIN_WIDTH, MIN_HEIGHT, MAX_WIDTH, MAX_HEIGHT, 16);
			}
			int * modes = _modes;
			int item_index = 0;
			int initial_mode = -1;
			int mode_index = 0;
			if (modes != NULL) {
				while (*modes != 0) {
					int width = *modes++;
					int height = *modes++;
					if (Options.AllowHiResModes || width == MIN_WIDTH || width == MAX_WIDTH) {
						if (width == TempOptions.ScreenWidth && height == TempOptions.ScreenHeight) {
							initial_mode = mode_index;
						}
						char buffer[64];
						sprintf(buffer, "%d x %d x 16", width, height);
						int index = ListBox_AddString(list, buffer);
						ListBox_SetItemData(list, index, item_index);
						mode_index++;
					}
					item_index++;
				}
			}
			ListBox_SetCurSel(list, initial_mode);
			_initialized = true;
			_current_mode = initial_mode;
			_previous_mode = initial_mode;

			HWND button = GetDlgItem(window, IDC_STRETCH_MOVIES);
			if (button) {
				bool stretch = Options.StretchMovies == true && DSurface::AllowStretchBlits == true;
				Button_SetCheck(button, stretch != false);
				EnableWindow(button, (BOOL)DSurface::AllowStretchBlits);
			}
		}
		break;

	}
	return(0);
}


/// <summary>
/// Handles the display options dialog.
/// This routine gives the owner draw dialog system first refusal on the message and only
/// deals with what it leaves behind.
/// </summary>
BOOL CALLBACK Display_Options_Dialog_Proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	int rc = OwnerDraw::Default_Dialog_Proc(window, message, wparam, lparam);
	if (rc == 0) {
		return(Display_Options_Dialog_Body(window, message, wparam));
	}
	return(rc);
}
