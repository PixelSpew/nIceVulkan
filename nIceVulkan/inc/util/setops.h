/*
The MIT License (MIT)

Copyright (c) 2016 Icy Defiance

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <functional>
#include <vector>
#include <memory>

namespace set
{
	// ************************************************************************
	//		FUNCTION TRAITS
	// ************************************************************************
#pragma region function traits

	template <typename T>
	struct function_traits
		: public function_traits<decltype(&T::operator())>
	{
	};

	template <typename ClassType, typename ReturnType, typename... Args>
	struct function_traits<ReturnType(ClassType::*)(Args...) const>
	{
		enum { arity = sizeof...(Args) };
		typedef ReturnType result_type;

		template <size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
		};
	};

#pragma endregion

	// ************************************************************************
	//		MACROS
	// ************************************************************************
#pragma region macros

#define PARAM_COUNT(Func) function_traits<Func>::arity
#define ARG_TYPE(Func, Index) function_traits<Func>::arg<Index>::type
#define ENABLE_IF(Cond) typename std::enable_if<Cond, int>::type = 0

#pragma endregion

	// ************************************************************************
	//		ITERATORS
	// ************************************************************************
#pragma region iterators

	template<typename InternalIter, typename Func>
	class where_iterator
	{
	public:
		using difference_type = typename std::iterator_traits<InternalIter>::difference_type;
		using value_type = typename std::iterator_traits<InternalIter>::value_type;
		using reference = typename std::iterator_traits<InternalIter>::reference;
		using pointer = typename std::iterator_traits<InternalIter>::pointer;
		typedef std::input_iterator_tag iterator_category;

		where_iterator(const InternalIter &iter, const InternalIter &end, const Func &filter) :
			iter_(iter),
			end_(end),
			filter_(filter)
		{
			while (iter_ != end_ && !filter_(*iter_)) {
				++iter_;
			}
			begin_ = iter_;
		}

		where_iterator& operator=(const where_iterator& rhs)
		{
			iter_ = rhs.iter_;
			end_ = rhs.end_;
			filter_ = rhs.filter_;
		}

		reference operator*() const
		{
			return *iter_;
		}

		where_iterator& operator++()
		{
			do {
				++iter_;
			} while (iter_ != end_ && !filter_(*iter_));
			return *this;
		}

		where_iterator operator++(int)
		{
			where_iterator tmp(*this);
			operator++();
			return tmp;
		}

		where_iterator& operator--()
		{
			do {
				--iter_;
			} while (iter_ != begin_ && !filter_(*iter_));
			return *this;
		}

		where_iterator operator--(int)
		{
			where_iterator tmp(*this);
			operator--();
			return tmp;
		}

		pointer operator->() const
		{
			return &operator*();
		}

		inline bool operator==(const where_iterator &rhs) const
		{
			return iter_ == rhs.iter_;
		}

		inline bool operator!=(const where_iterator &rhs) const
		{
			return !operator==(rhs);
		}

		InternalIter iter_;
		InternalIter begin_;
		InternalIter end_;
		Func filter_;
	};

	template<typename InternalIter, typename Func>
	class select_iterator
	{
		using ret = typename function_traits<Func>::result_type;

	public:
		using difference_type = typename std::iterator_traits<InternalIter>::difference_type;
		using value_type = ret;
		using reference = const ret&;
		using pointer = const ret*;
		using iterator_category = std::input_iterator_tag;

		select_iterator(const InternalIter &iter, const InternalIter &end, const Func &transform) :
			iter_(iter),
			end_(end),
			transform_(transform)
		{
		}

		select_iterator& operator=(const select_iterator& rhs)
		{
			iter_ = rhs.iter_;
			end_ = rhs.end_;
			filter_ = rhs.filter_;
		}

		value_type operator*() const
		{
			if (iter_ != end_) {
				return transform_(*iter_);
			} else {
				throw std::runtime_error("iterator not dereferencable");
			}
		}

		select_iterator& operator++()
		{
			++iter_;
			return *this;
		}

		select_iterator operator++(int)
		{
			select_iterator tmp(*this);
			operator++();
			return tmp;
		}

		select_iterator& operator--()
		{
			--iter_;
			return *this;
		}

		select_iterator operator--(int)
		{
			select_iterator tmp(*this);
			operator--();
			return tmp;
		}

		pointer operator->() const
		{
			return &operator*();
		}

		inline bool operator==(const select_iterator &rhs) const
		{
			return iter_ == rhs.iter_;
		}

		inline bool operator!=(const select_iterator &rhs) const
		{
			return !operator==(rhs);
		}

		InternalIter iter_;
		InternalIter end_;
		Func transform_;
	};

	template<typename InternalIter, typename KeyFunc, typename CompFunc>
	class order_by_iterator
	{
	public:
		using difference_type = typename std::iterator_traits<InternalIter>::difference_type;
		using value_type = const typename std::iterator_traits<InternalIter>::value_type;
		using reference = typename std::iterator_traits<InternalIter>::reference;
		using pointer = typename std::iterator_traits<InternalIter>::pointer;
		typedef std::input_iterator_tag iterator_category;

	private:
		using storage_type = std::vector<std::reference_wrapper<value_type>>;

	public:
		order_by_iterator(const InternalIter &iter, const InternalIter &end, const KeyFunc &keySelector, const CompFunc &comparison) :
			begin_(iter),
			end_(end)
		{
			sorted_ = std::shared_ptr<storage_type>(new storage_type());

			for (InternalIter copyIter = iter; copyIter != end; copyIter++) {
				sorted_->push_back(*copyIter);
			}

			if (sorted_->size() > 0)
				dual_pivot_quicksort(0, sorted_->size() - 1, sorted_->size() - 1, keySelector, comparison);

			iter_ = sorted_->begin();
		}

		order_by_iterator end()
		{
			order_by_iterator ret = *this;
			ret.iter_ = sorted_->end();
			return ret;
		}

		reference operator*() const
		{
			return (*iter_).get();
		}

		order_by_iterator& operator++()
		{
			++iter_;
			return *this;
		}

		order_by_iterator operator++(int)
		{
			order_by_iterator tmp(*this);
			operator++();
			return tmp;
		}

		order_by_iterator& operator--()
		{
			--iter_;
			return *this;
		}

		order_by_iterator operator--(int)
		{
			order_by_iterator tmp(*this);
			operator--();
			return tmp;
		}

		pointer operator->() const
		{
			return &operator*();
		}

		inline bool operator==(const order_by_iterator &rhs) const
		{
			return iter_ == rhs.iter_;
		}

		inline bool operator!=(const order_by_iterator &rhs) const
		{
			return !operator==(rhs);
		}

	private:

#define LESS_THAN(A, B) (comparison(keySelector(A), keySelector(B)) < 0)
#define GREATER_THAN(A, B) (comparison(keySelector(A), keySelector(B)) > 0)
#define EQUAL_TO(A, B) (comparison(keySelector(A), keySelector(B)) == 0)

		void dual_pivot_quicksort(size_t left, size_t right, size_t div, const KeyFunc &keySelector, const CompFunc &comparison)
		{
			size_t len = right - left;

			if (len < 27) { // insertion sort for tiny array
				for (size_t i = left + 1; i <= right; i++) {
					for (size_t j = i; j > left && LESS_THAN((*sorted_)[j].get(), (*sorted_)[j - 1].get()); j--) {
						swap(j, j - 1);
					}
				}
				return;
			}

			size_t third = len / div;

			// "medians"
			size_t m1 = left + third;
			size_t m2 = right - third;

			if (m1 <= left)
				m1 = left + 1;
			if (m2 >= right)
				m2 = right - 1;

			if (LESS_THAN((*sorted_)[m1].get(), (*sorted_)[m2].get())) {
				swap(m1, left);
				swap(m2, right);
			} else {
				swap(m1, right);
				swap(m2, left);
			}

			// pivots
			value_type pivot1 = (*sorted_)[left].get();
			value_type pivot2 = (*sorted_)[right].get();

			// pointers
			size_t less = left + 1;
			size_t great = right - 1;

			// sorting
			for (size_t k = less; k <= great; k++) {
				if (LESS_THAN((*sorted_)[k].get(), pivot1)) {
					swap(k, less++);
				} else if (GREATER_THAN((*sorted_)[k].get(), pivot2)) {
					while (k < great && GREATER_THAN((*sorted_)[great].get(), pivot2)) {
						great--;
					}
					swap(k, great--);

					if (LESS_THAN((*sorted_)[k].get(), pivot1)) {
						swap(k, less++);
					}
				}
			}

			// swaps
			size_t dist = great - less;

			if (dist < 13) {
				div++;
			}
			swap(less - 1, left);
			swap(great + 1, right);

			// subarrays
			dual_pivot_quicksort(left, less - 2, div, keySelector, comparison);
			dual_pivot_quicksort(great + 2, right, div, keySelector, comparison);

			// equal elements
			if (dist > len - 13 && !EQUAL_TO(pivot1, pivot2)) {
				for (size_t k = less; k <= great; k++) {
					if (EQUAL_TO((*sorted_)[k].get(), pivot1)) {
						swap(k, less++);
					} else if (EQUAL_TO((*sorted_)[k].get(), pivot2)) {
						swap(k, great--);

						if (EQUAL_TO((*sorted_)[k].get(), pivot1)) {
							swap(k, less++);
						}
					}
				}
			}

			// subarray
			if (LESS_THAN(pivot1, pivot2))
				dual_pivot_quicksort(less, great, div, keySelector, comparison);
		}

#undef EQUAL_TO
#undef GREATER_THAN
#undef LESS_THAN

		void swap(size_t a, size_t b)
		{
			std::reference_wrapper<value_type> tmp = (*sorted_)[a];
			(*sorted_)[a] = (*sorted_)[b];
			(*sorted_)[b] = tmp;
		}

		typename storage_type::const_iterator iter_;
		std::shared_ptr<storage_type> sorted_;
		InternalIter begin_;
		InternalIter end_;
	};

#pragma endregion

	// ************************************************************************
	//		ENUMERABLE
	// ************************************************************************
#pragma region enumerable

	template<typename T, typename Iterator>
	class enumerable
	{
	public:
		using const_iterator = Iterator;
		using difference_type = typename std::iterator_traits<Iterator>::difference_type;
		using value_type = typename std::iterator_traits<Iterator>::value_type;
		using reference = typename std::iterator_traits<Iterator>::reference;
		using pointer = typename std::iterator_traits<Iterator>::pointer;

		enumerable(const Iterator &begin, const Iterator &end, size_t size) :
			begin_(begin),
			end_(end),
			size_(size)
		{
		}

		// todo: replace Func with something more self-explanatory
		template<typename Func>
		auto where(const Func &filter) const
		{
			return enumerable<T, where_iterator<Iterator, Func>>(
				where_iterator<Iterator, Func>(begin_, end_, filter),
				where_iterator<Iterator, Func>(end_, end_, filter),
				SIZE_MAX);
		}

		template<typename Func>
		auto select(const Func &transform) const
		{
			return enumerable<function_traits<Func>::result_type, select_iterator<Iterator, Func>>(
				select_iterator<Iterator, Func>(begin_, end_, transform),
				select_iterator<Iterator, Func>(end_, end_, transform),
				size_);
		}

		reference first()
		{
			return *begin_;
		}

		template<typename Func>
		reference first(const Func &filter)
		{
			for (Iterator iter = begin_; iter != end_; iter++)
				if (filter(*iter))
					return *iter;
			throw runtime_error("Could not find element");
		}

		reference last()
		{
			Iterator last = end_;
			return *(--last);
		}

		/// <summary>
		/// Returns -1 if no element is found
		/// </summary>
		/// <param name="filter">function&lt;any(item)&gt;</param>
		template<typename Func, ENABLE_IF(PARAM_COUNT(Func) == 1)>	// todo: god this is evil
		difference_type first_index(const Func &filter)
		{
			for (Iterator iter = begin_; iter != end_; iter++)
				if (filter(*iter))
					return iter - begin_;
			return -1;
		}

		/// <summary>
		/// Returns -1 if no element is found
		/// </summary>
		/// <param name="filter">function&lt;any(item, index)&gt;</param>
		template<typename Func, ENABLE_IF(PARAM_COUNT(Func) == 2)>
		difference_type first_index(const Func &filter)
		{
			ARG_TYPE(Func, 1) i = 0;
			for (Iterator iter = begin_; iter != end_; iter++, i++)
				if (filter(*iter, i))
					return i;
			return -1;
		}

		template<typename KeyFunc>
		auto order_by(const KeyFunc &keySelector)
		{
			using key_type = function_traits<KeyFunc>::result_type;
			static const auto comparison = [](const key_type &a, const key_type &b) {
				if (a > b) {
					return 1;
				} else if (a < b) {
					return -1;
				} else {
					return 0;
				}
			};

			order_by_iterator<Iterator, KeyFunc, decltype(comparison)> begin(begin_, end_, keySelector, comparison);
			order_by_iterator<Iterator, KeyFunc, decltype(comparison)> end = begin.end();
			return enumerable<value_type, order_by_iterator<Iterator, KeyFunc, decltype(comparison)>>(begin, end, size_);
		}

		template<typename KeyFunc, typename CompFunc>
		auto order_by(const KeyFunc &keySelector, const CompFunc &comparison)
		{
			order_by_iterator<Iterator, KeyFunc, CompFunc> begin(begin_, end_, keySelector, comparison);
			order_by_iterator<Iterator, KeyFunc, CompFunc> end = begin.end();
			return enumerable<value_type, order_by_iterator<Iterator, KeyFunc, CompFunc>>(begin, end, size_);
		}

		std::vector<T> to_vector() const
		{
			std::vector<T> ret;
			if (size_ != SIZE_MAX) {
				ret.reserve(size_);
			}
			for (Iterator iter = begin_; iter != end_; iter++) {
				ret.push_back(*iter);
			}
			return ret;
		}

		Iterator begin() const
		{
			return begin_;
		}

		Iterator end() const
		{
			return end_;
		}

	private:
		Iterator begin_;
		Iterator end_;
		size_t size_;
	};

#pragma endregion

	// ************************************************************************
	//		BUILDERS
	// ************************************************************************
#pragma region builders

	template<typename T>
	using ienumerable = enumerable<typename T::value_type, typename T::const_iterator>;

	template<typename T>
	ienumerable<T> from(const T &container)
	{
		return ienumerable<T>(
			container.begin(),
			container.end(),
			container.size());
	}

	template<typename T>
	ienumerable<std::initializer_list<T>> from(const std::initializer_list<T> &container)
	{
		return ienumerable<std::initializer_list<T>>(
			container.begin(),
			container.end(),
			container.size());
	}

#pragma endregion
}

#undef ENABLE_IF
#undef ARG_TYPE
#undef PARAM_COUNT
