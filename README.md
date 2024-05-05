
## CodeRed Generator 3 (v1.1.4)

This is a C++20 Unreal Engine 3 SDK generator that was originally based off the source of [TheFeckless's UE3 SDK Generator](https://www.unknowncheats.me/forum/unreal-engine-3-a/71911-thefeckless-ue3-sdk-generator.html). It has since grown into its own project which utilizes C++20, strings, filesystem paths, and modern file streams; along with converting legacy UE3 features to more modern and user friendly ones while still being compatible with UE3.

### Features

- **Accessibility**
Final generated SDK is plug and play, just `#include "SdkHeaders.hpp"` in your project, initialize globals, and you're ready to go.

- **Global Initialization**
You have the option to generate an SDK using either offsets or patterns for GObjects and GNames.

- **Class Alignment**
Full support for both x32 bit and x64 bit games, just change the `Alignment` value in `Configuration.cpp`.

- **Full Customization**
You have full customization over the final generated SDK, use enum classes, remove native flags, class alignment, and even comment spacing.

### Requirements

- ISO C++20 Standard.
- Visual Studio or another Windows based compiler (For Windows header files, along with the PSAPI library).

## Getting Started

Included in this project is a template folder located in `Engine/Template`, to get started copy and paste this folder and rename it to the game you would like to use. To generate from your newly created engine folder don't forget to change the includes in the `Engine.hpp` file.

In the `Configuration.hpp` file there are two defines, one is `NO_LOGGING` which disables writing to a log file, and another define called `UTF16`. If your game is using wide characters you will need to uncomment this define out, if not `UTF8` will be used by default.

Any further configuration **MUST BE DONE IN THE `Configuration.cpp` ONLY!** This file contains everything from class alignment, process event settings, global patterns and offsets, your games name and version, and most importantly the directory used for SDK generation.

![](https://i.imgur.com/q2tFJ7I.png)

![](https://i.imgur.com/2GrLwTD.png)

![](https://i.imgur.com/K6sAwPm.png)

## Generation

Once you have your custom engine folder setup, all that's left is to fill out your class and struct fields for your game. This generator uses a unique `REGISTER_MEMBER` macro to define class/struct members. This is very important for your final generated SDK as it's used to calculate offsets and unknown data all automatically, without needing to modify your `PiecesOfCode.cpp` unlike in Feckless's generator.

Any class/struct member outside of whats in the `EMemberTypes` does NOT need to be registered with the `REGISTER_MEMBER` macro, so feel free to place padding or buffers in between.

![](https://i.imgur.com/k9vawPv.png)

Once all your classes are filled out and you've made the necessary changes in `Configuration.cpp`, double check you didn't forget to set an out path in `Configuration.cpp` and have the right files included in `Engine.hpp`. After that just compile as a DLL and manually inject into your game, generation will start automatically and will prompt you when it is completed.

## Changelog

###  v1.1.4
- Added a new engine folder for Dishonered, which contains all needed fields to generate an sdk for that game.
- Moved the "Printer.cpp/hpp" files to the "Framework" folder.

### v1.1.3
- Added a new "UTF16_FSTRING" define in "Configuration.hpp", this is required if your game uses wide chars for FStrings but not FNames.
- Added the pragma push and pragma pop directives for the "GameDefines.hpp" file for forced class alignment.
- Added some extra safety checking when dumping instances and printing spacing for their names.
- Fixed a crash that could happen related to property names being longer than the spacing set in your "Configuration.cpp" file.

### v1.1.2
- Added a new "GLogger" class in "dllmain.hpp/cpp" which will now handle any logging to the log file.
- Added extra logging for the time it takes for caching every object before sdk generation starts.
- Improved the caching system for packages, includes, and constants.
- Fixed a bug that could cause UClassProperty's to be confused with UObjectProperty's.

### v1.1.1
- Added a new caching system for counting objects, which increases generation speed over by over 5x.
- Added additional enums for flags to "PiecesOfCode.cpp", including state flags and class flags.
- Updated the property flags enum and added some extra ones in \"GameDefines.hpp", and changed the "GetAllPropertyFlags" function to reflect that.
- Removed the "CountObject" function from "UObject" and added a new one to the "GCache" class in "dllmain.hpp/cpp".

### v1.1.0
- Added a new caching system for unreal properties, which increases sdk generation speed a bit.
- Added a couple new string helper functions in the "Printers.hpp/cpp" files related to replacing characters.
- Return types that were enums now use their proper enum class name if "UsingEnumClasses" is set to true in "Configuration.cpp".
- Dump instances are now a lot faster, a new line character is now used instead of "std::endl", as "std::endl" flushes stream which is why it took so long.
- Moved some of the helper functions in the "Utils" namespace to be inside of the new "UnrealObject" and "UnrealProperty" classes.
- Changed the include file for "Member.hpp", as its now in a different folder, in your "GameDefines.hpp" file.
- Renamed the values in the "EWidthTypes" to be more consistent with the previous naming standard changes.
- Modified some of the functions in "GameDefines.cpp" and updated "PiecesOfCode.cpp" to match that.
- Fixed the name and comment spacing for "UnknownData" being too short compared to all the other names in classes and structs.
- Fixed miscalculating the size of array parameters when copying them for process event.

### v1.0.9
- Fixed forgetting to call "GCache::Initialize()" in the last update.
- Fixed array properties not showing up in the parameters of functions, they only generated in the parameter struct itself.
- Fixed array parameters not properly accounting for their element size when copying for ProcessEvent.
- Array parameters are now printed in normal decimal instead of hexadecimal.

### v1.0.8
- Added a new caching system for objects, which increases generation speed by over 50%.
- Added a new class type for the "EClassTypes" enum in "Member.hpp", this is required for the new cache system.
- Fixed the header files in "SdkHeaders.hpp" sometimes being in the wrong order in the final sdk.
- Fixed the "UsingConstants" setting for your config file generating improper names in the final sdk.
- Fixed a capitalization error in the final generated sdk if you had "UsingConstants" set to true in your config file.
- Updated some of the default "PiecesOfCode.cpp" functions slightly.

### v1.0.7
- This update will require you to change your "GameDefines.hpp" file, as I've renamed all the enum values for the "REGISTER_MEMBER" macro, also delete the "EPropertyTypes" enum that same file as I've moved it to "Member.hpp" to prevent a multiple declarations error.
- Added a new "SUPERFIELDS_IN_UFIELD" #define in "GameDefines.hpp", which is used by "Member.cpp" to grab class offsets.
- Added support for the "MinAlignment" member in the "UStruct" class, this is necessary to account for padding not defined in structs "PropertySize". This is required for certain games such as Rocket League for proper struct sizes and class offsets.
- Added the "HashNext" member for the "FNameEntry" struct so it can be generated in the final sdk now.
- Added a new "BlacklistedTypes" vector in "Configuration.hpp/cpp", this allows you to prevent certain structs or classes from being generated in the final sdk.
- Added a new "TypeOverrides" vector in "Configuration.hpp/cpp", this allows you to override a struct or class fields with your own custom string in the final sdk.
- Added a new "PiecesOfTypes" namespace in "PiecesOfCode.hpp/cpp" which is used for the new "TypeOverrides" feature.
- Fixed "UClassProperty" members not being generated in the final sdk.
- Fixed "UClassProperty" not calculating the right class size in the final sdk, resulting in adding padding that wasn't needed.
- Moved the "EPropertyTypes" enum from "GameDefines.hpp" to "Member.hpp", it should have been in there from the start.
- Renamed all the values for "EClassTypes", EMemberTypes", and "EPropertyTypes" enums.
- Remade the "Member" class to be more dynamic on calculating class properties and registered members.
- Removed "UDelegateProperty" from registered members as its not used for anything at the moment.
- Internal improvements to memory streams and logging.

### v1.0.6
- This update will break your previous configuration files, as I've reformatted them entirely! Remember that before just copy pasting files. I'm in the process of remaking a lot of stuff and this is just the first step.
- Changed the namespaces in "Engine.hpp/cpp" as well as in "Configuration.hpp/cpp" to store their variables in classes as opposed to externs.
- Got rid of the "EAlignment" enum in "Configuration.hpp", it was just to serve as an example but it could be confusing to new people.
- Fixed the process event index still being used even if you had the boolean set to false.
- Internal improvements to naming standards and code organization.

### v1.0.5
- Fixed some incorrect operators being printed from "PiecesOfCode.cpp".

### v1.0.4
- Fixed the "==" and "!=" operators not working properly for the "FString" class.

### v1.0.3
- All function parameters that don't have out/return flags on them will be const references now, to avoid unnecessary copying of data.

### v1.0.2
- Added a bool option called "UsingWindows" in the template "Configuration.cpp" file, when set to true it will include "Windows.h" along with removing windows macro functions from unreal ones. Note that this only makes changes in the final generated sdk.
- Decided to replace the "ZeroMemory" macro with use memset instead, "ZeroMemory" calls this anyway.

### v1.0.1
- Added an include for "Windows.h" in the generated "GameDefines.hpp" file, this is needed for the "ZeroMemory" maco.