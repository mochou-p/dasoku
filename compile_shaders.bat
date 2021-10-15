:: zzz

set VULKAN_SDK_PATH=C:\VulkanSDK\1.2.189.2
set GLSLC_PATH=%VULKAN_SDK_PATH%\Bin\glslc.exe

for %%f in (shaders\*.*) do %GLSLC_PATH% %%f -o %%f.spv
