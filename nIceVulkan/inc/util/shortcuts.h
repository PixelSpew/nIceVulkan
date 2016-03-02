#pragma once

#define VK_TRY(Func) if (Func != vk::Result::eVkSuccess) throw runtime_error("fail")