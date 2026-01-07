#include <Framework/Printer.hpp>
#include <Engine.hpp>

namespace Printer
{
    static constexpr uint32_t MAX_FILL_LENGTH = 1024;

    void Empty(std::ostringstream& stream)
    {
        stream.str(std::string());
    }

    void FillRight(std::ostringstream& stream, char fill, uint32_t width)
    {
        if (width > MAX_FILL_LENGTH)
        {
            width = MAX_FILL_LENGTH;
        }

        stream << std::setfill(fill) << std::setw(width) << std::right;
    }

    void FillLeft(std::ostringstream& stream, char fill, uint32_t width)
    {
        if (width > MAX_FILL_LENGTH)
        {
            width = MAX_FILL_LENGTH;
        }

        stream << std::setfill(fill) << std::setw(width) << std::left;
    }

    void FillRight(std::ofstream& stream, char fill, uint32_t width)
    {
        if (width > MAX_FILL_LENGTH)
        {
            width = MAX_FILL_LENGTH;
        }

        stream << std::setfill(fill) << std::setw(width) << std::right;
    }

    void FillLeft(std::ofstream& stream, char fill, uint32_t width)
    {
        if (width > MAX_FILL_LENGTH)
        {
            width = MAX_FILL_LENGTH;
        }

        stream << std::setfill(fill) << std::setw(width) << std::left;
    }

    void ReplaceChars(std::string& baseStr, char oldChar, char newChar)
    {
        for (char& c : baseStr)
        {
            if (c == oldChar)
            {
                c = newChar;
            }
        }
    }

    void ReplaceString(std::string& baseStr, const std::string& strToReplace, const std::string& replaceWithStr)
    {
        if (!baseStr.empty() && !strToReplace.empty())
        {
            size_t replaceLength = strToReplace.length();
            size_t replacePos = baseStr.find(strToReplace);

            while (replacePos != std::string::npos)
            {
                baseStr.replace(replacePos, replaceLength, replaceWithStr);
                replacePos = baseStr.find(strToReplace);
            }
        }
    }

    std::string Hex(uintptr_t decimal, uint32_t width)
    {
        std::ostringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(width) << std::right << std::uppercase << std::hex << decimal;
        return stream.str();
    }

    std::string Hex(uintptr_t decimal, EWidthTypes width)
    {
        return Hex(decimal, static_cast<uint32_t>(width));
    }

    std::string Hex(void* pointer)
    {
        return Hex(reinterpret_cast<uintptr_t>(pointer), sizeof(uintptr_t));
    }

    std::string Decimal(uintptr_t hex, uint32_t width)
    {
        std::ostringstream stream;
        stream << std::setfill('0') << std::setw(width) << std::right << std::uppercase << std::dec << hex;
        return stream.str();
    }

    std::string Decimal(uintptr_t hex, EWidthTypes width)
    {
        return Decimal(hex, static_cast<uint32_t>(width));
    }

    std::string Precision(float value, uint32_t precision)
    {
        std::ostringstream stream;
        stream << std::setprecision(precision) << value;
        return stream.str();
    }

    std::string ToUpper(std::string str)
    {
        if (!str.empty())
        {
            std::transform(str.begin(), str.end(), str.begin(), toupper);
        }

        return str;
    }

    std::string ToLower(std::string str)
    {
        if (!str.empty())
        {
            std::transform(str.begin(), str.end(), str.begin(), tolower);
        }

        return str;
    }

    void Header(std::ostringstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush)
    {
        stream << "/*\n";
        stream << "#############################################################################################\n";
        stream << "# " << GConfig::GetGameNameLong() << " (" << GConfig::GetGameNameShort() + ") SDK " << GConfig::GetGameVersion() << "\n";
        stream << "# Generated with the " << GEngine::GetName() << " " << GEngine::GetVersion() << "\n";
        stream << "# ========================================================================================= #\n";
        stream << "# File: " << fileName << "." << fileExtension << "\n";
        stream << "# ========================================================================================= #\n";
        stream << "# Credits: " << GEngine::GetCredits() << "\n";
        stream << "# Links: " << GEngine::GetLinks() << "\n";
        stream << "#############################################################################################\n";
        stream << "*/\n";

        if ((fileName != "SdkHeaders") && (fileName != "SdkConstants") && (fileName != "GameDefines"))
        {
            if (fileExtension == "hpp")
            {
                stream << "#pragma once\n";

                if (GConfig::UsingConstants())
                {
                    stream << "#include \"../SdkConstants.hpp\"\n";
                }
            }
            else if (fileExtension == "cpp")
            {
                stream << "#include \"../SdkHeaders.hpp\"\n";
            }
        }

        if (bPragmaPush)
        {
            stream << "\n#ifdef _MSC_VER\n";
            stream << "#pragma pack(push, " + Hex(GConfig::GetFinalAlignment(), 1) + ")\n";
            stream << "#endif\n";
        }
    }

    void Header(std::ofstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush)
    {
        std::ostringstream sStream;
        Header(sStream, fileName, fileExtension, bPragmaPush);
        stream << sStream.str();
    }

    void Section(std::ostringstream& stream, const std::string& sectionName)
    {
        stream << "\n/*\n";
        stream << "# ========================================================================================= #\n";
        stream << "# " << sectionName << "\n";
        stream << "# ========================================================================================= #\n";
        stream << "*/\n\n";
    }

    void Section(std::ofstream& stream, const std::string& sectionName)
    {
        std::ostringstream sStream;
        Section(sStream, sectionName);
        stream << sStream.str();
    }

    void Footer(std::ostringstream& stream, bool bPragmaPop)
    {
        stream << "/*\n";
        stream << "# ========================================================================================= #\n";
        stream << "#\n";
        stream << "# ========================================================================================= #\n";
        stream << "*/\n";

        if (bPragmaPop)
        {
            stream << "\n#ifdef _MSC_VER\n";
            stream << "#pragma pack(pop)\n";
            stream << "#endif\n";
        }
    }

    void Footer(std::ofstream& stream, bool bPragmaPop)
    {
        std::ostringstream sStream;
        Footer(sStream, bPragmaPop);
        stream << sStream.str();
    }
}