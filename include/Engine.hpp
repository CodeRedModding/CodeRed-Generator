#pragma once

#include <string_view>

// Preprocessor helpers for computed includes
#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)
#define ENGINE_INCLUDE(file) STRINGIFY(ENGINE/file)

// Engine files are included based on build tool's "ENGINE" option (e.g., -DENGINE=Template), only "GameDefines.hpp" and "PiecesOfCode.hpp" is needed.
#include ENGINE_INCLUDE(GameDefines.hpp)
#include ENGINE_INCLUDE(PiecesOfCode.hpp)

/*
# ========================================================================================= #
# Engine
# ========================================================================================= #
*/

// These are global variables for the generator, you do not need to change them; they are assigned in the cpp file.

class GEngine
{
private:
	static std::string_view m_name;
	static std::string_view m_version;
	static std::string_view m_credits;
	static std::string_view m_links;

public:
	static const std::string_view& GetName();
	static const std::string_view& GetVersion();
	static const std::string_view& GetCredits();
	static const std::string_view& GetLinks();

public:
	GEngine() = delete;
};

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/