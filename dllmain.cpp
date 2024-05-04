#include "dllmain.hpp"

static constexpr uint32_t LOG_FILE_SPACING = 75;
static constexpr uint32_t CONST_VALUE_SPACING = 175;
static constexpr uint32_t INSTANCE_DUMP_SPACING = 50;

UnrealObject::UnrealObject() : Type(EClassTypes::Unknown), Object(nullptr), Package(nullptr) {}

UnrealObject::UnrealObject(class UObject* uObject, bool bIsPackage) : Type(EClassTypes::Unknown), Object(nullptr), Package(nullptr)
{
    Assign(uObject, bIsPackage);
}

UnrealObject::UnrealObject(const UnrealObject& unrealObj) :
    Type(unrealObj.Type),
    Object(unrealObj.Object),
    Package(unrealObj.Package),
    FullName(unrealObj.FullName),
    ValidName(unrealObj.ValidName)
{

}

UnrealObject::~UnrealObject() {}

bool UnrealObject::IsValid() const
{
    return ((Type != EClassTypes::Unknown) && Object && Package);
}

std::string UnrealObject::Hash() const
{
    if (IsValid())
    {
        return (std::to_string(static_cast<int32_t>(Type)) + "." + FullName + "." + ValidName);
    }

    return "";
}

void UnrealObject::Assign(class UObject* uObject, bool bIsPackage)
{
    if (uObject)
    {
        UObject* packageObj = (bIsPackage ? uObject : uObject->GetPackageObj());

        if (packageObj)
        {
            Object = uObject;
            Package = packageObj;
            FullName = Utils::CreateValidName(Object->GetFullName());
            ValidName = Utils::CreateValidName(Object->GetName());

            if (!bIsPackage)
            {
                AssignType();
            }
            else
            {
                Type = EClassTypes::UObject;
            }
        }
    }
}

void UnrealObject::AssignType()
{
    if (Object)
    {
        if (Object->IsA<UConst>())
        {
            ValidName = ConstGenerator::GenerateConstName(static_cast<UConst*>(Object));

            if (!ValidName.empty())
            {
                Type = EClassTypes::UConst;
            }
        }
        else if (Object->IsA<UEnum>())
        {
            ValidName = EnumGenerator::GenerateEnumName(static_cast<UEnum*>(Object));

            if (!ValidName.empty())
            {
                Type = EClassTypes::UEnum;
            }
        }
        else if (Object->IsA<UScriptStruct>())
        {
            if ((ValidName.find("Default__") == std::string::npos) && (ValidName.find("<uninitialized>") == std::string::npos))
            {
                Type = EClassTypes::UStruct;
            }
        }
        else if (Object->IsA<UClass>())
        {
            if ((ValidName.find("Default__") == std::string::npos) && (ValidName.find("<uninitialized>") == std::string::npos))
            {
                Type = EClassTypes::UClass;
            }
        }
        else if (Object->IsA<UFunction>())
        {
            Type = EClassTypes::UFunction;
        }
    }
}

bool UnrealObject::operator>(const UnrealObject& unrealObj)
{
    return (Hash() > unrealObj.Hash());
}

bool UnrealObject::operator<(const UnrealObject& unrealObj)
{
    return (Hash() < unrealObj.Hash());
}

bool UnrealObject::operator==(const UnrealObject& unrealObj)
{
    return (Hash() == unrealObj.Hash());
}

bool UnrealObject::operator!=(const UnrealObject& unrealObj)
{
    return (Hash() != unrealObj.Hash());
}

UnrealObject& UnrealObject::operator=(const UnrealObject& unrealObj)
{
    Type = unrealObj.Type;
    Object = unrealObj.Object;
    Package = unrealObj.Package;
    FullName = unrealObj.FullName;
    ValidName = unrealObj.ValidName;
    return *this;
}

UnrealProperty::UnrealProperty() : Type(EPropertyTypes::Unknown), Property(nullptr) {}

UnrealProperty::UnrealProperty(class UProperty* uProperty) : Type(EPropertyTypes::Unknown), Property(nullptr)
{
    Assign(uProperty);
}

UnrealProperty::UnrealProperty(const UnrealProperty& unrealProp) : Type(unrealProp.Type), Property(unrealProp.Property), ValidName(unrealProp.ValidName) {}

UnrealProperty::~UnrealProperty() {}

bool UnrealProperty::IsValid() const
{
    return ((Type != EPropertyTypes::Unknown) && Property && (Property->ElementSize > 0));
}

std::string UnrealProperty::Hash() const
{
    if (IsValid())
    {
        return (std::to_string(static_cast<int32_t>(Type)) + "." + std::to_string(reinterpret_cast<uintptr_t>(Property)) + "." + ValidName);
    }

    return "";
}

bool UnrealProperty::IsContainer() const
{
    if (IsValid())
    {
        return ((Type == EPropertyTypes::FName)
            || (Type == EPropertyTypes::FString)
            || (Type == EPropertyTypes::FScriptDelegate)
            || (Type == EPropertyTypes::FStruct)
            || (Type == EPropertyTypes::TArray)
            || (Type == EPropertyTypes::TMap));
    }

    return false;
}

bool UnrealProperty::IsParameter() const
{
    if (IsValid())
    {
        return (Property->PropertyFlags & EPropertyFlags::CPF_Parm);
    }

    return false;
}

bool UnrealProperty::IsReturnParameter() const
{
    if (IsValid())
    {
        return (IsParameter() && (Property->PropertyFlags & EPropertyFlags::CPF_ReturnParm));
    }

    return false;
}

bool UnrealProperty::IsOutParameter() const
{
    if (IsValid())
    {
        return (IsParameter() && (Property->PropertyFlags & EPropertyFlags::CPF_OutParm));
    }

    return false;
}

bool UnrealProperty::IsOptionalParameter() const
{
    if (IsValid())
    {
        return (IsParameter() && (Property->PropertyFlags & EPropertyFlags::CPF_OptionalParm));
    }

    return false;
}

bool UnrealProperty::IsAnArray() const
{
    if (IsValid())
    {
        return (Property->ArrayDim > 1);
    }

    return false;
}

bool UnrealProperty::CantConst() const
{
    if (IsValid())
    {
        return (IsReturnParameter()
            || IsOutParameter()
            || ((Type != EPropertyTypes::FName)
                && (Type != EPropertyTypes::FString)
                && (Type != EPropertyTypes::FScriptDelegate)
                && (Type != EPropertyTypes::FStruct)
                && (Type != EPropertyTypes::TArray)
                && (Type != EPropertyTypes::TMap)));
    }

    return false;
}

bool UnrealProperty::CantReference() const
{
    if (IsValid())
    {
        return IsAnArray();
    }

    return false;
}

bool UnrealProperty::CantMemcpy() const
{
    if (IsValid())
    {
        return ((Type == EPropertyTypes::Bool)
            || (!IsAnArray()
                && ((Type == EPropertyTypes::UClass)
                || (Type == EPropertyTypes::UObject)
                || (Type == EPropertyTypes::UInterface))));
    }

    return false;
}

size_t UnrealProperty::GetSize() const
{
    if (IsValid())
    {
        if (Type == EPropertyTypes::Int32)
        {
            return sizeof(int32_t);
        }
        else if (Type == EPropertyTypes::UInt8)
        {
            return Property->ElementSize;
        }
        else if (Type == EPropertyTypes::UInt32)
        {
            return sizeof(uint32_t);
        }
        else if (Type == EPropertyTypes::UInt64)
        {
            return sizeof(uint64_t);
        }
        else if (Type == EPropertyTypes::Double)
        {
            return sizeof(double);
        }
        else if (Type == EPropertyTypes::Float)
        {
            return sizeof(float);
        }
        else if (Type == EPropertyTypes::Bool)
        {
            return Property->ElementSize;
        }
        else if (Type == EPropertyTypes::FName)
        {
            return sizeof(FName);
        }
        else if (Type == EPropertyTypes::FString)
        {
            return sizeof(FString);
        }
        else if (Type == EPropertyTypes::FScriptDelegate)
        {
            return sizeof(FScriptDelegate);
        }
        else if (Type == EPropertyTypes::FStruct)
        {
            return Property->ElementSize;
        }
        else if (Type == EPropertyTypes::UClass)
        {
            return sizeof(uintptr_t);
        }
        else if (Type == EPropertyTypes::UObject)
        {
            return sizeof(uintptr_t);
        }
        else if (Type == EPropertyTypes::UInterface)
        {
            return sizeof(uintptr_t);
        }
        else if (Type == EPropertyTypes::TArray)
        {
            return sizeof(TArray<uintptr_t>);
        }
        else if (Type == EPropertyTypes::TMap)
        {
            return Property->ElementSize;
        }
    }

    return 0;
}

std::string UnrealProperty::GetType(bool bIgnoreEnum, bool bFunctionParam, bool bIgnoreConst) const
{
    std::string typeStr;

    if (IsValid())
    {
        if (Type == EPropertyTypes::Int32)
        {
            typeStr = "int32_t";
        }
        else if (Type == EPropertyTypes::UInt8)
        {
            if (!bIgnoreEnum && GConfig::UsingEnumClasses())
            {
                UByteProperty* byteProperty = static_cast<UByteProperty*>(Property);

                if (byteProperty && byteProperty->Enum)
                {
                    typeStr = EnumGenerator::GenerateEnumName(byteProperty->Enum);
                }
            }

            if (typeStr.empty())
            {
                typeStr = "uint8_t";
            }
        }
        else if (Type == EPropertyTypes::UInt32)
        {
            typeStr = "uint32_t";
        }
        else if (Type == EPropertyTypes::UInt64)
        {
            typeStr = "uint64_t";
        }
        else if (Type == EPropertyTypes::Double)
        {
            typeStr = "double";
        }
        else if (Type == EPropertyTypes::Float)
        {
            typeStr = "float";
        }
        else if (Type == EPropertyTypes::Bool)
        {
            if (bFunctionParam || IsReturnParameter())
            {
                typeStr = "bool";
            }
            else
            {
                typeStr = "uint32_t";
            }
        }
        else if (Type == EPropertyTypes::FName)
        {
            typeStr = "class FName";
        }
        else if (Type == EPropertyTypes::FString)
        {
            typeStr = "class FString";
        }
        else if (Type == EPropertyTypes::FScriptDelegate)
        {
            typeStr = "struct FScriptDelegate";
        }
        else if (Type == EPropertyTypes::FStruct)
        {
            UStructProperty* structProperty = static_cast<UStructProperty*>(Property);

            if (structProperty && structProperty->Struct)
            {
                uint32_t propertyCount = GCache::CountObject<UScriptStruct>(structProperty->Struct->GetName());

                if (propertyCount > 1)
                {
                    if (structProperty->Struct->Outer)
                    {
                        typeStr = ("struct " + Utils::CreateValidName(structProperty->Struct->Outer->GetNameCPP()) + "_" + Utils::CreateValidName(structProperty->Struct->GetNameCPP()));
                    }
                    else
                    {
                        typeStr = ("struct " + Utils::CreateValidName(structProperty->Struct->GetNameCPP()) + "_Outer" + Printer::Decimal(propertyCount, EWidthTypes::Byte));
                    }
                }
                else
                {
                    typeStr = ("struct " + Utils::CreateValidName(structProperty->Struct->GetNameCPP()));
                }
            }
        }
        else if (Type == EPropertyTypes::UClass)
        {
            UClassProperty* classProperty = static_cast<UClassProperty*>(Property);

            if (classProperty && classProperty->MetaClass)
            {
                typeStr = ("class " + Utils::CreateValidName(classProperty->MetaClass->GetNameCPP()) + "*");
            }
        }
        else if (Type == EPropertyTypes::UObject)
        {
            UObjectProperty* objectProperty = static_cast<UObjectProperty*>(Property);

            if (objectProperty && objectProperty->PropertyClass)
            {
                typeStr = ("class " + Utils::CreateValidName(objectProperty->PropertyClass->GetNameCPP()) + "*");
            }
        }
        else if (Type == EPropertyTypes::UInterface)
        {
            UInterfaceProperty* interfaceProperty = static_cast<UInterfaceProperty*>(Property);

            if (interfaceProperty && interfaceProperty->InterfaceClass)
            {
                typeStr = ("class " + Utils::CreateValidName(interfaceProperty->InterfaceClass->GetNameCPP()) + "*");
            }
        }
        else if (Type == EPropertyTypes::TArray)
        {
            UArrayProperty* arrayProperty = static_cast<UArrayProperty*>(Property);

            if (arrayProperty && arrayProperty->Inner)
            {
                UnrealProperty innerProperty(arrayProperty->Inner);

                if (innerProperty.IsValid())
                {
                    typeStr = ("class TArray<" + innerProperty.GetType((bIgnoreEnum || IsReturnParameter()), false, true) + ">");
                }
            }
        }
        else if (Type == EPropertyTypes::TMap)
        {
            UMapProperty* mapProperty = static_cast<UMapProperty*>(Property);

            if (mapProperty && mapProperty->Key && mapProperty->Value)
            {
                UnrealProperty keyProperty(mapProperty->Key);
                UnrealProperty valueProperty(mapProperty->Value);

                if (keyProperty.IsValid() && valueProperty.IsValid())
                {
                    typeStr = ("class TMap<" + keyProperty.GetType((bIgnoreEnum || IsReturnParameter()), false, true) + ", " + valueProperty.GetType((bIgnoreEnum || IsReturnParameter()), false, true) + ">");
                }
            }
        }
    }

    if (!typeStr.empty() && bFunctionParam && !bIgnoreConst && !CantReference() && !CantConst())
    {
        typeStr = ("const " + typeStr + "&");
    }

    return typeStr;
}

std::string UnrealProperty::GetTypeForClass() const
{
    return GetType(false, false, false);
}

std::string UnrealProperty::GetTypeForStruct() const
{
    return GetType(true, false, false);
}

std::string UnrealProperty::GetTypeForParameter(bool bIgnoreConst) const
{
    return GetType(false, true, bIgnoreConst);
}

bool UnrealProperty::Assign(class UProperty* uProperty)
{
    if (uProperty)
    {
        Property = uProperty;
        ValidName = Utils::CreateValidName(uProperty->GetName());
        return AssignType();
    }

    return false;
}

