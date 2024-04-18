#include "dllmain.hpp"

namespace Utils
{
    void Messagebox(const std::string& message, uint32_t flags)
    {
        MessageBoxA(NULL, message.c_str(), GEngine::GetName().c_str(), flags);
    }

    bool MapExists(std::multimap<std::string, std::string>& map, const std::string& key, const std::string& value)
    {
        std::pair<std::multimap<std::string, std::string>::iterator, std::multimap<std::string, std::string>::iterator> prRange;
        prRange = map.equal_range(key);

        for (auto rangeIt = prRange.first; rangeIt != prRange.second; rangeIt++)
        {
            if (rangeIt->second == value)
            {
                return true;
            }
        }

        return false;
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

    bool IsStructProperty(EPropertyTypes propertyType)
    {
        return ((propertyType == EPropertyTypes::TYPE_TARRAY)
            || (propertyType == EPropertyTypes::TYPE_TMAP)
            || (propertyType == EPropertyTypes::TYPE_FNAMEENTRY)
            || (propertyType == EPropertyTypes::TYPE_FNAME)
            || (propertyType == EPropertyTypes::TYPE_FSTRING)
            || (propertyType == EPropertyTypes::TYPE_FSCRIPTDELEGATE)
            || (propertyType == EPropertyTypes::TYPE_FPOINTER)
            || (propertyType == EPropertyTypes::TYPE_FSTRUCT));
    }

    bool IsBitField(uint32_t arrayDim)
    {
        return (arrayDim && (arrayDim < 4));
    }

    bool CantMemcpy(EPropertyTypes propertyType)
    {
        return ((propertyType == EPropertyTypes::TYPE_LONG)
            || (propertyType == EPropertyTypes::TYPE_ULONG)
            || (propertyType == EPropertyTypes::TYPE_BOOL));
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
            std::string superClassNameCPP = Printer::ToUpper(CreateValidName(superClass->GetNameCPP()));
            uniqueName += ("_" + superClassNameCPP);
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
        size_t gctW = functionName.find("GetCurrentTime");
        size_t goW = functionName.find("GetObject");
        size_t dfW = functionName.find("DeleteFile");
        size_t dtW = functionName.find("DrawText");
        size_t smW = functionName.find("SendMessage");
        size_t gmW = functionName.find("GetMessage");

        if ((gctW != std::string::npos)
            || (goW != std::string::npos)
            || (dfW != std::string::npos)
            || (dtW != std::string::npos)
            || (smW != std::string::npos)
            || (gmW != std::string::npos))
        {
            functionName += "W";
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

                    return EPropertyTypes::TYPE_FSTRUCT;
                }
            }
            else if (uProperty->IsA<UStrProperty>())
            {
                outPropertyType = "class FString";

                if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                {
                    outPropertyType = ("const " + outPropertyType + "&");
                }

                return EPropertyTypes::TYPE_FSTRING;
            }
            else if (uProperty->IsA<UQWordProperty>())
            {
                outPropertyType = "uint64_t";
                return EPropertyTypes::TYPE_UINT64;
            }
            else if (uProperty->IsA<UObjectProperty>())
            {
                UObjectProperty* objectProperty = static_cast<UObjectProperty*>(uProperty);

                if (objectProperty && objectProperty->PropertyClass)
                {
                    outPropertyType = ("class " + Utils::CreateValidName(objectProperty->PropertyClass->GetNameCPP()) + "*");
                    return EPropertyTypes::TYPE_UPOINTER;
                }
            }
            else if (uProperty->IsA<UClassProperty>())
            {
                UClassProperty* classProperty = static_cast<UClassProperty*>(uProperty);

                if (classProperty && classProperty->MetaClass)
                {
                    outPropertyType = ("class " + Utils::CreateValidName(classProperty->MetaClass->GetNameCPP()) + "*");
                    return EPropertyTypes::TYPE_UPOINTER;
                }
            }
            else if (uProperty->IsA<UNameProperty>())
            {
                outPropertyType = "struct FName";

                if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                {
                    outPropertyType = ("const " + outPropertyType + "&");
                }

                return EPropertyTypes::TYPE_FNAME;
            }
            else if (uProperty->IsA<UMapProperty>())
            {
                UMapProperty* mapProperty = static_cast<UMapProperty*>(uProperty);
                std::string mapKey, mapValue;

                if (mapProperty && mapProperty->Key && mapProperty->Value)
                {
                    if (GetPropertyTypeInternal(mapProperty->Key, mapKey, bIgnoreEnum, false, bIsBitField) != EPropertyTypes::TYPE_UNKNOWN && GetPropertyTypeInternal(mapProperty->Value, mapValue, bIgnoreEnum, bDescription, bIsBitField) != EPropertyTypes::TYPE_UNKNOWN)
                    {
                        outPropertyType = ("class TMap<" + mapKey + ", " + mapValue + ">");

                        if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                        {
                            outPropertyType = ("const " + outPropertyType + "&");
                        }

                        return EPropertyTypes::TYPE_TMAP;
                    }
                }
            }
            else if (uProperty->IsA<UIntProperty>())
            {
                outPropertyType = "int32_t";
                return EPropertyTypes::TYPE_INT32;
            }
            else if (uProperty->IsA<UInterfaceProperty>())
            {
                UInterfaceProperty* interfaceProperty = static_cast<UInterfaceProperty*>(uProperty);

                if (interfaceProperty && interfaceProperty->InterfaceClass)
                {
                    outPropertyType = ("class " + Utils::CreateValidName(interfaceProperty->InterfaceClass->GetNameCPP()) + "*");
                    return EPropertyTypes::TYPE_UPOINTER;
                }
            }
            else if (uProperty->IsA<UFloatProperty>())
            {
                outPropertyType = "float";
                return EPropertyTypes::TYPE_FLOAT;
            }
            else if (uProperty->IsA<UDelegateProperty>())
            {
                outPropertyType = "struct FScriptDelegate";

                if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                {
                    outPropertyType = ("const " + outPropertyType + "&");
                }

                return EPropertyTypes::TYPE_FSCRIPTDELEGATE;
            }
            else if (uProperty->IsA<UByteProperty>())
            {
                if (!bIgnoreEnum && GConfig::UsingEnumClasses())
                {
                    UByteProperty* byteProperty = static_cast<UByteProperty*>(uProperty);

                    if (byteProperty && byteProperty->Enum)
                    {
                        outPropertyType = EnumGenerator::GenerateEnumName(byteProperty->Enum);
                        return EPropertyTypes::TYPE_UINT8;
                    }
                }

                outPropertyType = "uint8_t";
                return EPropertyTypes::TYPE_UINT8;
            }
            else if (uProperty->IsA<UBoolProperty>())
            {
                if (bIsBitField)
                {
                    outPropertyType = "uint32_t";
                    return EPropertyTypes::TYPE_UINT32;
                }

                outPropertyType = "bool";
                return EPropertyTypes::TYPE_BOOL;
            }
            else if (uProperty->IsA<UArrayProperty>())
            {
                UArrayProperty* arrayProperty = static_cast<UArrayProperty*>(uProperty);

                if (arrayProperty && arrayProperty->Inner)
                {
                    std::string innerProperty;

                    if (GetPropertyTypeInternal(arrayProperty->Inner, innerProperty, bIgnoreEnum, false, bIsBitField) != EPropertyTypes::TYPE_UNKNOWN)
                    {
                        outPropertyType = ("class TArray<" + innerProperty + ">");

                        if (bDescription && !(uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                        {
                            outPropertyType = ("const " + outPropertyType + "&");
                        }

                        return EPropertyTypes::TYPE_TARRAY;
                    }
                }
            }
        }

        return EPropertyTypes::TYPE_UNKNOWN;
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
    static std::multimap<std::string, std::string> m_generatedConsts;

    void GenerateConst(std::ofstream& file, class UConst* constant)
    {
        std::string constName = Utils::CreateValidName(constant->GetName());

        if (constName.find("Default__") == std::string::npos)
        {
#ifndef NO_LOGGING
            if (Generator::LogFile.is_open())
            {
                std::string propertyTitle = "Const: ";

                Generator::LogFile << propertyTitle;
                Printer::FillRight(Generator::LogFile, ' ', constName.size());
                Generator::LogFile << constName;
                Printer::FillRight(Generator::LogFile, ' ', (75 - (constName.size() + propertyTitle.size())));
                Generator::LogFile << " - Instance: " << Printer::Hex(constant) << std::endl;
            }
#endif
            std::string constValue = constant->Value.ToString();
            size_t mapSize = m_generatedConsts.count(constName);

            if (mapSize == 0)
            {
                m_generatedConsts.insert({ constName, constValue });
                file << "#define CONST_" << constName;
                Printer::FillLeft(file, ' ', (GConfig::GetConstSpacing() - constName.length()));
                file << " " << constValue << "\n";
            }
            else if (!Utils::MapExists(m_generatedConsts, constName, constValue))
            {
                m_generatedConsts.insert({ constName, constValue });
                file << "#define CONST_" << constName << Printer::Decimal(mapSize, EWidthTypes::BYTE);
                Printer::FillLeft(file, ' ', (GConfig::GetConstSpacing() - constName.length()));
                file << " " << constValue << "\n";
            }
        }
    }

    void ProcessConsts(std::ofstream& file, class UObject* packageObj)
    {
        for (UObject* uObject : *UObject::GObjObjects())
        {
            if (uObject && uObject->IsA<UConst>())
            {
                UObject* packageObject = uObject->GetPackageObj();

                if (packageObject && (packageObject == packageObj))
                {
                    GenerateConst(file, static_cast<UConst*>(uObject));
                }
            }
        }
    }
}

namespace EnumGenerator
{
    static std::map<std::string, std::vector<class UEnum*>> m_enumCache;
    static std::map<class UEnum*, std::string> m_generatedNames;

    std::string GenerateEnumName(class UEnum* uEnum)
    {
        if (m_enumCache.empty())
        {
            for (UObject* uObject : *UObject::GObjObjects())
            {
                if (uObject && uObject->IsA<UEnum>())
                {
                    UEnum* newEnum = static_cast<UEnum*>(uObject);
                    std::string enumName = Utils::CreateValidName(newEnum->GetName());

                    if (!m_enumCache.contains(enumName))
                    {
                        m_enumCache[enumName] = { newEnum };
                    }
                    else
                    {
                        m_enumCache[enumName].push_back(newEnum);
                    }
                }
            }
        }

        if (uEnum)
        {
            if (!m_generatedNames.contains(uEnum))
            {
                std::string enumName = Utils::CreateValidName(uEnum->GetName());

                if (m_enumCache.contains(enumName))
                {
                    if (m_enumCache[enumName].size() > 1)
                    {
                        size_t index = 0;

                        for (UEnum* cachedEnum : m_enumCache[enumName])
                        {
                            if (cachedEnum)
                            {
                                if (cachedEnum != uEnum)
                                {
                                    index++;
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }

                        enumName += ("_" + std::to_string(index));
                    }
                }

                if (enumName.find("E") != 0)
                {
                    enumName = ("E" + enumName);
                }

                m_generatedNames[uEnum] = enumName;
            }

            return m_generatedNames[uEnum];
        }

        return "UnknownName";
    }

    void GenerateEnum(std::ofstream& file, class UEnum* uEnum)
    {
        std::ostringstream enumStream;
        std::ostringstream propertyStream;
        std::string enumName = GenerateEnumName(uEnum);

        if (enumName.find("Default__") == std::string::npos)
        {
#ifndef NO_LOGGING
            if (Generator::LogFile.is_open())
            {
                std::string propertyTitle = "Enum: ";

                Generator::LogFile << propertyTitle;
                Printer::FillRight(Generator::LogFile, ' ', enumName.size());
                Generator::LogFile << enumName;
                Printer::FillRight(Generator::LogFile, ' ', (75 - (enumName.size() + propertyTitle.size())));
                Generator::LogFile << " - Instance: " << Printer::Hex(uEnum) << std::endl;
            }
#endif
            if (GConfig::UsingEnumClasses())
            {
                enumStream << "// " << uEnum->GetFullName() << "\n";
                enumStream << "enum class " << enumName << " : " << GConfig::GetEnumClassType() << "\n";
                enumStream << "{" << "\n";
            }
            else
            {
                enumStream << "// " << uEnum->GetFullName() << "\n";
                enumStream << "enum " << enumName << "\n";
                enumStream << "{" << "\n";
            }

            std::map<std::string, uint32_t> propertiesMap;
            int32_t lastName = (uEnum->Names.size() - 1);

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
                    std::string rawName = uEnum->GetName();

                    if (rawName.find("E") != 0)
                    {
                        rawName = ("E" + rawName);
                    }

                    propertyName = (rawName + "_" + propertyName);
                }

                if (propertiesMap.count(propertyName) == 0)
                {
                    propertiesMap[propertyName] = 1;
                    propertyStream << propertyName;
                }
                else
                {
                    propertyStream << propertyName << Printer::Decimal(propertiesMap[propertyName], EWidthTypes::BYTE);
                    propertiesMap[propertyName]++;
                }

                enumStream << "\t";
                Printer::FillLeft(enumStream, ' ', GConfig::GetEnumSpacing());
                enumStream << propertyStream.str() << " = " << std::to_string(i);

                if (i != lastName)
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

    void ProcessEnums(std::ofstream& file, class UObject* packageObj)
    {
        for (UObject* uObject : *UObject::GObjObjects())
        {
            if (uObject && uObject->IsA<UEnum>())
            {
                UObject* packageObject = uObject->GetPackageObj();

                if (packageObject && (packageObject == packageObj))
                {
                    GenerateEnum(file, static_cast<UEnum*>(uObject));
                }
            }
        }
    }
}

namespace StructGenerator
{
    std::vector<std::string> vGeneratedStructs{};

    class UScriptStruct* FindLargestStruct(const std::string& structFullName)
    {
        size_t propertySize = 0;
        UScriptStruct* largestStruct = nullptr;

        for (UObject* object : *UObject::GObjObjects())
        {
            if (object && object->IsA<UScriptStruct>())
            {
                if (object->GetFullName() == structFullName)
                {
                    UScriptStruct* scriptStruct = static_cast<UScriptStruct*>(object);

                    if (scriptStruct->PropertySize >= propertySize)
                    {
                        largestStruct = scriptStruct;
                        propertySize = scriptStruct->PropertySize;
                    }
                }
            }
        }

        return largestStruct;
    }

    void GenerateStructFields(std::ofstream& structStream, EClassTypes structType)
    {
        std::ostringstream propertyStream;

        size_t localSize = 0;
        size_t startOffset = 0;
        size_t missedOffset = 0;
        size_t lastOffset = 0;

        localSize = Member::GetClassSize(structType);
        startOffset = Member::GetClassOffset(structType);
        std::map<uintptr_t, Member> members = Member::GetRegistered(structType);

        if (!members.empty())
        {
            lastOffset = startOffset;
            int32_t unknownDataIndex = 0;

            for (const auto& member : members)
            {
                if ((lastOffset + member.second.Size) < member.second.Offset)
                {
                    missedOffset = (member.second.Offset - lastOffset);

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

                structStream << "\t" << member.second.Label << " // " << Printer::Hex(member.second.Offset, EWidthTypes::SIZE);
                structStream << " (" << Printer::Hex(member.second.Size, EWidthTypes::SIZE) << ")\n";
                lastOffset = (member.second.Offset + member.second.Size);
            }

            if ((structType != EClassTypes::STRUCT_FNAMEENTRY) && (lastOffset < localSize))
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
            Generator::LogFile << "Error: No registered members found for class type \"" << std::to_string(static_cast<int32_t>(structType)) << "\"\n";
#endif
            Utils::Messagebox("Error: No registered members found for class type \"" + std::to_string(static_cast<int32_t>(structType)) + "\"!", (MB_OK | MB_ICONERROR));
        }
    }

    void GenerateStruct(std::ofstream& file, class UScriptStruct* scriptStruct)
    {
        std::ostringstream structStream;
        std::ostringstream propertyStream;
        std::ostringstream flagStream;

        std::string structFullName = scriptStruct->GetFullName();
        std::string structName = Utils::CreateValidName(scriptStruct->GetName());
        std::string structNameCPP = Utils::CreateValidName(scriptStruct->GetNameCPP());
        std::string structOuterNameCPP = Utils::CreateValidName(scriptStruct->Outer->GetNameCPP());

        if (structNameCPP == "FPointer" || structNameCPP == "FQWord")
        {
            return;
        }
#ifndef NO_LOGGING
        if (Generator::LogFile.is_open())
        {
            std::string propertyTitle = "ScriptStruct: ";

            Generator::LogFile << propertyTitle;
            Printer::FillRight(Generator::LogFile, ' ', structName.size());
            Generator::LogFile << structName;
            Printer::FillRight(Generator::LogFile, ' ', (75 - (structName.size() + propertyTitle.size())));
            Generator::LogFile << " - Instance: " << Printer::Hex(scriptStruct) << std::endl;
        }
#endif
        structStream << "// " << structFullName << "\n";

        size_t size = 0;
        size_t lastOffset = 0;
        size_t missedOffset = 0;

        UScriptStruct* superField = static_cast<UScriptStruct*>(scriptStruct->SuperField);
        uint32_t structCount = UObject::CountObject<UScriptStruct>(structName);

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

                if (Retrievers::GetPropertyTypeInternal(uProperty, propertyType, true, false, true) != EPropertyTypes::TYPE_UNKNOWN)
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
                propertyStream << "[" << Printer::Hex(missedOffset, EWidthTypes::NONE) << "];";

                structStream << "\t";
                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                structStream << "uint8_t ";
                Printer::FillLeft(structStream, ' ', GConfig::GetStructSpacing());
                structStream << propertyStream.str() << "\t\t// " << Printer::Hex(lastOffset, EWidthTypes::SIZE);
                structStream << " (" << Printer::Hex(missedOffset, EWidthTypes::SIZE) << ") MISSED OFFSET\n";
                Printer::Empty(propertyStream);
            }
        }

        structStream << "};\n\n";

        file << structStream.str();
    }

    void GenerateStructProperties(std::ofstream& file, class UScriptStruct* scriptStruct, class UObject* packageObj)
    {
        UObject* packageObject = scriptStruct->GetPackageObj();

        if (packageObject && packageObject == packageObj)
        {
            std::string structFullName = scriptStruct->GetFullName();
            std::string structName = scriptStruct->GetName();

            if ((structName.find("Default__") != std::string::npos) && (structName.find("<uninitialized>") != std::string::npos))
            {
                return;
            }

            if (find(vGeneratedStructs.begin(), vGeneratedStructs.end(), structFullName) == vGeneratedStructs.end())
            {
                scriptStruct = FindLargestStruct(structFullName);

                if (scriptStruct->SuperField && scriptStruct->SuperField != scriptStruct && std::find(vGeneratedStructs.begin(), vGeneratedStructs.end(), (static_cast<UScriptStruct*>(scriptStruct->SuperField))->GetFullName()) == vGeneratedStructs.end())
                {
                    GenerateStructProperties(file, static_cast<UScriptStruct*>(scriptStruct->SuperField), packageObj);
                }

                for (UProperty* structChild = static_cast<UProperty*>(scriptStruct->Children); structChild; structChild = static_cast<UProperty*>(structChild->Next))
                {
                    std::string propertyType;
                    EPropertyTypes childType = Retrievers::GetPropertyType(structChild, propertyType);

                    if (childType == EPropertyTypes::TYPE_FSTRUCT)
                    {
                        UScriptStruct* propertyStruct = static_cast<UScriptStruct*>(static_cast<UStructProperty*>(structChild)->Struct);

                        if (propertyStruct && propertyStruct != scriptStruct && std::find(vGeneratedStructs.begin(), vGeneratedStructs.end(), propertyStruct->GetFullName()) == vGeneratedStructs.end())
                        {
                            GenerateStructProperties(file, propertyStruct, packageObj);
                        }
                    }
                    else if (childType == EPropertyTypes::TYPE_TARRAY)
                    {
                        UScriptStruct* propertyStruct = static_cast<UScriptStruct*>(static_cast<UStructProperty*>(static_cast<UArrayProperty*>(structChild)->Inner)->Struct);

                        if (propertyStruct != scriptStruct
                            && Retrievers::GetPropertyType(static_cast<UArrayProperty*>(structChild)->Inner, propertyType) == EPropertyTypes::TYPE_FSTRUCT
                            && std::find(vGeneratedStructs.begin(), vGeneratedStructs.end(), propertyStruct->GetFullName()) == vGeneratedStructs.end())
                        {
                            GenerateStructProperties(file, propertyStruct, packageObj);
                        }
                    }
                }

                GenerateStruct(file, scriptStruct);
                vGeneratedStructs.push_back(structFullName);
            }
        }
    }

    void ProcessStructs(std::ofstream& file, class UObject* packageObj)
    {
        for (UObject* uObject : *UObject::GObjObjects())
        {
            if (uObject && uObject->IsA<UScriptStruct>())
            {
                UObject* packageObject = uObject->GetPackageObj();

                if (packageObject && (packageObject == packageObj))
                {
                    GenerateStructProperties(file, static_cast<UScriptStruct*>(uObject), packageObject);
                }
            }
        }
    }
}

namespace ClassGenerator
{
    extern std::unordered_map<std::string, int32_t> mGeneratedClasses{};

    void GenerateClassFields(std::ostringstream& classStream, class UClass* uClass, EClassTypes classType)
    {
        std::ostringstream propertyStream;

        size_t localSize = 0;
        size_t startOffset = 0;
        size_t missedOffset = 0;
        size_t lastOffset = 0;

        localSize = Member::GetClassSize(classType);
        startOffset = Member::GetClassOffset(classType);
        std::map<uintptr_t, Member> members = Member::GetRegistered(classType);

        if (!members.empty())
        {
            if (uClass->PropertySize == localSize)
            {
                lastOffset = startOffset;
                int32_t unknownDataIndex = 0;

                for (const auto& member : members)
                {
                    if ((lastOffset + member.second.Size) < member.second.Offset)
                    {
                        missedOffset = (member.second.Offset - lastOffset);

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

                    classStream << "\t" << member.second.Label << " // " << Printer::Hex(member.second.Offset, EWidthTypes::SIZE);
                    classStream << " (" << Printer::Hex(member.second.Size, EWidthTypes::SIZE) << ")\n";

                    lastOffset = (member.second.Offset + member.second.Size);
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
                    Generator::LogFile << "Error: REGISTERED CLASS SIZE: " << std::to_string(localSize) << "\n";
                    Generator::LogFile << "Error: ACTUAL CLASS SIZE: " << std::to_string(uClass->PropertySize) << "\n";
                }
#endif
                Utils::Messagebox("Error: Incorrect class size detected, check the log file for more details!", (MB_OK | MB_ICONERROR));
            }
        }
        else
        {
#ifndef NO_LOGGING
            Generator::LogFile << "Error: No registered members found for class \"" << uClass->GetName() << "\"\n";
#endif
            Utils::Messagebox("Error: No registered members found for class \"" + uClass->GetName() + "\"!", (MB_OK | MB_ICONERROR));
        }
    }

    void GenerateClass(std::ofstream& file, class UClass* uClass)
    {
        std::ostringstream classStream;
        std::ostringstream propertyStream;
        std::ostringstream flagStream;

        size_t size = 0;
        size_t lastOffset = 0;
        size_t missedOffset = 0;

        UClass* uSuperClass = static_cast<UClass*>(uClass->SuperField);
        std::string className = Utils::CreateValidName(uClass->GetName());
        std::string classNameCPP = Utils::CreateValidName(uClass->GetNameCPP());
        std::string classFullName = uClass->GetFullName();

#ifndef NO_LOGGING
        if (Generator::LogFile.is_open())
        {
            std::string propertyTitle = "Class: ";

            Generator::LogFile << propertyTitle;
            Printer::FillRight(Generator::LogFile, ' ', className.size());
            Generator::LogFile << className;
            Printer::FillRight(Generator::LogFile, ' ', (75 - (className.size() + propertyTitle.size())));
            Generator::LogFile << " - Instance: " << Printer::Hex(uClass) << std::endl;
        }
#endif
        std::vector<UProperty*> classProperties;

        for (UProperty* uProperty = static_cast<UProperty*>(uClass->Children); uProperty; uProperty = static_cast<UProperty*>(uProperty->Next))
        {
            if (!uProperty->IsA<UFunction>()
                && !uProperty->IsA<UConst>()
                && !uProperty->IsA<UEnum>()
                && !uProperty->IsA<UScriptStruct>()
                && (uProperty->ElementSize > 0)
                && (!uSuperClass || (uSuperClass && uSuperClass != uClass && uProperty->Offset >= uSuperClass->PropertySize)))
            {
                classProperties.push_back(uProperty);
            }
        }

        std::sort(classProperties.begin(), classProperties.end(), Utils::SortProperty);
        classStream << "// " << classFullName << "\n";

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

        if (uClass == UField::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UFIELD); }
        else if (uClass == UEnum::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UENUM); }
        else if (uClass == UConst::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UCONST); }
        else if (uClass == UProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UPROPERTY); }
        else if (uClass == UStruct::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_USTRUCT); }
        else if (uClass == UFunction::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UFUNCTION); }
        else if (uClass == UStructProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_USTRUCTPROPERTY); }
        else if (uClass == UObjectProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UOBJECTPROPERTY); }
        else if (uClass == UMapProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UMAPPROPERTY); }
        else if (uClass == UInterfaceProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UINTERFACEPROPERTY); }
        else if (uClass == UDelegateProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UDELEGATEPROPERTY); }
        else if (uClass == UByteProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UBYTEPROPERTY); }
        else if (uClass == UBoolProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UBOOLPROPERTY); }
        else if (uClass == UArrayProperty::StaticClass()) { GenerateClassFields(classStream, uClass, EClassTypes::CLASS_UARRAYPROPERTY); }
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

                    if (Retrievers::GetPropertyTypeInternal(uProperty, propertyType, false, false, true) != EPropertyTypes::TYPE_UNKNOWN)
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
            classStream << "\t\t\tuClassPointer = reinterpret_cast<UClass*>(UObject::GObjObjects()->At(" << Utils::CreateValidName(Generator::GenerateIndex(uClass)) << "));\n";
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
            classStream << "\t\t\tuClassPointer = UObject::FindClass(\"" << classFullName << "\");\n";
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

        FunctionGenerator::GenerateFunctionDescription(file, uClass);

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

        file << classStream.str();
    }

    void GenerateClassProperties(std::ofstream& file, class UClass* uClass, class UObject* uPackageObj)
    {
        UObject* packageObject = uClass->GetPackageObj();

        if (packageObject)
        {
            if (std::find(Generator::vIncludes.begin(), Generator::vIncludes.end(), uPackageObj) == Generator::vIncludes.end())
            {
                Generator::vIncludes.push_back(uPackageObj);
            }

            if (packageObject != uPackageObj)
            {
                const auto& itPO = std::find(Generator::vIncludes.begin(), Generator::vIncludes.end(), packageObject);
                const auto& itPTP = std::find(Generator::vIncludes.begin(), Generator::vIncludes.end(), uPackageObj);

                if (itPO == Generator::vIncludes.end())
                {
                    Generator::vIncludes.insert(itPTP, packageObject);
                }
                else if (itPO >= itPTP)
                {
                    Generator::vIncludes.insert(itPTP, packageObject);
                    Generator::vIncludes.erase(itPO);
                }

                return;
            }

            std::string className = Utils::CreateValidName(uClass->GetName());
            std::string classFullName = uClass->GetFullName();

            if (className.find("Default__") == std::string::npos)
            {
                if (mGeneratedClasses.find(classFullName) == mGeneratedClasses.end())
                {
                    if (uClass->SuperField && uClass->SuperField != uClass)
                    {
                        if (mGeneratedClasses.find(uClass->SuperField->GetFullName()) == mGeneratedClasses.end())
                        {
                            GenerateClassProperties(file, static_cast<UClass*>(uClass->SuperField), uPackageObj);
                        }
                    }

                    GenerateClass(file, uClass);
                    mGeneratedClasses.emplace(classFullName, uClass->ObjectInternalInteger);
                }
            }
        }
    }

    void ProcessClasses(std::ofstream& file, class UObject* uPackageObj)
    {
        for (UObject* uObject : *UObject::GObjObjects())
        {
            if (uObject && uObject->IsA<UClass>())
            {
                UObject* packageObject = uObject->GetPackageObj();

                if (packageObject && (packageObject == uPackageObj))
                {
                    GenerateClassProperties(file, static_cast<UClass*>(uObject), packageObject);
                }
            }
        }
    }
}

