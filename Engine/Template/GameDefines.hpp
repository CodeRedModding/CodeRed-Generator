#pragma once
#include <algorithm>
#include <stdlib.h>
#include <locale.h>
#include <xlocale>
#include <ctype.h>
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include <functional>
#include "Configuration.hpp"
#include "../../Member.hpp"

/*
# ========================================================================================= #
# Flags
# ========================================================================================= #
*/

// Function Flags
// https://docs.unrealengine.com/en-US/API/Runtime/CoreUObject/UObject/EFunctionFlags/index.html
// https://github.com/EliotVU/Unreal-Library/blob/59a9774fd7c972d55810676521e4b874d7a242e8/src/UnrealFlags.cs
enum EFunctionFlags
{
	FUNC_None = 0x00000000,
	FUNC_Final = 0x00000001,
	FUNC_Defined = 0x00000002,
	FUNC_Iterator = 0x00000004,
	FUNC_Latent = 0x00000008,
	FUNC_PreOperator = 0x00000010,
	FUNC_Singular = 0x00000020,
	FUNC_Net = 0x00000040,
	FUNC_NetReliable = 0x00000080,
	FUNC_Simulated = 0x00000100,
	FUNC_Exec = 0x00000200,
	FUNC_Native = 0x00000400,
	FUNC_Event = 0x00000800,
	FUNC_Operator = 0x00001000,
	FUNC_Static = 0x00002000,
	FUNC_NoExport = 0x00004000,
	FUNC_OptionalParm = 0x00004000,
	FUNC_Const = 0x00008000,
	FUNC_Invariant = 0x00010000,
	FUNC_Public = 0x00020000,
	FUNC_Private = 0x00040000,
	FUNC_Protected = 0x00080000,
	FUNC_Delegate = 0x00100000,
	FUNC_NetServer = 0x00200000,
	FUNC_HasOutParms = 0x00400000,
	FUNC_HasDefaults = 0x00800000,
	FUNC_NetClient = 0x01000000,
	FUNC_DLLImport = 0x02000000,
	FUNC_K2Call = 0x04000000,
	FUNC_K2Override = 0x08000000,
	FUNC_K2Pure = 0x10000000,
	FUNC_EditorOnly = 0x20000000,
	FUNC_Lambda = 0x40000000,
	FUNC_NetValidate = 0x80000000,
	FUNC_AllFlags = 0xFFFFFFFF
};

