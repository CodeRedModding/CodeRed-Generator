#include "Engine/Engine.hpp"

/*
# ========================================================================================= #
# Members
# ========================================================================================= #
*/

Member::Member() : Type(EMemberTypes::Unknown), Label(GetLabel(Type)), Offset(0), Size(1) {}

Member::Member(EMemberTypes type, size_t size) : Type(type), Label(GetLabel(type)), Offset(GetOffset(type)), Size(size) {}

Member::Member(const Member& member) : Type(member.Type), Label(member.Label), Offset(member.Offset), Size(member.Size) {}

Member::~Member() {}

std::string Member::GetName(EClassTypes type)
{
	switch (type)
	{
	case EClassTypes::FNameEntry:
		return "FNameEntry";
	case EClassTypes::UObject:
		return "UObject";
	case EClassTypes::UField:
		return "UField";
	case EClassTypes::UEnum:
		return "UEnum";
	case EClassTypes::UConst:
		return "UConst";
	case EClassTypes::UProperty:
		return "UProperty";
	case EClassTypes::UStruct:
		return "UStruct";
	case EClassTypes::UFunction:
		return "UFunction";
	case EClassTypes::UStructProperty:
		return "UStructProperty";
	case EClassTypes::UObjectProperty:
		return "UObjectProperty";
	case EClassTypes::UClassProperty:
		return "UClassProperty";
	case EClassTypes::UMapProperty:
		return "UMapProperty";
	case EClassTypes::UInterfaceProperty:
		return "UInterfaceProperty";
	case EClassTypes::UByteProperty:
		return "UByteProperty";
	case EClassTypes::UBoolProperty:
		return "UBoolProperty";
	case EClassTypes::UArrayProperty:
		return "UArrayProperty";
	default:
		return "Unknown";
	}
}

std::string Member::GetLabel(EMemberTypes type)
{
	switch (type)
	{
	case EMemberTypes::FNameEntry_HashNext:
		return "struct FNameEntry* HashNext;";
	case EMemberTypes::FNameEntry_Index:
		return "int32_t Index;";
	case EMemberTypes::FNameEntry_Flags:
		return "uint64_t Flags;";
	case EMemberTypes::FNameEntry_Name:
#ifdef UTF16
		return "wchar_t Name[0x400];";
#else
		return "char Name[0x400];";
#endif
	case EMemberTypes::UObject_VfTable:
		return "struct FPointer VfTableObject;";
	case EMemberTypes::UObject_Integer:
		return "int32_t ObjectInternalInteger;";
	case EMemberTypes::UObject_Outer:
		return "class UObject* Outer;";
	case EMemberTypes::UObject_Name:
		return "class FName Name;";
	case EMemberTypes::UObject_Class:
		return "class UClass* Class;";
	case EMemberTypes::UField_Next:
		return "class UField* Next;";
	case EMemberTypes::UField_SuperField:
		return "class UField* SuperField;";
	case EMemberTypes::UEnum_Names:
		return "class TArray<class FName> Names;";
	case EMemberTypes::UConst_Value:
		return "class FString Value;";
	case EMemberTypes::UProperty_Dim:
		return "int32_t ArrayDim;";
	case EMemberTypes::UProperty_Size:
		return "int32_t ElementSize;";
	case EMemberTypes::UProperty_Flags:
		return "uint64_t PropertyFlags;";
	case EMemberTypes::UProperty_Offset:
		return "int32_t Offset;";
	case EMemberTypes::UStruct_SuperField:
		return "class UField* SuperField;";
	case EMemberTypes::UStruct_Children:
		return "class UField* Children;";
	case EMemberTypes::UStruct_Size:
		return "int32_t PropertySize;";
	case EMemberTypes::UStruct_Alignment:
		return "int32_t MinAlignment;";
	case EMemberTypes::UFunction_Flags:
		return "uint64_t FunctionFlags;";
	case EMemberTypes::UFunction_Native:
		return "uint16_t iNative;";
	case EMemberTypes::UStructProperty_Struct:
		return "class UStruct* Struct;";
	case EMemberTypes::UObjectProperty_Class:
		return "class UClass* PropertyClass;";
	case EMemberTypes::UClassProperty_Meta:
		return "class UClass* MetaClass;";
	case EMemberTypes::UMapProperty_Key:
		return "class UProperty* Key;";
	case EMemberTypes::UMapProperty_Value:
		return "class UProperty* Value;";
	case EMemberTypes::UInterfaceProperty_Class:
		return "class UClass* InterfaceClass;";
	case EMemberTypes::UByteProperty_Enum:
		return "class UEnum* Enum;";
	case EMemberTypes::UBoolProperty_BitMask:
		return "uint64_t BitMask;";
	case EMemberTypes::UArrayProperty_Inner:
		return "class UProperty* Inner;";
	default:
		return "uint8_t UnknownMemberType[0x1];";
	}
}

