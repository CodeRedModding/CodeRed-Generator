#pragma once
/*
	Changes in v1.0.6
	- This update will break your previous configuration files, as I've reformatted them entirely! Remember that before just copy pasting stuff.
	- Changed the namespaces in "Engine.hpp/cpp" as well as in "Configuration.hpp/cpp" to store their variables in classes as opposed to externs.
	- Got rid of the "EAlignment" enum in "Configuration.hpp", it was just to serve as an example but it could be confusing to new people.
	- Fixed the process event index still being used even if you had the boolean set to false.
	- Internal improvements to naming standards and code organization.

	Changes in v1.0.5
	- Fixed some incorrect operators being printed from "PiecesOfCode.cpp".

	Changes in v1.0.4
	- Fixed the "==" and "!=" operators not working properly for the "FString" class.

	Changes in v1.0.3
	- All function parameters that don't have out/return flags on them will be const references now, to avoid unnecessary copying of data.

	Changes in v1.0.2
	- Added a bool option called "UsingWindows" in the template "Configuration.cpp" file, when set to true it will include "Windows.h" along with removing windows macro functions from unreal ones. Note that this only makes changes in the final generated sdk.
	- Decided to replace the "ZeroMemory" macro with use memset instead, "ZeroMemory" calls this anyway.

	Changes in v1.0.1
	- Added an include for "Windows.h" in the generated "GameDefines.hpp" file, this is needed for the "ZeroMemory" maco.
*/