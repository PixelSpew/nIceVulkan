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

namespace set
{

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

#pragma region macros

#define PARAM_COUNT(Func) function_traits<Func>::arity
#define ARG_TYPE(Func, Index) function_traits<Func>::arg<Index>::type
#define ENABLE_IF(Cond) typename std::enable_if<Cond, int>::type = 0

#pragma endregion

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
		}

		where_iterator& operator=(const where_iterator& rhs)
		{
			iter_ = rhs.iter_;
		}

		virtual reference operator*() const
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
		}

		value_type operator*() const
		{
			if (iter_ != end_) {
				return transform_(*iter_);
			} else {
				throw std::runtime_error("iterator not dereferencable");
			}
		}

		virtual select_iterator& operator++()
		{
			++iter_;
			return *this;
		}

		select_iterator operator++(int)
		{
			select_iterator ret(*this);
			operator++();
			return ret;
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

#pragma endregion

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

		template<typename Func>
		reference first(const Func &filter)
		{
			for (Iterator iter = begin_; iter != end_; iter++)
				if (filter(*iter))
					return *iter;
			throw runtime_error("Could not find element");
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

		std::vector<T> to_vector() const
		{
			std::vector<T> ret;
			if (size_ != SIZE_MAX) {
				ret.reserve(size_);
			}
			for (const T& item : *this) {
				ret.push_back(item);
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