bool UnrealProperty::AssignType()
{
    if (Property)
    {
        if (Property->IsA<UStructProperty>())
        {
            UStructProperty* structProperty = static_cast<UStructProperty*>(Property);

            if (structProperty && structProperty->Struct)
            {
                Type = EPropertyTypes::FStruct;
            }
        }
        else if (Property->IsA<UStrProperty>())
        {
            Type = EPropertyTypes::FString;
        }
        else if (Property->IsA<UQWordProperty>())
        {
            Type = EPropertyTypes::UInt64;
        }
        else if (Property->IsA<UClassProperty>())
        {
            UClassProperty* classProperty = static_cast<UClassProperty*>(Property);

            if (classProperty && classProperty->MetaClass)
            {
                Type = EPropertyTypes::UClass;
            }
        }
        else if (Property->IsA<UObjectProperty>())
        {
            UObjectProperty* objectProperty = static_cast<UObjectProperty*>(Property);

            if (objectProperty && objectProperty->PropertyClass)
            {
                Type = EPropertyTypes::UObject;
            }
        }
        else if (Property->IsA<UInterfaceProperty>())
        {
            UInterfaceProperty* interfaceProperty = static_cast<UInterfaceProperty*>(Property);

            if (interfaceProperty && interfaceProperty->InterfaceClass)
            {
                Type = EPropertyTypes::UInterface;
            }
        }
        else if (Property->IsA<UNameProperty>())
        {
            Type = EPropertyTypes::FName;
        }
        else if (Property->IsA<UMapProperty>())
        {
            UMapProperty* mapProperty = static_cast<UMapProperty*>(Property);
            std::string mapKey, mapValue;

            if (mapProperty && mapProperty->Key && mapProperty->Value)
            {
                if (UnrealProperty(mapProperty->Key).IsValid() && UnrealProperty(mapProperty->Value).IsValid())
                {
                    Type = EPropertyTypes::TMap;
                }
            }
        }
        else if (Property->IsA<UIntProperty>())
        {
            Type = EPropertyTypes::Int32;
        }
        else if (Property->IsA<UFloatProperty>())
        {
            Type = EPropertyTypes::Float;
        }
        else if (Property->IsA<UDelegateProperty>())
        {
            Type = EPropertyTypes::FScriptDelegate;
        }
        else if (Property->IsA<UByteProperty>())
        {
            Type = EPropertyTypes::UInt8;
        }
        else if (Property->IsA<UBoolProperty>())
        {
            Type = EPropertyTypes::Bool;
        }
        else if (Property->IsA<UArrayProperty>())
        {
            UArrayProperty* arrayProperty = static_cast<UArrayProperty*>(Property);

            if (arrayProperty && arrayProperty->Inner)
            {
                UnrealProperty innerProperty(arrayProperty->Inner);

                if (innerProperty.IsValid())
                {
                    Type = EPropertyTypes::TArray;
                }
            }
        }
    }

    return (Type != EPropertyTypes::Unknown);
}

bool UnrealProperty::operator>(const UnrealProperty& unrealProp)
{
    return (Hash() > unrealProp.Hash());
}

bool UnrealProperty::operator<(const UnrealProperty& unrealProp)
{
    return (Hash() < unrealProp.Hash());
}

bool UnrealProperty::operator==(const UnrealProperty& unrealProp)
{
    return (Hash() == unrealProp.Hash());
}

bool UnrealProperty::operator!=(const UnrealProperty& unrealProp)
{
    return (Hash() != unrealProp.Hash());
}

UnrealProperty& UnrealProperty::operator=(const UnrealProperty& unrealProp)
{
    Type = unrealProp.Type;
    Property = unrealProp.Property;
    ValidName = unrealProp.ValidName;
    return *this;
}

void GCache::Initialize()
{
    if (m_packages.empty() && Generator::AreGlobalsValid())
    {
        for (int32_t i = 0; i < (UObject::GObjObjects()->size() - 1); i++)
        {
            UObject* uObject = UObject::GObjObjects()->at(i);

            if (uObject)
            {
                UnrealObject unrealObj(uObject);

                if (unrealObj.IsValid())
                {
                    CacheObject(unrealObj);
                }
            }
        }
    }
}

void GCache::ClearCache()
{
    m_objects.clear();
    m_consts.clear();
    m_enums.clear();
    m_structs.clear();
    m_classes.clear();
    m_constants.clear();
    m_packages.clear();
}

std::vector<UnrealObject>* GCache::GetCache(class UObject* packageObj, EClassTypes type)
{
    if (type == EClassTypes::UConst)
    {
        return &m_consts[packageObj];
    }
    else if (type == EClassTypes::UEnum)
    {
        return &m_enums[packageObj];
    }
    else if (type == EClassTypes::UStruct)
    {
        return &m_structs[packageObj];
    }
    else if (type == EClassTypes::UClass)
    {
        return &m_classes[packageObj];
    }

    return nullptr;
}

std::map<std::string, class UObject*>* GCache::GetConstants()
{
    return &m_constants;
}

std::vector<UnrealObject>* GCache::GetPackages()
{
    return &m_packages;
}

std::pair<std::string, class UObject*> GCache::GetConstant(const UnrealObject& unrealObj)
{
    if (GConfig::UsingConstants() && unrealObj.IsValid())
    {
        for (const auto& constantPair : m_constants)
        {
            if (constantPair.second == unrealObj.Object)
            {
                return constantPair;
            }
        }
    }

    return { "", nullptr };
}

UnrealObject GCache::GetLargestStruct(const std::string& structFullName)
{
    size_t propertySize = 0;
    UnrealObject largestStruct;

    for (auto& classPair : m_structs)
    {
        for (UnrealObject& unrealObj : classPair.second)
        {
            if (unrealObj.FullName == structFullName)
            {
                UScriptStruct* scriptStruct = static_cast<UScriptStruct*>(unrealObj.Object);

                if (scriptStruct->PropertySize >= propertySize)
                {
                    largestStruct = unrealObj;
                    propertySize = scriptStruct->PropertySize;
                }
            }
        }
    }

    return largestStruct;
}

UnrealObject GCache::GetClass(class UClass* staticClass)
{
    for (auto& classPair : m_classes)
    {
        for (UnrealObject& unrealObj : classPair.second)
        {
            if (unrealObj.Object == staticClass)
            {
                return unrealObj.Object;
            }
        }
    }

    return UnrealObject();
}

void GCache::CacheObject(UnrealObject& unrealObj)
{
    if (unrealObj.IsValid())
    {
        std::vector<UnrealObject>* cache = GetCache(unrealObj.Package, unrealObj.Type);

        if (cache)
        {
            if (std::find(cache->begin(), cache->end(), unrealObj) == cache->end())
            {
                cache->push_back(unrealObj);
            }

            UnrealObject packageObj(unrealObj.Package, true);

            if (std::find(m_packages.begin(), m_packages.end(), packageObj) == m_packages.end())
            {
                m_packages.push_back(packageObj);
            }
        }

        CacheConstant(unrealObj);
        CacheCount(unrealObj);
    }
}

void GCache::CacheConstant(UnrealObject& unrealObj)
{
    if (GConfig::UsingConstants() && unrealObj.IsValid())
    {
        if ((unrealObj.Type == EClassTypes::UClass) || (unrealObj.Type == EClassTypes::UFunction))
        {
            std::string constantName = ("IDX_" + Printer::ToUpper(unrealObj.FullName));
            Printer::ReplaceChars(constantName, '.', '_');
            Printer::ReplaceChars(constantName, ' ', '_');

            if (!m_constants.contains(constantName))
            {
                m_constants[constantName] = unrealObj.Object;
            }
        }
    }
}

void GCache::CacheCount(UnrealObject& unrealObj)
{
    if (unrealObj.IsValid())
    {
        std::pair<std::string, class UClass*> objectPair{ unrealObj.ValidName, unrealObj.Object->Class };

        if (!m_objects.contains(objectPair))
        {
            m_objects[objectPair] = 0;
        }

        m_objects[objectPair]++;
    }
}

bool GLogger::Open()
{
#ifndef NO_LOGGING
    if (!m_file.is_open() && GConfig::HasOutputPath())
    {
        std::filesystem::path fullDirectory = (GConfig::GetOutputPath() / GConfig::GetGameNameShort());
        std::filesystem::create_directory(GConfig::GetOutputPath());
        std::filesystem::create_directory(fullDirectory);

        if (std::filesystem::exists(fullDirectory))
        {
            m_file.open(fullDirectory / (GEngine::GetName() + ".log"));
            return true;
        }
        else
        {
            Utils::MessageboxError("Error: Failed to create the log file, might not have the right permissions or your directory is invalid!");
        }
    }
#endif

    return false;
}

void GLogger::Close()
{
#ifndef NO_LOGGING
    if (m_file.is_open())
    {
        m_file.close();
    }
#endif
}

void GLogger::Flush()
{
#ifndef NO_LOGGING
    if (m_file.is_open())
    {
        m_file.flush();
    }
#endif
}

void GLogger::Log(const std::string& str, bool bFlush)
{
#ifndef NO_LOGGING
    if (m_file.is_open() && !str.empty())
    {
        m_file << str << "\n";

        if (bFlush)
        {
            Flush();
        }
    }
#endif
}

void GLogger::LogObject(const std::string& title, const UnrealObject& unrealObj)
{
#ifndef NO_LOGGING
    if (m_file.is_open() && !title.empty() && unrealObj.IsValid())
    {
        m_file << title;
        Printer::FillRight(m_file, ' ', unrealObj.ValidName.length());
        m_file << unrealObj.ValidName;

        if (unrealObj.ValidName.length() < LOG_FILE_SPACING)
        {
            Printer::FillRight(m_file, ' ', (LOG_FILE_SPACING - (unrealObj.ValidName.length() + title.length())));
        }

        m_file << " - Instance: " << Printer::Hex(unrealObj.Object) << std::endl;
    }
#endif
}

void GLogger::LogClassSize(class UClass* uClass, size_t localSize)
{
#ifndef NO_LOGGING
    if (m_file.is_open() && uClass)
    {
        m_file << "Error: Incorrect class size detected for \"" << uClass->GetFullName() << "\"!\n";
        m_file << "Error: Reported size " << localSize << "\n";
        m_file << "Error: Actual size " << uClass->PropertySize << std::endl;
    }
#endif
}

void GLogger::LogStructPadding(class UScriptStruct* uScriptStruct, size_t padding)
{
#ifndef NO_LOGGING
    if (m_file.is_open() && uScriptStruct)
    {
        m_file << "Info: Extra padding detected!";
        m_file << "Info: Property size " << Printer::Hex(uScriptStruct->PropertySize, 1) << "\n";
        m_file << "Info: Min alignment " << Printer::Hex(uScriptStruct->MinAlignment, 1) << "\n";
        m_file << "Info: Extra padding " << Printer::Hex(padding, 1) << std::endl;
    }
#endif
}

namespace Utils
{
    void MessageboxExt(const std::string& message, uint32_t flags)
    {
#ifdef _WIN32
        MessageBoxA(NULL, message.c_str(), GEngine::GetName().c_str(), flags);
#endif
    }

    void MessageboxInfo(const std::string& message)
    {
#ifdef _WIN32
        MessageboxExt(message, (MB_OK | MB_ICONINFORMATION));
#endif
    }

    void MessageboxWarn(const std::string& message)
    {
#ifdef _WIN32
        MessageboxExt(message, (MB_OK | MB_ICONWARNING));
#endif
    }

    void MessageboxError(const std::string& message)
    {
#ifdef _WIN32
        MessageboxExt(message, (MB_OK | MB_ICONERROR));
#endif
    }

    bool SortProperty(const UnrealProperty& unrealPropA, const UnrealProperty& unrealPropB)
    {
        if (unrealPropA.Property && unrealPropB.Property)
        {
            if ((unrealPropA.Property->Offset == unrealPropB.Property->Offset)
                && (unrealPropA.Type == EPropertyTypes::Bool)
                && (unrealPropB.Type == EPropertyTypes::Bool))
            {
                return (static_cast<UBoolProperty*>(unrealPropA.Property)->BitMask < static_cast<UBoolProperty*>(unrealPropB.Property)->BitMask);
            }

            return (unrealPropA.Property->Offset < unrealPropB.Property->Offset);
        }

        return false;
    }

    bool SortPropertyPair(const std::pair<UnrealProperty, std::string>& pairA, const std::pair<UnrealProperty, std::string>& pairB)
    {
        return SortProperty(pairA.first.Property, pairB.first.Property);
    }

    void CreateWindowsName(std::string& functionName)
    {
        if (GConfig::UsingWindows())
        {
            static std::vector<std::string> unsafeNames = {
                "GetCurrentTime",
                "GetObject",
                "DeleteFile",
                "DrawText",
                "SendMessage",
                "GetMessage"
            };

            for (const std::string& name : unsafeNames)
            {
                if (functionName.find(name) != std::string::npos)
                {
                    functionName += "W";
                    break;
                }
            }
        }
    }

    std::string CreateValidName(std::string name)
    {
        if (!name.empty())
        {
            for (char& c : name)
            {
                if ((c == '`')
                    || (c == '!')
                    || (c == '@')
                    || (c == '#')
                    || (c == '$')
                    || (c == '%')
                    || (c == '^')
                    || (c == '&')
                    || (c == '*')
                    || (c == '(')
                    || (c == ')')
                    || (c == '-')
                    || (c == '=')
                    || (c == '+')
                    || (c == '[')
                    || (c == '{')
                    || (c == ']')
                    || (c == '}')
                    || (c == '\\')
                    || (c == '|')
                    || (c == ';')
                    || (c == ':')
                    || (c == '\'')
                    || (c == '"')
                    || (c == ',')
                    || (c == '/')
                    || (c == '?'))
                {
                    c = '_';
                }
            }

            if (name == "_")
            {
                name = "instance";
            }
        }

        return name;
    }
}

