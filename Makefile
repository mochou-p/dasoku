# dasoku

VULKAN_SDK_PATH = C:\VulkanSDK\1.3.211.0

GLFW_PATH    = .\..\libs\GLFW
GLM_PATH     = .\..\libs\GLM
GLSLC_PATH   = .\libs\GLSLC
TINYOBJ_PATH = .\..\libs\TINYOBJ
IMGUI_PATH   = .\..\libs\IMGUI
STB_PATH     = .\..\libs\STB

SHADERS_PATH = .\resources\shaders

CFLAGS  = -std=c++17 -I. -I$(GLM_PATH) -I$(GLFW_PATH)\include -I$(VULKAN_SDK_PATH)\include -I$(TINYOBJ_PATH) -I$(IMGUI_PATH) -I$(STB_PATH)
LDFLAGS = -L$(VULKAN_SDK_PATH)\lib -L$(GLFW_PATH)\lib -lglfw3 -lvulkan-1 -lgdi32

CC = g++

IMGUI_CPP_FILES = $(IMGUI_PATH)\*.cpp

COMPILE = $(CC) $(CFLAGS) -o ..\Dasoku.exe *.cpp $(IMGUI_CPP_FILES) $(LDFLAGS)

COMPILE_SHADERS = for /r $(SHADERS_PATH)\source %%f in (*.*) do $(GLSLC_PATH)\glslc.exe %%f -o %%f.spv
MOVE_SHADERS    = for /r $(SHADERS_PATH)\source %%f in (*.spv) do move %%f %%~pf.. >nul
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

all:
	make shaders
	make
