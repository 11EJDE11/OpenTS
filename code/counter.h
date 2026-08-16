/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "vector.h"
#include "win.h"

struct IStream;

class CounterClass : protected VectorClass<int>
{
public:
	CounterClass(void);
	~CounterClass(void);

	int Increment(int index);
	int Decrement(int index);

	int Value(int index) const;

	void Clear(void);

	int Total(void) const;

	HRESULT Save(IStream * stream);
	HRESULT Load(IStream * stream);

	bool Reserve(int index);

	/*
	 * Carries the counters, how many of them there are first and then the values
	 * themselves. Loading resizes the counter list before any value is read.
	 */
	template<typename S>
	void Serialize(S & stream)
	{
		int count = Length();
		stream.Serialize(count);

		if (stream.Is_Loading()) {
			VectorClass<int>::Clear();
			if (count > 0 && !Resize(count)) {
				return;
			}
		}

		for (int index = 0; index < count; index++) {
			stream.Serialize(Vector[index]);
		}
	}

	int operator[](int index) {return(Vector[index]);};

	enum {
		GROWTH_STEP = 10,
	};
};
