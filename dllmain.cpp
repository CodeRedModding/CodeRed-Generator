#include "dllmain.hpp"

UnrealObject::UnrealObject() : Type(EClassTypes::Unknown), Object(nullptr), Package(nullptr) {}

UnrealObject::UnrealObject(class UObject* uObject) : Type(EClassTypes::Unknown), Object(nullptr), Package(nullptr)
{
    Assign(uObject);
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

bool UnrealObject::Assign(class UObject* uObject)
{
    if (uObject)
    {
        UObject* packageObj = uObject->GetPackageObj();

        if (packageObj)
        {
            Object = uObject;
            Package = packageObj;
            FullName = Utils::CreateValidName(Object->GetFullName());
            ValidName = Utils::CreateValidName(Object->GetName());
            return AssignType();
        }
    }

    return false;
}

bool UnrealObject::AssignType()
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
            if (ValidName.find("Default__") == std::string::npos)
            {
                Type = EClassTypes::UStruct;
            }
        }
        else if (Object->IsA<UClass>())
        {
            if (ValidName.find("Default__") == std::string::npos)
            {
                Type = EClassTypes::UClass;
            }
        }
        else if (Object->IsA<UFunction>())
        {
            Type = EClassTypes::UFunction;
        }
    }

    return (Type != EClassTypes::Unknown);
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

std::vector <std::pair<class UObject*, std::string>>* GCache::GetIncludes()
{
    return &m_includes;
}

std::map<std::string, class UObject*>* GCache::GetConstants()
{
    return &m_constants;
}

std::vector<class UObject*>* GCache::GetPackages()
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

            if (std::find(m_packages.begin(), m_packages.end(), unrealObj.Package) == m_packages.end())
            {
                m_packages.push_back(unrealObj.Package);
                m_includes.push_back({ unrealObj.Package, Utils::CreateValidName(unrealObj.Package->GetName()) });
            }
        }

        CacheConstant(unrealObj);
    }
}

void GCache::CacheConstant(UnrealObject& unrealObj)
{
    if (GConfig::UsingConstants() && unrealObj.IsValid())
    {
        if ((unrealObj.Type == EClassTypes::UClass) || (unrealObj.Type == EClassTypes::UFunction))
        {
            std::string fullName = ("IDX_" + Printer::ToUpper(unrealObj.FullName));

            for (char& c : fullName)
            {
                if ((c == '.') || (c == ' '))
                {
                    c = '_';
                }
            }

            if (!m_constants.contains(fullName))
            {
                m_constants[fullName] = unrealObj.Object;
            }
        }
    }
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
        MessageboxExt(message, (MB_OK | MB_ICONINFORMATION));
    }

    void MessageboxWarn(const std::string& message)
    {
        MessageboxExt(message, (MB_OK | MB_ICONWARNING));
    }

    void MessageboxError(const std::string& message)
    {
        MessageboxExt(message, (MB_OK | MB_ICONERROR));
    }

    bool SortPropertyPair(const std::pair<class UProperty*, std::string>& pairA, const std::pair<class UProperty*, std::string>& pairB)
    {
        return SortProperty(pairA.first, pairB.first);
    }

    bool SortProperty(class UProperty* propertyA, class UProperty* propertyB)
    {
        if ((propertyA->Offset == propertyB->Offset) && propertyA->IsA<UBoolProperty>() && propertyB->IsA<UBoolProperty>())
        {
            return (static_cast<UBoolProperty*>(propertyA)->BitMask < static_cast<UBoolProperty*>(propertyB)->BitMask);
        }

        return (propertyA->Offset < propertyB->Offset);
    }

    bool CantMemcpy(EPropertyTypes propertyType)
    {
        return (propertyType == EPropertyTypes::Bool);
    }

    bool IsStructProperty(EPropertyTypes propertyType)
    {
        return ((propertyType == EPropertyTypes::TArray)
            || (propertyType == EPropertyTypes::TMap)
            || (propertyType == EPropertyTypes::FName)
            || (propertyType == EPropertyTypes::FString)
            || (propertyType == EPropertyTypes::FScriptDelegate)
            || (propertyType == EPropertyTypes::FPointer)
            || (propertyType == EPropertyTypes::FStruct));
    }

    bool IsBitField(int32_t arrayDim)
    {
        return (arrayDim && (arrayDim < 4));
    }

    std::string CreateValidName(std::string name)
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

        return name;
    }

    std::string CreateUniqueName(class UClass* uClass)
    {
        UClass* superClass = static_cast<UClass*>(uClass->SuperField);
        std::string classNameCPP = Printer::ToUpper(CreateValidName(uClass->GetNameCPP()));
        std::string uniqueName = classNameCPP;

        if (superClass)
        {
            uniqueName += ("_" + Printer::ToUpper(CreateValidName(superClass->GetNameCPP())));
        }

        return uniqueName;
    }

    std::string CreateUniqueName(class UFunction* function, class UClass* uClass)
    {
        std::string classNameCPP = Printer::ToUpper(CreateValidName(uClass->GetNameCPP()));
        std::string functionName = Printer::ToUpper(CreateValidName(function->GetName()));
        return (classNameCPP + "_" + functionName);
    }

    void MakeWinSafe(std::string& functionName)
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

