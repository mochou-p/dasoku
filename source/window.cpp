// dsk

#include "window.hpp"

#include <stdexcept>

namespace dsk
{
    DskWindow::DskWindow
    (
        int w,
        int h,
        std::string name
    ): width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    DskWindow::~DskWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void DskWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void DskWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void DskWindow::framebufferResizeCallback
    (
        GLFWwindow *window,
        int width,
        int height
    )
    {
        auto dskWindow = reinterpret_cast<DskWindow *>(glfwGetWindowUserPointer(window));
        dskWindow->framebufferResized = true;
        dskWindow->width = width;
        dskWindow->height = height;
    }
}
