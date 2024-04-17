#include "Engine/Engine.hpp"

/*
# ========================================================================================= #
# Members
# ========================================================================================= #
*/

Member::Member() : Type(EMemberTypes::UNKNOWN), Label(GetLabel(Type)), Offset(0x0), Size(1) {}

Member::Member(EMemberTypes type, size_t size) : Type(type), Label(GetLabel(type)), Offset(GetOffset(type)), Size(size) {}

Member::Member(const Member& member) : Type(member.Type), Label(member.Label), Offset(member.Offset), Size(member.Size) {}

Member::~Member() {}

std::string Member::GetLabel(EMemberTypes type)
{
	switch (type)
	{
	case EMemberTypes::FNAMEENTRY_FLAGS:
		return "uint64_t Flags;";
	case EMemberTypes::FNAMEENTRY_INDEX:
		return "int32_t Index;";
	case EMemberTypes::FNAMEENTRY_NAME:
#ifdef UTF16
		return "wchar_t Name[0x400];";
#else
		return "char Name[0x400];";
#endif
	case EMemberTypes::UOBJECT_VFTABLE:
		return "struct FPointer VfTableObject;";
	case EMemberTypes::UOBJECT_INDEX:
		return "int32_t ObjectInternalInteger;";
	case EMemberTypes::UOBJECT_OUTER:
		return "class UObject* Outer;";
	case EMemberTypes::UOBJECT_NAME:
		return "struct FName Name;";
	case EMemberTypes::UOBJECT_CLASS:
		return "class UClass* Class;";
	case EMemberTypes::UFIELD_NEXT:
		return "class UField* Next;";
	case EMemberTypes::UFIELD_SUPERFIELD:
		return "class UField* SuperField;";
	case EMemberTypes::UENUM_NAMES:
		return "TArray<struct FName> Names;";
	case EMemberTypes::UCONST_VALUE:
		return "class FString Value;";
	case EMemberTypes::UPROPERTY_DIMENSION:
		return "uint32_t ArrayDim;";
	case EMemberTypes::UPROPERTY_SIZE:
		return "uint32_t ElementSize;";
	case EMemberTypes::UPROPERTY_FLAGS:
		return "uint64_t PropertyFlags;";
	case EMemberTypes::UPROPERTY_OFFSET:
		return "uint32_t Offset;";
	case EMemberTypes::USTRUCT_SUPERFIELD:
		return "class UField* SuperField;";
	case EMemberTypes::USTRUCT_CHILDREN:
		return "class UField* Children;";
	case EMemberTypes::USTRUCT_SIZE:
		return "uint32_t PropertySize;";
	case EMemberTypes::UFUNCTION_FLAGS:
		return "uint64_t FunctionFlags;";
	case EMemberTypes::UFUNCTION_NATIVE:
		return "uint16_t iNative;";
	case EMemberTypes::USTRUCTPROPERTY_STRUCT:
		return "class UStruct* Struct;";
	case EMemberTypes::UOBJECTPROPERTY_PROPERTY:
		return "class UClass* PropertyClass;";
	case EMemberTypes::UCLASSPROPERTY_METACLASS:
		return "class UClass* MetaClass;";
	case EMemberTypes::UMAPPROPERTY_KEY:
		return "class UProperty* Key;";
	case EMemberTypes::UMAPPROPERTY_VALUE:
		return "class UProperty* Value;";
	case EMemberTypes::UINTERFACEPROPERTY_CLASS:
		return "class UClass* InterfaceClass;";
	case EMemberTypes::UDELEGATEPROPERTY_FUNCTION:
		return "class UFuncton* DelegateFunction;";
	case EMemberTypes::UDELEGATEPROPERTY_NAME:
		return "struct FName DelegateName;";
	case EMemberTypes::UBYTEPROPERTY_ENUM:
		return "class UEnum* Enum;";
	case EMemberTypes::UBOOLPROPERTY_BITMASK:
		return "uint64_t BitMask;";
	case EMemberTypes::UARRAYPROPERTY_INNTER:
		return "class UProperty* Inner;";
	default:
		return "uint8_t UnknownMemberType[0x1];";
	}
}