uintptr_t Member::GetOffset(EMemberTypes type)
{
	switch (type)
	{
	case EMemberTypes::FNameEntry_HashNext:
		return offsetof(FNameEntry, HashNext);
	case EMemberTypes::FNameEntry_Index:
		return offsetof(FNameEntry, Index);
	case EMemberTypes::FNameEntry_Flags:
		return offsetof(FNameEntry, Flags);
	case EMemberTypes::FNameEntry_Name:
		return offsetof(FNameEntry, Name);
	case EMemberTypes::UObject_VfTable:
		return offsetof(UObject, VfTableObject);
	case EMemberTypes::UObject_Integer:
		return offsetof(UObject, ObjectInternalInteger);
	case EMemberTypes::UObject_Outer:
		return offsetof(UObject, Outer);
	case EMemberTypes::UObject_Name:
		return offsetof(UObject, Name);
	case EMemberTypes::UObject_Class:
		return offsetof(UObject, Class);
	case EMemberTypes::UField_Next:
		return offsetof(UField, Next);
#ifdef SUPERFIELDS_IN_UFIELD
	case EMemberTypes::UField_SuperField:
		return offsetof(UField, SuperField);
#endif
	case EMemberTypes::UEnum_Names:
		return offsetof(UEnum, Names);
	case EMemberTypes::UConst_Value:
		return offsetof(UConst, Value);
	case EMemberTypes::UProperty_Dim:
		return offsetof(UProperty, ArrayDim);
	case EMemberTypes::UProperty_Size:
		return offsetof(UProperty, ElementSize);
	case EMemberTypes::UProperty_Flags:
		return offsetof(UProperty, PropertyFlags);
	case EMemberTypes::UProperty_Offset:
		return offsetof(UProperty, Offset);
#ifndef SUPERFIELDS_IN_UFIELD
	case EMemberTypes::UStruct_SuperField:
		return offsetof(UStruct, SuperField);
#endif
	case EMemberTypes::UStruct_Children:
		return offsetof(UStruct, Children);
	case EMemberTypes::UStruct_Size:
		return offsetof(UStruct, PropertySize);
	case EMemberTypes::UStruct_Alignment:
		return offsetof(UStruct, MinAlignment);
	case EMemberTypes::UFunction_Flags:
		return offsetof(UFunction, FunctionFlags);
	case EMemberTypes::UFunction_Native:
		return offsetof(UFunction, iNative);
	case EMemberTypes::UStructProperty_Struct:
		return offsetof(UStructProperty, Struct);
	case EMemberTypes::UObjectProperty_Class:
		return offsetof(UObjectProperty, PropertyClass);
	case EMemberTypes::UClassProperty_Meta:
		return offsetof(UClassProperty, MetaClass);
	case EMemberTypes::UMapProperty_Key:
		return offsetof(UMapProperty, Key);
	case EMemberTypes::UMapProperty_Value:
		return offsetof(UMapProperty, Value);
	case EMemberTypes::UInterfaceProperty_Class:
		return offsetof(UInterfaceProperty, InterfaceClass);
	case EMemberTypes::UByteProperty_Enum:
		return offsetof(UByteProperty, Enum);
	case EMemberTypes::UBoolProperty_BitMask:
		return offsetof(UBoolProperty, BitMask);
	case EMemberTypes::UArrayProperty_Inner:
		return offsetof(UArrayProperty, Inner);
	default:
		return 0;
	}
}

size_t Member::GetClassSize(EClassTypes type)
{
	switch (type)
	{
	case EClassTypes::FNameEntry:
		return sizeof(FNameEntry);
	case EClassTypes::UObject:
		return sizeof(UObject);
	case EClassTypes::UField:
		return sizeof(UField);
	case EClassTypes::UEnum:
		return sizeof(UEnum);
	case EClassTypes::UConst:
		return sizeof(UConst);
	case EClassTypes::UProperty:
		return  sizeof(UProperty);
	case EClassTypes::UStruct:
		return sizeof(UStruct);
	case EClassTypes::UFunction:
		return sizeof(UFunction);
	case EClassTypes::UStructProperty:
		return sizeof(UStructProperty);
	case EClassTypes::UObjectProperty:
		return sizeof(UObjectProperty);
	case EClassTypes::UClassProperty:
		return sizeof(UClassProperty);
	case EClassTypes::UMapProperty:
		return sizeof(UMapProperty);
	case EClassTypes::UInterfaceProperty:
		return sizeof(UInterfaceProperty);
	case EClassTypes::UByteProperty:
		return sizeof(UByteProperty);
	case EClassTypes::UBoolProperty:
		return sizeof(UBoolProperty);
	case EClassTypes::UArrayProperty:
		return sizeof(UArrayProperty);
	default:
		return 0;
	}
}

