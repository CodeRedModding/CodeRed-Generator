#include <utils.hpp>

#include <Engine.hpp>

#ifdef _WIN32

#include <windows.h>
#include <psapi.h>

namespace Utils
{
    void MessageBoxExt(const std::string_view& message, uint32_t flags)
    {
        MessageBoxA(NULL, message.data(), GEngine::GetName().data(), flags);
    }

    void MessageBoxInfo(const std::string_view& message)
    {
        MessageBoxExt(message, (MB_OK | MB_ICONINFORMATION));
    }

    void MessageBoxWarn(const std::string_view& message)
    {
        MessageBoxExt(message, (MB_OK | MB_ICONWARNING));
    }

    void MessageBoxError(const std::string_view& message)
    {
        MessageBoxExt(message, (MB_OK | MB_ICONERROR));
    }
}

namespace Retrievers
{
    uintptr_t GetBaseAddress()
    {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
    }

    uintptr_t FindPattern(const std::basic_string_view<uint8_t>& pattern, const std::string_view& mask)
    {
        if (!pattern.empty() && !mask.empty())
        {
            MODULEINFO miInfos;
            ZeroMemory(&miInfos, sizeof(MODULEINFO));

            HMODULE hModule = GetModuleHandle(NULL);
            K32GetModuleInformation(GetCurrentProcess(), hModule, &miInfos, sizeof(MODULEINFO));

            uintptr_t start = reinterpret_cast<uintptr_t>(hModule);
            uintptr_t end = (start + miInfos.SizeOfImage);

            size_t currentPos = 0;
            size_t maskLength = (mask.length() - 1);

            for (uintptr_t retAddress = start; retAddress < end; retAddress++)
            {
                if (*reinterpret_cast<uint8_t*>(retAddress) == pattern[currentPos] || mask[currentPos] == '?')
                {
                    if (currentPos == maskLength)
                    {
                        return (retAddress - maskLength);
                    }

                    currentPos++;
                }
                else
                {
                    retAddress -= currentPos;
                    currentPos = 0;
                }
            }
        }

        return 0;
    }
}

#elif __linux__

#include <cstdint>
#include <cstdio>
#include <cassert>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <string>
#include <vector>

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace Utils
{
    namespace
    {
        enum MessageBoxLevel : uint8_t
        {
            Info = 0,
            Warn = 1,
            Error = 2,
        };

        bool TryExec(const std::vector<const char*>& args)
        {
            pid_t pid = fork();

            if (pid == -1)
            {
                return false;
            }

            if (pid == 0)
            {
                // Child process: redirect stderr to /dev/null
                int devnull = open("/dev/null", O_WRONLY);
                if (devnull != -1)
                {
                    dup2(devnull, STDERR_FILENO);
                    close(devnull);
                }

                execvp(args[0], const_cast<char* const*>(args.data()));
                _exit(127);
            }

            // Parent process: wait for child
            int status;
            waitpid(pid, &status, 0);

            return WIFEXITED(status) && WEXITSTATUS(status) == 0;
        }
    }

    void MessageBoxExt(const std::string_view& message, MessageBoxLevel level)
    {
        std::string_view title(GEngine::GetName());
        std::string_view msg(message);

        // Blocking dialog commands (ordered by preference)
        std::array commands = {
            std::vector<const char*>{
                "kdialog", std::array{"--msgbox", "--sorry", "--error"}[level],
                msg.data(), "--title", title.data(), nullptr
            },
            std::vector<const char*>{
                "zenity", std::array{"--info", "--warning", "--error"}[level],
                "--title", title.data(), "--text", msg.data(), nullptr
            },
            std::vector<const char*>{
                "notify-send", "-t", "0", "-w",
                "-u", std::array{"normal", "critical", "critical"}[level],
                "-a", title.data(), msg.data(), nullptr
            },
            std::vector<const char*>{
                "xmessage", "-buttons", "OK:0",
                "-title", title.data(), msg.data(), nullptr
            }
        };

        for (const auto& args : commands)
        {
            if (TryExec(args))
                return;
        }

        // Fallback to stderr
        std::cerr << "[" << title << "] " << msg << std::endl;
    }

    void MessageBoxInfo(const std::string_view& message)
    {
        MessageBoxExt(message, MessageBoxLevel::Info);
    }

    void MessageBoxWarn(const std::string_view& message)
    {
        MessageBoxExt(message, MessageBoxLevel::Warn);
    }

    void MessageBoxError(const std::string_view& message)
    {
        MessageBoxExt(message, MessageBoxLevel::Error);
    }
}

