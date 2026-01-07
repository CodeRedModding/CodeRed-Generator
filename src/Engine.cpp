#include <Engine.hpp>

#include <version.hpp>

/*
# ========================================================================================= #
# Engine
# ========================================================================================= #
*/

std::string_view GEngine::m_name = PROJECT_NAME;
std::string_view GEngine::m_version = PROJECT_VERSION;
std::string_view GEngine::m_credits = PROJECT_AUTHORS;
std::string_view GEngine::m_links = PROJECT_HOMEPAGE_URL;

const std::string_view& GEngine::GetName()
{
	return m_name;
}

const std::string_view& GEngine::GetVersion()
{
	return m_version;
}

const std::string_view& GEngine::GetCredits()
{
	return m_credits;
}

const std::string_view& GEngine::GetLinks()
{
	return m_links;
}

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/