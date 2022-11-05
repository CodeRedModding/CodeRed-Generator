#include "GameDefines.hpp"

/*
# ========================================================================================= #
# Initialize Globals
# ========================================================================================= #
*/

TArray<class UObject*>* GObjects{};
TArray<struct FNameEntry*>* GNames{};

/*
# ========================================================================================= #
# Class Functions
# ========================================================================================= #
*/

TArray<class UObject*>* UObject::GObjObjects()
{
	TArray<UObject*>* recastedArray = reinterpret_cast<TArray<UObject*>*>(GObjects);
	return recastedArray;
}

std::string UObject::GetName()
{
	return this->Name.ToString();
}

std::string UObject::GetNameCPP()
{
	std::string nameCPP;

	if (this->IsA<UClass>())
	{
		UClass* uClass = static_cast<UClass*>(this);

		while (uClass)
		{
			std::string className = uClass->GetName();

			if (className == "Actor")
			{
				nameCPP += "A";
				break;
			}
			else if (className == "Object")
			{
				nameCPP += "U";
				break;
			}

			uClass = static_cast<UClass*>(uClass->SuperField);
		}
	}
	else
	{
		nameCPP += "F";
	}

	nameCPP += this->GetName();

	return nameCPP;
}

std::string UObject::GetFullName()
{
	std::string fullName = this->GetName();

	for (UObject* uOuter = this->Outer; uOuter; uOuter = uOuter->Outer)
	{
		fullName = (uOuter->GetName() + "." + fullName);
	}

	fullName = this->Class->GetName() + " " + fullName;

	return fullName;
}

std::string UObject::GetPackageName()
{
	UObject* uPackageObj = this->GetPackageObj();

	if (uPackageObj)
	{
		static std::string packageName = uPackageObj->GetName();

		return packageName;
	}

	return "null";
}

UObject* UObject::GetPackageObj()
{
	UObject* uPackage = nullptr;

	for (UObject* uOuter = this->Outer; uOuter; uOuter = uOuter->Outer)
	{
		uPackage = uOuter;
	}

	return uPackage;
}

class UClass* UObject::FindClass(const std::string& classFullName)
{
	static bool initialized = false;
	static std::map<std::string, UClass*> foundClasses{};

	if (!initialized)
	{
		for (UObject* uObject : *UObject::GObjObjects())
		{
			if (uObject)
			{
				std::string objectFullName = uObject->GetFullName();

				if (objectFullName.find("Class") == 0)
				{
					foundClasses[objectFullName] = static_cast<UClass*>(uObject);
				}
			}
		}

		initialized = true;
	}

	if (foundClasses.find(classFullName) != foundClasses.end())
	{
		return foundClasses[classFullName];
	}

	return nullptr;
}

bool UObject::IsA(class UClass* uClass)
{
	for (UClass* uSuperClass = this->Class; uSuperClass; uSuperClass = static_cast<UClass*>(uSuperClass->SuperField))
	{
		if (uSuperClass == uClass)
		{
			return true;
		}
	}

	return false;
}

bool UObject::IsA(int32_t objInternalInteger)
{
	UClass* uClass = UObject::GObjObjects()->at(objInternalInteger)->Class;

	if (uClass)
	{
		return this->IsA(uClass);
	}

	return false;
}

class UFunction* UFunction::FindFunction(const std::string& functionFullName)
{
	static bool initialized = false;
	static std::map<std::string, UFunction*> foundFunctions{};

	if (!initialized)
	{
		for (UObject* uObject : *UObject::GObjObjects())
		{
			if (uObject)
			{
				std::string objectFullName = uObject->GetFullName();

				if (objectFullName.find("Function") == 0)
				{
					foundFunctions[objectFullName] = static_cast<UFunction*>(uObject);
				}
			}
		}

		initialized = true;
	}

	if (foundFunctions.find(functionFullName) != foundFunctions.end())
	{
		return foundFunctions[functionFullName];
	}

	return nullptr;
}

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/