uintptr_t Member::GetOffset(EMemberTypes type)
{
	switch (type)
	{
	case EMemberTypes::FNAMEENTRY_FLAGS:
		return offsetof(FNameEntry, Flags);
	case EMemberTypes::FNAMEENTRY_INDEX:
		return offsetof(FNameEntry, Index);
	case EMemberTypes::FNAMEENTRY_NAME:
		return offsetof(FNameEntry, Name);
	case EMemberTypes::UOBJECT_VFTABLE:
		return offsetof(UObject, VfTableObject);
	case EMemberTypes::UOBJECT_INDEX:
		return offsetof(UObject, ObjectInternalInteger);
	case EMemberTypes::UOBJECT_OUTER:
		return offsetof(UObject, Outer);
	case EMemberTypes::UOBJECT_NAME:
		return offsetof(UObject, Name);
	case EMemberTypes::UOBJECT_CLASS:
		return offsetof(UObject, Class);
	case EMemberTypes::UFIELD_NEXT:
		return offsetof(UField, Next);
	case EMemberTypes::UFIELD_SUPERFIELD:
		return offsetof(UStruct, SuperField);
	case EMemberTypes::UENUM_NAMES:
		return offsetof(UEnum, Names);
	case EMemberTypes::UCONST_VALUE:
		return offsetof(UConst, Value);
	case EMemberTypes::UPROPERTY_DIMENSION:
		return offsetof(UProperty, ArrayDim);
	case EMemberTypes::UPROPERTY_SIZE:
		return offsetof(UProperty, ElementSize);
	case EMemberTypes::UPROPERTY_FLAGS:
		return offsetof(UProperty, PropertyFlags);
	case EMemberTypes::UPROPERTY_OFFSET:
		return offsetof(UProperty, Offset);
	case EMemberTypes::USTRUCT_SUPERFIELD:
		return offsetof(UStruct, SuperField);
	case EMemberTypes::USTRUCT_CHILDREN:
		return offsetof(UStruct, Children);
	case EMemberTypes::USTRUCT_SIZE:
		return offsetof(UStruct, PropertySize);
	case EMemberTypes::UFUNCTION_FLAGS:
		return offsetof(UFunction, FunctionFlags);
	case EMemberTypes::UFUNCTION_NATIVE:
		return offsetof(UFunction, iNative);
	case EMemberTypes::USTRUCTPROPERTY_STRUCT:
		return offsetof(UStructProperty, Struct);
	case EMemberTypes::UOBJECTPROPERTY_PROPERTY:
		return offsetof(UObjectProperty, PropertyClass);
	case EMemberTypes::UCLASSPROPERTY_METACLASS:
		return offsetof(UClassProperty, MetaClass);
	case EMemberTypes::UMAPPROPERTY_KEY:
		return offsetof(UMapProperty, Key);
	case EMemberTypes::UMAPPROPERTY_VALUE:
		return offsetof(UMapProperty, Value);
	case EMemberTypes::UINTERFACEPROPERTY_CLASS:
		return offsetof(UInterfaceProperty, InterfaceClass);
	case EMemberTypes::UDELEGATEPROPERTY_FUNCTION:
		return offsetof(UDelegateProperty, DelegateFunction);
	case EMemberTypes::UDELEGATEPROPERTY_NAME:
		return offsetof(UDelegateProperty, DelegateName);
	case EMemberTypes::UBYTEPROPERTY_ENUM:
		return offsetof(UByteProperty, Enum);
	case EMemberTypes::UBOOLPROPERTY_BITMASK:
		return offsetof(UBoolProperty, BitMask);
	case EMemberTypes::UARRAYPROPERTY_INNTER:
		return offsetof(UArrayProperty, Inner);
	default:
		return 0x0;
	}
}

size_t Member::GetClassSize(EClassTypes type)
{
	switch (type)
	{
	case EClassTypes::STRUCT_FNAMEENTRY:
		return sizeof(FNameEntry);
	case EClassTypes::CLASS_UOBJECT:
		return sizeof(UObject);
	case EClassTypes::CLASS_UFIELD:
		return sizeof(UField);
	case EClassTypes::CLASS_UENUM:
		return sizeof(UEnum);
	case EClassTypes::CLASS_UCONST:
		return sizeof(UConst);
	case EClassTypes::CLASS_UPROPERTY:
		return  sizeof(UProperty);
	case EClassTypes::CLASS_USTRUCT:
		return sizeof(UStruct);
	case EClassTypes::CLASS_UFUNCTION:
		return sizeof(UFunction);
	case EClassTypes::CLASS_USTRUCTPROPERTY:
		return sizeof(UStructProperty);
	case EClassTypes::CLASS_UOBJECTPROPERTY:
		return sizeof(UObjectProperty);
	case EClassTypes::CLASS_UCLASSPROPERTY:
		return sizeof(UClassProperty);
	case EClassTypes::CLASS_UMAPPROPERTY:
		return sizeof(UMapProperty);
	case EClassTypes::CLASS_UINTERFACEPROPERTY:
		return sizeof(UInterfaceProperty);
	case EClassTypes::CLASS_UDELEGATEPROPERTY:
		return sizeof(UDelegateProperty);
	case EClassTypes::CLASS_UBYTEPROPERTY:
		return sizeof(UByteProperty);
	case EClassTypes::CLASS_UBOOLPROPERTY:
		return sizeof(UBoolProperty);
	case EClassTypes::CLASS_UARRAYPROPERTY:
		return sizeof(UArrayProperty);
	default:
		return 0;
	}
}

