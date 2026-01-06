#pragma once

#include <cstdint>

#include <sstream>
#include <fstream>
#include <string>
#include <cstdarg>

enum class EWidthTypes : uint32_t
{
	None = 0,
	Byte = 2,
	Size = 4,
	BitMask = 8,
	FunctionFlags = 8,
	FieldWidth = 14,
	PropertyFlags = 16
};

namespace Printer
{
	void Empty(std::ostringstream& stream);
	void FillRight(std::ostringstream& stream, char fill, uint32_t width);
	void FillLeft(std::ostringstream& stream, char fill, uint32_t width);
	void FillRight(std::ofstream& stream, char fill, uint32_t width);
	void FillLeft(std::ofstream& stream, char fill, uint32_t width);
	void ReplaceChars(std::string& baseStr, char oldChar, char newChar);
	void ReplaceString(std::string& baseStr, const std::string& strToReplace, const std::string& replaceWithStr);
	std::string Hex(uintptr_t decimal, uint32_t width);
	std::string Hex(uintptr_t decimal, EWidthTypes width = EWidthTypes::None);
	std::string Hex(void* pointer);
	std::string Decimal(uintptr_t hex, uint32_t width);
	std::string Decimal(uintptr_t hex, EWidthTypes width = EWidthTypes::None);
	std::string Precision(float value, uint32_t precision);
	std::string ToUpper(std::string str);
	std::string ToLower(std::string str);

	void Header(std::ostringstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush);
	void Header(std::ofstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush);
	void Section(std::ostringstream& stream, const std::string& sectionName);
	void Section(std::ofstream& stream, const std::string& sectionName);
	void Footer(std::ostringstream& stream, bool bPragmaPop);
	void Footer(std::ofstream& stream, bool bPragmaPop);
}