size_t Member::GetClassOffset(EClassTypes type)
{
	switch (type)
	{
	case EClassTypes::FNameEntry:
		return 0;
	case EClassTypes::UObject:
		return 0;
	case EClassTypes::UField:
		return sizeof(UObject);
	case EClassTypes::UEnum:
		return sizeof(UField);
	case EClassTypes::UConst:
		return sizeof(UField);
	case EClassTypes::UProperty:
		return sizeof(UField);
	case EClassTypes::UStruct:
		return sizeof(UField);
	case EClassTypes::UFunction:
		return sizeof(UStruct);
	case EClassTypes::UStructProperty:
		return sizeof(UProperty);
	case EClassTypes::UObjectProperty:
		return sizeof(UProperty);
	case EClassTypes::UClassProperty:
		return sizeof(UObjectProperty);
	case EClassTypes::UMapProperty:
		return sizeof(UProperty);
	case EClassTypes::UInterfaceProperty:
		return sizeof(UProperty);
	case EClassTypes::UByteProperty:
		return sizeof(UProperty);
	case EClassTypes::UBoolProperty:
		return sizeof(UProperty);
	case EClassTypes::UArrayProperty:
		return sizeof(UProperty);
	default:
		return 0;
	}
}

void Member::Register(EMemberTypes type, size_t size)
{
	m_registeredMembers[type] = Member(type, size);
}

std::map<size_t, Member*> Member::GetRegistered(EClassTypes type)
{
	std::map<size_t, Member*> members;

	if (m_classMembers.contains(type))
	{
		for (EMemberTypes member : m_classMembers[type])
		{
			AddRegistered(members, member);
		}
	}

	return members;
}

void Member::AddRegistered(std::map<size_t, Member*>& members, EMemberTypes type)
{
	if (m_registeredMembers.contains(type))
	{
		members[m_registeredMembers[type].Offset] = &m_registeredMembers[type];
	}
}

std::map<EClassTypes, std::vector<EMemberTypes>> Member::m_classMembers = {
	// Core Objects

	{ EClassTypes::FNameEntry, {
		EMemberTypes::FNameEntry_HashNext,
		EMemberTypes::FNameEntry_Index,
		EMemberTypes::FNameEntry_Flags,
		EMemberTypes::FNameEntry_Name
	} },

	{ EClassTypes::UObject, {
		EMemberTypes::UObject_VfTable,
		EMemberTypes::UObject_Integer,
		EMemberTypes::UObject_Outer,
		EMemberTypes::UObject_Name,
		EMemberTypes::UObject_Class
	} },

	{ EClassTypes::UField, {
		EMemberTypes::UField_Next,
		EMemberTypes::UField_Flags,
		EMemberTypes::UField_SuperField
	} },

	{ EClassTypes::UEnum, {
		EMemberTypes::UEnum_Names
	} },

	{ EClassTypes::UConst, {
		EMemberTypes::UConst_Value
	} },

	{ EClassTypes::UProperty, {
		EMemberTypes::UProperty_Dim,
		EMemberTypes::UProperty_Size,
		EMemberTypes::UProperty_Flags,
		EMemberTypes::UProperty_Offset
	} },

	{ EClassTypes::UStruct, {
		EMemberTypes::UStruct_SuperField,
		EMemberTypes::UStruct_Children,
		EMemberTypes::UStruct_Size,
		EMemberTypes::UStruct_Alignment
	} },

	{ EClassTypes::UFunction, {
		EMemberTypes::UFunction_Flags,
		EMemberTypes::UFunction_Native
	} },

	// Property Objects

	{ EClassTypes::UStructProperty, {
		EMemberTypes::UStructProperty_Struct
	} },

	{ EClassTypes::UObjectProperty, {
		EMemberTypes::UObjectProperty_Class
	} },

	{ EClassTypes::UClassProperty, {
		EMemberTypes::UClassProperty_Meta
	} },

	{ EClassTypes::UMapProperty, {
		EMemberTypes::UMapProperty_Key,
		EMemberTypes::UMapProperty_Value,
	} },

	{ EClassTypes::UInterfaceProperty, {
		EMemberTypes::UInterfaceProperty_Class
	} },

	{ EClassTypes::UByteProperty, {
		EMemberTypes::UByteProperty_Enum
	} },

	{ EClassTypes::UBoolProperty, {
		EMemberTypes::UBoolProperty_BitMask
	} },

	{ EClassTypes::UArrayProperty, {
		EMemberTypes::UArrayProperty_Inner
	} },
};

Member& Member::operator=(const Member& member)
{
	Type = member.Type;
	Label = member.Label;
	Offset = member.Offset;
	Size = member.Size;
	return *this;
}

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/