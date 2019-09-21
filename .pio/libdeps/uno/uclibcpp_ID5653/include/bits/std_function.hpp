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

#include <basic_definitions>
#include <memory>
#include <cstddef>
#include <typeinfo>

namespace std {
	/** @class std::bad_function_call functional
	 * @brief std::bad_function_call is the type of the exception thrown by std::function::operator()
	 *   if the function wrapper has no target.
	 * 
	 * @since c++11
	 */
	class bad_function_call : public exception {
	public:
		~bad_function_call() override = default;
		const char* what() const
		#if __is_cpp_std_11
			noexcept
		#endif
		override { return "bad function call"; }
	};

	namespace detail {
#if !__is_cpp_std_17
	// DELETEME: use std::tuple instead
		template <class ...Args> struct maybe_binary_or_unary_function { };
		template <class Arg> struct maybe_binary_or_unary_function<Arg> {
			using argument_type = Arg;
		};
		template <class Arg1, class Arg2> struct maybe_binary_or_unary_function<Arg1, Arg2> {
			using first_argument_type = Arg1;
			using second_argument_type = Arg2;
		};
#endif

		// TODO: implement
		template <class Functor, class Return, class ...Args> struct is_invocable_r : true_type { };
	}

	/** @class std::function functional
	 * @brief Class template std::function is a general-purpose polymorphic function wrapper.
	 * 
	 * Instances of std::function can store, copy, and invoke any Callable target -- functions,
	 * lambda expressions, bind expressions, or other function objects,
	 * as well as pointers to member functions and pointers to data members.
	 * 
	 * The stored callable object is called the target of std::function.
	 * If a std::function contains no target, it is called empty.
	 * Invoking the target of an empty std::function results in std::bad_function_call exception being thrown.
	 * std::function satisfies the requirements of CopyConstructible and CopyAssignable.
	 * 
	 * @tparam Functor - function type
	 * 
	 * @since c++11
	 */
	template <class Functor> class _UCXXEXPORT function; /*undefined*/
	template <class Return, class ...Args> class function<Return(Args...)>
	#if !__is_cpp_std_17
		: detail::maybe_binary_or_unary_function<Args...>
	#endif
	{
		// DELETEME: use std::reference_wrapper instead
		class invocable {
		public:
			virtual ~invocable() = default;
			virtual invocable *copy() const = 0;
		#if __RTTI_ENABLED_
			virtual const type_info& target_type() const noexcept = 0;
		#endif
			virtual void *target() noexcept = 0;
			virtual const void *target() const noexcept = 0;
			virtual Return invoke(Args &&...) const = 0;
		};
		template <class Functor> class functor : public invocable {
			Functor _f;
		public:
			using functor_type = Functor;
			functor(const Functor &f) : _f(f) { }
			functor(Functor &&f) : _f(std::move(f)) { }
			~functor() override = default;
			functor *copy() const override {
				return new functor(Functor(_f));
			}
		#if __RTTI_ENABLED_
			const type_info& target_type() const noexcept override { return typeid(_f); }
		#endif
			void *target() noexcept override { return reinterpret_cast<void *>(&_f); }
			const void *target() const noexcept { return reinterpret_cast<const void *>(&_f); }
			Return invoke(Args &&...args) const override { return _f(forward<Args>(args)...); }
		};

		unique_ptr<invocable> _target;

	public:
		using result_type = Return;
		
		// @brief Creates an empty function.
		function(nullptr_t p = nullptr) noexcept : _target(nullptr) { }
		// @brief Copies the target of other to the target of *this.
		function(const function &other) { *this = other; }
		// @brief Moves the target of other to the target of *this.
		function(function &&other) { *this = std::move(other); }
		/* @brief Initializes the target with std::move(f).
		 * @tparam Functor - must meet the requirements of Callable.
		 */
		template <class Functor, class = typename enable_if<
			detail::is_invocable_r<Functor, Return, Args...>::value
		>::type> function(Functor f) {
			_target.reset(new functor<Functor>(std::move(f)));
		}

		// @brief Assigns a copy of target of other.
		function &operator =(const function &other) {
			_target = other._target->copy();
			return *this;
		}
		// @brief Moves the target of other to *this.
		function &operator =(function &&other) {
			_target = std::move(other._target);
			return *this;
		}
		// @brief Drops the current target.
		function &operator =(nullptr_t) {
			_target = nullptr;
			return *this;
		}
		/* @brief Sets the target of *this to the callable f.
		 * @tparam Functor - must meet the requirements of Callable.
		 */
		template <class Functor> typename enable_if<
			detail::is_invocable_r<Functor, Return, Args...>::value, function &
		>::type operator =(Functor f) {
			_target = make_unique<functor<Functor>>(f);
			return *this;
		}
		// TODO: implement std::referece_wrapper
		// /* @brief Sets the target of *this to a copy of f.
		//  * @tparam Functor - must meet the requirements of Callable.
		//  */
		// template <class Functor> typename enable_if<
		// 	detail::is_invocable_r<Functor, Return, Args...>::value, function &
		// >::type operator =(reference_wrapper<Functor> f) {
		// 	_target = reference_wrapper<Functor>(f);
		// 	return *this;
		// }

		// @brief Exchanges the stored callable objects of *this and other.
		void swap(function &other) noexcept {
			_target.swap(other._target);
		}
	#if !__is_cpp_std_14
		// TODO: assign with alloc
		// @brief Initializes the target with f.
		template <class Functor/*, class Alloc*/> void assign(Functor &&f/*, const Alloc& alloc*/) {
			function(forward<Functor>(f)).swap(*this);
		}
	#endif

		// @brief Checks whether *this stores a callable function target, i.e. is not empty.
		explicit operator bool() const noexcept {
			return _target != nullptr;
		}

		// @brief Invokes the stored callable function target with the parameters args.
		Return operator()(Args &&...args) const {
			if (_target == nullptr) {
				throw bad_function_call();
			}
			return _target->invoke(forward<Args>(args)...);
		}

	#if __RTTI_ENABLED_
		// @brief Returns the type of the stored function.
		const type_info& target_type() const noexcept {
			return _target != nullptr ? _target->target_type() : typeid(void);
		}
	#endif

		/* @brief Returns a pointer to the stored callable function target.
		 * @tparam T - must be equal to stored function.
		 */
	#if __RTTI_ENABLED_
		template <class T> T *target() noexcept {
			if (_target != nullptr && typeid(T) == target_type()) {
				return reinterpret_cast<T *>(_target->target());
			} else {
				return nullptr;
			}
		}
	#else
		template <class T> T *target() noexcept {
			return reinterpret_cast<T *>(_target->target());
		}
	#endif
	#if __RTTI_ENABLED_
		template <class T> const T *target() const noexcept {
			if (_target != nullptr && typeid(T) == target_type()) {
				return reinterpret_cast<const T *>(_target->target());
			} else {
				return nullptr;
			}
		}
	#else
		template <class T> const T *target() const noexcept {
			return reinterpret_cast<T *>(_target->target());
		}
	#endif
	};
	template <class Return, class ...Args> class function<Return(*)(Args...)> : public function<Return(Args...)> {
	public:
		using function<Return(Args...)>::function;
	};
	template <class Functor, class Return, class ...Args> class function<Return(Functor::*)(Args...)> : public function<Return(Args...)> {
	public:
		using function<Return(Args...)>::function;
	};

	template <class Return, class ...Args> void swap(function<Return(Args...)> &lhs, function<Return(Args...)> &rhs) {
		lhs.swap(rhs);
	}
}

template <class Return, class ...Args> bool operator ==(const std::function<Return(Args...)> &f, std::nullptr_t) noexcept {
	return !static_cast<bool>(f);
}
template <class Return, class ...Args> bool operator ==(std::nullptr_t, const std::function<Return(Args...)> &f) noexcept {
	return !static_cast<bool>(f);
}
template <class Return, class ...Args> bool operator !=(const std::function<Return(Args...)> &f, std::nullptr_t) noexcept {
	return static_cast<bool>(f);
}
template <class Return, class ...Args> bool operator !=(std::nullptr_t, const std::function<Return(Args...)> &f) noexcept {
	return static_cast<bool>(f);
}
