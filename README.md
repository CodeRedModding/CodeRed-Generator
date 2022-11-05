
## CodeRed Generator v1.0.0

### About

This is a C++20 Unreal Engine 3 SDK generator that was originally based off the source of [TheFeckless's UE3 SDK Generator](https://www.unknowncheats.me/forum/unreal-engine-3-a/71911-thefeckless-ue3-sdk-generator.html). It has since grown into its own project which utilizes C++20, strings, filesystem paths, and modern file streams; along with converting legacy UE3 features to more modern and user friendly ones while still being compatible with UE3.

### Features

- **Accessibility**
Generated SDK is plug and play, just `#include "SdkHeaders.hpp"` in your project, initialize globals, and you're ready to go.

- **Global Initialization**
You have the option to generate an SDK using either offsets or patterns for GObjects and GNames.

- **Class Alignment**
Full support for both x32 bit and x64 bit games, just change the `Alignment` value in `Configuration.cpp`.

- **Full Customization**
You have full customization over the final generated sdk, use enum classes, remove native flags, custom class alignment, and even comment spacing.

### Requirements

- ISO C++20 Standard.
- Visual Studio or another Windows based compiler (For Windows header files, along with the PSAPI library).

### Configuration

### Finalization

Once your sdk has been generated you might need to make a minor change to it. Depending on the game the header files in `SdkHeaders.hpp` could be placed out of order, if they are make sure to swap it out in the order of `Core` first, then `Engine`; here is an example:

```cpp
#include "GameDefines.hpp"
#include "SDK_HEADERS\Core_structs.hpp"
#include "SDK_HEADERS\Core_classes.hpp"
#include "SDK_HEADERS\Core_parameters.hpp"
#include "SDK_HEADERS\Engine_structs.hpp"
#include "SDK_HEADERS\Engine_classes.hpp"
#include "SDK_HEADERS\Engine_parameters.hpp"
```