namespace Retrievers
{
    void GetAllPropertyFlags(std::ostringstream& stream, uint64_t propertyFlags)
    {
        bool first = true;

        if (propertyFlags & EPropertyFlags::CPF_Edit) { stream << (first ? "(" : " | ") << "CPF_Edit";							                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Const) { stream << (first ? "(" : " | ") << "CPF_Const";						                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Input) { stream << (first ? "(" : " | ") << "CPF_Input";						                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ExportObject) { stream << (first ? "(" : " | ") << "CPF_ExportObject";			                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_OptionalParm) { stream << (first ? "(" : " | ") << "CPF_OptionalParm";			                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Net) { stream << (first ? "(" : " | ") << "CPF_Net";							                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditFixedSize) { stream << (first ? "(" : " | ") << "CPF_EditFixedSize";		                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Parm) { stream << (first ? "(" : " | ") << "CPF_Parm";							                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_OutParm) { stream << (first ? "(" : " | ") << "CPF_OutParm";					                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_SkipParm) { stream << (first ? "(" : " | ") << "CPF_SkipParm";					                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ReturnParm) { stream << (first ? "(" : " | ") << "CPF_ReturnParm";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_CoerceParm) { stream << (first ? "(" : " | ") << "CPF_CoerceParm";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Native) { stream << (first ? "(" : " | ") << "CPF_Native";						                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Transient) { stream << (first ? "(" : " | ") << "CPF_Transient";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Config) { stream << (first ? "(" : " | ") << "CPF_Config";						                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Localized) { stream << (first ? "(" : " | ") << "CPF_Localized";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Travel) { stream << (first ? "(" : " | ") << "CPF_Travel";						                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditConst) { stream << (first ? "(" : " | ") << "CPF_EditConst";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_GlobalConfig) { stream << (first ? "(" : " | ") << "CPF_GlobalConfig";			                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Component) { stream << (first ? "(" : " | ") << "CPF_Component";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NeedCtorLink) { stream << (first ? "(" : " | ") << "CPF_NeedCtorLink";			                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NoExport) { stream << (first ? "(" : " | ") << "CPF_NoExport";					                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NoClear) { stream << (first ? "(" : " | ") << "CPF_NoClear";					                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditInline) { stream << (first ? "(" : " | ") << "CPF_EditInline";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditInlineUse) { stream << (first ? "(" : " | ") << "CPF_EditInlineUse";				        first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditFindable) { stream << (first ? "(" : " | ") << "CPF_EditFindable";				            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditInlineUse) { stream << (first ? "(" : " | ") << "CPF_EditInlineUse";		                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Deprecated) { stream << (first ? " " : " | ") << "CPF_Deprecated";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_DataBinding) { stream << (first ? "(" : " | ") << "CPF_DataBinding";	                        first = false; }
        if (propertyFlags & EPropertyFlags::CPF_SerializeText) { stream << (first ? "(" : " | ") << "CPF_SerializeText";	                    first = false; }
        if (propertyFlags & EPropertyFlags::CPF_RepNotify) { stream << (first ? "(" : " | ") << "CPF_RepNotify";				                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Interp) { stream << (first ? "(" : " | ") << "CPF_Interp";						                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NonTransactional) { stream << (first ? "(" : " | ") << "CPF_NonTransactional";	                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditorOnly) { stream << (first ? "(" : " | ") << "CPF_EditorOnly";	                            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NotForConsole) { stream << (first ? "(" : " | ") << "CPF_NotForConsole";	                    first = false; }
        if (propertyFlags & EPropertyFlags::CPF_RepRetry) { stream << (first ? "(" : " | ") << "CPF_RepRetry";	                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_PrivateWrite) { stream << (first ? "(" : " | ") << "CPF_PrivateWrite";	                        first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ProtectedWrite) { stream << (first ? "(" : " | ") << "CPF_ProtectedWrite";	                    first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ArchetypeProperty) { stream << (first ? "(" : " | ") << "CPF_ArchetypeProperty";	            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditHide) { stream << (first ? "(" : " | ") << "CPF_EditHide";	                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditTextBox) { stream << (first ? "(" : " | ") << "CPF_EditTextBox";	                        first = false; }
        if (propertyFlags & EPropertyFlags::CPF_CrossLevelPassive) { stream << (first ? "(" : " | ") << "CPF_CrossLevelPassive";	            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_CrossLevelActive) { stream << (first ? "(" : " | ") << "CPF_CrossLevelActive";	                first = false; }

        if (!first)
        {
            stream << ")";
        }
    }

    void GetAllFunctionFlags(std::ostringstream& stream, uint64_t functionFlags)
    {
        bool first = true;

        if (functionFlags & EFunctionFlags::FUNC_None) { stream << (first ? "(" : " | ") << "FUNC_None";						                first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Final) { stream << (first ? "(" : " | ") << "FUNC_Final";						                first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Defined) { stream << (first ? "(" : " | ") << "FUNC_Defined";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Iterator) { stream << (first ? "(" : " | ") << "FUNC_Iterator";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Latent) { stream << (first ? "(" : " | ") << "FUNC_Latent";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_PreOperator) { stream << (first ? "(" : " | ") << "FUNC_PreOperator";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Singular) { stream << (first ? "(" : " | ") << "FUNC_Singular";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Net) { stream << (first ? "(" : " | ") << "FUNC_Net";						                    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_NetReliable) { stream << (first ? "(" : " | ") << "FUNC_NetReliable";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Simulated) { stream << (first ? "(" : " | ") << "FUNC_Simulated";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Exec) { stream << (first ? "(" : " | ") << "FUNC_Exec";						                first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Native) { stream << (first ? "(" : " | ") << "FUNC_Native";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Event) { stream << (first ? "(" : " | ") << "FUNC_Event";						                first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Operator) { stream << (first ? "(" : " | ") << "FUNC_Operator";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Static) { stream << (first ? "(" : " | ") << "FUNC_Static";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_NoExport) { stream << (first ? "(" : " | ") << "FUNC_NoExport";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_OptionalParm) { stream << (first ? "(" : " | ") << "FUNC_OptionalParm";						first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Const) { stream << (first ? "(" : " | ") << "FUNC_Const";						                first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Invariant) { stream << (first ? "(" : " | ") << "FUNC_Invariant";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Public) { stream << (first ? "(" : " | ") << "FUNC_Public";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Private) { stream << (first ? "(" : " | ") << "FUNC_Private";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Protected) { stream << (first ? "(" : " | ") << "FUNC_Protected";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Delegate) { stream << (first ? "(" : " | ") << "FUNC_Delegate";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_NetServer) { stream << (first ? "(" : " | ") << "FUNC_NetServer";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_HasOutParms) { stream << (first ? "(" : " | ") << "FUNC_HasOutParms";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_HasDefaults) { stream << (first ? "(" : " | ") << "FUNC_HasDefaults";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_NetClient) { stream << (first ? "(" : " | ") << "FUNC_NetClient";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_DLLImport) { stream << (first ? "(" : " | ") << "FUNC_DLLImport";						        first = false; }
        if (functionFlags & EFunctionFlags::FUNC_K2Call) { stream << (first ? "(" : " | ") << "FUNC_K2Call";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_K2Override) { stream << (first ? "(" : " | ") << "FUNC_K2Override";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_K2Pure) { stream << (first ? "(" : " | ") << "FUNC_K2Pure";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_EditorOnly) { stream << (first ? "(" : " | ") << "FUNC_EditorOnly";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_Lambda) { stream << (first ? "(" : " | ") << "FUNC_Lambda";						            first = false; }
        if (functionFlags & EFunctionFlags::FUNC_NetValidate) { stream << (first ? "(" : " | ") << "FUNC_NetValidate";						    first = false; }
        if (functionFlags & EFunctionFlags::FUNC_AllFlags) { stream << (first ? "(" : " | ") << "FUNC_AllFlags";						        first = false; }

        if (!first)
        {
            stream << ")";
        }
    }

    uintptr_t GetBaseAddress()
    {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
    }

    uintptr_t GetOffset(void* pointer)
    {
        uintptr_t baseAddress = GetBaseAddress();
        uintptr_t address = reinterpret_cast<uintptr_t>(pointer);

        if (address > baseAddress)
        {
            return (address - baseAddress);
        }

        return NULL;
    }

    uintptr_t FindPattern(const uint8_t* pattern, const std::string& mask)
    {
        if (pattern && !mask.empty())
        {
            MODULEINFO miInfos;
            ZeroMemory(&miInfos, sizeof(MODULEINFO));

            HMODULE hModule = GetModuleHandle(NULL);
            K32GetModuleInformation(GetCurrentProcess(), hModule, &miInfos, sizeof(MODULEINFO));

            uintptr_t start = reinterpret_cast<uintptr_t>(hModule);
            uintptr_t end = (start + miInfos.SizeOfImage);

            size_t currentPos = 0;
            size_t maskLength = (mask.length() - 1);

            for (uintptr_t retAddress = start; retAddress < end; retAddress++)
            {
                if (*reinterpret_cast<uint8_t*>(retAddress) == pattern[currentPos] || mask[currentPos] == '?')
                {
                    if (currentPos == maskLength)
                    {
                        return (retAddress - maskLength);
                    }

                    currentPos++;
                }
                else
                {
                    retAddress -= currentPos;
                    currentPos = 0;
                }
            }
        }

        return NULL;
    }
}

namespace ConstGenerator
{
    static std::map<std::string, class UConst*> m_constNames;

    std::string GenerateConstName(class UConst* uConst)
    {
        if (uConst)
        {
            std::string constName = Utils::CreateValidName(uConst->GetName());

            if (constName.find("Default__") == std::string::npos)
            {
                if (uConst->Name.GetInstance())
                {
                    constName += ("_" + std::to_string(uConst->Name.GetInstance()));
                }

                if (constName.find("CONST_") != 0)
                {
                    constName = ("CONST_" + constName);
                }

                if (m_constNames.contains(constName) && (m_constNames[constName] != uConst))
                {
                    uint32_t freeIndex = 0;

                    for (uint32_t i = freeIndex; i < UINT32_MAX; i++)
                    {
                        std::string freeName = (constName + "_" + std::to_string(i));

                        if (!m_constNames.contains(freeName))
                        {
                            constName = freeName;
                            break;
                        }
                    }
                }

                m_constNames[constName] = uConst;
                return constName;
            }
        }

        return "";
    }

    void GenerateConst(std::ofstream& stream, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid())
        {
#ifndef NO_LOGGING
            GLogger::LogObject("Const: ", unrealObj);
#endif

            if (!unrealObj.ValidName.empty())
            {
                stream << "#define " << unrealObj.ValidName;

                if (unrealObj.ValidName.length() < GConfig::GetConstSpacing())
                {
                    Printer::FillLeft(stream, ' ', (GConfig::GetConstSpacing() - unrealObj.ValidName.length()));
                }

                stream << " " << static_cast<UConst*>(unrealObj.Object)->Value.ToString() << "\n";
            }
        }
    }

    void ProcessConsts(std::ofstream& stream, class UObject* packageObj)
    {
        if (packageObj)
        {
            std::vector<UnrealObject>* objCache = GCache::GetCache(packageObj, EClassTypes::UConst);

            if (objCache)
            {
                for (const UnrealObject& unrealObj : *objCache)
                {
                    GenerateConst(stream, unrealObj);
                }
            }
        }
    }
}

namespace EnumGenerator
{
    static std::map<std::string, class UEnum*> m_enumNames;

    std::string GenerateEnumName(class UEnum* uEnum)
    {
        if (uEnum)
        {
            std::string enumName = Utils::CreateValidName(uEnum->GetName());

            if (enumName.find("Default__") == std::string::npos)
            {
                if (uEnum->Name.GetInstance())
                {
                    enumName += ("_" + std::to_string(uEnum->Name.GetInstance()));
                }

                if (enumName.find("E") != 0)
                {
                    enumName = ("E" + enumName);
                }

                if (m_enumNames.contains(enumName) && (m_enumNames[enumName] != uEnum))
                {
                    uint32_t freeIndex = 0;

                    for (uint32_t i = freeIndex; i < UINT32_MAX; i++)
                    {
                        std::string freeName = (enumName + "_" + std::to_string(i));

                        if (!m_enumNames.contains(freeName))
                        {
                            enumName = freeName;
                            break;
                        }
                    }
                }

                m_enumNames[enumName] = uEnum;
                return enumName;
            }
        }

        return "";
    }

    void GenerateEnum(std::ofstream& file, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid())
        {
#ifndef NO_LOGGING
            GLogger::LogObject("Enum: ", unrealObj);
#endif

            std::ostringstream enumStream;
            std::ostringstream propertyStream;
            enumStream << "// " << unrealObj.FullName << "\n";

            if (GConfig::UsingEnumClasses())
            {
                enumStream << "enum class " << unrealObj.ValidName << " : " << GConfig::GetEnumClassType() << "\n";
                enumStream << "{" << "\n";
            }
            else
            {
                enumStream << "enum " << unrealObj.ValidName << "\n";
                enumStream << "{" << "\n";
            }

            UEnum* uEnum = static_cast<UEnum*>(unrealObj.Object);
            std::map<std::string, size_t> enumValues;

            for (int32_t i = 0; i < uEnum->Names.size(); i++)
            {
                std::string propertyName = Utils::CreateValidName(uEnum->Names[i].ToString());
                size_t maxPos = propertyName.find("_MAX");

                if (maxPos != std::string::npos)
                {
                    propertyName.replace(maxPos, 4, "_END");
                }

                if (!GConfig::UsingEnumClasses())
                {
                    propertyName = (unrealObj.ValidName + "_" + propertyName);
                }

                if (enumValues.count(propertyName) == 0)
                {
                    enumValues[propertyName] = 1;
                    propertyStream << propertyName;
                }
                else
                {
                    propertyStream << propertyName << Printer::Decimal(enumValues[propertyName], EWidthTypes::Byte);
                    enumValues[propertyName]++;
                }

                enumStream << "\t";
                Printer::FillLeft(enumStream, ' ', GConfig::GetEnumSpacing());
                enumStream << propertyStream.str() << " = " << i;

                if (i != (uEnum->Names.size() - 1))
                {
                    enumStream << "," << "\n";
                }
                else
                {
                    enumStream << "\n";
                }

                Printer::Empty(propertyStream);
            }

            enumStream << "};\n\n";
            file << enumStream.str();
        }
    }

    void ProcessEnums(std::ofstream& stream, class UObject* packageObj)
    {
        if (packageObj)
        {
            std::vector<UnrealObject>* objCache = GCache::GetCache(packageObj, EClassTypes::UEnum);

            if (objCache)
            {
                for (const UnrealObject& unrealObj : *objCache)
                {
                    GenerateEnum(stream, unrealObj);
                }
            }
        }
    }
}

namespace StructGenerator
{
    static std::map<std::string, int32_t> m_generatedStructs;

    void GenerateStructMembers(std::ofstream& structStream, EClassTypes structType)
    {
        if (structType != EClassTypes::Unknown)
        {
            std::ostringstream propertyStream;

            size_t localSize = 0;
            size_t startOffset = 0;
            size_t missedOffset = 0;
            size_t lastOffset = 0;

            localSize = Member::GetClassSize(structType);
            startOffset = Member::GetClassOffset(structType);
            std::map<size_t, Member*> members = Member::GetRegistered(structType);

            if (!members.empty())
            {
                lastOffset = startOffset;
                uint32_t unknownDataIndex = 0;

                for (const auto& member : members)
                {
                    if ((lastOffset + member.second->Size) < member.second->Offset)
                    {
                        missedOffset = (member.second->Offset - lastOffset);

                        if (missedOffset >= GConfig::GetGameAlignment())
                        {
                            propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                            propertyStream << "[" << Printer::Hex(missedOffset) << "];";

                            structStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                            structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") DYNAMIC FIELD PADDING\n";
                            Printer::Empty(propertyStream);

                            unknownDataIndex++;
                        }
                    }

                    structStream << "\t" << member.second->Label << " // " << Printer::Hex(member.second->Offset, EWidthTypes::Size);
                    structStream << " (" << Printer::Hex(member.second->Size, EWidthTypes::Size) << ")\n";
                    lastOffset = (member.second->Offset + member.second->Size);
                }

                if ((structType != EClassTypes::FNameEntry) && (lastOffset < localSize))
                {
                    missedOffset = (localSize - lastOffset);

                    if (missedOffset >= GConfig::GetGameAlignment())
                    {
                        propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                        propertyStream << "[" << Printer::Hex(missedOffset) << "];";

                        structStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                        structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") DYNAMIC FIELD PADDING\n";
                    }
                }
            }
            else
            {
#ifndef NO_LOGGING
                GLogger::Log("Error: No registered members found for struct type \"" + Member::GetName(structType) + "\"!");
#endif
                Utils::MessageboxError("Error: No registered members found for struct type \"" + Member::GetName(structType) + "\"!");
            }
        }
    }

