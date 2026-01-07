#pragma once

#include <cstdint>
#include <string_view>

namespace Utils
{
    void MessageBoxInfo(const std::string_view& message);
    void MessageBoxWarn(const std::string_view& message);
    void MessageBoxError(const std::string_view& message);
}

namespace Retrievers
{
    uintptr_t GetBaseAddress();
    uintptr_t FindPattern(const std::basic_string_view<uint8_t>& pattern, const std::string_view& mask);
}
