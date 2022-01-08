The purpose of this project is to learn game programming by making a 
small 2D game.

The libraries needed to run this project are:
<br />[Vulkan SDK](https://sdk.lunarg.com/sdk/download/latest/windows/vulkan-sdk.exe)
<br />[DiligentCore](https://github.com/DiligentGraphics/DiligentCore/tree/6d4b60e80c47874d137bb02fe95e40830e6b8b2b)
<br />[Boxer](https://github.com/aaronmjacobs/Boxer)
<br />[GLFW](https://www.glfw.org/download.html)
<br />[EnTT](https://github.com/skypjack/entt/releases)
<br />[GLM](https://github.com/g-truc/glm/releases)
<br />[YAML](https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.7.0.zip)

To install Vulkan, run the sdk and make sure these environment variables are set:
Vulkan_INCLUDE_DIR, Vulkan_LIBRARY, and VULKAN_SDK.
<br /> The first points to the include directory, ex: C:\VulkanSDK\1.2.182.0\Include
<br /> The second and third point to the directory of the sdk, ex: C:\VulkanSDK\1.2.182.0

To install the rest of the libraries, first you need to set your environment variable.
Create a new environment variable called CMAKE_PREFIX_PATH and set it to be the location
where your libraries will be stored. For example, I have mine as C:\LIBRARIES

To install DiligentCore, download the latest release and put it into a folder in your libraries
path called DiligentCore. 

To install Boxer, clone the library to CMAKE_PREFIX_PATH/Boxer.

To install GLFW, download the precompiled binaries for your platform. Then,
Put the include and docs directories inside CMAKE_PREFIX_PATH/GLFW.
Then create a new directory called lib and put inside there the library files
for your platform. For example, in the windows precompiled binaries I took
the three files for mingw-64 and put them in the lib folder.

To install EnTT download the library to CMAKE_PREFIX_PATH/EnTT.

To install GLM, download the library to CMAKE_PREFIX_PATH/glm.

To install YAML, download the library to CMAKE_PREFIX_PATH/yaml.

After this, reload your clion and then let the cmake compile and it should work.