#pragma once
/*
	Changes in v1.0.3
	- All function parameters that dont' have out/return flags on them will be const reference now, to avoid unnecessary copying of data.

	Changes in v1.0.2
	- Added a bool option called "UsingWindows" in the template "Configuration.cpp" file, when set to true it will include "Windows.h" along with removing windows macro functions from unreal ones. Note that this only makes changes in the final generated sdk.
	- Decided to replace the "ZeroMemory" macro with use memset instead, "ZeroMemory" calls this anyway.

	Changes in v1.0.1
	- Added an include for "Windows.h" in the generated "GameDefines.hpp" file, this is needed for the "ZeroMemory" maco.
*/