# dasoku

DEPS = .\..\.dependencies
_DEPS = .\.dependencies

VULKAN_SDK_PATH = C:\VulkanSDK\1.2.189.2
GLFW_PATH = $(DEPS)\GLFW
GLM_PATH = $(DEPS)\GLM
GLSLC_PATH = .\.dependencies\GLSLC
TINYOBJ_PATH = $(DEPS)\TINYOBJ
IMGUI_PATH = $(DEPS)\IMGUI
SHADERS_PATH = .\shaders

CFLAGS = -std=c++17 -I. -I$(GLM_PATH) -I$(GLFW_PATH)\include -I$(VULKAN_SDK_PATH)\include -I$(TINYOBJ_PATH) -I$(IMGUI_PATH)
LDFLAGS = -L$(VULKAN_SDK_PATH)\lib -L$(GLFW_PATH)\lib -lglfw3 -lvulkan-1 -lgdi32

COMPILE_SHADERS = for /r $(SHADERS_PATH) %%f in (*.*) do $(GLSLC_PATH)\glslc.exe %%f -o %%f.spv
DESTROY_SHADERS = del $(SHADERS_PATH)\2D\*.spv; $(SHADERS_PATH)\3D\*.spv

IMGUI_CPP_FILES = $(IMGUI_PATH)\*.cpp

COMPILE = $(CC) $(CFLAGS) -o main.exe *.cpp $(IMGUI_CPP_FILES) $(LDFLAGS)

CC = g++

TARGET shaders:
	$(DESTROY_SHADERS)
	$(COMPILE_SHADERS)
	
ashi 3d:
	cd ".\Ashi (3D)\" && $(COMPILE)
	.\Ashi (3D)\main

hebi 2d:
	cd ".\Hebi (2D)\" && $(COMPILE)
	.\Hebi (2D)\main

.SILENT:
