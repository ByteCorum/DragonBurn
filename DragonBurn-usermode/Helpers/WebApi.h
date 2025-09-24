#pragma once
#include <string>
#include <Windows.h>
#include <vector>
#include <regex>
#include <stdexcept>

namespace Web
{
    inline void Get(std::string url, std::string& response)
    {
        response = "";
        std::string cmd = "curl -s -X GET " + url;

        std::array<char, 128> buffer;
        std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);

        if (!pipe)
           throw std::runtime_error("failed to get curl request");
        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
            response += buffer.data();

        if (response == "")
            throw std::runtime_error("bad internet connection");

        std::regex pattern("\\d{3}:");
        if (std::regex_search(response, pattern))
            throw std::runtime_error(response);
    }
}