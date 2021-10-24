# dasoku

VULKAN_SDK_PATH = C:\VulkanSDK\1.2.189.2
GLSLC_PATH = $(VULKAN_SDK_PATH)\Bin\glslc.exe
SHADERS_PATH = .\shaders

COMPILE_SHADERS = for %%f in ($(SHADERS_PATH)\*.*) do $(GLSLC_PATH) %%f -o %%f.spv
DESTROY_SHADERS = del $(SHADERS_PATH)\*.spv

BUILD_ASHI = 
BUILD_HEBI = 

TARGET shaders:
	$(DESTROY_SHADERS)
	$(COMPILE_SHADERS)
	
ashi 3d:
	cd ".\Ashi (3D)\" && make

hebi 2d:
	cd ".\Hebi (2D)\" && make

.SILENT:
