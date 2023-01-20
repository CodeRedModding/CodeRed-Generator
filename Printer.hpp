#pragma once
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdarg>

// Printer Width Types
enum class EWidthTypes : size_t
{
	NONE = 0,
	BYTE = 2,
	SIZE = 4,
	BITMASK = 8,
	FUNCTION_FLAGS = 8,
	WIDTH_FIELD = 14,
	PROPERTY_FLAGS = 16
};

namespace Printer
{
	void Empty(std::ostringstream& stream);
	void FillRight(std::ostringstream& stream, char fill, size_t width);
	void FillLeft(std::ostringstream& stream, char fill, size_t width);
	void FillRight(std::ofstream& stream, char fill, size_t width);
	void FillLeft(std::ofstream& stream, char fill, size_t width);
	std::string Hex(uint64_t decimal, size_t width);
	std::string Hex(uint64_t decimal, EWidthTypes width);
	std::string Hex(void* pointer);
	std::string Decimal(uint64_t hex, size_t width);
	std::string Decimal(uint64_t hex, EWidthTypes width);
	std::string Precision(float value, size_t precision);

	void Header(std::ostringstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush);
	void Header(std::ofstream& stream, const std::string& fileName, const std::string& fileExtension, bool bPragmaPush);
	void Section(std::ostringstream& stream, const std::string& sectionName);
	void Section(std::ofstream& stream, const std::string& sectionName);
	void Footer(std::ostringstream& stream, bool bPragmaPop);
	void Footer(std::ofstream& stream, bool bPragmaPop);
}