// Proprerty Flags
// https://docs.unrealengine.com/en-US/API/Runtime/CoreUObject/UObject/EPropertyFlags/index.html
enum EPropertyFlags
{
	CPF_Edit = 0x0000000000000001,	// Property is user-settable in the editor.
	CPF_Const = 0x0000000000000002,	// Actor's property always matches class's default actor property.
	CPF_Input = 0x0000000000000004,	// Variable is writable by the input system.
	CPF_ExportObject = 0x0000000000000008,	// Object can be exported with actor.
	CPF_OptionalParm = 0x0000000000000010,	// Optional parameter (if CPF_Param is set).
	CPF_Net = 0x0000000000000020,	// Property is relevant to network replication.
	CPF_EditConstArray = 0x0000000000000040,	// Prevent adding/removing of items from dynamic a array in the editor.
	CPF_Parm = 0x0000000000000080,	// Function/When call parameter.
	CPF_OutParm = 0x0000000000000100,	// Value is copied out after function call.
	CPF_SkipParm = 0x0000000000000200,	// Property is a short-circuitable evaluation function parm.
	CPF_ReturnParm = 0x0000000000000400,	// Return value.
	CPF_CoerceParm = 0x0000000000000800,	// Coerce args into this function parameter.
	CPF_Native = 0x0000000000001000,	// Property is native: C++ code is responsible for serializing it.
	CPF_Transient = 0x0000000000002000,	// Property is transient: shouldn't be saved, zero-filled at load time.
	CPF_Config = 0x0000000000004000,	// Property should be loaded/saved as permanent profile.
	CPF_Localized = 0x0000000000008000,	// Property should be loaded as localizable text.
	CPF_Travel = 0x0000000000010000,	// Property travels across levels/servers.
	CPF_EditConst = 0x0000000000020000,	// Property is uneditable in the editor.
	CPF_GlobalConfig = 0x0000000000040000,	// Load config from base class, not subclass.
	CPF_Component = 0x0000000000080000,	// Property containts component references.
	CPF_NeedCtorLink = 0x0000000000400000,	// Fields need construction/destruction.
	CPF_NoExport = 0x0000000000800000,	// Property should not be exported to the native class header file.
	CPF_NoClear = 0x0000000002000000,	// Hide clear (and browse) button.
	CPF_EditInline = 0x0000000004000000,	// Edit this object reference inline.
	CPF_EdFindable = 0x0000000008000000,	// References are set by clicking on actors in the editor viewports.
	CPF_EditInlineUse = 0x0000000010000000,	// EditInline with Use button.
	CPF_Deprecated = 0x0000000020000000,	// Property is deprecated.  Read it from an archive, but don't save it.
	CPF_EditInlineNotify = 0x0000000040000000,	// EditInline, notify outer object on editor change.
	CPF_RepNotify = 0x0000000100000000,	// Notify actors when a property is replicated
	CPF_Interp = 0x0000000200000000,	// interpolatable property for use with matinee
	CPF_NonTransactional = 0x0000000400000000,	// Property isn't transacted
	CPF_EditorOnly = 0x0000000800000000,	// Property should only be loaded in the editor.
	CPF_NoDestructor = 0x0000001000000000,	// No destructor.
	CPF_AutoWeak = 0x0000004000000000,	// CPF_ = 0x0000002000000000, ///<.
	CPF_ContainsInstancedReference = 0x0000008000000000,	// Property contains component refuerences.
	CPF_AssetRegistrySearchable = 0x0000010000000000,	// Asset instances will add properties with this flag to the asset registry automatically
	CPF_SimpleDisplay = 0x0000020000000000,	// The property is visible by default in the editor details view.
	CPF_AdvancedDisplay = 0x0000040000000000,	// The property is advanced and not visible by default in the editor details view.
	CPF_Protected = 0x0000080000000000,	// Property is protected from the perspective of scrip
	CPF_BlueprintCallable = 0x0000100000000000,	// MC Delegates only. Property should be exposed for calling in blueprint code.
	CPF_BlueprintAuthorityOnly = 0x0000200000000000,	// MC Delegates only. This delegate accepts (only in blueprint) only events with BlueprintAuthorityOnly.
	CPF_TextExportTransient = 0x0000400000000000,	// Property shouldn't be exported to text format (e.g. copy/paste)
	CPF_NonPIEDuplicateTransient = 0x0000800000000000,	// Property should only be copied in PIE.
	CPF_ExposeOnSpawn = 0x0001000000000000,	// Property is exposed on spawn.
	CPF_PersistentInstance = 0x0002000000000000,	// A object referenced by the property is duplicated like a component. (Each actor should have an own instance.)
	CPF_UObjectWrapper = 0x0004000000000000,	// Property was parsed as a wrapper class like TSubclassOf , FScriptInterface etc., rather than a USomething*.
	CPF_HasGetValueTypeHash = 0x0008000000000000,	// This property can generate a meaningful hash value.
	CPF_NativeAccessSpecifierPublic = 0x0010000000000000,	// Public native access specifier.
	CPF_NativeAccessSpecifierProtected = 0x0020000000000000,	// Protected native access specifier.
	CPF_NativeAccessSpecifierPrivate = 0x0040000000000000,	// Private native access specifier.
	CPF_SkipSerialization = 0x0080000000000000	// Property shouldn't be serialized, can still be exported to text.
};

// Object Flags
// https://docs.unrealengine.com/4.26/en-US/API/Runtime/CoreUObject/UObject/EObjectFlags/
enum EObjectFlags
{
	RF_NoFlags = 0x00000000,
	RF_Public = 0x00000001,
	RF_Standalone = 0x00000002,
	RF_MarkAsNative = 0x00000004,
	RF_Transactional = 0x00000008,
	RF_ClassDefaultObject = 0x00000010,
	RF_ArchetypeObject = 0x00000020,
	RF_Transient = 0x00000040,
	RF_MarkAsRootSet = 0x00000080,
	RF_TagGarbageTemp = 0x00000100,
	RF_NeedInitialization = 0x00000200,
	RF_NeedLoad = 0x00000400,
	RF_KeepForCooker = 0x00000800,
	RF_NeedPostLoad = 0x00001000,
	RF_NeedPostLoadSubobjects = 0x00002000,
	RF_NewerVersionExists = 0x00004000,
	RF_BeginDestroyed = 0x00008000,
	RF_FinishDestroyed = 0x00010000,
	RF_BeingRegenerated = 0x00020000,
	RF_DefaultSubObject = 0x00040000,
	RF_WasLoaded = 0x00080000,
	RF_TextExportTransient = 0x00100000,
	RF_LoadCompleted = 0x00200000,
	RF_InheritableComponentTemplate = 0x00400000,
	RF_DuplicateTransient = 0x00800000,
	RF_StrongRefOnFrame = 0x01000000,
	RF_NonPIEDuplicateTransient = 0x02000000,
	RF_Dynamic = 0x04000000,
	RF_WillBeLoaded = 0x08000000,
};

/*
# ========================================================================================= #
# Classes
# ========================================================================================= #
*/

template<typename TArray>
class TIterator
{
public:
	using ElementType = typename TArray::ElementType;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstReference = const ElementType&;

private:
	ElementPointer IteratorData;

public:
	TIterator(ElementPointer inElementPointer) : IteratorData(inElementPointer) {}
	~TIterator() {}

public:
	TIterator& operator++()
	{
		IteratorData++;
		return *this;
	}

