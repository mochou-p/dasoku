// zzz

#include "zzz_window.hpp"

#include <stdexcept>

namespace zzz
{
    zzzWindow::zzzWindow(int w, int h, std::string name): width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    zzzWindow::~zzzWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void zzzWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    void zzzWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }
}
