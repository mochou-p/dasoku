// ashi

#include "app.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "keyboard_movement_controller.hpp"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>
#include <chrono>

namespace ashi
{
    App::App() { loadGameObjects(); }

    App::~App()
    {
        vkDestroyDescriptorPool(ashiDevice.device(), imguiPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
    }

    void App::run()
    {
        AshiRenderSystem ashiRenderSystem
        { ashiDevice, ashiRenderer.getSwapChainRenderPass() };

        AshiCamera camera {};
        camera.setViewTarget
        (
            glm::vec3(-1.0f, -2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 2.5f)
        );

        auto viewerObject = AshiGameObject::createGameObject();
        AshiKeyboardMovementController cameraController {};

        initImgui();

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!ashiWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration
                <float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            // fixes weird behavior when resizing and using the keyboard at the same time
            // frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ
            (
                ashiWindow.getGLFWwindow(),
                frameTime,
                viewerObject
            );
            camera.setViewYXZ
            (
                viewerObject.transform3d.translation,
                viewerObject.transform3d.rotation
            );

            float aspect = ashiRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 20.0f);

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();

            ImGui::Begin("hello");
            ImGui::Button("reynep");
            ImGui::End();

            ImGui::Render();

            if (auto commandBuffer = ashiRenderer.beginFrame())
            {
                ashiRenderer.beginSwapChainRenderPass(commandBuffer);
                ashiRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
                ashiRenderer.endSwapChainRenderPass(commandBuffer);
                ashiRenderer.endFrame();

            }
        }

        vkDeviceWaitIdle(ashiDevice.device());

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void App::loadGameObjects()
    {
        std::shared_ptr<AshiModel> ashiModel;

        ashiModel =
            AshiModel::createModelFromFile(ashiDevice, ".\\models\\smooth_vase.obj");
        auto vase = AshiGameObject::createGameObject();
        vase.model = ashiModel;
        vase.transform3d.translation = {0.0f, 0.5f, 2.5f};
        vase.transform3d.scale = glm::vec3(3.0f);
        gameObjects.push_back(std::move(vase));
    }

    void App::initImgui()
    {
        VkDescriptorPoolSize pool_sizes[] =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;

        vkCreateDescriptorPool(ashiDevice.device(), &pool_info, nullptr, &imguiPool);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();

        ImGui_ImplGlfw_InitForVulkan(ashiWindow.getGLFWwindow(), true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = ashiDevice.getInstance();
        init_info.PhysicalDevice = ashiDevice.getPhysicalDevice();
        init_info.Device = ashiDevice.device();
        init_info.Queue = ashiDevice.getGraphicsQueue();
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, ashiRenderer.getSwapChainRenderPass());

        ImGui::StyleColorsDark();

        ImGui_ImplVulkan_CreateFontsTexture(ashiDevice.beginSingleTimeCommands());

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}
