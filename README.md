## CodeRed Generator v1.0.2

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
![](https://i.imgur.com/YK4iRMg.png)

## Generation

Once you have your custom engine folder setup, all that's left is to fill out your class and struct fields for your game. This generator uses a unique `REGISTER_MEMBER` macro to define class/struct members. This is very important for your final generated SDK as it's used to calculate offsets and unknown data all automatically, without needing to modify your `PiecesOfCode.cpp` unlike in Feckless's generator.

Any class/struct member outside of whats in the `EMemberTypes` does NOT need to be registered with the `REGISTER_MEMBER` macro, so feel free to place padding or buffers in between.

![](https://i.imgur.com/k0ewmaC.png)
Once all your classes are filled out and you've made the necessary changes in `Configuration.cpp`, double check you didn't forget to set a path in `Configuration::Directory` and have the right files included in `Engine.hpp`. After that just compile as a DLL and manually inject into your game, generation will start automatically and will prompt you when it is completed.

## Finalization

After your SDK has been generated you might need to make a minor change to it. Depending on the game the header files in `SdkHeaders.hpp` could be placed out of order, if they are make sure to swap it out in the order of the includes to `Core` first, then `Engine`. I'm not sure why this happens in some games and not others so I am unable to automate this step without a guarantee of messing something up.

Here is an example what it should look like:
```cpp
#include "GameDefines.hpp"
#include "SDK_HEADERS\Core_structs.hpp"
#include "SDK_HEADERS\Core_classes.hpp"
#include "SDK_HEADERS\Core_parameters.hpp"
#include "SDK_HEADERS\Engine_structs.hpp"
#include "SDK_HEADERS\Engine_classes.hpp"
#include "SDK_HEADERS\Engine_parameters.hpp"
...etc...
```
