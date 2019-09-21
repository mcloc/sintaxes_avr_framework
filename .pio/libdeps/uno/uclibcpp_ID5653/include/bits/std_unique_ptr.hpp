/*	Copyright (C) 2018 Anton Mikhaylov aka anion155
	This file is part of the uClibc++ Library.
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#pragma once

#include <utility>

namespace std {
	template <class T> class auto_ptr;

	/** @class std::unique_ptr memory
	 * @brief std::unique_ptr is a smart pointer that owns and manages another object
	 *   through a pointer and disposes of that object when the unique_ptr goes out of scope.
	 * 
	 * The object is disposed of using the associated deleter when either of the following happens:
	 * - the managing unique_ptr object is destroyed
	 * - the managing unique_ptr object is assigned another pointer via operator= or reset().
	 * The object is disposed of using a potentially user-supplied deleter by calling get_deleter()(ptr).
	 * The default deleter uses the delete operator, which destroys the object and deallocates the memory.
	 * 
	 * A unique_ptr may alternatively own no object, in which case it is called empty.
	 * 
	 * There are two versions of std::unique_ptr:
	 * 1) Manages a single object (e.g. allocated with new)
	 * 2) Manages a dynamically-allocated array of objects (e.g. allocated with new[])
	 * 
	 * The class satisfies the requirements of MoveConstructible and MoveAssignable,
	 * but not the requirements of either CopyConstructible or CopyAssignable.
	 * 
	 * @tparam T - storable type
	 * @tparam [Deleter = std::default_delete<T>] - must be FunctionObject or lvalue reference
	 *   to a FunctionObject or lvalue reference to function, callable with an argument of
	 *   type unique_ptr<T, Deleter>::pointer
	 * 
	 * @since c++11
	 */
	template <class T> class unique_ptr : public auto_ptr<T> {
	public:
		using auto_ptr<T>::auto_ptr;

		void swap(unique_ptr &o) noexcept {
			auto o_obj = o.release();
			auto t_obj = this->release();
			o.reset(t_obj);
			this->reset(o_obj);
		}

		unique_ptr &operator=(unique_ptr &&o) noexcept { this->reset(o.release()); return *this; }
		unique_ptr &operator =(T *o) { this->reset(o); return *this; }
		unique_ptr &operator =(nullptr_t) { this->reset(nullptr); return *this; }

		bool operator ==(const nullptr_t &) const { return this->get() == nullptr; }
		bool operator !=(const nullptr_t &) const { return this->get() != nullptr; }
	};

	template <class T, class ...Args> inline unique_ptr<T> make_unique(Args &&...args) {
		return unique_ptr<T>(new T(forward<Args>(args)...));
	}
}