	TIterator operator++(int32_t)
	{
		TIterator iteratorCopy = *this;
		++(*this);
		return iteratorCopy;
	}

	TIterator& operator--()
	{
		IteratorData--;
		return *this;
	}

	TIterator operator--(int32_t)
	{
		TIterator iteratorCopy = *this;
		--(*this);
		return iteratorCopy;
	}

	ElementReference operator[](int32_t index)
	{
		return *(IteratorData[index]);
	}

	ElementPointer operator->()
	{
		return IteratorData;
	}

	ElementReference operator*()
	{
		return *IteratorData;
	}

public:
	bool operator==(const TIterator& other) const
	{
		return (IteratorData == other.IteratorData);
	}

	bool operator!=(const TIterator& other) const
	{
		return !(*this == other);
	}
};

template<typename InElementType>
class TArray
{
public:
	using ElementType = InElementType;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstPointer = const ElementType*;
	using ElementConstReference = const ElementType&;
	using Iterator = TIterator<TArray<ElementType>>;

private:
	ElementPointer ArrayData;
	int32_t ArrayCount;
	int32_t ArrayMax;

public:
	TArray() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0)
	{
		//ReAllocate(sizeof(ElementType));
	}

	~TArray()
	{
		//clear();
		//::operator delete(ArrayData, ArrayMax * sizeof(ElementType));
	}

public:
	ElementConstReference operator[](int32_t index) const
	{
		return ArrayData[index];
	}

	ElementReference operator[](int32_t index)
	{
		return ArrayData[index];
	}

	ElementConstReference at(int32_t index) const
	{
		return ArrayData[index];
	}

	ElementReference at(int32_t index)
	{
		return ArrayData[index];
	}

	ElementConstPointer data() const
	{
		return ArrayData;
	}

	void push_back(ElementConstReference newElement)
	{
		if (ArrayCount >= ArrayMax)
		{
			ReAllocate(sizeof(ElementType) * (ArrayCount + 1));
		}

		new(&ArrayData[ArrayCount]) ElementType(newElement);
		ArrayCount++;
	}

	void push_back(ElementReference& newElement)
	{
		if (ArrayCount >= ArrayMax)
		{
			ReAllocate(sizeof(ElementType) * (ArrayCount + 1));
		}

		new(&ArrayData[ArrayCount]) ElementType(newElement);
		ArrayCount++;
	}

	void pop_back()
	{
		if (ArrayCount > 0)
		{
			ArrayCount--;
			ArrayData[ArrayCount].~ElementType();
		}
	}

	void clear()
	{
		for (int32_t i = 0; i < ArrayCount; i++)
		{
			ArrayData[i].~ElementType();
		}

		ArrayCount = 0;
	}

	int32_t size() const
	{
		return ArrayCount;
	}

	int32_t capacity() const
	{
		return ArrayMax;
	}

	bool empty() const
	{
		if (ArrayData)
		{
			return (size() == 0);
		}

		return true;
	}

	Iterator begin()
	{
		return Iterator(ArrayData);
	}

	Iterator end()
	{
		return Iterator(ArrayData + ArrayCount);
	}

private:
	void ReAllocate(int32_t newArrayMax)
	{
		ElementPointer newArrayData = (ElementPointer)::operator new(newArrayMax * sizeof(ElementType));
		int32_t newNum = ArrayCount;

		if (newArrayMax < newNum)
		{
			newNum = newArrayMax;
		}

		for (int32_t i = 0; i < newNum; i++)
		{
			new(newArrayData + i) ElementType(std::move(ArrayData[i]));
		}

		for (int32_t i = 0; i < ArrayCount; i++)
		{
			ArrayData[i].~ElementType();
		}

		::operator delete(ArrayData, ArrayMax * sizeof(ElementType));
		ArrayData = newArrayData;
		ArrayMax = newArrayMax;
	}
};

// THIS CLASS CAN BE GAME SPECIFIC, MOST GAMES WILL GENERATE A STRUCT MIRROR; JUST PASTE THE FIELDS HERE IF SO.
template<typename TKey, typename TValue>
class TMap
{
private:
	struct TPair
	{
		TKey Key;
		TValue Value;
		int32_t* HashNext;
	};

public:
	using ElementType = TPair;
	using ElementPointer = ElementType*;
	using ElementReference = ElementType&;
	using ElementConstReference = const ElementType&;
	using Iterator = TIterator<TArray<ElementType>>;

public:
	TArray<ElementType> Elements;									// 0x0000 (0x0010)
	uintptr_t IndirectData;											// 0x0010 (0x0008)
	int32_t InlineData[0x4];										// 0x0018 (0x0010)
	int32_t NumBits;												// 0x0028 (0x0004)
	int32_t MaxBits;												// 0x002C (0x0004)
	int32_t FirstFreeIndex;											// 0x0030 (0x0004)
	int32_t NumFreeIndices;											// 0x0034 (0x0004)
	int64_t InlineHash;												// 0x0038 (0x0008)
	int32_t* Hash;													// 0x0040 (0x0008)
	int32_t HashCount;												// 0x0048 (0x0004)

public:
	TMap() :
		IndirectData(NULL),
		NumBits(0),
		MaxBits(0),
		FirstFreeIndex(0),
		NumFreeIndices(0),
		InlineHash(0),
		Hash(nullptr),
		HashCount(0)
	{

	}

