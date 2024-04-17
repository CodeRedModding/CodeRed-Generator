#pragma once
#include "pch.hpp"
#include "Printer.hpp"
#include "Engine/Engine.hpp"

namespace Utils
{
	void Messagebox(const std::string& message, uint32_t flags);
	bool MapExists(std::multimap<std::string, std::string>& map, const std::string& key, const std::string& value);
	bool SortProperty(class UProperty* uPropertyA, class UProperty* uPropertyB);
	bool SortPropertyPair(const std::pair<class UProperty*, std::string>& pairA, const std::pair<class UProperty*, std::string>& pairB);
	bool IsStructProperty(EPropertyTypes propertyType);
	bool IsBitField(uint32_t arrayDim);
	bool CantMemcpy(EPropertyTypes propertyType);

	std::string CreateValidName(std::string name);
	std::string CreateUniqueName(class UClass* uClass);
	std::string CreateUniqueName(class UFunction* uFunction, class UClass* uClass);
	void MakeWinSafe(std::string& functionName);
}

namespace Retrievers
{
	void GetAllPropertyFlags(std::ostringstream& stream, uint64_t propertyFlags);
	void GetAllFunctionFlags(std::ostringstream& stream, uint64_t functionFlags);
	EPropertyTypes GetPropertyTypeInternal(class UProperty* uProperty, std::string& outPropertyType, bool bIgnoreEnum, bool bDescription, bool bIsBitField);
	EPropertyTypes GetPropertyTypeDesc(class UProperty* uProperty, std::string& outPropertyType, bool bIsBitField = false);
	EPropertyTypes GetPropertyType(class UProperty* uProperty, std::string& outPropertyType, bool bIsBitField = false);
	size_t GetPropertySize(class UProperty* uProperty, bool bIsBitField = true);
	uintptr_t GetEntryPoint();
	uintptr_t GetOffset(void* pointer);
	uintptr_t FindPattern(const uint8_t* pattern, const std::string& mask);
}

namespace ConstGenerator
{
	void GenerateConst(std::ofstream& file, class UConst* constant);
	void ProcessConsts(std::ofstream& file, class UObject* packageObj);
}

namespace EnumGenerator
{
	extern std::unordered_map<std::string, std::vector<class UEnum*>> mEnumCache;
	extern std::unordered_map<UEnum*, std::string> mGeneratedNames;
	std::string GenerateEnumName(class UEnum* uEnum);
	void GenerateEnum(std::ofstream& file, class UEnum* uEnum);
	void ProcessEnums(std::ofstream& file, class UObject* packageObj);
}

namespace StructGenerator
{
	extern std::vector<std::string> vGeneratedStructs;
	class UScriptStruct* FindLargestStruct(const std::string& structFullName);
	void GenerateStructFields(std::ofstream& structStream, EClassTypes structType);
	void GenerateStruct(std::ofstream& file, class UScriptStruct* scriptStruct);
	void GenerateStructProperties(std::ofstream& file, class UScriptStruct* scriptStruct, class UObject* packageObj);
	void ProcessStructs(std::ofstream& file, class UObject* packageObj);
}

namespace ClassGenerator
{
	extern std::unordered_map<std::string, int32_t> mGeneratedClasses;
	void GenerateClassFields(std::ostringstream& classStream, class UClass* uClass, EClassTypes classType);
	void GenerateClass(std::ofstream& file, class UClass* uClass);
	void GenerateClassProperties(std::ofstream& file, class UClass* uClass, class UObject* packageObj);
	void ProcessClasses(std::ofstream& file, class UObject* packageObj);
}

namespace ParameterGenerator
{
	void GenerateParameter(std::ofstream& file, class UClass* uClass);
	void ProcessParameters(std::ofstream& file, class UObject* packageObj);
}

namespace FunctionGenerator
{
	void GenerateVirtualFunctions(std::ofstream& file);
	void GenerateFunctionCode(std::ofstream& file, class UClass* uClass);
	void GenerateFunctionDescription(std::ofstream& file, class UClass* uClass);
	void ProcessFunctions(std::ofstream& file, class UObject* packageObj);
}

namespace Generator
{
	extern bool GlobalsInitialized;
	extern std::ofstream LogFile;
	extern std::vector<class UObject*> vPackages;
	extern std::vector<class UObject*> vIncludes;
	extern std::vector<std::pair<std::string, int32_t>> vConstants;

	std::string GenerateIndex(class UObject* uObject, bool bPushBack = true);
	void GenerateConstants();
	void GenerateHeaders();
	void GenerateDefines();
	void ProcessPackages(const std::filesystem::path& directory);
	void GenerateSDK();

	bool Initialize(bool bCreateLog);
	void DumpInstances(bool bNames, bool bObjects);
	void DumpGObjects();
	void DumpGNames();

	bool AreGObjectsValid();
	bool AreGNamesValid();
	bool AreGlobalsValid();
}