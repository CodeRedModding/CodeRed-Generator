#pragma once
#include <string>

// Which engine folder to generate from is chosen by the build, which defines ENGINE as the
// folder name (for example -DENGINE=Template). Only "GameDefines.hpp" and "PiecesOfCode.hpp"
// are needed from it.
#ifndef ENGINE
#error "ENGINE is not defined, configure the build with -DENGINE=<engine folder name>."
#endif

#define ENGINE_STRINGIFY_IMPL(x) #x
#define ENGINE_STRINGIFY(x) ENGINE_STRINGIFY_IMPL(x)
#define ENGINE_INCLUDE(file) ENGINE_STRINGIFY(ENGINE/file)

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