    void GenerateStruct(std::ofstream& file, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid())
        {
            std::string structNameCPP = Utils::CreateValidName(unrealObj.Object->GetNameCPP());
            std::string structOuterNameCPP = Utils::CreateValidName(unrealObj.Object->Outer->GetNameCPP());

            if (GConfig::IsTypeBlacklisted(structNameCPP))
            {
                return;
            }

#ifndef NO_LOGGING
            GLogger::LogObject("ScriptStruct: ", unrealObj);
#endif

            std::ostringstream structStream;
            std::ostringstream propertyStream;
            std::ostringstream flagStream;

            structStream << "// " << unrealObj.FullName << "\n";

            if (!GConfig::IsTypeOveridden(structNameCPP))
            {
                size_t size = 0;
                size_t lastOffset = 0;
                size_t missedOffset = 0;

                UScriptStruct* scriptStruct = static_cast<UScriptStruct*>(unrealObj.Object);
                UScriptStruct* superField = static_cast<UScriptStruct*>(scriptStruct->SuperField);
                uint32_t structCount = GCache::CountObject<UScriptStruct>(unrealObj.ValidName);

                if (superField && (superField != scriptStruct))
                {
                    size = (scriptStruct->PropertySize - superField->PropertySize);
                    lastOffset = superField->PropertySize;

                    std::string fieldName = Utils::CreateValidName(superField->GetName());
                    std::string fieldNameCPP = Utils::CreateValidName(superField->GetNameCPP());
                    uint32_t fieldStructCount = GCache::CountObject<UScriptStruct>(fieldName);

                    structStream << "// " << Printer::Hex(size, EWidthTypes::Size);
                    structStream << " (" << Printer::Hex(superField->PropertySize, EWidthTypes::Size);
                    structStream << " - " << Printer::Hex(scriptStruct->PropertySize, EWidthTypes::Size);
                    structStream << ")\n";

                    if (structCount > 1)
                    {
                        structStream << "struct " << structOuterNameCPP << "_" << structNameCPP << " : ";
                    }
                    else
                    {
                        structStream << "struct " << structNameCPP << " : ";
                    }

                    if ((fieldStructCount > 1) && superField->Outer)
                    {
                        structStream << Utils::CreateValidName(superField->Outer->GetNameCPP()) << "_" << fieldNameCPP << "\n";
                    }
                    else
                    {
                        structStream << fieldNameCPP << "\n";
                    }
                }
                else
                {
                    size = scriptStruct->PropertySize;
                    structStream << "// " << Printer::Hex(size, EWidthTypes::Size) << "\n";

                    if (structCount > 1)
                    {
                        structStream << "struct " << structOuterNameCPP << "_" << structNameCPP << "\n";
                    }
                    else
                    {
                        structStream << "struct " << structNameCPP << "\n";
                    }
                }

                structStream << "{\n";
                std::vector<UnrealProperty> structProperties;

                for (UProperty* uProperty = static_cast<UProperty*>(scriptStruct->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                {
                    if (uProperty
                        && (uProperty->ElementSize > 0)
                        && !uProperty->IsA<UScriptStruct>())
                    {
                        UnrealProperty unrealProp(uProperty);

                        if (unrealProp.IsValid())
                        {
                            structProperties.push_back(uProperty);
                        }
                    }
                }

                std::sort(structProperties.begin(), structProperties.end(), Utils::SortProperty);
                std::map<std::string, uint32_t> propertyNameMap;
                uint32_t unknownDataIndex = 0;

                for (const UnrealProperty& unrealProp : structProperties)
                {
                    if (unrealProp.IsValid())
                    {
                        if (lastOffset < unrealProp.Property->Offset)
                        {
                            missedOffset = (unrealProp.Property->Offset - lastOffset);

                            if (missedOffset >= GConfig::GetGameAlignment())
                            {
                                std::string missedStr = Printer::Hex(missedOffset);
                                propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                                propertyStream << "[" << missedStr << "];";

                                Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() + 2));
                                structStream << "\tuint8_t";
                                Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                                structStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                                structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") MISSED OFFSET\n";
                                Printer::Empty(propertyStream);

                                unknownDataIndex++;
                            }
                        }

                        size_t correctElementSize = unrealProp.GetSize();

                        if (propertyNameMap.count(unrealProp.ValidName) == 0)
                        {
                            propertyNameMap[unrealProp.ValidName] = 1;
                            propertyStream << unrealProp.ValidName;
                        }
                        else
                        {
                            propertyStream << unrealProp.ValidName << Printer::Decimal(propertyNameMap[unrealProp.ValidName], EWidthTypes::Byte);
                            propertyNameMap[unrealProp.ValidName]++;
                        }

                        if (unrealProp.IsAnArray())
                        {
                            if (unrealProp.Type != EPropertyTypes::UInterface)
                            {
                                propertyStream << "[" << unrealProp.Property->ArrayDim << "]";
                            }

                            correctElementSize *= unrealProp.Property->ArrayDim;
                        }

                        if (unrealProp.Type == EPropertyTypes::Bool)
                        {
                            propertyStream << " : 1";
                        }

                        Retrievers::GetAllPropertyFlags(flagStream, unrealProp.Property->PropertyFlags);
                        int32_t offsetError = ((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim) - (correctElementSize * unrealProp.Property->ArrayDim));

                        if (unrealProp.Type == EPropertyTypes::UInterface)
                        {
                            size_t interfaceSize = unrealProp.GetSize();

                            if (offsetError == interfaceSize)
                            {
                                offsetError -= interfaceSize;
                            }

                            structStream << "\t";
                            Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                            structStream << unrealProp.GetTypeForStruct() << " " << propertyStream.str();

                            if (unrealProp.IsAnArray())
                            {
                                structStream << "_Object[" << unrealProp.Property->ArrayDim << "];";
                            }
                            else
                            {
                                structStream << "_Object;";
                            }

                            uint32_t propSpacing = (propertyStream.str().size() + 8);

                            if (propSpacing < GConfig::GetStructSpacing())
                            {
                                Printer::FillRight(structStream, ' ', (GConfig::GetStructSpacing() - propSpacing));
                            }

                            structStream << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                            structStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size) << ")";
                            structStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";
                            structStream << flagStream.str() << "\n";

                            structStream << "\t";
                            Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                            structStream << unrealProp.GetTypeForStruct() << " " << propertyStream.str();

                            if (unrealProp.IsAnArray())
                            {
                                structStream << "_Interface[" << unrealProp.Property->ArrayDim << "];";
                            }
                            else
                            {
                                structStream << "_Interface;";
                            }

                            uint32_t interfaceSpacing = (propertyStream.str().size() + 11);

                            if (interfaceSpacing < GConfig::GetStructSpacing())
                            {
                                Printer::FillRight(structStream, ' ', (GConfig::GetStructSpacing() - interfaceSpacing));
                            }

                            structStream << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                            structStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size) << ")";
                            structStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";
                            Printer::FillLeft(structStream, ' ', static_cast<size_t>(EWidthTypes::FieldWidth));
                        }
                        else
                        {
                            structStream << "\t";
                            Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                            structStream << unrealProp.GetTypeForStruct() << " " << propertyStream.str() << ";";

                            uint32_t propSpacing = (propertyStream.str().size() + 1);

                            if (propSpacing < GConfig::GetStructSpacing())
                            {
                                Printer::FillRight(structStream, ' ', (GConfig::GetStructSpacing() - propSpacing));
                            }

                            structStream << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                            structStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size) << ")";
                            structStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";

                            if (unrealProp.Type == EPropertyTypes::Bool)
                            {
                                structStream << "[" << Printer::Hex(static_cast<UBoolProperty*>(unrealProp.Property)->BitMask, EWidthTypes::BitMask) << "] ";
                            }
                            else
                            {
                                Printer::FillLeft(structStream, ' ', static_cast<size_t>(EWidthTypes::FieldWidth));
                            }
                        }

                        structStream << flagStream.str() << "\n";

                        if (offsetError > 0)
                        {
                            std::string missedStr = Printer::Hex(offsetError);
                            propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                            propertyStream << "[" << missedStr << "];";

                            Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() + 2));
                            structStream << "\tuint8_t";
                            Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                            structStream << propertyStream.str() << "// " << Printer::Hex(unrealProp.Property->Offset + offsetError, EWidthTypes::Size);
                            Printer::Empty(propertyStream);

                            structStream << " (" << Printer::Hex(offsetError, EWidthTypes::Size) << ") FIX WRONG SIZE OF PREVIOUS PROPERTY";
                            structStream << " [Original: " << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size);
                            structStream << ", Missing: " << Printer::Hex(offsetError, EWidthTypes::Size) << "]\n";

                            unknownDataIndex++;
                        }

                        Printer::Empty(propertyStream);
                        Printer::Empty(flagStream);

                        lastOffset = (unrealProp.Property->Offset + (unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim));
                    }
                    else if (unrealProp.Property)
                    {
                        std::string missedStr = Printer::Hex(unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim);
                        propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                        propertyStream << "[" << missedStr << "];";

                        Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() + 2));
                        structStream << "\tuint8_t";
                        Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                        structStream << propertyStream.str() << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                        structStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size);
                        structStream << ") UNKNOWN PROPERTY: " << unrealProp.Property->GetFullName() << "\n";
                        Printer::Empty(propertyStream);

                        lastOffset = (unrealProp.Property->Offset + (unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim));
                        unknownDataIndex++;
                    }
                }

                if (lastOffset < scriptStruct->PropertySize)
                {
                    missedOffset = (scriptStruct->PropertySize - lastOffset);

                    if (missedOffset >= GConfig::GetGameAlignment())
                    {
                        std::string missedStr = Printer::Hex(missedOffset);
                        propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                        propertyStream << "[" << missedStr << "];";

                        Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing() + 2);
                        structStream << "\tuint8_t";
                        Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                        structStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                        structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") MISSED OFFSET\n";
                        Printer::Empty(propertyStream);
                    }

                    lastOffset += missedOffset;
                }

#ifndef SKIP_MIN_ALIGNMENT
                if (scriptStruct->MinAlignment)
                {
                    int32_t actualSize = 0;

                    for (int32_t i = 0; actualSize < scriptStruct->PropertySize; i++)
                    {
                        actualSize += scriptStruct->MinAlignment;
                    }

                    if ((lastOffset < actualSize) && (actualSize > scriptStruct->PropertySize))
                    {
                        int32_t padding = (actualSize - lastOffset);
                        std::string paddingStr = Printer::Hex(padding);
                        propertyStream << "MinStructAlignment" << "[" << paddingStr << "];";

                        Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() + 2));
                        structStream << "\tuint8_t";
                        Printer::FillLeft(structStream, ' ', (GConfig::GetStructSpacing() - paddingStr.size()));

                        structStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                        structStream << " (" << Printer::Hex(padding, EWidthTypes::Size) << ") ADDED PADDING\n";

#ifndef NO_LOGGING
                        GLogger::LogStructPadding(scriptStruct, padding);
#endif
                    }
                }
#endif

                structStream << "};\n\n";
            }
            else
            {
                structStream << "// (Custom Override)\n";
                structStream << GConfig::GetTypeOverride(structNameCPP);
            }

            file << structStream.str();
        }
    }

    void GenerateStructPre(std::ofstream& stream, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid() && !m_generatedStructs.contains(unrealObj.FullName))
        {
            UScriptStruct* scriptStruct = static_cast<UScriptStruct*>(unrealObj.Object);
            UnrealObject largestObj = GCache::GetLargestStruct(unrealObj.FullName);

            if (largestObj.IsValid() && (largestObj.Object != unrealObj.Object))
            {
                scriptStruct = static_cast<UScriptStruct*>(unrealObj.Object);
            }

            if (scriptStruct->SuperField && (scriptStruct->SuperField != scriptStruct))
            {
                UScriptStruct* superStruct = static_cast<UScriptStruct*>(scriptStruct->SuperField);

                if (!m_generatedStructs.contains(superStruct->GetFullName()))
                {
                    GenerateStructPre(stream, superStruct);
                }
            }

            for (UProperty* structChild = static_cast<UProperty*>(scriptStruct->Children); structChild; structChild = static_cast<UProperty*>(structChild->Next))
            {
                UnrealProperty structProp(structChild);

                if (structProp.IsValid())
                {
                    if (structProp.Type == EPropertyTypes::FStruct)
                    {
                        UScriptStruct* propertyStruct = static_cast<UScriptStruct*>(static_cast<UStructProperty*>(structProp.Property)->Struct);

                        if (propertyStruct
                            && (propertyStruct != scriptStruct)
                            && !m_generatedStructs.contains(propertyStruct->GetFullName()))
                        {
                            GenerateStructPre(stream, propertyStruct);
                        }
                    }
                    else if (structProp.Type == EPropertyTypes::TArray)
                    {
                        UScriptStruct* propertyStruct = static_cast<UScriptStruct*>(static_cast<UStructProperty*>(static_cast<UArrayProperty*>(structProp.Property)->Inner)->Struct);

                        if (propertyStruct && (propertyStruct != scriptStruct))
                        {
                            UnrealProperty innerProp(static_cast<UArrayProperty*>(structProp.Property)->Inner);

                            if (innerProp.IsValid()
                                && (innerProp.Type == EPropertyTypes::FStruct)
                                && !m_generatedStructs.contains(innerProp.Property->GetFullName()))
                            {
                                GenerateStructPre(stream, propertyStruct);
                            }
                        }
                    }
                }
            }

            GenerateStruct(stream, unrealObj);
            m_generatedStructs[unrealObj.FullName] = unrealObj.Object->ObjectInternalInteger;
        }
    }

    void ProcessStructs(std::ofstream& stream, class UObject* packageObj)
    {
        if (packageObj)
        {
            std::vector<UnrealObject>* objCache = GCache::GetCache(packageObj, EClassTypes::UStruct);

            if (objCache)
            {
                for (const UnrealObject& unrealObj : *objCache)
                {
                    GenerateStructPre(stream, unrealObj);
                }
            }
        }
    }
}

namespace ClassGenerator
{
    static std::map<std::string, int32_t> m_generatedClasses;

