#pragma once
#include <iterator>
#include <functional>
#include <vector>

template<typename T, typename InternalIterator, typename InternalType = T>
class linq
{
	template<typename Q, typename InternalIterator, typename InternalType> friend class linq;

public:
	using value_type = typename T;

private:
	using filter_func = const std::function<bool(const InternalType &item)>;
	using transform_func = const std::function<value_type(const InternalType &item)>;

public:
	class const_iterator : public std::iterator<std::forward_iterator_tag, T>
	{
	public:
		const_iterator(const InternalIterator &iter, const InternalIterator &begin, const InternalIterator &end, filter_func &filter, transform_func &transform)
			: iter_(iter), begin_(begin), end_(end), filter_(filter), transform_(transform)
		{
			if (iter_ != end_ && !filter_(*iter_))
				operator++();
		}

		const_iterator& operator++()
		{
			do
			{
				iter_++;
			} while (iter_ != end_ && !filter_(*iter_));
			return *this;
		}

		const_iterator operator++(int)
		{
			const_iterator tmp(*this);
			operator++();
			return tmp;
		}

		bool operator==(const const_iterator &rhs)
		{
			return iter_ == rhs.iter_;
		}

		bool operator!=(const const_iterator &rhs)
		{
			return iter_ != rhs.iter_;
		}

		T operator*()
		{
			return transform_(*iter_);
		}

	private:
		InternalIterator iter_;
		InternalIterator begin_;
		InternalIterator end_;
		filter_func filter_;
		transform_func transform_;
	};

public:
	linq(const InternalIterator &begin, const InternalIterator &end)
		: linq(begin, end, default_filter, default_transform)
	{
	}

	const_iterator begin()
	{
		return const_iterator(begin_, begin_, end_, filter_, transform_);
	}

	const_iterator end()
	{
		return const_iterator(end_, begin_, end_, filter_, transform_);
	}

	linq<T, const_iterator> where(filter_func &filter)
	{
		return linq<T, const_iterator>(begin(), end(), filter);
	}

	template<typename Ret>
	linq<Ret, const_iterator, T> select(const std::function<Ret(const InternalType &item)> &transform)
	{
		return linq<Ret, const_iterator, T>(begin(), end(), transform);
	}

	std::vector<T> to_vector()
	{
		return std::vector<T>(begin(), end());
	}

private:
	linq(const InternalIterator &begin,
		const InternalIterator &end,
		filter_func &filter)
		: linq(begin, end, filter, default_transform)
	{
	}

	linq(const InternalIterator &begin,
		const InternalIterator &end,
		transform_func &transform)
		: linq(begin, end, default_filter, transform)
	{
	}

	linq(const InternalIterator &begin, const InternalIterator &end, filter_func &filter, transform_func &transform)
		: begin_(begin), end_(end), filter_(filter), transform_(transform)
	{
	}

	static bool default_filter(const InternalType &item)
	{
		return true;
	}

	static const T default_transform(const InternalType &item)
	{
		return item;
	}

	InternalIterator begin_;
	InternalIterator end_;
	filter_func filter_;
	transform_func transform_;
};

template<typename T>
linq<typename T::value_type, typename T::const_iterator> from(const T &container)
{
	return linq<typename T::value_type, typename T::const_iterator>(container.begin(), container.end());
}