namespace ParameterGenerator
{
    void GenerateParameter(std::ofstream& file, class UClass* uClass)
    {
        std::ostringstream parameterStream;
        std::ostringstream propertyStream;
        std::ostringstream flagStream;

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
                    parameterStream << " (iNative[" << std::to_string(uFunction->iNative) << "])";
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
                        EPropertyTypes returnPropertyType = EPropertyTypes::TYPE_UNKNOWN;

                        if ((uProperty->PropertyFlags & EPropertyFlags::CPF_OutParm) || (uProperty->PropertyFlags & EPropertyFlags::CPF_ReturnParm))
                        {
                            returnPropertyType = Retrievers::GetPropertyType(uProperty, propertyType, (uProperty->IsA<UArrayProperty>() ? false : true));
                        }
                        else
                        {
                            returnPropertyType = Retrievers::GetPropertyTypeInternal(uProperty, propertyType, true, false, true);
                        }

                        if (returnPropertyType != EPropertyTypes::TYPE_UNKNOWN)
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

    void ProcessParameters(std::ofstream& file, UObject* packageObj)
    {
        for (UObject* uObject : *UObject::GObjObjects())
        {
            if (uObject && uObject->IsA<UClass>())
            {
                UObject* packageObject = uObject->GetPackageObj();

                if (packageObject && (packageObject == packageObj))
                {
                    GenerateParameter(file, static_cast<UClass*>(uObject));
                }
            }
        }
    }
}

namespace FunctionGenerator
{
    void GenerateVirtualFunctions(std::ofstream& file)
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
            file << "\n\t// FIX PROCESS EVENT IN CONFIGURATION.CPP, INVALID INDEX";
            Utils::Messagebox("Warning: Process event is not configured correctly in \"Configuration.cpp\", you set \"UsingOffsets\" to true yet you did not provide a valid index for process event!", (MB_OK | MB_ICONWARNING));
        }

