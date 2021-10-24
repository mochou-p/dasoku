# hebi

GLM_PATH = C:/GLM
GLFW_PATH = C:/GLFW
VULKAN_SDK_PATH = C:/VulkanSDK/1.2.189.2

CFLAGS = -std=c++17 -I. -I$(GLM_PATH) -I$(GLFW_PATH)/include -I$(VULKAN_SDK_PATH)/include
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib -L$(GLFW_PATH)/lib -lglfw3 -lvulkan-1 -lgdi32

CC = g++

TARGET:
	$(CC) $(CFLAGS) -o main.exe *.cpp $(LDFLAGS)
	./main
