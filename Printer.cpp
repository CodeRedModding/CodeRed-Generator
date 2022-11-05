#include "Printer.hpp"
#include "Engine/Engine.hpp"

namespace Printer
{
    void Empty(std::ostringstream& stream)
    {
        stream.str(std::string());
    }

    void FillRight(std::ostringstream& stream, char fill, size_t width)
    {
        stream <<  std::setfill(fill) << std::setw(width) << std::right;
    }

    void FillLeft(std::ostringstream& stream, char fill, size_t width)
    {
        stream <<  std::setfill(fill) << std::setw(width) << std::left;
    }

    void FillRight(std::ofstream& stream, char fill, size_t width)
    {
        stream <<  std::setfill(fill) << std::setw(width) << std::right;
    }

    void FillLeft(std::ofstream& stream, char fill, size_t width)
    {
        stream <<  std::setfill(fill) << std::setw(width) << std::left;
    }

    std::string Hex(uintptr_t decimal, size_t width)
    {
        std::ostringstream stream;
        stream << "0x" << std::setfill('0') << std::setw(width) << std::right << std::uppercase << std::hex << decimal;
        return stream.str();
    }

    std::string Hex(uintptr_t decimal, EWidthTypes width)
    {
        return Hex(decimal, static_cast<size_t>(width));
    }

    std::string Hex(void* pointer)
    {
        return Hex(reinterpret_cast<uintptr_t>(pointer), sizeof(uintptr_t));
    }

    std::string Decimal(uintptr_t hex, size_t width)
    {
        std::ostringstream stream;
        stream << std::setfill('0') << std::setw(width) << std::right << std::uppercase << std::dec << hex;
        return stream.str();
    }

    std::string Decimal(uintptr_t hex, EWidthTypes width)
    {
        return Decimal(hex, static_cast<size_t>(width));
    }

    std::string Precision(float value, size_t precision)
    {
        std::ostringstream stream;
        stream << std::setprecision(precision) << value;
        return stream.str();
    }

    void Header(std::ostringstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush)
    {
        stream << "/*\n";
        stream << "#############################################################################################\n";
        stream << "# " << Configuration::GameName << " (" << Configuration::GameVersion + ") SDK\n";
        stream << "# Generated with the " << Engine::GeneratorName << " " << Engine::GeneratorVersion << "\n";
        stream << "# ========================================================================================= #\n";
        stream << "# File: " << fileName << "." << fileExtension << "\n";
        stream << "# ========================================================================================= #\n";
        stream << "# Credits: " << Engine::GeneratorCredits << "\n";
        stream << "# Links: " << Engine::GeneratorLinks << "\n";
        stream << "#############################################################################################\n";
        stream << "*/\n";

        if ((fileName != "SdkHeaders") && (fileName != "GameDefines"))
        {
            if (fileExtension == "hpp")
            {
                stream << "#pragma once\n";

                if (Configuration::UsingConstants)
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
            stream << "\t#pragma pack(push, 0x" + std::to_string(Configuration::FinalAlignment) + ")\n";
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
            stream << "\t#pragma pack(pop)\n";
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