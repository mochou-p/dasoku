// zzz

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace zzz
{
    class zzzWindow
    {
        public:
            zzzWindow(int w, int h, std::string name);
            ~zzzWindow();

            zzzWindow(const zzzWindow &) = delete;
            zzzWindow &operator=(const zzzWindow &) = delete;

            bool shouldClose() { return glfwWindowShouldClose(window); }

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        private:
            void initWindow();

            const int width;
            const int height;

            std::string windowName;
            GLFWwindow *window;
    };
}
