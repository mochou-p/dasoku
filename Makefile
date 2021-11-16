# dasoku

DEPS = .\..\libs
_DEPS = .\libs

VULKAN_SDK_PATH = C:\VulkanSDK\1.2.189.2

GLFW_PATH = $(DEPS)\GLFW
GLM_PATH = $(DEPS)\GLM
GLSLC_PATH = $(_DEPS)\GLSLC
TINYOBJ_PATH = $(DEPS)\TINYOBJ
IMGUI_PATH = $(DEPS)\IMGUI
STB_PATH = $(DEPS)\STB

SHADERS_PATH = .\resources\shaders

CFLAGS = -std=c++17 -I. -I$(GLM_PATH) -I$(GLFW_PATH)\include -I$(VULKAN_SDK_PATH)\include -I$(TINYOBJ_PATH) -I$(IMGUI_PATH) -I$(STB_PATH)
LDFLAGS = -L$(VULKAN_SDK_PATH)\lib -L$(GLFW_PATH)\lib -lglfw3 -lvulkan-1 -lgdi32

CC = g++

IMGUI_CPP_FILES = $(IMGUI_PATH)\*.cpp

COMPILE = $(CC) $(CFLAGS) -o ..\Dasoku.exe *.cpp $(IMGUI_CPP_FILES) $(LDFLAGS)

COMPILE_SHADERS = for /r $(SHADERS_PATH)\source %%f in (*.*) do $(GLSLC_PATH)\glslc.exe %%f -o %%f.spv
MOVE_SHADERS = for /r $(SHADERS_PATH)\source %%f in (*.spv) do move %%f %%~pf..
DESTROY_SHADERS = del $(SHADERS_PATH)\*.spv

TARGET:
	cd ".\source\" && $(COMPILE)
	make run
	
shaders:
	$(DESTROY_SHADERS)
	$(COMPILE_SHADERS)
	$(MOVE_SHADERS)

run:
	.\Dasoku

.SILENT:
