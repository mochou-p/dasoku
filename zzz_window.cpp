// zzz

#include "zzz_window.hpp"

#include <stdexcept>

namespace zzz
{
    ZzzWindow::ZzzWindow(int w, int h, std::string name): width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    ZzzWindow::~ZzzWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void ZzzWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void ZzzWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }
}
