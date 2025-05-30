#include "Engine.hpp"

/*
# ========================================================================================= #
# Engine
# ========================================================================================= #
*/

std::string GEngine::m_name = "CodeRedGenerator";

std::string GEngine::m_version = "v1.1.6";

std::string GEngine::m_credits = "ItsBranK, TheFeckless";

std::string GEngine::m_links = "www.github.com/CodeRedModding/CodeRed-Generator";

const std::string& GEngine::GetName()
{
	return m_name;
}

const std::string& GEngine::GetVersion()
{
	return m_version;
}

const std::string& GEngine::GetCredits()
{
	return m_credits;
}

const std::string& GEngine::GetLinks()
{
	return m_links;
}

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/