    void GenerateClassMembers(std::ostringstream& classStream, class UClass* uClass, EClassTypes classType)
    {
        if (uClass && (classType != EClassTypes::Unknown))
        {
            std::ostringstream propertyStream;

            size_t localSize = 0;
            size_t startOffset = 0;
            size_t missedOffset = 0;
            size_t lastOffset = 0;

            localSize = Member::GetClassSize(classType);
            startOffset = Member::GetClassOffset(classType);
            std::map<size_t, Member*> members = Member::GetRegistered(classType);

            if (!members.empty())
            {
                if (uClass->PropertySize == localSize)
                {
                    lastOffset = startOffset;
                    uint32_t unknownDataIndex = 0;

                    for (const auto& member : members)
                    {
                        if ((lastOffset + member.second->Size) < member.second->Offset)
                        {
                            missedOffset = (member.second->Offset - lastOffset);

                            if (missedOffset >= GConfig::GetGameAlignment())
                            {
                                propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                                propertyStream << "[" << Printer::Hex(missedOffset) << "];";

                                classStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                                classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") DYNAMIC FIELD PADDING\n";
                                Printer::Empty(propertyStream);

                                unknownDataIndex++;
                            }
                        }

                        classStream << "\t" << member.second->Label << " // " << Printer::Hex(member.second->Offset, EWidthTypes::Size);
                        classStream << " (" << Printer::Hex(member.second->Size, EWidthTypes::Size) << ")\n";

                        lastOffset = (member.second->Offset + member.second->Size);
                    }

                    if (lastOffset < uClass->PropertySize)
                    {
                        missedOffset = (uClass->PropertySize - lastOffset);

                        if (missedOffset >= GConfig::GetGameAlignment())
                        {
                            propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                            propertyStream << "[" << Printer::Hex(missedOffset) << "];";

                            classStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                            classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") DYNAMIC FIELD PADDING\n";
                        }
                    }
                }
                else
                {
#ifndef NO_LOGGING
                    GLogger::LogClassSize(uClass, localSize);
#endif
                    Utils::MessageboxError("Error: Incorrect class size detected for \"" + Member::GetName(classType) + "\", check the log file for more details!");
                }
            }
            else
            {
#ifndef NO_LOGGING
                GLogger::Log("Error: No registered members found for class \"" + uClass->GetName() + "\"!");
#endif
                Utils::MessageboxError("Error: No registered members found for \"" + Member::GetName(classType) + "\"!");
            }
        }
    }

    void GenerateClass(std::ofstream& file, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid())
        {
            UClass* uClass = static_cast<UClass*>(unrealObj.Object);
            UClass* uSuperClass = static_cast<UClass*>(uClass->SuperField);
            std::string classNameCPP = Utils::CreateValidName(uClass->GetNameCPP());

            if (GConfig::IsTypeBlacklisted(classNameCPP))
            {
                return;
            }

#ifndef NO_LOGGING
            GLogger::LogObject("Class: ", unrealObj);
#endif

            std::ostringstream classStream;
            std::ostringstream propertyStream;
            std::ostringstream flagStream;

            size_t size = 0;
            size_t lastOffset = 0;
            size_t missedOffset = 0;

            classStream << "// " << unrealObj.FullName << "\n";

            if (!GConfig::IsTypeOveridden(classNameCPP))
            {
                std::vector<UnrealProperty> classProperties;

                for (UProperty* uProperty = static_cast<UProperty*>(uClass->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                {
                    if (uProperty
                        && (uProperty->ElementSize > 0)
                        && !uProperty->IsA<UFunction>()
                        && !uProperty->IsA<UConst>()
                        && !uProperty->IsA<UEnum>()
                        && !uProperty->IsA<UScriptStruct>()
                        && (!uSuperClass || (uSuperClass && (uSuperClass != uClass) && (uProperty->Offset >= uSuperClass->PropertySize))))
                    {
                        UnrealProperty unrealProp(uProperty);

                        if (unrealProp.IsValid())
                        {
                            classProperties.push_back(unrealProp);
                        }
                    }
                }

                std::sort(classProperties.begin(), classProperties.end(), Utils::SortProperty);

                if (uSuperClass && (uSuperClass != uClass))
                {
                    size = (uClass->PropertySize - uSuperClass->PropertySize);
                    lastOffset = uSuperClass->PropertySize;

                    classStream << "// " << Printer::Hex(size, EWidthTypes::Size);
                    classStream << " (" << Printer::Hex(uSuperClass->PropertySize, EWidthTypes::Size);
                    classStream << " - " << Printer::Hex(uClass->PropertySize, EWidthTypes::Size);
                    classStream << ")\n" << "class " << classNameCPP << " : public " << Utils::CreateValidName(uSuperClass->GetNameCPP());
                }
                else
                {
                    size = uClass->PropertySize;

                    classStream << "// " << Printer::Hex(size, EWidthTypes::Size) << "\n";
                    classStream << "class " << classNameCPP;
                }

                classStream << "\n{\npublic:\n";

                if (uClass == UField::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UField); }
                else if (uClass == UEnum::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UEnum); }
                else if (uClass == UConst::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UConst); }
                else if (uClass == UProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UProperty); }
                else if (uClass == UStruct::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UStruct); }
                else if (uClass == UFunction::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UFunction); }
                else if (uClass == UStructProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UStructProperty); }
                else if (uClass == UClassProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UClassProperty); }
                else if (uClass == UObjectProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UObjectProperty); }
                else if (uClass == UMapProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UMapProperty); }
                else if (uClass == UInterfaceProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UInterfaceProperty); }
                else if (uClass == UByteProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UByteProperty); }
                else if (uClass == UBoolProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UBoolProperty); }
                else if (uClass == UArrayProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UArrayProperty); }
                else
                {
                    std::map<std::string, uint32_t> propertyNameMap;
                    uint32_t unknownDataIndex = 0;

                    for (const UnrealProperty& unrealProp : classProperties)
                    {
                        if (unrealProp.IsValid())
                        {
                            if (uClass == UObject::StaticClass())
                            {
                                if (!GConfig::UsingProcessEventIndex() && (unrealProp.ValidName.find("VfTable") != std::string::npos))
                                {
                                    lastOffset = (unrealProp.Property->Offset + (unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim));
                                    continue;
                                }
                            }

                            if (lastOffset < unrealProp.Property->Offset)
                            {
                                missedOffset = (unrealProp.Property->Offset - lastOffset);

                                if (missedOffset >= GConfig::GetGameAlignment())
                                {
                                    std::string missedStr = Printer::Hex(missedOffset);
                                    propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                                    propertyStream << "[" << missedStr << "];";

                                    Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() + 2));
                                    classStream << "\tuint8_t";
                                    Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                                    classStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                                    classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") MISSED OFFSET\n";
                                    Printer::Empty(propertyStream);

                                    unknownDataIndex++;
                                }
                            }

                            size_t correctElementSize = unrealProp.GetSize();

                            if (propertyNameMap.count(unrealProp.ValidName) == 0)
                            {
                                propertyNameMap[unrealProp.ValidName] = 1;
                                propertyStream << unrealProp.ValidName;
                            }
                            else
                            {
                                propertyStream << unrealProp.ValidName << Printer::Decimal(propertyNameMap[unrealProp.ValidName], EWidthTypes::Byte);
                                propertyNameMap[unrealProp.ValidName]++;
                            }

                            if (unrealProp.IsAnArray())
                            {
                                if (unrealProp.Type != EPropertyTypes::UInterface)
                                {
                                    propertyStream << "[" << unrealProp.Property->ArrayDim << "]";
                                }

                                correctElementSize *= unrealProp.Property->ArrayDim;
                            }

                            if (unrealProp.Type == EPropertyTypes::Bool)
                            {
                                propertyStream << " : 1";
                            }

                            Retrievers::GetAllPropertyFlags(flagStream, unrealProp.Property->PropertyFlags);
                            int32_t offsetError = ((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim) - (correctElementSize * unrealProp.Property->ArrayDim));

                            if (unrealProp.Type == EPropertyTypes::UInterface)
                            {
                                size_t interfaceSize = unrealProp.GetSize();

                                if (offsetError == interfaceSize)
                                {
                                    offsetError -= interfaceSize;
                                }

                                classStream << "\t";
                                Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                classStream << unrealProp.GetTypeForClass() << " " << propertyStream.str();

                                if (unrealProp.IsAnArray())
                                {
                                    classStream << "_Object[" << unrealProp.Property->ArrayDim << "];";
                                    correctElementSize *= unrealProp.Property->ArrayDim;
                                }
                                else
                                {
                                    classStream << "_Object;";
                                }

                                uint32_t propSpacing = (propertyStream.str().size() + 8);

                                if (propSpacing < GConfig::GetStructSpacing())
                                {
                                    Printer::FillRight(classStream, ' ', (GConfig::GetStructSpacing() - propSpacing));
                                }

                                classStream << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                                classStream << " (" << Printer::Hex(((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim) - interfaceSize), EWidthTypes::Size) << ")";
                                classStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";
                                classStream << flagStream.str() << "\n";

                                classStream << "\t";
                                Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                classStream << unrealProp.GetTypeForClass() << " " << propertyStream.str();

                                if (unrealProp.IsAnArray())
                                {
                                    classStream << "_Interface[" << unrealProp.Property->ArrayDim << "];";
                                    correctElementSize *= unrealProp.Property->ArrayDim;
                                }
                                else
                                {
                                    classStream << "_Interface;";
                                }

                                uint32_t interfaceSpacing = (propertyStream.str().size() + 11);

                                if (interfaceSpacing < GConfig::GetStructSpacing())
                                {
                                    Printer::FillRight(classStream, ' ', (GConfig::GetStructSpacing() - interfaceSpacing));
                                }

                                classStream << "// " << Printer::Hex(unrealProp.Property->Offset + interfaceSize, EWidthTypes::Size);
                                classStream << " (" << Printer::Hex(((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim) - interfaceSize), EWidthTypes::Size) << ")";
                                classStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";
                                Printer::FillLeft(classStream, ' ', static_cast<size_t>(EWidthTypes::FieldWidth));
                            }
                            else
                            {
                                classStream << "\t";
                                Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                classStream << unrealProp.GetTypeForClass() << " " << propertyStream.str() << ";";

                                uint32_t propSpacing = (propertyStream.str().size() + 1);

                                if (propSpacing < GConfig::GetStructSpacing())
                                {
                                    Printer::FillRight(classStream, ' ', (GConfig::GetStructSpacing() - propSpacing));
                                }

                                classStream << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                                classStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size) << ")";
                                classStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";

                                if (unrealProp.Type == EPropertyTypes::Bool)
                                {
                                    classStream << "[" << Printer::Hex(static_cast<UBoolProperty*>(unrealProp.Property)->BitMask, EWidthTypes::BitMask) << "] ";
                                }
                                else
                                {
                                    Printer::FillLeft(classStream, ' ', static_cast<size_t>(EWidthTypes::FieldWidth));
                                }
                            }

                            classStream << flagStream.str() << "\n";

                            if (offsetError > 0)
                            {
                                std::string missedStr = Printer::Hex(offsetError);
                                propertyStream << "_UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                                propertyStream << "[" << missedStr << "];";

                                Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() + 2));
                                classStream << "\tuint8_t";
                                Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                                classStream << propertyStream.str() << "// " << Printer::Hex((unrealProp.Property->Offset + offsetError), EWidthTypes::Size);
                                classStream << " (" << Printer::Hex(offsetError, EWidthTypes::Size) << ") FIX WRONG SIZE OF PREVIOUS PROPERTY ";
                                classStream << " [Original:" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size);
                                classStream << ", Missing: " << Printer::Hex(offsetError, EWidthTypes::Size) << "]\n";
                                Printer::Empty(propertyStream);

                                unknownDataIndex++;
                            }

                            Printer::Empty(propertyStream);
                            Printer::Empty(flagStream);

                            lastOffset = (unrealProp.Property->Offset + (unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim));
                        }
                        else if (unrealProp.Property)
                        {
                            std::string missedStr = Printer::Hex(unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim);
                            propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                            propertyStream << "[" << missedStr << "];";

                            Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() + 2));
                            classStream << "\tuint8_t";
                            Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                            classStream << propertyStream.str() << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                            classStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size);
                            classStream << ") UNKNOWN PROPERTY: " << unrealProp.Property->GetFullName() << "\n";
                            Printer::Empty(propertyStream);

                            lastOffset = (unrealProp.Property->Offset + (unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim));
                            unknownDataIndex++;
                        }
                    }

                    if (lastOffset < uClass->PropertySize)
                    {
                        missedOffset = (uClass->PropertySize - lastOffset);

                        if (missedOffset >= GConfig::GetGameAlignment())
                        {
                            std::string missedStr = Printer::Hex(missedOffset);
                            propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::Byte);
                            propertyStream << "[" << missedStr << "];";

                            Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() + 2));
                            classStream << "\tuint8_t";
                            Printer::FillLeft(classStream, ' ', (GConfig::GetStructSpacing() - missedStr.size()));

                            classStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::Size);
                            classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::Size) << ") MISSED OFFSET\n";
                            Printer::Empty(propertyStream);
                        }
                    }
                }

                classStream << "\npublic:\n";
                classStream << "\tstatic UClass* StaticClass()\n";
                classStream << "\t{\n";
                classStream << "\t\tstatic UClass* uClassPointer = nullptr;\n\n";
                classStream << "\t\tif (!uClassPointer)\n";
                classStream << "\t\t{\n";

                if (GConfig::UsingConstants())
                {
                    classStream << "\t\t\tuClassPointer = reinterpret_cast<UClass*>(UObject::GObjObjects()->at(" << GCache::GetConstant(unrealObj).first << "));\n";
                }
                else
                {
                    classStream << "\t\t\tuClassPointer = UObject::FindClass(\"" << unrealObj.FullName << "\");\n";
                }

                classStream << "\t\t}\n\n";
                classStream << "\t\treturn uClassPointer;\n";
                classStream << "\t};\n\n";

                if (uClass == UObject::StaticClass())
                {
                    classStream << PiecesOfCode::UObject_FunctionDescriptions;
                }

                file << classStream.str();
                Printer::Empty(classStream);

                FunctionGenerator::GenerateFunctionDescription(file, unrealObj);

                if (uClass == UObject::StaticClass())
                {
                    if (GConfig::UsingProcessEventIndex())
                    {
                        classStream << "\tvoid ProcessEvent(class UFunction* uFunction, void* uParams, void* uResult = nullptr);\n";
                    }
                    else if (GConfig::GetProcessEventIndex() != -1)
                    {
                        FunctionGenerator::GenerateVirtualFunctions(file);
                    }
                }
                else if (uClass == UFunction::StaticClass())
                {
                    classStream << "\tstatic UFunction* FindFunction(const std::string& functionFullName);\n";
                }

                classStream << "};\n\n";
            }
            else
            {
                classStream << "// (Custom Override)\n";
                classStream << GConfig::GetTypeOverride(classNameCPP);
            }

            file << classStream.str();
        }
    }

    void GenerateClassPre(std::ofstream& file, const UnrealObject& unrealObj, class UObject* packageObj)
    {
        if (unrealObj.IsValid() && !m_generatedClasses.contains(unrealObj.FullName))
        {
            if (unrealObj.Package != packageObj)
            {
                return;
            }

            UClass* uClass = static_cast<UClass*>(unrealObj.Object);

            if (uClass->SuperField && (uClass->SuperField != uClass))
            {
                if (!m_generatedClasses.contains(uClass->SuperField->GetFullName()))
                {
                    GenerateClassPre(file, static_cast<UClass*>(uClass->SuperField), packageObj);
                }
            }

            GenerateClass(file, uClass);
            m_generatedClasses[unrealObj.FullName] = uClass->ObjectInternalInteger;
        }
    }

    void ProcessClasses(std::ofstream& stream, class UObject* packageObj)
    {
        if (packageObj)
        {
            std::vector<UnrealObject>* objCache = GCache::GetCache(packageObj, EClassTypes::UClass);

            if (objCache)
            {
                for (const UnrealObject& unrealObj : *objCache)
                {
                    GenerateClassPre(stream, unrealObj, packageObj);
                }
            }
        }
    }
}