namespace Retrievers
{
    namespace
    {
        struct ProcMapsEntry
        {
            ProcMapsEntry(std::string& line)
            {
                parse(line);
            }

            uintptr_t start;
            uintptr_t end;
            struct
            {
                bool r;
                bool w;
                bool x;
                bool s;
            } perms;
            uintptr_t offset;
            struct
            {
                uint16_t major;
                uint16_t minor;
            } dev;
            uint64_t inode;
            std::string pathname;
        
            void parse(std::string& line)
            {
                this->pathname.resize(4096, '\0');
                char perms[4] = {0};
                sscanf(line.data(), "%tx-%tx %4c %tx %02hx:%02hx %lu %s",
                    &this->start, &this->end,
                    perms, &this->offset,
                    &this->dev.major, &this->dev.minor,
                    &this->inode, this->pathname.data());
                this->parse_perms(perms);
                this->pathname.shrink_to_fit();
            }

        private:
            void parse_perms(const char perms[4])
            {
                this->perms.r = (perms[0] == 'r');
                this->perms.w = (perms[1] == 'w');
                this->perms.x = (perms[2] == 'x');
                this->perms.s = (perms[3] == 's' || perms[3] == 'S');
            }
        };
    }

    uintptr_t GetBaseAddress()
    {
        std::ifstream input("/proc/self/maps");

        for (std::string line; std::getline(input, line);) {
            // Parse the proc_pid_maps line
            ProcMapsEntry entry(line);

            // Check if this line corresponds to the main executable
            if (entry.inode != 0 && std::filesystem::equivalent(entry.pathname, "/proc/self/exe")) {
                return entry.start;
            }
        }

        assert(false && "Failed to find base address from /proc/self/maps");

        return 0;
    }

    uintptr_t FindPattern(const std::basic_string_view<uint8_t>& pattern, const std::string_view& mask)
    {
        if (pattern.empty() || mask.empty())
        {
            return 0;
        }

        std::ifstream input("/proc/self/maps");
        uintptr_t start = 0;
        uintptr_t end = 0;

        // Find the executable's memory range
        for (std::string line; std::getline(input, line);)
        {
            ProcMapsEntry entry(line);

            // Only scan readable segments of the main executable
            if (entry.inode != 0 && entry.perms.r &&
                std::filesystem::equivalent(entry.pathname, "/proc/self/exe"))
            {
                if (start == 0 || entry.start < start)
                    start = entry.start;
                if (entry.end > end)
                    end = entry.end;
            }
        }

        if (start == 0 || end == 0)
        {
            return 0;
        }

        // Pattern matching (same algorithm as Windows)
        size_t currentPos = 0;
        size_t maskLength = (mask.length() - 1);

        for (uintptr_t retAddress = start; retAddress < end; retAddress++)
        {
            if (*reinterpret_cast<uint8_t*>(retAddress) == pattern[currentPos] || mask[currentPos] == '?')
            {
                if (currentPos == maskLength)
                {
                    return (retAddress - maskLength);
                }

                currentPos++;
            }
            else
            {
                retAddress -= currentPos;
                currentPos = 0;
            }
        }

        return 0;
    }
}

#else

#error "Unsupported platform"

#endif
