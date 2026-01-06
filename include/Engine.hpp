#pragma once

#include <string>

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
	static std::string m_name;
	static std::string m_version;
	static std::string m_credits;
	static std::string m_links;

public:
	static const std::string& GetName();
	static const std::string& GetVersion();
	static const std::string& GetCredits();
	static const std::string& GetLinks();

public:
	GEngine() = delete;
};

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/