namespace ParameterGenerator
{
    void GenerateParameter(std::ofstream& file, const UnrealObject& unrealObj)
    {
        std::ostringstream parameterStream;
        std::ostringstream propertyStream;
        std::ostringstream flagStream;

        UClass* uClass = static_cast<UClass*>(unrealObj.Object);
        std::vector<UFunction*> classFunctions;

        for (UProperty* uProperty = static_cast<UProperty*>(uClass->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
        {
            if (uProperty && uProperty->IsA<UFunction>())
            {
                classFunctions.push_back(reinterpret_cast<UFunction*>(uProperty));
            }
        }

        for (UFunction* uFunction : classFunctions)
        {
            if (uFunction)
            {
                std::string functionFullName = uFunction->GetFullName();
                std::string functionName = Utils::CreateValidName(uFunction->GetName());
                std::string classNameCPP = Utils::CreateValidName(uClass->GetNameCPP());

                parameterStream << "// " << functionFullName << "\n";
                parameterStream << "// [" << Printer::Hex(uFunction->FunctionFlags, EWidthTypes::FunctionFlags) << "] ";

                if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Native) && uFunction->iNative)
                {
                    parameterStream << " (iNative[" << uFunction->iNative << "])";
                }

                if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { propertyStream << "exec"; }
                else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { propertyStream << "event"; }
                else { propertyStream << "exec"; }

                if (GConfig::UsingWindows())
                {
                    Utils::CreateWindowsName(functionName);
                }

                parameterStream << "\nstruct " << classNameCPP << "_" << propertyStream.str() << functionName << "_Params\n" << "{\n";
                Printer::Empty(propertyStream);

                std::vector<UnrealProperty> funcProperties;

                for (UProperty* uProperty = static_cast<UProperty*>(uFunction->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                {
                    if (uProperty && (uProperty->ElementSize > 0))
                    {
                        funcProperties.push_back(UnrealProperty(uProperty));
                    }
                }

                std::sort(funcProperties.begin(), funcProperties.end(), Utils::SortProperty);
                std::map<std::string, uint32_t> propertyNameMap;

                for (UnrealProperty& unrealProp : funcProperties)
                {
                    if (unrealProp.IsValid())
                    {
                        std::string propertyType = unrealProp.GetTypeForStruct();

                        if (propertyNameMap.count(unrealProp.ValidName) == 0)
                        {
                            propertyNameMap[unrealProp.ValidName] = 1;
                            propertyStream << unrealProp.ValidName;
                        }
                        else
                        {
                            propertyStream << unrealProp.ValidName << Printer::Decimal(propertyNameMap[unrealProp.ValidName], EWidthTypes::Byte);
                            propertyNameMap[unrealProp.ValidName]++;
                        }

                        if (unrealProp.Property->ArrayDim > 1)
                        {
                            propertyStream << "[" << unrealProp.Property->ArrayDim << "]";
                        }

                        if (unrealProp.Type == EPropertyTypes::Bool)
                        {
                            propertyStream << " : 1";
                        }

                        propertyStream << ";";
                        Retrievers::GetAllPropertyFlags(flagStream, unrealProp.Property->PropertyFlags);

                        parameterStream << "\t";
                        int32_t spacingSub = 0;

                        if (!unrealProp.IsParameter())
                        {
                            parameterStream << "// ";
                            spacingSub = 3;
                        }

                        Printer::FillLeft(parameterStream, ' ', (GConfig::GetFunctionSpacing() - spacingSub));
                        parameterStream << propertyType << " ";
                        Printer::FillLeft(parameterStream, ' ', GConfig::GetFunctionSpacing());

                        parameterStream << propertyStream.str() << "// " << Printer::Hex(unrealProp.Property->Offset, EWidthTypes::Size);
                        parameterStream << " (" << Printer::Hex((unrealProp.Property->ElementSize * unrealProp.Property->ArrayDim), EWidthTypes::Size) << ")";
                        parameterStream << " [" << Printer::Hex(unrealProp.Property->PropertyFlags, EWidthTypes::PropertyFlags) << "] ";

                        if (unrealProp.Type == EPropertyTypes::Bool)
                        {
                            parameterStream << "[" << Printer::Hex(static_cast<UBoolProperty*>(unrealProp.Property)->BitMask, EWidthTypes::BitMask) << "] ";
                        }
                        else
                        {
                            Printer::FillLeft(parameterStream, ' ', static_cast<size_t>(EWidthTypes::FieldWidth));
                        }

                        parameterStream << flagStream.str() << "\n";
                        Printer::Empty(propertyStream);
                        Printer::Empty(flagStream);
                    }
                    else if (unrealProp.Property)
                    {
                        parameterStream << "\t// UNKNOWN PROPERTY: " << unrealProp.Property->GetFullName() << "\n";
                    }
                }

                parameterStream << "};\n\n";
            }
        }

        file << parameterStream.str();
    }

    void ProcessParameters(std::ofstream& stream, UObject* packageObj)
    {
        if (packageObj)
        {
            std::vector<UnrealObject>* objCache = GCache::GetCache(packageObj, EClassTypes::UClass);

            if (objCache)
            {
                for (const UnrealObject& unrealObj : *objCache)
                {
                    GenerateParameter(stream, unrealObj);
                }
            }
        }
    }
}

namespace FunctionGenerator
{
    static bool m_printedObjects = false;
    static bool m_printedFunctions = false;

    void GenerateVirtualFunctions(std::ofstream& stream)
    {
        uintptr_t processEventAddress = 0;

        if (!GConfig::UsingProcessEventIndex())
        {
            processEventAddress = Retrievers::FindPattern(GConfig::GetProcessEventPattern(), GConfig::GetProcessEventMask());
        }
        else if (GConfig::GetProcessEventIndex() != -1)
        {
            processEventAddress = reinterpret_cast<uintptr_t*>(UObject::StaticClass()->VfTableObject.Dummy)[GConfig::GetProcessEventIndex()];
        }
        else
        {
            stream << "\n\t// FIX PROCESSEVENT IN CONFIGURATION.CPP, INVALID INDEX";
            Utils::MessageboxWarn("Warning: ProcessEvent is not configured correctly in \"Configuration.cpp\", you set \"UsingIndex\" to true yet you did not provide a valid index for process event!");
        }

        if (processEventAddress)
        {
            stream << "\n\t// Virtual Functions\n\n";

            for (uint32_t index = 0; index < 2560; index++)
            {
                uintptr_t virtualFunction = reinterpret_cast<uintptr_t*>(UObject::StaticClass()->VfTableObject.Dummy)[index];

                if (virtualFunction == processEventAddress)
                {
                    stream << "\tvirtual void ProcessEvent(class UFunction* uFunction, void* uParams, void* uResult = nullptr);";
                    stream << "\t// " << Printer::Hex(virtualFunction, sizeof(virtualFunction)) << "\n";
                    break;
                }
                else
                {
                    stream << "\tvirtual void VirtualFunction" << Printer::Decimal(index, EWidthTypes::Byte) << "();";
                    stream << "\t// " << Printer::Hex(virtualFunction, sizeof(virtualFunction)) << "\n";
                }
            }
        }
        else
        {
            stream << "\n\t// FIX PROCESSEVENT IN CONFIGURATION.CPP, INVALID ADDRESS";
            Utils::MessageboxWarn("Warning: ProcessEvent is not configured correctly in \"Configuration.cpp\", failed to find a valid address!");
        }
    }

    void GenerateFunctionCode(std::ofstream& stream, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid())
        {
            UClass* uClass = static_cast<UClass*>(unrealObj.Object);
            std::ostringstream codeStream;
            std::ostringstream functionStream;

            if (!m_printedObjects && (uClass == UObject::StaticClass()))
            {
                codeStream << PiecesOfCode::UObject_Functions;

                if (GConfig::UsingProcessEventIndex())
                {
                    codeStream << "template<typename T> T GetVirtualFunction(const void* instance, size_t index)\n";
                    codeStream << "{\n";
                    codeStream << "\tauto vtable = *static_cast<const void***>(const_cast<void*>(instance));\n";
                    codeStream << "\treturn reinterpret_cast<T>(vtable[index]);\n";
                    codeStream << "}\n\n";

                    codeStream << "void UObject::ProcessEvent(class UFunction* uFunction, void* uParams, void* uResult)\n";
                    codeStream << "{\n";
                    codeStream << "\tGetVirtualFunction<void(*)(class UObject*, class UFunction*, void*)>(this, " << GConfig::GetProcessEventIndex() << ")(this, uFunction, uParams);\n";
                    codeStream << "}\n\n";
                }

                m_printedObjects = true;
            }

            if (!m_printedFunctions && (uClass == UFunction::StaticClass()))
            {
                codeStream << PiecesOfCode::UFunction_Functions;
                m_printedFunctions = true;
            }

            std::vector<UFunction*> classFunctions;

            for (UField* uField = uClass->Children; uField; uField = uField->Next)
            {
                if (uField && uField->IsA<UFunction>())
                {
                    classFunctions.push_back(static_cast<UFunction*>(uField));
                }
            }

            for (UFunction* uFunction : classFunctions)
            {
                if (uFunction)
                {
                    std::string functionFullName = uFunction->GetFullName();
                    std::string functionName = Utils::CreateValidName(uFunction->GetName());
                    std::string classNameCPP = Utils::CreateValidName(uClass->GetNameCPP());

                    Retrievers::GetAllFunctionFlags(functionStream, uFunction->FunctionFlags);
                    codeStream << "// " << functionFullName << "\n";
                    codeStream << "// [" << Printer::Hex(uFunction->FunctionFlags, EWidthTypes::FunctionFlags) << "] " << functionStream.str();
                    Printer::Empty(functionStream);

                    if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Native) && uFunction->iNative)
                    {
                        codeStream << " (iNative[" << uFunction->iNative << "])";
                    }

                    std::vector<std::pair<UnrealProperty, std::string>> propertyParams;
                    std::vector<std::pair<UnrealProperty, std::string>> propertyOutParams;
                    std::vector<std::pair<UnrealProperty, std::string>> propertyBothParams;
                    std::pair<UnrealProperty, std::string> returnParam;
                    std::map<std::string, uint32_t> propertyNameMap;

                    for (UProperty* uProperty = static_cast<UProperty*>(uFunction->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                    {
                        UnrealProperty unrealProp(uProperty);

                        if (unrealProp.IsValid())
                        {
                            std::string propertyNameUnique;

                            if (propertyNameMap.count(unrealProp.ValidName) == 0)
                            {
                                propertyNameMap[unrealProp.ValidName] = 1;
                                propertyNameUnique = unrealProp.ValidName;
                            }
                            else
                            {
                                functionStream << unrealProp.ValidName << Printer::Decimal(propertyNameMap[unrealProp.ValidName], EWidthTypes::Byte);
                                propertyNameUnique = functionStream.str();
                                Printer::Empty(functionStream);
                                propertyNameMap[unrealProp.ValidName]++;
                            }

                            if (unrealProp.IsReturnParameter())
                            {
                                returnParam = { uProperty, propertyNameUnique };
                            }
                            else if (unrealProp.IsParameter())
                            {
                                if (unrealProp.IsOutParameter())
                                {
                                    propertyOutParams.push_back({ unrealProp, propertyNameUnique });
                                    propertyBothParams.push_back({ unrealProp, propertyNameUnique });
                                }
                                else
                                {
                                    propertyParams.push_back({ unrealProp, propertyNameUnique });
                                }
                            }
                        }
                    }

                    std::sort(propertyParams.begin(), propertyParams.end(), Utils::SortPropertyPair);
                    std::sort(propertyOutParams.begin(), propertyOutParams.end(), Utils::SortPropertyPair);
                    std::sort(propertyBothParams.begin(), propertyBothParams.end(), Utils::SortPropertyPair);

                    codeStream << "\n// Parameter Info:\n";

                    if (returnParam.first.IsValid())
                    {
                        Retrievers::GetAllPropertyFlags(functionStream, returnParam.first.Property->PropertyFlags);
                        codeStream << "// ";
                        Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                        codeStream << returnParam.first.GetTypeForParameter() << " ";
                        Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                        codeStream << returnParam.second << " " << functionStream.str() << "\n";
                        Printer::Empty(functionStream);
                    }

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            Retrievers::GetAllPropertyFlags(functionStream, propertyPair.first.Property->PropertyFlags);
                            codeStream << "// ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyPair.first.GetTypeForClass() << " ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyPair.second << " " << functionStream.str() << "\n";
                            Printer::Empty(functionStream);
                        }
                    }

                    for (const auto& propertyPair : propertyOutParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            Retrievers::GetAllPropertyFlags(functionStream, propertyPair.first.Property->PropertyFlags);
                            codeStream << "// ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyPair.first.GetTypeForClass() << " ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyPair.second << " " << functionStream.str() << "\n";
                            Printer::Empty(functionStream);
                        }
                    }

                    if (returnParam.first.IsValid())
                    {
                        codeStream << "\n" << returnParam.first.GetTypeForClass();
                    }
                    else
                    {
                        codeStream << "\nvoid";
                    }

                    if (GConfig::UsingWindows())
                    {
                        Utils::CreateWindowsName(functionName);
                    }

                    if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { codeStream << " " << classNameCPP << "::" << functionName << "("; }
                    else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { codeStream << " " << classNameCPP << "::event" << functionName << "("; }
                    else { codeStream << " " << classNameCPP << "::" << functionName << "("; }

                    bool printComma = false;

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            if (printComma)
                            {
                                codeStream << ", ";
                            }

                            codeStream << propertyPair.first.GetTypeForParameter() << " " << propertyPair.second;

                            if (propertyPair.first.IsAnArray())
                            {
                                codeStream << "[" << propertyPair.first.Property->ArrayDim << "]";
                            }

                            printComma = true;
                        }
                    }

