#pragma once
#include <vector>
#include <functional>

namespace nif
{
	template<typename P, typename F>
	class dispatcher {
		template <bool B, typename...>
		struct dependent_bool : std::integral_constant<bool, B> {};

		static_assert(dependent_bool<false, F>::value, "template argument must be a function signature");
	};

	template<typename P, typename ...Args>
	class dispatcher<P, void(Args...)>
	{
		friend P;

	public:
		void add(const std::function<void(Args...)> &func)
		{
			listeners.push_back(func);
		}

	private:
		void operator()(Args ...args) const
		{
			for (auto func : listeners)
				func(args...);
		}

	private:
		std::vector<std::function<void(Args...)>> listeners;
	};

	template<typename P, typename Ret, typename ...Args>
	class dispatcher<P, Ret(Args...)>
	{
		friend P;

	public:
		void add(const std::function<Ret(Args...)> &func)
		{
			listeners.push_back(func);
		}

	private:
		std::vector<Ret> operator()(Args ...args) const
		{
			std::vector<Ret> retvals;
			retvals.reserve(listeners.size());
			for (auto func : listeners)
				retvals.push_back(func(args...));
			return retvals;
		}

	private:
		std::vector<std::function<Ret(Args...)>> listeners;
	};
}