size_t Member::GetClassOffset(EClassTypes type)
{
	switch (type)
	{
	case EClassTypes::STRUCT_FNAMEENTRY:
		return 0;
	case EClassTypes::CLASS_UOBJECT:
		return 0;
	case EClassTypes::CLASS_UFIELD:
		return sizeof(UObject);
	case EClassTypes::CLASS_UENUM:
		return sizeof(UField);
	case EClassTypes::CLASS_UCONST:
		return sizeof(UField);
	case EClassTypes::CLASS_UPROPERTY:
		return sizeof(UField);
	case EClassTypes::CLASS_USTRUCT:
		return sizeof(UField);
	case EClassTypes::CLASS_UFUNCTION:
		return sizeof(UStruct);
	case EClassTypes::CLASS_USTRUCTPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UOBJECTPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UCLASSPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UMAPPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UINTERFACEPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UDELEGATEPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UBYTEPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UBOOLPROPERTY:
		return sizeof(UProperty);
	case EClassTypes::CLASS_UARRAYPROPERTY:
		return sizeof(UProperty);
	default:
		return 0;
	}
}

void Member::Register(EMemberTypes type, size_t size)
{
	RegisteredMembers[type] = Member(type, size);
}

std::map<uintptr_t, Member> Member::GetRegistered(EClassTypes type)
{
	std::map<uintptr_t, Member> returnMembers;

	switch (type)
	{
	case EClassTypes::STRUCT_FNAMEENTRY:
		returnMembers[RegisteredMembers[EMemberTypes::FNAMEENTRY_FLAGS].Offset] = RegisteredMembers[EMemberTypes::FNAMEENTRY_FLAGS];
		returnMembers[RegisteredMembers[EMemberTypes::FNAMEENTRY_INDEX].Offset] = RegisteredMembers[EMemberTypes::FNAMEENTRY_INDEX];
		returnMembers[RegisteredMembers[EMemberTypes::FNAMEENTRY_NAME].Offset] = RegisteredMembers[EMemberTypes::FNAMEENTRY_NAME];
		break;
	case EClassTypes::CLASS_UOBJECT:
		returnMembers[RegisteredMembers[EMemberTypes::UOBJECT_VFTABLE].Offset] = RegisteredMembers[EMemberTypes::UOBJECT_VFTABLE];
		returnMembers[RegisteredMembers[EMemberTypes::UOBJECT_INDEX].Offset] = RegisteredMembers[EMemberTypes::UOBJECT_INDEX];
		returnMembers[RegisteredMembers[EMemberTypes::UOBJECT_OUTER].Offset] = RegisteredMembers[EMemberTypes::UOBJECT_OUTER];
		returnMembers[RegisteredMembers[EMemberTypes::UOBJECT_NAME].Offset] = RegisteredMembers[EMemberTypes::UOBJECT_NAME];
		returnMembers[RegisteredMembers[EMemberTypes::UOBJECT_CLASS].Offset] = RegisteredMembers[EMemberTypes::UOBJECT_CLASS];
		break;
	case EClassTypes::CLASS_UFIELD:
		returnMembers[RegisteredMembers[EMemberTypes::UFIELD_NEXT].Offset] = RegisteredMembers[EMemberTypes::UFIELD_NEXT];

		if (RegisteredMembers.contains(EMemberTypes::UFIELD_SUPERFIELD))
		{
			returnMembers[RegisteredMembers[EMemberTypes::UFIELD_SUPERFIELD].Offset] = RegisteredMembers[EMemberTypes::UFIELD_SUPERFIELD];
		}

		break;
	case EClassTypes::CLASS_UENUM:
		returnMembers[RegisteredMembers[EMemberTypes::UENUM_NAMES].Offset] = RegisteredMembers[EMemberTypes::UENUM_NAMES];
		break;
	case EClassTypes::CLASS_UCONST:
		returnMembers[RegisteredMembers[EMemberTypes::UCONST_VALUE].Offset] = RegisteredMembers[EMemberTypes::UCONST_VALUE];
		break;
	case EClassTypes::CLASS_UPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UPROPERTY_DIMENSION].Offset] = RegisteredMembers[EMemberTypes::UPROPERTY_DIMENSION];
		returnMembers[RegisteredMembers[EMemberTypes::UPROPERTY_SIZE].Offset] = RegisteredMembers[EMemberTypes::UPROPERTY_SIZE];
		returnMembers[RegisteredMembers[EMemberTypes::UPROPERTY_FLAGS].Offset] = RegisteredMembers[EMemberTypes::UPROPERTY_FLAGS];
		returnMembers[RegisteredMembers[EMemberTypes::UPROPERTY_OFFSET].Offset] = RegisteredMembers[EMemberTypes::UPROPERTY_OFFSET];
		break;
	case EClassTypes::CLASS_USTRUCT:
		if (RegisteredMembers.contains(EMemberTypes::USTRUCT_SUPERFIELD))
		{
			returnMembers[RegisteredMembers[EMemberTypes::USTRUCT_SUPERFIELD].Offset] = RegisteredMembers[EMemberTypes::USTRUCT_SUPERFIELD];
		}

		returnMembers[RegisteredMembers[EMemberTypes::USTRUCT_CHILDREN].Offset] = RegisteredMembers[EMemberTypes::USTRUCT_CHILDREN];
		returnMembers[RegisteredMembers[EMemberTypes::USTRUCT_SIZE].Offset] = RegisteredMembers[EMemberTypes::USTRUCT_SIZE];
		break;
	case EClassTypes::CLASS_UFUNCTION:
		returnMembers[RegisteredMembers[EMemberTypes::UFUNCTION_FLAGS].Offset] = RegisteredMembers[EMemberTypes::UFUNCTION_FLAGS];
		returnMembers[RegisteredMembers[EMemberTypes::UFUNCTION_NATIVE].Offset] = RegisteredMembers[EMemberTypes::UFUNCTION_NATIVE];
		break;
	case EClassTypes::CLASS_USTRUCTPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::USTRUCTPROPERTY_STRUCT].Offset] = RegisteredMembers[EMemberTypes::USTRUCTPROPERTY_STRUCT];
		break;
	case EClassTypes::CLASS_UOBJECTPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UOBJECTPROPERTY_PROPERTY].Offset] = RegisteredMembers[EMemberTypes::UOBJECTPROPERTY_PROPERTY];
		break;
	case EClassTypes::CLASS_UCLASSPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UCLASSPROPERTY_METACLASS].Offset] = RegisteredMembers[EMemberTypes::UCLASSPROPERTY_METACLASS];
		break;
	case EClassTypes::CLASS_UMAPPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UMAPPROPERTY_KEY].Offset] = RegisteredMembers[EMemberTypes::UMAPPROPERTY_KEY];
		returnMembers[RegisteredMembers[EMemberTypes::UMAPPROPERTY_VALUE].Offset] = RegisteredMembers[EMemberTypes::UMAPPROPERTY_VALUE];
		break;
	case EClassTypes::CLASS_UINTERFACEPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UINTERFACEPROPERTY_CLASS].Offset] = RegisteredMembers[EMemberTypes::UINTERFACEPROPERTY_CLASS];
		break;
	case EClassTypes::CLASS_UDELEGATEPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UDELEGATEPROPERTY_FUNCTION].Offset] = RegisteredMembers[EMemberTypes::UDELEGATEPROPERTY_FUNCTION];
		returnMembers[RegisteredMembers[EMemberTypes::UDELEGATEPROPERTY_NAME].Offset] = RegisteredMembers[EMemberTypes::UDELEGATEPROPERTY_NAME];
		break;
	case EClassTypes::CLASS_UBYTEPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UBYTEPROPERTY_ENUM].Offset] = RegisteredMembers[EMemberTypes::UBYTEPROPERTY_ENUM];
		break;
	case EClassTypes::CLASS_UBOOLPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UBOOLPROPERTY_BITMASK].Offset] = RegisteredMembers[EMemberTypes::UBOOLPROPERTY_BITMASK];
		break;
	case EClassTypes::CLASS_UARRAYPROPERTY:
		returnMembers[RegisteredMembers[EMemberTypes::UARRAYPROPERTY_INNTER].Offset] = RegisteredMembers[EMemberTypes::UARRAYPROPERTY_INNTER];
		break;
	}

	return returnMembers;
}

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/