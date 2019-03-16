#pragma once

#include "tags.hpp"

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <memory>
#include <mutex>

namespace TST {
	template <class T, Ownership>
	struct Thread_private;

	template <class T, Ownership owning = Non_owner, Convertibility convertability = Waitable>
	struct Mutex_protected;

	template <class T, Ownership owning = Non_owner, Convertibility convertability = Waitable>
	struct Read_only;

	template <class T>
	struct Thread_private<T, Non_owner> {
		Thread_private() = default;
		Thread_private(T *tp)
			: t{tp} {}
		Thread_private(const Thread_private &) = delete;
		Thread_private(Thread_private &&other) noexcept
			: t{other.t} {
			other.t = nullptr;
		}
		Thread_private &operator=(const Thread_private &) = delete;
		Thread_private &operator=(Thread_private &&other) noexcept {
			std::swap(t, other.t);
			return *this;
		}

		T &operator*() {
			assert(t);
			return *t;
		}
		explicit operator bool() {
			return t != nullptr;
		}

		friend constexpr bool operator==(const Thread_private &lhs, std::nullptr_t) {
			return lhs.t == nullptr;
		}
		friend constexpr bool operator==(std::nullptr_t, const Thread_private &rhs) {
			return rhs == nullptr;
		}
		friend constexpr bool operator==(const Thread_private &lhs, const T *rhs) {
			return lhs.t == rhs;
		}
		friend constexpr bool operator==(const T *lhs, const Thread_private &rhs) {
			return rhs == lhs;
		}

		private:
		T *t = nullptr;

		friend struct Mutex_protected<T, Non_owner, None>;
		friend struct Mutex_protected<T, Non_owner, Checkable>;
		friend struct Mutex_protected<T, Non_owner, Waitable>;
		friend struct Read_only<T, Non_owner, None>;
		friend struct Read_only<T, Non_owner, Checkable>;
		friend struct Read_only<T, Non_owner, Waitable>;
	};

	template <class T>
	struct Thread_private<T, Owner> {
		Thread_private() = default;
		Thread_private(std::unique_ptr<T> &&tp)
			: t{std::move(tp)} {}
		Thread_private(const Thread_private &) = delete;
		Thread_private(Thread_private &&other) noexcept
			: t{other.t} {
			other.t = nullptr;
		}
		Thread_private &operator=(const Thread_private &) = delete;
		Thread_private &operator=(Thread_private &&other) noexcept {
			std::swap(t, other.t);
			return *this;
		}

		T &operator*() {
			assert(t);
			return *t;
		}
		explicit operator bool() {
			return t != nullptr;
		}

		friend constexpr bool operator==(std::nullptr_t, const Thread_private &tp) {
			return tp.t == nullptr;
		}
		friend constexpr bool operator==(const Thread_private &tp, std::nullptr_t) {
			return tp.t == nullptr;
		}
		friend constexpr bool operator==(T *t, const Thread_private &tp) {
			return t == tp.t.get();
		}
		friend constexpr bool operator==(const Thread_private &tp, T *t) {
			return t == tp.t.get();
		}

		private:
		std::unique_ptr<T> t = nullptr;

		friend struct Mutex_protected<T, Non_owner, None>;
		friend struct Mutex_protected<T, Non_owner, Checkable>;
		friend struct Mutex_protected<T, Non_owner, Waitable>;
		friend struct Read_only<T, Non_owner, None>;
		friend struct Read_only<T, Non_owner, Checkable>;
		friend struct Read_only<T, Non_owner, Waitable>;
	};

	template <class T>
	struct Lock {
		Lock(T &value, std::mutex &mutex)
			: t{&value}
			, lock{mutex} {}

		T *operator->() {
			return t;
		}
		T &operator*() {
			return *t;
		}

		private:
		T *t;
		std::unique_lock<std::mutex> lock;
	};

	template <class T>
	struct Mutex_protected<T, Non_owner, None> {
		Lock<T> lock() {
			return t;
		}

		private:
		T *t;
	};

	template <class T>
	struct Mutex_protected<T, Non_owner, Waitable> {
		Mutex_protected(Thread_private<T, Non_owner> &&tp)
			: control_block{new Control_block(tp.t)} {
			tp.t = nullptr;
		}
		Mutex_protected(const Mutex_protected &other) noexcept
			: control_block{other.control_block} {
			control_block->reference_count++;
		}
		Mutex_protected(Mutex_protected &&other) noexcept
			: control_block{nullptr} {
			std::swap(control_block, other.control_block);
		}
		Mutex_protected &operator=(const Mutex_protected &other) {
			detach();
			control_block = other.control_block;
			++control_block->reference_count;
		}
		friend constexpr bool operator==(const Mutex_protected &mp, std::nullptr_t) {
			return mp.control_block;
		}
		friend constexpr bool operator==(std::nullptr_t, const Mutex_protected &mp) {
			return mp.control_block;
		}
		friend constexpr bool operator==(const Mutex_protected &mp, T *t) {
			return mp.control_block && mp.control_block->t == t;
		}
		friend constexpr bool operator==(T *t, const Mutex_protected &mp) {
			return mp.control_block && mp.control_block->t == t;
		}

		~Mutex_protected() {
			detach();
		}

		Lock<T> lock() const {
			return {*control_block->t, control_block->mutex};
		}

		Thread_private<T, Non_owner> wait() && {
			std::unique_lock<std::mutex> lock{control_block->mutex};
			if (control_block->reference_count != 1) {
				control_block->condition_variable.wait(lock, [this] { return control_block->reference_count == 1; });
			}
			auto tc = control_block->t;
			control_block->t = nullptr;
			return tc;
		}

		private:
		void detach() {
			if (control_block != nullptr) {
				switch (--control_block->reference_count) {
					case 1:
						control_block->condition_variable.notify_one();
						break;
					case 0:
						delete control_block;
				}
			}
		}

		struct Control_block {
			Control_block(T *t)
				: t{t} {}
			T *t = nullptr;
			std::mutex mutex;
			std::condition_variable condition_variable;
			std::atomic<unsigned int> reference_count{1};
		};
		Control_block *control_block;
	};
} // namespace TST