	TMap(struct FMap_Mirror& other) :
		IndirectData(NULL),
		NumBits(0),
		MaxBits(0),
		FirstFreeIndex(0),
		NumFreeIndices(0),
		InlineHash(0),
		Hash(nullptr),
		HashCount(0)
	{
		assign(other);
	}

	TMap(const TMap<TKey, TValue>& other) :
		IndirectData(NULL),
		NumBits(0),
		MaxBits(0),
		FirstFreeIndex(0),
		NumFreeIndices(0),
		InlineHash(0),
		Hash(nullptr),
		HashCount(0)
	{
		assign(other);
	}

	~TMap() {}

public:
	TMap<TKey, TValue>& assign(struct FMap_Mirror& other)
	{
		*this = *reinterpret_cast<TMap<TKey, TValue>*>(&other);
		return *this;
	}

	TMap<TKey, TValue>& assign(const TMap<TKey, TValue>& other)
	{
		Elements = other.Elements;
		IndirectData = other.IndirectData;
		InlineData[0] = other.InlineData[0];
		InlineData[1] = other.InlineData[1];
		InlineData[2] = other.InlineData[2];
		InlineData[3] = other.InlineData[3];
		NumBits = other.NumBits;
		MaxBits = other.MaxBits;
		FirstFreeIndex = other.FirstFreeIndex;
		NumFreeIndices = other.NumFreeIndices;
		InlineHash = other.InlineHash;
		Hash = other.Hash;
		HashCount = other.HashCount;
		return *this;
	}

	TValue& at(const TKey& key)
	{
		for (TPair& pair : Elements)
		{
			if (pair.Key == key)
			{
				return pair.Value;
			}
		}
	}

	const TValue& at(const TKey& key) const
	{
		for (const TPair& pair : Elements)
		{
			if (pair.Key == key)
			{
				return pair.Value;
			}
		}
	}

	TPair& at_index(int32_t index)
	{
		return Elements[index];
	}

	const TPair& at_index(int32_t index) const
	{
		return Elements[index];
	}

	int32_t size() const
	{
		return Elements.size();
	}

	int32_t capacity() const
	{
		return Elements.capacity();
	}

	bool empty() const
	{
		return Elements.empty();
	}

	Iterator begin()
	{
		return Elements.begin();
	}

	Iterator end()
	{
		return Elements.end();
	}

public:
	TValue& operator[](const TKey& key)
	{
		return at(key);
	}

	const TValue& operator[](const TKey& key) const
	{
		return at(key);
	}

	TMap<TKey, TValue>& operator=(const struct FMap_Mirror& other)
	{
		return assign(other);
	}

	TMap<TKey, TValue>& operator=(const TMap<TKey, TValue>& other)
	{
		return assign(other);
	}
};

/*
# ========================================================================================= #
# Globals
# ========================================================================================= #
*/

extern TArray<class UObject*>* GObjects;
extern TArray<class FNameEntry*>* GNames;

/*
# ========================================================================================= #
# Structs
# ========================================================================================= #
*/

// FNameEntry
// (0x0000 - 0x000C)
class FNameEntry
{
public:
	uint64_t Flags;							REGISTER_MEMBER(uint64_t, Flags, EMemberTypes::FNameEntry_Flags)					// 0x0000 (0x08)
	int32_t Index;							REGISTER_MEMBER(int32_t, Index, EMemberTypes::FNameEntry_Index)						// 0x0008 (0x04)
	FNameEntry* HashNext;

#ifdef UTF16
	wchar_t			Name[0x400];			REGISTER_MEMBER(wchar_t, Name, EMemberTypes::FNameEntry_Name)						// 0x000C (0x00)
#else
	char			Name[0x400];			REGISTER_MEMBER(char, Name, EMemberTypes::FNameEntry_Name)							// 0x000C (0x00)
#endif

public:
	FNameEntry() : Flags(0), Index(-1), HashNext(nullptr) {}
	~FNameEntry() {}

public:
	uint64_t GetFlags() const
	{
		return Flags;
	}

	int32_t GetIndex() const
	{
		return Index;
	}

#ifdef UTF16
	const wchar_t* GetWideName() const
	{
		return Name;
	}

