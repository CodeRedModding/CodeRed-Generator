
## CodeRed Generator v1.0.0

### About

This is a C++20 Unreal Engine 3 SDK generator that was originally based off the source of [TheFeckless's UE3 SDK Generator](https://www.unknowncheats.me/forum/unreal-engine-3-a/71911-thefeckless-ue3-sdk-generator.html). It has since grown into its own project which utilizes C++20, strings, filesystem, and modern file streams; along with converting legacy UE3 features to more modern and user friendly ones while still being compatible with UE3.

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

To get started in generating an sdk, copy  and paste the `Template` folder included in the Engine folder and rename it to the game you would like to use. The `GameDefines.hpp` file will need to be reversed by hand because every game is different, there is no way to automate this process. In `Configuration.hpp` uncomment which character type your game uses, it will either be wide char (UTF16) or const char (UTF8).

![](https://i.imgur.com/gbIfB3R.png)

Hard coded fields in `GameDefines.hpp` are dynamically generated in the final sdk, along with their offsets. In order for the offsets to be correct you must properly "register" the field, the template includes all fields needed for sdk generation; anything other than that you do NOT need to register as long as it's not part of the `EFieldIds` enum.

Here is an example of how to register fields, there are more examples in the `Template` folder, as well as comments throughout to help you if you encounter any errors with the template.

![](https://i.imgur.com/qbTOPWd.png)

Once you have the necessary classes filled out all that's left to do is make the desired changes in the `Configuration.cpp` file (don't forget to set the GeneratorDirectory path) and make sure you have the right files included for your game in `Engine.hpp`. When you inject the compiled dll into your game you will be prompted with a message saying that sdk generation has started, do not close your game until you recieve another message confirming generation is completed.

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