namespace Retrievers
{
    void GetAllPropertyFlags(std::ostringstream& stream, uint64_t propertyFlags)
    {
        bool first = true;

        if (propertyFlags & EPropertyFlags::CPF_Edit) { stream << (first ? "(" : " | ") << "CPF_Edit";							                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Const) { stream << (first ? "(" : " | ") << "CPF_Const";						                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Input) { stream << (first ? "(" : " | ") << "CPF_Input";						                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ExportObject) { stream << (first ? "(" : " | ") << "CPF_ExportObject";			                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_OptionalParm) { stream << (first ? "(" : " | ") << "CPF_OptionalParm";			                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Net) { stream << (first ? "(" : " | ") << "CPF_Net";							                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditConstArray) { stream << (first ? "(" : " | ") << "CPF_EditConstArray";		                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Parm) { stream << (first ? "(" : " | ") << "CPF_Parm";							                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_OutParm) { stream << (first ? "(" : " | ") << "CPF_OutParm";					                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_SkipParm) { stream << (first ? "(" : " | ") << "CPF_SkipParm";					                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ReturnParm) { stream << (first ? "(" : " | ") << "CPF_ReturnParm";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_CoerceParm) { stream << (first ? "(" : " | ") << "CPF_CoerceParm";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Native) { stream << (first ? "(" : " | ") << "CPF_Native";						                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Transient) { stream << (first ? "(" : " | ") << "CPF_Transient";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Config) { stream << (first ? "(" : " | ") << "CPF_Config";						                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Localized) { stream << (first ? "(" : " | ") << "CPF_Localized";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Travel) { stream << (first ? "(" : " | ") << "CPF_Travel";						                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditConst) { stream << (first ? "(" : " | ") << "CPF_EditConst";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_GlobalConfig) { stream << (first ? "(" : " | ") << "CPF_GlobalConfig";			                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Component) { stream << (first ? "(" : " | ") << "CPF_Component";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NeedCtorLink) { stream << (first ? "(" : " | ") << "CPF_NeedCtorLink";			                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NoExport) { stream << (first ? "(" : " | ") << "CPF_NoExport";					                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NoClear) { stream << (first ? "(" : " | ") << "CPF_NoClear";					                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditInline) { stream << (first ? "(" : " | ") << "CPF_EditInline";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EdFindable) { stream << (first ? "(" : " | ") << "CPF_EdFindable";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditInlineUse) { stream << (first ? "(" : " | ") << "CPF_EditInlineUse";		                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Deprecated) { stream << (first ? " " : " | ") << "CPF_Deprecated";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditInlineNotify) { stream << (first ? "(" : " | ") << "CPF_EditInlineNotify";	                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_RepNotify) { stream << (first ? "(" : " | ") << "CPF_RepNotify";				                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Interp) { stream << (first ? "(" : " | ") << "CPF_Interp";						                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NonTransactional) { stream << (first ? "(" : " | ") << "CPF_NonTransactional";	                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_EditorOnly) { stream << (first ? "(" : " | ") << "CPF_EditorOnly";	                                            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NoDestructor) { stream << (first ? "(" : " | ") << "CPF_NoDestructor";	                                        first = false; }
        if (propertyFlags & EPropertyFlags::CPF_AutoWeak) { stream << (first ? "(" : " | ") << "CPF_AutoWeak";	                                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ContainsInstancedReference) { stream << (first ? "(" : " | ") << "CPF_ContainsInstancedReference";              first = false; }
        if (propertyFlags & EPropertyFlags::CPF_SimpleDisplay) { stream << (first ? "(" : " | ") << "CPF_SimpleDisplay";	                                    first = false; }
        if (propertyFlags & EPropertyFlags::CPF_AdvancedDisplay) { stream << (first ? "(" : " | ") << "CPF_AdvancedDisplay";	                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_Protected) { stream << (first ? "(" : " | ") << "CPF_Protected";	                                            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_BlueprintCallable) { stream << (first ? "(" : " | ") << "CPF_BlueprintCallable";                                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_BlueprintAuthorityOnly) { stream << (first ? "(" : " | ") << "CPF_BlueprintAuthorityOnly";                      first = false; }
        if (propertyFlags & EPropertyFlags::CPF_TextExportTransient) { stream << (first ? "(" : " | ") << "CPF_TextExportTransient";                            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NonPIEDuplicateTransient) { stream << (first ? "(" : " | ") << "CPF_NonPIEDuplicateTransient";	                first = false; }
        if (propertyFlags & EPropertyFlags::CPF_ExposeOnSpawn) { stream << (first ? "(" : " | ") << "CPF_ExposeOnSpawn";                                        first = false; }
        if (propertyFlags & EPropertyFlags::CPF_PersistentInstance) { stream << (first ? "(" : " | ") << "CPF_PersistentInstance";                              first = false; }
        if (propertyFlags & EPropertyFlags::CPF_UObjectWrapper) { stream << (first ? "(" : " | ") << "CPF_UObjectWrapper";	                                    first = false; }
        if (propertyFlags & EPropertyFlags::CPF_HasGetValueTypeHash) { stream << (first ? "(" : " | ") << "CPF_HasGetValueTypeHash";                            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NativeAccessSpecifierPublic) { stream << (first ? "(" : " | ") << "CPF_NativeAccessSpecifierPublic";            first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NativeAccessSpecifierProtected) { stream << (first ? "(" : " | ") << "CPF_NativeAccessSpecifierProtected";      first = false; }
        if (propertyFlags & EPropertyFlags::CPF_NativeAccessSpecifierPrivate) { stream << (first ? "(" : " | ") << "CPF_NativeAccessSpecifierPrivate";          first = false; }
        if (propertyFlags & EPropertyFlags::CPF_SkipSerialization) { stream << (first ? "(" : " | ") << "CPF_SkipSerialization";                                first = false; }

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

    EPropertyTypes GetPropertyTypeInternal(class UProperty* uProperty, std::string& outPropertyType, bool bIgnoreEnum, bool bDescription, bool bIsBitField)
    {
        if (uProperty)
        {
            if (uProperty->IsA<UStructProperty>())
            {
                UStructProperty* structProperty = static_cast<UStructProperty*>(uProperty);

                if (structProperty && structProperty->Struct)
                {
                    uint32_t propertyCount = UObject::CountObject<UScriptStruct>(structProperty->Struct->GetName());

                    if (propertyCount > 1)
                    {
                        if (structProperty->Struct->Outer)
                        {
                            outPropertyType = ("struct " + Utils::CreateValidName(structProperty->Struct->Outer->GetNameCPP()) + "_" + Utils::CreateValidName(structProperty->Struct->GetNameCPP()));
                        }
                        else
                        {
                            outPropertyType = ("struct " + Utils::CreateValidName(structProperty->Struct->GetNameCPP()) + "_Outer" + Printer::Decimal(propertyCount, EWidthTypes::BYTE));
                        }
                    }
                    else
                    {
                        outPropertyType = ("struct " + Utils::CreateValidName(structProperty->Struct->GetNameCPP()));
                    }

                    if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                    {
                        outPropertyType = ("const " + outPropertyType + "&");
                    }

                    return EPropertyTypes::FStruct;
                }
            }
            else if (uProperty->IsA<UStrProperty>())
            {
                outPropertyType = "class FString";

                if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                {
                    outPropertyType = ("const " + outPropertyType + "&");
                }

                return EPropertyTypes::FString;
            }
            else if (uProperty->IsA<UQWordProperty>())
            {
                outPropertyType = "uint64_t";
                return EPropertyTypes::UInt64;
            }
            else if (uProperty->IsA<UObjectProperty>())
            {
                UObjectProperty* objectProperty = static_cast<UObjectProperty*>(uProperty);

                if (objectProperty && objectProperty->PropertyClass)
                {
                    outPropertyType = ("class " + Utils::CreateValidName(objectProperty->PropertyClass->GetNameCPP()) + "*");
                    return EPropertyTypes::UPointer;
                }
            }
            else if (uProperty->IsA<UClassProperty>())
            {
                UClassProperty* classProperty = static_cast<UClassProperty*>(uProperty);

                if (classProperty && classProperty->MetaClass)
                {
                    outPropertyType = ("class " + Utils::CreateValidName(classProperty->MetaClass->GetNameCPP()) + "*");
                    return EPropertyTypes::UPointer;
                }
            }
            else if (uProperty->IsA<UNameProperty>())
            {
                outPropertyType = "class FName";

                if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                {
                    outPropertyType = ("const " + outPropertyType + "&");
                }

                return EPropertyTypes::FName;
            }
            else if (uProperty->IsA<UMapProperty>())
            {
                UMapProperty* mapProperty = static_cast<UMapProperty*>(uProperty);
                std::string mapKey, mapValue;

                if (mapProperty && mapProperty->Key && mapProperty->Value)
                {
                    if (GetPropertyTypeInternal(mapProperty->Key, mapKey, bIgnoreEnum, false, bIsBitField) != EPropertyTypes::Unknown && GetPropertyTypeInternal(mapProperty->Value, mapValue, bIgnoreEnum, bDescription, bIsBitField) != EPropertyTypes::Unknown)
                    {
                        outPropertyType = ("class TMap<" + mapKey + ", " + mapValue + ">");

                        if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                        {
                            outPropertyType = ("const " + outPropertyType + "&");
                        }

                        return EPropertyTypes::TMap;
                    }
                }
            }
            else if (uProperty->IsA<UIntProperty>())
            {
                outPropertyType = "int32_t";
                return EPropertyTypes::Int32;
            }
            else if (uProperty->IsA<UInterfaceProperty>())
            {
                UInterfaceProperty* interfaceProperty = static_cast<UInterfaceProperty*>(uProperty);

                if (interfaceProperty && interfaceProperty->InterfaceClass)
                {
                    outPropertyType = ("class " + Utils::CreateValidName(interfaceProperty->InterfaceClass->GetNameCPP()) + "*");
                    return EPropertyTypes::UPointer;
                }
            }
            else if (uProperty->IsA<UFloatProperty>())
            {
                outPropertyType = "float";
                return EPropertyTypes::Float;
            }
            else if (uProperty->IsA<UDelegateProperty>())
            {
                outPropertyType = "struct FScriptDelegate";

                if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                {
                    outPropertyType = ("const " + outPropertyType + "&");
                }

                return EPropertyTypes::FScriptDelegate;
            }
            else if (uProperty->IsA<UByteProperty>())
            {
                if (!bIgnoreEnum && GConfig::UsingEnumClasses())
                {
                    UByteProperty* byteProperty = static_cast<UByteProperty*>(uProperty);

                    if (byteProperty && byteProperty->Enum)
                    {
                        outPropertyType = EnumGenerator::GenerateEnumName(byteProperty->Enum);
                        return EPropertyTypes::UInt8;
                    }
                }

                outPropertyType = "uint8_t";
                return EPropertyTypes::UInt8;
            }
            else if (uProperty->IsA<UBoolProperty>())
            {
                if (bIsBitField)
                {
                    outPropertyType = "uint32_t";
                    return EPropertyTypes::UInt32;
                }

                outPropertyType = "bool";
                return EPropertyTypes::Bool;
            }
            else if (uProperty->IsA<UArrayProperty>())
            {
                UArrayProperty* arrayProperty = static_cast<UArrayProperty*>(uProperty);

                if (arrayProperty && arrayProperty->Inner)
                {
                    std::string innerProperty;

                    if (GetPropertyTypeInternal(arrayProperty->Inner, innerProperty, bIgnoreEnum, false, bIsBitField) != EPropertyTypes::Unknown)
                    {
                        outPropertyType = ("class TArray<" + innerProperty + ">");

                        if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                        {
                            outPropertyType = ("const " + outPropertyType + "&");
                        }

                        return EPropertyTypes::TArray;
                    }
                }
            }
        }

        return EPropertyTypes::Unknown;
    }

    EPropertyTypes GetPropertyTypeDesc(class UProperty* uProperty, std::string& outPropertyType, bool bIsBitField)
    {
        return GetPropertyTypeInternal(uProperty, outPropertyType, false, true, bIsBitField);
    }

    EPropertyTypes GetPropertyType(class UProperty* uProperty, std::string& outPropertyType, bool bIsBitField)
    {
        return GetPropertyTypeInternal(uProperty, outPropertyType, false, false, bIsBitField);
    }

    size_t GetPropertySize(UProperty* uProperty, bool bIsBitField)
    {
        if (uProperty)
        {
            if (uProperty->IsA<UStructProperty>())
            {
                return uProperty->ElementSize;
            }
            else if (uProperty->IsA<UStrProperty>())
            {
                return sizeof(FString);
            }
            else if (uProperty->IsA<UQWordProperty>())
            {
                return sizeof(uint64_t);
            }
            else if (uProperty->IsA<UObjectProperty>())
            {
                return sizeof(uintptr_t);
            }
            else if (uProperty->IsA<UClassProperty>())
            {
                return sizeof(uintptr_t);
            }
            else if (uProperty->IsA<UNameProperty>())
            {
                return sizeof(FName);
            }
            else if (uProperty->IsA<UMapProperty>())
            {
                return uProperty->ElementSize;
            }
            else if (uProperty->IsA<UIntProperty>())
            {
                return sizeof(int32_t);
            }
            else if (uProperty->IsA<UInterfaceProperty>())
            {
                return sizeof(uintptr_t);
            }
            else if (uProperty->IsA<UFloatProperty>())
            {
                return sizeof(float);
            }
            else if (uProperty->IsA<UDelegateProperty>())
            {
                return uProperty->ElementSize;
            }
            else if (uProperty->IsA<UByteProperty>())
            {
                if (bIsBitField)
                {
                    return uProperty->ElementSize;
                }

                return sizeof(uint8_t);
            }
            else if (uProperty->IsA<UBoolProperty>())
            {
                return uProperty->ElementSize;
            }
            else if (uProperty->IsA<UArrayProperty>())
            {
                return sizeof(TArray<uintptr_t>);
            }
        }

        return 0;
    }

    uintptr_t GetEntryPoint()
    {
        return reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
    }

    uintptr_t GetOffset(void* pointer)
    {
        uintptr_t entryPoint = GetEntryPoint();
        uintptr_t address = reinterpret_cast<uintptr_t>(pointer);

        if (address > entryPoint)
        {
            return (address - entryPoint);
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
            if (Generator::LogFile.is_open())
            {
                std::string propertyTitle = "Const: ";
                Generator::LogFile << propertyTitle;
                Printer::FillRight(Generator::LogFile, ' ', unrealObj.ValidName.size());
                Generator::LogFile << unrealObj.ValidName;
                Printer::FillRight(Generator::LogFile, ' ', (75 - (unrealObj.ValidName.size() + propertyTitle.size())));
                Generator::LogFile << " - Instance: " << Printer::Hex(unrealObj.Object) << std::endl;
            }
#endif

            if (!unrealObj.ValidName.empty())
            {
                stream << "#define " << unrealObj.ValidName;
                Printer::FillLeft(stream, ' ', (GConfig::GetConstSpacing() - unrealObj.ValidName.length()));
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
            if (Generator::LogFile.is_open())
            {
                std::string propertyTitle = "Enum: ";
                Generator::LogFile << propertyTitle;
                Printer::FillRight(Generator::LogFile, ' ', unrealObj.ValidName.size());
                Generator::LogFile << unrealObj.ValidName;
                Printer::FillRight(Generator::LogFile, ' ', (75 - (unrealObj.ValidName.size() + propertyTitle.size())));
                Generator::LogFile << " - Instance: " << Printer::Hex(unrealObj.Object) << std::endl;
            }
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
                    propertyStream << propertyName << Printer::Decimal(enumValues[propertyName], EWidthTypes::BYTE);
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
    static std::vector<std::string> m_generatedStructs;

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
            std::map<uintptr_t, Member*> members = Member::GetRegistered(structType);

            if (!members.empty())
            {
                lastOffset = startOffset;
                int32_t unknownDataIndex = 0;

                for (const auto& member : members)
                {
                    if ((lastOffset + member.second->Size) < member.second->Offset)
                    {
                        missedOffset = (member.second->Offset - lastOffset);

                        if (missedOffset >= GConfig::GetGameAlignment())
                        {
                            propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                            propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                            structStream << propertyStream.str() << " // " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                            structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") DYNAMIC FIELD PADDING\n";
                            Printer::Empty(propertyStream);

                            unknownDataIndex++;
                        }
                    }

                    structStream << "\t" << member.second->Label << " // " << Printer::Hex(member.second->Offset, EWidthTypes::SIZE);
                    structStream << " (" << Printer::Hex(member.second->Size, EWidthTypes::SIZE) << ")\n";
                    lastOffset = (member.second->Offset + member.second->Size);
                }

                if ((structType != EClassTypes::FNameEntry) && (lastOffset < localSize))
                {
                    missedOffset = (localSize - lastOffset);

                    if (missedOffset >= GConfig::GetGameAlignment())
                    {
                        propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                        propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                        structStream << propertyStream.str() << " // " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                        structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") DYNAMIC FIELD PADDING\n";
                    }
                }
            }
            else
            {
#ifndef NO_LOGGING
                Generator::LogFile << "Error: No registered members found for struct type \"" << Member::GetName(structType) << "\"\n";
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
            if (Generator::LogFile.is_open())
            {
                std::string propertyTitle = "ScriptStruct: ";
                Generator::LogFile << propertyTitle;
                Printer::FillRight(Generator::LogFile, ' ', unrealObj.ValidName.size());
                Generator::LogFile << unrealObj.ValidName;
                Printer::FillRight(Generator::LogFile, ' ', (75 - (unrealObj.ValidName.size() + propertyTitle.size())));
                Generator::LogFile << " - Instance: " << Printer::Hex(unrealObj.Object) << std::endl;
            }
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
                uint32_t structCount = UObject::CountObject<UScriptStruct>(unrealObj.ValidName);

                if (superField && superField != scriptStruct)
                {
                    size = scriptStruct->PropertySize - superField->PropertySize;
                    lastOffset = superField->PropertySize;

                    std::string fieldName = Utils::CreateValidName(superField->GetName());
                    std::string fieldNameCPP = Utils::CreateValidName(superField->GetNameCPP());
                    std::string fieldOuterNameCPP = Utils::CreateValidName(superField->Outer->GetNameCPP());

                    uint32_t fieldStructCount = UObject::CountObject<UScriptStruct>(fieldName);

                    structStream << "// " << Printer::Hex(size, EWidthTypes::SIZE);
                    structStream << " (" << Printer::Hex(superField->PropertySize, EWidthTypes::SIZE);
                    structStream << " - " << Printer::Hex(scriptStruct->PropertySize, EWidthTypes::SIZE);
                    structStream << ")\n";

                    if (structCount > 1)
                    {
                        structStream << "struct " << structOuterNameCPP << "_" << structNameCPP << " : ";
                    }
                    else
                    {
                        structStream << "struct " << structNameCPP << " : ";
                    }

                    if (fieldStructCount > 1)
                    {
                        structStream << fieldOuterNameCPP << "_" << fieldNameCPP << "\n";
                    }
                    else
                    {
                        structStream << fieldNameCPP << "\n";
                    }
                }
                else
                {
                    size = scriptStruct->PropertySize;
                    structStream << "// " << Printer::Hex(size, EWidthTypes::SIZE) << "\n";

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
                std::vector<UProperty*> structProperties;

                for (UProperty* uProperty = static_cast<UProperty*>(scriptStruct->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                {
                    if ((uProperty->ElementSize > 0) && !uProperty->IsA<UScriptStruct>())
                    {
                        structProperties.push_back(uProperty);
                    }
                }

                std::sort(structProperties.begin(), structProperties.end(), Utils::SortProperty);
                std::map<std::string, uint32_t> propertyNameMap;
                uint32_t unknownDataIndex = 0;

                for (UProperty* uProperty : structProperties)
                {
                    if (uProperty)
                    {
                        if (lastOffset < uProperty->Offset)
                        {
                            missedOffset = (uProperty->Offset - lastOffset);

                            if (missedOffset >= GConfig::GetGameAlignment())
                            {
                                propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                                propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                                structStream << "\t";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << "uint8_t ";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << propertyStream.str() << "\t\t// " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                                structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") MISSED OFFSET\n";
                                Printer::Empty(propertyStream);

                                unknownDataIndex++;
                            }
                        }

                        std::string propertyType;

                        if (Retrievers::GetPropertyTypeInternal(uProperty, propertyType, true, false, true) != EPropertyTypes::Unknown)
                        {
                            size_t correctElementSize = Retrievers::GetPropertySize(uProperty);
                            std::string propertyName = Utils::CreateValidName(uProperty->GetName());

                            if (propertyNameMap.count(propertyName) == 0)
                            {
                                propertyNameMap[propertyName] = 1;
                                propertyStream << propertyName;
                            }
                            else
                            {
                                propertyStream << propertyName << Printer::Decimal(propertyNameMap[propertyName], EWidthTypes::BYTE);
                                propertyNameMap[propertyName]++;
                            }

                            if (uProperty->ArrayDim > 1)
                            {
                                if (!uProperty->IsA<UInterfaceProperty>())
                                {
                                    propertyStream << "[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "]";
                                }

                                correctElementSize *= uProperty->ArrayDim;
                            }

                            if (uProperty->IsA<UBoolProperty>())
                            {
                                propertyStream << " : 1";
                            }

                            Retrievers::GetAllPropertyFlags(flagStream, uProperty->PropertyFlags);
                            int32_t offsetError = ((uProperty->ElementSize * uProperty->ArrayDim) - (correctElementSize * uProperty->ArrayDim));

                            if (uProperty->IsA<UInterfaceProperty>())
                            {
                                size_t interfaceSize = Retrievers::GetPropertySize(uProperty);

                                if (offsetError == interfaceSize)
                                {
                                    offsetError -= interfaceSize;
                                }

                                structStream << "\t";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << propertyType << " " << propertyStream.str();

                                if (uProperty->ArrayDim > 1)
                                {
                                    structStream << "_Object[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "];";
                                }
                                else
                                {
                                    structStream << "_Object;";
                                }

                                Printer::FillRight(structStream, ' ', GConfig::GetStructSpacing() - (propertyStream.str().size() + 8));
                                structStream << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                structStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ")";
                                structStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";
                                structStream << flagStream.str() << "\n";

                                structStream << "\t";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << propertyType << " " << propertyStream.str();

                                if (uProperty->ArrayDim > 1)
                                {
                                    structStream << "_Interface[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "];";
                                }
                                else
                                {
                                    structStream << "_Interface;";
                                }

                                Printer::FillRight(structStream, ' ', GConfig::GetStructSpacing() - (propertyStream.str().size() + 11));
                                structStream << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                structStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ")";
                                structStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";
                                Printer::FillLeft(structStream, ' ', static_cast<size_t>(EWidthTypes::WIDTH_FIELD));
                            }
                            else
                            {
                                structStream << "\t";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << propertyType << " " << propertyStream.str() << ";";

                                Printer::FillRight(structStream, ' ', GConfig::GetStructSpacing() - (propertyStream.str().size() + 1));
                                structStream << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                structStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ")";
                                structStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";

                                if (uProperty->IsA<UBoolProperty>())
                                {
                                    structStream << "[" << Printer::Hex(static_cast<UBoolProperty*>(uProperty)->BitMask, EWidthTypes::BITMASK) << "] ";
                                }
                                else
                                {
                                    Printer::FillLeft(structStream, ' ', static_cast<size_t>(EWidthTypes::WIDTH_FIELD));
                                }
                            }

                            structStream << flagStream.str() << "\n";

                            if (offsetError > 0)
                            {
                                propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                                propertyStream << "[" << Printer::Hex(offsetError, EWidthTypes::NONE) << "];";

                                structStream << "\t";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << "uint8_t ";
                                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                                structStream << propertyStream.str() << "// " << Printer::Hex(uProperty->Offset + offsetError, EWidthTypes::SIZE);
                                Printer::Empty(propertyStream);

                                structStream << " (" << Printer::Hex(offsetError, EWidthTypes::SIZE) << ") FIX WRONG SIZE OF PREVIOUS PROPERTY";
                                structStream << " [Original: " << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE);
                                structStream << ", Missing: " << Printer::Hex(offsetError, EWidthTypes::SIZE) << "]\n";

                                unknownDataIndex++;
                            }

                            Printer::Empty(propertyStream);
                            Printer::Empty(flagStream);
                        }
                        else
                        {
                            propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                            propertyStream << "[" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::NONE) << "];";

                            structStream << "\t";
                            Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                            structStream << "uint8_t ";
                            Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                            structStream << propertyStream.str() << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                            structStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE);
                            structStream << ") UNKNOWN PROPERTY: " << uProperty->GetFullName() << "\n";
                            Printer::Empty(propertyStream);

                            unknownDataIndex++;
                        }

                        lastOffset = (uProperty->Offset + (uProperty->ElementSize * uProperty->ArrayDim));
                    }
                }

                if (lastOffset < scriptStruct->PropertySize)
                {
                    missedOffset = (scriptStruct->PropertySize - lastOffset);

                    if (missedOffset >= GConfig::GetGameAlignment())
                    {
                        propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                        propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];\t";

                        Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                        structStream << "uint8_t ";
                        Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                        structStream << propertyStream.str() << "\t\t// " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                        structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") MISSED OFFSET\n";
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
#ifndef NO_LOGGING
                        if (Generator::LogFile.is_open())
                        {
                            Generator::LogFile << "Info: EXTRA PADDING DETECTED!\n";
                            Generator::LogFile << "Info: LAST PROPERTY OFFSET " << Printer::Hex(lastOffset, 1) << "\n";
                            Generator::LogFile << "Info: PROVIDED STRUCT SIZE " << Printer::Hex(scriptStruct->PropertySize, 1) << "\n";
                            Generator::LogFile << "Info: CALCULATED STRUCT SIZE " << Printer::Hex(actualSize, 1) << "\n";
                            Generator::LogFile << "Info: MIN ALIGNMENT " << Printer::Hex(scriptStruct->MinAlignment, 1) << "\n";
                            Generator::LogFile << "Info: ADDED PADDING " << Printer::Hex(padding, 1) << std::endl;
                        }
#endif
                        structStream << "\tuint8_t MinClassPadding[" << Printer::Hex(padding, EWidthTypes::BYTE) << "];\n";
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
        if (unrealObj.IsValid())
        {
            UScriptStruct* scriptStruct = static_cast<UScriptStruct*>(unrealObj.Object);

            if (std::find(m_generatedStructs.begin(), m_generatedStructs.end(), unrealObj.FullName) == m_generatedStructs.end())
            {
                scriptStruct = static_cast<UScriptStruct*>(GCache::GetLargestStruct(unrealObj.FullName).Object);

                if (scriptStruct->SuperField
                    && (scriptStruct->SuperField != scriptStruct)
                    && (std::find(m_generatedStructs.begin(), m_generatedStructs.end(), static_cast<UScriptStruct*>(scriptStruct->SuperField)->GetFullName()) == m_generatedStructs.end()))
                {
                    GenerateStructPre(stream, static_cast<UScriptStruct*>(scriptStruct->SuperField));
                }

                for (UProperty* structChild = static_cast<UProperty*>(scriptStruct->Children); structChild; structChild = static_cast<UProperty*>(structChild->Next))
                {
                    std::string propertyType;
                    EPropertyTypes childType = Retrievers::GetPropertyType(structChild, propertyType);

                    if (childType == EPropertyTypes::FStruct)
                    {
                        UScriptStruct* propertyStruct = static_cast<UScriptStruct*>(static_cast<UStructProperty*>(structChild)->Struct);

                        if (propertyStruct
                            && (propertyStruct != scriptStruct)
                            && std::find(m_generatedStructs.begin(), m_generatedStructs.end(), propertyStruct->GetFullName()) == m_generatedStructs.end())
                        {
                            GenerateStructPre(stream, propertyStruct);
                        }
                    }
                    else if (childType == EPropertyTypes::TArray)
                    {
                        UScriptStruct* propertyStruct = static_cast<UScriptStruct*>(static_cast<UStructProperty*>(static_cast<UArrayProperty*>(structChild)->Inner)->Struct);

                        if (propertyStruct
                            && (propertyStruct != scriptStruct)
                            && (Retrievers::GetPropertyType(static_cast<UArrayProperty*>(structChild)->Inner, propertyType) == EPropertyTypes::FStruct)
                            && (std::find(m_generatedStructs.begin(), m_generatedStructs.end(), propertyStruct->GetFullName()) == m_generatedStructs.end()))
                        {
                            GenerateStructPre(stream, propertyStruct);
                        }
                    }
                }

                GenerateStruct(stream, unrealObj);
                m_generatedStructs.push_back(unrealObj.FullName);
            }
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
            std::map<uintptr_t, Member*> members = Member::GetRegistered(classType);

            if (!members.empty())
            {
                if (uClass->PropertySize == localSize)
                {
                    lastOffset = startOffset;
                    int32_t unknownDataIndex = 0;

                    for (const auto& member : members)
                    {
                        if ((lastOffset + member.second->Size) < member.second->Offset)
                        {
                            missedOffset = (member.second->Offset - lastOffset);

                            if (missedOffset >= GConfig::GetGameAlignment())
                            {
                                propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                                propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                                classStream << propertyStream.str() << " // " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                                classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") DYNAMIC FIELD PADDING\n";
                                Printer::Empty(propertyStream);

                                unknownDataIndex++;
                            }
                        }

                        classStream << "\t" << member.second->Label << " // " << Printer::Hex(member.second->Offset, EWidthTypes::SIZE);
                        classStream << " (" << Printer::Hex(member.second->Size, EWidthTypes::SIZE) << ")\n";

                        lastOffset = (member.second->Offset + member.second->Size);
                    }

                    if (lastOffset < uClass->PropertySize)
                    {
                        missedOffset = (uClass->PropertySize - lastOffset);

                        if (missedOffset >= GConfig::GetGameAlignment())
                        {
                            propertyStream << "\tuint8_t UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                            propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                            classStream << propertyStream.str() << " // " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                            classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") DYNAMIC FIELD PADDING\n";
                        }
                    }
                }
                else
                {
#ifndef NO_LOGGING
                    if (Generator::LogFile.is_open())
                    {
                        Generator::LogFile << "Error: INCORRECT CLASS SIZE DETECTED FOR CLASS " << uClass->GetName() << "\n";
                        Generator::LogFile << "Error: REGISTERED CLASS SIZE " << localSize << "\n";
                        Generator::LogFile << "Error: ACTUAL CLASS SIZE " << uClass->PropertySize << std::endl;
                    }
#endif
                    Utils::MessageboxError("Error: Incorrect class size detected for \"" + Member::GetName(classType) + "\", check the log file for more details!");
                }
            }
            else
            {
#ifndef NO_LOGGING
                Generator::LogFile << "Error: No registered members found for class \"" << uClass->GetName() << "\"\n";
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
            if (Generator::LogFile.is_open())
            {
                std::string propertyTitle = "Class: ";

                Generator::LogFile << propertyTitle;
                Printer::FillRight(Generator::LogFile, ' ', unrealObj.ValidName.size());
                Generator::LogFile << unrealObj.ValidName;
                Printer::FillRight(Generator::LogFile, ' ', (75 - (unrealObj.ValidName.size() + propertyTitle.size())));
                Generator::LogFile << " - Instance: " << Printer::Hex(uClass) << std::endl;
            }
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
                std::vector<UProperty*> classProperties;

                for (UProperty* uProperty = static_cast<UProperty*>(uClass->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                {
                    if (!uProperty->IsA<UFunction>()
                        && !uProperty->IsA<UConst>()
                        && !uProperty->IsA<UEnum>()
                        && !uProperty->IsA<UScriptStruct>()
                        && (uProperty->ElementSize > 0)
                        && (!uSuperClass || (uSuperClass && (uSuperClass != uClass) && (uProperty->Offset >= uSuperClass->PropertySize))))
                    {
                        classProperties.push_back(uProperty);
                    }
                }

                std::sort(classProperties.begin(), classProperties.end(), Utils::SortProperty);

                if (uSuperClass && uSuperClass != uClass)
                {
                    size = uClass->PropertySize - uSuperClass->PropertySize;
                    lastOffset = uSuperClass->PropertySize;

                    classStream << "// " << Printer::Hex(size, EWidthTypes::SIZE);
                    classStream << " (" << Printer::Hex(uSuperClass->PropertySize, EWidthTypes::SIZE);
                    classStream << " - " << Printer::Hex(uClass->PropertySize, EWidthTypes::SIZE);
                    classStream << ")\n" << "class " << classNameCPP << " : public " << Utils::CreateValidName(uSuperClass->GetNameCPP());
                }
                else
                {
                    size = uClass->PropertySize;

                    classStream << "// " << Printer::Hex(size, EWidthTypes::SIZE) << "\n";
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
                else if (uClass == UObjectProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UObjectProperty); }
                else if (uClass == UClassProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UClassProperty); }
                else if (uClass == UMapProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UMapProperty); }
                else if (uClass == UInterfaceProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UInterfaceProperty); }
                else if (uClass == UByteProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UByteProperty); }
                else if (uClass == UBoolProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UBoolProperty); }
                else if (uClass == UArrayProperty::StaticClass()) { GenerateClassMembers(classStream, uClass, EClassTypes::UArrayProperty); }
                else
                {
                    std::map<std::string, uint32_t> propertyNameMap;
                    uint32_t unknownDataIndex = 0;

                    for (UProperty* uProperty : classProperties)
                    {
                        if (uProperty)
                        {
                            std::string propertyName = Utils::CreateValidName(uProperty->GetName());

                            if (uClass == UObject::StaticClass())
                            {
                                if (!GConfig::UsingProcessEventIndex() && (propertyName.find("VfTable") != std::string::npos))
                                {
                                    lastOffset = (uProperty->Offset + (uProperty->ElementSize * uProperty->ArrayDim));
                                    continue;
                                }
                            }

                            if (lastOffset < uProperty->Offset)
                            {
                                missedOffset = (uProperty->Offset - lastOffset);

                                if (missedOffset >= GConfig::GetGameAlignment())
                                {
                                    propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                                    propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                                    classStream << "\t";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                    classStream << "uint8_t ";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing() - 3);
                                    classStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                                    classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") MISSED OFFSET\n";
                                    Printer::Empty(propertyStream);

                                    unknownDataIndex++;
                                }
                            }

                            std::string propertyType;

                            if (Retrievers::GetPropertyTypeInternal(uProperty, propertyType, false, false, true) != EPropertyTypes::Unknown)
                            {
                                size_t correctElementSize = Retrievers::GetPropertySize(uProperty);

                                if (propertyNameMap.count(propertyName) == 0)
                                {
                                    propertyNameMap[propertyName] = 1;
                                    propertyStream << propertyName;
                                }
                                else
                                {
                                    propertyStream << propertyName << Printer::Decimal(propertyNameMap[propertyName], EWidthTypes::BYTE);
                                    propertyNameMap[propertyName]++;
                                }

                                if (uProperty->ArrayDim > 1)
                                {
                                    if (!uProperty->IsA<UInterfaceProperty>())
                                    {
                                        propertyStream << "[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "]";
                                    }

                                    correctElementSize *= uProperty->ArrayDim;
                                }

                                if (uProperty->IsA<UBoolProperty>())
                                {
                                    propertyStream << " : 1";
                                }

                                Retrievers::GetAllPropertyFlags(flagStream, uProperty->PropertyFlags);

                                int32_t offsetError = ((uProperty->ElementSize * uProperty->ArrayDim) - (correctElementSize * uProperty->ArrayDim));

                                if (uProperty->IsA<UInterfaceProperty>())
                                {
                                    size_t interfaceSize = Retrievers::GetPropertySize(uProperty);

                                    if (offsetError == interfaceSize)
                                    {
                                        offsetError -= interfaceSize;
                                    }

                                    classStream << "\t";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                    classStream << propertyType << " " << propertyStream.str();

                                    if (uProperty->ArrayDim > 1)
                                    {
                                        classStream << "_Object[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "];";
                                    }
                                    else
                                    {
                                        classStream << "_Object;";
                                    }

                                    Printer::FillRight(classStream, ' ', GConfig::GetClassSpacing() - (propertyStream.str().size() + 8));
                                    classStream << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                    classStream << " (" << Printer::Hex(((uProperty->ElementSize * uProperty->ArrayDim) - interfaceSize), EWidthTypes::SIZE) << ")";
                                    classStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";
                                    classStream << flagStream.str() << "\n";

                                    classStream << "\t";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                    classStream << propertyType << " " << propertyStream.str();

                                    if (uProperty->ArrayDim > 1)
                                    {
                                        classStream << "_Interface[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "];";
                                    }
                                    else
                                    {
                                        classStream << "_Interface;";
                                    }

                                    Printer::FillRight(classStream, ' ', GConfig::GetClassSpacing() - (propertyStream.str().size() + 11));
                                    classStream << "// " << Printer::Hex(uProperty->Offset + interfaceSize, EWidthTypes::SIZE);
                                    classStream << " (" << Printer::Hex(((uProperty->ElementSize * uProperty->ArrayDim) - interfaceSize), EWidthTypes::SIZE) << ")";
                                    classStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";
                                    Printer::FillLeft(classStream, ' ', static_cast<size_t>(EWidthTypes::WIDTH_FIELD));
                                }
                                else
                                {
                                    classStream << "\t";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                    classStream << propertyType << " " << propertyStream.str() << ";";

                                    Printer::FillRight(classStream, ' ', GConfig::GetClassSpacing() - (propertyStream.str().size() + 1));
                                    classStream << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                    classStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ")";
                                    classStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";

                                    if (uProperty->IsA<UBoolProperty>())
                                    {
                                        classStream << "[" << Printer::Hex(static_cast<UBoolProperty*>(uProperty)->BitMask, EWidthTypes::BITMASK) << "] ";
                                    }
                                    else
                                    {
                                        Printer::FillLeft(classStream, ' ', static_cast<size_t>(EWidthTypes::WIDTH_FIELD));
                                    }
                                }

                                classStream << flagStream.str() << "\n";

                                if (offsetError > 0)
                                {
                                    propertyStream << "_UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                                    propertyStream << "[" << Printer::Hex(offsetError, EWidthTypes::NONE) << "];";

                                    classStream << "\t";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                    classStream << "uint8_t ";
                                    Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing() - 3);
                                    classStream << propertyStream.str() << "// " << Printer::Hex((uProperty->Offset + offsetError), EWidthTypes::SIZE);
                                    classStream << " (" << Printer::Hex(offsetError, EWidthTypes::SIZE) << ") FIX WRONG SIZE OF PREVIOUS PROPERTY ";
                                    classStream << " [Original:" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE);
                                    classStream << ", Missing: " << Printer::Hex(offsetError, EWidthTypes::SIZE) << "]\n";
                                    Printer::Empty(propertyStream);

                                    unknownDataIndex++;
                                }

