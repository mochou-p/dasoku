// ashi

#include "window.hpp"

#include <stdexcept>

namespace ashi
{
    AshiWindow::AshiWindow
    (
        int w,
        int h,
        std::string name
    ): width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    AshiWindow::~AshiWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void AshiWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void AshiWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface");
        }
    }

    void AshiWindow::framebufferResizeCallback
    (
        GLFWwindow *window,
        int width,
        int height
    )
    {
        auto ashiWindow = reinterpret_cast<AshiWindow *>(glfwGetWindowUserPointer(window));
        ashiWindow->framebufferResized = true;
        ashiWindow->width = width;
        ashiWindow->height = height;
    }
}
