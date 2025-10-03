#pragma once
#include <cstdio>
#include <string>
#include <utility>

template <typename... Args>
inline std::string Format(const char* pFormat, Args&&... args)
{
        const int Length = std::snprintf(nullptr, 0, pFormat, std::forward<Args>(args)...);
        if (Length <= 0)
                return {};

        std::string Result(static_cast<std::size_t>(Length) + 1, '\0');
        std::snprintf(Result.data(), Result.size(), pFormat, std::forward<Args>(args)...);
        Result.resize(static_cast<std::size_t>(Length));
        return Result;
}