                                Printer::Empty(propertyStream);
                                Printer::Empty(flagStream);
                            }
                            else
                            {
                                propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                                propertyStream << "[" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::NONE) << "];";

                                classStream << "\t";
                                Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                                classStream << "uint8_t ";
                                Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing() - 3);
                                classStream << propertyStream.str() << "// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                classStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ") UNKNOWN PROPERTY: " << uProperty->GetFullName() << "\n";
                                Printer::Empty(propertyStream);

                                unknownDataIndex++;
                            }

                            lastOffset = (uProperty->Offset + (uProperty->ElementSize * uProperty->ArrayDim));
                        }
                    }

                    if (lastOffset < uClass->PropertySize)
                    {
                        missedOffset = (uClass->PropertySize - lastOffset);

                        if (missedOffset >= GConfig::GetGameAlignment())
                        {
                            propertyStream << "UnknownData" << Printer::Decimal(unknownDataIndex, EWidthTypes::BYTE);
                            propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                            classStream << "\t";
                            Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing());
                            classStream << "uint8_t ";
                            Printer::FillLeft(classStream, ' ', GConfig::GetClassSpacing() - 3);
                            classStream << propertyStream.str() << "// " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                            classStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") MISSED OFFSET\n";
                            Printer::Empty(propertyStream);
                        }
                    }
                }

                classStream << "\npublic:\n";

                if (GConfig::UsingConstants())
                {
                    classStream << "\tstatic UClass* StaticClass()\n";
                    classStream << "\t{\n";
                    classStream << "\t\tstatic UClass* uClassPointer = nullptr;\n\n";
                    classStream << "\t\tif (!uClassPointer)\n";
                    classStream << "\t\t{\n";
                    classStream << "\t\t\tuClassPointer = reinterpret_cast<UClass*>(UObject::GObjObjects()->at(" << GCache::GetConstant(unrealObj).first << "));\n";
                    classStream << "\t\t}\n\n";
                    classStream << "\t\treturn uClassPointer;\n";
                    classStream << "\t};\n\n";;
                }
                else
                {
                    classStream << "\tstatic UClass* StaticClass()\n";
                    classStream << "\t{\n";
                    classStream << "\t\tstatic UClass* uClassPointer = nullptr;\n\n";
                    classStream << "\t\tif (!uClassPointer)\n";
                    classStream << "\t\t{\n";
                    classStream << "\t\t\tuClassPointer = UObject::FindClass(\"" << unrealObj.FullName << "\");\n";
                    classStream << "\t\t}\n\n";
                    classStream << "\t\treturn uClassPointer;\n";
                    classStream << "\t};\n\n";
                }

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
        if (unrealObj.IsValid())
        {
            if (unrealObj.Package != packageObj)
            {
                return;
            }

            if (!m_generatedClasses.contains(unrealObj.FullName))
            {
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
        std::vector<UFunction*> vFunction;

        for (UProperty* uProperty = static_cast<UProperty*>(uClass->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
        {
            if (uProperty->IsA<UFunction>())
            {
                vFunction.push_back(reinterpret_cast<UFunction*>(uProperty));
            }
        }

        for (UFunction* uFunction : vFunction)
        {
            if (uFunction)
            {
                std::string functionFullName = uFunction->GetFullName();
                std::string functionName = Utils::CreateValidName(uFunction->GetName());
                std::string classNameCPP = Utils::CreateValidName(uClass->GetNameCPP());

                parameterStream << "// " << functionFullName << "\n";
                parameterStream << "// [" << Printer::Hex(uFunction->FunctionFlags, EWidthTypes::FUNCTION_FLAGS) << "] ";

                if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Native) && uFunction->iNative)
                {
                    parameterStream << " (iNative[" << uFunction->iNative << "])";
                }

                if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { propertyStream << "exec"; }
                else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { propertyStream << "event"; }
                else { propertyStream << "exec"; }

                if (GConfig::UsingWindows())
                {
                    Utils::MakeWinSafe(functionName);
                }

                parameterStream << "\nstruct " << classNameCPP << "_" << propertyStream.str() << functionName << "_Params\n" << "{\n";
                Printer::Empty(propertyStream);
                std::vector<UProperty*> vProperty;

                for (UProperty* uProperty = static_cast<UProperty*>(uFunction->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                {
                    if (uProperty->ElementSize > 0)
                    {
                        vProperty.push_back(uProperty);
                    }
                }

                std::sort(vProperty.begin(), vProperty.end(), Utils::SortProperty);
                std::map<std::string, uint32_t> propertyNameMap;

                for (UProperty* uProperty : vProperty)
                {
                    if (uProperty)
                    {
                        std::string propertyType;
                        EPropertyTypes returnPropertyType = EPropertyTypes::Unknown;

                        if ((uProperty->PropertyFlags & EPropertyFlags::CPF_OutParm) || (uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                        {
                            returnPropertyType = Retrievers::GetPropertyType(uProperty, propertyType, (uProperty->IsA<UArrayProperty>() ? false : true));
                        }
                        else
                        {
                            returnPropertyType = Retrievers::GetPropertyTypeInternal(uProperty, propertyType, true, false, true);
                        }

                        if (returnPropertyType != EPropertyTypes::Unknown)
                        {
                            std::string propertyName = Utils::CreateValidName(uProperty->GetName());

                            if (propertyNameMap.count(propertyName) == 0)
                            {
                                propertyNameMap[propertyName] = 1;
                                propertyStream << propertyName;
                            }
                            else
                            {
                                propertyStream << propertyName << Printer::Decimal(propertyNameMap[propertyName], EWidthTypes::BYTE);
                                propertyNameMap[propertyName]++;
                            }

                            if (uProperty->ArrayDim > 1)
                            {
                                propertyStream << "[" << Printer::Hex(uProperty->ArrayDim, EWidthTypes::NONE) << "]";
                            }

                            if (uProperty->IsA<UBoolProperty>())
                            {
                                propertyStream << " : 1";
                            }

                            propertyStream << ";";
                            Retrievers::GetAllPropertyFlags(flagStream, uProperty->PropertyFlags);

                            if (uProperty->PropertyFlags & EPropertyFlags::CPF_Parm)
                            {
                                parameterStream << "\t";
                                Printer::FillLeft(parameterStream, ' ', GConfig::GetFunctionSpacing());
                                parameterStream << propertyType << " ";
                                Printer::FillLeft(parameterStream, ' ', GConfig::GetFunctionSpacing());
                                parameterStream << propertyStream.str() << "\t\t// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                parameterStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ")";
                                parameterStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";
                            }
                            else
                            {
                                parameterStream << "\t// ";
                                Printer::FillLeft(parameterStream, ' ', GConfig::GetFunctionSpacing());
                                parameterStream << propertyType << " ";
                                Printer::FillLeft(parameterStream, ' ', GConfig::GetFunctionSpacing());
                                parameterStream << propertyStream.str() << "\t\t// " << Printer::Hex(uProperty->Offset, EWidthTypes::SIZE);
                                parameterStream << " (" << Printer::Hex((uProperty->ElementSize * uProperty->ArrayDim), EWidthTypes::SIZE) << ")";
                                parameterStream << " [" << Printer::Hex(uProperty->PropertyFlags, EWidthTypes::PROPERTY_FLAGS) << "] ";
                            }

                            if (uProperty->IsA<UBoolProperty>())
                            {
                                parameterStream << "[" << Printer::Hex(static_cast<UBoolProperty*>(uProperty)->BitMask, EWidthTypes::BITMASK) << "] ";
                            }
                            else
                            {
                                Printer::FillLeft(parameterStream, ' ', static_cast<size_t>(EWidthTypes::WIDTH_FIELD));
                            }

                            parameterStream << flagStream.str() << "\n";

                            Printer::Empty(propertyStream);
                            Printer::Empty(flagStream);
                        }
                        else
                        {
                            parameterStream << "\t// UNKNOWN PROPERTY: " << uProperty->GetFullName() << "\n";
                        }
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
            Utils::MessageboxWarn("Warning: ProcessEvent is not configured correctly in \"Configuration.cpp\", you set \"UsingOffsets\" to true yet you did not provide a valid index for process event!");
        }

        if (processEventAddress)
        {
            stream << "\n\t// Virtual Functions\n\n";

            for (int32_t index = 0; index < 512; index++)
            {
                uintptr_t virtualFunction = reinterpret_cast<uintptr_t*>(UObject::StaticClass()->VfTableObject.Dummy)[index];

                if (virtualFunction == processEventAddress)
                {
                    stream << "\tvirtual void ProcessEvent(class UFunction* uFunction, void* uParams, void* uResult = nullptr);";
                    stream << "\t\t\t// " << Printer::Hex(virtualFunction, sizeof(virtualFunction)) << "\n";
                    break;
                }
                else
                {
                    stream << "\tvirtual void VirtualFunction" << index << "();";
                    stream << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t// " << Printer::Hex(virtualFunction, sizeof(virtualFunction)) << "\n";
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
            std::vector<UFunction*> classFunctions;

            static bool objectFunctions = false;
            static bool functionFunctions = false;

            if (!objectFunctions && (uClass == UObject::StaticClass()))
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

                objectFunctions = true;
            }

            if (!functionFunctions && (uClass == UFunction::StaticClass()))
            {
                codeStream << PiecesOfCode::UFunction_Functions;
                functionFunctions = true;
            }

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
                    codeStream << "// [" << Printer::Hex(uFunction->FunctionFlags, EWidthTypes::FUNCTION_FLAGS) << "] " << functionStream.str();
                    Printer::Empty(functionStream);

                    if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Native) && uFunction->iNative)
                    {
                        codeStream << " (iNative[" << uFunction->iNative << "])";
                    }

                    std::vector<std::pair<UProperty*, std::string>> propertyParams;
                    std::vector<std::pair<UProperty*, std::string>> propertyOutParams;
                    std::vector<std::pair<UProperty*, std::string>> propertyBothParams;
                    std::pair<UProperty*, std::string> propertyReturnParm;
                    std::map<std::string, uint32_t> propertyNameMap;

                    for (UProperty* uProperty = static_cast<UProperty*>(uFunction->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                    {
                        if (uProperty->ElementSize > 0)
                        {
                            std::string propertyNameBuffer = Utils::CreateValidName(uProperty->GetName());
                            std::string propertyNameUnique;

                            if (propertyNameMap.count(propertyNameBuffer) == 0)
                            {
                                propertyNameMap[propertyNameBuffer] = 1;
                                propertyNameUnique = propertyNameBuffer;
                            }
                            else
                            {
                                functionStream << propertyNameBuffer << Printer::Decimal(propertyNameMap[propertyNameBuffer], EWidthTypes::BYTE);
                                propertyNameUnique = functionStream.str();
                                Printer::Empty(functionStream);
                                propertyNameMap[propertyNameBuffer]++;
                            }

                            if (uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm)
                            {
                                propertyReturnParm = std::make_pair(uProperty, propertyNameUnique);
                            }
                            else if ((uProperty->PropertyFlags & EPropertyFlags::CPF_Parm) && (uProperty->PropertyFlags & EPropertyFlags::CPF_OutParm))
                            {
                                propertyOutParams.push_back(std::make_pair(uProperty, propertyNameUnique));
                                propertyBothParams.push_back(std::make_pair(uProperty, propertyNameUnique));
                            }
                            else if (uProperty->PropertyFlags & EPropertyFlags::CPF_Parm)
                            {
                                propertyParams.push_back(std::make_pair(uProperty, propertyNameUnique));
                            }
                        }
                    }

                    std::sort(propertyParams.begin(), propertyParams.end(), Utils::SortPropertyPair);
                    std::sort(propertyOutParams.begin(), propertyOutParams.end(), Utils::SortPropertyPair);
                    std::sort(propertyBothParams.begin(), propertyBothParams.end(), Utils::SortPropertyPair);

                    codeStream << "\n// Parameter Info:\n";

                    std::string propertyType;

                    if (propertyReturnParm.first && Retrievers::GetPropertyTypeDesc(propertyReturnParm.first, propertyType) != EPropertyTypes::Unknown)
                    {
                        Retrievers::GetAllPropertyFlags(functionStream, propertyReturnParm.first->PropertyFlags);
                        codeStream << "// ";
                        Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                        codeStream << propertyType << " ";
                        Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                        codeStream << propertyReturnParm.second << " " << functionStream.str() << "\n";
                        Printer::Empty(functionStream);
                    }

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (Retrievers::GetPropertyTypeDesc(propertyPair.first, propertyType) != EPropertyTypes::Unknown)
                        {
                            Retrievers::GetAllPropertyFlags(functionStream, propertyPair.first->PropertyFlags);
                            codeStream << "// ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyType << " ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyPair.second << " " << functionStream.str() << "\n";
                            Printer::Empty(functionStream);
                        }
                    }

                    for (const auto& propertyPair : propertyOutParams)
                    {
                        if (Retrievers::GetPropertyType(propertyPair.first, propertyType) != EPropertyTypes::Unknown)
                        {
                            Retrievers::GetAllPropertyFlags(functionStream, propertyPair.first->PropertyFlags);
                            codeStream << "// ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyType << " ";
                            Printer::FillLeft(codeStream, ' ', GConfig::GetCommentSpacing());
                            codeStream << propertyPair.second << " " << functionStream.str() << "\n";
                            Printer::Empty(functionStream);
                        }
                    }

                    if (propertyReturnParm.first && Retrievers::GetPropertyType(propertyReturnParm.first, propertyType) != EPropertyTypes::Unknown)
                    {
                        codeStream << "\n" << propertyType;
                    }
                    else
                    {
                        codeStream << "\nvoid";
                    }

                    if (GConfig::UsingWindows())
                    {
                        Utils::MakeWinSafe(functionName);
                    }

                    if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { codeStream << " " << classNameCPP << "::" << functionName << "("; }
                    else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { codeStream << " " << classNameCPP << "::event" << functionName << "("; }
                    else { codeStream << " " << classNameCPP << "::" << functionName << "("; }

                    bool printComma = false;

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (Retrievers::GetPropertyTypeDesc(propertyPair.first, propertyType) != EPropertyTypes::Unknown)
                        {
                            if (printComma)
                            {
                                codeStream << ", ";
                            }

                            codeStream << propertyType << " " << propertyPair.second;
                            printComma = true;
                        }
                    }

                    for (const auto& propertyPair : propertyOutParams)
                    {
                        if (Retrievers::GetPropertyType(propertyPair.first, propertyType) != EPropertyTypes::Unknown)
                        {
                            if (propertyPair.first->PropertyFlags & EPropertyFlags::CPF_Parm)
                            {
                                if (printComma)
                                {
                                    codeStream << ", ";
                                }

                                codeStream << propertyType << "& " << propertyPair.second;
                                printComma = true;
                            }
                        }
                    }

                    if (GConfig::UsingConstants())
                    {
                        codeStream << ")\n";
                        codeStream << "{\n";
                        codeStream << "\tstatic UFunction* uFn" << functionName << " = nullptr;\n\n";
                        codeStream << "\tif (!uFn" << functionName << ")\n";
                        codeStream << "\t{\n";
                        codeStream << "\t\tuFn" << functionName << " = reinterpret_cast<UFunction*>(UObject::GObjObjects()->at(" << GCache::GetConstant(unrealObj).first << "));\n";
                        codeStream << "\t}\n\n";
                        codeStream << "\t" << classNameCPP << "_";
                    }
                    else
                    {
                        codeStream << ")\n";
                        codeStream << "{\n";
                        codeStream << "\tstatic UFunction* uFn" << functionName << " = nullptr;\n\n";
                        codeStream << "\tif (!uFn" << functionName << ")\n";
                        codeStream << "\t{\n";
                        codeStream << "\t\tuFn" << functionName << " = UFunction::FindFunction(\"" << functionFullName << "\");\n";
                        codeStream << "\t}\n\n";
                        codeStream << "\t" << classNameCPP << "_";
                    }

                    if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { codeStream << "exec"; }
                    else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { codeStream << "event"; }
                    else { codeStream << "exec"; }

                    codeStream << functionName << "_Params " << functionName << "_Params;\n";
                    codeStream << "\tmemset(&" << functionName << "_Params, 0, sizeof(" << functionName << "_Params));\n";
                    //codeStream << "\tZeroMemory(&" << functionName << "_Params, sizeof(" << functionName << "_Params));\n";

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (propertyPair.first->PropertyFlags & EPropertyFlags::CPF_Parm)
                        {
                            EPropertyTypes propertyTypeResult = Retrievers::GetPropertyType(propertyPair.first, propertyType);

                            if (propertyTypeResult != EPropertyTypes::Unknown)
                            {
                                if ((propertyTypeResult != EPropertyTypes::Bool) && (!Utils::CantMemcpy(propertyTypeResult) || !Utils::IsBitField(propertyPair.first->ArrayDim)))
                                {
                                    codeStream << "\tmemcpy_s(&" << functionName << "_Params." << propertyPair.second << ", sizeof(" << functionName << "_Params." << propertyPair.second;
                                    codeStream << "), &" << propertyPair.second << ", sizeof(" << propertyPair.second << ")";
                                    codeStream << ");\n";
                                }
                                else if (!Utils::IsStructProperty(propertyTypeResult))
                                {
                                    codeStream << "\t" << functionName << "_Params." << propertyPair.second << " = " << propertyPair.second << ";\n";
                                }
                            }
                        }
                    }

                    for (const auto& propertyPair : propertyBothParams)
                    {
                        EPropertyTypes propertyTypeResult = Retrievers::GetPropertyType(propertyPair.first, propertyType);

                        if (propertyPair.first->PropertyFlags & EPropertyFlags::CPF_Parm)
                        {
                            EPropertyTypes propertyTypeResult = Retrievers::GetPropertyType(propertyPair.first, propertyType);

                            if (propertyTypeResult != EPropertyTypes::Unknown)
                            {
                                if ((propertyTypeResult != EPropertyTypes::Bool) && (!Utils::CantMemcpy(propertyTypeResult) || !Utils::IsBitField(propertyPair.first->ArrayDim)))
                                {
                                    codeStream << "\tmemcpy_s(&" << functionName << "_Params." << propertyPair.second << ", sizeof(" << functionName << "_Params." << propertyPair.second;
                                    codeStream << "), &" << propertyPair.second << ", sizeof(" << propertyPair.second << ")";
                                    codeStream << ");\n";
                                }
                                else if (!Utils::IsStructProperty(propertyTypeResult))
                                {
                                    codeStream << "\t" << functionName << "_Params." << propertyPair.second << " = " << propertyPair.second << ";\n";
                                }
                            }
                        }
                    }

                    bool hasNativeIndex = (uFunction->iNative ? true : false);
                    bool isNativeFunction = (uFunction->FunctionFlags & EFunctionFlags::FUNC_Native);

                    if (isNativeFunction && hasNativeIndex && GConfig::RemoveNativeIndex())
                    {
                        codeStream << "\n\tuFn" << functionName << "->iNative = 0;";
                    }

                    if (GConfig::RemoveNativeFlags())
                    {
                        if (isNativeFunction)
                        {
                            codeStream << "\n\tuFn" << functionName << "->FunctionFlags &= ~" << Printer::Hex(EFunctionFlags::FUNC_Native, EWidthTypes::NONE) << ";";
                        }
                    }

                    if ((uFunction->FunctionFlags & EFunctionFlags::FUNC_Static) && (uFunction->FunctionFlags != EFunctionFlags::FUNC_AllFlags))
                    {
                        codeStream << "\n\t" << classNameCPP << "::StaticClass()->ProcessEvent(" << "uFn" << functionName << ", &" << functionName << "_Params, nullptr);\n";
                    }
                    else
                    {
                        codeStream << "\n\tthis->ProcessEvent(uFn" << functionName << ", &" << functionName << "_Params, nullptr);\n";
                    }

                    if (GConfig::RemoveNativeFlags())
                    {
                        if (isNativeFunction)
                        {
                            codeStream << "\tuFn" << functionName << "->FunctionFlags |= " << Printer::Hex(EFunctionFlags::FUNC_Native, EWidthTypes::NONE) << ";\n";
                        }
                    }

                    if (isNativeFunction && hasNativeIndex && GConfig::RemoveNativeIndex())
                    {
                        codeStream << "\tuFn" << functionName << "->iNative = " << uFunction->iNative << ";\n";
                    }

                    if (!propertyOutParams.empty())
                    {
                        codeStream << "\n";

                        for (const auto& propertyPair : propertyOutParams)
                        {
                            if (propertyPair.first->PropertyFlags & EPropertyFlags::CPF_OutParm)
                            {
                                EPropertyTypes propertyTypeResult = Retrievers::GetPropertyType(propertyPair.first, propertyType);

                                if (propertyTypeResult != EPropertyTypes::Unknown)
                                {
                                    if ((propertyTypeResult != EPropertyTypes::Bool) && (!Utils::CantMemcpy(propertyTypeResult) || !Utils::IsBitField(propertyPair.first->ArrayDim)))
                                    {
                                        codeStream << "\tmemcpy_s(&" << propertyPair.second << ", sizeof(" << propertyPair.second;
                                        codeStream << "), &" << functionName << "_Params." << propertyPair.second << ", sizeof(" << functionName << "_Params." << propertyPair.second;
                                        codeStream << "));\n";
                                    }
                                    else if (!Utils::IsStructProperty(propertyTypeResult))
                                    {
                                        codeStream << "\t" << propertyPair.second << " = " << functionName << "_Params." << propertyPair.second << ";\n";
                                    }
                                }
                            }
                        }
                    }

                    if (propertyReturnParm.first && Retrievers::GetPropertyType(propertyReturnParm.first, propertyType) != EPropertyTypes::Unknown)
                    {
                        codeStream << "\n\treturn " << functionName << "_Params." << propertyReturnParm.second << ";\n";
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
                if (uField->IsA<UFunction>())
                {
                    classFunctions.push_back(static_cast<UFunction*>(uField));
                }
            }

            for (UFunction* uFunction : classFunctions)
            {
                if (uFunction)
                {
                    std::string functionName = Utils::CreateValidName(uFunction->GetName());

                    std::vector<std::pair<UProperty*, std::string>> propertyParams;
                    std::vector<std::pair<UProperty*, std::string>> propertyOutParams;
                    std::pair<UProperty*, std::string> propertyReturnParm;
                    std::map<std::string, uint32_t> propertyNameMap;

                    for (UProperty* uProperty = static_cast<UProperty*>(uFunction->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
                    {
                        if (uProperty->ElementSize > 0)
                        {
                            std::string propertyNameBuffer = Utils::CreateValidName(uProperty->GetName());
                            std::string propertyNameUnique;

                            if (propertyNameMap.count(propertyNameBuffer) == 0)
                            {
                                propertyNameMap[propertyNameBuffer] = 1;
                                propertyNameUnique = propertyNameBuffer;
                            }
                            else
                            {
                                propertyStream << propertyNameBuffer << Printer::Decimal(propertyNameMap[propertyNameBuffer], EWidthTypes::BYTE);
                                propertyNameUnique = propertyStream.str();
                                Printer::Empty(propertyStream);
                                propertyNameMap[propertyNameBuffer]++;
                            }

                            if (uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm) { propertyReturnParm = std::make_pair(uProperty, propertyNameUnique); }
                            else if (uProperty->PropertyFlags & EPropertyFlags::CPF_OutParm) { propertyOutParams.push_back(std::make_pair(uProperty, propertyNameUnique)); }
                            else if (uProperty->PropertyFlags & EPropertyFlags::CPF_Parm)
                            {
                                if (uProperty->PropertyFlags & EPropertyFlags::CPF_OptionalParm)
                                {
                                    propertyNameUnique[0] = std::toupper(propertyNameUnique[0]);
                                    propertyParams.push_back(std::make_pair(uProperty, ("optional" + propertyNameUnique)));
                                }
                                else
                                {
                                    propertyParams.push_back(std::make_pair(uProperty, propertyNameUnique));
                                }
                            }
                        }
                    }

                    std::sort(propertyParams.begin(), propertyParams.end(), Utils::SortPropertyPair);
                    std::sort(propertyOutParams.begin(), propertyOutParams.end(), Utils::SortPropertyPair);

                    bool isStatic = (uFunction->FunctionFlags & EFunctionFlags::FUNC_Static) && (uFunction->FunctionFlags != EFunctionFlags::FUNC_AllFlags);

                    std::string propertyType;

                    if (propertyReturnParm.first && Retrievers::GetPropertyType(propertyReturnParm.first, propertyType) != EPropertyTypes::Unknown)
                    {
                        functionStream << "\t" << (isStatic ? "static " : "") << propertyType;
                    }
                    else
                    {
                        functionStream << "\t" << (isStatic ? "static " : "") << "void";
                    }

                    if (GConfig::UsingWindows())
                    {
                        Utils::MakeWinSafe(functionName);
                    }

                    if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Exec) { functionStream << " " << functionName << "("; }
                    else if (uFunction->FunctionFlags & EFunctionFlags::FUNC_Event) { functionStream << " event" << functionName << "("; }
                    else { functionStream << " " << functionName << "("; }

                    bool printComma = false;

                    for (const auto& propertyPair : propertyParams)
                    {
                        if (Retrievers::GetPropertyTypeDesc(propertyPair.first, propertyType) != EPropertyTypes::Unknown)
                        {
                            if (printComma)
                            {
                                functionStream << ", ";
                            }

                            functionStream << propertyType << " " << propertyPair.second;
                            printComma = true;
                        }
                    }

                    for (const auto& propertyPair : propertyOutParams)
                    {
                        if (Retrievers::GetPropertyType(propertyPair.first, propertyType) != EPropertyTypes::Unknown)
                        {
                            if (propertyPair.first->PropertyFlags & EPropertyFlags::CPF_Parm)
                            {
                                if (printComma)
                                {
                                    functionStream << ", ";
                                }

                                functionStream << propertyType << "& " << propertyPair.second;
                                printComma = true;
                            }
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
    std::ofstream LogFile;


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
                    Printer::FillRight(constantsFile, ' ', (175 - constantPair.first.size()));
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
        std::vector<std::pair<class UObject*, std::string>>* includes = GCache::GetIncludes();

        for (auto& includePair : *includes)
        {
            if (!includePair.second.empty())
            {
                headersFile << "#include \"SDK_HEADERS\\" << includePair.second << "_structs.hpp\"\n";
                headersFile << "#include \"SDK_HEADERS\\" << includePair.second << "_classes.hpp\"\n";
                headersFile << "#include \"SDK_HEADERS\\" << includePair.second << "_parameters.hpp\"\n";
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

        Printer::Section(definesFile, "Globals");
        definesFile << "extern class TArray<class UObject*>* GObjects;\n";
        definesFile << "extern class TArray<class FNameEntry*>* GNames;\n";

        Printer::Section(definesFile, "Structs");
        definesFile << PiecesOfCode::FNameEntry_Struct << "\n";
        StructGenerator::GenerateStructMembers(definesFile, EClassTypes::FNameEntry);

#ifdef UTF16
        definesFile << PiecesOfCode::FNameEntry_Struct_UTF16 << "\n";
        definesFile << PiecesOfCode::FName_Struct_UTF16 << "\n";
        definesFile << PiecesOfCode::FString_Class_UTF16 << "\n";
#else
        definesFile << PiecesOfCode::FNameEntry_Struct_UTF8 << "\n";
        definesFile << PiecesOfCode::FName_Struct_UTF8 << "\n";
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
        Printer::Section(definesFile, "Initialize Global Variables");
        definesFile << "class TArray<class UObject*>* GObjects{};\n";
        definesFile << "class TArray<class FNameEntry*>* GNames{};\n\n";

        Printer::Footer(definesFile, false);
        definesFile.close();
    }

    void InitializeCache()
    {
#ifndef NO_LOGGING
        LogFile << "\nInitializing cache..." << std::endl;
        std::chrono::time_point startTime = std::chrono::system_clock::now();
#endif

        GCache::Initialize();

#ifndef NO_LOGGING
        std::chrono::time_point endTime = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsedTime = (endTime - startTime);
        std::string formattedTime = Printer::Precision(elapsedTime.count(), 4);

        LogFile << "Cache initialized in " << formattedTime << " seconds.\n" << std::endl;
#endif
    }

    void ProcessPackages(const std::filesystem::path& directory)
    {
        if (std::filesystem::exists(directory))
        {
            std::vector<class UObject*>* packages = GCache::GetPackages();

#ifndef NO_LOGGING
            LogFile << "\nPackages count: " << packages->size() << "\n" << std::endl;
#endif

            for (class UObject* packageObj : *packages)
            {
                if (packageObj)
                {
                    std::string packageName = Utils::CreateValidName(packageObj->GetName());
#ifndef NO_LOGGING
                    LogFile << "\nProcessing Package: " << packageName << "\n" << std::endl;
#endif
                    std::ofstream file;

                    // Structs
                    file.open(directory / (packageName + "_structs.hpp"));
                    Printer::Header(file, (packageName + "_structs"), "hpp", true);
                    Printer::Section(file, "Structs");
                    StructGenerator::ProcessStructs(file, packageObj);
                    Printer::Footer(file, true);
                    file.close();

                    // Classes
                    file.open(directory / (packageName + "_classes.hpp"));
                    Printer::Header(file, (packageName + "_classes"), "hpp", true);
                    Printer::Section(file, "Constants");
                    ConstGenerator::ProcessConsts(file, packageObj);
                    Printer::Section(file, "Enums");
                    EnumGenerator::ProcessEnums(file, packageObj);
                    Printer::Section(file, "Classes");
                    ClassGenerator::ProcessClasses(file, packageObj);
                    Printer::Footer(file, true);
                    file.close();

                    // Parameters
                    file.open(directory / (packageName + "_parameters.hpp"));
                    Printer::Header(file, (packageName + "_parameters"), "hpp", true);
                    Printer::Section(file, "Parameters");
                    ParameterGenerator::ProcessParameters(file, packageObj);
                    Printer::Footer(file, true);
                    file.close();

                    // Functions
                    file.open(directory / (packageName + "_classes.cpp"));
                    Printer::Header(file, (packageName + "_classes"), "cpp", true);
                    Printer::Section(file, "Functions");
                    FunctionGenerator::ProcessFunctions(file, packageObj);
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

                InitializeCache();
                ProcessPackages(headerDirectory);
                GenerateHeaders();
                GenerateDefines();
                GenerateConstants();

                std::chrono::time_point endTime = std::chrono::system_clock::now();
                std::chrono::duration<float> elapsedTime = (endTime - startTime);
                std::string formattedTime = Printer::Precision(elapsedTime.count(), 4);

#ifndef NO_LOGGING
                if (LogFile.is_open())
                {
                    LogFile << "\n" << GConfig::GetGameNameShort() << " generated in " << formattedTime << " seconds.";
                    LogFile.close();
                }
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
        if (!AreGlobalsValid())
        {
            if (GConfig::UsingOffsets())
            {
                GObjects = reinterpret_cast<TArray<UObject*>*>(Retrievers::GetEntryPoint() + GConfig::GetGObjectOffset());
                GNames = reinterpret_cast<TArray<FNameEntry*>*>(Retrievers::GetEntryPoint() + GConfig::GetGNameOffset());
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
                FNameEntry::Register_Flags();
                FNameEntry::Register_Index();
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
                UStruct::Register_SuperField(); // Not needed if SuperField is in the UStruct class, leave "SUPERFIELDS_IN_UFIELD" in your "GameDefines.hpp" file!
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
            }
            else
            {
                Utils::MessageboxError("Failed to validate GObject & GNames, please make sure you have them configured properly in \"Configuration.cpp\"!");
                return false;
            }
        }

        if (AreGlobalsValid())
        {
            if (GConfig::HasOutputPath())
            {
#ifndef NO_LOGGING
                if (bCreateLog)
                {
                    std::filesystem::path fullDirectory = (GConfig::GetOutputPath() / GConfig::GetGameNameShort());
                    std::filesystem::create_directory(GConfig::GetOutputPath());
                    std::filesystem::create_directory(fullDirectory);

                    if (std::filesystem::exists(fullDirectory))
                    {
                        LogFile.open(fullDirectory / (GEngine::GetName() + ".log"));
                        LogFile << "Entry Point: " << Printer::Hex(Retrievers::GetEntryPoint(), sizeof(uintptr_t)) << "\n";
                        LogFile << "GObjects: " << Printer::Hex(GObjects) << "\n";
                        LogFile << "GNames: " << Printer::Hex(GNames) << "\n";
                        LogFile.flush();
                    }
                    else
                    {
                        Utils::MessageboxError("Error: Failed to create the log file, might not have the right permissions or your directory is invalid!");
                        return false;
                    }
                }
#endif
                return true;
            }
            else
            {
                Utils::MessageboxError("Looks like you forgot to set an output path for the generator! Please edit the output path in \"Configuration.cpp\" and recompile.");
                return false;
            }
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

                file << "Entry Point: " << Printer::Hex(Retrievers::GetEntryPoint(), sizeof(uintptr_t)) << std::endl;
                file << "GNames: " << Printer::Hex(GNames) << std::endl;
                file << "Offset: " << Printer::Hex(Retrievers::GetOffset(GNames), sizeof(uintptr_t)) << "\n" << std::endl;

                for (FNameEntry* nameEntry : *FName::Names())
                {
                    if (nameEntry)
                    {
                        std::string name = nameEntry->ToString();

                        file << "Name[";
                        Printer::FillRight(file, '0', 6);
                        file << nameEntry->GetIndex() << "] " << name << " ";
                        Printer::FillRight(file, ' ', (50 - name.length()));
                        file << Printer::Hex(nameEntry) << std::endl;
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

                file << "Entry Point: " << Printer::Hex(Retrievers::GetEntryPoint(), sizeof(uintptr_t)) << std::endl;
                file << "GObjects: " << Printer::Hex(GObjects) << std::endl;
                file << "Offset: " << Printer::Hex(Retrievers::GetOffset(GObjects), sizeof(uintptr_t)) << "\n" << std::endl;

                for (int32_t i = 0; i < (UObject::GObjObjects()->size() - 1); i++)
                {
                    UObject* uObject = UObject::GObjObjects()->at(i);

                    if (uObject)
                    {
                        std::string objectName = uObject->GetFullName();

                        file << "UObject[";
                        Printer::FillRight(file, '0', 6);
                        file << uObject->ObjectInternalInteger << "] " << objectName << " ";
                        Printer::FillRight(file, ' ', (50 - objectName.length()));
                        file << Printer::Hex(uObject) << std::endl;
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