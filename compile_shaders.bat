:: zzz

set VULKAN_SDK_PATH=C:/VulkanSDK/1.2.189.2

%VULKAN_SDK_PATH%/Bin/glslc.exe shaders/simple_shader.vert -o shaders/simple_shader.vert.spv
%VULKAN_SDK_PATH%/Bin/glslc.exe shaders/simple_shader.frag -o shaders/simple_shader.frag.spv