        if (processEventAddress != 0)
        {
            file << "\n\t// Virtual Functions\n\n";

            for (int32_t index = 0; index < 512; index++)
            {
                uintptr_t virtualFunction = reinterpret_cast<uintptr_t*>(UObject::StaticClass()->VfTableObject.Dummy)[index];

                if (virtualFunction == processEventAddress)
                {
                    file << "\tvirtual void ProcessEvent(class UFunction* uFunction, void* uParams, void* uResult = nullptr);";
                    file << "\t\t\t// " << Printer::Hex(virtualFunction, sizeof(virtualFunction)) << "\n";
                    break;
                }
                else
                {
                    file << "\tvirtual void VirtualFunction" << std::to_string(index) << "();";
                    file << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t// " << Printer::Hex(virtualFunction, sizeof(virtualFunction)) << "\n";
                }
            }
        }
        else
        {
            file << "\n\t// FIX PROCESS EVENT IN CONFIGURATION.CPP, INVALID ADDRESS";
            Utils::Messagebox("Warning: Process event is not configured correctly in \"Configuration.cpp\", failed to find a valid address!", (MB_OK | MB_ICONWARNING));
        }
    }

    void GenerateFunctionCode(std::ofstream& file, class UClass* uClass)
    {
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
                    codeStream << " (iNative[" << std::to_string(uFunction->iNative) << "])";
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

                if (propertyReturnParm.first && Retrievers::GetPropertyTypeDesc(propertyReturnParm.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    if (Retrievers::GetPropertyTypeDesc(propertyPair.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    if (Retrievers::GetPropertyType(propertyPair.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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

                if (propertyReturnParm.first && Retrievers::GetPropertyType(propertyReturnParm.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    if (Retrievers::GetPropertyTypeDesc(propertyPair.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    if (Retrievers::GetPropertyType(propertyPair.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    codeStream << "\t\tuFn" << functionName << " = reinterpret_cast<UFunction*>(UObject::GObjObjects()->At(" << Utils::CreateValidName(Generator::GenerateIndex(uFunction)) << "));\n";
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

                        if (propertyTypeResult != EPropertyTypes::TYPE_UNKNOWN)
                        {
                            if ((propertyTypeResult != EPropertyTypes::TYPE_BOOL) && (!Utils::CantMemcpy(propertyTypeResult) || !Utils::IsBitField(propertyPair.first->ArrayDim)))
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

                        if (propertyTypeResult != EPropertyTypes::TYPE_UNKNOWN)
                        {
                            if ((propertyTypeResult != EPropertyTypes::TYPE_BOOL) && (!Utils::CantMemcpy(propertyTypeResult) || !Utils::IsBitField(propertyPair.first->ArrayDim)))
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
                    codeStream << "\tuFn" << functionName << "->iNative = " << std::to_string(uFunction->iNative) << ";\n";
                }

                if (!propertyOutParams.empty())
                {
                    codeStream << "\n";

                    for (const auto& propertyPair : propertyOutParams)
                    {
                        if (propertyPair.first->PropertyFlags & EPropertyFlags::CPF_OutParm)
                        {
                            EPropertyTypes propertyTypeResult = Retrievers::GetPropertyType(propertyPair.first, propertyType);

                            if (propertyTypeResult != EPropertyTypes::TYPE_UNKNOWN)
                            {
                                if ((propertyTypeResult != EPropertyTypes::TYPE_BOOL) && (!Utils::CantMemcpy(propertyTypeResult) || !Utils::IsBitField(propertyPair.first->ArrayDim)))
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

                if (propertyReturnParm.first && Retrievers::GetPropertyType(propertyReturnParm.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
                {
                    codeStream << "\n\treturn " << functionName << "_Params." << propertyReturnParm.second << ";\n";
                }

                codeStream << "};\n\n";
            }
        }

        file << codeStream.str();
    }

    void GenerateFunctionDescription(std::ofstream& file, class UClass* uClass)
    {
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

                if (propertyReturnParm.first && Retrievers::GetPropertyType(propertyReturnParm.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    if (Retrievers::GetPropertyTypeDesc(propertyPair.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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
                    if (Retrievers::GetPropertyType(propertyPair.first, propertyType) != EPropertyTypes::TYPE_UNKNOWN)
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

        file << functionStream.str();
    }

    void ProcessFunctions(std::ofstream& file, class UObject* packageObj)
    {
        for (UObject* uObject : *UObject::GObjObjects())
        {
            if (uObject && uObject->IsA<UClass>())
            {
                UObject* packageObject = uObject->GetPackageObj();

                if (packageObject && (packageObject == packageObj))
                {
                    GenerateFunctionCode(file, static_cast<UClass*>(uObject));
                }
            }
        }
    }
}

namespace Generator
{
    bool GlobalsInitialized = false;
    std::ofstream LogFile;
    std::vector<class UObject*> vPackages{};
    std::vector<class UObject*> vIncludes{};
    std::vector<std::pair<std::string, int32_t>> vConstants{};

    std::string GenerateIndex(class UObject* uObject, bool bPushBack)
    {
        std::string objectFullName = Printer::ToUpper(Utils::CreateValidName(uObject->GetFullName()));

        for (char& c : objectFullName)
        {
            if (c == '.')
            {
                c = '_';
            }
        }

        objectFullName = ("IDX_" + objectFullName);

        if (GConfig::UsingConstants() && bPushBack)
        {
            std::pair<std::string, int32_t> pConstant = std::make_pair(objectFullName, uObject->ObjectInternalInteger);

            if (std::find(vConstants.begin(), vConstants.end(), pConstant) == vConstants.end())
            {
                vConstants.push_back(pConstant);
            }
        }

        return objectFullName;
    }

    void GenerateConstants()
    {
        if (GConfig::UsingConstants())
        {
            std::ofstream constantsFile(GConfig::GetOutputPath() / GConfig::GetGameNameShort() / "SdkConstants.hpp");
            constantsFile << "#pragma once\n";

            for (UObject* object : *UObject::GObjObjects())
            {
                if (object)
                {
                    if (object->IsA<UFunction>() || object->IsA<UClass>())
                    {
                        std::string objectFullName = Generator::GenerateIndex(object, false);
                        std::pair<std::string, int32_t> constant = std::make_pair(objectFullName, object->ObjectInternalInteger);

                        if (std::find(vConstants.begin(), vConstants.end(), constant) == vConstants.end())
                        {
                            vConstants.push_back(constant);
                        }
                    }
                }
            }

            for (const auto& constant : vConstants)
            {
                constantsFile << "#define " << constant.first;
                Printer::FillRight(constantsFile, ' ', (175 - constant.first.size()));
                constantsFile << std::to_string(constant.second) << "\n";
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

        for (auto& package : vIncludes)
        {
            if (package)
            {
                std::string packageName = package->GetName();
                headersFile << "#include \"SDK_HEADERS\\" << packageName << "_structs.hpp\"\n";
                headersFile << "#include \"SDK_HEADERS\\" << packageName << "_classes.hpp\"\n";
                headersFile << "#include \"SDK_HEADERS\\" << packageName << "_parameters.hpp\"\n";
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
            definesFile << "#include <Windows.h>\n";
        }

        definesFile << "#include <algorithm>\n";
        definesFile << "#include <locale>\n";
        definesFile << "#include <stdlib.h>\n";
        definesFile << "#include <xlocale>\n";
        definesFile << "#include <ctype.h>\n";
        definesFile << "#include <chrono>\n";
        definesFile << "#include <thread>\n";
        definesFile << "#include <vector>\n";
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
        definesFile << PiecesOfCode::TMap_Class;

        Printer::Section(definesFile, "Globals");
        definesFile << "extern TArray<class UObject*>* GObjects;\n";
        definesFile << "extern TArray<struct FNameEntry*>* GNames;\n";

        Printer::Section(definesFile, "Structs");
        definesFile << PiecesOfCode::FNameEntry_Struct << "\n";
        StructGenerator::GenerateStructFields(definesFile, EClassTypes::STRUCT_FNAMEENTRY);

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
        definesFile << "TArray<class UObject*>* GObjects{};\n";
        definesFile << "TArray<struct FNameEntry*>* GNames{};\n\n";

        Printer::Footer(definesFile, false);
        definesFile.close();
    }

    void ProcessPackages(const std::filesystem::path& directory)
    {
        if (std::filesystem::exists(directory))
        {
            for (UObject* uObject : *UObject::GObjObjects())
            {
                if (uObject && uObject->IsA<UClass>())
                {
                    UObject* packageObject = uObject->GetPackageObj();

                    if (packageObject)
                    {
                        if (find(vPackages.begin(), vPackages.end(), packageObject) == vPackages.end())
                        {
                            vPackages.push_back(packageObject);
                            std::string packageName = Utils::CreateValidName(packageObject->GetName());
#ifndef NO_LOGGING
                            LogFile << "\nProcessing Package: " << packageName << "\n" << std::endl;
#endif
                            std::ofstream file;

                            // Structs
                            file.open(directory / (packageName + "_structs.hpp"));
                            Printer::Header(file, (packageName + "_structs"), "hpp", true);
                            Printer::Section(file, "Structs");
                            StructGenerator::ProcessStructs(file, packageObject);
                            Printer::Footer(file, true);
                            file.close();

                            // Classes
                            file.open(directory / (packageName + "_classes.hpp"));
                            Printer::Header(file, (packageName + "_classes"), "hpp", true);
                            Printer::Section(file, "Constants");
                            ConstGenerator::ProcessConsts(file, packageObject);
                            Printer::Section(file, "Enums");
                            EnumGenerator::ProcessEnums(file, packageObject);
                            Printer::Section(file, "Classes");
                            ClassGenerator::ProcessClasses(file, packageObject);
                            Printer::Footer(file, true);
                            file.close();

                            // Parameters
                            file.open(directory / (packageName + "_parameters.hpp"));
                            Printer::Header(file, (packageName + "_parameters"), "hpp", true);
                            Printer::Section(file, "Parameters");
                            ParameterGenerator::ProcessParameters(file, packageObject);
                            Printer::Footer(file, true);
                            file.close();

                            // Functions
                            file.open(directory / (packageName + "_classes.cpp"));
                            Printer::Header(file, (packageName + "_classes"), "cpp", true);
                            Printer::Section(file, "Functions");
                            FunctionGenerator::ProcessFunctions(file, packageObject);
                            Printer::Footer(file, true);
                            file.close();
                        }
                    }
                }
            }
        }
        else
        {
            Utils::Messagebox("Failed locate the given directory, cannot generate an SDK at this time!", (MB_OK | MB_ICONERROR));
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
                Utils::Messagebox("SDK generation has started, do not close the game until prompted to do so!", (MB_OK | MB_ICONINFORMATION));
                std::chrono::time_point startTime = std::chrono::system_clock::now();

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
                Utils::Messagebox("SDK generation complete, finished in " + formattedTime + " seconds!", (MB_OK | MB_ICONINFORMATION));
            }
        }
        else
        {
            Utils::Messagebox("Failed to create the desired directory, cannot generate an SDK at this time!", (MB_OK | MB_ICONERROR));
        }
    }

    bool Initialize(bool bCreateLog)
    {
        if (!GlobalsInitialized)
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
                // There might be a better and automated way of doing this, so maybe I'll change this in the future.

                FNameEntry::Register_Flags();
                FNameEntry::Register_Index();
                FNameEntry::Register_Name();
                UObject::Register_VfTableObject();
                UObject::Register_ObjectInternalInteger();
                UObject::Register_Outer();
                UObject::Register_Name();
                UObject::Register_Class();
                UField::Register_Next();
                //UField::Register_SuperField(); // Safe to comment out if "UStruct" has a "SuperField".
                UEnum::Register_Names();
                UConst::Register_Value();
                UProperty::Register_ArrayDim();
                UProperty::Register_ElementSize();
                UProperty::Register_PropertyFlags();
                UProperty::Register_Offset();
                UStruct::Register_SuperField(); // Safe to comment out if "UField" has a "SuperField".
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
                //UDelegateProperty::Register_Function(); // Not actually needed in sdk generation at the moment.
                //UDelegateProperty::Register_DelegateName(); // Not actually needed in sdk generation at the moment.
                UByteProperty::Register_Enum();
                UBoolProperty::Register_BitMask();
                UArrayProperty::Register_Inner();
                GlobalsInitialized = true;
            }
            else
            {
                Utils::Messagebox("Failed to validate GObject & GNames, please make sure you have them configured properly!", (MB_OK | MB_ICONERROR));
                return false;
            }
        }

        if (GlobalsInitialized)
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
                        Utils::Messagebox("Error: Failed to create the log file, might not have the right permissions or your directory is invalid!", (MB_OK | MB_ICONERROR));
                        return false;
                    }
                }
#endif
                return true;
            }
            else
            {
                Utils::Messagebox("Looks like you forgot to set an output path for the generator! Please edit the output path in \"Configuration.cpp\" and recompile.", (MB_OK | MB_ICONERROR));
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

            Utils::Messagebox("Finished dumping instances!", (MB_OK | MB_ICONINFORMATION));
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
                        file << std::to_string(nameEntry->GetIndex()) << "] " << name << " ";
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

                for (UObject* uObject : *UObject::GObjObjects())
                {
                    if (uObject)
                    {
                        std::string objectName = uObject->GetFullName();

                        file << "UObject[";
                        Printer::FillRight(file, '0', 6);
                        file << std::to_string(uObject->ObjectInternalInteger) << "] " << objectName << " ";
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
        return (AreGObjectsValid() && AreGNamesValid());
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