// zzz

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace zzz
{
    class ZzzWindow
    {
        public:
            ZzzWindow(int w, int h, std::string name);
            ~ZzzWindow();

            ZzzWindow(const ZzzWindow &) = delete;
            ZzzWindow &operator=(const ZzzWindow &) = delete;

            bool shouldClose() { return glfwWindowShouldClose(window); }
            VkExtent2D getExtent()
            {
                return
                {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
                }; 
            }

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

        private:
            void initWindow();

            const int width;
            const int height;

            std::string windowName;
            GLFWwindow *window;
    };
}