	std::wstring ToWideString() const
	{
		const wchar_t* wideName = GetWideName();

		if (wideName)
		{
			return std::wstring(wideName);
		}

		return L"";
	}

	std::string ToString() const
	{
		std::wstring wstr = ToWideString();
		std::string str(wstr.begin(), wstr.end());
		return str;
	}
#else
	const char* GetAnsiName() const
	{
		return Name;
	}

	std::string ToString() const
	{
		return std::string(Name);
	}
#endif
};

// FName
// (0x0000 - 0x0008)
class FName
{
public:
#ifdef UTF16
	using ElementType = const wchar_t;
#else
	using ElementType = const char;
#endif
	using ElementPointer = ElementType*;

private:
	int32_t			FNameEntryId;									// 0x0000 (0x04)
	int32_t			InstanceNumber;									// 0x0004 (0x04)

public:
	FName() : FNameEntryId(-1), InstanceNumber(0) {}

	FName(int32_t id) : FNameEntryId(id), InstanceNumber(0) {}

#ifdef UTF16
	FName(const ElementPointer nameToFind) : FNameEntryId(-1), InstanceNumber(0)
	{
		static std::vector<int32_t> foundNames{};

		for (int32_t entryId : foundNames)
		{
			if (Names()->at(entryId))
			{
				if (wcscmp(Names()->at(entryId)->Name, nameToFind) == 0)
				{
					FNameEntryId = entryId;
					return;
				}
			}
		}

		for (int32_t i = 0; i < Names()->size(); i++)
		{
			if (Names()->at(i))
			{
				if (wcscmp(Names()->at(i)->Name, nameToFind) == 0)
				{
					foundNames.push_back(i);
					FNameEntryId = i;
					return;
				}
			}
		}
	}
#else
	FName(ElementPointer nameToFind) : FNameEntryId(-1), InstanceNumber(0)
	{
		static std::vector<int32_t> nameCache{};

		for (int32_t entryId : nameCache)
		{
			if (Names()->at(entryId))
			{
				if (strcmp(Names()->at(entryId)->Name, nameToFind) == 0)
				{
					FNameEntryId = entryId;
					return;
				}
			}
		}

		for (int32_t i = 0; i < Names()->size(); i++)
		{
			if (Names()->at(i))
			{
				if (strcmp(Names()->at(i)->Name, nameToFind) == 0)
				{
					nameCache.push_back(i);
					FNameEntryId = i;
				}
			}
		}
	}
#endif

	FName(const FName& name) : FNameEntryId(name.FNameEntryId), InstanceNumber(name.InstanceNumber) {}

	~FName() {}

public:
	static TArray<FNameEntry*>* Names()
	{
		TArray<FNameEntry*>* recastedArray = reinterpret_cast<TArray<FNameEntry*>*>(GNames);
		return recastedArray;
	}

	int32_t GetDisplayIndex() const
	{
		return FNameEntryId;
	}

	const FNameEntry GetDisplayNameEntry() const
	{
		if (IsValid())
		{
			return *Names()->at(FNameEntryId);
		}

		return FNameEntry();
	}

	FNameEntry* GetEntry()
	{
		if (IsValid())
		{
			return Names()->at(FNameEntryId);
		}

		return nullptr;
	}

	int32_t GetInstance() const
	{
		return InstanceNumber;
	}

	void SetInstance(int32_t newNumber)
	{
		InstanceNumber = newNumber;
	}

	std::string ToString() const
	{
		if (IsValid())
		{
			return GetDisplayNameEntry().ToString();
		}

		return "UnknownName";
	}

	bool IsValid() const
	{
		if ((FNameEntryId < 0 || FNameEntryId > Names()->size()))
		{
			return false;
		}

		return true;
	}

public:
	FName& operator=(const FName& other)
	{
		FNameEntryId = other.FNameEntryId;
		InstanceNumber = other.InstanceNumber;
		return *this;
	}

	bool operator==(const FName& other) const
	{
		return ((FNameEntryId == other.FNameEntryId) && (InstanceNumber == other.InstanceNumber));
	}

	bool operator!=(const FName& other) const
	{
		return !(*this == other);
	}
};

// FString
// (0x0000 - 0x0010)
class FString
{
public:
#ifdef UTF16
	using ElementType = const wchar_t;
#else
	using ElementType = const char;
#endif
	using ElementPointer = ElementType*;

private:
	ElementPointer	ArrayData;										// 0x0000 (0x08)
	int32_t			ArrayCount;										// 0x0008 (0x04)
	int32_t			ArrayMax;										// 0x000C (0x04)

public:
	FString() : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) {}

	FString(ElementPointer other) : ArrayData(nullptr), ArrayCount(0), ArrayMax(0) { assign(other); }

	~FString() {}

