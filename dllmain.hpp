#pragma once
#include "pch.hpp"
#include "Printer.hpp"
#include "Engine/Engine.hpp"

class UnrealObject
{
public:
	EClassTypes Type;
	class UObject* Object;
	class UObject* Package;
	std::string FullName;
	std::string ValidName;

public:
	UnrealObject();
	UnrealObject(class UObject* uObject);
	UnrealObject(const UnrealObject& unrealObj);
	~UnrealObject();

public:
	bool IsValid() const;
	std::string Hash() const;

private:
	bool Assign(class UObject* uObject);
	bool AssignType();

public:
	bool operator>(const UnrealObject& unrealObj);
	bool operator<(const UnrealObject& unrealObj);
	bool operator==(const UnrealObject& unrealObj);
	bool operator!=(const UnrealObject& unrealObj);
	UnrealObject& operator=(const UnrealObject& unrealObj);
};

namespace std
{
	template<>
	struct hash<UnrealObject>
	{
		size_t operator()(const UnrealObject& unrealObj) const
		{
			return hash<string>()(unrealObj.Hash());
		}
	};
}

class GCache
{
private:
	static inline std::map<class UObject*, std::vector<UnrealObject>> m_consts;
	static inline std::map<class UObject*, std::vector<UnrealObject>> m_enums;
	static inline std::map<class UObject*, std::vector<UnrealObject>> m_structs;
	static inline std::map<class UObject*, std::vector<UnrealObject>> m_classes;
	static inline std::vector<std::pair<class UObject*, std::string>> m_includes;
	static inline std::map<std::string, class UObject*> m_constants;
	static inline std::vector<class UObject*> m_packages;

public:
	static void Initialize();
	static std::vector<UnrealObject>* GetCache(class UObject* packageObj, EClassTypes type);
	static std::vector<std::pair<class UObject*, std::string>>* GetIncludes();
	static std::map<std::string, class UObject*>* GetConstants();
	static std::vector<class UObject*>* GetPackages();

public:
	static std::pair<std::string, class UObject*> GetConstant(const UnrealObject& unrealObj);
	static UnrealObject GetLargestStruct(const std::string& structFullName);
	static UnrealObject GetClass(class UClass* staticClass);

private:
	static void CacheObject(UnrealObject& unrealObj);
	static void CacheConstant(UnrealObject& unrealObj);
};

namespace Utils
{
	void MessageboxInfo(const std::string& message);
	void MessageboxWarn(const std::string& message);
	void MessageboxError(const std::string& message);

	bool SortProperty(class UProperty* uPropertyA, class UProperty* uPropertyB);
	bool SortPropertyPair(const std::pair<class UProperty*, std::string>& pairA, const std::pair<class UProperty*, std::string>& pairB);
	bool CantMemcpy(EPropertyTypes propertyType);
	bool IsStructProperty(EPropertyTypes propertyType);
	bool IsBitField(int32_t arrayDim);

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
	std::string GenerateConstName(class UConst* uConst);
	void GenerateConst(std::ofstream& stream, const UnrealObject& unrealObj);
	void ProcessConsts(std::ofstream& stream, class UObject* packageObj);
}

namespace EnumGenerator
{
	std::string GenerateEnumName(class UEnum* uEnum);
	void GenerateEnum(std::ofstream& stream, const UnrealObject& unrealObj);
	void ProcessEnums(std::ofstream& stream, class UObject* packageObj);
}

namespace StructGenerator
{
	void GenerateStructMembers(std::ofstream& structStream, EClassTypes type);
	void GenerateStruct(std::ofstream& stream, const UnrealObject& unrealObj);
	void GenerateStructPre(std::ofstream& stream, const UnrealObject& unrealObj);
	void ProcessStructs(std::ofstream& stream, class UObject* packageObj);
}

namespace ClassGenerator
{
	void GenerateClassMembers(std::ostringstream& classStream, class UClass* uClass, EClassTypes classType);
	void GenerateClass(std::ofstream& stream, const UnrealObject& unrealObj);
	void GenerateClassPre(std::ofstream& stream, const UnrealObject& unrealObj, class UObject* packageObj);
	void ProcessClasses(std::ofstream& stream, class UObject* packageObj);
}

namespace ParameterGenerator
{
	void GenerateParameter(std::ofstream& stream, const UnrealObject& unrealObj);
	void ProcessParameters(std::ofstream& stream, class UObject* packageObj);
}

namespace FunctionGenerator
{
	void GenerateVirtualFunctions(std::ofstream& stream);
	void GenerateFunctionCode(std::ofstream& stream, const UnrealObject& unrealObj);
	void GenerateFunctionDescription(std::ofstream& stream, const UnrealObject& unrealObj);
	void ProcessFunctions(std::ofstream& stream, class UObject* packageObj);
}

namespace Generator
{
	extern std::ofstream LogFile;

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