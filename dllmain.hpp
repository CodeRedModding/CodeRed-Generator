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

class UnrealProperty
{
public:
	EPropertyTypes Type;
	class UProperty* Property;
	std::string ValidName;

public:
	UnrealProperty();
	UnrealProperty(class UProperty* uProperty);
	UnrealProperty(const UnrealProperty& unrealProp);
	~UnrealProperty();

public:
	bool IsValid() const;
	std::string Hash() const;

public:
	bool IsContainer() const;
	bool IsParameter() const;
	bool IsReturnParameter() const;
	bool IsOutParameter() const;
	bool IsOptionalParameter() const;
	bool IsAnArray() const;
	bool CantConst() const;
	bool CantReference() const;
	bool CantMemcpy() const;

public:
	size_t GetSize() const;
	std::string GetType(bool bIgnoreEnum, bool bFunctionParam, bool bIgnoreConst) const;
	std::string GetTypeForClass() const;
	std::string GetTypeForStruct() const;
	std::string GetTypeForParameter(bool bIgnoreConst = false) const;

private:
	bool Assign(class UProperty* uProperty);
	bool AssignType();

public:
	bool operator>(const UnrealProperty& unrealProp);
	bool operator<(const UnrealProperty& unrealProp);
	bool operator==(const UnrealProperty& unrealProp);
	bool operator!=(const UnrealProperty& unrealProp);
	UnrealProperty& operator=(const UnrealProperty& unrealProp);
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

	template<>
	struct hash<UnrealProperty>
	{
		size_t operator()(const UnrealProperty& unrealProp) const
		{
			return hash<string>()(unrealProp.Hash());
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
	static void ClearCache();
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

	bool SortProperty(const UnrealProperty& unrealPropA, const UnrealProperty& unrealPropB);
	bool SortPropertyPair(const std::pair<UnrealProperty, std::string>& pairA, const std::pair<UnrealProperty, std::string>& pairB);

	void CreateWindowsName(std::string& functionName);
	std::string CreateValidName(std::string name);
}

namespace Retrievers
{
	void GetAllPropertyFlags(std::ostringstream& stream, uint64_t propertyFlags);
	void GetAllFunctionFlags(std::ostringstream& stream, uint64_t functionFlags);

	uintptr_t GetBaseAddress();
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
	void LogInstance(const std::string& title, const UnrealObject& unrealObj);

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