public:
#ifdef UTF16
	FString& assign(ElementPointer other)
	{
		ArrayCount = (other ? (wcslen(other) + 1) : 0);
		ArrayMax = ArrayCount;
		ArrayData = (ArrayCount > 0 ? other : nullptr);
		return *this;
	}

	std::wstring ToWideString() const
	{
		if (!empty())
		{
			return std::wstring(c_str());
		}

		return L"";
	}

	std::string ToString() const
	{
		if (!empty())
		{
			std::wstring wstr = ToWideString();
			return std::string(wstr.begin(), wstr.end());
		}

		return "";
	}
#else
	FString& assign(ElementPointer other)
	{
		ArrayCount = (other ? (strlen(other) + 1) : 0);
		ArrayMax = ArrayCount;
		ArrayData = (ArrayCount > 0 ? other : nullptr);
		return *this;
	}

	std::string ToString() const
	{
		if (!empty())
		{
			return std::string(ArrayData);
		}

		return "";
	}
#endif

	ElementPointer c_str() const
	{
		return ArrayData;
	}

	bool empty() const
	{
		if (ArrayData)
		{
			return (ArrayCount == 0);
		}

		return true;
	}

	int32_t length() const
	{
		return ArrayCount;
	}

	int32_t size() const
	{
		return ArrayMax;
	}

public:
	FString& operator=(ElementPointer other)
	{
		return assign(other);
	}

	FString& operator=(const FString& other)
	{
		return assign(other.c_str());
	}

	bool operator==(const FString& other)
	{
#ifdef UTF16
		return (wcscmp(ArrayData, other.ArrayData) == 0);
#else
		return (strcmp(ArrayData, other.ArrayData) == 0);
#endif
	}

	bool operator!=(const FString& other)
	{
#ifdef UTF16
		return (wcscmp(ArrayData, other.ArrayData) != 0);
#else
		return (strcmp(ArrayData, other.ArrayData) != 0);
#endif
	}
};

// FScriptDelegate [THIS STRUCT CAN BE GAME SPECIFIC]
// (0x0000 - 0x0010)
struct FScriptDelegate
{
	class UObject* Object;											// 0x0000 (0x08)
	class FName FunctionName;										// 0x0008 (0x08)
};

// FPointer
// (0x0000 - 0x0008)
struct FPointer
{
	uintptr_t Dummy;
};

// FQWord
// (0x0000 - 0x0008)
struct FQWord
{
	int32_t	A;
	int32_t B;
};

/*
# ========================================================================================= #
# Classes
# ========================================================================================= #
*/

// Comment this out if "SuperField" is located in UField instead of UStruct!
#define SUPERFIELDS_IN_UFIELD

// Uncommenting this will disabling using the "MinAlignment" field in UStruct, it is recommended you keep this commented.
//#define SKIP_MIN_ALIGNMENT

// Class Core.Object
// (0x0000 - 0x0034)
class UObject
{
public:
	struct FPointer VfTableObject;			REGISTER_MEMBER(struct FPointer, VfTableObject, EMemberTypes::UObject_VfTable)		// 0x0000 (0x08)
	uint8_t UnknownData00[0x10];			// Example of padding, you do not need to register this because offsets are all automatically calculated.
	int32_t ObjectInternalInteger;			REGISTER_MEMBER(int32_t, ObjectInternalInteger, EMemberTypes::UObject_Integer)		// 0x0018 (0x04)
	class UObject* Outer;					REGISTER_MEMBER(class UObject*, Outer, EMemberTypes::UObject_Outer)					// 0x001C (0x08)
	class FName Name;						REGISTER_MEMBER(class FName, Name, EMemberTypes::UObject_Name)						// 0x0024 (0x08)
	class UClass* Class;					REGISTER_MEMBER(class UClass*, Class, EMemberTypes::UObject_Class)					// 0x002C (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Object");
		}

		return uClassPointer;
	}

	static TArray<class UObject*>* GObjObjects();

	std::string GetName();
	std::string GetNameCPP();
	std::string GetFullName();
	std::string GetPackageName();
	class UObject* GetPackageObj();
	template<typename T> static T* FindObject(const std::string& objectFullName)
	{
		for (UObject* uObject : *UObject::GObjObjects())
		{
			if (uObject && uObject->IsA<T>())
			{
				if (uObject->GetFullName() == objectFullName)
				{
					return static_cast<T*>(uObject);
				}
			}
		}

		return nullptr;
	}
	template<typename T> static int32_t CountObject(const std::string& objectName)
	{
		static std::map<std::string, int32_t> objectCache;

		if (!objectCache.contains(objectName))
		{
			objectCache[objectName] = 0;

			for (UObject* uObject : *UObject::GObjObjects())
			{
				if (uObject && uObject->IsA<T>())
				{
					if (uObject->GetName() == objectName)
					{
						objectCache[uObject->GetName()]++;
					}
				}
			}
		}

		return objectCache[objectName];
	}
	static class UClass* FindClass(const std::string& classFullName);
	template<typename T> bool IsA()
	{
		if (std::is_base_of<UObject, T>::value)
		{
			return IsA(T::StaticClass());
		}
	}
	bool IsA(class UClass* uClass);
	bool IsA(int32_t objInternalInteger);
};

 //Class Core.Field
