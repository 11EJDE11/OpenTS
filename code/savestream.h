/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once

#include "abstract.h"
#include "loco.h"
#include "swizzle.h"
#include "win.h"

#include <optional>
#include <source_location>
#include <type_traits>
#include <typeinfo>
#include <vector>

class SaveStreamClass;

/*
 * A pointer is worth swizzling only when the object it names announces a swizzle
 * identity as it loads. The two persistent roots do that; nothing else does.
 */
template<typename T>
concept SwizzleTarget = std::is_base_of_v<AbstractClass, std::remove_cv_t<T>>
	|| std::is_base_of_v<LocomotionClass, std::remove_cv_t<T>>;

template<typename T>
concept HasSerializeMember = requires(T & object, SaveStreamClass & stream) {
	object.Serialize(stream);
};

template<typename T>
concept HasSerializeOverload = requires(T & object, SaveStreamClass & stream) {
	Serialize_Custom(stream, object);
};


/*
 * This carries one object's members to and from a save game. The same Serialize call
 * reads or writes depending on the mode the stream was opened in, so a class describes
 * its members once and cannot drift between saving and loading.
 *
 * Two rules govern what may be handed to Serialize:
 *
 * A pointer must be serialized where it finally lives. The swizzle manager remembers
 * the address of the slot it has to patch, so a pointer routed through a temporary, or
 * held in storage that is reallocated afterwards, is fixed up over memory that no
 * longer belongs to the object.
 *
 * A union may travel as its raw image only while every alternative is trivially
 * copyable and free of pointers. One that gains a pointer has to be serialized arm by
 * arm, switched on whatever discriminates it.
 */
class SaveStreamClass
{
	public:
		enum ModeType {
			MODE_SAVE,
			MODE_LOAD
		};

		SaveStreamClass(IStream * stream, ModeType mode);

		bool Is_Saving(void) const {return(Mode == MODE_SAVE);}
		bool Is_Loading(void) const {return(Mode == MODE_LOAD);}

		/*
		 * The first stream failure freezes this object and every Serialize that follows
		 * does nothing, so a class lists its members without checking each one and the
		 * caller asks once whether the whole pass worked.
		 */
		HRESULT Result(void) const {return(ErrorCode);}
		bool Was_Error(void) const {return(FAILED(ErrorCode));}

		/*
		 * The version stamp of the save being read, or the one being written. A member
		 * added to a later format is serialized only when this reaches the version that
		 * introduced it.
		 */
		unsigned int Version(void) const {return(FormatVersion);}

		/*
		 * The stream underneath, for the sub-objects that are still framed by OLE.
		 */
		IStream * Get_Stream(void) const {return(Stream);}

		/*
		 * Names the record this stream is carrying, so that a pointer which nothing
		 * answers for can be reported against the object that asked for it.
		 */
		void Set_Context(char const * ownertype, uintptr_t ownerid = 0)
		{
			OwnerType = ownertype;
			OwnerID = ownerid;
		}

		void Serialize_Bytes(void * data, int length);

		/*
		 * Numbers and enumerations travel as their declared width.
		 */
		template<typename T> requires (std::is_arithmetic_v<T> || std::is_enum_v<T>)
		void Serialize(T & value)
		{
			Serialize_Bytes(&value, sizeof(value));
		}

		/*
		 * A pointer to a game object is written as the address the object occupied when
		 * the game was saved, which is the identity it announces on the way back in.
		 * Loading leaves the slot registered with the swizzle manager, which clears it
		 * until every object has arrived and the real address is known.
		 */
		template<SwizzleTarget T>
		void Serialize(T * & pointer, std::source_location const & where = std::source_location::current())
		{
			Serialize_Bytes((void *)&pointer, sizeof(pointer));

			if (Is_Loading() && !Was_Error()) {
				Swizzler.Swizzle((void **)&pointer, OwnerType, OwnerID, typeid(T).name(), where.file_name(), where.line());
			}
		}

		/*
		 * Anything that describes its own members.
		 */
		template<typename T> requires HasSerializeMember<T>
		void Serialize(T & object)
		{
			object.Serialize(*this);
		}

		/*
		 * A type that cannot be given a member of its own is served by a global
		 * Serialize_Custom overload instead.
		 */
		template<typename T> requires (!HasSerializeMember<T> && HasSerializeOverload<T>)
		void Serialize(T & object)
		{
			Serialize_Custom(*this, object);
		}

		/*
		 * Arrays of a fixed size. Numbers go out in one block; anything else is
		 * serialized in place, one element at a time.
		 */
		template<typename T, int N>
		void Serialize(T (&array)[N], std::source_location const & where = std::source_location::current())
		{
			if constexpr (std::is_arithmetic_v<T> || std::is_enum_v<T>) {
				Serialize_Bytes(array, sizeof(array));
			} else {
				for (int index = 0; index < N; index++) {
					/*
					 * The elements are serialized against the array's own call site, so a
					 * pointer that nothing answers for names the member rather than this loop.
					 */
					if constexpr (std::is_pointer_v<T>) {
						Serialize(array[index], where);
					} else {
						Serialize(array[index]);
					}
				}
			}
		}

		/*
		 * A vector travels as its length followed by its elements. Loading sizes it in
		 * full before any element is read, because an element holding a pointer
		 * registers the slot it occupies with the swizzle manager and a reallocation
		 * afterwards would leave that address behind.
		 */
		template<typename T>
		void Serialize(std::vector<T> & value, std::source_location const & where = std::source_location::current())
		{
			int count = (int)value.size();
			Serialize(count);

			if (Is_Loading()) {
				value.clear();
				if (count > 0) {
					value.resize(count);
				}
			}

			for (int index = 0; index < count; index++) {
				/*
				 * The elements are serialized against the vector's own call site, so a
				 * pointer that nothing answers for names the member rather than this loop.
				 */
				if constexpr (std::is_pointer_v<T>) {
					Serialize(value[index], where);
				} else {
					Serialize(value[index]);
				}
			}
		}

		/*
		 * An optional value is a flag followed by the value itself when there is one.
		 */
		template<typename T>
		void Serialize(std::optional<T> & value)
		{
			bool present = value.has_value();
			Serialize(present);

			if (Is_Loading()) {
				value.reset();
				if (present) {
					value.emplace();
				}
			}

			if (present) {
				Serialize(*value);
			}
		}

	private:
		IStream * Stream;
		ModeType Mode;
		HRESULT ErrorCode;
		unsigned int FormatVersion;

		/*
		 * The record this stream is carrying, named for the swizzle manager's report.
		 * Nothing on the save side needs it.
		 */
		char const * OwnerType;
		uintptr_t OwnerID;
};


/*
 * A bit field has no address to hand out, so its value makes the trip in an ordinary
 * variable. Assigning it back is harmless while saving.
 */
#define SERIALIZE_BIT(stream, field) \
	do { \
		bool serialize_bit = ((field) != 0); \
		(stream).Serialize(serialize_bit); \
		(field) = serialize_bit; \
	} while (false)


/*
 * The version stamp of the save game currently being read. Each object builds its own
 * stream inside IPersistStream::Load, which has no way to be told, so the value is left
 * here by the load as a whole.
 */
extern unsigned int LoadedSaveVersion;
