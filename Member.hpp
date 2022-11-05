#pragma once
#include <string>

// Class Types
enum class EClassTypes : uint8_t
{
	UNKNOWN,
	STRUCT_FNAMEENTRY,
	STRUCT_FNAME,
	CLASS_UOBJECT,
	CLASS_UFIELD,
	CLASS_UENUM,
	CLASS_UCONST,
	CLASS_UPROPERTY,
	CLASS_USTRUCT,
	CLASS_UFUNCTION,
	CLASS_USCRIPTSTRUCT,
	CLASS_USTATE,
	CLASS_UCLASS,
	CLASS_USTRUCTPROPERTY,
	CLASS_USTRPROPERTY,
	CLASS_UQWORDPROPERTY,
	CLASS_UOBJECTPROPERTY,
	CLASS_UCOMPONENTPROPERTY,
	CLASS_UCLASSPROPERTY,
	CLASS_UNAMEPROPERTY,
	CLASS_UMAPPROPERTY,
	CLASS_UINTPROPERTY,
	CLASS_UINTERFACEPROPERTY,
	CLASS_UFLOATPROPERTY,
	CLASS_UDELEGATEPROPERTY,
	CLASS_UBYTEPROPERTY,
	CLASS_UBOOLPROPERTY,
	CLASS_UARRAYPROPERTY
};

// Class Member Types
enum class EMemberTypes : uint8_t
{
	UNKNOWN,
	FNAMEENTRY_FLAGS,
	FNAMEENTRY_INDEX,
	FNAMEENTRY_NAME,
	UOBJECT_VFTABLE,
	UOBJECT_INDEX,
	UOBJECT_OUTER,
	UOBJECT_NAME,
	UOBJECT_CLASS,
	UFIELD_NEXT,
	UFIELD_SUPERFIELD, // Not needed if "SuperField" is in the "UStruct" class!
	UENUM_NAMES,
	UCONST_VALUE,
	UPROPERTY_DIMENSION,
	UPROPERTY_SIZE,
	UPROPERTY_FLAGS,
	UPROPERTY_OFFSET,
	USTRUCT_SUPERFIELD, // Not needed if "SuperField" is in the "UField" class!
	USTRUCT_CHILDREN,
	USTRUCT_SIZE,
	UFUNCTION_FLAGS,
	UFUNCTION_NATIVE,
	USTRUCTPROPERTY_STRUCT,
	UOBJECTPROPERTY_PROPERTY,
	UCLASSPROPERTY_METACLASS,
	UMAPPROPERTY_KEY,
	UMAPPROPERTY_VALUE,
	UINTERFACEPROPERTY_CLASS,
	UDELEGATEPROPERTY_FUNCTION,
	UDELEGATEPROPERTY_NAME,
	UBYTEPROPERTY_ENUM,
	UBOOLPROPERTY_BITMASK,
	UARRAYPROPERTY_INNTER
};

/*
# ========================================================================================= #
# Members
# ========================================================================================= #
*/

class Member
{
public:
	EMemberTypes Type;		// Internal id used to tell which member this class is.
	std::string Label;		// String label used for printing the member inside the class.
	uintptr_t Offset;		// Member offset from the base of the class.
	size_t Size;			// Size of the member at the given offset.

public:
	Member();
	Member(EMemberTypes type, size_t size);
	Member(const Member& member);
	~Member();

public: // Global Utils
	static std::string GetLabel(EMemberTypes type);		// Returns the string version of the member used for printing in the generated sdk.
	static uintptr_t GetOffset(EMemberTypes type);		// Returns the members offset in its defined class.
	static size_t GetClassSize(EClassTypes type);		// Returns the "sizeof" for the given class type.
	static size_t GetClassOffset(EClassTypes type);		// Returns the start offset of the given type, taking into account its inherited classes.

public:
	static void Register(EMemberTypes type, size_t size);					// This should only be called by the "REGISTER_MEMBER" macro!
	static std::map<uintptr_t, Member> GetRegistered(EClassTypes type);		// Returns registered members for the given class type, ordered by their offsets.

private:
	static inline std::unordered_map<EMemberTypes, Member> RegisteredMembers;
};

#define REGISTER_MEMBER(memberVariable, memberName, memberType) static void Register_##memberName(){ Member::Register(memberType, sizeof(memberVariable)); }

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/