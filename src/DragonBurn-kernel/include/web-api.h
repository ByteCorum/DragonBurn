#pragma once
#include <string>
#include <Windows.h>
#include <array>
#include <regex>
#include <stdexcept>

namespace Web
{
    inline std::string Get(std::string url)
    {
        std::string response;
        std::array<char, 128> buffer;

        std::string command = "curl -s -X GET " + url + " 2>&1";

        FILE* pipePtr = _popen(command.c_str(), "r");

        if (!pipePtr)
            throw std::runtime_error("failed to create process");

        std::unique_ptr<FILE, decltype(&_pclose)> pipe(pipePtr, _pclose);

        while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr)
            response += buffer.data();

        int exitCode = _pclose(pipe.release());

        if (exitCode != 0 && !response.empty())
            throw std::runtime_error(response);

        if (response.empty())
            throw std::runtime_error("failed to connect to the remote host");

        std::regex pattern("\\d{3}:");
        if (std::regex_search(response, pattern))
            throw std::runtime_error(response);

        return response;
    }
}