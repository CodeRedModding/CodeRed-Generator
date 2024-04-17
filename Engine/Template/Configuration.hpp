#pragma once
#include <string>
#include <filesystem>

/*
# ========================================================================================= #
# Configuration
# ========================================================================================= #
*/

// Uncomment this if you want to disable file logging during generation.
//#define NO_LOGGING

// Uncomment this if your game uses wide characters (UTF16), the default is UTF8!
//#define UTF16

// Below are global variables for the generator, make changes in in the cpp file only!

class GConfig
{
private: // Cosmetics
	static uint32_t m_constantSpacing;
	static uint32_t m_commentSpacing; 
	static uint32_t m_enumSpacing;
	static uint32_t m_classSpacing;
	static uint32_t m_structSpacing;
	static uint32_t m_functionSpacing;

public:
	static uint32_t GetConstSpacing();
	static uint32_t GetCommentSpacing();
	static uint32_t GetEnumSpacing();
	static uint32_t GetClassSpacing();
	static uint32_t GetStructSpacing();
	static uint32_t GetFunctionSpacing();

private: // Generator Settings
	static bool m_useWindows;
	static bool m_useConstants;
	static bool m_removeNativeIndex;
	static bool m_removeNativeFlags;
	static bool m_printEnumFlags;
	static bool m_useEnumClasses;
	static std::string m_enumClassType;
	static uint32_t m_gameAlignment;
	static uint32_t m_finalAlignment;

public:
	static bool UsingWindows();
	static bool UsingConstants();
	static bool RemoveNativeIndex();
	static bool RemoveNativeFlags();
	static bool PrintEnumFlags();
	static bool UsingEnumClasses();
	static const std::string& GetEnumClassType();
	static uint32_t GetGameAlignment();
	static uint32_t GetFinalAlignment();

private: // Process Event
	static bool m_useIndex;
	static int32_t m_peIndex;
	static std::string m_peMask;
	static std::pair<uint8_t*, std::string> m_pePattern;

public:
	static bool UsingProcessEventIndex();
	static int32_t GetProcessEventIndex();
	static uint8_t* GetProcessEventPattern();
	static const std::string& GetProcessEventStr();
	static const std::string& GetProcessEventMask();

private: // Global Objects & Names
	static bool m_useOffsets;
	static uintptr_t m_gobjectOffset;
	static std::string m_gobjectMask;
	static std::pair<uint8_t*, std::string> m_gobjectPattern;
	static uintptr_t m_gnameOffset;
	static std::string m_gnameMask;
	static std::pair<uint8_t*, std::string> m_gnamePattern;

public:
	static bool UsingOffsets();
	static uintptr_t GetGObjectOffset();
	static uint8_t* GetGObjectPattern();
	static const std::string& GetGObjectStr();
	static const std::string& GetGObjectMask();
	static uintptr_t GetGNameOffset();
	static uint8_t* GetGNamePattern();
	static const std::string& GetGNameStr();
	static const std::string& GetGNameMask();

private: // Game Info
	static std::string m_gameNameLong;
	static std::string m_gameNameShort;
	static std::string m_gameVersion;
	static std::filesystem::path m_outputPath;

public:
	static const std::string& GetGameNameLong();
	static const std::string& GetGameNameShort();
	static const std::string& GetGameVersion();
	static const std::filesystem::path& GetOutputPath();
	static bool HasOutputPath();

public:
	GConfig() = delete;
};

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/