// 0x0010 (0x0034 - 0x0044)
class UField : public UObject
{
public:
	class UField* Next;						REGISTER_MEMBER(class UField*, Next, EMemberTypes::UField_Next)						// 0x0034 (0x08)
	class UField* SuperField;				REGISTER_MEMBER(class UField*, SuperField, EMemberTypes::UField_SuperField)			// 0x003C (0x08) [SUPERFIELD CAN EITHER BE HERE, OR IN USTRUCT DPENDING ON THE GAME!]

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Field");
		}

		return uClassPointer;
	};
};

// Class Core.Enum
// 0x0010 (0x0044 - 0x0054)
class UEnum : public UField
{
public:
	class TArray<class FName> Names;		REGISTER_MEMBER(class TArray<class FName>, Names, EMemberTypes::UEnum_Names)		// 0x0044 (0x10)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Enum");
		}

		return uClassPointer;
	};
};

// Class Core.Const
// 0x0010 (0x0044 - 0x0054)
class UConst : public UField
{
public:
	class FString Value;					REGISTER_MEMBER(class FString, Value, EMemberTypes::UConst_Value)					// 0x0044 (0x10)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Const");
		}

		return uClassPointer;
	};
};

// Class Core.Property
// 0x0014 (0x0044 - 0x0058)
class UProperty : public UField
{
public:
	int32_t ArrayDim;						REGISTER_MEMBER(int32_t, ArrayDim, EMemberTypes::UProperty_Dim)					// 0x0044 (0x04)
	int32_t ElementSize;					REGISTER_MEMBER(int32_t, ElementSize, EMemberTypes::UProperty_Size)				// 0x0048 (0x04)
	uint64_t PropertyFlags;					REGISTER_MEMBER(uint64_t, PropertyFlags, EMemberTypes::UProperty_Flags)				// 0x004C (0x08)
	int32_t Offset;							REGISTER_MEMBER(int32_t, Offset, EMemberTypes::UProperty_Offset)					// 0x0054 (0x04)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Property");
		}

		return uClassPointer;
	};
};

// Class Core.Struct
// 0x0014 (0x0044 - 0x0060)
class UStruct : public UField
{
public:
	class UField* SuperField;				REGISTER_MEMBER(class UField*, SuperField, EMemberTypes::UStruct_SuperField)		// 0x0044 (0x08) [SUPERFIELD CAN EITHER BE HERE, OR IN UFIELD DPENDING ON THE GAME. COMMENT OUT ACCORDINGLY!]
	class UField* Children;					REGISTER_MEMBER(class UField*, Children, EMemberTypes::UStruct_Children)			// 0x004C (0x08)
	int32_t PropertySize;					REGISTER_MEMBER(int32_t, PropertySize, EMemberTypes::UStruct_Size)					// 0x0054 (0x04)
	int32_t MinAlignment;					REGISTER_MEMBER(int32_t, MinAlignment, EMemberTypes::UStruct_Alignment)				// 0x0058 (0x04)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Struct");
		}

		return uClassPointer;
	};
};

// Class Core.Function
// 0x000A (0x0058 - 0x0062)
class UFunction : public UStruct
{
public:
	uint64_t FunctionFlags;					REGISTER_MEMBER(uint64_t, FunctionFlags, EMemberTypes::UFunction_Flags)				// 0x0058 (0x08)
	uint16_t iNative;						REGISTER_MEMBER(uint16_t, iNative, EMemberTypes::UFunction_Native)					// 0x0060 (0x02)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Function");
		}

		return uClassPointer;
	};

	static UFunction* FindFunction(const std::string& functionFullName);
};

// Class Core.ScriptStruct
// 0x0001 (0x0058 - 0x0059)
class UScriptStruct : public UStruct
{
public:
	uint8_t UnknownData00[0x01]; // 0x0058 (0x01) [USE THIS CLASSES PROPERTYSIZE IN RECLASS TO DETERMINE THE SIZE OF THE UNKNOWNDATA]

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ScriptStruct");
		}

		return uClassPointer;
	};
};

// Class Core.State
// 0x0001 (0x0058 - 0x0059)
class UState : public UStruct
{
public:
	uint8_t UnknownData00[0x01]; // 0x0058 (0x01) [USE THIS CLASSES PROPERTYSIZE IN RECLASS TO DETERMINE THE SIZE OF THE UNKNOWNDATA]

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.State");
		}

		return uClassPointer;
	};
};

