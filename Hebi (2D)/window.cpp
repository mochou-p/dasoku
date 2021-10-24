// hebi

#include "window.hpp"

#include <stdexcept>

namespace hebi
{
    HebiWindow::HebiWindow
    (
        int w,
        int h,
        std::string name
    ): width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    HebiWindow::~HebiWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void HebiWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void HebiWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void HebiWindow::framebufferResizeCallback
    (
        GLFWwindow *window,
        int width,
        int height
    )
    {
        auto hebiWindow = reinterpret_cast<HebiWindow *>(glfwGetWindowUserPointer(window));
        hebiWindow->framebufferResized = true;
        hebiWindow->width = width;
        hebiWindow->height = height;
    }
}
