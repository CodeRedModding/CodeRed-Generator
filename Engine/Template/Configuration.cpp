#include "Configuration.hpp"

/*
# ========================================================================================= #
# Configuration
# ========================================================================================= #
*/

namespace Configuration
{
    const bool UsingWindows =                   true;           // When set to true this will auto include "Windows.h" in your sdk, along with renaming some windows macro functions.
    const bool UsingConstants =                 false;			// If you want to use class and function indexes instead of "FindClass/FindFunction".
    const bool UsingOffsets =                   false;			// If you're using the offsets below or patterns to initialize globals.
    const bool UsingEventIndex =                false;			// If you want to use "ProcessEventIndex" change this to true, if not virutal voids will be generated from "ProcessEventPattern".
    const bool UsingEnumClasses =               true;			// If you want to use strongly typed enum classes over normal ones.
    const bool PrintFlagEnums =                 true;			// If you want the EFunctionFlags, EPropertyFlags, and EObjectFlags enums so be included in the final sdk.
    const bool RemoveNativeIndex =              true;			// If you want to remove the "iNative" index on functions before calling process event.
    const bool RemoveNativeFlags =              true;			// If you want to remove the "FUNC_Native" flag on functions before calling process event.	
    const std::string EnumClassType =           "uint8_t";		// Underlying enum type if you set "UsingEnumClasses" to true.

    const bool UsingPrecompiledHeaders = true;
    const std::string PrecompiledHeaderName = "pch.h";

    const int32_t CommentSpacer =               30;
    const int32_t ConstSpacer =                 50;
    const int32_t EnumSpacer =                  50;
    const int32_t StructSpacer =                50;
    const int32_t FunctionSpacer =              50;
    const int32_t ClassSpacer =                 50;
    const int32_t GameAlignment =               static_cast<int32_t>(EAlignment::X32BIT);   // Used to calculate property sizes and missed offsets.
    const int32_t FinalAlignment =              0x4;			                            // Forced alignment used in the final sdk, "UsingEnumClasses" is dependent on this value!

    const int32_t ProcessEventIndex =           -1;				                            // Position where the Process Event function is in "UObject"'s VfTable.
    const std::string ProcessEventString =      "";
    const uint8_t* ProcessEventPattern =        (uint8_t*)"";
    const char* ProcessEventMask =              (char*)"";

    const std::string GObjectsString =          "";
    const uint8_t* GObjectsPattern =            (uint8_t*)"";
    const char* GObjectsMask =                  (char*)"";
    const uintptr_t GObjectsOffset =            0x0;

    const std::string GNamesString =            "";
    const uint8_t* GNamesPattern =              (uint8_t*)"";
    const char* GNamesMask =                    (char*)"";
    const uintptr_t GNamesOffset =              0x0;

    const std::string GameName =                "Template";
    const std::string GameNameShort =           "TSDK";
    const std::string GameVersion =             "1.0.0.0";
    const std::filesystem::path Directory =     "I_FORGOT_TO_SET_A_PATH";
}

/*
# ========================================================================================= #
#
# ========================================================================================= #
*/