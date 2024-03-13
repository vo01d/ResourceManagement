#ifndef _TASK3_H_
#define _TASK3_H_

#include <algorithm>

// for a better understanding of how shared_ptr and weak_ptr work, I used std::shared_ptrand std::weak_ptr implementations and related documentation. 
// I tried to understand as much as possible how they work and develop their simplified implementation as similar as possible to the original

namespace sp {
	template <class _Ty>
	class ReferenceCounter {
	public:
		using counter_type = long;

		explicit ReferenceCounter(_Ty* ptr) : m_ptr(ptr) {}

		ReferenceCounter(const ReferenceCounter&) = delete;
		ReferenceCounter& operator=(const ReferenceCounter&) = delete;
		~ReferenceCounter() noexcept = default;

		void incStrong() {
			++m_strong_refs;
		}

		void incWeak() {
			++m_weak_refs;
		}

		void decStrong() {
			if (--m_strong_refs == 0) {
				destroy();
				decWeak();
			}
		}

		void decWeak() {
			if (--m_weak_refs == 0) {
				deleteThis();
			}
		}

		long getStrongCount() const {
			return m_strong_refs;
		}
	private:
		void destroy() {
			delete m_ptr;
		}

		void deleteThis() {
			delete this;
		}

		counter_type m_strong_refs = 1;
		counter_type m_weak_refs = 1;

		_Ty* m_ptr;
	};

	template<typename _Ty>
	class shared_ptr;

	template<typename _Ty>
	class weak_ptr;

	template<typename _Ty>
	class PtrBase {
	public:
		using element_type = _Ty;

		PtrBase(const PtrBase&) = delete;
		PtrBase& operator=(const PtrBase&) = delete;

		long use_count() const {
			return m_base ? m_base->getStrongCount() : 0;
		}
	protected:
		PtrBase() noexcept = default;
		~PtrBase() noexcept = default;

		element_type* get() const {
			return m_ptr;
		}

		void incStrong() const {
			if (m_base) {
				m_base->incStrong();
			}
		}

		void incWeak() const {
			if (m_base) {
				m_base->incWeak();
			}
		}

		void decStrong() {
			if (m_base) {
				m_base->decStrong();
			}
		}

		void decWeak() {
			if (m_base) {
				m_base->decWeak();
			}
		}

		void swapWith(PtrBase<_Ty>& right) {
			std::swap(m_ptr, right.m_ptr);
			std::swap(m_base, right.m_base);
		}
	private:
		element_type* m_ptr;
		ReferenceCounter<_Ty>* m_base;

		friend shared_ptr<_Ty>;
		friend weak_ptr<_Ty>;
	};

	template <typename _Ty>
	class shared_ptr : public PtrBase<_Ty> {
	public:
		using typename PtrBase<_Ty>::element_type;

		shared_ptr() noexcept = default;

		shared_ptr(nullptr_t) noexcept {}

		explicit shared_ptr(element_type* ptr) {
			this->m_ptr = ptr;
			this->m_base = new ReferenceCounter<_Ty>(ptr);
		}

		shared_ptr(const shared_ptr& right) noexcept {
			right.incStrong();

			this->m_ptr = right.m_ptr;
			this->m_base = right.m_base;
		}

		shared_ptr(shared_ptr&& right) noexcept {
			this->m_ptr = right.m_ptr;
			this->m_base = right.m_base;

			right.m_ptr = nullptr;
			right.m_base = nullptr;
		}

		shared_ptr(const weak_ptr<_Ty>& right) {
			if (right.m_base && right.useCount()) {
				right.m_base->incStrong();
				this->m_ptr = right.m_ptr;
				this->m_base = right.m_base;
			}
		}

		~shared_ptr() {
			this->decStrong();
		}

		shared_ptr& operator=(const shared_ptr& right) noexcept {
			shared_ptr(right).swap(*this);

			return *this;
		}

		shared_ptr& operator=(shared_ptr&& right) noexcept {
			shared_ptr(std::move(right)).swap(*this);

			return *this;
		}

		void reset() {
			shared_ptr().swap(*this);
		}

		void reset(element_type* ptr) {
			shared_ptr(ptr).swap(*this);
		}

		void swap(shared_ptr& other) {
			this->swapWith(other);
		}

		using PtrBase<_Ty>::get;

		element_type& operator*() const {
			return *get();
		}

		element_type* operator->() const {
			return get();
		}
	};

	template<typename _Ty>
	class weak_ptr : public PtrBase<_Ty> {
	public:
		using typename PtrBase<_Ty>::element_type;

		weak_ptr() noexcept = default;

		weak_ptr(const weak_ptr& right) noexcept {
			if (right.m_base) {
				this->m_ptr = right.m_ptr;
				this->m_base = right.m_base;
				this->m_base->incWeak();
			}
		}

		weak_ptr(const shared_ptr<_Ty>& right) noexcept {
			if (right.m_base) {
				this->m_ptr = right.m_ptr;
				this->m_base = right.m_base;
				this->m_base->incWeak();
			}
		}

		weak_ptr(weak_ptr&& right) noexcept {
			this->m_ptr = right.m_ptr;
			this->m_base = right.m_base;

			right.m_ptr = nullptr;
			right.m_base = nullptr;
		}

		weak_ptr& operator=(const weak_ptr& right) noexcept {
			weak_ptr(right).swap(*this);

			return *this;
		}

		weak_ptr& operator=(weak_ptr&& right) noexcept {
			weak_ptr(std::move(right)).swap(*this);

			return *this;
		}

		weak_ptr& operator=(const shared_ptr<_Ty>& right) noexcept {
			weak_ptr(right).swap(*this);

			return *this;
		}

		~weak_ptr() noexcept {
			this->decWeak();
		}

		void reset() {
			weak_ptr().swap(*this);
		}

		void swap(weak_ptr& other) {
			this->swapWith(other);
		}

		bool expired() const {
			return this->useCount() == 0;
		}

		shared_ptr<_Ty> lock() const {
			shared_ptr<_Ty> ptr(*this);

			return ptr;
		}
	};
};

#endif