                    for (const auto& propertyPair : propertyOutParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            if (printComma)
                            {
                                codeStream << ", ";
                            }

                            codeStream << propertyPair.first.GetTypeForParameter(true) << "& " << propertyPair.second;
                            printComma = true;
                        }
                    }

                    codeStream << ")\n";
                    codeStream << "{\n";
                    codeStream << "\tstatic UFunction* uFn" << functionName << " = nullptr;\n\n";
                    codeStream << "\tif (!uFn" << functionName << ")\n";
                    codeStream << "\t{\n";

                    if (GConfig::UsingConstants())
                    {
                        codeStream << "\t\tuFn" << functionName << " = reinterpret_cast<UFunction*>(UObject::GObjObjects()->at(" << GCache::GetConstant(unrealObj).first << "));\n";
                    }
                    else
                    {
                        codeStream << "\t\tuFn" << functionName << " = UFunction::FindFunction(\"" << functionFullName << "\");\n";
                    }

                    codeStream << "\t}\n\n";
                    codeStream << "\t" << classNameCPP << "_";

                    if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { codeStream << "exec"; }
                    else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { codeStream << "event"; }
                    else { codeStream << "exec"; }

                    codeStream << functionName << "_Params " << functionName << "_Params;\n";
                    codeStream << "\tmemset(&" << functionName << "_Params, 0, sizeof(" << functionName << "_Params));\n";

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            if (!propertyPair.first.CantMemcpy())
                            {
                                codeStream << "\tmemcpy_s(&" << functionName << "_Params." << propertyPair.second << ", sizeof(" << functionName << "_Params." << propertyPair.second << ")";

                                //if (propertyPair.first.IsAnArray())
                                //{
                                //    codeStream << " * " << propertyPair.first.Property->ArrayDim;
                                //}

                                codeStream << ", &" << propertyPair.second << ", sizeof(" << propertyPair.second << ")";

                                //if (propertyPair.first.IsAnArray())
                                //{
                                //    codeStream << " * " << propertyPair.first.Property->ArrayDim;
                                //}

                                codeStream << ");\n";
                            }
                            else if (!propertyPair.first.IsContainer())
                            {
                                codeStream << "\t" << functionName << "_Params." << propertyPair.second << " = " << propertyPair.second << ";\n";
                            }
                        }
                    }

                    for (const auto& propertyPair : propertyBothParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            if (!propertyPair.first.CantMemcpy())
                            {
                                codeStream << "\tmemcpy_s(&" << functionName << "_Params." << propertyPair.second << ", sizeof(" << functionName << "_Params." << propertyPair.second << ")";

                                //if (propertyPair.first.IsAnArray())
                                //{
                                //      codeStream << " * " << propertyPair.first.Property->ArrayDim;
                                //}

                                codeStream << ", &" << propertyPair.second << ", sizeof(" << propertyPair.second << ")";

                                //if (propertyPair.first.IsAnArray())
                                //{
                                //      codeStream << " * " << propertyPair.first.Property->ArrayDim;
                                //}

                                codeStream << ");\n";
                            }
                            else if (!propertyPair.first.IsContainer())
                            {
                                codeStream << "\t" << functionName << "_Params." << propertyPair.second << " = " << propertyPair.second << ";\n";
                            }
                        }
                    }

                    bool hasNativeIndex = (uFunction->iNative ? true : false);
                    bool hasNativeFlags = (uFunction->FunctionFlags & EFunctionFlags::FUNC_Native);

                    if (hasNativeFlags && hasNativeIndex && GConfig::RemoveNativeIndex())
                    {
                        codeStream << "\n\tuFn" << functionName << "->iNative = 0;";
                    }

                    if (GConfig::RemoveNativeFlags() && hasNativeFlags)
                    {
                        codeStream << "\n\tuFn" << functionName << "->FunctionFlags &= ~" << Printer::Hex(EFunctionFlags::FUNC_Native) << ";";
                    }

                    if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Static) && (uFunction->FunctionFlags != EFunctionFlags::FUNC_AllFlags))
                    {
                        codeStream << "\n\t" << classNameCPP << "::StaticClass()->ProcessEvent(" << "uFn" << functionName << ", &" << functionName << "_Params, nullptr);\n";
                    }
                    else
                    {
                        codeStream << "\n\tthis->ProcessEvent(uFn" << functionName << ", &" << functionName << "_Params, nullptr);\n";
                    }

                    if (hasNativeFlags && GConfig::RemoveNativeFlags())
                    {
                        codeStream << "\tuFn" << functionName << "->FunctionFlags |= " << Printer::Hex(EFunctionFlags::FUNC_Native) << ";\n";
                    }

                    if (hasNativeFlags && hasNativeIndex && GConfig::RemoveNativeIndex())
                    {
                        codeStream << "\tuFn" << functionName << "->iNative = " << uFunction->iNative << ";\n";
                    }

                    if (!propertyOutParams.empty())
                    {
                        codeStream << "\n";

                        for (const auto& propertyPair : propertyOutParams)
                        {
                            if (propertyPair.first.IsValid())
                            {
                                if (!propertyPair.first.CantMemcpy())
                                {
                                    codeStream << "\tmemcpy_s(&" << propertyPair.second << ", sizeof(" << propertyPair.second << ")";

                                    //if (propertyPair.first.IsAnArray())
                                    //{
                                    //     codeStream << " * " << propertyPair.first.Property->ArrayDim;
                                    //}

                                    codeStream << ", &" << functionName << "_Params." << propertyPair.second << ", sizeof(" << functionName << "_Params." << propertyPair.second << ")";

                                    //if (propertyPair.first.IsAnArray())
                                    //{
                                    //    codeStream << " * " << propertyPair.first.Property->ArrayDim;
                                    //}

                                    codeStream << ");\n";
                                }
                                else if (!propertyPair.first.IsContainer())
                                {
                                    codeStream << "\t" << propertyPair.second << " = " << functionName << "_Params." << propertyPair.second << ";\n";
                                }
                            }
                        }
                    }

                    if (returnParam.first.IsValid())
                    {
                        codeStream << "\n\treturn ";

                        if (GConfig::UsingEnumClasses() && (returnParam.first.Type == EPropertyTypes::UInt8))
                        {
                            std::string returnType = returnParam.first.GetTypeForClass();

                            if (returnType != "uint8_t")
                            {
                                codeStream << "static_cast<" << returnType << ">(" << functionName << "_Params." << returnParam.second << ");\n";
                            }
                            else
                            {
                                codeStream << functionName << "_Params." << returnParam.second << ";\n";
                            }
                        }
                        else
                        {
                            codeStream << functionName << "_Params." << returnParam.second << ";\n";
                        }
                    }

                    codeStream << "};\n\n";
                }
            }

            stream << codeStream.str();
        }
    }

    void GenerateFunctionDescription(std::ofstream& stream, const UnrealObject& unrealObj)
    {
        if (unrealObj.IsValid())
        {
            UClass* uClass = static_cast<UClass*>(unrealObj.Object);
            std::ostringstream functionStream;
            std::ostringstream propertyStream;
            std::vector<UFunction*> classFunctions;

            for (UField* uField = uClass->Children; uField; uField = uField->Next)
            {
                if (uField && uField->IsA<UFunction>())
                {
                    classFunctions.push_back(static_cast<UFunction*>(uField));
                }
            }

            for (UFunction* uFunction : classFunctions)
            {
                if (uFunction)
                {
                    std::string functionName = Utils::CreateValidName(uFunction->GetName());
                    std::vector<std::pair<UnrealProperty, std::string>> funcParams;
                    std::vector<std::pair<UnrealProperty, std::string>> outParams;
                    std::pair<UnrealProperty, std::string> returnParam;
                    std::map<std::string, uint32_t> propertyNameMap;

                    for (UProperty* uProperty = static_cast<UProperty*>(uFunction->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                    {
                        UnrealProperty unrealProp(uProperty);

                        if (unrealProp.IsValid())
                        {
                            std::string propertyNameUnique;

                            if (propertyNameMap.count(unrealProp.ValidName) == 0)
                            {
                                propertyNameMap[unrealProp.ValidName] = 1;
                                propertyNameUnique = unrealProp.ValidName;
                            }
                            else
                            {
                                propertyStream << unrealProp.ValidName << Printer::Decimal(propertyNameMap[unrealProp.ValidName], EWidthTypes::Byte);
                                propertyNameUnique = propertyStream.str();
                                Printer::Empty(propertyStream);
                                propertyNameMap[unrealProp.ValidName]++;
                            }

                            if (unrealProp.IsReturnParameter())
                            {
                                returnParam = { unrealProp, propertyNameUnique };
                            }
                            else if (unrealProp.IsOutParameter())
                            {
                                propertyNameUnique[0] = std::toupper(propertyNameUnique[0]);
                                outParams.push_back({ unrealProp, ("out" + propertyNameUnique) });
                            }
                            else if (unrealProp.IsParameter())
                            {
                                if (unrealProp.IsOptionalParameter())
                                {
                                    propertyNameUnique[0] = std::toupper(propertyNameUnique[0]);
                                    funcParams.push_back({ unrealProp, ("optional" + propertyNameUnique) });
                                }
                                else
                                {
                                    funcParams.push_back({ unrealProp, propertyNameUnique });
                                }
                            }
                        }
                    }

                    std::sort(funcParams.begin(), funcParams.end(), Utils::SortPropertyPair);
                    std::sort(outParams.begin(), outParams.end(), Utils::SortPropertyPair);

                    functionStream << "\t";

                    if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Static) && (uFunction->FunctionFlags != EFunctionFlags::FUNC_AllFlags))
                    {
                        functionStream << "static ";
                    }

                    if (returnParam.first.IsValid())
                    {
                        functionStream << returnParam.first.GetTypeForParameter();
                    }
                    else
                    {
                        functionStream << "void";
                    }

                    if (GConfig::UsingWindows())
                    {
                        Utils::CreateWindowsName(functionName);
                    }

                    if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { functionStream << " " << functionName << "("; }
                    else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { functionStream << " event" << functionName << "("; }
                    else { functionStream << " " << functionName << "("; }

                    bool printComma = false;

                    for (const auto& propertyPair : funcParams)
                    {
                        if (propertyPair.first.IsValid())
                        {
                            if (printComma)
                            {
                                functionStream << ", ";
                            }

                            functionStream << propertyPair.first.GetTypeForParameter() << " " << propertyPair.second;

                            if (propertyPair.first.IsAnArray())
                            {
                                functionStream << "[" << propertyPair.first.Property->ArrayDim << "]";
                            }

                            printComma = true;
                        }
                    }

                    for (const auto& propertyPair : outParams)
                    {
                        if (propertyPair.first.IsValid() && propertyPair.first.IsParameter())
                        {
                            if (printComma)
                            {
                                functionStream << ", ";
                            }

                            functionStream << propertyPair.first.GetTypeForParameter() << "& " << propertyPair.second;
                            printComma = true;
                        }
                    }

                    functionStream << ");\n";
                }
            }

            stream << functionStream.str();
        }
    }

    void ProcessFunctions(std::ofstream& stream, class UObject* packageObj)
    {
        if (packageObj)
        {
            std::vector<UnrealObject>* objCache = GCache::GetCache(packageObj, EClassTypes::UClass);

            if (objCache)
            {
                for (const UnrealObject& unrealObj : *objCache)
                {
                    GenerateFunctionCode(stream, unrealObj);
                }
            }
        }
    }
}

namespace Generator
{
    void GenerateConstants()
    {
        if (GConfig::UsingConstants())
        {
            std::ofstream constantsFile(GConfig::GetOutputPath() / GConfig::GetGameNameShort() / "SdkConstants.hpp");
            constantsFile << "#pragma once\n";

            std::map<std::string, class UObject*>* constants = GCache::GetConstants();

            for (const auto& constantPair : *constants)
            {
                if (constantPair.second)
                {
                    constantsFile << "#define " << constantPair.first;
                    Printer::FillRight(constantsFile, ' ', (CONST_VALUE_SPACING - constantPair.first.size()));
                    constantsFile << constantPair.second->ObjectInternalInteger << "\n";
                }
            }

            constantsFile.close();
        }
    }

    void GenerateHeaders()
    {
        std::ofstream headersFile(GConfig::GetOutputPath() / GConfig::GetGameNameShort() / "SdkHeaders.hpp");

        Printer::Header(headersFile, "SdkHeaders", "hpp", false);
        headersFile << "#pragma once\n";
        Printer::Section(headersFile, "Includes");

        headersFile << "#include \"GameDefines.hpp\"\n";
        std::vector<UnrealObject>* packages = GCache::GetPackages();

        for (const UnrealObject& packageObj : *packages)
        {
            if (packageObj.IsValid())
            {
                headersFile << "#include \"SDK_HEADERS/" << packageObj.ValidName << "_structs.hpp\"\n";
                headersFile << "#include \"SDK_HEADERS/" << packageObj.ValidName << "_classes.hpp\"\n";
                headersFile << "#include \"SDK_HEADERS/" << packageObj.ValidName << "_parameters.hpp\"\n";
            }
        }

        headersFile << "\n";

        Printer::Footer(headersFile, false);
        headersFile.close();
    }

