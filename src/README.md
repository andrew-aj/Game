The libraries needed to run this project are:
<br />[Vulkan SDK](https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe)
<br />[BGFX](https://github.com/bkaradzic/bgfx.cmake)
<br />[Boxer](https://github.com/aaronmjacobs/Boxer)
<br />[GLFW](https://www.glfw.org/download.html)
<br />[EnTT](https://github.com/skypjack/entt/releases)
<br />[GLM](https://github.com/g-truc/glm/releases)

To install Vulkan, run the sdk and make sure these environment variables are set:
Vulkan_INCLUDE_DIR, Vulkan_LIBRARY, and VULKAN_SDK.
<br /> The first points to the include directory, ex: C:\VulkanSDK\1.2.182.0\Include
<br /> The second and third point to the directory of the sdk, ex: C:\VulkanSDK\1.2.182.0

To install the rest of the libraries, first you need to set your environment variable.
Create a new environment variable called CMAKE_PREFIX_PATH and set it to be the location
where your libraries will be stored. For example, I have mine as C:\LIBRARIES

To install BGFX, follow the instructions and clone the repository with all it's submoules
to CMAKE_PREFIX_PATH/bgfx so that all the source files are in that folder.

To install Boxer, clone the library to CMAKE_PREFIX_PATH/Boxer.

To install GLFW, download the precompiled binaries for your platform. Then,
Put the include and docs directories inside CMAKE_PREFIX_PATH/GLFW.
Then create a new directory called lib and put inside there the library files
for your platform. For example, in the windows precompiled binaries I took
the three files for mingw-64 and put them in the lib folder.

To install EnTT download the library to CMAKE_PREFIX_PATH/EnTT.

To install GLM, download the library to CMAKE_PREFIX_PATH/glm.

After this, reload your clion and then let the cmake compile and it should work.