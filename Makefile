# zzz

GLM_PATH = C:/GLM
GLFW_PATH = C:/GLFW
VULKAN_SDK_PATH = C:/VulkanSDK/1.2.189.2
GLSLC_PATH = $(VULKAN_SDK_PATH)/Bin/glslc.exe

SHADERS_PATH = shaders

CFLAGS = -std=c++17 -I. -I$(GLM_PATH) -I$(GLFW_PATH)/include -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib -L$(GLFW_PATH)/lib -lglfw3 -lvulkan-1 -lgdi32

COMPILE_SHADERS = for %%f in ($(SHADERS_PATH)\*.*) do $(GLSLC_PATH) %%f -o %%f.spv
CLEAN = del $(SHADERS_PATH)\*.spv; main.exe

CC = g++

TARGET:
	$(CLEAN)
	$(COMPILE_SHADERS)
	$(CC) $(CFLAGS) -o main.exe *.cpp $(LDFLAGS)
	./main
	cls
