#pragma once
#include "util/dispatcher.h"
#include <Windows.h>
#include <vector>
#include <queue>

namespace nif
{
	enum class keys
	{
		escape = 27
	};

	class keyboard
	{
	public:
		using keyevent = dispatcher<keyboard, void(void)>;

		keyboard();
		~keyboard();
		void update();
		void set_key(const WPARAM key, const LPARAM mod, const bool down);

		keyevent& keyhit(const keys key);

	private:
		std::vector<bool> keys_;
		std::queue<keys> keyhits_;
		std::vector<keyevent> keyhit_events_;
	};
}