    void GenerateDefines()
    {
        std::ofstream definesFile(GConfig::GetOutputPath() / GConfig::GetGameNameShort() / "GameDefines.hpp");
        Printer::Header(definesFile, "GameDefines", "hpp", false);

        definesFile << "#pragma once\n";

        if (GConfig::UsingWindows())
        {
            definesFile << "#include <windows.h>\n";
        }

        definesFile << "#include <algorithm>\n";
        definesFile << "#include <locale>\n";
        definesFile << "#include <stdlib.h>\n";
        definesFile << "#include <xlocale>\n";
        definesFile << "#include <ctype.h>\n";
        definesFile << "#include <chrono>\n";
        definesFile << "#include <thread>\n";
        definesFile << "#include <vector>\n";
        definesFile << "#include <string>\n";
        definesFile << "#include <map>\n";

        if (GConfig::PrintEnumFlags())
        {
            Printer::Section(definesFile, "Flags");
            definesFile << PiecesOfCode::EEnumFlags;
        }

        Printer::Section(definesFile, "Globals");

        if (GConfig::UsingOffsets())
        {
            definesFile << "// GObjects\n";
            definesFile << "#define GObjects_Offset\t\t(uintptr_t)" << Printer::Hex(GConfig::GetGObjectOffset(), sizeof(uintptr_t)) << "\n";

            definesFile << "// GNames\n";
            definesFile << "#define GNames_Offset\t\t(uintptr_t)" << Printer::Hex(GConfig::GetGNameOffset(), sizeof(uintptr_t)) << "\n";
        }
        else
        {
            definesFile << "// GObjects\n";
            definesFile << "#define GObjects_Pattern\t\t(const uint8_t*)\"" << GConfig::GetGObjectStr() + "\"\n";
            definesFile << "#define GObjects_Mask\t\t\t(const char*)\"" << GConfig::GetGObjectMask() + "\"\n";

            definesFile << "// GNames\n";
            definesFile << "#define GNames_Pattern\t\t\t(const uint8_t*)\"" << GConfig::GetGNameStr() + "\"\n";
            definesFile << "#define GNames_Mask\t\t\t\t(const char*)\"" << GConfig::GetGNameStr() + "\"\n";

            definesFile << "// Process Event\n";
            definesFile << "#define ProcessEvent_Pattern\t(const uint8_t*)\"" << GConfig::GetProcessEventStr() << "\"\n";
            definesFile << "#define ProcessEvent_Mask\t\t(const char*)\"" << GConfig::GetProcessEventMask() << "\"\n";
        }

        Printer::Section(definesFile, "Classes");
        definesFile << PiecesOfCode::TArray_Iterator << "\n";
        definesFile << PiecesOfCode::TArray_Class << "\n";
        definesFile << PiecesOfCode::TMap_Class << "\n";

        definesFile << "extern class TArray<class UObject*>* GObjects;\n";
        definesFile << "extern class TArray<class FNameEntry*>* GNames;\n";

        Printer::Section(definesFile, "Structs");
        definesFile << PiecesOfCode::FNameEntry_Struct << "\n";
        StructGenerator::GenerateStructMembers(definesFile, EClassTypes::FNameEntry);

#ifdef UTF16
        definesFile << PiecesOfCode::FNameEntry_Struct_UTF16 << "\n";
        definesFile << PiecesOfCode::FName_Struct_UTF16 << "\n";
#else
        definesFile << PiecesOfCode::FNameEntry_Struct_UTF8 << "\n";
        definesFile << PiecesOfCode::FName_Struct_UTF8 << "\n";
#endif

#ifdef UTF16_FSTRING
        definesFile << PiecesOfCode::FString_Class_UTF16 << "\n";
#else
        definesFile << PiecesOfCode::FString_Class_UTF8 << "\n";
#endif

        definesFile << PiecesOfCode::FScriptDelegate_Struct << "\n";
        definesFile << PiecesOfCode::FPointer_Struct << "\n";
        definesFile << PiecesOfCode::FQWord_Struct << "\n";

        Printer::Footer(definesFile, false);
        definesFile.close();

        definesFile.open(GConfig::GetOutputPath() / GConfig::GetGameNameShort() / "GameDefines.cpp");
        Printer::Header(definesFile, "GameDefines", "cpp", false);

        definesFile << "#include \"GameDefines.hpp\"\n";
        Printer::Section(definesFile, "Initialize Globals");
        definesFile << "class TArray<class UObject*>* GObjects{};\n";
        definesFile << "class TArray<class FNameEntry*>* GNames{};\n\n";

        Printer::Footer(definesFile, false);
        definesFile.close();
    }

    void ProcessPackages(const std::filesystem::path& directory)
    {
        if (std::filesystem::exists(directory))
        {
            std::vector<UnrealObject>* packages = GCache::GetPackages();

            for (const UnrealObject& packageObj : *packages)
            {
                if (packageObj.IsValid())
                {
#ifndef NO_LOGGING
                    GLogger::Log("\nProcessing Package: " + packageObj.ValidName + "\n");
#endif

                    std::ofstream file;

                    // Structs
                    file.open(directory / (packageObj.ValidName + "_structs.hpp"));
                    Printer::Header(file, (packageObj.ValidName + "_structs"), "hpp", true);
                    Printer::Section(file, "Structs");
                    StructGenerator::ProcessStructs(file, packageObj.Object);
                    Printer::Footer(file, true);
                    file.close();

                    // Classes
                    file.open(directory / (packageObj.ValidName + "_classes.hpp"));
                    Printer::Header(file, (packageObj.ValidName + "_classes"), "hpp", true);
                    Printer::Section(file, "Constants");
                    ConstGenerator::ProcessConsts(file, packageObj.Object);
                    Printer::Section(file, "Enums");
                    EnumGenerator::ProcessEnums(file, packageObj.Object);
                    Printer::Section(file, "Classes");
                    ClassGenerator::ProcessClasses(file, packageObj.Object);
                    Printer::Footer(file, true);
                    file.close();

                    // Parameters
                    file.open(directory / (packageObj.ValidName + "_parameters.hpp"));
                    Printer::Header(file, (packageObj.ValidName + "_parameters"), "hpp", true);
                    Printer::Section(file, "Parameters");
                    ParameterGenerator::ProcessParameters(file, packageObj.Object);
                    Printer::Footer(file, true);
                    file.close();

                    // Functions
                    file.open(directory / (packageObj.ValidName + "_classes.cpp"));
                    Printer::Header(file, (packageObj.ValidName + "_classes"), "cpp", true);
                    Printer::Section(file, "Functions");
                    FunctionGenerator::ProcessFunctions(file, packageObj.Object);
                    Printer::Footer(file, true);
                    file.close();
                }
            }
        }
        else
        {
            Utils::MessageboxError("Failed locate the given directory, cannot generate an SDK!");
        }
    }

    void GenerateSDK()
    {
        std::filesystem::path fullDirectory = (GConfig::GetOutputPath() / GConfig::GetGameNameShort());
        std::filesystem::path headerDirectory = (fullDirectory / "SDK_HEADERS");

        std::filesystem::create_directory(GConfig::GetOutputPath());
        std::filesystem::create_directory(fullDirectory);
        std::filesystem::create_directory(headerDirectory);

        if (std::filesystem::exists(headerDirectory))
        {
            if (Initialize(true))
            {
                Utils::MessageboxInfo("SDK generation has started, do not close the game until prompted to do so!");
                std::chrono::time_point startTime = std::chrono::system_clock::now();

                ProcessPackages(headerDirectory);
                GenerateHeaders();
                GenerateDefines();
                GenerateConstants();
                GCache::ClearCache();

                std::chrono::time_point endTime = std::chrono::system_clock::now();
                std::string formattedTime = Printer::Precision(std::chrono::duration<float>(endTime - startTime).count(), 4);

#ifndef NO_LOGGING
                GLogger::Log("\n" + GConfig::GetGameNameShort() + " generated in " + formattedTime + " seconds.");
                GLogger::Close();
#endif

                Utils::MessageboxInfo("SDK generation complete, finished in " + formattedTime + " seconds!");
            }
        }
        else
        {
            Utils::MessageboxError("Failed to create the desired directory, cannot generate an SDK!");
        }
    }

    bool Initialize(bool bCreateLog)
    {
        if (!GConfig::HasOutputPath())
        {
            Utils::MessageboxError("Looks like you forgot to set an output path for the generator! Please edit the output path in \"Configuration.cpp\" and recompile.");
            return false;
        }

        if (!AreGlobalsValid())
        {
            if (GConfig::UsingOffsets())
            {
                GObjects = reinterpret_cast<TArray<UObject*>*>(Retrievers::GetBaseAddress() + GConfig::GetGObjectOffset());
                GNames = reinterpret_cast<TArray<FNameEntry*>*>(Retrievers::GetBaseAddress() + GConfig::GetGNameOffset());
            }
            else
            {
                GObjects = reinterpret_cast<TArray<UObject*>*>(Retrievers::FindPattern(GConfig::GetGObjectPattern(), GConfig::GetGObjectMask()));
                GNames = reinterpret_cast<TArray<FNameEntry*>*>(Retrievers::FindPattern(GConfig::GetGNamePattern(), GConfig::GetGNameMask()));
            }

            if (AreGlobalsValid())
            {
                // Here is where that "REGISTER_MEMBER" macro is used, these functions calculate offsets for each class member.
                // There might be a better and automated way of doing this, so maybe I'll change this in the future when I'm less lazy.

                // Structs
                FNameEntry::Register_HashNext();
                FNameEntry::Register_Index();
                FNameEntry::Register_Flags();
                FNameEntry::Register_Name();

                // Objects
                UObject::Register_VfTableObject();
                UObject::Register_ObjectInternalInteger();
                UObject::Register_Outer();
                UObject::Register_Name();
                UObject::Register_Class();
                UField::Register_Next();
#ifdef SUPERFIELDS_IN_UFIELD
                UField::Register_SuperField(); // Not needed if SuperField is in the UStruct class, leave "SUPERFIELDS_IN_UFIELD" in your "GameDefines.hpp" file!
#endif
                UEnum::Register_Names();
                UConst::Register_Value();
                UProperty::Register_ArrayDim();
                UProperty::Register_ElementSize();
                UProperty::Register_PropertyFlags();
                UProperty::Register_Offset();
#ifndef SUPERFIELDS_IN_UFIELD
                UStruct::Register_SuperField(); // Not needed if SuperField is in the UField class, comment out "SUPERFIELDS_IN_UFIELD" in your "GameDefines.hpp" file!
#endif
                UStruct::Register_Children();
                UStruct::Register_PropertySize();
                UFunction::Register_FunctionFlags();
                UFunction::Register_iNative();
                UStructProperty::Register_Struct();
                UObjectProperty::Register_PropertyClass();
                UClassProperty::Register_MetaClass();
                UMapProperty::Register_Key();
                UMapProperty::Register_Value();
                UInterfaceProperty::Register_InterfaceClass();
                UByteProperty::Register_Enum();
                UBoolProperty::Register_BitMask();
                UArrayProperty::Register_Inner();

#ifndef NO_LOGGING
                std::chrono::time_point startTime = std::chrono::system_clock::now();
#endif

                GCache::Initialize(); // Cache all object instances needed for generation.

#ifndef NO_LOGGING
                std::chrono::time_point endTime = std::chrono::system_clock::now();
                std::string formattedTime = Printer::Precision(std::chrono::duration<float>(endTime - startTime).count(), 4);

                if (bCreateLog && GLogger::Open())
                {
                    GLogger::Log("Base: " + Printer::Hex(Retrievers::GetBaseAddress(), sizeof(uintptr_t)));
                    GLogger::Log("GObjects: " + Printer::Hex(GObjects));
                    GLogger::Log("GNames: " + Printer::Hex(GNames));
                    GLogger::Log("\n" + GConfig::GetGameNameShort() + " objects cached in " + formattedTime + " seconds.");
                }
#endif
            }
            else
            {
                Utils::MessageboxError("Failed to validate GObject & GNames, please make sure you have them configured properly in \"Configuration.cpp\"!");
                return false;
            }
        }

        if (AreGlobalsValid())
        {
#ifndef NO_LOGGING
            if (bCreateLog && GLogger::Open()) // Will return false if the file is already open.
            {
                GLogger::Log("Base: " + Printer::Hex(Retrievers::GetBaseAddress(), sizeof(uintptr_t)));
                GLogger::Log("GObjects: " + Printer::Hex(GObjects));
                GLogger::Log("GNames: " + Printer::Hex(GNames));
            }
#endif
            return true;
        }

        return false;
    }

    void DumpInstances(bool bNames, bool bObjects)
    {
        if (Initialize(false))
        {
            if (bNames)
            {
                DumpGNames();
            }

            if (bObjects)
            {
                DumpGObjects();
            }

            Utils::MessageboxInfo("Finished dumping instances!");
        }
    }

    void DumpGNames()
    {
        if (Initialize(false) && AreGlobalsValid())
        {
            std::filesystem::path fullDirectory = (GConfig::GetOutputPath() / GConfig::GetGameNameShort());
            std::filesystem::create_directory(GConfig::GetOutputPath());            
            std::filesystem::create_directory(fullDirectory);

            if (std::filesystem::exists(fullDirectory))
            {
                std::ofstream file(fullDirectory / "NameDump.txt");
                file << "Base: " << Printer::Hex(Retrievers::GetBaseAddress(), sizeof(uintptr_t)) << std::endl;
                file << "GNames: " << Printer::Hex(GNames) << std::endl;
                file << "Offset: " << Printer::Hex(Retrievers::GetOffset(GNames), sizeof(uintptr_t)) << "\n" << std::endl;

                for (int32_t i = 0; i < FName::Names()->size(); i++)
                {
                    FNameEntry* nameEntry = FName::Names()->at(i);

                    if (nameEntry)
                    {
                        std::string name = nameEntry->ToString();

                        if (!name.empty())
                        {
                            file << "Name[";
                            Printer::FillRight(file, '0', 6);
                            file << nameEntry->GetIndex() << "] " << name << " ";

                            if (name.length() < INSTANCE_DUMP_SPACING)
                            {
                                Printer::FillRight(file, ' ', (INSTANCE_DUMP_SPACING - name.length()));
                            }

                            file << Printer::Hex(nameEntry) << "\n";
                        }
                    }
                }

                file.close();
            }
        }
    }

    void DumpGObjects()
    {
        if (Initialize(false) && AreGlobalsValid())
        {
            std::filesystem::path fullDirectory = (GConfig::GetOutputPath() / GConfig::GetGameNameShort());
            std::filesystem::create_directory(GConfig::GetOutputPath());
            std::filesystem::create_directory(fullDirectory);

            if (std::filesystem::exists(fullDirectory))
            {
                std::ofstream file(fullDirectory / "ObjectDump.txt");
                file << "Base: " << Printer::Hex(Retrievers::GetBaseAddress(), sizeof(uintptr_t)) << std::endl;
                file << "GObjects: " << Printer::Hex(GObjects) << std::endl;
                file << "Offset: " << Printer::Hex(Retrievers::GetOffset(GObjects), sizeof(uintptr_t)) << "\n" << std::endl;

                for (int32_t i = 0; i < (UObject::GObjObjects()->size() - 1); i++)
                {
                    UObject* uObject = UObject::GObjObjects()->at(i);

                    if (uObject)
                    {
                        std::string name = uObject->GetFullName();

                        if (!name.empty())
                        {
                            file << "UObject[";
                            Printer::FillRight(file, '0', 6);
                            file << uObject->ObjectInternalInteger << "] " << name << " ";

                            if (name.length() < INSTANCE_DUMP_SPACING)
                            {
                                Printer::FillRight(file, ' ', (INSTANCE_DUMP_SPACING - name.length()));
                            }

                            file << Printer::Hex(uObject) << "\n";
                        }
                    }
                }

                file.close();
            }
        }
    }

    bool AreGObjectsValid()
    {
        if (GObjects
            && !UObject::GObjObjects()->empty()
            && (UObject::GObjObjects()->capacity() > UObject::GObjObjects()->size()))
        {
            return true;
        }

        return false;
    }

    bool AreGNamesValid()
    {
        if (GNames
            && !FName::Names()->empty()
            && (FName::Names()->capacity() > FName::Names()->size()))
        {
            return true;
        }

        return false;
    }

    bool AreGlobalsValid()
    {
        static bool globalsValid = false;

        if (!globalsValid)
        {
            globalsValid = (AreGObjectsValid() && AreGNamesValid());
        }

        return globalsValid;
    }
}

void OnAttach(HMODULE hModule)
{
    DisableThreadLibraryCalls(hModule);
    Generator::GenerateSDK();
    Generator::DumpInstances(true, true);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(OnAttach), nullptr, 0, nullptr);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}