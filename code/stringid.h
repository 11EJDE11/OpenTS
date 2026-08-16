/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include <cassert>


template<int SIZE>
struct TStringID
{
	public:
		TStringID(void)
		{
			StringBuffer[0] = '\0';
		}

		TStringID(char const * string)
		{
			if (string == NULL) {
				StringBuffer[0] = '\0';
			} else {
				strncpy(StringBuffer, string, SIZE);
				StringBuffer[SIZE] = '\0';
			}
		}

		TStringID(TStringID const & that) = default;

		TStringID(NoInitClass const & x) {}

		operator char * (void) { return(StringBuffer); }
		operator char const * (void) const { return(StringBuffer); }

		operator bool (void) const {return(strlen(StringBuffer) != 0);}

		char & operator[](int index) { return (StringBuffer[index]); }
		char const & operator[](int index) const { return (StringBuffer[index]); }

		bool operator==(char const * string) const { return (stricmp(StringBuffer, string) == 0); }
		bool operator!=(char const * string) const { return (stricmp(StringBuffer, string) != 0); }
		bool operator==(TStringID const & that) const { return (strcmp(that.StringBuffer, StringBuffer) == 0); }
		bool operator!=(TStringID const & that) const { return (strcmp(that.StringBuffer, StringBuffer) != 0); }


		TStringID & operator=(TStringID const & that) = default;


		/// Returns the size of the buffer
		int Size(void) const
		{
			return(SIZE+1);
		}

		/// Returns the length of the string in the buffer.
		int Length(void) const
		{
			return(strlen(StringBuffer));
		}

		/// Carries the string to or from a save game.
		template<typename S>
		void Serialize(S & stream)
		{
			stream.Serialize(StringBuffer);
		}

	private:
		/*
		 * This is the string storage, embedded in the object rather than allocated -- SIZE
		 * characters plus the slot for the null terminator. A longer string assigned to this
		 * object is silently truncated to fit.
		 */
		char StringBuffer[SIZE + 1];
};
