// dsk

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

namespace dsk
{
    App::App() { loadGameObjects(); }

    App::~App()
    {
        vkDestroyDescriptorPool(dskDevice.device(), imguiPool, nullptr);
    }

    void App::run()
    {
        DskRenderSystem dskRenderSystem
        { dskDevice, dskRenderer.getSwapChainRenderPass() };

        DskCamera camera {};
        camera.setViewTarget
        (
            glm::vec3(-1.0f, -2.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 2.5f)
        );

        auto viewerObject = DskGameObject::createGameObject();
        DskKeyboardMovementController cameraController {};

        initImgui();

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!dskWindow.shouldClose())
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
                dskWindow.getGLFWwindow(),
                frameTime,
                viewerObject
            );
            camera.setViewYXZ
            (
                viewerObject.transform3d.translation,
                viewerObject.transform3d.rotation
            );

            float aspect = dskRenderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 50.0f);

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();

            ImGui::PushStyleColor(ImGuiCol_TitleBg,  {0.0f, 0.0f, 0.0f, 1.0f});
            ImGui::PushStyleColor(ImGuiCol_WindowBg, {0.0f, 0.0f, 0.0f, 0.8f});
            ImGui::PushStyleColor(ImGuiCol_Border,   {0.0f, 0.0f, 0.0f, 1.0f});

            ImGui::Begin
            (
                "Frametime",
                nullptr,
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove
            );
            ImGui::Text
            (
                "%.2fms (%.0f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate
            );
            ImGui::End();

            ImGui::Begin
            (
                "Viewer",
                nullptr,
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove
            );
            ImGui::BeginChild("position");
            ImGui::Text("x: %.4f", viewerObject.transform3d.translation.x);
            ImGui::Text("y: %.4f", -viewerObject.transform3d.translation.y);
            ImGui::Text("z: %.4f", viewerObject.transform3d.translation.z);
            ImGui::EndChild();
            ImGui::End();

            ImGui::PopStyleColor(3);

            ImGui::Render();

            if (auto commandBuffer = dskRenderer.beginFrame())
            {
                dskRenderer.beginSwapChainRenderPass(commandBuffer);
                dskRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
                dskRenderer.endSwapChainRenderPass(commandBuffer);
                dskRenderer.endFrame();
            }

            gameObjects[2].transform3d.rotation.y += glm::radians(0.2f);
        }

        vkDeviceWaitIdle(dskDevice.device());

        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void App::loadGameObjects()
    {
        std::shared_ptr<DskModel> dskModel;

        dskModel =
            DskModel::createModelFromFile(dskDevice, ".\\resources\\models\\colored_cube.obj");
        auto cube = DskGameObject::createGameObject();
        cube.model = dskModel;
        cube.transform3d.translation = {0.0f, 0.3f, 2.5f};
        cube.transform3d.scale = {10.0f, 0.2f, 10.0f};
        gameObjects.push_back(std::move(cube));

        dskModel =
            DskModel::createModelFromFile(dskDevice, ".\\resources\\models\\smooth_vase.obj");
        auto vase = DskGameObject::createGameObject();
        vase.model = dskModel;
        vase.transform3d.translation = {0.3f, 0.1f, 2.5f};
        vase.transform3d.scale = glm::vec3(1.0f);
        gameObjects.push_back(std::move(vase));

        dskModel =
            DskModel::createModelFromFile(dskDevice, ".\\resources\\models\\neco-arc\\PBR - Metallic Roughness.obj");
        auto catgirl = DskGameObject::createGameObject();
        catgirl.model = dskModel;
        catgirl.transform3d.translation = {-0.3f, 0.02f, 2.5f};
        catgirl.transform3d.scale = {0.3f, -0.3f, 0.3f};
        gameObjects.push_back(std::move(catgirl));
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

        vkCreateDescriptorPool(dskDevice.device(), &pool_info, nullptr, &imguiPool);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();

        ImGui_ImplGlfw_InitForVulkan(dskWindow.getGLFWwindow(), true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = dskDevice.getInstance();
        init_info.PhysicalDevice = dskDevice.getPhysicalDevice();
        init_info.Device = dskDevice.device();
        init_info.Queue = dskDevice.getGraphicsQueue();
        init_info.DescriptorPool = imguiPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&init_info, dskRenderer.getSwapChainRenderPass());

        ImGui::StyleColorsDark();

        auto textureCommandBuffer = dskDevice.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture(textureCommandBuffer);
        dskDevice.endSingleTimeCommands(textureCommandBuffer);

        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}