// Class Core.Class
// 0x0001 (0x0058 - 0x0059)
class UClass : public UState
{
public:
	uint8_t UnknownData00[0x01]; // 0x0058 (0x00) [USE THIS CLASSES PROPERTYSIZE IN RECLASS TO DETERMINE THE SIZE OF THE UNKNOWNDATA]

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.Class");
		}
		
		return uClassPointer;
	};
};

/*
# ========================================================================================= #
# Property Classes
# ========================================================================================= #
*/

 //Class Core.StructProperty
// 0x0008 (0x0058 - 0x0060)
class UStructProperty : public UProperty
{
public:
	class UStruct* Struct;					REGISTER_MEMBER(class UStruct*, Struct, EMemberTypes::UStructProperty_Struct)			// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.StructProperty");
		}

		return uClassPointer;
	};
};

// Class Core.StrProperty
// 0x0000 (0x0058 - 0x0058)
class UStrProperty : public UProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.StrProperty");
		}

		return uClassPointer;
	};
};

// Class Core.QWordProperty
// 0x0000 (0x0058 - 0x0058)
class UQWordProperty : public UProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.QWordProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ObjectProperty
// 0x0008 (0x0058 - 0x0060)
class UObjectProperty : public UProperty
{
public:
	class UClass* PropertyClass;			REGISTER_MEMBER(class UClass*, PropertyClass, EMemberTypes::UObjectProperty_Class)		// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ObjectProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ComponentProperty
// 0x0000 (0x0058 - 0x0058)
class UComponentProperty : public UObjectProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ComponentProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ClassProperty
// 0x0008 (0x0058 - 0x0060)
class UClassProperty : public UObjectProperty
{
public:
	class UClass* MetaClass;				REGISTER_MEMBER(class UClass*, MetaClass, EMemberTypes::UClassProperty_Meta)			// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ClassProperty");
		}

		return uClassPointer;
	};
};

// Class Core.NameProperty
// 0x0000 (0x0058 - 0x0000)
class UNameProperty : public UProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.NameProperty");
		}

		return uClassPointer;
	};
};

// Class Core.MapProperty
// 0x0010 (0x0058 - 0x0068)
class UMapProperty : public UProperty
{
public:
	class UProperty* Key;					REGISTER_MEMBER(class UProperty*, Key, EMemberTypes::UMapProperty_Key)					// 0x0058 (0x08)
	class UProperty* Value;					REGISTER_MEMBER(class UProperty*, Value, EMemberTypes::UMapProperty_Value)				// 0x0060 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.MapProperty");
		}

		return uClassPointer;
	};
};

// Class Core.IntProperty
// 0x0000 (0x0058 - 0x0058)
class UIntProperty : public UProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.IntProperty");
		}

		return uClassPointer;
	};
};

// Class Core.InterfaceProperty
// 0x0008 (0x0058 - 0x0060)
class UInterfaceProperty : public UProperty
{
public:
	class UClass* InterfaceClass;			REGISTER_MEMBER(class UClass*, InterfaceClass, EMemberTypes::UInterfaceProperty_Class)	// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.InterfaceProperty");
		}

		return uClassPointer;
	};
};

// Class Core.FloatProperty
// 0x0000 (0x0058 - 0x0058)
class UFloatProperty : public UProperty
{
public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.FloatProperty");
		}

		return uClassPointer;
	};
};

// Class Core.DelegateProperty
// 0x0010 (0x0088 - 0x0068)
class UDelegateProperty : public UProperty
{
public:
	class UFunction* DelegateFunction;		// 0x0058 (0x08)
	class FName DelegateName;				// 0x0060 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.DelegateProperty");
		}

		return uClassPointer;
	};
};


// Class Core.ByteProperty
// 0x0008 (0x0058 - 0x0060)
class UByteProperty : public UProperty
{
public:
	class UEnum* Enum;						REGISTER_MEMBER(class UEnum*, Enum, EMemberTypes::UByteProperty_Enum)						// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ByteProperty");
		}

		return uClassPointer;
	};
};

// Class Core.BoolProperty
// 0x0008 (0x0058 - 0x0060)
class UBoolProperty : public UProperty
{
public:
	uint64_t BitMask;						REGISTER_MEMBER(uint64_t, BitMask, EMemberTypes::UBoolProperty_BitMask)						// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.BoolProperty");
		}

		return uClassPointer;
	};
};

// Class Core.ArrayProperty
// 0x0008 (0x0058 - 0x0060)
class UArrayProperty : public UProperty
{
public:
	class UProperty* Inner;					REGISTER_MEMBER(class UProperty*, Inner, EMemberTypes::UArrayProperty_Inner)				// 0x0058 (0x08)

public:
	static UClass* StaticClass()
	{
		static UClass* uClassPointer = nullptr;

		if (!uClassPointer)
		{
			uClassPointer = UObject::FindClass("Class Core.ArrayProperty");
		}

		return uClassPointer;
	};
};

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/